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

#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include "Debug.h"
#include "globals.h"


//! \addtogroup bgpm_api_impl
//@{
/**
 * \file
 *
 * \brief BGPM key global variables implemenation.
 *
 */
//@}

using namespace bgpm;

/*
- User selects environment values, or via bgpm routines:
  - PRINT_ON_ERROR -- Print error message to stderr before returning error code.
                      Message include bgpm func name and original file error location.
  - EXIT_ON_ERROR -- Exit instead of returning.
  - DUMP_ON_ERROR -- Do an assert(0) dump to get the stack when an error occurs, this occurs
                     in the low level routine instead of top routine
*/
// Using reverse logic in global vars to allow initialization to 0 to start with.
extern "C" ushort Bgpm_PrintOnError(ushort val) {
    BGPM_EXEMPT_L2;
    bool tmp = GetPrintOnError();
    printOnError = val;
    return tmp;
}
extern "C" ushort Bgpm_ExitOnError(ushort val)  {
    BGPM_EXEMPT_L2;
    bool tmp = GetExitOnError();
    exitOnError = val;
    return tmp;
}
extern "C" ushort Bgpm_DumpOnError(ushort val)  {
    BGPM_EXEMPT_L2;
    bool tmp = GetDumpOnError();
    dumpStackOnError = val;
    return tmp;
}






