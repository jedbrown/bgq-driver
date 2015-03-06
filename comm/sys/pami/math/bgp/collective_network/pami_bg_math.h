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
 * \file math/bgp/collective_network/pami_bg_math.h
 * \brief Prototypes for all the collective network math functions
 *
 * These routines are used when math operations are being performed
 * on the BlueGene/P collective network hardware. The N-way functions
 * perform local math and convert the results for use on the tree
 * with the same operation and datatype. The unary functions are
 * used to perform conversion to/from the tree.
 */


#ifndef __math_bgp_collective_network_pami_bg_math_h__
#define __math_bgp_collective_network_pami_bg_math_h__

#include <stdint.h>
#include "math/math_coremath.h"

/**
 * \brief Convert a LOC value TO a tree value
 * Convert LOC (MINLOC/MAXLOC) values to collective network
 * values that ensures signs are observed.
 */
#define LOC_INT_TOTREE(i)     ((i) ^ 0x7fffffffUL)
/**
 * \brief Convert a LOC value FROM a tree value
 * Convert LOC (MINLOC/MAXLOC) values to collective network
 * values that ensures signs are observed.
 */
#define LOC_INT_FRTREE(i)     ((i) ^ 0x7fffffffUL)

/* Must be included after math_coremath.h */
#include "pami_optibgmath.h"

/**
 * \brief Translate operation, datatype, and optimization to math function
 *
 * This table should not be accessed directly. The inline/macro
 * PAMI_PRE_OP_FUNCS(dt,op,n) should be used instead. That returns
 * the proper function (pointer) for the given combination of
 * datatype "dt", operand "op", and optimization level "n", taking
 * into account conbinations that are not optimized.
 */
extern void *pami_pre_op_funcs[PAMI_OP_COUNT][PAMI_DT_COUNT][2];
/**
 * \brief Translate operation, datatype, and optimization to math function
 *
 * This table should not be accessed directly. The inline/macro
 * PAMI_POST_OP_FUNCS(dt,op,n) should be used instead. That returns
 * the proper function (pointer) for the given combination of
 * datatype "dt", operand "op", and optimization level "n", taking
 * into account conbinations that are not optimized.
 */
extern void *pami_post_op_funcs[PAMI_OP_COUNT][PAMI_DT_COUNT][2];

/**
 * \brief Return best tree pre-processing routine for datatype and operand.
 *
 * Selects an optimized conversion routine from pami_pre_op_funcs[][][] or
 * defaults to the generic, unoptimized, routine if no
 * better one exists.
 *
 * \param dt    Datatype being used
 * \param op    Operand being used
 * \param optim Optimization flag (0/1)
 * \return      Pointer to coremath1 function. NULL if no processing
 * is needed on the tree, (void *)PAMI_UNIMPL if the tree does not support
 * the operation on the datatype.
 */
static inline coremath1 PAMI_PRE_OP_FUNCS(pami_dt dt, pami_op op, int optim) {
        return (coremath1)(pami_pre_op_funcs[op][dt][1] ?
                                pami_pre_op_funcs[op][dt][1] :
                                pami_pre_op_funcs[op][dt][0]);
}

/**
 * \brief Return best tree post-processing routine for datatype and operand.
 *
 * Selects an optimized conversion routine from pami_post_op_funcs[][][] or
 * defaults to the generic, unoptimized, routine if no
 * better one exists.
 *
 * \param dt    Datatype being used
 * \param op    Operand being used
 * \param optim Optimization flag (0/1)
 * \return      Pointer to coremath1 function. NULL if no processing
 * is needed on the tree, (void *)PAMI_UNIMPL if the tree does not support
 * the operation on the datatype.
 */
static inline coremath1 PAMI_POST_OP_FUNCS(pami_dt dt, pami_op op, int optim) {
        return (coremath1)(pami_post_op_funcs[op][dt][1] ?
                                pami_post_op_funcs[op][dt][1] :
                                pami_post_op_funcs[op][dt][0]);
}

/* Here is how we expand defines in pami_optibgmath.h into code: */
#ifndef PAMI_NO_OPTIMATH
/** \brief Create a "case" value for use in a switch statement */
#define OPTIMATH_NSRC(dt,op,n,f)	case n: f(dst, srcs, nsrc, count); break;
#else /* PAMI_NO_OPTIMATH */
#define OPTIMATH_NSRC(dt,op,n,f)
#endif /* PAMI_NO_OPTIMATH */

#if defined(__cplusplus)
extern "C"
{
#endif

/**** N-way routines ****/

#undef OPTIMATH_NSRC

/**** Unary routines ****/

/* Here is how we expand defines in pami_optibgmath.h into code: */
#ifndef PAMI_NO_OPTIMATH
/** \brief Create a "case" value for use in a switch statement */
#define OPTIMATH_UNARY(dt,op,f)	case 1: f(dst, src, count); break;
#else /* PAMI_NO_OPTIMATH */
#define OPTIMATH_UNARY(dt,op,f)
#endif /* PAMI_NO_OPTIMATH */

/**
 * \brief Optimized pre-processing for default on signed-char datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for default operation on \a count signed-char
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_int8_pre_all(uint8_t *dst, const int8_t *src, int count) {
        switch(1) {
        OPTIMIZED_int8_pre_all(PAMI_NOOP)
        default:
                _pami_core_int8_pre_all(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for default on signed-char datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for default operation on \a count signed-char
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_int8_post_all(int8_t *dst, const uint8_t *src, int count) {
        switch(1) {
        OPTIMIZED_int8_post_all(PAMI_NOOP)
        default:
                _pami_core_int8_post_all(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for MIN on signed-char datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for MIN operation on \a count signed-char
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_int8_pre_min(uint8_t *dst, const int8_t *src, int count) {
        switch(1) {
        OPTIMIZED_int8_pre_min
        default:
                _pami_core_int8_pre_min(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for MIN on signed-char datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for MIN operation on \a count signed-char
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_int8_post_min(int8_t *dst, const uint8_t *src, int count) {
        switch(1) {
        OPTIMIZED_int8_post_min
        default:
                _pami_core_int8_post_min(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for default on unsigned-char datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for default operation on \a count unsigned-char
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_uint8_pre_all(uint8_t *dst, const uint8_t *src, int count) {
        switch(1) {
        OPTIMIZED_uint8_pre_all(PAMI_NOOP)
        default:
                _pami_core_uint8_pre_all(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for default on unsigned-char datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for default operation on \a count unsigned-char
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_uint8_post_all(uint8_t *dst, const uint8_t *src, int count) {
        switch(1) {
        OPTIMIZED_uint8_post_all(PAMI_NOOP)
        default:
                _pami_core_uint8_post_all(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for MIN on unsigned-char datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for MIN operation on \a count unsigned-char
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_uint8_pre_min(uint8_t *dst, const uint8_t *src, int count) {
        switch(1) {
        OPTIMIZED_uint8_pre_min
        default:
                _pami_core_uint8_pre_min(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for MIN on unsigned-char datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for MIN operation on \a count unsigned-char
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_uint8_post_min(uint8_t *dst, const uint8_t *src, int count) {
        switch(1) {
        OPTIMIZED_uint8_post_min
        default:
                _pami_core_uint8_post_min(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for default on signed-short datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for default operation on \a count signed-short
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_int16_pre_all(uint16_t *dst, const int16_t *src, int count) {
        switch(1) {
        OPTIMIZED_int16_pre_all(PAMI_NOOP)
        default:
                _pami_core_int16_pre_all(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for default on signed-short datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for default operation on \a count signed-short
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_int16_post_all(int16_t *dst, const uint16_t *src, int count) {
        switch(1) {
        OPTIMIZED_int16_post_all(PAMI_NOOP)
        default:
                _pami_core_int16_post_all(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for MIN on signed-short datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for MIN operation on \a count signed-short
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_int16_pre_min(uint16_t *dst, const int16_t *src, int count) {
        switch(1) {
        OPTIMIZED_int16_pre_min
        default:
                _pami_core_int16_pre_min(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for MIN on signed-short datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for MIN operation on \a count signed-short
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_int16_post_min(int16_t *dst, const uint16_t *src, int count) {
        switch(1) {
        OPTIMIZED_int16_post_min
        default:
                _pami_core_int16_post_min(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for MAXLOC on signed-short/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for MAXLOC operation on \a count signed-short/signed-int
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_int16_int32_pre_maxloc(uint16_int32_t *dst, const int16_int32_t *src, int count) {
        switch(1) {
        OPTIMIZED_int16_int32_pre_maxloc
        default:
                _pami_core_int16_int32_pre_maxloc(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for MAXLOC on signed-short/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for MAXLOC operation on \a count signed-short/signed-int
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_int16_int32_post_maxloc(int16_int32_t *dst, const uint16_int32_t *src, int count) {
        switch(1) {
        OPTIMIZED_int16_int32_post_maxloc
        default:
                _pami_core_int16_int32_post_maxloc(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for MINLOC on signed-short/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for MINLOC operation on \a count signed-short/signed-int
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_int16_int32_pre_minloc(uint16_int32_t *dst, const int16_int32_t *src, int count) {
        switch(1) {
        OPTIMIZED_int16_int32_pre_minloc
        default:
                _pami_core_int16_int32_pre_minloc(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for MINLOC on signed-short/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for MINLOC operation on \a count signed-short/signed-int
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_int16_int32_post_minloc(int16_int32_t *dst, const uint16_int32_t *src, int count) {
        switch(1) {
        OPTIMIZED_int16_int32_post_minloc
        default:
                _pami_core_int16_int32_post_minloc(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for default on unsigned-short datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for default operation on \a count unsigned-short
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_uint16_pre_all(uint16_t *dst, const uint16_t *src, int count) {
        switch(1) {
        OPTIMIZED_uint16_pre_all(PAMI_NOOP)
        default:
                _pami_core_uint16_pre_all(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for default on unsigned-short datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for default operation on \a count unsigned-short
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_uint16_post_all(uint16_t *dst, const uint16_t *src, int count) {
        switch(1) {
        OPTIMIZED_uint16_post_all(PAMI_NOOP)
        default:
                _pami_core_uint16_post_all(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for MIN on unsigned-short datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for MIN operation on \a count unsigned-short
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_uint16_pre_min(uint16_t *dst, const uint16_t *src, int count) {
        switch(1) {
        OPTIMIZED_uint16_pre_min
        default:
                _pami_core_uint16_pre_min(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for MIN on unsigned-short datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for MIN operation on \a count unsigned-short
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_uint16_post_min(uint16_t *dst, const uint16_t *src, int count) {
        switch(1) {
        OPTIMIZED_uint16_post_min
        default:
                _pami_core_uint16_post_min(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for default on signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for default operation on \a count signed-int
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_int32_pre_all(uint32_t *dst, const int32_t *src, int count) {
        switch(1) {
        OPTIMIZED_int32_pre_all(PAMI_NOOP)
        default:
                _pami_core_int32_pre_all(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for default on signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for default operation on \a count signed-int
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_int32_post_all(int32_t *dst, const uint32_t *src, int count) {
        switch(1) {
        OPTIMIZED_int32_post_all(PAMI_NOOP)
        default:
                _pami_core_int32_post_all(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for MIN on signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for MIN operation on \a count signed-int
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_int32_pre_min(uint32_t *dst, const int32_t *src, int count) {
        switch(1) {
        OPTIMIZED_int32_pre_min
        default:
                _pami_core_int32_pre_min(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for MIN on signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for MIN operation on \a count signed-int
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_int32_post_min(int32_t *dst, const uint32_t *src, int count) {
        switch(1) {
        OPTIMIZED_int32_post_min
        default:
                _pami_core_int32_post_min(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for MAXLOC on signed-int/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for MAXLOC operation on \a count signed-int/signed-int
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_int32_int32_pre_maxloc(uint32_int32_t *dst, const int32_int32_t *src, int count) {
        switch(1) {
        OPTIMIZED_int32_int32_pre_maxloc
        default:
                _pami_core_int32_int32_pre_maxloc(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for MAXLOC on signed-int/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for MAXLOC operation on \a count signed-int/signed-int
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_int32_int32_post_maxloc(int32_int32_t *dst, const uint32_int32_t *src, int count) {
        switch(1) {
        OPTIMIZED_int32_int32_post_maxloc
        default:
                _pami_core_int32_int32_post_maxloc(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for MINLOC on signed-int/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for MINLOC operation on \a count signed-int/signed-int
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_int32_int32_pre_minloc(uint32_int32_t *dst, const int32_int32_t *src, int count) {
        switch(1) {
        OPTIMIZED_int32_int32_pre_minloc
        default:
                _pami_core_int32_int32_pre_minloc(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for MINLOC on signed-int/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for MINLOC operation on \a count signed-int/signed-int
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_int32_int32_post_minloc(int32_int32_t *dst, const uint32_int32_t *src, int count) {
        switch(1) {
        OPTIMIZED_int32_int32_post_minloc
        default:
                _pami_core_int32_int32_post_minloc(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for default on unsigned-int datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for default operation on \a count unsigned-int
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_uint32_pre_all(uint32_t *dst, const uint32_t *src, int count) {
        switch(1) {
        OPTIMIZED_uint32_pre_all(PAMI_NOOP)
        default:
                _pami_core_uint32_pre_all(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for default on unsigned-int datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for default operation on \a count unsigned-int
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_uint32_post_all(uint32_t *dst, const uint32_t *src, int count) {
        switch(1) {
        OPTIMIZED_uint32_post_all(PAMI_NOOP)
        default:
                _pami_core_uint32_post_all(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for MIN on unsigned-int datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for MIN operation on \a count unsigned-int
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_uint32_pre_min(uint32_t *dst, const uint32_t *src, int count) {
        switch(1) {
        OPTIMIZED_uint32_pre_min
        default:
                _pami_core_uint32_pre_min(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for MIN on unsigned-int datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for MIN operation on \a count unsigned-int
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_uint32_post_min(uint32_t *dst, const uint32_t *src, int count) {
        switch(1) {
        OPTIMIZED_uint32_post_min
        default:
                _pami_core_uint32_post_min(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for default on signed-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for default operation on \a count signed-long-long
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_int64_pre_all(uint64_t *dst, const int64_t *src, int count) {
        switch(1) {
        OPTIMIZED_int64_pre_all(PAMI_NOOP)
        default:
                _pami_core_int64_pre_all(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for default on signed-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for default operation on \a count signed-long-long
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_int64_post_all(int64_t *dst, const uint64_t *src, int count) {
        switch(1) {
        OPTIMIZED_int64_post_all(PAMI_NOOP)
        default:
                _pami_core_int64_post_all(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for MIN on signed-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for MIN operation on \a count signed-long-long
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_int64_pre_min(uint64_t *dst, const int64_t *src, int count) {
        switch(1) {
        OPTIMIZED_int64_pre_min
        default:
                _pami_core_int64_pre_min(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for MIN on signed-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for MIN operation on \a count signed-long-long
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_int64_post_min(int64_t *dst, const uint64_t *src, int count) {
        switch(1) {
        OPTIMIZED_int64_post_min
        default:
                _pami_core_int64_post_min(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for default on unsigned-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for default operation on \a count unsigned-long-long
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_uint64_pre_all(uint64_t *dst, const uint64_t *src, int count) {
        switch(1) {
        OPTIMIZED_uint64_pre_all(PAMI_NOOP)
        default:
                _pami_core_uint64_pre_all(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for default on unsigned-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for default operation on \a count unsigned-long-long
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_uint64_post_all(uint64_t *dst, const uint64_t *src, int count) {
        switch(1) {
        OPTIMIZED_uint64_post_all(PAMI_NOOP)
        default:
                _pami_core_uint64_post_all(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for MIN on unsigned-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for MIN operation on \a count unsigned-long-long
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_uint64_pre_min(uint64_t *dst, const uint64_t *src, int count) {
        switch(1) {
        OPTIMIZED_uint64_pre_min
        default:
                _pami_core_uint64_pre_min(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for MIN on unsigned-long-long datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for MIN operation on \a count unsigned-long-long
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_uint64_post_min(uint64_t *dst, const uint64_t *src, int count) {
        switch(1) {
        OPTIMIZED_uint64_post_min
        default:
                _pami_core_uint64_post_min(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for MAX on float datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for MAX operation on \a count float
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_fp32_pre_max(float *dst, const float *src, int count) {
        switch(1) {
        OPTIMIZED_fp32_pre_max
        default:
                _pami_core_fp32_pre_max(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for MAX on float datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for MAX operation on \a count float
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_fp32_post_max(float *dst, const float *src, int count) {
        switch(1) {
        OPTIMIZED_fp32_post_max
        default:
                _pami_core_fp32_post_max(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for MAXLOC on float/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for MAXLOC operation on \a count float/signed-int
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_fp32_int32_pre_maxloc(fp32_int32_t *dst, const fp32_int32_t *src, int count) {
        switch(1) {
        OPTIMIZED_fp32_int32_pre_maxloc
        default:
                _pami_core_fp32_int32_pre_maxloc(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for MAXLOC on float/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for MAXLOC operation on \a count float/signed-int
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_fp32_int32_post_maxloc(fp32_int32_t *dst, const fp32_int32_t *src, int count) {
        switch(1) {
        OPTIMIZED_fp32_int32_post_maxloc
        default:
                _pami_core_fp32_int32_post_maxloc(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for MAXLOC on float/float datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for MAXLOC operation on \a count float/float
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_fp32_fp32_pre_maxloc(fp32_fp32_t *dst, const fp32_fp32_t *src, int count) {
        switch(1) {
        OPTIMIZED_fp32_fp32_pre_maxloc
        default:
                _pami_core_fp32_fp32_pre_maxloc(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for MAXLOC on float/float datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for MAXLOC operation on \a count float/float
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_fp32_fp32_post_maxloc(fp32_fp32_t *dst, const fp32_fp32_t *src, int count) {
        switch(1) {
        OPTIMIZED_fp32_fp32_post_maxloc
        default:
                _pami_core_fp32_fp32_post_maxloc(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for MIN on float datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for MIN operation on \a count float
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_fp32_pre_min(float *dst, const float *src, int count) {
        switch(1) {
        OPTIMIZED_fp32_pre_min
        default:
                _pami_core_fp32_pre_min(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for MIN on float datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for MIN operation on \a count float
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_fp32_post_min(float *dst, const float *src, int count) {
        switch(1) {
        OPTIMIZED_fp32_post_min
        default:
                _pami_core_fp32_post_min(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for MINLOC on float/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for MINLOC operation on \a count float/signed-int
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_fp32_int32_pre_minloc(fp32_int32_t *dst, const fp32_int32_t *src, int count) {
        switch(1) {
        OPTIMIZED_fp32_int32_pre_minloc
        default:
                _pami_core_fp32_int32_pre_minloc(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for MINLOC on float/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for MINLOC operation on \a count float/signed-int
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_fp32_int32_post_minloc(fp32_int32_t *dst, const fp32_int32_t *src, int count) {
        switch(1) {
        OPTIMIZED_fp32_int32_post_minloc
        default:
                _pami_core_fp32_int32_post_minloc(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for MINLOC on float/float datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for MINLOC operation on \a count float/float
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_fp32_fp32_pre_minloc(fp32_fp32_t *dst, const fp32_fp32_t *src, int count) {
        switch(1) {
        OPTIMIZED_fp32_fp32_pre_minloc
        default:
                _pami_core_fp32_fp32_pre_minloc(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for MINLOC on float/float datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for MINLOC operation on \a count float/float
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_fp32_fp32_post_minloc(fp32_fp32_t *dst, const fp32_fp32_t *src, int count) {
        switch(1) {
        OPTIMIZED_fp32_fp32_post_minloc
        default:
                _pami_core_fp32_fp32_post_minloc(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for MAX on double datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for MAX operation on \a count double
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_fp64_pre_max(double *dst, const double *src, int count) {
        switch(1) {
        OPTIMIZED_fp64_pre_max
        default:
                _pami_core_fp64_pre_max(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for MAX on double datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for MAX operation on \a count double
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_fp64_post_max(double *dst, const double *src, int count) {
        switch(1) {
        OPTIMIZED_fp64_post_max
        default:
                _pami_core_fp64_post_max(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for default on double datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for default operation on \a count double
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_fp64_pre_all(double *dst, const double *src, int count) {
        switch(1) {
        OPTIMIZED_fp64_pre_all(PAMI_NOOP)
        default:
                _pami_core_fp64_pre_all(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for default on double datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for default operation on \a count double
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_fp64_post_all(double *dst, const double *src, int count) {
        switch(1) {
        OPTIMIZED_fp64_post_all(PAMI_NOOP)
        default:
                _pami_core_fp64_post_all(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for MAXLOC on double/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for MAXLOC operation on \a count double/signed-int
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_fp64_int32_pre_maxloc(fp64_int32_t *dst, const fp64_int32_t *src, int count) {
        switch(1) {
        OPTIMIZED_fp64_int32_pre_maxloc
        default:
                _pami_core_fp64_int32_pre_maxloc(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for MAXLOC on double/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for MAXLOC operation on \a count double/signed-int
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_fp64_int32_post_maxloc(fp64_int32_t *dst, const fp64_int32_t *src, int count) {
        switch(1) {
        OPTIMIZED_fp64_int32_post_maxloc
        default:
                _pami_core_fp64_int32_post_maxloc(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for MAXLOC on double/double datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for MAXLOC operation on \a count double/double
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_fp64_fp64_pre_maxloc(fp64_fp64_t *dst, const fp64_fp64_t *src, int count) {
        switch(1) {
        OPTIMIZED_fp64_fp64_pre_maxloc
        default:
                _pami_core_fp64_fp64_pre_maxloc(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for MAXLOC on double/double datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for MAXLOC operation on \a count double/double
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_fp64_fp64_post_maxloc(fp64_fp64_t *dst, const fp64_fp64_t *src, int count) {
        switch(1) {
        OPTIMIZED_fp64_fp64_post_maxloc
        default:
                _pami_core_fp64_fp64_post_maxloc(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for MIN on double datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for MIN operation on \a count double
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_fp64_pre_min(double *dst, const double *src, int count) {
        switch(1) {
        OPTIMIZED_fp64_pre_min
        default:
                _pami_core_fp64_pre_min(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for MIN on double datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for MIN operation on \a count double
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_fp64_post_min(double *dst, const double *src, int count) {
        switch(1) {
        OPTIMIZED_fp64_post_min
        default:
                _pami_core_fp64_post_min(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for MINLOC on double/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for MINLOC operation on \a count double/signed-int
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_fp64_int32_pre_minloc(fp64_int32_t *dst, const fp64_int32_t *src, int count) {
        switch(1) {
        OPTIMIZED_fp64_int32_pre_minloc
        default:
                _pami_core_fp64_int32_pre_minloc(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for MINLOC on double/signed-int datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for MINLOC operation on \a count double/signed-int
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_fp64_int32_post_minloc(fp64_int32_t *dst, const fp64_int32_t *src, int count) {
        switch(1) {
        OPTIMIZED_fp64_int32_post_minloc
        default:
                _pami_core_fp64_int32_post_minloc(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized pre-processing for MINLOC on double/double datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized pre-processing for MINLOC operation on \a count double/double
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_fp64_fp64_pre_minloc(fp64_fp64_t *dst, const fp64_fp64_t *src, int count) {
        switch(1) {
        OPTIMIZED_fp64_fp64_pre_minloc
        default:
                _pami_core_fp64_fp64_pre_minloc(dst, src, count);
                break;
        }
}

/**
 * \brief Optimized post-processing for MINLOC on double/double datatypes.
 *
 * \param dst The destination buffer.
 * \param src The source buffer.
 * \param count The number of elements.
 *
 * Optimized post-processing for MINLOC operation on \a count double/double
 * elements of the source buffer, results in destination buffer.
 */
static inline void Core_fp64_fp64_post_minloc(fp64_fp64_t *dst, const fp64_fp64_t *src, int count) {
        switch(1) {
        OPTIMIZED_fp64_fp64_post_minloc
        default:
                _pami_core_fp64_fp64_post_minloc(dst, src, count);
                break;
        }
}

#undef OPTIMATH_UNARY

#if defined(__cplusplus)
}; // extern "C"
#endif

#endif /* _pami_bg_math_h_ */
