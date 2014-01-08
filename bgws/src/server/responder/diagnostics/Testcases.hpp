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

#ifndef BGWS_RESPONDER_DIAGNOSTICS_TESTCASES_HPP_
#define BGWS_RESPONDER_DIAGNOSTICS_TESTCASES_HPP_


#include "../../AbstractResponder.hpp"

#include "../../query/diagnostics/Testcases.hpp"

#include "capena-http/http/http.hpp"


namespace bgws {
namespace responder {
namespace diagnostics {


class Testcases : public AbstractResponder
{
public:

    static const capena::http::uri::Path RESOURCE_PATH;
    static const capena::http::uri::Path RESOURCE_PATH_EMPTY_CHILD;


    static bool matchesUrl(
            const capena::http::uri::Path& requested_resource
        )
    { return (requested_resource == RESOURCE_PATH || requested_resource == RESOURCE_PATH_EMPTY_CHILD); }


    Testcases(
            CtorArgs& args
        ) :
            AbstractResponder( args ),
            _blocking_operations_thread_pool(args.blocking_operations_thread_pool)
    { /* Nothing to do */ }

    capena::http::Methods _getAllowedMethods() const  { return { capena::http::Method::GET }; }

    // override
    void _doGet();


private :

    BlockingOperationsThreadPool &_blocking_operations_thread_pool;


    void _startQuery(
            capena::server::ResponderPtr,
            const query::diagnostics::TestcasesOptions& query_options
        );

    void _queryComplete(
            capena::server::ResponderPtr,
            cxxdb::ConnectionPtr,
            cxxdb::ResultSetPtr rs_ptr
        );
};

} } } // namespace bgws::responder::diagnostics

#endif
