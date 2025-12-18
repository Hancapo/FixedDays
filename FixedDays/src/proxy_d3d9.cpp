#include <windows.h>
#include "log.h"
#include "hooks_d3d9.h"


struct IDirect3D9;
struct IDirect3D9Ex;

static HMODULE g_real = nullptr;

using Direct3DCreate9_t   = IDirect3D9*  (WINAPI*)(UINT);
using Direct3DCreate9Ex_t = HRESULT      (WINAPI*)(UINT, IDirect3D9Ex**);

static Direct3DCreate9_t   real_Create9   = nullptr;
static Direct3DCreate9Ex_t real_Create9Ex = nullptr;

static void LoadRealD3D9()
{
    if (g_real) return;

    char sys[MAX_PATH]{};
    GetSystemDirectoryA(sys, MAX_PATH);

    char path[MAX_PATH]{};
    lstrcpyA(path, sys);
    lstrcatA(path, "\\d3d9.dll");

    g_real = LoadLibraryA(path);
    if (!g_real) {
        Log("[days-fix] ERROR: LoadLibrary real d3d9.dll failed");
        return;
    }

    real_Create9   = (Direct3DCreate9_t)GetProcAddress(g_real, "Direct3DCreate9");
    real_Create9Ex = (Direct3DCreate9Ex_t)GetProcAddress(g_real, "Direct3DCreate9Ex");

    Log("[days-fix] Loaded real d3d9.dll real_Create9=%p real_Create9Ex=%p",
        (void*)real_Create9, (void*)real_Create9Ex);
}

extern "C" IDirect3D9* WINAPI Direct3DCreate9(UINT sdk)
{
    Log("[days-fix] Direct3DCreate9 called");
    LoadRealD3D9();
    IDirect3D9* d3d = real_Create9 ? real_Create9(sdk) : nullptr;
    if (d3d) HookD3D9CreateDevice(d3d);
    return d3d;
}


extern "C" HRESULT WINAPI Direct3DCreate9Ex(UINT sdk, IDirect3D9Ex** out)
{
    Log("[days-fix] Direct3DCreate9Ex called");
    LoadRealD3D9();
    return real_Create9Ex ? real_Create9Ex(sdk, out) : E_FAIL;
}
