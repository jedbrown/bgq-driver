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
 * \file test/internals/BG/bgq/commthreads/misc-util.h
 * \brief ???
 */

#ifndef __test_internals_BG_bgq_commthreads_misc_util_h__
#define __test_internals_BG_bgq_commthreads_misc_util_h__

#include <sys/pami.h>

#ifdef __cplusplus
extern "C" {
#endif

pami_task_t TEST_Global_index2task(size_t x);

size_t TEST_Global_size();

size_t TEST_Global_myindex();

pami_task_t TEST_Local_index2task(size_t x);

size_t TEST_Local_size();

size_t TEST_Local_myindex();

#ifdef __cplusplus
}
#endif

#endif // __test_internals_BG_bgq_commthreads_misc_util_h__
