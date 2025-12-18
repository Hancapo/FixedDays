#pragma once
#include <windows.h>



struct State
{
    HWND hwnd = nullptr;

    UINT bbW = 0;
    UINT bbH = 0;

    RECT lastDst = {0,0,0,0};
    
    bool forceInternal720 = true;
    UINT forcedW = 1280;
    UINT forcedH = 720;
};

extern State g_state;


