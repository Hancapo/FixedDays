#include "minhook_init.h"
#include "log.h"

#include "MinHook.h"
#include "../thirdparty/include/MinHook.h"

void InitMinHook()
{
    if (MH_Initialize() == MH_OK)
        Log("[days-fix] MinHook initialized");
    else
        Log("[days-fix] ERROR: MinHook init failed");
}

void ShutdownMinHook()
{
    MH_Uninitialize();
    Log("[days-fix] MinHook shutdown");
}
