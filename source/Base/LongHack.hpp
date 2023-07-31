#pragma once

// Have to do this because GCC on 64-bit targets makes longs 64-bit.
#ifdef ORIGINAL_CODE
#define TLong long
#else
#define TLong int
#endif
