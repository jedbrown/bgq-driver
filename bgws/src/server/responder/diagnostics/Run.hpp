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

#ifndef BGWS_RESPONDER_DIAGNOSTICS_RUN_HPP_
#define BGWS_RESPONDER_DIAGNOSTICS_RUN_HPP_


#include "../../AbstractResponder.hpp"

#include "../../blue_gene/diagnostics/Runs.hpp"

#include "capena-http/http/http.hpp"

#include "chiron-json/json.hpp"


namespace bgws {
namespace responder {
namespace diagnostics {


class Run : public AbstractResponder
{
public:

    static bool matchesUrl( const capena::http::uri::Path& requested_resource );

    static capena::http::uri::Path calcUri(
            const capena::http::uri::Path& path_base,
            blue_gene::diagnostics::RunId run_id
        );


    Run(
            CtorArgs& args
        ) :
            AbstractResponder( args ),
            _diagnostics_runs(args.diagnostics_runs)
    { /* Nothing to do */ }


    capena::http::Methods _getAllowedMethods() const
    { return { capena::http::Method::GET, capena::http::Method::POST }; }


    // override
    void _doGet();

    // override
    void _doPost( json::ConstValuePtr val_ptr );


private:

    blue_gene::diagnostics::Runs &_diagnostics_runs;


    void _gotRunSnapshot(
            capena::server::ResponderPtr /*shared_ptr*/,
            json::ObjectValuePtr obj_val_ptr,
            blue_gene::diagnostics::Runs::RunInfoPtr run_info_ptr
        );

    void _gotCancelResult(
            capena::server::ResponderPtr /*shared_ptr*/,
            blue_gene::diagnostics::RunId run_id,
            std::string operation,
            blue_gene::diagnostics::Runs::CancelResult::Value res
        );

};


} } } // namespace bgws::responder::diagnostics

#endif
