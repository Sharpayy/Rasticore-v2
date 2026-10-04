#include "bm.h"

inline uint64_t _divnz(uint64_t a, uint64_t b)
{
	uint64_t v = a / b;
	uint64_t m = a % b;
	return v + (m != 0);
}

int BmInitializeBitmask(BM_BITMASK_TREE3_64* bm, uint32_t bit_size)
{
	if (bit_size > BITMASK_TREE2_64_MAX_SIZE)
		return FALSE;

	uint64_t sub_t = _divnz<4096>(bit_size);
	uint64_t leaf = _divnz<64>(bit_size);

	bm->hint2 = BITMASK_HINT_INVALID;
	bm->bitlen = bit_size;
	bm->root = (uint64_t*)malloc(sizeof(uint64_t) * (1 + sub_t + leaf));

	// 0  - empty
	// 1  - full
	if (sub_t != 64)
		*bm->root = ~((1ull << sub_t) - 1); // allocate root entries
	else
		*bm->root = 0;

	if (sub_t == 1) // one subtree entry needed
	{
		if (leaf != 64)
			*(bm->root + 1) = ~((1ull << leaf) - 1);
		else
			*(bm->root + 1) = 0;
	}
	else
	{
		for (int i = 0; i < sub_t; i++)
		{
			*(bm->root + 1 + i) = 0;
		}
		if (sub_t != 64)
			//*(bm->root + sub_t) = ~((1ull << (leaf & 63ull)) - 1);
			*(bm->root + 1 + sub_t) = ~((1ull << (bm->bitlen & 63ull)) - 1);
		else
			*(bm->root + 1 + sub_t) = 0;
	}

	ZeroMemory(bm->root + sub_t + 1, sizeof(uint64_t) * leaf);
	if ((bit_size & 63ull) != 0)
		bm->root[1 + sub_t + leaf - 1] = ~(bit_size & 63ull);
	//else
	//	bm->root[1 + sub_t + leaf - 1] = 0;

	return TRUE;
}

void BmDeleteBitmask(BM_BITMASK_TREE3_64* bm)
{
	free(bm->root);
	bm->hint2 = BITMASK_HINT_INVALID;
	bm->bitlen = 0;
}

int BmSetBitmaskEntry(BM_BITMASK_TREE3_64* bm, uint64_t index)
{
	uint32_t leaf_index = index / 64;
	uint32_t leaf_start = _divnz<4096>(bm->bitlen);

	uint64_t leaf_data = *(bm->root + 1 + leaf_start + leaf_index);
	//if ((leaf_data >> (index & 63ull) & 1ull) == 1)
	//	return 1;

	*(bm->root + 1 + leaf_start + leaf_index) = leaf_data | (1ull << (index & 63ull));
	leaf_data = *(bm->root + 1 + leaf_start + leaf_index);
	if (leaf_data != BITMASK_LEAF64_FULL)
		return 0;

	uint32_t subt_index = index / 4096;
	*(bm->root + 1 + subt_index) = *(bm->root + 1 + subt_index) | (1ull << (leaf_index & 63ull));

	if (*(bm->root + 1 + subt_index) != BITMASK_LEAF64_FULL)
		return 0;

	*(bm->root) = *(bm->root) | (1ull << subt_index);
	
	return 0;
}

int BmClearBitmaskEntry(BM_BITMASK_TREE3_64* bm, uint64_t index)
{
	uint32_t leaf_index = index / 64;
	uint32_t leaf_start = _divnz<4096>(bm->bitlen);

	uint64_t leaf_data = *(bm->root + 1 + leaf_start + leaf_index);
	if ((leaf_data >> (index & 63ull) & 1ull) == 0)
		return 0;

	*(bm->root + 1 + leaf_start + leaf_index) = leaf_data & ~(1ull << (index & 63ull));

	uint32_t subt_index = index / 4096;
	if (*(bm->root + 1 + subt_index) != BITMASK_LEAF64_FULL)
		return 1;

	*(bm->root + 1 + subt_index) = *(bm->root + 1 + subt_index) & ~(1ull << (leaf_index & 63ull));
	*(bm->root) = *(bm->root) & ~(1ull << (subt_index & 63ull));

	return 1;
}

int BmGetBitmaskEntry(BM_BITMASK_TREE3_64* bm, uint64_t index)
{
	uint32_t leaf_index = index / 64;
	uint32_t leaf_start = _divnz<4096>(bm->bitlen);

	return (*(bm->root + 1 + leaf_start + leaf_index) >> (index & 63ull) & 1ull);
}

uint64_t __declspec(noinline) BmSetBitmaskNextEntry(BM_BITMASK_TREE3_64* bm)
{
	if (*bm->root == BITMASK_LEAF64_FULL)
		return BITMASK_INVALID_INDEX;

	unsigned long index;
	_BitScanForward64(&index, ~(*bm->root));
	//if (!_BitScanForward64(&index, ~(*bm->root)))
	//	return BITMASK_INVALID_INDEX;

	unsigned long index1;
	_BitScanForward64(&index1, ~(*(bm->root + 1 + index)));
	//if (!_BitScanForward64(&index1, ~(*(bm->root + 1 + index))))
	//	return BITMASK_INVALID_INDEX;

	unsigned long index2;
	_BitScanForward64(&index2, ~(*(bm->root + 1 + _divnz<4096>(bm->bitlen) + index * 64 + index1)));
	//if (!_BitScanForward64(&index2, ~(*(bm->root + 1 + _divnz<4096>(bm->bitlen) + index * 64 + index1))))
	//	return BITMASK_INVALID_INDEX;

	uint64_t idx = index * 4096 + 64 * index1 + index2;
	BmSetBitmaskEntry(bm, idx); // can be done faster
	return idx;
}
