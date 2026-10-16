// Taken from Boost
#pragma once
#include "common/utility/hashing/HashMix.hpp"
#include "common/utility/HashMap.hpp"

template <class T>
inline void hash_combine(size_t& seed, T const& v)
{
    seed = hash_mix(seed + 0x9e3779b9 + HashFunction<T>()(v));
}
