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

#define _VARS_C
#include "vars.h"



void ProcVars::Print()
{
    fprintf(stderr, "%02d: ProcVars::Print values: var1=%d, var2=%d, strg1=\"%s\"\n", Kernel_ProcessorID(), var1, var2, strg1);
}


void ThreadVars::Print()
{
    fprintf(stderr, "%02d: ThreadVars::Print threadid=%d, threadstrg=\"%s\"\n", Kernel_ProcessorID(), threadid, threadstrg);
}



void PrintVarState()
{
    prc.Print();
    //thd.Print();   Can't have thread local non-scalar variables
    fprintf(stderr, "%02d: prcScalar=%d\n", Kernel_ProcessorID(), prcScalar);
    fprintf(stderr, "%02d: thdScalar=%d\n", Kernel_ProcessorID(), thdScalar);
}

