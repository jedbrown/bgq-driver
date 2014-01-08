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

#ifndef BGWS_RESPONDER_ENVIRONMENTALS_FAN_HPP_
#define BGWS_RESPONDER_ENVIRONMENTALS_FAN_HPP_


#include "../../AbstractResponder.hpp"

#include "../../query/env/Query.hpp"

#include "capena-http/http/uri/Path.hpp"


namespace bgws {
namespace responder {
namespace environmentals {


class Fan : public AbstractResponder
{
public:

    static const capena::http::uri::Path RESOURCE_PATH;

    static bool matchesUrl( const capena::http::uri::Path& requested_resource )
    { return (requested_resource == RESOURCE_PATH); }


    Fan(
            CtorArgs& args
        ) :
            AbstractResponder( args ),
            _blocking_operations_thread_pool(args.blocking_operations_thread_pool)
    { /* Nothing to do */ }

    // override
    capena::http::Methods _getAllowedMethods() const  { return { capena::http::Method::GET }; }

    // override
    void _doGet();


    // override
    void notifyDisconnect();


private:

    BlockingOperationsThreadPool &_blocking_operations_thread_pool;

    query::env::Query::Ptr _query_ptr;


    void _queryComplete(
            capena::server::ResponderPtr,
            RequestRange req_range,
            query::env::Query::Result res
        );

};


} } } // namespace bgws::responder::environmentals

#endif
