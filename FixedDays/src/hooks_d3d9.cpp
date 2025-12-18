#include <windows.h>
#include "log.h"
#include "../thirdparty/include/MinHook.h"
#include "state.h"
#include "hooks_device.h"



struct IDirect3D9;

struct D3DPRESENT_PARAMETERS_MIN
{
    UINT BackBufferWidth;
    UINT BackBufferHeight;
    UINT BackBufferFormat;
    UINT BackBufferCount;
    UINT MultiSampleType;
    UINT MultiSampleQuality;
    UINT SwapEffect;
    HWND hDeviceWindow;
    BOOL Windowed;
    BOOL EnableAutoDepthStencil;
    UINT AutoDepthStencilFormat;
    DWORD Flags;
    UINT FullScreen_RefreshRateInHz;
    UINT PresentationInterval;
};

using CreateDevice_t = HRESULT (WINAPI*)(
    IDirect3D9* self,
    UINT Adapter,
    UINT DeviceType,
    HWND hFocusWindow,
    DWORD BehaviorFlags,
    D3DPRESENT_PARAMETERS_MIN* pPresentationParameters,
    void** ppReturnedDeviceInterface
);

static CreateDevice_t real_CreateDevice = nullptr;

static HRESULT WINAPI hk_CreateDevice(
    IDirect3D9* self,
    UINT Adapter,
    UINT DeviceType,
    HWND hFocusWindow,
    DWORD BehaviorFlags,
    D3DPRESENT_PARAMETERS_MIN* pp,
    void** outDev
)
{
    if (pp)
    {
        g_state.hwnd = pp->hDeviceWindow;
        g_state.bbW  = pp->BackBufferWidth;
        g_state.bbH  = pp->BackBufferHeight;
        Log("[days-fix] CreateDevice: Windowed=%d BB=%ux%u hWnd=%p SwapEffect=%u Flags=0x%08X",
            (int)pp->Windowed,
            (unsigned)pp->BackBufferWidth,
            (unsigned)pp->BackBufferHeight,
            pp->hDeviceWindow,
            (unsigned)pp->SwapEffect,
            (unsigned)pp->Flags
        );
    }
    else
    {
        Log("[days-fix] CreateDevice: pp=null");
    }

    HRESULT hr = real_CreateDevice(self, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pp, outDev);

    if (SUCCEEDED(hr) && outDev && *outDev)
    {
        HookDevicePresent(*outDev);
    }

    return hr;

}

void HookD3D9CreateDevice(IDirect3D9* d3d)
{
    if (!d3d) return;
    if (real_CreateDevice) return;

    void** vtbl = *(void***)(d3d);

    void* target = vtbl[16];

    if (MH_CreateHook(target, &hk_CreateDevice, (void**)&real_CreateDevice) == MH_OK)
    {
        MH_EnableHook(target);
        Log("[days-fix] Hooked IDirect3D9::CreateDevice target=%p hk=%p real=%p",
            target, (void*)&hk_CreateDevice, (void*)real_CreateDevice);
    }
    else
    {
        Log("[days-fix] ERROR: MH_CreateHook(CreateDevice) failed");
    }
}
