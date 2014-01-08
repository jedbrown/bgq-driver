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

#include "Polling.h"

#include "BulkPower.h"
#include "Coolant.h"
#include "IoDrawer.h"
#include "Location.h"
#include "McServerConnection.h"
#include "NodeBoard.h"
#include "Optical.h"
#include "PerfData.h"
#include "ServiceCard.h"
#include "types.h"

#include "../HardwareBlockList.h"

#include "common/Properties.h"

#include <utility/include/Log.h>

#include <xml/include/c_api/MCServerMessageSpec.h>

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/asio.hpp>
#include <boost/utility.hpp>

#include <limits>

using mmcs::common::Properties;

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace env {

Polling::Polling(
        boost::asio::io_service& io_service,
        unsigned interval
        ) :
    _start( ),
    _seconds( interval ),
    _enabled( true ),
    _counters( new PerformanceCounters("mmcs") ),
    _io_service( io_service ),
    _timer( _io_service )
{

}

Polling::~Polling()
{
}

void
Polling::poll(
        const boost::system::error_code& error
        )
{
    if ( !_enabled ) {
        this->done();
        return;
    }

    if ( error ) {
        LOG_ERROR_MSG( boost::system::system_error(error).what() );
        return;
    }

    if ( !_seconds ) {
        LOG_TRACE_MSG( this->getDescription() << " polling disabled" );
        this->wait();
        return;
    }

    if ( HardwareBlockList::list_size() ) {
        LOG_INFO_MSG( this->getDescription() << " Subnet failover in progress, polling suspended." );
        this->wait();
        return;
    }

    if ( Properties::getProperty("bypass_envs") == "true" ) {
        // disable polling if a full system boot is in progress
        LOG_INFO_MSG( this->getDescription() << " suspending polling while full system boot is in progress." );
        this->wait();
        return;
    }

    _start = boost::posix_time::second_clock::local_time();

    const McServerConnection::Ptr mc_server(
            McServerConnection::create(
                _io_service
                )
            );

    mc_server->start(
            this->getDescription(),
            boost::bind(
                &Polling::connectHandler,
                shared_from_this(),
                _1,
                _2,
                mc_server
                )
            );
}

void
Polling::connectHandler(
        const bgq::utility::Connector::Error::Type error,
        const std::string& message,
        const McServerConnection::Ptr& mc_server
        )
{
    if ( error ) {
        LOG_WARN_MSG( this->getDescription() << ": could not connect: " << message );
        this->wait();
    }

    this->impl( mc_server );
}

Polling::Timer::Ptr
Polling::time()
{
    static const boost::posix_time::ptime epoch(boost::gregorian::date(1970,1,1));
    const Timer::Ptr result = _counters->create()->
        id( "EnvMon" )->
        function( this->getDescription() )->
        mode( bgq::utility::performance::Mode::Value::Basic )->
        qualifier( (_start - epoch).total_seconds() )
        ;

    return result;
}

unsigned
Polling::getPollingProperty()
{
    std::string name;

    if ( dynamic_cast<env::BulkPower*>(this) ) {
        name = MMCS_ENVS_BULK_INTERVAL_SECONDS;
    } else if ( dynamic_cast<env::Coolant*>(this) ) {
        name = MMCS_ENVS_COOLANT_INTERVAL_SECONDS;
    } else if ( dynamic_cast<env::IoDrawer*>(this) ) {
        name = MMCS_ENVS_IO_INTERVAL_SECONDS;
    } else if ( dynamic_cast<env::NodeBoard*>(this) ) {
        name = MMCS_ENVS_NC_INTERVAL_SECONDS;
    } else if ( dynamic_cast<env::Optical*>(this) ) {
        name = MMCS_ENVS_OPT_INTERVAL_SECONDS;
    } else if ( dynamic_cast<env::ServiceCard*>(this) ) {
        name = MMCS_ENVS_SC_INTERVAL_SECONDS;
    } else if ( dynamic_cast<env::PerfData*>(this) ) {
        name = MMCS_PERFDATA_INTERVAL_SECONDS;
    } else if ( dynamic_cast<env::Location*>(this) ) {
        // location specific polling never comes from properties file
        return _seconds;
    }

    BOOST_ASSERT( !name.empty() );

    if ( Properties::getProperty(name.c_str()).empty() ) {
        LOG_DEBUG_MSG( "Missing " << name << " key" );
        return _seconds;
    }

    try {
        return boost::lexical_cast<unsigned>( Properties::getProperty(name) );
    } catch ( const boost::bad_lexical_cast& e ) {
        LOG_WARN_MSG( "Bad " << name << " value: " << Properties::getProperty(name.c_str()) );
        LOG_WARN_MSG( "Using default of " << _seconds );
        return _seconds;
    }
}

void
Polling::wait()
{
    // output any counters collected from previous interval
    _counters->output();

    const unsigned property = this->getPollingProperty();

    // 0 seconds means disable polling
    if ( !property ) {
        _seconds = 0;
    }

    // valid intervals are 1 - 60 minutes
    if ( property < 60 ) {
        // do nothing;
    } else if ( property > 3600 ) {
        // do nothing
    } else if ( property == _seconds ) {
        // do nothing
    } else {
        _seconds = property;
        LOG_WARN_MSG(this->getDescription() << " interval changed to " << _seconds << " seconds.");
    }

    _timer.expires_from_now(
            // wait for 5 minutes if we are disabled
            boost::posix_time::seconds( _seconds ? _seconds : ENVS_POLLING_PERIOD )
            );
    LOG_TRACE_MSG(this->getDescription() << " waiting " << _timer.expires_from_now().total_seconds() << " seconds.");
    _timer.async_wait(
            boost::bind(
                &Polling::poll,
                shared_from_this(),
                boost::asio::placeholders::error
                )
            );
}

void
Polling::closeTarget(
        const McServerConnection::Ptr& mc_server,
        const std::string& set,
        const int handle,
        const Callback& callback
        )
{
    MCServerMessageSpec::CloseTargetRequest request;
    request._set = set;
    request._handle = handle;

    mc_server->send(
            request.getClassName(),
            request,
            boost::bind(
                &Polling::closeTargetHandler,
                shared_from_this(),
                _1,
                set,
                handle,
                callback
                )
            );
}

void
Polling::closeTargetHandler(
        std::istream& response,
        const std::string& set,
        const int handle,
        const Callback& callback
        )
{
    MCServerMessageSpec::CloseTargetReply reply;
    reply.read( response );
    if ( reply._rc ) {
        LOG_ERROR_MSG(
                this->getDescription() << ": could not close target " << set << " with handle " <<
                handle << " (" << reply._rc << ")"
                );
        if ( !reply._rt.empty() ) {
            LOG_ERROR_MSG( this->getDescription() << reply._rt );
        }
    } else {
        LOG_TRACE_MSG("Closed target " << set << " with handle " << handle );
    }

    // post callback through I/O service because it may be long running. There's an implicit
    // strand in the asio SSL service implementation, see boost/asio/ssl/detail/openssl_stream_service.hpp
    _io_service.post( callback );
}

} } } // namespace mmcs::server::env
