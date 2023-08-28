#include <iostream>
#include <stdarg.h>
#include <windows.h>

#include "LoggerWindows.hpp"
#include "common/Util.hpp"

void LoggerWindows::print(eLogLevel ll, const char* const str)
{
    Logger::print(ll, str);

    // wellp, this sucks, but it's fine
    OutputDebugStringA(GetTag(ll));
    OutputDebugStringA(str);
    OutputDebugStringA("\n");
}

void LoggerWindows::print(eLogLevel ll, std::string str)
{
    print(ll, str.c_str());
}

void LoggerWindows::vprintf(eLogLevel ll, const char* const fmt, va_list argPtr)
{
    print(ll, Util::vformat(fmt, argPtr));
}

void LoggerWindows::printf(eLogLevel ll, const char* const fmt, ...)
{
    va_list argList;
    va_start(argList, fmt);

    vprintf(ll, fmt, argList);

    va_end(argList);
}