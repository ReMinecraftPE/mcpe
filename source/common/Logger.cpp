#include <iostream>
#include <stdarg.h>

#include "Logger.hpp"
#include "Util.hpp"
#include "Utils.hpp"

Logger* Logger::m_singleton = nullptr;

Logger* const Logger::singleton()
{
    return m_singleton;
}

void Logger::setSingleton(Logger* logger)
{
    // Stick with the first output handle we get
    if (!m_singleton) {
        m_singleton = logger;
    } else {
        m_singleton->print(LOG_ERR, "Logging already setup!");
    }
}

Logger::~Logger()
{
    if (m_singleton == this)
        m_singleton = nullptr;
}

const char* Logger::GetTag(eLogLevel ll)
{
    switch (ll)
    {
        default:
            return "Unk: ";
        case LOG_INFO:
            return "";
        case LOG_WARN:
            return "Warning: ";
        case LOG_ERR:
            return "ERROR: ";
    }
}

void Logger::print(eLogLevel ll, const char* const str)
{
    // iProgramInCpp changed this to printf because he was worried that
    // the std::cout features wouldn't work in emscripten.
    ::printf("%s%s\n", GetTag(ll), str);
}

void Logger::print(eLogLevel ll, std::string str)
{
    print(ll, str.c_str());
}

void Logger::vprintf(eLogLevel ll, const char* const fmt, va_list argPtr)
{
    print(ll, Util::vformat(fmt, argPtr));
}

void Logger::printf(eLogLevel ll, const char* const fmt, ...)
{
    va_list argList;
    va_start(argList, fmt);

    vprintf(ll, fmt, argList);

    va_end(argList);
}