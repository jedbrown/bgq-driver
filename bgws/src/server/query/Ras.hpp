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

#ifndef BGWS_QUERY_RAS_HPP_
#define BGWS_QUERY_RAS_HPP_


#include "RasOptions.hpp"

#include <db/include/api/cxxdb/cxxdb.h>

#include <stdint.h>


namespace bgws {
namespace query {


class Ras
{
public:

    Ras( const RasOptions& options );

    void execute(
            cxxdb::ConnectionPtr conn_ptr,
            uint64_t* row_count_out,
            cxxdb::ResultSetPtr* rs_ptr_out
        );

private:

    RasOptions _options;


    uint64_t _queryRowCount(
            const std::string& where_clause_sql,
            const cxxdb::ParameterNames& parameter_names,
            cxxdb::ConnectionPtr conn_ptr
        );
};

}} // namespace bgws::query

#endif
