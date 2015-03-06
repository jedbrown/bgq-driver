/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q 5765-PER 5765-PRP                                    */
/*                                                                  */
/* (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/* US Government Users Restricted Rights -                          */
/* Use, duplication, or disclosure restricted                       */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/**
 * \file util/common.h
 * \brief Basic header file to define simple and common items
 */
#ifndef __util_common_h__
#define __util_common_h__

#include <new>
#include <stdio.h>
#include <assert.h>
#include <stddef.h>

#if defined(__xlc__) || defined(__xlC__)
#include <builtins.h>
#endif

#include <pami.h>
#include "Compiler.h"
#include "Arch.h"
#include "Memory.h"

#ifndef MIN
#define MIN(a,b)  (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b)  (((a)>(b))?(a):(b))
#endif

#ifndef CEIL
#define CEIL(x,y) (((x)+(y)-1)/(y))
#endif

#define CEILING_P(x) ((x-(int)(x)) > 0 ? (int)(x+1) : (int)(x))
#define CEILING_N(x) ((x-(int)(x)) < 0 ? (int)(x-1) : (int)(x))
#define CEILING(x) ( ((x) > 0) ? CEILING_P(x) : CEILING_N(x) )

/*
 * The following are only respected by GCC and the latest XL
 * compilers.  The "expected" value is assumed to be either a 1 or 0,
 * so best results are achieved when your expression evaluates to 1 or
 * 0.  It makes sense to use unlikely for error checking, since it
 * will move the object code away from the good-path.
 *
 * They can be used like this:

  if (likely(expression_that_is_probably_one(input)) { do_something() }

  if (unlikely(error_check_expression_that_is_probably_zero(input)) { do_something_to_handle_error() }

 */
#define   likely(x) (__builtin_expect(x,1))
#define unlikely(x) (__builtin_expect(x,0))

#define PAMIQuad_sizeof(x)  ((sizeof(x)+15)>>4)

/// abort macros defined for all assertion levels
#define PAMI_abort()                       abort()
#define PAMI_abortf(fmt...)                { fprintf(stderr, __FILE__ ":%d: \n", __LINE__); fprintf(stderr, fmt); abort(); }

#ifndef ASSERT_LEVEL
#define ASSERT_LEVEL 2
#warning ASSERT_LEVEL not set by config.  Defaulting to all asserts enabled
#endif

#if ASSERT_LEVEL==0    // All asserts are disabled
#define PAMI_assert(expr)
#define PAMI_assertf(expr, fmt...)
#define PAMI_assert_debug(expr)
#define PAMI_assert_debugf(expr, fmt...)

#elif ASSERT_LEVEL==1  // Only "normal" asserts, not debug, are enabled
#define PAMI_assert(expr)                assert(expr)
#define PAMI_assertf(expr, fmt...)       { if (!(expr)) PAMI_abortf(fmt); }
#define PAMI_assert_debug(expr)
#define PAMI_assert_debugf(expr, fmt...)

#else // ASSERT_LEVEL==2 ... All asserts are enabled
#define PAMI_assert(expr)                assert(expr)
#define PAMI_assertf(expr, fmt...)       { if (!(expr)) PAMI_abortf(fmt); }
#define PAMI_assert_debug(expr)          assert(expr)
#define PAMI_assert_debugf(expr, fmt...) PAMI_assertf(expr, fmt)

#endif // ASSERT_LEVEL

#define PAMI_assert_alwaysf(expr, fmt...)	{ if (!(expr)) PAMI_abortf(fmt); }
#define PAMI_assert_always(expr)		assert(expr)

static inline int64_t min_nb64(int64_t x, int64_t y)
{
  return x + (((y - x) >> (63))&(y - x));
}

static inline int32_t min_nb32(int32_t x, int32_t y)
{
  return x + (((y - x) >> (31))&(y - x));
}

static inline int64_t max_nb64(int64_t x, int64_t y)
{
  return x - (((x - y) >> (63))&(x - y));
}

static inline int32_t max_nb32(int32_t x, int32_t y)
{
  return x - (((x - y) >> (31))&(x - y));
}
#if 0
inline void* operator new(size_t obj_size, void* pointer)
{
  /*   printf("%s: From %p for %u\n", __PRETTY_FUNCTION__, pointer, obj_size); */
//  CCMI_assert_debug(pointer != NULL);
  return pointer;
}
#endif

/**
 * \brief Creates a compile error if the condition is false.
 *
 * This macro must be used within a function for the compiler to process it.
 * It is suggested that C++ classes and C files create an inline function
 * similar to the following example. The inline function is never used at
 * runtime and should be optimized out by the compiler. It exists for the sole
 * purpose of moving runtime \c assert calls to compile-time errors.
 *
 * \code
 * static inline void compile_time_assert ()
 * {
 *   // This compile time assert will succeed.
 *   COMPILE_TIME_ASSERT(sizeof(char) <= sizeof(double));
 *
 *   // This compile time assert will fail.
 *   COMPILE_TIME_ASSERT(sizeof(double) <= sizeof(char));
 * }
 * \endcode
 *
 * Compile time assert errors will look similar to the following:
 *
 * \code
 * foo.h: In function compile_time_assert:
 * foo.h:43: error: duplicate case value
 * foo.h:43: error: previously used here
 * \endcode
 *
 * \note C++ template code must actually invoke the compile_time_assert
 *       function, typically in a class constructor, for the assertion
 *       to be evaluated. This is because the compile will not even
 *       parse the function unless it is used.
 */
#define COMPILE_TIME_ASSERT(expr) if(0){switch(0){case 0:case expr:;}}

/**
 * \brief This is like COMPILE_TIME_ASSERT, but should match the new
 *        system used in "C++ 0x"
 *
 *  http://en.wikipedia.org/wiki/C%2B%2B0x#Static_assertions
 */
#define static_assert(expr, string) COMPILE_TIME_ASSERT(expr)

typedef pami_geometry_t (*pami_mapidtogeometry_fn) (pami_context_t c, int comm);

#ifdef __cplusplus
#define ENFORCE_CLASS_MEMBER(class,member)	{ PAMI_assert_debug(&((class *)this)->member || true); }
#define ENFORCE_STATIC_CONST_CLASS_MEMBER(class,member)	{ PAMI_assert_debug(((class *)this)->member || true); }
#endif // __cplusplus

/*
 * The following inlines help with initialization of shared (memory) resources,
 * such that only one participant will initialize and no participants start to
 * use the resource until initialization has finished.
 */
#ifndef __local_barriered_ctrzero_fn__
#define __local_barriered_ctrzero_fn__
#ifdef __cplusplus
///
/// This assumes that the last two counters will not be accessed
/// by any participant too soon after return from this routine...
/// That is true for CounterBarrier.
///
template <class T_Counter>
inline void local_barriered_ctrzero(T_Counter *ctrs, size_t num,
                                size_t participants, bool master) {
        PAMI_assertf(num >= 2, "local_barriered_ctrzero() requires at least two counters\n");
        size_t c0 = num - 2;
        size_t c1 = num - 1;
        if (master) { // -----------------------------------------------------------
                size_t value = ctrs[c1].fetch() + participants;
                size_t i;
                for (i = 0; i < c0; ++i) {	// 0: clear most of the counters
                        ctrs[i].clear();
                }
                ctrs[c1].fetch_and_inc();	// 2: count entry of 1 participant
                while (ctrs[c1].fetch() != value) { // 2: await arrival of all participants
                        ctrs[c0].fetch_and_inc(); // 1: trigger entry of slaves
                }
                ctrs[c1].clear();
                ctrs[c0].clear();	// 3: trigger completion of slaves
        } else { // ----------------------------------------------------------------
                size_t value = ctrs[c0].fetch();
                while (ctrs[c0].fetch() == value); // 1: await entry of master
                ctrs[c1].fetch_and_inc();	// 2: count entry of 1 participant
                while (ctrs[c0].fetch() != 0);	// 3: await completion of master
        } // -----------------------------------------------------------------------
}
#endif // __cplusplus
// try to do an un-templated version for C programs?
#endif // __local_barriered_ctrzero_fn__

#ifndef __local_barriered_shmemzero_fn__
#define __local_barriered_shmemzero_fn__
#include <string.h>
///
/// This assumes that the area at the end of "mem" will not be accessed
/// by any participant too soon after return from this routine...
///
/// This also requires that compiler builtin atomics exist, and
/// are named (or aliased) the same as GCC __sync_*() atomics.
/// It also assumes these atomics work on "size_t" values.
/// It additionally assumes that counters may be fetched without special
/// semantics (i.e. just a load from 'volatile *').
///
inline void local_barriered_shmemzero(void *shmem, size_t len,
                                size_t participants, bool master) {
        volatile size_t *ctrs = (volatile size_t *)shmem;
        size_t num = len / sizeof(size_t);
        PAMI_assertf(num >= 2, "local_barriered_shmemzero() requires enough shmem for at least two counters\n");
        size_t c0 = num - 2;
        size_t c1 = num - 1;
        if (master) {
                size_t value = ctrs[c1] + participants;
                size_t blk1 = (char *)&ctrs[c0] - (char *)shmem;
                size_t blk2 = len - ((char *)&ctrs[c1] - (char *)shmem);
                memset(shmem, 0, blk1);
                __sync_fetch_and_add(&ctrs[c1], 1);
                while (ctrs[c1] != value) {
                        __sync_fetch_and_add(&ctrs[c0], 1);
                        PAMI::Memory::sync();
                }
                memset((void *)&ctrs[c1], 0, blk2);
                __sync_fetch_and_and(&ctrs[c0], 0);
        } else {
                size_t value = ctrs[c0];
                while (ctrs[c0] == value);
                __sync_fetch_and_add(&ctrs[c1], 1);
                while (ctrs[c0] != 0);
        }
}
#endif // __local_barriered_shmemzero_fn__

///
/// \brief Structure to resize, or cast, a pointer to a fixed size array type
///
/// Generic programming in C++ can specify the dimension of an array as a
/// template parameter for a method. To change the dimension template parameter
/// a different fixed size array type must be specified by the caller. C++
/// doesn't provide any neat and tidy way of doing this kind of type conversion.
/// The solution is to cast the pointer to a pointer to a structure that
/// contains an array of the appropriate size, then specify this structure
/// field as the parameter to the templatized method.
///
/// \code
/// template <unsigned T_Size>
/// void foo (uint8_t (&parameter)[T_Size])
/// {
///   fprintf (stderr, "The array size is: %d\n", T_Size);
/// }
///
/// uint8_t bigarray[1024];
/// foo (bigarray);       // prints: "The array size is: 1024"
///
/// array_t<uint8_t,16> * resized = (array_t<uint8_t,16> *) &bigarray[512];
/// foo (resized->array); // prints: "The array size is: 16"
/// \endcode
///
/// \tparam T Resized array type
/// \tparam N Resized array element count
///
template <typename T, unsigned N>
struct array_t
{
  T array[N];
};



#endif // __util_common_h__
