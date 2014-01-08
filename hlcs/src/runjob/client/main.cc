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
#include "client/bgsched/ClientImpl.h"
#include "client/options/Parser.h"
#include "client/SignalHandler.h"

#include "common/logging.h"

#include <utility/include/ExitStatus.h>

#include <boost/make_shared.hpp>

#include <csignal>
#include <iostream>

LOG_DECLARE_FILE( runjob::client::log );

int
main(int argc, char** argv)
{
    using namespace runjob::client;

    bgq::utility::ExitStatus status;
    bool raise = false;
    try {
        bgsched::runjob::Client::Impl runjob( argc, argv );

        raise = runjob.options()->getRaise();
        if ( runjob.options()->getHelp() ) {
            runjob.options()->help( std::cout );
            status = bgq::utility::ExitStatus( EXIT_SUCCESS );
        } else if ( runjob.options()->getVersion() ) {
            runjob.options()->version( std::cout );
            std::cout << std::endl;
            status = bgq::utility::ExitStatus( EXIT_SUCCESS );
        } else {
            LOGGING_DECLARE_JOB_MDC( getpid() );
            LOGGING_DECLARE_BLOCK_MDC( runjob.options()->getJobInfo().getBlock() );

            const SignalHandler::Ptr signals(
                    SignalHandler::create( runjob )
                    );

            status = runjob.start( STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO );
        }
    } catch ( const boost::program_options::error& e ) {
        // eat this error, we have already logged it
        exit( EXIT_FAILURE );
    } catch ( const std::exception& e ) {
        LOG_FATAL_MSG( e.what() );
        exit( EXIT_FAILURE );
    }

    if ( status.exited() ) {
        LOG_DEBUG_MSG( "exit status: " << status.getExitStatus() );
        exit( status.getExitStatus() );
    } else if ( status.signaled() && raise ) {
        LOG_DEBUG_MSG( "raising signal: " << status.getSignal() );
        ::raise( status.getSignal() );
    } else if ( status.signaled() ) {
        // death by signal, add 128 to signal number per UNIX conventions
        LOG_DEBUG_MSG( "terminated by signal: " << status.getSignal() );
        exit( status.getSignal() + 128 );
    } else {
        exit( EXIT_FAILURE );
    }
}

