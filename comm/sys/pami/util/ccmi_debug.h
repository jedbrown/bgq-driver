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
 * \file util/ccmi_debug.h
 * \brief common debug macros
 */

#ifndef __util_ccmi_debug_h__
#define __util_ccmi_debug_h__

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

//#define CCMI_TRACE_ALL 1

#ifdef CCMI_TRACE_ALL
  #define TRACE_MSG(x) fprintf x
#endif
#ifndef TRACE_MSG
  #define TRACE_MSG(x)
#endif

#ifdef CCMI_TRACE_ALL
  #define CCMI_DEBUG 1
#endif

#ifdef CCMI_DEBUG

//  #warning CCMI DEBUG enabled

  #undef TRACE_MSG
  #define TRACE_MSG(x) fprintf x

  #undef TRACE_BUF
  #define TRACE_BUF(x) fprintf x
  #ifndef TRACE_BUF
    #define TRACE_BUF(x)
  #endif

  #define TRACE_DATA(x) CCMI_ADAPTOR_DEBUG_trace_data x
// #define TRACE_DATA(x) if((PAMI_SINGLE_COMPLEX == _dt) && (PAMI_SUM == _op)) CCMI_ADAPTOR_DEBUG_trace_data x
inline void CCMI_ADAPTOR_DEBUG_trace_data(const char* string, const char* buffer,unsigned size)
{
  unsigned nChunks = size / 32;
  TRACE_MSG((stderr, "<%p> %s, length=%#X\n", buffer, string, size));
  if(!buffer) return;
  for(unsigned i = 0; i < nChunks; i++)
  {
    TRACE_BUF((stderr,
               "<%p>: %8.8X %8.8X %8.8X %8.8X %8.8X %8.8X %8.8X %8.8X \n",
               buffer+(i*32),
               *(int*)(buffer+(i*32)+0),
               *(int*)(buffer+(i*32)+4),
               *(int*)(buffer+(i*32)+8),
               *(int*)(buffer+(i*32)+12),
               *(int*)(buffer+(i*32)+16),
               *(int*)(buffer+(i*32)+20),
               *(int*)(buffer+(i*32)+24),
               *(int*)(buffer+(i*32)+28)
              ));
  }
  if(size % 32)
  {
    unsigned lastChunk = nChunks * 32;
    TRACE_BUF((stderr,
               "<%p>: %8.8X %8.8X %8.8X %8.8X %8.8X %8.8X %8.8X %8.8X \n",
               buffer+lastChunk,
               lastChunk+0<size?*(int*)(buffer+lastChunk+0):0xDEADDEAD,
               lastChunk+4<size?*(int*)(buffer+lastChunk+4):0xDEADDEAD,
               lastChunk+8<size?*(int*)(buffer+lastChunk+8):0xDEADDEAD,
               lastChunk+12<size?*(int*)(buffer+lastChunk+12):0xDEADDEAD,
               lastChunk+16<size?*(int*)(buffer+lastChunk+16):0xDEADDEAD,
               lastChunk+20<size?*(int*)(buffer+lastChunk+20):0xDEADDEAD,
               lastChunk+24<size?*(int*)(buffer+lastChunk+24):0xDEADDEAD,
               lastChunk+28<size?*(int*)(buffer+lastChunk+28):0xDEADDEAD
              ));
    lastChunk = 0; // gets rid of an annoying warning when not tracing the buffer
  }
}

#else // CCMI_DEBUG not defined
  #define TRACE_DATA(x)
  #ifndef TRACE_BUF
    #define TRACE_BUF(x)
  #endif
#endif

#ifdef CCMI_TRACE_ALL
  #define TRACE_FLOW(x) fprintf x
#endif
#ifndef TRACE_FLOW
  #define TRACE_FLOW(x)
#endif

#ifdef CCMI_TRACE_ALL
  #define TRACE_INIT(x) fprintf x
#endif
#ifndef TRACE_INIT
  #define TRACE_INIT(x)
#endif

#ifdef CCMI_TRACE_ALL
  #define TRACE_ADVANCE(x) fprintf x
#endif
#ifndef TRACE_ADVANCE
  #define TRACE_ADVANCE(x)
#endif

#ifdef CCMI_TRACE_ALL
  #define TRACE_REDUCEOP(x) fprintf x
#endif
#ifndef TRACE_REDUCEOP
  #define TRACE_REDUCEOP(x)
#endif

#ifdef CCMI_TRACE_ALL
  #define CCMI_DEBUG_SCHEDULE
  #define TRACE_STATE(x) fprintf x
#endif
#ifndef TRACE_STATE
  #define TRACE_STATE(x)
#endif

#ifdef CCMI_TRACE_ALL
  #define TRACE_SCHEDULE(x) fprintf x
#endif
#ifndef TRACE_SCHEDULE
  #define TRACE_SCHEDULE(x)
#endif

// TRACE_ALERT helps (minimally) trace object ctor/resets that help determine whether
// they're being reused appropriately.
#ifdef CCMI_TRACE_ALL
  #define TRACE_ALERT(x) fprintf x
#endif
#ifndef TRACE_ALERT
  #define TRACE_ALERT(x)
#endif

#ifdef CCMI_TRACE_ALL
  #define TRACE_ADAPTOR(x) fprintf x
#endif
#ifndef TRACE_ADAPTOR
  #define TRACE_ADAPTOR(x)
#endif



static inline void CCMI_FATALERROR (int errcode, const char * strg, ...)
{
    char buffer[120];
    va_list ap;
    va_start(ap, strg);
    vsnprintf (buffer, 119, strg, ap);
    va_end(ap);
    if (errcode==0) errcode=-1;
    fprintf(stderr, "Collective Runtime Error %d: %s\n",errcode,buffer);

    // char * z = 0;
    // *z = 0xFF;
    exit(1);
}



#endif //__adaptor_ccmi_debug_h__
