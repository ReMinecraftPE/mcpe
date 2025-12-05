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

#ifndef _WIN32

namespace std
{
    template<class T> struct remove_reference { typedef T type; };
    template<class T> struct remove_reference<T&> { typedef T type; };
    template<class T> struct remove_reference<T&&> { typedef T type; };
    
    template< class T >
    typename remove_reference<T>::type&& move(T&& t)
    {
        return static_cast<typename remove_reference<T>::type&&>(t);
    }
}

#endif // _WIN32

#endif // USE_OLD_CPP