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

#include "../../types.hpp"

#include <db/include/api/cxxdb/cxxdb.h>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/asio.hpp>

#include <exception>

#include <stdint.h>


namespace bgws {
namespace query {
namespace env {


class Query
{
public:


    typedef boost::shared_ptr<Query> Ptr;


    Query(
            boost::shared_ptr<AbstractOptions> abstract_options_ptr
        );

    void execute(
            cxxdb::ConnectionPtr conn_ptr,
            uint64_t* row_count_out,
            cxxdb::ResultSetPtr* rs_ptr_out
        );


    struct Result
    {
        std::exception_ptr exc_ptr;
        uint64_t all_count;
        cxxdb::ConnectionPtr connection_ptr;
        cxxdb::ResultSetPtr rs_ptr;
    };

    typedef boost::function<void ( Result )> ExecuteCbFn;


    void executeAsync(
            BlockingOperationsThreadPool& blocking_operations_thread_pool,
            ExecuteCbFn cb_fn
        );

    void cancel();


private:

    boost::shared_ptr<AbstractOptions> _options_ptr;

    cxxdb::QueryStatementPtr _stmt_ptr;


    uint64_t _queryRowCount( cxxdb::ConnectionPtr conn_ptr );

    cxxdb::ResultSetPtr _queryRows( cxxdb::ConnectionPtr conn_ptr );


    void _executeAsyncImpl(
            ExecuteCbFn cb_fn
        );

};

} } } // namespace bgws::query::env


#endif
