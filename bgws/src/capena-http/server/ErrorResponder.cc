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


#include "ErrorResponder.hpp"

#include "Response.hpp"


namespace capena {
namespace server {


ResponderPtr ErrorResponder::create(
        RequestPtr request_ptr,
        std::exception_ptr exception_ptr
    )
{
    return ResponderPtr( new ErrorResponder( request_ptr, exception_ptr ) );
        // Tried boost::make_shared but didn't work because constructor is private.
}


ErrorResponder::ErrorResponder(
        RequestPtr request_ptr,
        std::exception_ptr exception_ptr
    ) :
        AbstractResponder( request_ptr ),
        _exception_ptr(exception_ptr)
{
    // Nothing to do.
}


void ErrorResponder::_processRequest()
{
    auto &response(_getResponse());

    try {
        std::rethrow_exception( _exception_ptr );
    } catch ( std::exception& e ) {
        response.setException( e );
    }
}


} } // namespace capena::server
