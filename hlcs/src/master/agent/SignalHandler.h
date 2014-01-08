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
#ifndef BGAGENTD_SIGNAL_HANDLER_H
#define BGAGENTD_SIGNAL_HANDLER_H

#include <utility/include/SignalHandler.h>

#include <boost/asio/io_service.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include <unistd.h>

typedef bgq::utility::SignalHandler<SIGINT,SIGUSR1,SIGUSR2,SIGPIPE,SIGHUP,SIGABRT,SIGALRM,SIGTERM,SIGSYS,SIGQUIT> Signals;

//!\brief Class for signal handling in bgagent.
//! This class is to be used by a boost io_service that is created in bgagent.
class SignalHandler : Signals, public boost::enable_shared_from_this<SignalHandler>
{
public:
    /*!
     * \brief Pointer type
     */
    typedef boost::shared_ptr<SignalHandler> Ptr;

public:
    /*!
     * \brief Factory.
     */
    static Ptr create(
            boost::asio::io_service& io_service    //!< [in]
            );

    //! \brief Destructor
    ~SignalHandler();

    /*!
     * \brief Get signal number.
     */
    int getSignal() const { return _signal; }

private:
    SignalHandler(
            boost::asio::io_service& io_service
            );

    void handler(
		 const boost::system::error_code& error,
		 const siginfo_t& siginfo
		 );

private:
    boost::asio::io_service& _io_service;
    int _signal;
};

#endif
