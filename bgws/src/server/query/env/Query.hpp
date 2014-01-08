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

#ifndef BGWS_QUERY_ENV_QUERY_HPP_
#define BGWS_QUERY_ENV_QUERY_HPP_


#include "AbstractOptions.hpp"

#include <db/include/api/cxxdb/cxxdb.h>

#include <boost/shared_ptr.hpp>

#include <stdint.h>


namespace bgws {
namespace query {
namespace env {


class Query
{
public:

    Query(
            boost::shared_ptr<AbstractOptions> abstract_options_ptr
        );

    void execute(
            cxxdb::ConnectionPtr conn_ptr,
            uint64_t* row_count_out,
            cxxdb::ResultSetPtr* rs_ptr_out
        );

private:

    boost::shared_ptr<AbstractOptions> _options_ptr;


    uint64_t _queryRowCount( cxxdb::ConnectionPtr conn_ptr );

    cxxdb::ResultSetPtr _queryRows( cxxdb::ConnectionPtr conn_ptr );

};

} } } // namespace bgws::query::env


#endif
