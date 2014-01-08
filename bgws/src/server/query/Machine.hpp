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


#ifndef BGWS_QUERY_MACHINE_HPP_
#define BGWS_QUERY_MACHINE_HPP_


#include "../types.hpp"

#include "chiron-json/fwd.hpp"

#include <db/include/api/cxxdb/fwd.h>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include <exception>


namespace bgws {
namespace query {


class Machine : public boost::enable_shared_from_this<Machine>
{
public:

    typedef boost::shared_ptr<Machine> Ptr;

    typedef boost::function<void ( std::exception_ptr exc_ptr, json::ObjectValuePtr obj_val_ptr )> CompleteCb;


    static Ptr create(
            BlockingOperationsThreadPool &blocking_operations_thread_pool
        );


    void start(
            CompleteCb complete_cb
        );


    ~Machine();


private:

    BlockingOperationsThreadPool &_blocking_operations_thread_pool;
    boost::asio::strand _strand;

    CompleteCb _complete_cb;

    std::exception_ptr _exc_ptr;
    json::ObjectValuePtr _obj_val_ptr;

    json::Object *_not_available_obj_p;


    Machine(
            BlockingOperationsThreadPool &blocking_operations_thread_pool
        );


    void _startMachineDetailsQuery();

    void _machineDetailsQueryComplete(
            cxxdb::ConnectionPtr,
            cxxdb::ResultSetPtr rs_ptr
        );


    void _startComputeRacks();

    void _computeRacksComplete(
            cxxdb::ConnectionPtr,
            cxxdb::ResultSetPtr rs_ptr
        );


    void _startIoRacks();

    void _ioRacksComplete(
            cxxdb::ConnectionPtr,
            cxxdb::ResultSetPtr rs_ptr
        );


    void _startUnavailableHardware();

    void _unavailableHardwareComplete(
            cxxdb::ConnectionPtr,
            cxxdb::ResultSetPtr rs_ptr
        );


    void _startNotAvailableServiceCard();

    void _notAvailableServiceCardComplete(
            cxxdb::ConnectionPtr,
            cxxdb::ResultSetPtr rs_ptr
        );


    void _startNotAvailableSwitches();

    void _notAvailableSwitchesComplete(
            cxxdb::ConnectionPtr,
            cxxdb::ResultSetPtr rs_ptr
        );


    void _startNotAvailableNodes();

    void _notAvailableNodesComplete(
            cxxdb::ConnectionPtr,
            cxxdb::ResultSetPtr rs_ptr
        );


    void _startNotAvailableIoNodes();

    void _notAvailableIoNodesComplete(
            cxxdb::ConnectionPtr,
            cxxdb::ResultSetPtr rs_ptr
        );


    void _startNotAvailableDcas();

    void _notAvailableDcasComplete(
            cxxdb::ConnectionPtr,
            cxxdb::ResultSetPtr rs_ptr
        );


    void _startNotAvailablePowerModules();

    void _notAvailablePowerModulesComplete(
            cxxdb::ConnectionPtr,
            cxxdb::ResultSetPtr rs_ptr
        );
};


} } // namespace bgws::query



#endif
