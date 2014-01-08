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


#include "TlrmalertExecution.hpp"

#include "errors.hpp"

#include "../utility/ChildProcesses.hpp"

#include <utility/include/Log.h>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include <boost/throw_exception.hpp>

#include <boost/xpressive/xpressive.hpp>

#include <string>


LOG_DECLARE_FILE( "bgws" );


using namespace boost::xpressive;

using boost::lexical_cast;

using std::string;


namespace bgws {
namespace teal {


TlrmalertExecution::Ptr TlrmalertExecution::create(
        Id id,
        CloseRemoveCallbackFn cb_fn,
        const boost::filesystem::path& executable,
        utility::ChildProcesses& child_processes,
        boost::asio::io_service& io_service
    )
{
    return boost::make_shared<TlrmalertExecution>(
            id,
            cb_fn,
            boost::ref( executable ),
            boost::ref( child_processes ),
            boost::ref( io_service )
        );
}


void TlrmalertExecution::start()
{
    _process_ptr->start(
            _strand.wrap( boost::bind( &TlrmalertExecution::_handleProcessEnded, shared_from_this(), _1 ) ),
            _strand.wrap( boost::bind( &TlrmalertExecution::_handleLine, shared_from_this(), _1, _2, _3 ) )
        );
}


TlrmalertExecution::TlrmalertExecution(
        Id id,
        CloseRemoveCallbackFn cb_fn,
        const boost::filesystem::path& executable,
        utility::ChildProcesses& child_processes,
        boost::asio::io_service& io_service
    ) :
        _id(id),
        _cb_fn(cb_fn),
        _strand( io_service ),
        _process_ptr(child_processes.createEasy(
                "tlrmalert",
                executable,
                utility::ChildProcess::Args { "-i", lexical_cast<string>( id ) }
            )),
        _exited(false),
        _stdout_complete(false),
        _stderr_complete(false),
        _result(_Result::Ok)
{
    // Nothing to do.
}


void TlrmalertExecution::_handleProcessEnded(
        const bgq::utility::ExitStatus& exit_status
    )
{
    _exit_status = exit_status;
    _exited = true;

    _checkComplete();
}


void TlrmalertExecution::_handleLine(
        utility::EasyChildProcess::OutputType output_type,
        utility::EasyChildProcess::OutputIndicator output_ind,
        const std::string& line
    )
{
    if ( output_ind == utility::EasyChildProcess::OutputIndicator::End || output_ind == utility::EasyChildProcess::OutputIndicator::Error ) {
        if ( output_type == utility::EasyChildProcess::OutputType::Err ) {
            _stderr_complete = true;
        } else if ( output_type == utility::EasyChildProcess::OutputType::Out ) {
            _stdout_complete = true;
        }
        _checkComplete();
        return;
    }

    /* Output is like this:

-- not closed
$ /opt/teal/bin/tlrmalert -i 3060
Alert '3060' cannot be removed.
        Reason: Alert is not closed

0 unique alerts removed

-- doesn't exist.
$ /opt/teal/bin/tlrmalert -i 3100

0 unique alerts removed

  -- Note that the command exits with exit status 0 event when no alerts are removed.


# Here's what I get when duplicate.
$ /opt/teal/bin/tlrmalert -i 664272
Alert '664272' cannot be removed.
        Reason: Alert is not closed
        Reason: Alert is associated with Alert '661751'

0 unique alerts removed

$ echo $?
0

     */

    _output = _output + line + "\n";

    if ( _result != _Result::Ok ) {
        // Already know the error.
        return;
    }

    // Check for "Reason: Alert is not closed"

    if ( line.find( "Reason: Alert is not closed" ) != string::npos ) {
        LOG_DEBUG_MSG( "Found error msg alert is not closed." );
        _result = _Result::InvalidState;
        return;
    }

    // Check for "0 unique alerts removed"
    if ( line.find( "0 unique alerts removed") != string::npos ) {
        LOG_DEBUG_MSG( "Found msg no alert removed." );
        _result = _Result::NotFound;
        return;
    }
}


void TlrmalertExecution::_checkComplete()
{
    if ( ! (_exited && _stdout_complete && _stderr_complete) )  return;

    LOG_DEBUG_MSG( "Removing " << _id << " complete. exit_status=" << _exit_status << " output=" << _output );

    try {

        if ( _exit_status && _result == _Result::Ok ) {

            BOOST_THROW_EXCEPTION( std::runtime_error( _output ) );

        }

        if ( _result == _Result::Ok ) {

            _cb_fn( std::exception_ptr() );
            _cb_fn = CloseRemoveCallbackFn();

            return;
        }

        if ( _result == _Result::NotFound ) {

            BOOST_THROW_EXCEPTION( errors::NotFound( _output ) );

        }

        if ( _result == _Result::InvalidState ) {

            BOOST_THROW_EXCEPTION( errors::InvalidState( _output ) );

        }

        BOOST_THROW_EXCEPTION( std::runtime_error( _output ) );


    } catch ( std::exception& e ) {

        _cb_fn( std::current_exception() );
        _cb_fn = CloseRemoveCallbackFn();

    }
}


} } // namespace bgws::teal
