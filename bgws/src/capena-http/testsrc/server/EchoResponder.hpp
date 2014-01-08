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


#ifndef ECHORESPONDER_HPP_
#define ECHORESPONDER_HPP_


#include "../../server/AbstractResponder.hpp"

#include "../../http/http.hpp"

#include "../../server/Request.hpp"
#include "../../server/Response.hpp"


// This responder echoes whatever was sent in.

class EchoResponder : public capena::server::AbstractResponder
{
public:

    static const std::string get_message;


    EchoResponder(
            capena::server::RequestPtr request_ptr,
            capena::http::Method* method_out
        ) :
            capena::server::AbstractResponder( request_ptr ),
            _method_out(method_out)
    { /* Nothing to do */ }


    static capena::server::ResponderPtr create(
            capena::server::RequestPtr request_ptr,
            capena::http::Method* method_out
        )
    {
        return capena::server::ResponderPtr( new EchoResponder(
                request_ptr,
                method_out
            ) );
    }


protected:

    // override
    void _processRequest()
    {
        if ( ! _getRequest().isComplete() )  return;

        if ( _method_out )  *_method_out = _getRequest().getMethod();

        const capena::server::Request &request(_getRequest());
        capena::server::Response &response(_getResponse());


        if ( request.getUrl().getPath() == (capena::http::uri::Path() / "empty") )
        {
            response.setStatus( capena::http::Status::NoContent );
            response.headersComplete();
            return;
        }


        if ( request.getMethod() == capena::http::Method::POST ||
                request.getMethod() == capena::http::Method::PUT ) {
            response.setContentType( "application/data" );
            response.headersComplete();
            response.out() << *(request.getBodyOpt());
            return;
        }

        response.setContentType( "application/data" );
        response.headersComplete();
        response.out() << get_message;
    }


private:

    capena::http::Method *_method_out;
};


#endif /* ECHORESPONDER_HPP_ */
