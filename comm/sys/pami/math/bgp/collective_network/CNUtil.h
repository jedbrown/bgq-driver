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
 * \file math/bgp/collective_network/CNUtil.h
 * \brief Floating point operation utilities
 *
 * Detailed description.
 */
#ifndef __math_bgp_collective_network_CNUtil_h__
#define __math_bgp_collective_network_CNUtil_h__
#include <stdint.h>
#include "math/FloatUtil.h"

/* ********************************************************* */
/* Datatype-to-tree conversion macros                        */
/* from bglsw/sysbringup/tree/include/BGL_CNReduction.h    */
/* ********************************************************* */

/* ********************************************************* */
/* DOUBLE_MACROS                                             */
/* ********************************************************* */
#define TREE_DOUBLE_EXP_NAN     (DOUBLE_EXP_MAX+2)
#define TREE_DOUBLE_EXP_POS_INF (DOUBLE_EXP_MAX+1)
#define TREE_DOUBLE_EXP_NEG_INF (DOUBLE_EXP_MAX+0)

/* ********************************************************* */
/* FLOAT_MACROS                                             */
/* ********************************************************* */
#define TREE_FLOAT_EXP_NAN     (FLOAT_EXP_MAX+2)
#define TREE_FLOAT_EXP_POS_INF (FLOAT_EXP_MAX+1)
#define TREE_FLOAT_EXP_NEG_INF (FLOAT_EXP_MAX+0)

/* ********************************************************* */
/* Per-element to/from tree macros                           */
/* see bglsw/sysbringup/tree/lib/src/use/BGL_CNReduction.c */
/* ********************************************************* */
/* ************************************************************** */
/* MAX: MPI_FLOAT to tree */
/* For MAX of 32-bit float, use this routine to convert operands entering tree. */
/* ************************************************************** */
#define MAX_FLOAT_TOTREE(u) ( FLOAT_NAN((u)) ? ((u)|FLOAT_BITS_SIGN) : \
                              ( FLOAT_SIGN((u)) ? (FLOAT_BITS_EXP - ((u)^FLOAT_BITS_SIGN)) : \
                                (u)^FLOAT_BITS_SIGN ) )
/* *********************** */
/* MAX: MPI_DOUBLE to tree */
/* *********************** */
#define MAX_DOUBLE_TOTREE(u) ( DOUBLE_NAN((u)) ? ((u)|DOUBLE_BITS_SIGN) : \
                              ( DOUBLE_SIGN((u)) ? (DOUBLE_BITS_EXP - ((u)^DOUBLE_BITS_SIGN)) : \
                                (u)^DOUBLE_BITS_SIGN ) )
/* ************************************************************** */
/* MAX: MPI_FLOAT from tree */
/* For MAX of 32-bit float, use this routine to convert results leaving tree. */
/* ************************************************************** */
#define MAX_FLOAT_FRTREE(u) ( FLOAT_NAN((u)) ? (u) : \
                                ( !FLOAT_SIGN((u)) ? (FLOAT_BITS_EXP - ((u)^FLOAT_BITS_SIGN)) : \
                                  (u)^FLOAT_BITS_SIGN ) )
/* ************************* */
/* MAX: MPI_DOUBLE from tree */
/* ************************* */
#define MAX_DOUBLE_FRTREE(u) ( DOUBLE_NAN((u)) ? (u) : \
                                 ( !DOUBLE_SIGN((u)) ? (DOUBLE_BITS_EXP - ((u)^DOUBLE_BITS_SIGN)) : \
                                   (u)^DOUBLE_BITS_SIGN ) )
/* ************************************************************** */
/* MIN: MPI_FLOAT to tree */
/* For MIN of 32-bit float, use this routine to convert operands entering tree. */
/* ************************************************************** */
#define MIN_FLOAT_TOTREE(u) ( FLOAT_NAN((u)) ? ((u)|FLOAT_BITS_SIGN) : \
                              ( !FLOAT_SIGN((u)) ? (FLOAT_BITS_EXP - (u)) : (u) ) )
/* *********************** */
/* MIN: MPI_DOUBLE to tree */
/* *********************** */
#define MIN_DOUBLE_TOTREE(u) ( DOUBLE_NAN((u)) ? ((u)|DOUBLE_BITS_SIGN) : \
                              ( !DOUBLE_SIGN((u)) ? (DOUBLE_BITS_EXP - (u)) : (u) ) )
/* ************************************************************** */
/* MIN: MPI_FLOAT from tree */
/* For MIN of 32-bit float, use this routine to convert results leaving tree. */
/* ************************************************************** */
#define MIN_FLOAT_FRTREE(u) ( FLOAT_NAN((u)) ? (u) : \
                                ( !FLOAT_SIGN((u)) ? (FLOAT_BITS_EXP - (u)) : (u) ) )
/* ************************* */
/* MIN: MPI_DOUBLE from tree */
/* ************************* */
#define MIN_DOUBLE_FRTREE(u) ( DOUBLE_NAN((u)) ? (u) : \
                                 ( !DOUBLE_SIGN((u)) ? (DOUBLE_BITS_EXP - (u)) : (u) ) )

#define BGL_TR_BAD_OPERAND (0x0000800000000000ull)
#define BGL_TR_HUMUNGOUS   (0x0000000040000000ull)
#define PKTWORDS 32

#endif /* __core_CNUtil_impl_h__ */
