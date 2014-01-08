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

#ifndef _BGPM_ERRBKT_H_  // Prevent multiple inclusion
#define _BGPM_ERRBKT_H_

#include "bgpm/include/err.h"
#include "Debug.h"

namespace bgpm {



//! ErrBkt
/*!
  Class to collect last error and handle error messages

  Note: Needs to be a POD object, so has no private data, assignment and constructors.

  One instance per thread


*/

class ErrBkt
{
public:
    //ErrBkt() : lastErrNum(0) { lastErrStrg[0] = '\0'; };
    //~ErrBkt() {};

    // POD object - Init before 1st use
    void Init();

    int LastErrNum() { return lastErrNum; }
    operator int() { return lastErrNum; }
    const char *LastErrStrg();
    const char *ErrStrg(int err);


    //! SetTopLevelFunc
    /*!
     *  Set the top level function name to use in error messages. Unfortunately, each top level
     *  BGPM function call will need to set this so we can give the user a message which
     *  they can relate back to the operations of their own code.
     *  Only the passed pointer is captured, the contents are not copied.
     */
    void SetTopLevelFunc(const char *func) { lastTopLevel = func; }

    //! PrintOrExitOp
    /*!
     *  Set last error value and handle the print and exit of error indicated by the global
     *  exitOnError and printOnError flags.
     *
     *  If not supposed to exit, then return the passed error value.
     *  The usage model is:
     *     return PrintOrExitOp(lastErr, "system operation", BGPM_ERRLOC);
     *
     */
    ErrBkt & PrintOrExitOp(int errNum, const char *op, const char *loc);


    //! PrintOrExit
    /*!
     *  Set the last error value and handle the print and exit of Bgpm errors as indicated by the global
     *  exitOnError and printOnError flags.
     *
     *  If not supposed to exit, then return the passed error value.
     *  The usage model is:
     *     return PrintOrExit(lastErr, BGPM_ERRLOC);
     *  which will return the passed error value, or exit as appropriate.
     */
    ErrBkt & PrintOrExit(int errNum, const char *loc) { return PrintOrExitOp(errNum, NULL, loc); }


    //! PrintMsg
    /*!
     *  Print a generic message (mostly the cause of an error) using a variable format
     */
    ErrBkt & PrintMsg(const char* format, ...);


    //int operator=( int val ) { lastErrNum = val; return val; }
    ErrBkt & operator=( int val ) { lastErrNum = val; return *this; }
    friend bool operator==( ErrBkt &o, int val ) { return (o.lastErrNum == val); }

//private:
    int  lastErrNum;
    const char *lastTopLevel;


#define BGPM_MAXERRSTRG 512
#define BGPM_LOCSIZE 256
    char lastErrStrg[BGPM_MAXERRSTRG];
    char ucStrg[BGPM_LOCSIZE];


    // hide these
    //ErrBkt(const ErrBkt &);
    //ErrBkt & operator=(const ErrBkt &);
};

void GetErrStrg(int err, char *strg, size_t strgSize);




}



#endif
