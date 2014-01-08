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

#ifndef US_DEBUG_H
#define US_DEBUG_H


#include "spi/include/upci/testlib/upci_debug.h"


// Initialize prefix and indent string for messages.
// In fwext, don't add a prefix,
// but in CNK, add a trace id string and hardware thread location
#ifdef __FWEXT__
#define INIT_INDENT() \
    char indentStrg[128];  \
    memset(indentStrg, ' ', indent*2); \
    indentStrg[indent*2] = '\0';
#else
#define INIT_INDENT() \
    char indentStrg[156];  \
    int ipos = sprintf(indentStrg, "UPCI_TRACE (%02d) ", Kernel_ProcessorID()); \
    memset(&indentStrg[ipos], ' ', indent*2); \
    indentStrg[ipos+indent*2] = '\0';
#endif


#endif
