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

#ifndef BGWS_RESPONDER_ALERTS_HPP_
#define BGWS_RESPONDER_ALERTS_HPP_


#include "../AbstractResponder.hpp"

#include "../query/Alerts.hpp"

#include <boost/shared_ptr.hpp>

#include <stdint.h>


namespace bgws {
namespace responder {


class Alerts : public AbstractResponder
{
public:


    static const capena::http::uri::Path RESOURCE_PATH;


    static bool matchesUrl(
            const capena::http::uri::Path& requested_resource
        )
    {
        static const capena::http::uri::Path RESOURCE_PATH_EMPTY_CHILD(RESOURCE_PATH / "");
        return (requested_resource == RESOURCE_PATH || requested_resource == RESOURCE_PATH_EMPTY_CHILD);
    }


    Alerts(
            CtorArgs& args
        ) :
            AbstractResponder( args ),
            _blocking_operations_thread_pool(args.blocking_operations_thread_pool)
    { /* Nothing to do */ }


    capena::http::Methods _getAllowedMethods() const
    {
        static const capena::http::Methods METHODS = { capena::http::Method::GET };
        return METHODS;
    }


    void _doGet();


private:

    BlockingOperationsThreadPool &_blocking_operations_thread_pool;


    void _checkAuthority();


    void _queryComplete(
            std::exception_ptr exc_ptr,
            json::ArrayValuePtr arr_val_ptr,
            uint64_t total_count,
            RequestRange req_range,
            capena::server::ResponderPtr,
            boost::shared_ptr<query::Alerts>
        );
};


}} // namespace bgws::responder


#endif
