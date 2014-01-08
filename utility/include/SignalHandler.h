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

#include <sys/signalfd.h>
#include <csignal>

namespace bgq {
namespace utility {

/*!
 * \brief Signal handler using Boost.Asio patterns.
 *
 * This library is header only. Support for handling up to 10 different signals is provided. It
 * is a small wrapper around the signalfd(2) API using Boost Asio. It uses the sigprocmask(2)
 * API so you should instantiate this class before creating any threads in your application. Use
 * of sigprocmask(2) in multi-threaded applications is undefined.
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
        _siginfo()
    {
        log4cxx::LoggerPtr log_logger_ = log4cxx::Logger::getLogger( "ibm.utility.SignalHandler" );

        sigset_t mask;
        sigemptyset( &mask );

        // create array for all signals we want to watch
        const int signals[] = {S1,S2,S3,S4,S5,S6,S7,S8,S9,S10};
        BOOST_FOREACH( const int signal, signals ) {
            if ( !signal ) continue;
            sigaddset( &mask, signal );
        }

        if ( sigprocmask(SIG_BLOCK, &mask, NULL) == -1 ) {
            UTILITY_THROW_EXCEPTION( std::runtime_error, "sigprocmask(SIG_BLOCK): " << strerror(errno) );
        }

        const int signal_fd = signalfd( -1, &mask, SFD_NONBLOCK | SFD_CLOEXEC );
        if ( signal_fd == -1 ) {
            UTILITY_THROW_EXCEPTION( std::runtime_error, "signalfd: " << strerror(errno) );
        }
       
        LOG_TRACE_MSG( "assigned descriptor " << signal_fd );
        _stream.assign( signal_fd );
    }

    /*!
     * \brief dtor.
     */
    ~SignalHandler()
    {
        log4cxx::LoggerPtr log_logger_ = log4cxx::Logger::getLogger( "ibm.utility.SignalHandler" );

        // create array for all signals we were watching
        const int signals[] = {S1,S2,S3,S4,S5,S6,S7,S8,S9,S10};

        struct sigaction action;
        memset( &action, 0, sizeof(action) );
        action.sa_handler = SIG_DFL;

        // install old action
        BOOST_FOREACH( const int signal, signals ) {
            // ignore signals that used default template argument
            if ( !signal ) continue;

            LOG_DEBUG_MSG( "installing default handler for signal " << signal );
            (void)sigaction( signal, &action, NULL );
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
                    sizeof(signalfd_siginfo)
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
            return;
        }
        LOG_TRACE_MSG( "read handler " << length << " bytes" );

        // need to translate into a siginfo_t structure
        siginfo_t result;
        memset( &result, 0, sizeof(result) );
        result.si_signo = _siginfo.ssi_signo;
        result.si_errno = _siginfo.ssi_errno;
        result.si_code = _siginfo.ssi_code;
        result.si_pid = _siginfo.ssi_pid;
        result.si_uid = _siginfo.ssi_uid;
        result.si_status = _siginfo.ssi_status;
        result.si_utime = _siginfo.ssi_utime;
        result.si_stime = _siginfo.ssi_stime;
        result.si_int = _siginfo.ssi_int;
        result.si_ptr = reinterpret_cast<void*>(_siginfo.ssi_ptr);
        result.si_overrun = _siginfo.ssi_overrun;
        result.si_timerid = _siginfo.ssi_tid;
        result.si_addr = reinterpret_cast<void*>(_siginfo.ssi_addr);
        result.si_band = _siginfo.ssi_band;
        result.si_fd = _siginfo.ssi_fd;

        // invoke callback
        callback( error, result );
    }

private:
    boost::asio::posix::stream_descriptor _stream;
    signalfd_siginfo _siginfo;
};

} // utility
} // bgq

#endif
