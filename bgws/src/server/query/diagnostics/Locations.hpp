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

#ifndef BGWS_QUERY_DIAGNOSTICS_LOCATIONS_HPP_
#define BGWS_QUERY_DIAGNOSTICS_LOCATIONS_HPP_


#include "LocationsOptions.hpp"

#include <db/include/api/cxxdb/cxxdb.h>

#include <stdint.h>


namespace bgws {
namespace query {
namespace diagnostics {


class Locations
{
public:

    Locations(
            const LocationsOptions& options
        );

    void execute(
            cxxdb::ConnectionPtr conn_ptr,
            uint64_t* all_count_out,
            cxxdb::ResultSetPtr* rs_ptr_out
        );


private:

    static const std::string HARDWARE_STATUS_STRING_TO_ORD_CLAUSE;
    static const std::string HARDWARE_STATUS_ORD_TO_STRING_CLAUSE;


    LocationsOptions _options;


    uint64_t _calcRowCount( cxxdb::ConnectionPtr conn_ptr );

    void _doQuery(
            cxxdb::ConnectionPtr conn_ptr,
            cxxdb::ResultSetPtr* rs_ptr_out
        );
};


} } } // namespace bgws::query::diagnostics


#endif
