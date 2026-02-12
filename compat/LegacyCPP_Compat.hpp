#pragma once

#include "LegacyCPP_Info.hpp"

#ifdef USE_OLD_CPP

#define CONSTEXPR_FUNC static
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

/*#if !defined(_WIN32)

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

#endif // !defined(_WIN32)*/

#else
#define CONSTEXPR_FUNC constexpr
#endif // USE_OLD_CPP

// https://gcc.gnu.org/legacy-ml/gcc-help/2006-04/msg00062.html

#define MC_FUNC_MOVE(className)                      \
className& operator=(const className& move)          \
{                                                    \
	className& other = const_cast<className&>(move); \
	_move(other);                                    \
	return *this;                                    \
}

#define MC_CTOR_MOVE(className)                      \
className(const className& move)          \
{                                                    \
    className& other = const_cast<className&>(move); \
    _move(other);                                    \
}

#define MC_CTOR_MOVE_CUSTOM(className)               \
className(const className& move)          \
{                                                    \
    className& other = const_cast<className&>(move); \
    _init(other);                                    \
}
