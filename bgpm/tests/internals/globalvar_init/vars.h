/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

#ifndef VARS_H
#define VARS_H

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#ifndef _LINUX_
#include "spi/include/kernel/location.h"
#else
#include <sys/syscall.h>
    #define gettid() syscall(207, 0) // not defined by glibc
    inline int Kernel_ProcessorID() { return gettid(); }
#endif


class ProcVars {
public:
    int var1;
    int var2;
    const char *strg1;
    ProcVars(): var1(5), var2(10), strg1("process variables") {
        fprintf(stderr, "%02d: ProcVars has been constructed with values: var1=%d, var2=%d, strg1=\"%s\"\n", Kernel_ProcessorID(), var1, var2, strg1);
    }
    void Print();
};


class ThreadVars {
public:
    int threadid;
    const char *threadstrg;
    ThreadVars() : threadid(-1), threadstrg("thread vars") {
        threadid = Kernel_ProcessorID();
        fprintf(stderr, "ThreadVars has been constructed with threadid=%d, threadstrg=\"%s\"\n", threadid, threadstrg);
    }
    void Print();
};


#ifdef _VARS_C
ProcVars prc;
// __thread ThreadVars thd;   Can't have thread local non-scalar variables
int prcScalar = -5;
__thread int thdScalar = -10;
#else
extern ProcVars prc;
// extern __thread ThreadVars thd;
extern int prcScalar;
extern __thread int thdScalar;
#endif


extern "C" void PrintVarState();


#endif
