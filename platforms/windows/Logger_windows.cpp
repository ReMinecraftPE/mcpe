#include <iostream>
#include <stdarg.h>
#include <windows.h>

#include "Logger_windows.hpp"
#include "common/Util.hpp"

void Logger_windows::print(const char* const str)
{
    Logger::print(str);

    OutputDebugStringA(str);
    OutputDebugStringA("\n");
}

void Logger_windows::print(std::string str)
{
    print(str.c_str());
}

void Logger_windows::vprintf(const char* const fmt, va_list argPtr)
{
    print(Util::vformat(fmt, argPtr));
}

void Logger_windows::printf(const char* const fmt, ...)
{
    va_list argList;
    va_start(argList, fmt);

    vprintf(fmt, argList);

    va_end(argList);
}