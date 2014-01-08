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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

#ifndef RECORDINGRESPONDER_HPP_
#define RECORDINGRESPONDER_HPP_


#include "../../server/AbstractResponder.hpp"

#include "../../server/exception.hpp"
#include "../../server/Request.hpp"
#include "../../server/Response.hpp"

#include <boost/throw_exception.hpp>

#include <sstream>


// This responder records calls to processRequest().

class RecordingResponder : public capena::server::AbstractResponder
{
public:

    enum class ProcessOption {
            REQUEST_COMPLETE, // complete response when request complete.
            FIRST_CALL, // complete response on first call to _processRequest()
            THROW_COMPLETE, // throw when request complete.
            THROW_FIRST // throw on first call to _processRequest().
        };


    RecordingResponder(
            capena::server::RequestPtr request_ptr,
            ProcessOption process_option,
            std::ostringstream* processRequestLog_out
        ) :
            capena::server::AbstractResponder( request_ptr ),
            _process_option(process_option),
            _processRequestLog_out(processRequestLog_out)
    { /* Nothing to do */ }


    static capena::server::ResponderPtr create(
            capena::server::RequestPtr request_ptr,
            ProcessOption process_option,
            std::ostringstream* processRequestLog_out
        )
    {
        return capena::server::ResponderPtr( new RecordingResponder( request_ptr, process_option, processRequestLog_out ) );
    }


protected:

    // override
    void _processRequest()
    {
        *_processRequestLog_out << "PR " << (_getRequest().isComplete() ? "C" : "N") << "\n";

        if ( _process_option == ProcessOption::THROW_FIRST ) {
            BOOST_THROW_EXCEPTION( capena::server::exception::NotFound( "throwing not found." ) );
        }

        if ( _process_option == ProcessOption::THROW_COMPLETE ) {
            if ( ! _getRequest().isComplete() )  return;
            BOOST_THROW_EXCEPTION( capena::server::exception::NotFound( "throwing not found." ) );
        }

        if ( _process_option == ProcessOption::FIRST_CALL ) {
            capena::server::Response &response(_getResponse());
            response.setStatus( capena::http::Status::NoContent );
            response.headersComplete();
        }

        if ( _process_option == ProcessOption::REQUEST_COMPLETE ) {
            if ( ! _getRequest().isComplete() )  return;
            capena::server::Response &response(_getResponse());
            response.setStatus( capena::http::Status::NoContent );
            response.headersComplete();
        }

    }


private:

    ProcessOption _process_option;
    std::ostringstream *_processRequestLog_out;

};


#endif
