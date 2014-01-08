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

#ifndef CAPENA_SERVER_ERROR_RESPONDER_HPP_
#define CAPENA_SERVER_ERROR_RESPONDER_HPP_


#include "AbstractResponder.hpp"

#include "../http/http.hpp"

#include <exception>


namespace capena {
namespace server {


/** \brief Responder creates an HTTP response for an exception.
 *
 *  This is used internally when, for example, can't get a responder from the application.
 *
 *  For example, if the client requests a resource that doesn't exist, the application
 *  might throw a capena::server::exception::NotFound exception.
 *  To generate the response for the request, the Connection creates an ErrorResponder.
 *
 */
class ErrorResponder : public AbstractResponder
{
public:

    static ResponderPtr create(
            RequestPtr request_ptr,
            std::exception_ptr exception_ptr
        );

protected:

    // override.
    void _processRequest();


private:

    ErrorResponder(
            RequestPtr request_ptr,
            std::exception_ptr exception_ptr
        );


    std::exception_ptr _exception_ptr;

};


} } // namespace capena::server

#endif
