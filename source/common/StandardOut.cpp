#include <iostream>
#include <stdarg.h>

#include "StandardOut.hpp"
#include "Util.hpp"

StandardOut* StandardOut::m_singleton = nullptr;

StandardOut* const StandardOut::singleton()
{
    return m_singleton;
}

StandardOut::StandardOut()
{
    // Stick with the first output handle we get
    if (!m_singleton)
        m_singleton = this;
}

StandardOut::~StandardOut()
{
    if (m_singleton == this)
        m_singleton = nullptr;
}

void StandardOut::print(const char* const str)
{
    std::cout << str << std::endl;
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