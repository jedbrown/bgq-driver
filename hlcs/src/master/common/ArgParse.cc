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

#include "ArgParse.h"

#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>

#include <utility/include/portConfiguration/ClientPortConfiguration.h>

#include <boost/assign.hpp>

#include <iostream>

#include <stdlib.h>
#include <string.h>

LOG_DECLARE_FILE( "master" );

typedef std::map<std::string,log4cxx::LevelPtr> Loggers;
Loggers loggers = boost::assign::map_list_of
    ( "ibm.master", log4cxx::Level::getFatal() )
    ( "ibm.utility", log4cxx::Level::getFatal() )
    ( "ibm.utility.cxxsockets", log4cxx::Level::getFatal());

void
Args::setupLoggerDefaults() const
{
    for ( Loggers::const_iterator i = loggers.begin(); i != loggers.end(); ++i ) {
        const std::string& logger = i->first;
        const log4cxx::LevelPtr& level = i->second;
        if ( log4cxx::LoggerPtr log = log4cxx::Logger::getLogger( logger ) ) {
            log->setLevel( level );
        }
    }
}

bool
Args::setupLogger(
        const std::string& verbarg
        ) const
{
    // parse --verbose arguments
    // they are in the form --verbose logger=level or --verbose level
    const std::string::size_type split_pos( verbarg.find( '=' ) );
    std::string logger_name;
    if ( split_pos != std::string::npos ) {
        logger_name = verbarg.substr( 0, split_pos );
    } else {
        // Assume level for default logger
        logger_name = "ibm.master";
    }

    // Create a logger.
    const log4cxx::LoggerPtr loggerp = log4cxx::Logger::getLogger(logger_name);

    // Remove any loggers specified in --verbose from the default logger list
    const Loggers::iterator i = loggers.find( logger_name );
    if ( i != loggers.end() ) {
        loggers.erase( i );
    }

    // Get the specified log level
    std::string log_level_string = verbarg.substr(split_pos + 1,verbarg.length());
    if (log_level_string == "O" || log_level_string == "OFF" || log_level_string == "0") log_level_string = "OFF";
    else if (log_level_string == "F" || log_level_string == "FATAL" || log_level_string == "1") log_level_string = "FATAL";
    else if (log_level_string == "E" || log_level_string == "ERROR" || log_level_string == "2") log_level_string = "ERROR";
    else if (log_level_string == "W" || log_level_string == "WARN" || log_level_string == "3") log_level_string = "WARN";
    else if (log_level_string == "I" || log_level_string == "INFO" || log_level_string == "4") log_level_string = "INFO";
    else if (log_level_string == "D" || log_level_string == "DEBUG" || log_level_string == "5") log_level_string = "DEBUG";
    else if (log_level_string == "T" || log_level_string == "TRACE" || log_level_string == "6") log_level_string = "TRACE";
    else if (log_level_string == "A" || log_level_string == "ALL" || log_level_string == "7") log_level_string = "ALL";
    else return false;

    const log4cxx::LevelPtr levelp = log4cxx::Level::toLevel(log_level_string);
    loggerp->setLevel(levelp);

    return true;
}

void
APusage(
        void (*usage)(),
        const bool silent = false
        )
{
    usage();
    if ( silent ) return;

    std::cerr << "Try the --help or -h option for more information." << std::endl;
}

Args::Args(
        const int argc, 
        const char** argv,
        void (*usage)(), 
        void (*help)(),
        std::vector<std::string>& valargs,
        const std::vector<std::string>& singles
        )
{
    valargs.push_back("--verbose");
    valargs.push_back("-v");
    valargs.push_back("--properties");
    valargs.push_back("-p");

    // First find the properties
    bool gotprops = false;
    std::string host_string;
    for (int i = 1; i < argc; ++i) {
        if (!strcasecmp(argv[i], "--properties") || !strcasecmp(argv[i], "-p")) {
            if (_props) {
                std::cerr << "--properties already specified." << std::endl;
                exit(EXIT_FAILURE);
            } else if (argc == ++i) {
                std::cerr << "Please give a file name after " << argv[i-1] << std::endl;
                exit(EXIT_FAILURE);
            }
            try {
                _props = bgq::utility::Properties::create(argv[i]);
            } catch (const std::runtime_error& e) {
                std::cerr << "Error reading properties: " << e.what() << std::endl;
                exit(EXIT_FAILURE);
            }
            gotprops = true;
        }
        if (!strcasecmp(argv[i], "--host") || !strcmp(argv[i], "-H")) {
            // Got a host parameter
            if (++i < argc) {
                host_string = argv[i];
                if (host_string.find(":") == std::string::npos) {
                    host_string = host_string + ":32042";
                }
                if (argc == i || host_string.find("--") != std::string::npos) {
                    std::cerr << "Must provide host/port pairs for --host parameter" << std::endl;
                    exit(EXIT_FAILURE);
                }
                valargs.push_back("--host");
                valargs.push_back("-H");
            } else {
                std::cerr << "Missing host/port pair." << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }

    if (!gotprops) {
        try {
            _props = bgq::utility::Properties::create();
        } catch (const std::runtime_error& e) {
            std::cerr << "Properties file error: " << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    const std::string default_logger("ibm.master");

    const bgq::utility::LoggingProgramOptions logging_program_options( default_logger );
    bgq::utility::initializeLogging(*_props, logging_program_options, "master");
    setupLoggerDefaults();

    // Needs to get master location from properties and command line
    bgq::utility::ClientPortConfiguration port_config(
            32042, bgq::utility::ClientPortConfiguration::ConnectionType::Command
            );
    port_config.setProperties( _props, "master.client");
    port_config.notifyComplete();

    if (!host_string.empty()) {
        bgq::utility::PortConfiguration::parsePortsStr(host_string, "32042", _portpairs);
    } else {
        _portpairs = port_config.getPairs();
    }

    if (_portpairs[0].first.empty()) {
        std::cerr << "No port pairs or invalid port pairs specified. Using defaults." << std::endl;
        bgq::utility::PortConfiguration::parsePortsStr("127.0.0.1:32042", "32042", _portpairs);
    }

    LOG_DEBUG_MSG("Using properties file " << _props->getFilename())

    unsigned optional_count = 0;
    for (std::vector<std::string>::const_iterator it = valargs.begin(); it != valargs.end(); ++it) {
        // Count up the "wild" args.
        if (*it == "*") {
            ++optional_count;
        } else if (*it == "**") {
            optional_count = std::numeric_limits<unsigned>::max();
            break;
        }
    }

    for (int i = 1; i < argc; ++i) {
        if (!strcasecmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
            help();
            APusage(usage, true);
            exit(0);
        }

        // See if the arg is in the pairs vector
        const std::string curr_arg = argv[i];

        if (std::find(valargs.begin(), valargs.end(), curr_arg) != valargs.end()) {
            // If it's in the vector, make sure there's a value associated with it
            if (i + 1 == argc) {
                std::cerr << "Missing value for " << curr_arg << std::endl;
                APusage(usage);
                exit(EXIT_FAILURE);
            }

            const std::string nextval = argv[i + 1];
            if (nextval.length() != 0 && nextval.find("--") == std::string::npos) {
                if (curr_arg == "--verbose" || curr_arg == "-v") {
                    const std::string verbarg = argv[++i];
                    if (setupLogger(verbarg) == false) {
                        std::cerr << "Invalid verbose option \'" << verbarg << "\'" << std::endl;
                        APusage(usage);
                        exit(EXIT_FAILURE);
                    }
                } else {
                    _argpairs[curr_arg] = argv[++i];
                }
            } else {
                std::cerr << "Invalid option \'" << curr_arg << "\'" << std::endl;
                APusage(usage);
                exit(EXIT_FAILURE);
            }
        } else {
            // It's not in the pairs vector, see if it's a wildcard or a single.
            if (curr_arg.find("--") != std::string::npos && curr_arg.find("-") != std::string::npos) {
                if (std::find(singles.begin(), singles.end(), curr_arg) != singles.end()) {
                    _otherargs.push_back(curr_arg);
                } else {
                    // It's not in the pairs list or the singles list, it isn't any good.
                    std::cerr << "Invalid option \'" << curr_arg << "\'" << std::endl;
                    APusage(usage);
                    exit(EXIT_FAILURE);
                }
            } else if (optional_count > 0) {
                _otherargs.push_back(curr_arg);
                --optional_count;
            } else {
                std::cerr << "Invalid option \'" << curr_arg << "\'" << std::endl;
                APusage(usage);
                exit(EXIT_FAILURE);
            }
        }
    }
}

std::string
Args::operator[](
        const std::string& value
        ) const
{
    try {
        return _argpairs.at(value);
    } catch ( const std::exception& e ) {
        LOG_TRACE_MSG( e.what() );
        return std::string();
    }
}
