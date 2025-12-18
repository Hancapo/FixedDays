#pragma once
#include <windows.h>

struct State
{
    HWND hwnd = nullptr;

    // resolución interna del backbuffer (la del juego)
    UINT bbW = 0;
    UINT bbH = 0;

    // último rect donde dibujamos dentro del client (para arreglar mouse luego)
    RECT lastDst = {0,0,0,0};
};

extern State g_state;
