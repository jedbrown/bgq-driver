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


#ifndef SIMPLE_RESPONDER_HPP_
#define SIMPLE_RESPONDER_HPP_


#include "../../server/AbstractResponder.hpp"
#include "../../server/Request.hpp"
#include "../../server/Response.hpp"

#include <utility/include/portConfiguration/types.h>

#include <iostream>
#include <string>


// This responder just returns a message.

class SimpleResponder : public capena::server::AbstractResponder
{
public:
    SimpleResponder(
            capena::server::RequestPtr request_ptr,
            const std::string& message
        ) :
            capena::server::AbstractResponder( request_ptr ),
            _message(message)
    { /* Nothing to do */ }

    static capena::server::ResponderPtr create(
            capena::server::RequestPtr request_ptr,
            const std::string& message
        )
    {
        return capena::server::ResponderPtr( new SimpleResponder(
                request_ptr,
                message
            ) );
    }

    static capena::server::ResponderPtr createDefault(
            capena::server::RequestPtr request_ptr
        )
    {
        return create(
                request_ptr,
                "default message"
            );
    }

    static capena::server::ResponderPtr createSaveConnType(
            capena::server::RequestPtr request_ptr,
            bgq::utility::portConfig::UserType::Value conn_user_type,
            bgq::utility::portConfig::UserType::Value* conn_user_type_out
        )
    {
        *conn_user_type_out = conn_user_type;

        return createDefault( request_ptr );
    }


protected:

    // override
    void _processRequest()
    {
        if ( ! _getRequest().isComplete() )  return;

        capena::server::Response &response(_getResponse());

        response.setContentType( "application/data" );
        response.headersComplete();

        std::cout << "SimpleResponder: sending " << _message.size() << " bytes.\n";

        response.out() << _message;
    }


private:

    const std::string _message;
};


#endif
