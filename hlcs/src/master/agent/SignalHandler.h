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

#include "Agent.h"

#include <utility/include/Properties.h>
#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/SignalHandler.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <iostream>

#include <unistd.h>

Agent* agent;

//!\brief Class for signal handling in bgagent.
//! This class is to be used by a boost io_service that is created in bgagent.
//! Signals sent to bgagent are caught by this io_service which is running in a secondary thread.
//! Signals are propagated to all the running binaries followed by bgagent cleaning up..   
class SignalHandler : public bgq::utility::SignalHandler<SIGINT,SIGUSR1,SIGUSR2,SIGPIPE,SIGHUP,SIGABRT,SIGALRM,SIGTERM,SIGSYS,SIGQUIT>, public boost::enable_shared_from_this<SignalHandler>
{
 public:
    //! \brief Constructor
    SignalHandler(
	      boost::asio::io_service& io_service
	      ) :
  bgq::utility::SignalHandler<SIGINT,SIGUSR1,SIGUSR2,SIGPIPE,SIGHUP,SIGABRT,SIGALRM,SIGTERM,SIGSYS,SIGQUIT>(io_service)
      {
	  LOG_DECLARE_FILE( "master" );
	  LOG_TRACE_MSG(__FUNCTION__);
      }
    //! \brief Destructor
    ~SignalHandler() {
        LOG_DECLARE_FILE( "master" );
        LOG_INFO_MSG("SignalHandler terminating ...")
    }
    //! \brief Sets up the handler and pointer to the agent we will need to shutdown.
    //! \param agentPtr A pointer to the agent we are setting up the signal handler for.
    void start(
	       Agent* agentPtr
	       )
    {
        LOG_DECLARE_FILE( "master" );
        LOG_TRACE_MSG(__FUNCTION__);
	LOG_INFO_MSG( "Starting SignalHandler ..." );

	//Set up a pointer in this to the agent for use when we need to signal shutdown.
	agent = agentPtr;

	this->async_wait(
			 boost::bind(
				     &SignalHandler::handler,
				     shared_from_this(),
				     _1,
				     _2
				     )
			 );
    }

 private:
    //! \brief Signal handler which calls Agent::clenup and propagates the signal number.
    //! \param error Boost error_code object.
    //! \param siginfo Signal information object.
    void handler(
		 const boost::system::error_code& error,
		 const siginfo_t& siginfo
		 )
    {
        LOG_DECLARE_FILE( "master" );
        LOG_TRACE_MSG(__FUNCTION__);

	if( !error ) {

	    switch (siginfo.si_signo) {
	        case SIGUSR1:
	        case SIGPIPE:
	        case SIGHUP:
	            LOG_DEBUG_MSG( "ignored signal " << siginfo.si_signo);
	            this->async_wait(
				     boost::bind(
						 &SignalHandler::handler,
						 shared_from_this(),
						 _1,
						 _2
						 )
				     );
		    break;
	        default:
	            LOG_INFO_MSG( "received signal " << siginfo.si_signo);
		    //Call the cleanup function.  It takes care of the shutdown for us.
		    agent->cleanup(siginfo.si_signo);
		    break;
	    }

	} else {
	    LOG_INFO_MSG( "signal handler: " << boost::system::system_error(error).what());
	}

    }

};

#endif
