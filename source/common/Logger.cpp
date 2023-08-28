#include <iostream>
#include <stdarg.h>

#include "Logger.hpp"
#include "Util.hpp"

Logger* Logger::m_singleton = nullptr;

Logger* const Logger::singleton()
{
    return m_singleton;
}

Logger::Logger()
{
    // Stick with the first output handle we get
    if (!m_singleton)
        m_singleton = this;
}

Logger::~Logger()
{
    if (m_singleton == this)
        m_singleton = nullptr;
}

void Logger::print(const char* const str)
{
    // iProgramInCpp changed this to printf because he was worried that
    // the std::cout features wouldn't work in emscripten.
    ::printf("%s\n", str);
}

void Logger::print(std::string str)
{
    print(str.c_str());
}

void Logger::vprintf(const char* const fmt, va_list argPtr)
{
    print(Util::vformat(fmt, argPtr));
}

void Logger::printf(const char* const fmt, ...)
{
    va_list argList;
    va_start(argList, fmt);

    vprintf(fmt, argList);

    va_end(argList);
}