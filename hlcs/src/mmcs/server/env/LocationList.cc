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

#include "LocationList.h"

#include "Location.h"

#include "libmmcs_client/CommandReply.h"

#include <utility/include/Log.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/thread.hpp>

LOG_DECLARE_FILE( "mmcs.server" );

namespace {

boost::once_flag init_once_flag = BOOST_ONCE_INIT;
boost::asio::io_service _io_service;

void
run_io_service()
{
    boost::asio::io_service::work work( _io_service );

    while (1) {
        try {
            _io_service.run();
        } catch ( const boost::exception& e ) {
            LOG_ERROR_MSG( boost::diagnostic_information(e) );
        } catch ( const std::exception& e ) {
            LOG_ERROR_MSG( e.what() );
        }

        sleep(10);
    }
}

void
start_io_service()
{
    LOG_TRACE_MSG("Starting location specific polling thread.");
    boost::thread t( &run_io_service );
}

}

namespace mmcs {
namespace server {
namespace env {

bool
LocationList::start(
        const std::string& location,
        const std::string& type,
        const unsigned seconds
        )
{
    boost::mutex::scoped_lock lock( _mutex );

    // ensure location does not already exist
    const Locations::const_iterator result = std::find_if(
            _locations.begin(),
            _locations.end(),
            boost::bind(
                std::equal_to<std::string>(),
                boost::bind(
                    &Location::getLocation,
                    _1
                    ),
                location
                )
            );

    if ( result != _locations.end() )  {
        return false;
    }

    const boost::shared_ptr<Location> polling(new Location( _io_service, type, location, seconds ));
    _io_service.post( boost::bind( &Polling::wait, polling ));

    boost::call_once( &start_io_service, init_once_flag );

    _locations.push_back( polling );

    return true;
}

bool
LocationList::stop(
        const std::string& location
        )
{
    boost::mutex::scoped_lock lock( _mutex );

    const Locations::const_iterator result = std::find_if(
            _locations.begin(),
            _locations.end(),
            boost::bind(
                std::equal_to<std::string>(),
                boost::bind(
                    &Location::getLocation,
                    _1
                    ),
                location
                )
            );

    if ( result == _locations.end() ) {
        return false;
    }

    (*result)->stop();

    return true;
}

void
LocationList::remove(
        const std::string& location
        )
{
    boost::mutex::scoped_lock lock( _mutex );
    //LOG_TRACE_MSG( __FUNCTION__ << " " << location );

    const Locations::iterator result = std::find_if(
            _locations.begin(),
            _locations.end(),
            boost::bind(
                std::equal_to<std::string>(),
                boost::bind(
                    &Location::getLocation,
                    _1
                    ),
                location
                )
            );

    if ( result == _locations.end() ) {
        return;
    }

    _locations.erase( result );
}


void
LocationList::list(
        mmcs_client::CommandReply& reply
        )
{
    boost::mutex::scoped_lock lock( _mutex );

    reply << mmcs_client::OK;
    BOOST_FOREACH( const boost::shared_ptr<Location>& i, _locations ) {
        reply << i->getType() << " location: " << i->getLocation() << " interval: " << i->getInterval() << std::endl;
    }
    reply << mmcs_client::DONE;
}

} } } // namespace mmcs::server::env
