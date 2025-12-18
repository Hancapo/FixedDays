#include <windows.h>
#include "log.h"
#include "minhook_init.h"
#include "hooks_user32.h"

BOOL APIENTRY DllMain(HMODULE h, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(h);
        Log("[days-fix] DLL_PROCESS_ATTACH");
        InitMinHook();
        HookUser32Cursor();
    }
    else if (reason == DLL_PROCESS_DETACH)
    {
        Log("[days-fix] DLL_PROCESS_DETACH");
        ShutdownMinHook();
    }
    return TRUE;
}
