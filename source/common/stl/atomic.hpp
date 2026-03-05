//
// Copyright (c) 2020-2020 Martin Moene
//
// https://github.com/martinmoene/atomic-lite
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef NONSTD_ATOMIC_LITE_HPP
#define NONSTD_ATOMIC_LITE_HPP

#define atomic_lite_MAJOR  0
#define atomic_lite_MINOR  0
#define atomic_lite_PATCH  0

#define atomic_lite_VERSION  atomic_STRINGIFY(atomic_lite_MAJOR) "." atomic_STRINGIFY(atomic_lite_MINOR) "." atomic_STRINGIFY(atomic_lite_PATCH)

#define atomic_STRINGIFY(  x )  atomic_STRINGIFY_( x )
#define atomic_STRINGIFY_( x )  #x

// any-lite configuration:

#define atomic_ATOMIC_DEFAULT  0
#define atomic_ATOMIC_NONSTD   1
#define atomic_ATOMIC_STD      2

#if !defined( atomic_CONFIG_SELECT_ATOMIC )
# define atomic_CONFIG_SELECT_ATOMIC  ( atomic_HAVE_STD_ATOMIC ? atomic_ATOMIC_STD : atomic_ATOMIC_NONSTD )
#endif

// ?? Control presence of exception handling (try and auto discover):

// C++ language version detection (C++23 is speculative):
// Note: VC14.0/1900 (VS2015) lacks too much from C++14.

#ifndef   atomic_CPLUSPLUS
# if defined(_MSVC_LANG ) && !defined(__clang__)
#  define atomic_CPLUSPLUS  (_MSC_VER == 1900 ? 201103L : _MSVC_LANG )
# else
#  define atomic_CPLUSPLUS  __cplusplus
# endif
#endif

#define atomic_CPP98_OR_GREATER  ( atomic_CPLUSPLUS >= 199711L )
#define atomic_CPP11_OR_GREATER  ( atomic_CPLUSPLUS >= 201103L )
#define atomic_CPP14_OR_GREATER  ( atomic_CPLUSPLUS >= 201402L )
#define atomic_CPP17_OR_GREATER  ( atomic_CPLUSPLUS >= 201703L )
#define atomic_CPP20_OR_GREATER  ( atomic_CPLUSPLUS >= 202002L )

// Use C++11 std::atomic if available and requested:

#if atomic_CPP11_OR_GREATER && defined(__has_include )
# if __has_include( <atomic> )
#  define atomic_HAVE_STD_ATOMIC  1
# else
#  define atomic_HAVE_STD_ATOMIC  0
# endif
#else
# define  atomic_HAVE_STD_ATOMIC  0
#endif

#define  atomic_USES_STD_ATOMIC  ( (atomic_CONFIG_SELECT_ATOMIC == atomic_ATOMIC_STD) || ((atomic_CONFIG_SELECT_ATOMIC == atomic_ATOMIC_DEFAULT) && atomic_HAVE_STD_ATOMIC) )

//
// Using std::atomic:
//

#if atomic_USES_STD_ATOMIC

#include <atomic>

// import names from std::

namespace nonstd {

	using std::memory_order;
	using std::memory_order_relaxed;
	using std::memory_order_consume;
	using std::memory_order_acquire;
	using std::memory_order_release;
	using std::memory_order_acq_rel;
	using std::memory_order_seq_cst;

	using std::atomic;
	using std::atomic_flag;

	using std::atomic_flag_test_and_set;
	using std::atomic_flag_test_and_set_explicit;
	using std::atomic_flag_clear;
	using std::atomic_flag_clear_explicit;

} // namespace nonstd

#else // atomic_USES_STD_ATOMIC

//
// Using nonstd::atomic:
//

// Compiler versions:
//
// MSVC++  6.0  _MSC_VER == 1200  atomic_COMPILER_MSVC_VERSION ==  60  (Visual Studio 6.0)
// MSVC++  7.0  _MSC_VER == 1300  atomic_COMPILER_MSVC_VERSION ==  70  (Visual Studio .NET 2002)
// MSVC++  7.1  _MSC_VER == 1310  atomic_COMPILER_MSVC_VERSION ==  71  (Visual Studio .NET 2003)
// MSVC++  8.0  _MSC_VER == 1400  atomic_COMPILER_MSVC_VERSION ==  80  (Visual Studio 2005)
// MSVC++  9.0  _MSC_VER == 1500  atomic_COMPILER_MSVC_VERSION ==  90  (Visual Studio 2008)
// MSVC++ 10.0  _MSC_VER == 1600  atomic_COMPILER_MSVC_VERSION == 100  (Visual Studio 2010)
// MSVC++ 11.0  _MSC_VER == 1700  atomic_COMPILER_MSVC_VERSION == 110  (Visual Studio 2012)
// MSVC++ 12.0  _MSC_VER == 1800  atomic_COMPILER_MSVC_VERSION == 120  (Visual Studio 2013)
// MSVC++ 14.0  _MSC_VER == 1900  atomic_COMPILER_MSVC_VERSION == 140  (Visual Studio 2015)
// MSVC++ 14.1  _MSC_VER >= 1910  atomic_COMPILER_MSVC_VERSION == 141  (Visual Studio 2017)
// MSVC++ 14.2  _MSC_VER >= 1920  atomic_COMPILER_MSVC_VERSION == 142  (Visual Studio 2019)

#if defined(_MSC_VER ) && !defined(__clang__)
# define atomic_COMPILER_MSVC_VER      (_MSC_VER )
# define atomic_COMPILER_MSVC_VERSION  (_MSC_VER / 10 - 10 * ( 5 + (_MSC_VER < 1900 ) ) )
#else
# define atomic_COMPILER_MSVC_VER      0
# define atomic_COMPILER_MSVC_VERSION  0
#endif

#define atomic_COMPILER_VERSION( major, minor, patch )  ( 10 * ( 10 * (major) + (minor) ) + (patch) )

#if defined(__clang__)
# define atomic_COMPILER_CLANG_VERSION  atomic_COMPILER_VERSION(__clang_major__, __clang_minor__, __clang_patchlevel__)
#else
# define atomic_COMPILER_CLANG_VERSION  0
#endif

#if defined(__GNUC__) && !defined(__clang__)
# define atomic_COMPILER_GNUC_VERSION  atomic_COMPILER_VERSION(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#else
# define atomic_COMPILER_GNUC_VERSION  0
#endif

#define atomic_MAYBE_UNUSED(a)  ((void)(a))

// half-open range [lo..hi):
//#define atomic_BETWEEN( v, lo, hi ) ( (lo) <= (v) && (v) < (hi) )

// Presence of language and library features:

#define atomic_HAVE( feature )  ( atomic_HAVE_##feature )

#ifdef _HAS_CPP0X
# define atomic_HAS_CPP0X  _HAS_CPP0X
#else
# define atomic_HAS_CPP0X  0
#endif

#define atomic_CPP11_90   (atomic_CPP11_OR_GREATER || atomic_COMPILER_MSVC_VER >= 1500)
#define atomic_CPP11_100  (atomic_CPP11_OR_GREATER || atomic_COMPILER_MSVC_VER >= 1600)
#define atomic_CPP11_110  (atomic_CPP11_OR_GREATER || atomic_COMPILER_MSVC_VER >= 1700)
#define atomic_CPP11_120  (atomic_CPP11_OR_GREATER || atomic_COMPILER_MSVC_VER >= 1800)
#define atomic_CPP11_140  (atomic_CPP11_OR_GREATER || atomic_COMPILER_MSVC_VER >= 1900)

#define atomic_CPP14_000  (atomic_CPP14_OR_GREATER)
#define atomic_CPP17_000  (atomic_CPP17_OR_GREATER)

// Presence of C++11 language features:

#define atomic_HAVE_CONSTEXPR_11	atomic_CPP11_140
#define atomic_HAVE_NOEXCEPT		atomic_CPP11_140
#define atomic_HAVE_ENUM_CLASS		atomic_CPP11_140
#define atomic_HAVE_IS_DEFAULT		atomic_CPP11_140
#define atomic_HAVE_IS_DELETE		atomic_CPP11_140

// Presence of C++14 language features:

#define atomic_HAVE_CONSTEXPR_14	atomic_CPP14_000

// Presence of C++17 language features:

#define atomic_HAVE_INLINE_17		atomic_CPP17_000

// Presence of C++ language features:

#if atomic_HAVE_CONSTEXPR_11
# define atomic_constexpr constexpr
#else
# define atomic_constexpr /*constexpr*/
#endif

#if atomic_HAVE_CONSTEXPR_14
# define atomic_constexpr14 constexpr
#else
# define atomic_constexpr14 /*constexpr*/
#endif

#if atomic_HAVE_ENUM_CLASS
# define atomic_enumclass class
#else
# define atomic_enumclass /*class*/
#endif

#if atomic_HAVE_IS_DEFAULT
#endif

#if atomic_HAVE_IS_DELETE
# define atomic_delete    = delete
# define atomic_delete_ac /*access:*/
#else
# define atomic_delete  /*= delete*/
# define atomic_delete_ac private:
#endif

#if atomic_HAVE_NOEXCEPT
# define atomic_noexcept noexcept
#else
# define atomic_noexcept /*noexcept*/
#endif

#if atomic_HAVE_INLINE_17
# define atomic_inline inline
#else
# define atomic_inline /*inline*/
#endif

// additional includes:

#include <cassert>

#if atomic_CPP11_OR_GREATER
# include <cstdint>
#endif

namespace nonstd { namespace atomic_lite {

	// [atomics.order], order and consistency

	enum atomic_enumclass memory_order /* : unspecified TODO */
	{
		relaxed
		, consume
		, acquire
		, release
		, acq_rel
		, seq_cst
	};

#if atomic_CPP11_OR_GREATER
	atomic_inline constexpr memory_order memory_order_relaxed = memory_order::relaxed;
	atomic_inline constexpr memory_order memory_order_consume = memory_order::consume;
	atomic_inline constexpr memory_order memory_order_acquire = memory_order::acquire;
	atomic_inline constexpr memory_order memory_order_release = memory_order::release;
	atomic_inline constexpr memory_order memory_order_acq_rel = memory_order::acq_rel;
	atomic_inline constexpr memory_order memory_order_seq_cst = memory_order::seq_cst;
#else
	const memory_order memory_order_relaxed = /*memory_order::*/relaxed;
	const memory_order memory_order_consume = /*memory_order::*/consume;
	const memory_order memory_order_acquire = /*memory_order::*/acquire;
	const memory_order memory_order_release = /*memory_order::*/release;
	const memory_order memory_order_acq_rel = /*memory_order::*/acq_rel;
	const memory_order memory_order_seq_cst = /*memory_order::*/seq_cst;
#endif

	// [atomics.lockfree], lock-free property

	// [atomics.ref.generic], class template atomic_­ref
	template<class T> struct atomic_ref;

	// [atomics.ref.pointer], partial specialization for pointers
	template<class T> struct atomic_ref<T*>;

	// [atomics.types.generic], class template atomic
	template<class T> struct atomic;

	// [atomics.types.pointer], partial specialization for pointers
	template<class T> struct atomic<T*>;

	// [atomics.nonmembers], non-member functions
	// [atomics.alias], type aliases

	//using atomic_signed_lock_free   = see below;
	//using atomic_unsigned_lock_free = see below;

	// [atomics.flag], flag type and operations

	struct atomic_flag;

	bool atomic_flag_test( atomic_flag const volatile * ) atomic_noexcept;

	// [atomics.fences], fences

} // namespace atomic_lite
} // namespace nonstd

//
// Implementation:
//

#ifdef __GNUC__
# include <xmmintrin.h>
#else
# include <Windows.h>
#endif

#ifndef __GNUC__
# if atomic_COMPILER_MSVC_VER > 1700
#  define __sync_bool_compare_and_swap(object, expected, desired) \
	 ( _InterlockedCompareExchange( (long *) object, desired, expected) == (expected) )
# else
#  define __sync_bool_compare_and_swap(object, expected, desired) \
	( InterlockedCompareExchangePointer( 			\
		reinterpret_cast<void*volatile*>(object) 	\
		, reinterpret_cast<PVOID>(expected) 		\
		, reinterpret_cast<PVOID>(desired) ) 		\
		, (*object) == (expected) )
# endif
#endif // __GNUC__

namespace nonstd { namespace atomic_lite {

template< class T >
struct atomic
{
	typedef T value_type;

	// [atomics.types.operations], operations on atomic types
};

typedef long atomic_flag_data_type;

struct atomic_flag_base
{
#if !atomic_CPP11_OR_GREATER
	atomic_constexpr atomic_flag_base() atomic_noexcept
		: flag(0) {}
#endif
	atomic_flag_data_type flag;
};

struct atomic_flag : atomic_flag_base
{
	bool test_and_set( memory_order order = memory_order_seq_cst ) volatile atomic_noexcept
	{
		atomic_MAYBE_UNUSED(order);
//		assert( order == memory_order_seq_cst );

		return !__sync_bool_compare_and_swap( &flag, 0, 1 );
	}

	bool test_and_set( memory_order order = memory_order_seq_cst ) atomic_noexcept
	{
		atomic_MAYBE_UNUSED(order);
//		assert( order == memory_order_seq_cst );

		return !__sync_bool_compare_and_swap( &flag, 0, 1 );
	}

	void clear( memory_order order = memory_order_seq_cst ) volatile atomic_noexcept
	{
		atomic_MAYBE_UNUSED(order);
//		assert( order == memory_order_seq_cst );

#ifdef __GNUC__
    	asm volatile ("":::"memory"); // acts as a memory barrier.
#else
		MemoryBarrier();
#endif
	   flag = 0;
	}

	void clear( memory_order order = memory_order_seq_cst ) atomic_noexcept
	{
		atomic_MAYBE_UNUSED(order);
//		assert( order == memory_order_seq_cst );

#ifdef __GNUC__
    	asm volatile ("":::"memory"); // acts as a memory barrier.
#else
		MemoryBarrier();
#endif
	   flag = 0;
	}

#if atomic_HAVE_IS_DEFAULT
	atomic_flag() atomic_noexcept = default;
#else
	atomic_flag() atomic_noexcept
		: atomic_flag_base() {}
#endif

	// Conversion to ATOMIC_FLAG_INIT as {0} when e.g. supplied from <thread>.

	atomic_constexpr atomic_flag( bool ) atomic_noexcept
#if atomic_CPP11_OR_GREATER
		: atomic_flag_base{0} {}
#else
		: atomic_flag_base() {}
#endif

atomic_delete_ac
	atomic_flag( atomic_flag const & ) atomic_delete;

	atomic_flag& operator=( atomic_flag const & ) atomic_delete;
#if atomic_COMPILER_MSVC_VER > 1920
	atomic_flag& operator=( atomic_flag const & ) volatile atomic_delete;
#endif
};

inline bool atomic_flag_test_and_set( atomic_flag volatile * flag ) atomic_noexcept
{
	return flag->test_and_set();
}

inline bool atomic_flag_test_and_set( atomic_flag * flag ) atomic_noexcept
{
	return flag->test_and_set();
}

inline bool atomic_flag_test_and_set_explicit( atomic_flag volatile * flag, memory_order order ) atomic_noexcept
{
	return flag->test_and_set( order );
}

inline bool atomic_flag_test_and_set_explicit( atomic_flag * flag, memory_order order ) atomic_noexcept
{
	return flag->test_and_set( order );
}

inline void atomic_flag_clear( volatile atomic_flag * flag ) atomic_noexcept
{
	flag->clear();
}

inline void atomic_flag_clear( atomic_flag * flag ) atomic_noexcept
{
	flag->clear();
}

inline void atomic_flag_clear_explicit( atomic_flag volatile * flag, memory_order order ) atomic_noexcept
{
	flag->clear( order );
}

inline void atomic_flag_clear_explicit( atomic_flag * flag, memory_order order ) atomic_noexcept
{
	flag->clear( order );
}

#if atomic_CPP11_OR_GREATER
# define ATOMIC_FLAG_INIT {0}
#else
# define ATOMIC_FLAG_INIT 0
#endif

} // namespace atomic_lite
} // namespace nonstd

namespace nonstd {

	using atomic_lite::memory_order;
	using atomic_lite::memory_order_relaxed;
	using atomic_lite::memory_order_consume;
	using atomic_lite::memory_order_acquire;
	using atomic_lite::memory_order_release;
	using atomic_lite::memory_order_acq_rel;
	using atomic_lite::memory_order_seq_cst;

	using atomic_lite::atomic;
	using atomic_lite::atomic_flag;

	using atomic_lite::atomic_flag_test_and_set;
	using atomic_lite::atomic_flag_test_and_set_explicit;
	using atomic_lite::atomic_flag_clear;
	using atomic_lite::atomic_flag_clear_explicit;

} // namespace nonstd

#endif // atomic_USES_STD_ATOMIC

#endif // NONSTD_ATOMIC_LITE_HPP
