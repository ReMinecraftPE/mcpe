#pragma once

// On MSVC, pass "/Zc:__cplusplus" to make this actually work correctly
#ifndef USE_OLD_CPP
#if __cplusplus < 201103L
#define USE_OLD_CPP
#endif
#endif

#ifdef USE_OLD_CPP
#ifndef constexpr
#define constexpr const
#endif
#ifndef nullptr
#include <stddef.h>
#ifndef nullptr
#define nullptr NULL
#endif
#endif
#ifndef override
#define override
#endif
#endif