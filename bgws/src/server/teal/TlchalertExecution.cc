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


#include "TlchalertExecution.hpp"

#include "errors.hpp"

#include "../utility/ChildProcesses.hpp"

#include <utility/include/Log.h>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>

#include <boost/xpressive/xpressive.hpp>

#include <string>


LOG_DECLARE_FILE( "bgws" );


using namespace boost::xpressive;

using boost::lexical_cast;

using std::string;


namespace bgws {
namespace teal {


TlchalertExecution::Ptr TlchalertExecution::create(
        Id id,
        CloseRemoveCallbackFn cb_fn,
        const boost::filesystem::path& executable,
        utility::ChildProcesses& child_processes,
        boost::asio::io_service& io_service
    )
{
    return boost::make_shared<TlchalertExecution>(
            id,
            cb_fn,
            boost::ref( executable ),
            boost::ref( child_processes ),
            boost::ref( io_service )
        );
}


void TlchalertExecution::start()
{
    _process_ptr->start(
            _strand.wrap( boost::bind( &TlchalertExecution::_handleProcessEnded, shared_from_this(), _1 ) ),
            _strand.wrap( boost::bind( &TlchalertExecution::_handleLine, shared_from_this(), _1, _2, _3 ) )
        );
}


TlchalertExecution::TlchalertExecution(
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
                "tlchalert",
                executable,
                utility::ChildProcess::Args { "-i", lexical_cast<string>( id ), "-s", "close" }
            )),
        _exited(false),
        _stdout_complete(false),
        _stderr_complete(false),
        _result(_Result::OtherError)
{
    // Nothing to do.
}


void TlchalertExecution::_handleProcessEnded(
        const bgq::utility::ExitStatus& exit_status
    )
{
    _exit_status = exit_status;
    _exited = true;

    _checkComplete();
}


void TlchalertExecution::_handleLine(
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
Cannot close alert. Reason: rc = 1: 'Alert with specified record id not found'

Cannot close alert. Reason: rc = 2: 'Current alert state does not allow this operation'

Cannot close alert. Reason: rc = 3: 'Operation not allowed on duplicate alert'
     */


    // Check for "rc = \d"
    static const sregex RC_REGEX = as_xpr( "rc" ) >> *_s >> "=" >> *_s >> (s1=+_d) >> ":";

    smatch what;

    if ( regex_search( line, what, RC_REGEX ) ) {
        string rc_str(what[1]);

        LOG_INFO_MSG( _process_ptr << " error rc=" << rc_str );

        int rc = lexical_cast<int>( rc_str );

        if ( rc == 1 ) {
            _result = _Result::NotFound;
        } else if ( rc == 2 ) {
            _result = _Result::InvalidState;
        } else if ( rc == 3 ) {
            _result = _Result::Duplicate;
        } else {
            _result = _Result::OtherError;
        }
    }

    _output = _output + line + "\n";
}


void TlchalertExecution::_checkComplete()
{
    if ( ! (_exited && _stdout_complete && _stderr_complete) )  return;

    try {
        LOG_DEBUG_MSG( "Closing " << _id << " complete. exit_status=" << _exit_status << " output=" << _output );

        if ( _exit_status ) {
            // an error occurred...

            if ( _result == _Result::NotFound ) {
                BOOST_THROW_EXCEPTION( errors::NotFound( _output ) );
            } else if ( _result == _Result::InvalidState ) {
                BOOST_THROW_EXCEPTION( errors::InvalidState( _output ) );
            } else if ( _result == _Result::Duplicate ) {
                BOOST_THROW_EXCEPTION( errors::Duplicate( _output ) );
            } else if ( _result == _Result::OtherError ) {
                BOOST_THROW_EXCEPTION( std::runtime_error( _output ) );
            }
        }

        _cb_fn( std::exception_ptr() );
        _cb_fn = CloseRemoveCallbackFn();

    } catch ( std::exception& e ) {

        _cb_fn( std::current_exception() );
        _cb_fn = CloseRemoveCallbackFn();

    }
}


} } // namespace bgws::teal
