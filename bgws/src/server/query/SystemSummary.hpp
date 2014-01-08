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


#ifndef BGWS_QUERY_SYSTEM_SUMMARY_HPP_
#define BGWS_QUERY_SYSTEM_SUMMARY_HPP_


#include "../types.hpp"

#include "chiron-json/fwd.hpp"

#include <db/include/api/cxxdb/fwd.h>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include <exception>

#include <stdint.h>


namespace bgws {
namespace query {


class SystemSummary : public boost::enable_shared_from_this<SystemSummary>
{
public:

    typedef boost::function<void ( std::exception_ptr exc_ptr, json::ObjectValuePtr val_ptr )> CompleteCb;

    typedef boost::shared_ptr<SystemSummary> Ptr;


    static Ptr create(
            BlockingOperationsThreadPool& blocking_operations_thread_pool
        );


    void start(
            CompleteCb complete_cb
        );


    ~SystemSummary();


private:


    boost::asio::strand _strand;
    BlockingOperationsThreadPool &_blocking_operations_thread_pool;

    CompleteCb _complete_cb;

    json::ObjectValuePtr _obj_val_ptr;

    std::exception_ptr _exc_ptr;


    SystemSummary(
            BlockingOperationsThreadPool& blocking_operations_thread_pool
        );


    void _doJobSummaryQuery();

    void _jobSummaryQueryComplete(
            uint64_t job_count,
            uint64_t job_cpus
        );

    void _doAlertSummaryQuery();

    void _alertSummaryQueryComplete(
            uint64_t alert_count
        );

    void _doMidplaneStatusQuery();

    void _midplaneStatusQueryComplete(
            cxxdb::ConnectionPtr,
            cxxdb::ResultSetPtr rs_ptr
        );

    void _doIoStatusQuery();

    void _ioStatusQueryComplete(
            cxxdb::ConnectionPtr,
            cxxdb::ResultSetPtr rs_ptr
        );

    void _doHardwareNotificationsQuery();

    void _hardwareNotificationsQueryComplete(
            cxxdb::ConnectionPtr,
            cxxdb::ResultSetPtr rs_ptr
        );

    void _doDiagnosticsQuery();

    void _diagnosticsQueryComplete(
            cxxdb::ConnectionPtr,
            cxxdb::ResultSetPtr rs_ptr
        );

    void _doServiceActionQuery();

    void _serviceActionQueryComplete(
            cxxdb::ConnectionPtr,
            cxxdb::ResultSetPtr rs_ptr
        );

};

} } // namespace bgws::query


#endif
