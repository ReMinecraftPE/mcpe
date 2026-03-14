#pragma once

#ifdef _XBOX
#include <xtl.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <xinput.h>
#endif

#include "common/Logger.hpp"

class XInput
{
public:

    static DWORD (WINAPI *GetState)(DWORD, XINPUT_STATE *);

    static void init(void)
    {
#ifdef _XBOX
        GetState = ::XInputGetState;
#else
        HMODULE module = LoadLibrary("XINPUT1_4.DLL");
        if (!module)
            module = LoadLibrary("XINPUT1_3.DLL");
        if (!module)
            module = LoadLibrary("XINPUT1_2.DLL");
        if (!module)
            module = LoadLibrary("XINPUT1_1.DLL");
        if (!module)
            module = LoadLibrary("XINPUT9_1_0.DLL");
        if (module)
            GetState = (DWORD (WINAPI *)(DWORD, XINPUT_STATE *))GetProcAddress(module, "XInputGetState");
        if (!GetState)
            LOG_W("Could not find xinput driver, xinput controllers will be disabled.");
#endif
    }

};
