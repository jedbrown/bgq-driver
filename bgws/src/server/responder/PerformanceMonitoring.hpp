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
/* (C) Copyright IBM Corp.  2010, 2012                              */
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

#ifndef BGWS_RESPONDER_PERFORMANCE_MONITORING_HPP_
#define BGWS_RESPONDER_PERFORMANCE_MONITORING_HPP_


#include "../AbstractResponder.hpp"

#include "../RequestRange.hpp"
#include "../TimeIntervalOption.hpp"

#include "capena-http/http/uri/Path.hpp"

#include <db/include/api/cxxdb/cxxdb.h>

#include <string>


namespace bgws {
namespace responder {


class PerformanceMonitoring : public AbstractResponder
{
public:

    static bool matchesUrl(
            const capena::http::uri::Path& requested_resource
        );


    PerformanceMonitoring(
            CtorArgs& args
        ) :
            AbstractResponder( args ),
            _blocking_operations_thread_pool(args.blocking_operations_thread_pool)
    { /* Nothing to do */ }


    // override
    capena::http::Methods _getAllowedMethods() const  { return { capena::http::Method::GET }; }

    // override
    void _doGet();

    //override
    void notifyDisconnect();


private:

    enum class _Grouping {
        None,
        Daily,
        Weekly,
        Monthly
    };


    static const std::string _FUNCTION_BOOT_MODE_BASIC_SQL;

    static _Grouping _parseGroupingCode( const std::string& grouping_code );


    BlockingOperationsThreadPool &_blocking_operations_thread_pool;

    cxxdb::QueryStatementPtr _stmt_ptr;


    void _checkAuthority();


    void _getMultiDetailsQuery(
            capena::server::ResponderPtr,
            const RequestRange& req_range,
            const TimeIntervalOption& interval
        );

    void _getMultiDetailsQueryComplete(
            capena::server::ResponderPtr,
            cxxdb::ConnectionPtr,
            cxxdb::ResultSetPtr rs_ptr,
            cxxdb::QueryStatementPtr comps_stmt_ptr,
            const RequestRange& req_range,
            unsigned total_count
        );


    void _getSingleQuery(
            capena::server::ResponderPtr,
            const TimeIntervalOption& interval,
            const std::string& detail,
            const std::string& block
        );

    void _getSingleQueryComplete(
            capena::server::ResponderPtr,
            cxxdb::ConnectionPtr,
            cxxdb::ResultSetPtr rs_ptr
        );


    void _getGroupedQuery(
            capena::server::ResponderPtr,
            const RequestRange& req_range,
            const TimeIntervalOption& interval,
            const std::string& detail,
            const std::string& block,
            _Grouping grouping
        );

    void _getGroupedQueryComplete(
            capena::server::ResponderPtr,
            const RequestRange& req_range,
            _Grouping grouping,
            cxxdb::ConnectionPtr,
            cxxdb::ResultSetPtr groups_rs_ptr,
            cxxdb::QueryStatementPtr comps_stmt_ptr,
            unsigned total_count
        );

};


}} // namespace bgws::responder


#endif
