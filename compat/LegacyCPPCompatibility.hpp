#pragma once

#ifdef USE_OLD_CPP
#ifndef constexpr
#define constexpr const
#endif
#ifndef nullptr
#define nullptr NULL
#endif
#ifndef override
#define override
#endif
#endif