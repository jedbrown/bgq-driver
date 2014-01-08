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

#include "job/exception.h"

namespace BGQDB {
namespace job {
namespace exception {

//-------------------------------------------------------------------
// class BlockNotFound

BlockNotFound::BlockNotFound( const std::string& message )
    : std::runtime_error( message )
{
    // Nothing to do.
}

//-------------------------------------------------------------------
// class BlockNotInitialized

BlockNotInitialized::BlockNotInitialized( const std::string& message )
    : std::runtime_error( message )
{
    // Nothing to do.
}

//-------------------------------------------------------------------
// class BlockActionNotEmpty

BlockActionNotEmpty::BlockActionNotEmpty( const std::string& message )
    : std::runtime_error( message )
{
    // Nothing to do.
}

//-------------------------------------------------------------------
// class SubNodeJobUserConflict

SubNodeJobUserConflict::SubNodeJobUserConflict( const std::string& message )
    : std::runtime_error( message )
{
    // Nothing to do.
}

//-------------------------------------------------------------------
// class JobNotFound

JobNotFound::JobNotFound( const std::string& message )
    : std::runtime_error( message )
{
    // Nothing to do.
}

//-------------------------------------------------------------------
// class InvalidStatus

InvalidStatus::InvalidStatus( const std::string& message )
    : std::runtime_error( message )
{
    // Nothing to do.
}

} } } // namespace BGQDB::job::exception
