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
 * \file math/math_coremath.h
 * \brief Prototypes for all the msglayer math functions
 *
 * This file is the user interface to the core math routines.
 * THese routines are general-purpose and platform-independent.
 */


#ifndef __math_math_coremath_h__
#define __math_math_coremath_h__

#include <stdint.h>
#include <stdlib.h>
#include <pami.h>
#include "Arch.h"
#include "util/common.h"
#include "common/type/TypeCode.h"
#include "common/type/TypeFunc.h"


/** \brief The maximum number of sources based on the number of cores (PEs) */
#ifndef MATH_MAX_NSRC
#define MATH_MAX_NSRC	PAMI_MAX_PROC_PER_NODE
#endif

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))   /**< Maximum macro */
#endif

#ifndef MIN
#define MIN(a,b) (((a)>(b))?(b):(a))   /**< Minimum macro */
#endif

/**
 * \brief MAXLOC and MINLOC operation element type for signed 16-bit and signed 32-bit data.
 */
typedef struct
{
    int16_t  a; /**< First operand */
    uint16_t z; /**< Blank hole     */
    int32_t  b; /**< Second operand */
} int16_int32_t;

/**
 * \brief MAXLOC and MINLOC operation element type for unsigned 16-bit and signed 32-bit data.
 */
typedef struct
{
    uint16_t a; /**< First operand */
    uint16_t z; /**< Blank hole     */
    int32_t  b; /**< Second operand */
} uint16_int32_t;

/**
 * \brief MAXLOC and MINLOC operation element type for signed 32-bit and signed 32-bit data.
 */
typedef struct
{
  int32_t  a; /**< First operand */
  int32_t  b; /**< Second operand */
} int32_int32_t;

/**
 * \brief MAXLOC and MINLOC operation element type for unsigned 32-bit and signed 32-bit data.
 */
typedef struct
{
  uint32_t a; /**< First operand */
  int32_t  b; /**< Second operand */
} uint32_int32_t;

/**
 * \brief MAXLOC and MINLOC operation element type for unsigned 32-bit and unsigned 32-bit data.
 */
typedef struct
{
  uint32_t a; /**< First operand */
  uint32_t b; /**< Second operand */
} uint32_uint32_t;

/**
 * \brief MAXLOC and MINLOC operation element type for unsigned 64-bit and signed 32-bit data.
 */
typedef struct
{
  uint64_t a; /**< First operand */
  int32_t b; /**< Second operand */
  int32_t z; /**< Blank hole     */
} uint64_int32_t;

/**
 * \brief MAXLOC and MINLOC operation element type for signed 64-bit and signed 32-bit data.
 */
typedef struct
{
  int64_t a; /**< First operand */
  int32_t b; /**< Second operand */
  int32_t z; /**< Blank hole     */
} int64_int32_t;

/**
 * \brief MAXLOC and MINLOC operation element type for unsigned 64-bit and unsigned 64-bit data.
 */
typedef struct
{
  uint64_t a; /**< First operand */
  uint64_t b; /**< Second operand */
} uint64_uint64_t;

/**
 * \brief MAXLOC and MINLOC operation element type for float (32-bit) and signed 32-bit data.
 */
typedef struct
{
  float    a; /**< First operand */
  int32_t  b; /**< Second operand */
} fp32_int32_t;

/**
 * \brief MAXLOC and MINLOC operation element type for float (32-bit) and float (32-bit) data.
 */
typedef struct
{
  float    a; /**< First operand */
  float    b; /**< Second operand */
} fp32_fp32_t;

/**
 * \brief MAXLOC and MINLOC operation element type for double (64-bit) and signed 32-bit data.
 */
typedef struct
{
  double   a; /**< First operand */
  int32_t  b; /**< Second operand */
  uint32_t z; /**< Blank hole     */
} fp64_int32_t;

/**
 * \brief MAXLOC and MINLOC operation element type for double (64-bit) and double (64-bit) data.
 */
typedef struct
{
  double   a; /**< First operand */
  double   b; /**< Second operand */
} fp64_fp64_t;

/**
 * \brief MAXLOC and MINLOC operation element type for long double (128-bit) and signed 32-bit data.
 */
typedef struct
{
  long double a; /**< First operand */
  int32_t  b;    /**< Second operand */
  uint32_t z[3]; /**< Blank hole     */
} fp128_int32_t;

/** \brief Generic prototype of math function with one source */
typedef void (*coremath1) (void *dst, void *src, int count);
/** \brief Generic prototype of math function with multiple sources */
typedef void (*coremath)  (void *dst, void **srcs, int nsrc, int count);

/**
 * \brief Select a math function for operation, datatype, and number of inputs
 * This table sohuld not be accessed directly. The inline/macro
 * MATH_OP_FUNCS(dt,op,n) should be used instead. That returns
 * the proper function (pointer) for the given combination of
 * datatype "dt", operand "op", and number of inputs "n", taking
 * into account conbinations that are not optimized.
 */
extern void *math_op_funcs[PAMI_OP_COUNT][PAMI_DT_COUNT][MATH_MAX_NSRC];

/**
 * \brief Return best math routine for datatype, operand, and number inputs.
 *
 * Selects an optimized math routine from math_op_funcs[][][] or
 * defaults to the generic, unoptimized, n-way math routine if no
 * better one exists.
 *
 * \param dt	Datatype being used
 * \param op	Operand being used
 * \param nsrc	Number of input buffers
 * \return	Pointer to coremath function
 */
inline coremath MATH_OP_FUNCS(pami_dt dt, pami_op op, int nsrc) {
        PAMI_assert(nsrc < MATH_MAX_NSRC);
        int n = nsrc - 1;
        return (coremath)(math_op_funcs[op][dt][n] ?
                        math_op_funcs[op][dt][n] :
                        math_op_funcs[op][dt][0]);
}

/**
 * \brief Optimized Memory Copy
 */
extern void *Core_memcpy(void *dst, const void *src, size_t len);

/*
 * We include this "late" so that it may use the above typedefs, etc.
 * Build should pull it out of the appropriate directory (common/ppc450d/etc)
 */
#include "math_optimath.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/**** N-way routines ****/

#ifndef MATH_NO_OPTIMATH
/** \brief Create a "case" value for use in a switch statement */
#define OPTIMATH_NSRC(dt,op,n,f)	case n: f(dst, srcs, nsrc, count); break;
#else /* MATH_NO_OPTIMATH */
#define OPTIMATH_NSRC(dt,op,n,f)
#endif /* MATH_NO_OPTIMATH */

/**
 * \brief Optimized bitwise AND on signed-char datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized bitwise AND operation on \a count signed-char
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int8_band(int8_t *dst, const int8_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int8_band
        default:
                _pami_core_int8_band(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized bitwise OR on signed-char datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized bitwise OR operation on \a count signed-char
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int8_bor(int8_t *dst, const int8_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int8_bor
        default:
                _pami_core_int8_bor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized bitwise XOR on signed-char datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized bitwise XOR operation on \a count signed-char
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int8_bxor(int8_t *dst, const int8_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int8_bxor
        default:
                _pami_core_int8_bxor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized logical AND on signed-char datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized logical AND operation on \a count signed-char
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int8_land(int8_t *dst, const int8_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int8_land
        default:
                _pami_core_int8_land(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized logical OR on signed-char datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized logical OR operation on \a count signed-char
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int8_lor(int8_t *dst, const int8_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int8_lor
        default:
                _pami_core_int8_lor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized logical XOR on signed-char datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized logical XOR operation on \a count signed-char
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int8_lxor(int8_t *dst, const int8_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int8_lxor
        default:
                _pami_core_int8_lxor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MAX on signed-char datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MAX operation on \a count signed-char
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int8_max(int8_t *dst, const int8_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int8_max
        default:
                _pami_core_int8_max(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MIN on signed-char datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MIN operation on \a count signed-char
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int8_min(int8_t *dst, const int8_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int8_min
        default:
                _pami_core_int8_min(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized multiplication on signed-char datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized multiplication operation on \a count signed-char
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int8_prod(int8_t *dst, const int8_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int8_prod
        default:
                _pami_core_int8_prod(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized addition on signed-char datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized addition operation on \a count signed-char
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int8_sum(int8_t *dst, const int8_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int8_sum
        default:
                _pami_core_int8_sum(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized bitwise AND on unsigned-char datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized bitwise AND operation on \a count unsigned-char
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint8_band(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint8_band
        default:
                _pami_core_uint8_band(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized bitwise OR on unsigned-char datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized bitwise OR operation on \a count unsigned-char
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint8_bor(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint8_bor
        default:
                _pami_core_uint8_bor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized bitwise XOR on unsigned-char datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized bitwise XOR operation on \a count unsigned-char
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint8_bxor(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint8_bxor
        default:
                _pami_core_uint8_bxor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized logical AND on unsigned-char datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized logical AND operation on \a count unsigned-char
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint8_land(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint8_land
        default:
                _pami_core_uint8_land(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized logical OR on unsigned-char datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized logical OR operation on \a count unsigned-char
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint8_lor(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint8_lor
        default:
                _pami_core_uint8_lor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized logical XOR on unsigned-char datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized logical XOR operation on \a count unsigned-char
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint8_lxor(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint8_lxor
        default:
                _pami_core_uint8_lxor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MAX on unsigned-char datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MAX operation on \a count unsigned-char
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint8_max(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint8_max
        default:
                _pami_core_uint8_max(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MIN on unsigned-char datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MIN operation on \a count unsigned-char
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint8_min(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint8_min
        default:
                _pami_core_uint8_min(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized multiplication on unsigned-char datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized multiplication operation on \a count unsigned-char
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint8_prod(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint8_prod
        default:
                _pami_core_uint8_prod(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized addition on unsigned-char datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized addition operation on \a count unsigned-char
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint8_sum(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint8_sum
        default:
                _pami_core_uint8_sum(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized bitwise AND on signed-short datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized bitwise AND operation on \a count signed-short
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int16_band(int16_t *dst, const int16_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int16_band
        default:
                _pami_core_int16_band(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized bitwise OR on signed-short datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized bitwise OR operation on \a count signed-short
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int16_bor(int16_t *dst, const int16_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int16_bor
        default:
                _pami_core_int16_bor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized bitwise XOR on signed-short datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized bitwise XOR operation on \a count signed-short
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int16_bxor(int16_t *dst, const int16_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int16_bxor
        default:
                _pami_core_int16_bxor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized logical AND on signed-short datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized logical AND operation on \a count signed-short
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int16_land(int16_t *dst, const int16_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int16_land
        default:
                _pami_core_int16_land(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized logical OR on signed-short datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized logical OR operation on \a count signed-short
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int16_lor(int16_t *dst, const int16_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int16_lor
        default:
                _pami_core_int16_lor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized logical XOR on signed-short datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized logical XOR operation on \a count signed-short
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int16_lxor(int16_t *dst, const int16_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int16_lxor
        default:
                _pami_core_int16_lxor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MAX on signed-short datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MAX operation on \a count signed-short
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int16_max(int16_t *dst, const int16_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int16_max
        default:
                _pami_core_int16_max(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MIN on signed-short datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MIN operation on \a count signed-short
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int16_min(int16_t *dst, const int16_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int16_min
        default:
                _pami_core_int16_min(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized multiplication on signed-short datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized multiplication operation on \a count signed-short
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int16_prod(int16_t *dst, const int16_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int16_prod
        default:
                _pami_core_int16_prod(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized addition on signed-short datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized addition operation on \a count signed-short
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int16_sum(int16_t *dst, const int16_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int16_sum
        default:
                _pami_core_int16_sum(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MAXLOC on signed-short/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MAXLOC operation on \a count signed-short/signed-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int16_int32_maxloc(int16_int32_t *dst, const int16_int32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int16_int32_maxloc
        default:
                _pami_core_int16_int32_maxloc(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MINLOC on signed-short/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MINLOC operation on \a count signed-short/signed-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int16_int32_minloc(int16_int32_t *dst, const int16_int32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int16_int32_minloc
        default:
                _pami_core_int16_int32_minloc(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized bitwise AND on unsigned-short datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized bitwise AND operation on \a count unsigned-short
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint16_band(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint16_band
        default:
                _pami_core_uint16_band(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized bitwise OR on unsigned-short datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized bitwise OR operation on \a count unsigned-short
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint16_bor(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint16_bor
        default:
                _pami_core_uint16_bor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized bitwise XOR on unsigned-short datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized bitwise XOR operation on \a count unsigned-short
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint16_bxor(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint16_bxor
        default:
                _pami_core_uint16_bxor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized logical AND on unsigned-short datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized logical AND operation on \a count unsigned-short
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint16_land(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint16_land
        default:
                _pami_core_uint16_land(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized logical OR on unsigned-short datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized logical OR operation on \a count unsigned-short
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint16_lor(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint16_lor
        default:
                _pami_core_uint16_lor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized logical XOR on unsigned-short datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized logical XOR operation on \a count unsigned-short
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint16_lxor(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint16_lxor
        default:
                _pami_core_uint16_lxor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MAX on unsigned-short datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MAX operation on \a count unsigned-short
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint16_max(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint16_max
        default:
                _pami_core_uint16_max(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MIN on unsigned-short datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MIN operation on \a count unsigned-short
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint16_min(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint16_min
        default:
                _pami_core_uint16_min(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized multiplication on unsigned-short datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized multiplication operation on \a count unsigned-short
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint16_prod(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint16_prod
        default:
                _pami_core_uint16_prod(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized addition on unsigned-short datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized addition operation on \a count unsigned-short
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint16_sum(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint16_sum
        default:
                _pami_core_uint16_sum(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized bitwise AND on signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized bitwise AND operation on \a count signed-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int32_band(int32_t *dst, const int32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int32_band
        default:
                _pami_core_int32_band(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized bitwise OR on signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized bitwise OR operation on \a count signed-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int32_bor(int32_t *dst, const int32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int32_bor
        default:
                _pami_core_int32_bor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized bitwise XOR on signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized bitwise XOR operation on \a count signed-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int32_bxor(int32_t *dst, const int32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int32_bxor
        default:
                _pami_core_int32_bxor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized logical AND on signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized logical AND operation on \a count signed-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int32_land(int32_t *dst, const int32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int32_land
        default:
                _pami_core_int32_land(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized logical OR on signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized logical OR operation on \a count signed-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int32_lor(int32_t *dst, const int32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int32_lor
        default:
                _pami_core_int32_lor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized logical XOR on signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized logical XOR operation on \a count signed-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int32_lxor(int32_t *dst, const int32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int32_lxor
        default:
                _pami_core_int32_lxor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MAX on signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MAX operation on \a count signed-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int32_max(int32_t *dst, const int32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int32_max
        default:
                _pami_core_int32_max(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MIN on signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MIN operation on \a count signed-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int32_min(int32_t *dst, const int32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int32_min
        default:
                _pami_core_int32_min(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized multiplication on signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized multiplication operation on \a count signed-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int32_prod(int32_t *dst, const int32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int32_prod
        default:
                _pami_core_int32_prod(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized addition on signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized addition operation on \a count signed-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int32_sum(int32_t *dst, const int32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int32_sum
        default:
                _pami_core_int32_sum(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MAXLOC on signed-int/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MAXLOC operation on \a count signed-int/signed-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int32_int32_maxloc(int32_int32_t *dst, const int32_int32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int32_int32_maxloc
        default:
                _pami_core_int32_int32_maxloc(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MINLOC on signed-int/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MINLOC operation on \a count signed-int/signed-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int32_int32_minloc(int32_int32_t *dst, const int32_int32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int32_int32_minloc
        default:
                _pami_core_int32_int32_minloc(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized bitwise AND on unsigned-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized bitwise AND operation on \a count unsigned-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint32_band(uint32_t *dst, const uint32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint32_band
        default:
                _pami_core_uint32_band(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized bitwise OR on unsigned-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized bitwise OR operation on \a count unsigned-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint32_bor(uint32_t *dst, const uint32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint32_bor
        default:
                _pami_core_uint32_bor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized bitwise XOR on unsigned-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized bitwise XOR operation on \a count unsigned-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint32_bxor(uint32_t *dst, const uint32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint32_bxor
        default:
                _pami_core_uint32_bxor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized logical AND on unsigned-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized logical AND operation on \a count unsigned-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint32_land(uint32_t *dst, const uint32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint32_land
        default:
                _pami_core_uint32_land(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized logical OR on unsigned-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized logical OR operation on \a count unsigned-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint32_lor(uint32_t *dst, const uint32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint32_lor
        default:
                _pami_core_uint32_lor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized logical XOR on unsigned-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized logical XOR operation on \a count unsigned-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint32_lxor(uint32_t *dst, const uint32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint32_lxor
        default:
                _pami_core_uint32_lxor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MAX on unsigned-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MAX operation on \a count unsigned-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint32_max(uint32_t *dst, const uint32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint32_max
        default:
                _pami_core_uint32_max(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MIN on unsigned-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MIN operation on \a count unsigned-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint32_min(uint32_t *dst, const uint32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint32_min
        default:
                _pami_core_uint32_min(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized multiplication on unsigned-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized multiplication operation on \a count unsigned-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint32_prod(uint32_t *dst, const uint32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint32_prod
        default:
                _pami_core_uint32_prod(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized addition on unsigned-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized addition operation on \a count unsigned-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint32_sum(uint32_t *dst, const uint32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint32_sum
        default:
                _pami_core_uint32_sum(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized bitwise AND on signed-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized bitwise AND operation on \a count signed-long-long
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int64_band(int64_t *dst, const int64_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int64_band
        default:
                _pami_core_int64_band(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized bitwise OR on signed-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized bitwise OR operation on \a count signed-long-long
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int64_bor(int64_t *dst, const int64_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int64_bor
        default:
                _pami_core_int64_bor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized bitwise XOR on signed-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized bitwise XOR operation on \a count signed-long-long
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int64_bxor(int64_t *dst, const int64_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int64_bxor
        default:
                _pami_core_int64_bxor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized logical AND on signed-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized logical AND operation on \a count signed-long-long
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int64_land(int64_t *dst, const int64_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int64_land
        default:
                _pami_core_int64_land(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized logical OR on signed-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized logical OR operation on \a count signed-long-long
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int64_lor(int64_t *dst, const int64_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int64_lor
        default:
                _pami_core_int64_lor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized logical XOR on signed-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized logical XOR operation on \a count signed-long-long
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int64_lxor(int64_t *dst, const int64_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int64_lxor
        default:
                _pami_core_int64_lxor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MAX on signed-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MAX operation on \a count signed-long-long
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int64_max(int64_t *dst, const int64_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int64_max
        default:
                _pami_core_int64_max(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MIN on signed-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MIN operation on \a count signed-long-long
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int64_min(int64_t *dst, const int64_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int64_min
        default:
                _pami_core_int64_min(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized multiplication on signed-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized multiplication operation on \a count signed-long-long
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int64_prod(int64_t *dst, const int64_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int64_prod
        default:
                _pami_core_int64_prod(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized addition on signed-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized addition operation on \a count signed-long-long
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int64_sum(int64_t *dst, const int64_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int64_sum
        default:
                _pami_core_int64_sum(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MAXLOC on signed-long/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MAXLOC operation on \a count signed-long/signed-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int64_int32_maxloc(int64_int32_t *dst, const int64_int32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int64_int32_maxloc
        default:
                _pami_core_int64_int32_maxloc(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MINLOC on signed-long/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MINLOC operation on \a count signed-long/signed-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_int64_int32_minloc(int64_int32_t *dst, const int64_int32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_int64_int32_minloc
        default:
                _pami_core_int64_int32_minloc(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized bitwise AND on unsigned-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized bitwise AND operation on \a count unsigned-long-long
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint64_band(uint64_t *dst, const uint64_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint64_band
        default:
                _pami_core_uint64_band(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized bitwise OR on unsigned-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized bitwise OR operation on \a count unsigned-long-long
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint64_bor(uint64_t *dst, const uint64_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint64_bor
        default:
                _pami_core_uint64_bor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized bitwise XOR on unsigned-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized bitwise XOR operation on \a count unsigned-long-long
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint64_bxor(uint64_t *dst, const uint64_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint64_bxor
        default:
                _pami_core_uint64_bxor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized logical AND on unsigned-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized logical AND operation on \a count unsigned-long-long
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint64_land(uint64_t *dst, const uint64_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint64_land
        default:
                _pami_core_uint64_land(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized logical OR on unsigned-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized logical OR operation on \a count unsigned-long-long
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint64_lor(uint64_t *dst, const uint64_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint64_lor
        default:
                _pami_core_uint64_lor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized logical XOR on unsigned-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized logical XOR operation on \a count unsigned-long-long
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint64_lxor(uint64_t *dst, const uint64_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint64_lxor
        default:
                _pami_core_uint64_lxor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MAX on unsigned-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MAX operation on \a count unsigned-long-long
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint64_max(uint64_t *dst, const uint64_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint64_max
        default:
                _pami_core_uint64_max(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MIN on unsigned-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MIN operation on \a count unsigned-long-long
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint64_min(uint64_t *dst, const uint64_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint64_min
        default:
                _pami_core_uint64_min(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized multiplication on unsigned-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized multiplication operation on \a count unsigned-long-long
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint64_prod(uint64_t *dst, const uint64_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint64_prod
        default:
                _pami_core_uint64_prod(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized addition on unsigned-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized addition operation on \a count unsigned-long-long
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_uint64_sum(uint64_t *dst, const uint64_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_uint64_sum
        default:
                _pami_core_uint64_sum(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MAX on float datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MAX operation on \a count float
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_fp32_max(float *dst, const float **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_fp32_max
        default:
                _pami_core_fp32_max(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized LXOR on float datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized LXOR operation on \a count float
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_fp32_lxor(float *dst, const float **srcs, int nsrc, int count) {
        switch(nsrc) {
/*        OPTIMIZED_fp32_lxor */
        default:
                _pami_core_fp32_lxor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MIN on float datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MIN operation on \a count float
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_fp32_min(float *dst, const float **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_fp32_min
        default:
                _pami_core_fp32_min(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized multiplication on float datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized multiplication operation on \a count float
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_fp32_prod(float *dst, const float **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_fp32_prod
        default:
                _pami_core_fp32_prod(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized addition on float datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized addition operation on \a count float
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_fp32_sum(float *dst, const float **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_fp32_sum
        default:
                _pami_core_fp32_sum(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MAXLOC on float/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MAXLOC operation on \a count float/signed-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_fp32_int32_maxloc(fp32_int32_t *dst, const fp32_int32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_fp32_int32_maxloc
        default:
                _pami_core_fp32_int32_maxloc(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MAXLOC on float/float datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MAXLOC operation on \a count float/float
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_fp32_fp32_maxloc(fp32_fp32_t *dst, const fp32_fp32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_fp32_fp32_maxloc
        default:
                _pami_core_fp32_fp32_maxloc(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MINLOC on float/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MINLOC operation on \a count float/signed-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_fp32_int32_minloc(fp32_int32_t *dst, const fp32_int32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_fp32_int32_minloc
        default:
                _pami_core_fp32_int32_minloc(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MINLOC on float/float datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MINLOC operation on \a count float/float
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_fp32_fp32_minloc(fp32_fp32_t *dst, const fp32_fp32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_fp32_fp32_minloc
        default:
                _pami_core_fp32_fp32_minloc(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized LXOR on double datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized LXOR operation on \a count double
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_fp64_lxor(double *dst, const double **srcs, int nsrc, int count) {
        switch(nsrc) {
/*        OPTIMIZED_fp64_lxor */
        default:
                _pami_core_fp64_lxor(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MAX on double datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MAX operation on \a count double
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_fp64_max(double *dst, const double **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_fp64_max
        default:
                _pami_core_fp64_max(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MIN on double datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MIN operation on \a count double
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_fp64_min(double *dst, const double **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_fp64_min
        default:
                _pami_core_fp64_min(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized multiplication on double datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized multiplication operation on \a count double
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_fp64_prod(double *dst, const double **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_fp64_prod
        default:
                _pami_core_fp64_prod(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized addition on double datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized addition operation on \a count double
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_fp64_sum(double *dst, const double **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_fp64_sum
        default:
                _pami_core_fp64_sum(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MAXLOC on double/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MAXLOC operation on \a count double/signed-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_fp64_int32_maxloc(fp64_int32_t *dst, const fp64_int32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_fp64_int32_maxloc
        default:
                _pami_core_fp64_int32_maxloc(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MAXLOC on long double/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MAXLOC operation on \a count double/signed-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_fp128_int32_maxloc(fp128_int32_t *dst, const fp128_int32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_fp128_int32_maxloc
        default:
                _pami_core_fp128_int32_maxloc(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MAXLOC on double/double datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MAXLOC operation on \a count double/double
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_fp64_fp64_maxloc(fp64_fp64_t *dst, const fp64_fp64_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_fp64_fp64_maxloc
        default:
                _pami_core_fp64_fp64_maxloc(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MINLOC on double/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MINLOC operation on \a count double/signed-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_fp64_int32_minloc(fp64_int32_t *dst, const fp64_int32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_fp64_int32_minloc
        default:
                _pami_core_fp64_int32_minloc(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MINLOC on double/double datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MINLOC operation on \a count double/double
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_fp64_fp64_minloc(fp64_fp64_t *dst, const fp64_fp64_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_fp64_fp64_minloc
        default:
                _pami_core_fp64_fp64_minloc(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MINLOC on long double/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MINLOC operation on \a count long double/signed-int
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_fp128_int32_minloc(fp128_int32_t *dst, const fp128_int32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_fp128_int32_minloc
        default:
                _pami_core_fp128_int32_minloc(dst, srcs, nsrc, count);
                break;
        }
}


/**
 * \brief Optimized MAX on long-double datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MAX operation on \a count long-double
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_fp128_max(long double *dst, const long double **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_fp128_max
        default:
                _pami_core_fp128_max(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized MIN on long-double datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized MIN operation on \a count long-double
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_fp128_min(long double *dst, const long double **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_fp128_min
        default:
                _pami_core_fp128_min(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized multiplication on long-double datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized multiplication operation on \a count long-double
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_fp128_prod(long double *dst, const long double **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_fp128_prod
        default:
                _pami_core_fp128_prod(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized addition on long-double datatypes.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized addition operation on \a count long-double
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_fp128_sum(long double *dst, const long double **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_fp128_sum
        default:
                _pami_core_fp128_sum(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized multiplication on single complex.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized multiplication operation on \a count single complex
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_fp32_fp32_cmplx_prod(fp32_fp32_t *dst, const fp32_fp32_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_fp32_fp32_cmplx_prod
        default:
                _pami_core_fp32_fp32_cplx_prod(dst, srcs, nsrc, count);
                break;
        }
}

/**
 * \brief Optimized multiplication on double complex.
 *
 * \param dst The destination buffer.
 * \param srcs The source buffers.
 * \param nsrc The number of source buffers.
 * \param count The number of elements.
 *
 * Optimized multiplication operation on \a count double complex
 * elements of the source buffers, results in destination buffer.
 */
static inline void Core_fp64_fp64_cmplx_prod(fp64_fp64_t *dst, const fp64_fp64_t **srcs, int nsrc, int count) {
        switch(nsrc) {
        OPTIMIZED_fp64_fp64_cmplx_prod
        default:
                _pami_core_fp64_fp64_cplx_prod(dst, srcs, nsrc, count);
                break;
        }
}


#undef OPTIMATH_NSRC

/**** Unary routines ****/

#ifndef MATH_NO_OPTIMATH
/** \brief Create a "case" value for use in a switch statement */
#define OPTIMATH_UNARY(dt,op,f)	case 1: f(dst, srcs, nsrc, count); break;
#else /* MATH_NO_OPTIMATH */
#define OPTIMATH_UNARY(dt,op,f)
#endif /* MATH_NO_OPTIMATH */

/* (no unary routines) */

#undef OPTIMATH_UNARY

#if defined(__cplusplus)
}; // extern "C"
#endif

#endif /* __math_math_coremath_h__ */
