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
/*!
 * \file utility/include/cxxsockets/exception.h
 */

#ifndef CXXSOCKET_EXCEPTION_H
#define CXXSOCKET_EXCEPTION_H

#include <stdexcept>

namespace CxxSockets {

//! Base CxxSockets exception class inheriting from runtime_error
class Error : public std::runtime_error
{
public:
    int errcode;
    Error(int err=0, const std::string what="") : std::runtime_error(what), errcode(err) {}
};

//! \brief Socket error exception class for networking/socket api errors.
//!
//! Socket will no longer be usable when we throw this.
class HardError : public Error
{
public:
    HardError(int err=0, const std::string what="") : Error(err, what) {}
};

//! \brief Soft error exception
//!
//! Socket error exception class for networking/socket api errors that are
//! non-fatal.  EAGAIN, EWOULDBLOCK, EINTR.
class SoftError : public Error
{
public:
    SoftError(int err=0, const std::string what="") : Error(err, what) {}
};

//! \brief Socket unexpected closure
//!
//! Socket closed unexpectedly during atomic send/receive operation
class CloseUnexpected : public Error
{
public:
    CloseUnexpected(int err=0, const std::string what="") : Error(err, what) {}
};

//! \brief Exception for problems indicating bugs internal to CxxSockets
class InternalError : public Error
{
public:
    InternalError(int err=0, const std::string what="") : Error(err,what) {}
};

//! \brief User error exception
//!
//! Exception for problems indicating CxxSockets usage errors like bad or
//! mismatched parameters or concurrency issues.
class UserError : public Error
{
public:
    UserError(int err=0, const std::string what="") : Error(err,what) {}
};

}

#endif
