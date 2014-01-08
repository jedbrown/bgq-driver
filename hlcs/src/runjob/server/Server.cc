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
#include "server/Server.h"

#include "common/ConnectionContainer.h"
#include "common/logging.h"

#include "server/block/Container.h"

#include "server/cios/Heartbeat.h"

#include "server/database/Init.h"

#include "server/job/Container.h"

#include "server/mux/Listener.h"

#include "server/performance/Counters.h"

#include "server/realtime/Connection.h"

#include "server/sim/Iosd.h"
#include "server/sim/SharedMemory.h"

#include "server/CommandListener.h"
#include "server/Options.h"
#include "server/Security.h"
#include "server/SignalHandler.h"

#include <hlcs/include/bgsched/bgsched.h>

#include <boost/thread/thread.hpp>

namespace runjob {
namespace server {

LOG_DECLARE_FILE( runjob::server::log );

Server::~Server()
{
    LOG_INFO_MSG( "terminating" );
}

Server::Server(
        const Options& options
        ) :
    _io_service( options.getThreadPoolSize() ),
    _stopped( false ),
    _options( options ),
    _signalHandler(),
    _childHandler(),
    _command(),
    _mux(),
    _database(),
    _performanceCounters(),
    _jobs(),
    _blocks(),
    _connections( new ConnectionContainer(_io_service) ),
    _simCounter( new sim::SharedMemory(options) ),
    _security(),
    _realtime()
{
    // set simulation counter
    sim::Iosd::setCounter( this->getSimCounter() );

    bgsched::init( _options.getProperties()->getFilename() );
}

int
Server::start()
{
    try {
        _signalHandler = SignalHandler::create(
                shared_from_this()
                );

        _database = database::Init::create(
                shared_from_this()
                );

        _security = Security::create(
                shared_from_this()
                );

        _command.reset(
                new CommandListener(
                    shared_from_this()
                    )
                );

        _mux.reset(
                new mux::Listener(
                    shared_from_this()
                    )
                );

        _performanceCounters = performance::Counters::create(
                shared_from_this()
                );

        _jobs.reset(
                new job::Container(
                    shared_from_this()
                    )
                );
        _blocks = block::Container::create(
                shared_from_this()
                );

        _realtime = realtime::Connection::create(
                shared_from_this()
                );

        cios::Heartbeat::create(
                shared_from_this()
                );
    } catch ( const std::exception& e ) {
        LOG_FATAL_MSG( e.what() );
        return EXIT_FAILURE;
    }

    // create thread pool to invoke io_service
    LOG_DEBUG_MSG( "creating " << _options.getThreadPoolSize() << " threads" );
    boost::thread_group threads;
    for ( int i = 1 ; i <= _options.getThreadPoolSize(); ++i ) {
        LOG_TRACE_MSG( "creating thread " << i );
        threads.create_thread( 
                boost::bind( 
                    &boost::asio::io_service::run, 
                    &_io_service
                    )
                );
    }

    // run service
    _io_service.run();

    // getting here means we're done
    LOG_INFO_MSG( "joining all threads" );
    threads.join_all();

    return EXIT_FAILURE;
}

} // server
} // runjob
