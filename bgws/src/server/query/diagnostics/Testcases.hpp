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

#ifndef BGWS_QUERY_DIAGNOSTICS_TESTCASES_HPP_
#define BGWS_QUERY_DIAGNOSTICS_TESTCASES_HPP_


#include "TestcasesOptions.hpp"

#include <db/include/api/cxxdb/fwd.h>


namespace bgws {
namespace query {
namespace diagnostics {


class Testcases
{
public:

    void execute(
            const TestcasesOptions& options,
            cxxdb::ConnectionPtr conn_ptr,
            cxxdb::ResultSetPtr *rs_ptr_out
        );

};


} } } // namespace bgws::query:: diagnostics

#endif
