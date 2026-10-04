#include "locals.h"

DWORD locals::g_LocalObjectStorageIndex = -1;

bool locals::InitializeThreadStorage()
{
    DWORD o = TlsAlloc();
    locals::g_LocalObjectStorageIndex = o;
    return true;
}
