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


#ifndef BGWS_QUERY_ALERTS_HPP_
#define BGWS_QUERY_ALERTS_HPP_


#include "AlertsOptions.hpp"

#include "../types.hpp"

#include "chiron-json/fwd.hpp"

#include <boost/function.hpp>

#include <exception>
#include <set>
#include <string>

#include <stdint.h>


namespace bgws {
namespace query {


class Alerts
{
public:


    typedef boost::function<void (
            std::exception_ptr exc_ptr,
            json::ArrayValuePtr arr_val_ptr,
            uint64_t total_count
        )> ResultFn;


    Alerts(
            BlockingOperationsThreadPool& blocking_operations_thread_pool,
            const capena::http::uri::Path& path_base
        ) :
            _blocking_operations_thread_pool(blocking_operations_thread_pool),
            _path_base(path_base)
    { /* Nothing to do */ }


    void executeAsync(
            const AlertsOptions& options,
            ResultFn result_fn
        );


private:

    BlockingOperationsThreadPool &_blocking_operations_thread_pool;
    const capena::http::uri::Path &_path_base;


    void _doQuery(
            const AlertsOptions& options,
            ResultFn result_fn
        );

};


} } // namespace bgws::query

#endif
