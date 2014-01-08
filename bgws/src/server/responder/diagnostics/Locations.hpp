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

#ifndef BGWS_RESPONDER_DIAGNOSTICS_LOCATIONS_HPP_
#define BGWS_RESPONDER_DIAGNOSTICS_LOCATIONS_HPP_


#include "../../AbstractResponder.hpp"
#include "../../RequestRange.hpp"

#include "../../query/diagnostics/LocationsOptions.hpp"

#include "capena-http/http/http.hpp"


namespace bgws {
namespace responder {
namespace diagnostics {


class Locations : public AbstractResponder
{
public:

    static const capena::http::uri::Path RESOURCE_PATH;
    static const capena::http::uri::Path RESOURCE_PATH_EMPTY_CHILD;

    static bool matchesUrl(
            const capena::http::uri::Path& requested_resource
        )
    {
        return (requested_resource == RESOURCE_PATH || requested_resource == RESOURCE_PATH_EMPTY_CHILD);
    }


    Locations(
            CtorArgs& args
        ) :
            AbstractResponder( args ),
            _blocking_operations_thread_pool(args.blocking_operations_thread_pool)
    { /* Nothing to do */ }


    capena::http::Methods _getAllowedMethods() const
    {
        static const capena::http::Methods methods = { capena::http::Method::GET };
        return methods;
    }


    // override
    void _doGet();


private:

    BlockingOperationsThreadPool &_blocking_operations_thread_pool;


    void _startQuery(
            capena::server::ResponderPtr,
            const query::diagnostics::LocationsOptions& options,
            const RequestRange& req_range
        );

    void _queryComplete(
            capena::server::ResponderPtr,
            uint64_t all_count,
            cxxdb::ConnectionPtr,
            cxxdb::ResultSetPtr rs_ptr,
            const RequestRange& req_range
        );
};

} } } // namespace bgws::responder::diagnostics


#endif
