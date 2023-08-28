#include <iostream>
#include <stdarg.h>
#include <windows.h>

#include "StandardOut_windows.hpp"
#include "common/Util.hpp"

void StandardOut_windows::print(const char* const str)
{
    StandardOut::print(str);

    OutputDebugStringA(str);
    OutputDebugStringA("\n");
}

void StandardOut_windows::print(std::string str)
{
    print(str.c_str());
}

void StandardOut_windows::vprintf(const char* const fmt, va_list argPtr)
{
    print(Util::vformat(fmt, argPtr));
}

void StandardOut_windows::printf(const char* const fmt, ...)
{
    va_list argList;
    va_start(argList, fmt);

    vprintf(fmt, argList);

    va_end(argList);
}