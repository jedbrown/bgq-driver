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
#include "common/commands/Connection.h"

#include "common/logging.h"

#include "server/commands/kill_job/Options.h"

#include <boost/foreach.hpp>

#include <iostream>

LOG_DECLARE_FILE( runjob::server::commands::log );

int
main(
    int argc,
    char** argv
    )
{
    using namespace runjob::server::commands;

    int status = EXIT_FAILURE;
    try {
        // parse options
        const kill_job::Options options(argc, argv);

        if ( options.getHelp() ) {
            options.help(std::cout);
            status = EXIT_SUCCESS;
        } else if ( options.getVersion() ) {
            options.version(std::cout);
            std::cout << std::endl;
            status = EXIT_SUCCESS;
        } else if ( options.getList() ) {
            BOOST_FOREACH( const kill_job::Options::Signal& i, kill_job::Options::signals ) {
                const std::string name = boost::get<0>(i);
                const int number = boost::get<1>(i);
                std::cout << number << ") SIG" << name << std::endl;
            }
            status = EXIT_SUCCESS;
        } else {
            options.validate();
            runjob::commands::Connection connection(options);
            runjob::commands::Request::Ptr request = options.getRequest();
            runjob::commands::Response::Ptr response = connection.send( request );
            status = options.handle(response);
        }
    } catch ( const bgq::utility::Connector::ConnectError& e ) {
        std::cerr << "could not connect: " << e.what() << std::endl;
    } catch ( const boost::program_options::error& e ) {
        // eat the exception since it was previously logged
        status = EXIT_FAILURE;
    } catch ( const std::exception& e ) {
        LOG_FATAL_MSG( e.what() );
    }
        
    exit( status );
}

