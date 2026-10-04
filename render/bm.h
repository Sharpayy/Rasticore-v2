#pragma once
#include <intrin.h>
#include <cstdint>
#include <windows.h>
#include <math.h>

#define BITMASK_TREE2_64_MAX_SIZE		262144

#define BITMASK_HINT_INVALID			(uint32_t)(-1)
#define BITMASK_INVALID_INDEX			(uint64_t)(-1)

#define BITMASK_LEAF64_FULL				(uint64_t)(-1)

typedef struct _BM_BITMASK_TREE3_64
{
	uint64_t* root;
	uint32_t bitlen;
	uint32_t hint2;
} BM_BITMASK_TREE3_64;

typedef struct _BM_BITMASK_TREE3_64_SEARCH
{
	uint32_t tree_level_1_idx;
	uint32_t tree_level_2_idx;

	uint64_t tree_level_1_mask;
	uint64_t tree_level_2_mask;
} BM_BITMASK_TREE3_64_SEARCH;

inline uint64_t _divnz(uint64_t a, uint64_t b);
template<uint64_t b>
inline uint64_t _divnz(uint64_t a);

int BmInitializeBitmask(BM_BITMASK_TREE3_64* bm, uint32_t bit_size);
void BmDeleteBitmask(BM_BITMASK_TREE3_64* bm);
int BmSetBitmaskEntry(BM_BITMASK_TREE3_64* bm, uint64_t index);
int BmClearBitmaskEntry(BM_BITMASK_TREE3_64* bm, uint64_t index);

int BmGetBitmaskEntry(BM_BITMASK_TREE3_64* bm, uint64_t index);

uint64_t BmSetBitmaskNextEntry(BM_BITMASK_TREE3_64* bm);
uint64_t BmClearNextEntry(BM_BITMASK_TREE3_64* bm);

uint64_t BmGetBitmaskSize(BM_BITMASK_TREE3_64* bm);




template<uint64_t b>
inline uint64_t _divnz(uint64_t a)
{
	uint64_t v = a / b;
	uint64_t m = a % b;
	return v + (m != 0);
}
