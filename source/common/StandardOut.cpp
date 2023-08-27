#include <iostream>
#include <stdarg.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "StandardOut.hpp"
#include "Util.hpp"

StandardOut* const StandardOut::singleton()
{
    // This is automatically allocated when accessed for the first time,
    // and automatically deallocated when runtime concludes.
    static StandardOut standardOut = StandardOut();
    return &standardOut;
}

void StandardOut::print(const char* const str)
{
    std::cout << str << std::endl;
#ifdef _WIN32
    OutputDebugStringA(str);
    OutputDebugStringA("\n");
#endif
}

void StandardOut::print(std::string str)
{
    print(str.c_str());
}

void StandardOut::vprintf(const char* const fmt, va_list argPtr)
{
    print(Util::vformat(fmt, argPtr));
}

void StandardOut::printf(const char* const fmt, ...)
{
    va_list argList;
    va_start(argList, fmt);

    vprintf(fmt, argList);

    va_end(argList);
}