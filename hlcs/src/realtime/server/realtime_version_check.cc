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
/* (C) Copyright IBM Corp.  2011, 2012                              */
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
#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/Properties.h>

#include <boost/foreach.hpp>
#include <boost/scope_exit.hpp>
#include <boost/regex.hpp>

#include <iostream>
#include <cstdio>

#include <unistd.h>

LOG_DECLARE_FILE( "realtime.server" );

int
main(int argc, char** argv)
{
    std::string db2path;
    std::string realtimepath;
    namespace po = boost::program_options;
    po::options_description options;
    options.add_options()
        ("help,h", po::bool_switch(), "this help text")
        ("db2-path", po::value(&db2path)->default_value(DB2PATH), "path to db2 installation")
        ("realtime-path", po::value(&realtimepath)->default_value(REALTIME_PATH), "path to realtime_server")
        ;

    // add properties and verbose options
    bgq::utility::Properties::ProgramOptions propertiesOptions;
    propertiesOptions.addTo( options );
    bgq::utility::LoggingProgramOptions lpo( "ibm.realtime" );
    lpo.addTo( options );

    // parse
    po::variables_map vm;
    po::command_line_parser cmd_line( argc, argv );
    cmd_line.allow_unregistered();
    cmd_line.options( options );
    const po::parsed_options parsed( cmd_line.run() );
    try {
        po::store( parsed, vm );

        // notify variables_map that we are done processing options
        po::notify( vm );
    } catch ( const std::exception& e ) {
        std::cerr << e.what() << std::endl;
        exit( EXIT_FAILURE );
    }

    if ( vm["help"].as<bool>() ) {
        std::cout << argv[0] << std::endl;
        std::cout << std::endl;
        std::cout << "OPTIONS:" << std::endl;
        std::cout << options << std::endl;
        exit(EXIT_SUCCESS);
    }

    try {
        using namespace bgq::utility;
        const Properties::Ptr properties( Properties::create(propertiesOptions.getFilename()) );

        // setup logging
        initializeLogging( *properties, lpo );
    } catch ( const std::runtime_error& e ) {
        std::cerr << e.what();
        exit( EXIT_FAILURE );
    }

    const std::string command( db2path + "/db2level" );
    LOG_DEBUG_MSG( "opening " << command );
    FILE* const db2level = popen( command.c_str(), "re" );
    if ( !db2level ) {
        std::cerr << "could not popen db2level: " << strerror(errno) << std::endl;
        exit( EXIT_FAILURE );
    }

    std::string realtime_version( "bg_realtime_server97" ); // default to 9.7 version
    while ( 1 ) {
        char* line = NULL;
        size_t len = 0;
        const ssize_t rc = getline( &line, &len, db2level );
        if ( rc != -1 && line ) {
            BOOST_SCOPE_EXIT( (line) )
            {
                free(line);
            } BOOST_SCOPE_EXIT_END
            boost::smatch match;
            const boost::regex pattern_97( "\"SQL(0907\\d)\"" );
            const boost::regex pattern_101( "\"SQL(1001\\d)\"" );

            if ( boost::regex_search(std::string(line), match, pattern_97) ) {
                LOG_DEBUG_MSG( "db2 level " << match[1] );
                break;
            } else if ( boost::regex_search(std::string(line), match, pattern_101) ) {
                LOG_DEBUG_MSG( "db2 level " << match[1] );
                realtime_version = "bg_realtime_server101";
                break;
            } else {
                LOG_DEBUG_MSG( line );
            }
        } else if ( errno ) {
            std::cerr << "Could not read db2level: " << strerror(errno) << std::endl;
            exit( EXIT_FAILURE );
        } else {
            break;
        }
    }

    const int rc = pclose( db2level );
    if ( WIFEXITED(rc) && WEXITSTATUS(rc) ) {
        std::cerr << "Could not determine db2 level: " << WEXITSTATUS(rc) << std::endl;
        exit( EXIT_FAILURE );
    } else if ( WIFSIGNALED(rc) ) {
        std::cerr << "Could not determine db2 level, terminated by signal " << WTERMSIG(rc) << std::endl;
        exit( EXIT_FAILURE );
    }

    LOG_DEBUG_MSG( "normal termination");

    std::vector<std::string> server_args(
            po::collect_unrecognized( parsed.options, po::include_positional )
            );

    // forward --properties argument
    if ( !propertiesOptions.getFilename().empty() ) {
        server_args.push_back( "--properties" );
        server_args.push_back( propertiesOptions.getFilename() );
    }

    // forward --verbose arguments
    for (
            bgq::utility::LoggingProgramOptions::LoggingLevels::const_iterator i( lpo.get().begin() );
            i != lpo.get().end();
            ++i
        )
    {
        server_args.push_back( "--verbose" );
        server_args.push_back( i->first + "=" + i->second->toString() );
    }
    
    BOOST_FOREACH( const auto& i, server_args ) {
        LOG_DEBUG_MSG( i );
    }


    std::vector<const char*> args;
    args.push_back( realtime_version.c_str() );
    LOG_DEBUG_MSG( server_args.size() << " arguments for real-time server" );
    BOOST_FOREACH( const auto& i, server_args ) {
        args.push_back( i.c_str() );
    }
    args.push_back( NULL );
    BOOST_FOREACH( const char* arg, args ) {
        if ( arg ) LOG_DEBUG_MSG( arg );
    }
    realtimepath.append( "/" + realtime_version );
    LOG_DEBUG_MSG( "path: " << realtimepath );
    execv( realtimepath.c_str(), const_cast<char**>(&args[0]) );
    std::cerr << "exec(" << realtimepath << "," << args[0] << ") failed: " << strerror(errno) << std::endl;
    _exit(EXIT_FAILURE);
}
