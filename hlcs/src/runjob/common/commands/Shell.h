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
#ifndef RUNJOB_SERVER_COMMANDS_SHELL_H
#define RUNJOB_SERVER_COMMANDS_SHELL_H

#include "common/commands/Connection.h"

#include <iostream>

namespace runjob {
namespace commands {

/*!
 * \brief this class holds options common across all commands.
 *
 * \tparam Options options specific to the command.
 */
template <typename Options>
class Shell
{
public:
    /*!
     * \brief Ctor.
     */
    Shell(
            unsigned int argc,  //!< [in]
            char** argv         //!< [in]
            ) :
        _status( EXIT_FAILURE ),
        _argc( argc ),
        _argv( argv )
    {

    }

    /*!
     * \brief Parse arguments and run the command.
     */
    int run()
    {
        try {
            // parse options
            const Options options( _argc, _argv );

            // check for help
            if ( options.getHelp() ) {
                options.help(std::cout);
                _status = EXIT_SUCCESS;
            } else if ( options.getVersion() ) {
                options.version(std::cout);
                std::cout << std::endl;
                _status = EXIT_SUCCESS;
            } else {
                options.validate();
                runjob::commands::Connection connection(options);
                runjob::commands::Request::Ptr request = options.getRequest();
                runjob::commands::Response::Ptr response = connection.send( request );
                _status = options.handle(response);
            }
        } catch ( const bgq::utility::Connector::ConnectError& e ) {
            std::cerr << "could not connect: " << e.what() << std::endl;
        } catch ( const boost::program_options::error& e ) {
            // eat the exception since it was previously logged
            _status = EXIT_FAILURE;
        }

        return _status;
    }

private:
    int _status;
    unsigned int _argc;
    char** _argv;
};

} // commands
} // runjob

#endif
