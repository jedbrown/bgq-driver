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
/*!
 * \file utility/include/SignalHandler.h
 * \brief \link bgq::utility::SignalHandler SignalHandler\endlink definition and implementation.
 */

#ifndef BGQ_UTILITY_SIGNAL_HANDLER_H
#define BGQ_UTILITY_SIGNAL_HANDLER_H

#include <utility/include/Log.h>
#include <utility/include/ThrowException.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>

#include <csignal>
#include <iostream>

namespace bgq {
namespace utility {

/*!
 * \brief Signal handler using Boost.Asio patterns.
 *
 * This library is header only. Support for handling up to 10 different signals is provided.
 *
 * Sample usage:
 *
 * \include test/SignalHandler/sample.cc
 *
 */
template <
    int S1, int S2=0, int S3=0, int S4=0, int S5=0,
    int S6=0, int S7=0, int S8=0, int S9=0, int S10=0
    >
class SignalHandler
{
public:
    /*!
     * \brief Class type.
     */
    typedef SignalHandler<S1,S2,S3,S4,S5,S6,S7,S8,S9,S10> Type;

    /*!
     * \brief Callback type.
     */
    typedef boost::function<
        void(
                const boost::system::error_code&,
                const siginfo_t&
            )
        > Callback;

public:
    /*!
     * \brief ctor.
     */
    explicit SignalHandler(
            boost::asio::io_service& io_service //!< [in]
            ) :
        _stream( io_service ),
        _siginfo(),
        _oldAction()
    {
        memset( &_oldAction, 0, sizeof(_oldAction) );
        log4cxx::LoggerPtr log_logger_ = log4cxx::Logger::getLogger( "ibm.utility.SignalHandler" );

        // create pipe for handler to write into
        int descriptors[2];
        if ( pipe(descriptors) != 0 ) {
            UTILITY_THROW_EXCEPTION( std::runtime_error, "could not create pipe: " << strerror(errno) );
        }
        LOG_TRACE_MSG( "created pipe on fds " << descriptors[0] << " and " << descriptors[1] );

        // enable close on exec
        BOOST_FOREACH( int descriptor, descriptors ) {
            int flags = fcntl( descriptor, F_GETFL, 0 );
            if ( flags < 0 ) {
                UTILITY_THROW_EXCEPTION( std::runtime_error, "fcntl F_GETFL: " << strerror(errno) );
            }
            if ( fcntl( descriptor, F_SETFL, FD_CLOEXEC | flags ) < 0 ) {
                UTILITY_THROW_EXCEPTION( std::runtime_error, "fcntl F_SETFL: " << strerror(errno) );
            }
        }

        // assign read fd to stream descriptor
        _stream.assign( descriptors[0] );

        // assign write fd to static fd for handler
        _signal_fd = descriptors[1];

        // create sigaction
        struct sigaction sa;
        memset( &sa, 0, sizeof(sa) );
        sa.sa_sigaction = &Type::handler;
        sa.sa_flags = SA_SIGINFO;

        // create array for all signals we want to watch
        const int signals[] = {S1,S2,S3,S4,S5,S6,S7,S8,S9,S10};

        // iterate over all signals and add them to the sigaction mask
        sigemptyset( &sa.sa_mask );
        BOOST_FOREACH( int signal, signals ) {
            if ( signal != 0 ) {
                sigaddset( &sa.sa_mask, signal );
            }
        }

        // iterate over all signals and install handler
        BOOST_FOREACH( int signal, signals ) {
            // ignore signals that used default template argument
            if ( signal != 0 ) {
                LOG_DEBUG_MSG( "watching for signal " << signal );
                int rc = sigaction( signal, &sa, &_oldAction );
                if ( rc != 0 ) {
                    LOG_ERROR_MSG( "could not register sigaction for signal " << signal );
                    LOG_ERROR_MSG( strerror(errno) );
                }
            }
        }
    }

    /*!
     * \brief dtor.
     */
    ~SignalHandler()
    {
        log4cxx::LoggerPtr log_logger_ = log4cxx::Logger::getLogger( "ibm.utility.SignalHandler" );

        // close signal descriptor
        if ( _signal_fd ) {
            LOG_DEBUG_MSG( "closing signal descriptor " << _signal_fd );
            ::close(_signal_fd);
        }

        // create array for all signals we were watching
        const int signals[] = {S1,S2,S3,S4,S5,S6,S7,S8,S9,S10};

        // install old action
        BOOST_FOREACH( int signal, signals ) {
            // ignore signals that used default template argument
            if ( signal != 0 ) {
                LOG_DEBUG_MSG( "installing old signal handler for signal " << signal );
                (void)sigaction( signal, &_oldAction, NULL );
            }
        }
    }

    /*!
     * \brief
     */
    template <typename Handler>
    void async_wait(
            Handler handler //!< [in]
            )
    {
        // create Callback
        Callback callback(handler);

        boost::asio::async_read(
                _stream,
                boost::asio::buffer(
                    &_siginfo,
                    sizeof(siginfo_t)
                    ),
                boost::bind(
                    &Type::readHandler,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred,
                    callback 
                    )
                );
    }

    /*!
     * \brief stop waiting for signals.
     */
    void stop()
    {
        log4cxx::LoggerPtr log_logger_ = log4cxx::Logger::getLogger( "ibm.utility.SignalHandler" );
        boost::system::error_code error;
        _stream.cancel( error );
        if ( error ) {
            LOG_WARN_MSG( "could not stop: " << boost::system::system_error(error).what() );
        } else {
            LOG_DEBUG_MSG( "stopping" );
        }
    }


private:
    static int _signal_fd;

    /*!
     * \brief Signal handler method given to sigaction.
     */
    static void handler(
            int /* signal */,       //!< [in]
            siginfo_t* siginfo,     //!< [in]
            void*                   //!< [in]
            )
    {
        // ignore return value here, there's nothing else that can be done if write fails
        (void)write( _signal_fd, siginfo, sizeof(siginfo_t) );
    }

private:
    /*!
     * \brief handle a read.
     */
    void readHandler(
            const boost::system::error_code& error, //!< [in]
            std::size_t length,                     //!< [in]
            Callback callback                       //!< [in]
            )
    {
        log4cxx::LoggerPtr log_logger_ = log4cxx::Logger::getLogger( "ibm.utility.SignalHandler" );
        if ( error == boost::asio::error::operation_aborted ) {
            // we were asked to cancel
            LOG_DEBUG_MSG( "operation aborted" );
            return;
        }

        if ( error ) {
            LOG_ERROR_MSG( "could not read: " << boost::system::system_error(error).what() );
        } else {
            LOG_TRACE_MSG( "read handler " << length << " bytes" );
        }

        // invoke callback
        callback( error, _siginfo );
    }

private:
    boost::asio::posix::stream_descriptor _stream;
    siginfo_t _siginfo;
    struct sigaction _oldAction;
};

// static storage
template <
    int S1, int S2, int S3, int S4, int S5,
    int S6, int S7, int S8, int S9, int S10
    >
int SignalHandler<S1,S2,S3,S4,S5,S6,S7,S8,S9,S10>::_signal_fd;

} // utility
} // bgq

#endif
