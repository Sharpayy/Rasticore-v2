#pragma once
#include <Windows.h>

namespace locals
{
	extern DWORD g_LocalObjectStorageIndex;
	bool InitializeThreadStorage();
}

