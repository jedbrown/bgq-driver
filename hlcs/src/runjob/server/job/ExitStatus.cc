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
#include "server/job/ExitStatus.h"

#include "common/logging.h"

#include <iomanip>

namespace runjob {
namespace server {
namespace job {

LOG_DECLARE_FILE( runjob::server::log );

ExitStatus::ExitStatus() :
    _message(),
    _error( error_code::success ),
    _status( 0 ),
    _offender( 0 ),
    _controlActionRecordId( 0 )
{

}

void
ExitStatus::set(
        const std::string& message,
        const error_code::rc error
   )
{
    if ( _message.empty() ) {
        _message = message;
    } else if ( error != _error ) {
        // append error message for different errors
        LOG_INFO_MSG( "adding to error text: " << _message );
        _message += ". ";
        _message += message;
    }
    
    this->set( error );
}

void
ExitStatus::set(
        const error_code::rc error
        )
{
    // only set error if it's different
    if ( _error == error ) return;

    if ( error != error_code::runjob_server_shutdown ) {
        LOG_INFO_MSG( error_code::toString(error) );
    } else {
        LOG_DEBUG_MSG( error_code::toString(error) );
    }

    _error = error;
}

void
ExitStatus::set(
        const int status,
        const uint32_t rank
        )
{
    LOG_TRACE_MSG( "status 0x" << std::setw(8) << std::setfill('0') << std::hex << status << " from rank " << std::dec << rank );

    // short circuit if status is already set
    if ( _status.get() & 0xFFFF ) {
        LOG_TRACE_MSG( "exit status already set: 0x" << std::setw(8) << std::setfill('0') << std::hex << _status.get() );
        return;
    }

    _status = bgq::utility::ExitStatus( _status.get() | status );
    _offender = rank;

    if ( _status.exited() ) {
        _message.
            append( "normal termination with status ").
            append( boost::lexical_cast<std::string>(_status.getExitStatus()) ).
            append( " from rank " ).
            append( boost::lexical_cast<std::string>(_offender) )
            ;
    } else if ( _status.signaled() ) {
        _message.
            append( "abnormal termination by signal " ).
            append( boost::lexical_cast<std::string>(_status.getSignal()) ).
            append( " from rank " ).
            append( boost::lexical_cast<std::string>(_offender) )
            ;
        if ( _controlActionRecordId ) {
            _message.
                append( " due to RAS event with record ID " ).
                append( boost::lexical_cast<std::string>(_controlActionRecordId) )
                ;
        } else if ( !_details.empty() ) {
            _message.append( ". " );
            _message.append( _details );
        }
    } else {
        LOG_WARN_MSG( "not exited or signaled: 0x" << std::setw(8) << std::setfill('0') << std::hex << _status );
    }

    if ( _status.exited() && !_status.getExitStatus() ) {
        // normal termination with status 0 is normal, so log
        // this at debug severity
        LOG_DEBUG_MSG( _message );
    } else {
        // abnormal termination, or normal termination with a non-zero status
        LOG_INFO_MSG( _message );
    }
}

void
ExitStatus::setRecordId(
        const int recid
        )
{
    LOG_DEBUG_MSG( __FUNCTION__ << " " << recid );
    if ( _controlActionRecordId ) {
        LOG_DEBUG_MSG( "recid already set" );
        return;
    }

    _controlActionRecordId = recid;
}

void
ExitStatus::setDetails(
        const std::string& details
        )
{
    LOG_DEBUG_MSG( details );
    if ( !_details.empty() ) {
        LOG_DEBUG_MSG( "details already set: " << _details );
        return;
    }

    _details = details;
}

} // job
} // server
} // runjob
