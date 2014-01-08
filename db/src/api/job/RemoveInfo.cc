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

#include "job/RemoveInfo.h"

namespace BGQDB {
namespace job {

RemoveInfo::RemoveInfo( int exit_status )
    : _terminated(true),
      _exit_status(exit_status)
{
    // Nothing to do.
}

RemoveInfo::RemoveInfo( const std::string& error_message ) :
    _terminated(false),
    _message(error_message)
{
    // Nothing to do
}

RemoveInfo::RemoveInfo(
        int exit_status,
        const std::string& error_message
        ) :
    _terminated(true),
    _exit_status(exit_status),
    _message(error_message)
{
    // Nothing to do
}

} } // namespace BGQDB::job
