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

    static DWORD (WINAPI *XInputGetState)(DWORD, XINPUT_STATE *);

    static void init(void)
    {
#ifdef _XBOX
        XInputGetState = ::XInputGetState;
#else
        XInputGetState = nullptr;
        HMODULE module = LoadLibraryA("xinput1_3.dll");
        if (module)
            XInputGetState = (DWORD (WINAPI *)(DWORD, XINPUT_STATE *))GetProcAddress(module, "XInputGetState");
        if (!XInputGetState)
            LOG_W("Could not find xinput driver, xinput controllers will be disabled.");
#endif
    }

};
