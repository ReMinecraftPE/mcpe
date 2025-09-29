//
//  EndianDefinitions.h
//  Minecraft
//
//  Created by Brent on 9/14/25.
//
//

#pragma once

// @TODO: __BIG_ENDIAN__ might be Apple-specific

#ifndef MC_ENDIANNESS_BIG
#if (defined(__BIG_ENDIAN__) && __BIG_ENDIAN__) || (defined(__powerpc__) && __powerpc__)
#define MC_ENDIANNESS_BIG 1
#else
#define MC_ENDIANNESS_BIG 0
#endif
#endif

#ifndef MC_ENDIANNESS_LITTLE
#if defined(__LITTLE_ENDIAN__) && __LITTLE_ENDIAN__
#define MC_ENDIANNESS_LITTLE 1
#else
#define MC_ENDIANNESS_LITTLE 0
#endif
#endif

#if !MC_ENDIANNESS_BIG && !MC_ENDIANNESS_LITTLE
#undef MC_ENDIANNESS_LITTLE
// If we don't know what we are, default to little endian
#define MC_ENDIANNESS_LITTLE 1
#endif