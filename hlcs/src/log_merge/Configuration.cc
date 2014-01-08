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


#include "Configuration.hpp"

#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/Properties.h>

#include <boost/assign.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <boost/throw_exception.hpp>

#include <boost/algorithm/string.hpp>

#include <boost/xpressive/xpressive.hpp>

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>

#include <unistd.h>


using bgq::utility::Properties;

using boost::bind;

using boost::gregorian::date;

using boost::posix_time::ptime;
using boost::posix_time::time_duration;

using std::string;
using std::istringstream;


LOG_DECLARE_FILE( "log_merge" );


namespace log_merge {


const std::string Configuration::DefaultBgqLogsDirectory( "/bgsys/logs/BGQ" );


const Configuration::Paths Configuration::DefaultPaths = boost::assign::list_of( DefaultBgqLogsDirectory );


boost::posix_time::ptime Configuration::parseTimestamp( const std::string& s, bool is_start )
{
    namespace xp = boost::xpressive;

    static const time_duration midnight_morning( 0, 0, 0, 0 );
    static const time_duration midnight_tomorrow( 24, 0, 0, 0 );

    const time_duration &day_input_duration(is_start ? midnight_morning : midnight_tomorrow);

    try {
        { // Look for a string like YYYY-mm-dd

            static const xp::sregex rex(xp::bos >> xp::repeat<4>(xp::_d) >> '-' >> xp::repeat<2>(xp::_d) >> '-' >> xp::repeat<2>(xp::_d) >> xp::eos);

            if ( xp::regex_match( s, rex ) ) {

                date d;
                istringstream ss( s );

                ss.imbue( std::locale( ss.getloc(), new boost::gregorian::date_input_facet( "%Y-%m-%d" ) ) );

                ss >> d;

                // if read the date and the date is the only thing in the string then use this value.
                if ( ss.good() && (ss.peek() == istringstream::traits_type::eof()) ) {
                    return ptime( d, day_input_duration );
                }

            }
        }

        { // Look for a string like mm-dd

            static const xp::sregex rex(xp::bos >> xp::repeat<2>(xp::_d) >> '-' >> xp::repeat<2>(xp::_d) >> xp::eos);

            if ( xp::regex_match( s, rex ) ) {

                date d;
                istringstream ss( s );

                ss.imbue( std::locale( ss.getloc(), new boost::gregorian::date_input_facet( "%m-%d" ) ) );

                ss >> d;

                // if read the date and the date is the only thing in the string then use this value.
                if ( ss.good() && (ss.peek() == istringstream::traits_type::eof()) ) {

                    ptime ret(
                            date( boost::gregorian::day_clock::local_day().year(), d.month(), d.day() ),
                            day_input_duration
                        );

                    // std::cout << "parsed mm-dd '" << s << "' -> " << ret << "\n";

                    return ret;
                }

            }
        }

        { // Look for a string like HH:MM:SS.sss

            static const xp::sregex rex(xp::bos >> xp::repeat<2>(xp::_d) >> !(':' >> xp::repeat<2>(xp::_d) >> !(':' >> xp::repeat<2>(xp::_d) >> !('.' >> xp::repeat<3>(xp::_d)))) >> xp::eos);

            if ( xp::regex_match( s, rex ) ) {
                time_duration t;

                istringstream ss( s );

                ss.imbue( std::locale( ss.getloc(), new boost::posix_time::time_input_facet( "%H:%M:%S%F" ) ) );

                ss >> t;

                // if read the time and the time is the only thing in the string then use this value.
                if ( ss.good() && (ss.peek() == istringstream::traits_type::eof()) ) {
                    ptime ret( boost::gregorian::day_clock::local_day(), t );
                    return ret;
                }
            }
        }

        boost::posix_time::ptime ret(boost::posix_time::time_from_string( s ));

        return ret;

    } catch ( std::exception& e ) {
        BOOST_THROW_EXCEPTION( std::runtime_error( string() + "invalid timestamp string '" + s + "'" ) );
        return boost::posix_time::ptime();
    }
}


Configuration::Configuration() :
        _is_p(&std::cin),
        _os_p(&std::cout),
        _help_requested(false),
        _read_null_terminated(false),
        _map_filenames(MapFilenames::Disabled),
        _is_live(false)
{
    // Nothing to do.
}


void Configuration::setStartTimeStr( const std::string& s )
{
    ptime t(parseTimestamp( s, true ));

    // std::cout << "parsed start time '" << s << "' -> " << t << "\n";

    setStartTime( t );
}


void Configuration::setEndTimeStr( const std::string& s )
{
    ptime t(parseTimestamp( s, false ));

    // std::cout << "parsed end time '" << s << "' -> " << t << "\n";

    setEndTime( t );
}


void Configuration::configure( const Args& args )
{
    namespace po = boost::program_options;

    Args args2( args.begin() + 1, args.end() );

    po::options_description visible_options("Allowed options");

    visible_options.add_options()
        ( "live", po::bool_switch()->notifier( bind( &Configuration::setLive, this, _1 ) ), "Live updates" )
        ( "map,m", po::bool_switch()->notifier( bind( &Configuration::setMapFilenamesBool, this, _1 ) ), "Display short names for log files" )
        ( "start", po::value<string>()->notifier( bind( &Configuration::setStartTimeStr, this, _1 ) ), "Interval start time" )
        ( "end", po::value<string>()->notifier( bind( &Configuration::setEndTimeStr, this, _1 ) ), "Interval end time" )
        ( "null,0", po::bool_switch( &_read_null_terminated ), "Read null-terminated file names from stdin" )
        ;

    Properties::ProgramOptions properties_program_options;

    properties_program_options.addTo( visible_options );

    const string default_logger( "ibm.log_merge" );
    bgq::utility::LoggingProgramOptions logging_program_options( default_logger );

    logging_program_options.addTo( visible_options );

    visible_options.add_options()
        ( "help,h", po::bool_switch( &_help_requested ), "Print this help text" )
        ;

    po::options_description all_options( "all" );

    all_options.add_options()
        ( "path,p", po::value<Paths>()->notifier( bind( &Configuration::setPaths, this, _1 ) ), "Files or directories to merge" )
        ;

    all_options.add( visible_options );


    po::positional_options_description p;
    p.add( "path", -1 );

    po::variables_map vm;

    try {
        po::store(
                po::command_line_parser( args2 )
                    .options( all_options )
                    .positional( p )
                    .run(),
                vm
            );
        po::notify( vm );
    } catch ( std::exception& e ) {

        BOOST_THROW_EXCEPTION( std::runtime_error( string() +

"failed to parse arguments, " + e.what() + "\n"
"Try '" + args[0] + " -h' for more information.\n"

            ) );
    }

    if ( _help_requested ) {

        (*_os_p) <<
"Usage: " << args[0] << " [OPTION]... [FILE or DIRECTORY]...\n"
"Merge Blue Gene log files.\n"
"\n"
<< visible_options <<
"\n"
"Use a FILENAME of - to read file or directory names from stdin.\n"
"\n";

        return;
    }

    if ( _is_live && (vm.count( "start" ) != 0 || vm.count( "end" ) != 0) ) {
        BOOST_THROW_EXCEPTION( std::runtime_error( "the --live option cannot be used with --start or --end" ) );
    }

    _interval.validate(); // throws if fails to validate.

    Properties properties( properties_program_options.getFilename() );

    bgq::utility::initializeLogging(
            properties,
            logging_program_options
        );

    if ( _paths.empty() && ! _read_null_terminated ) {

        try {
            string directory_names_str(properties.getValue( "log_merge", "log_dir" ));

            if ( directory_names_str.empty() ) {
                LOG_DEBUG_MSG( "[log_merge].log_dir not found in properties file, using default" );
                _paths = DefaultPaths;
            } else {
                boost::algorithm::split( _paths, directory_names_str, boost::is_any_of( "," ) );
            }
        } catch ( std::exception& e ) {
            *_os_p << args[0] << ": failed to get the default directory from the config file, will use the default.\n";
            _paths = DefaultPaths;
        }

    } else if ( _read_null_terminated || (std::find( _paths.begin(), _paths.end(), "-" ) != _paths.end()) ) {
        Paths path_args;

        path_args.swap( _paths );

        bool read_paths(false);

        BOOST_FOREACH( const std::string path_str, path_args ) {
            if ( path_str == "-" ) {
                if ( ! read_paths ) {
                    _readPathsFromInput();
                    read_paths = true;
                }
                continue;
            }

            _paths.push_back( path_str );
        }

        if ( ! read_paths ) {
            _readPathsFromInput();
        }
    }
}


void Configuration::_readPathsFromInput()
{
    char line_terminator_char(_read_null_terminated ? '\0' : '\n');

    while ( true ) {
        string line;
        std::getline( *_is_p, line, line_terminator_char );

        if ( ! *_is_p ) {
            break;
        }

        _paths.push_back( line );
    }
}


} // namespace log_merge
