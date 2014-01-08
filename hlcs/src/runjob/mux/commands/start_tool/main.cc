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

#include "common/logging.h"

#include "mux/commands/start_tool/Options.h"

#include <boost/asio.hpp>

LOG_DECLARE_FILE( runjob::mux::commands::log );

int
main(
    int argc,
    char** argv
    )
{
    int status = EXIT_SUCCESS;
    try {
        using namespace runjob::mux::commands;
        start_tool::Options options( argc, argv );

        // check for help and version
        if ( options.getHelp() ) {
            options.help(std::cout);
        } else if ( options.getVersion() ) {
            options.version(std::cout);
            std::cout << std::endl;
        } else {
            options.validate();
            boost::asio::io_service io_service;
            boost::asio::local::stream_protocol::socket socket( io_service );

            // create remote endpoint
            std::string path( 1, '\0' ); // first byte is NULL for anonymous namespace
            path.append( options.socket() );
            LOG_DEBUG_MSG( "connecting to " << options.socket() );
            const boost::asio::local::stream_protocol::endpoint ep( path );

            socket.connect( ep );
            LOG_DEBUG_MSG( "connected to " << options.socket() );
            options.handle( socket );
        }
    } catch ( const boost::program_options::error& e ) {
        // eat this error
    } catch ( const std::exception& e ) {
        LOG_FATAL_MSG( e.what() );
        status = EXIT_FAILURE;
    }
        
    exit( status );
}

