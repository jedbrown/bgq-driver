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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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
#ifndef RASLOG_H_
#define RASLOG_H_


// Log definition for RAS .
// For Linux: use log facility in bgp/utility.
// For others: use cout

#ifdef __linux
#include <utility/include/Log.h>
#else
#define LOG_PRINT(x) 
// #define LOG_PRINT(x) cout << x << endl
#define LOG_DEBUG_MSG(x) LOG_PRINT(x) 
#define LOG_TRACE_MSG(x) LOG_PRINT(x)
#define LOG_INFO_MSG(x)  LOG_PRINT(x)
#define LOG_WARN_MSG(x)  LOG_PRINT(x)
#define LOG_ERROR_MSG(x) LOG_PRINT(x)
#define LOG_FATAL_MSG(x) LOG_PRINT(x)

#endif


#endif /*RASLOG_H_*/
