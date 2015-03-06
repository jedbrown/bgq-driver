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

/// \file test/internals/atomics/compilation.h
/// \brief Basic comilation helper macros
///

#ifndef __test_internals_atomics_compilation_h__
#define __test_internals_atomics_compilation_h__

#include <stdio.h>
#include "util/common.h"
#include "components/memory/MemoryManager.h"

#define COUNTER_HELPER(class, var, mm, key)\
        class var;			\
        var.init(mm, key);		\
        var.fetch_and_inc();		\
        var.fetch_and_dec();		\
        var.fetch_and_clear();

#define MUTEX_HELPER(class, var, mm, key)\
        class var;			\
        var.init(mm, key);		\
        var.acquire();			\
        var.tryAcquire();		\
        var.release();			\
        var.isLocked();

#define COUNTER_HELPER2(class, var)	\
        class var;			\
        var.fetch_and_inc();		\
        var.fetch_and_dec();		\
        var.fetch_and_clear();

#define MUTEX_HELPER2(class, var)	\
        class var;			\
        var.acquire();			\
        var.tryAcquire();		\
        var.release();			\
        var.isLocked();

#define BARRIER_HELPER(class, var, mm, key, partic, master)	\
        class var(partic, master);					\
        var.init(mm, key);		\
        var.enter();

#define BARRIER_HELPER2(class, var, mm, key, partic, master)	\
        class var(partic, master);					\
        var.enter();

#endif // __test_atomics_compilation_h__
