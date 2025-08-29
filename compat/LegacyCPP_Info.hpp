#pragma once

// On MSVC, pass "/Zc:__cplusplus" to make this actually work correctly
#ifndef USE_OLD_CPP
#if __cplusplus < 201103L
#define USE_OLD_CPP
#endif
#endif