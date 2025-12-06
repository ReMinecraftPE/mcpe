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

#endif // USE_OLD_CPP

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
