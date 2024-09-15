#pragma once

#ifdef USE_OLD_CPP
#ifndef constexpr
#define constexpr const
#endif

#ifndef __XBOX_360__
#ifndef nullptr
#define nullptr NULL
#endif
#ifndef override
#define override
#endif
#endif

#endif