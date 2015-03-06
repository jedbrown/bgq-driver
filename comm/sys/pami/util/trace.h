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
///
/// \file util/trace.h
/// \brief Yet another attempt at a trace/debug facility
///
#ifndef __util_trace_h__
#define __util_trace_h__

#include "Platform.h"

static size_t __trace_fn_counter = 0;
static char   __trace_indent_string[64] = { "                                                               " }; // 63 ' ' and 1 NULL

#define TRACE_FN_ENTER() \
  if (DO_TRACE_ENTEREXIT) \
    fprintf (stderr, "%u %s> %s() [%s:%d]\n",PLATFORM_TID, &__trace_indent_string[63-(((++__trace_fn_counter)<<1)-2)], __FUNCTION__, __FILE__, __LINE__);

#define TRACE_FN_EXIT() \
  if (DO_TRACE_ENTEREXIT) \
    fprintf (stderr, "%u %s< %s() [%s:%d]\n",PLATFORM_TID, &__trace_indent_string[63-(((__trace_fn_counter--)<<1)-2)], __FUNCTION__, __FILE__, __LINE__);

#define TRACE_STRING(str) \
  if (DO_TRACE_DEBUG) \
    fprintf (stderr, "%u %s  %s():%d .. " str "\n",PLATFORM_TID, &__trace_indent_string[63-((__trace_fn_counter<<1)-2)], __FUNCTION__, __LINE__);

#define TRACE_FORMAT(fmt, ...) \
  if (DO_TRACE_DEBUG) \
    {\
    if (!DO_TRACE_ENTEREXIT&&!__trace_fn_counter) __trace_fn_counter=1; \
    fprintf (stderr, "%u %s  %s():%d .. " fmt "\n",PLATFORM_TID, &__trace_indent_string[63-((__trace_fn_counter<<1)-2)], __FUNCTION__, __LINE__, __VA_ARGS__);\
    }

#define TRACE_HEXDATA(ptr,n) \
  if (DO_TRACE_DEBUG) { \
    size_t byte; char bigstring[1000];\
    size_t index = sprintf(&bigstring[0], "%u %s  %s():%d [%p]",PLATFORM_TID,&__trace_indent_string[63-((__trace_fn_counter<<1)-2)], __FUNCTION__, __LINE__, ptr); \
    size_t indent = index; \
    for(byte=0; byte<n; byte++) { \
      if(byte%32==0) { fprintf(stderr,"%s\n",bigstring); index = sprintf(&bigstring[0], "%s", &__trace_indent_string[63-indent]); } \
      if(byte%8==0) index += sprintf(&bigstring[index], " "); \
      if(byte%4==0) index += sprintf(&bigstring[index], " "); \
      index += sprintf(&bigstring[index], "%02x", ((char *)ptr)[byte]); \
    } \
    fprintf(stderr, "%s\n", &bigstring[0]); \
  }





#endif // __util_trace_h__
