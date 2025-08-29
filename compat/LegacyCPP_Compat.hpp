#pragma once

#include "LegacyCPP_Info.hpp"

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
#ifndef noexcept
#define noexcept
#endif
#endif