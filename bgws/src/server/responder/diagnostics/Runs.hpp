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

#ifndef BGWS_RESPONDER_DIAGNOSTICS_RUNS_HPP_
#define BGWS_RESPONDER_DIAGNOSTICS_RUNS_HPP_


#include "../../AbstractResponder.hpp"

#include "../../blue_gene/diagnostics/Runs.hpp"

#include "capena-http/http/http.hpp"


namespace bgws {
namespace responder {
namespace diagnostics {


class RunsQueryOptions;


class Runs : public AbstractResponder
{
public:

    static const capena::http::uri::Path RESOURCE_PATH;
    static const capena::http::uri::Path RESOURCE_PATH_EMPTY_CHILD;


    static bool matchesUrl( const capena::http::uri::Path& requested_resource );


    Runs(
            CtorArgs& args
        ) :
            AbstractResponder( args ),
            _blocking_operations_thread_pool(args.blocking_operations_thread_pool),
            _diagnostics_runs(args.diagnostics_runs)
    { /* Nothing to do */ }


    capena::http::Methods _getAllowedMethods() const
    {
        static const capena::http::Methods methods = { capena::http::Method::GET, capena::http::Method::POST };
        return methods;
    }

    // override
    void _doGet();

    // override
    void _doPost( json::ConstValuePtr val_ptr );


private:

    BlockingOperationsThreadPool &_blocking_operations_thread_pool;
    blue_gene::diagnostics::Runs &_diagnostics_runs;


    void _gotSnapshot(
            capena::server::ResponderPtr responder_ref_ptr,
            blue_gene::diagnostics::Runs::SnapshotPtr snapshot_ptr,
            const RunsQueryOptions& query_options
        );

    void _startQuery(
            capena::server::ResponderPtr,
            blue_gene::diagnostics::Runs::SnapshotPtr snapshot_ptr,
            const RunsQueryOptions& query_options
        );

    void _queryComplete(
            capena::server::ResponderPtr,
            json::ArrayValuePtr arr_val_ptr
        );


    void _handleGotNewDiagnosticsRunId(
    		capena::server::ResponderPtr shared_ptr,
    		blue_gene::diagnostics::RunId run_id,
    		const std::string& error_msg
		);

};


} } } // namespace bgws::responder::diagnostics


#endif
