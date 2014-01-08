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

#ifndef BGWS_RESPONDER_SUMMARY_SYSTEM_HPP_
#define BGWS_RESPONDER_SUMMARY_SYSTEM_HPP_


#include "../../AbstractResponder.hpp"

#include "capena-http/http/uri/Path.hpp"

#include "chiron-json/fwd.hpp"

#include <exception>


namespace bgws {
namespace responder {
namespace summary {


class System : public AbstractResponder
{
public:

    static bool matchesUrl(
            const capena::http::uri::Path& requested_resource
        );


    System(
            CtorArgs& args
        ) :
            AbstractResponder( args ),
            _blocking_operations_thread_pool(args.blocking_operations_thread_pool)
    { /* Nothing to do */ }


    capena::http::Methods _getAllowedMethods() const;

    void _doGet();


private:

    BlockingOperationsThreadPool &_blocking_operations_thread_pool;


    void _queryComplete(
            capena::server::ResponderPtr,
            std::exception_ptr exc_ptr,
            json::ObjectValuePtr val_ptr
        );

};


} } } // namespace bgws::responder::summary


#endif
