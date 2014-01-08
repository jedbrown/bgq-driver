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


#include "BlockingOperationsThreadPool.hpp"

#include <utility/include/Log.h>

#include <boost/exception/all.hpp>
#include <boost/bind.hpp>

#include <unistd.h>


using boost::bind;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {


BlockingOperationsThreadPool::BlockingOperationsThreadPool()
{
    // Nothing to do.
}


void BlockingOperationsThreadPool::start( unsigned size )
{
    _work_ptr.reset( new boost::asio::io_service::work( _io_service ) );

    unsigned threads(0);
    while ( threads < size ) {
        _thread_group.create_thread( bind( &BlockingOperationsThreadPool::_runThread, this ) );
        ++threads;
    }
}


void BlockingOperationsThreadPool::post( boost::function<void( void )> fn )
{
    LOG_DEBUG_MSG( "Posting operation to blocking operations thread pool." );

    _io_service.post( fn );
}


void BlockingOperationsThreadPool::shutdown()
{
    LOG_INFO_MSG( "Shutting down blocking operations thread pool." );
    _work_ptr.reset();
}


void BlockingOperationsThreadPool::_runThread()
{
    LOG_DEBUG_MSG( "Starting blocking operations thread." );

    while ( true ) {
        try {
            _io_service.run();
            break;
        } catch ( boost::exception& e ) {
            LOG_ERROR_MSG( "Exception escaped from blocking operations thread pool event loop. Diagnostic Info:\n" << boost::diagnostic_information( e ) );
            sleep( 10 );
        } catch ( std::exception& e ) {
            LOG_ERROR_MSG( "Exception escaped from blocking operations event loop. The exception is " << e.what() );
            sleep( 10 ); // wait a little while before calling run again to avoid a tight loop.
        }
    }

    LOG_INFO_MSG( "Blocking operations thread completed." );
}

} // namespace bgws
