#pragma once

#ifdef __GNUC__
#define MC_UNUSED __attribute__((unused))
#else
#define MC_UNUSED
#endif
