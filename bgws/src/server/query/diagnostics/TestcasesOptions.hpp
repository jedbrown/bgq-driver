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

#ifndef BGWS_QUERY_DIAGNOSTICS_TESTCASES_OPTIONS_HPP_
#define BGWS_QUERY_DIAGNOSTICS_TESTCASES_OPTIONS_HPP_


#include "../../StringDbColumnOption.hpp"
#include "../../DiagnosticsRunIdOption.hpp"

#include "capena-http/http/uri/Query.hpp"


namespace bgws {
namespace query {
namespace diagnostics {


struct TestcasesOptions
{
    StringDbColumnOption block_id;
    StringDbColumnOption hardware_status;
    StringDbColumnOption location;
    DiagnosticsRunIdOption run_id_option;
    StringDbColumnOption testcase;


    TestcasesOptions(
            const capena::http::uri::Query& query
        );

};

} } } // namespace bgws::query:: diagnostics

#endif
