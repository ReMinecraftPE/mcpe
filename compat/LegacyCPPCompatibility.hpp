#pragma once

// On MSVC, pass "/Zc:__cplusplus" to make this actually work correctly
#if __cplusplus < 201103L
#define USE_OLD_CPP
#endif

#ifdef USE_OLD_CPP
#ifndef constexpr
#define constexpr const
#endif
#ifndef nullptr
#include <stddef.h>
#define nullptr NULL
#endif
#ifndef override
#define override
#endif
#endif