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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#include "Monitor.h"

#include "BulkPower.h"
#include "Coolant.h"
#include "HealthCheck.h"
#include "IoDrawer.h"
#include "Location.h"
#include "NodeBoard.h"
#include "Optical.h"
#include "PerfData.h"
#include "ServiceCard.h"

#include "common/Properties.h"

#include <utility/include/Log.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/exception.hpp>
#include <boost/thread.hpp>

#include <csignal>

LOG_DECLARE_FILE( "mmcs.server" );

namespace {

void
run_io_service(
        boost::asio::io_service& io_service
        )
{
    while (1) {
        try {
            io_service.run();
            return;
        } catch ( const boost::exception& e ) {
            LOG_ERROR_MSG( boost::diagnostic_information(e) );
        } catch ( const std::exception& e ) {
            LOG_ERROR_MSG( e.what() );
        }

        sleep(10);
    }
}

}


namespace mmcs {
namespace server {
namespace env {


Monitor::Monitor() :
    Thread(),
    _io_service()
{

}

unsigned
Monitor::calculateThreadPool()
{
    int result = 0;
    const std::string key( "thread_pool_size" );
    const std::string value( common::Properties::getProperty(key.c_str()) );
    if ( value == "auto" || value.empty() ) {
        // calculate it below
    } else {
        try {
            result = boost::lexical_cast<int>( value );
            if ( result <= 0 ) {
                LOG_WARN_MSG( "invalid " << key << " value: '" << value << "'" );
                result = 0;
            } else {
                LOG_INFO_MSG( "set thread pool size to " << result << " from properties file" );
            }
        } catch ( const boost::bad_lexical_cast& e ) {
            LOG_WARN_MSG( "invalid " << key << " value: '" << value << "'" );
        }
    }

    if ( !result ) {
        // no specific reason to divide by 4 here, just that we don't need
        // a thread per cpu since most of these threads will be idle at any
        // given time
        result = boost::thread::hardware_concurrency() / 4;
    }

    return result;
}

void*
Monitor::threadStart()
{
    const Polling::Ptr nodeBoard(
            new NodeBoard( _io_service )
            );
    const Polling::Ptr ioDrawer(
            new IoDrawer( _io_service )
            );
    const Polling::Ptr serviceCard(
            new ServiceCard( _io_service )
            );
    const Polling::Ptr bulkPower(
            new BulkPower( _io_service )
            );
    const Polling::Ptr coolant(
            new Coolant( _io_service )
            );
    const Polling::Ptr optical(
            new Optical( _io_service )
            );
    const Polling::Ptr perf(
            new PerfData( _io_service )
            );

    _io_service.post(
            boost::bind( &Polling::wait, nodeBoard )
            );
    _io_service.post(
            boost::bind( &Polling::wait, ioDrawer )
            );
    _io_service.post(
            boost::bind( &Polling::wait, serviceCard )
            );
    _io_service.post(
            boost::bind( &Polling::wait, bulkPower )
            );
    _io_service.post(
            boost::bind( &Polling::wait, coolant )
            );
    _io_service.post(
            boost::bind( &Polling::wait, optical )
            );
    _io_service.post(
            boost::bind( &Polling::wait, perf )
            );

    // Create a thread for health check  (NOTE: this cannot be turned off or changed)
    HealthCheck* healthThread = new HealthCheck;
    healthThread->seconds = 300;
    healthThread->start();

    boost::thread_group threads;
    const unsigned numThreads = this->calculateThreadPool();
    LOG_TRACE_MSG( "creating " << numThreads << " threads" );
    for ( unsigned i = 1; i <= numThreads; ++i ) {
        LOG_TRACE_MSG( "creating thread " << i );
        threads.create_thread(
                boost::bind(
                    &run_io_service,
                    boost::ref(_io_service)
                    )
                );
    }

    while ( !isThreadStopping() ) {
        sleep(5);
    }

    _io_service.stop();

    // getting here means we're done
    LOG_INFO_MSG( "joining all threads" );
    threads.join_all();

    return NULL;
}

} } } // namespace mmcs::server::env
