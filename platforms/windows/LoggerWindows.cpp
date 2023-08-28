#include <iostream>
#include <stdarg.h>
#include <windows.h>

#include "LoggerWindows.hpp"
#include "common/Util.hpp"

void LoggerWindows::print(const char* const str)
{
    Logger::print(str);

    OutputDebugStringA(str);
    OutputDebugStringA("\n");
}

void LoggerWindows::print(std::string str)
{
    print(str.c_str());
}

void LoggerWindows::vprintf(const char* const fmt, va_list argPtr)
{
    print(Util::vformat(fmt, argPtr));
}

void LoggerWindows::printf(const char* const fmt, ...)
{
    va_list argList;
    va_start(argList, fmt);

    vprintf(fmt, argList);

    va_end(argList);
}