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
#include "server/sim/Inotify.h"

#include "server/cios/Connection.h"

#include "common/logging.h"

#include <boost/filesystem/fstream.hpp>

#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include <fstream>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace sim {

Inotify::Inotify(
        boost::asio::io_service& io_service,
        const std::string& block
        ) :
    _strand( io_service ),
    _started( false ),
    _notify( io_service ),
    _events(),
    _watches(),
    _block( block )
{

}

Inotify::~Inotify()
{
    LOGGING_DECLARE_BLOCK_MDC( _block );
    LOG_TRACE_MSG( "terminating" );
}

void
Inotify::start()
{
    _strand.post(
            boost::bind(
                &Inotify::startImpl,
                shared_from_this()
                )
            );
}

void
Inotify::startImpl()
{
    _started = true;
    _notify.async_read(
            _events,
            _strand.wrap(
                boost::bind(
                    &Inotify::handleRead,
                    shared_from_this(),
                    boost::asio::placeholders::error
                    )
                )
            );
}

void
Inotify::stop()
{
    _strand.post(
            boost::bind(
                &Inotify::stopImpl,
                shared_from_this()
                )
            );
}

void
Inotify::stopImpl()
{
    LOGGING_DECLARE_BLOCK_MDC( _block );
    LOG_TRACE_MSG( "stopping" );
    boost::system::error_code error;
    _notify.cancel( error );
    if ( error ) {
        LOG_WARN_MSG( "stopping: " << error.message() );
    }
}

void
Inotify::watch(
        const boost::filesystem::path& path,
        const std::string& file,
        const cios::Connection::Ptr& connection
        )
{
    const WatchPtr watch(
            new Watch()
            );
    watch->_path /= path / file;
    watch->_connection = connection;

    // open and close the file we are watching, the bgq::utility::Inotify object
    // requires the file to exist prior to watching it
    LOG_TRACE_MSG( "watching " << watch->_path );
    {
        const boost::filesystem::ofstream file(watch->_path);
    }

    _strand.post(
            boost::bind(
                &Inotify::watchImpl,
                shared_from_this(),
                watch
                )
            );
}

void
Inotify::watchImpl(
        const WatchPtr& watch
        )
{
    if ( _started ) {
        LOG_ERROR_MSG( "cannot add watch " << watch->_path << " after being started" );
        return;
    }

    try {
        watch->_watch = _notify.watch(
                watch->_path,
                IN_CLOSE_WRITE
                );

        _watches.push_back( watch );
    } catch ( const boost::system::system_error& e ) {
        LOG_WARN_MSG( "could not watch " << watch->_path << ": " << e.what() );
    }
}

void
Inotify::handleRead(
        const boost::system::error_code& error
        )
{
    LOGGING_DECLARE_BLOCK_MDC( _block );
    LOG_TRACE_MSG( "handle read events " << _events.size() );
    if ( error == boost::asio::error::operation_aborted ) {
        LOG_TRACE_MSG( "could not read: " << error.message() );
        return;
    } else if ( error ) {
        LOG_ERROR_MSG( "could not read: " << error.message() );
        return;
    }

    BOOST_FOREACH( const bgq::utility::Inotify::Event& event, _events ) {
        // promote Watch to a shared_ptr
        bgq::utility::Inotify::Watch watch = event.watch_ref.lock();
        if ( !watch ) continue;

        // look for this watched event in our container
        // by comparing pointers
        const WatchContainer::iterator i = std::find_if(
                _watches.begin(),
                _watches.end(),
                boost::bind(
                    std::equal_to<bgq::utility::Inotify::Watch>(),
                    watch,
                    boost::bind(
                        &Watch::_watch,
                        _1
                        )
                    )
                );
        if ( i != _watches.end() ) {
            const WatchPtr& watch = *i;
            LOG_TRACE_MSG( "found " << watch->_path );
            this->connect( watch->_path, watch->_connection );
            _watches.erase( i );
        } else {
            // couldn't find event, nothing more we can do
            LOG_WARN_MSG( "ignoring event" );
        }
    }

    this->start();
}

void
Inotify::connect(
        const boost::filesystem::path& path,
        const cios::Connection::Ptr& connection
        )
{
    LOGGING_DECLARE_BLOCK_MDC( _block );
    try {
        LOG_TRACE_MSG( "opening " << path );
        boost::filesystem::ifstream file( path );
        if ( !file ) {
            LOG_ERROR_MSG( "could not open '" << path << "'" );
            return;
        }

        uint16_t port;
        file >> port;
        if ( !file ) {
            LOG_ERROR_MSG( "could not read port" );
        } else {
            connection->start(
                    boost::asio::ip::tcp::endpoint(
                        boost::asio::ip::address::from_string("127.0.0.1"),
                        port
                        )
                    );
        }
    } catch ( const boost::filesystem::filesystem_error& e ) {
        LOG_WARN_MSG( e.what() );
    }
}

} // sim
} // server
} // runjob
