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

#include "Options.h"

#include "common/Properties.h"

#include <utility/include/version.h>

#include <utility/include/Log.h>

#include <boost/filesystem.hpp>

#include <boost/tokenizer.hpp>

#include <log4cxx/logger.h>

#include <iostream>


using mmcs::common::Properties;


LOG_DECLARE_FILE( "mmcs.lite" );


namespace mmcs {
namespace lite {


Options::Options(
        unsigned int argc,
        char** argv
        ) :
    _argc( argc ),
    _argv( argv ),
    _variables_map(),
    _options( "Options" ),
    _propertiesOptions(),
    _loggingOptions( "ibm.mmcs" )
{
    // create options
    _options.add_options()
        ("help,h", boost::program_options::bool_switch(), "this help text")
        ("version,v", boost::program_options::bool_switch(), "display version information")
        ("no-default-listener,n", boost::program_options::bool_switch(), "do not create a default RAS listener")
        ("no-eof-exit", boost::program_options::bool_switch(), "do not terminate after reading a script" )
        ("no-bringup,b", boost::program_options::bool_switch(), "do not compare bringup results to properties" )
        ("host,o", boost::program_options::value<std::string>()->default_value(""), "mc_server's listening host:port." )
        ;


    // add properties options
    _propertiesOptions.addTo( _options );

    // add logging options
    _loggingOptions.addTo( _options );

    // parse
    boost::program_options::command_line_parser cmd_line( _argc, _argv );
    cmd_line.options( _options );
    boost::program_options::store( cmd_line.run(), _variables_map );

    // notify variables_map that we are done processing options
    boost::program_options::notify( _variables_map );

    // check for help
    if ( _variables_map["help"].as<bool>() ) {
        std::cout << _options << std::endl;
        exit(EXIT_SUCCESS);
    }

    // check for version
    if ( _variables_map["version"].as<bool>() ) {
        std::string basename = boost::filesystem::basename( boost::filesystem::path(_argv[0]) );
        std::cout << "BG/Q " << basename << " " << bgq::utility::DriverName;
        std::cout << " (revision " << bgq::utility::Revision << ")" << std::endl;
        exit(EXIT_SUCCESS);
    }

    // open properties
    this->openProperties();

    Properties::setProperty(
            MMCS_PROCESS,
            boost::filesystem::basename(
                boost::filesystem::path( _argv[0])
                )
            );

    std::ostringstream version;
    version << bgq::utility::DriverName << " (revision " << bgq::utility::Revision << ")";
    version << " " << __DATE__ << " " << __TIME__;
    Properties::setProperty(
            MMCS_VERSION,
            version.str()
            );
    if ( _variables_map["no-eof-exit"].as<bool>() ) {
        Properties::setProperty(
                FILE_EXIT,
                boost::lexical_cast<std::string>( _variables_map["no-eof-exit"].as<bool>() )
                );
    } else if ( !isatty(STDIN_FILENO) ) {
        // backwards compatibility for scripting environments
        Properties::setProperty(FILE_EXIT, "true");
    }

    Properties::init( _propertiesOptions.getFilename(), Properties::lite);
    _host_port = _variables_map["host"].as<std::string>();
    if(_host_port.length() != 0) {
        // Get host:port pair
        typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
        boost::char_separator<char> sep(":");
        tokenizer port_tok(_host_port, sep);
        tokenizer::iterator curr_tok = port_tok.begin();
        Properties::setProperty(MC_SERVER_IP, *curr_tok);
        ++curr_tok;
        if(curr_tok == port_tok.end()) {
            std::cerr << "invalid host:port specified" << std::endl;
            exit(EXIT_FAILURE);
        }
        Properties::setProperty(MC_SERVER_PORT, *curr_tok);
    }

    // log each arg
    std::ostringstream args;
    std::copy(_argv, _argv + _argc, std::ostream_iterator<char*>(args, " "));
    LOG_DEBUG_MSG("startup parameters: " << args.str());
}

void
Options::openProperties()
{
    bgq::utility::Properties::Ptr properties;
    try {
        if ( !_propertiesOptions.getFilename().empty() ) {
            properties = bgq::utility::Properties::create( _propertiesOptions.getFilename() );
        } else {
            properties = bgq::utility::Properties::create();
        }

        // setup logging
        bgq::utility::initializeLogging( *properties, _loggingOptions );
    } catch (bgq::utility::Properties::FileError& e) {
        std::cerr << e.what() << std::endl;
    } catch (bgq::utility::Properties::DuplicateKey& e) {
        std::cerr << e.what() << std::endl;
    } catch (bgq::utility::Properties::DuplicateSection& e) {
        std::cerr <<  e.what() << std::endl;
    } catch (bgq::utility::Properties::MalformedKey& e) {
        std::cerr << e.what() << std::endl;
    } catch (bgq::utility::Properties::MalformedSection& e) {
        std::cerr << e.what() << std::endl;
    } catch (bgq::utility::Properties::MissingSection& e) {
        std::cerr << e.what() << std::endl;
    }
}

} } // namespace mmcs::lite
