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
#include "mux/Multiplexer.h"

#include "common/ConnectionContainer.h"
#include "common/logging.h"
#include "common/TerminateHandler.h"

#include "mux/performance/Counters.h"

#include "mux/client/Container.h"
#include "mux/client/Listener.h"

#include "mux/server/Connection.h"

#include "mux/CommandListener.h"
#include "mux/Options.h"
#include "mux/Plugin.h"

#include <boost/thread.hpp>


namespace runjob {
namespace mux {

LOG_DECLARE_FILE( runjob::mux::log );

Multiplexer::Ptr
Multiplexer::create(
        const Options& options
        )
{
    return Ptr( new Multiplexer(options) );
}

Multiplexer::~Multiplexer()
{
    LOG_TRACE_MSG( __FUNCTION__ );
}

Multiplexer::Multiplexer(
        const Options& options
        ) :
    _options( options ),
    _io_service( options.getThreadPoolSize() ),
    _plugin(),
    _clientContainer( new client::Container(_io_service) ),
    _connections( new runjob::ConnectionContainer(_io_service) ),
    _server( new server::Connection(_io_service, _options, _clientContainer) ),
    _counters( new performance::Counters(_io_service, _options, _server) ),
    _commandListener(),
    _runjobListener(),
    _signalHandler( new TerminateHandler(_io_service) )
{

}

int
Multiplexer::start()
{
    try {
        _plugin.reset(
                new Plugin( shared_from_this() )
                );
        _commandListener.reset(
                new CommandListener( shared_from_this() )
                );
        _runjobListener.reset(
                new client::Listener( shared_from_this() )
                );
    } catch ( const std::exception& e ) {
        LOG_FATAL_MSG( e.what() );
        return EXIT_FAILURE;
    }

    _runjobListener->start();
    _plugin->start();
    _commandListener->start();
    _server->start();
    _counters->start();

    // create thread pool to invoke io_service
    LOG_DEBUG_MSG("creating " << _options.getThreadPoolSize() << " threads");
    boost::thread_group threads;
    for ( int i = 1 ; i <= _options.getThreadPoolSize(); ++i ) {
        LOG_TRACE_MSG("creating thread " << i);
        threads.create_thread( 
                boost::bind( 
                    &boost::asio::io_service::run, 
                    &_io_service
                    )
                );
    }

    // run service from main thread
    _io_service.run();

    // getting here means we're done
    LOG_INFO_MSG( "joining all threads" );
    threads.join_all();

    return EXIT_FAILURE;
}

} // mux
} // runjob
