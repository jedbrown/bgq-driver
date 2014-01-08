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
#include <utility/include/Exec.h>
#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/Properties.h>

#include <iostream>

#include <sys/wait.h>

#include <boost/assert.hpp>
#include <boost/program_options.hpp>

class PidScopeGuard
{
public:
    PidScopeGuard() : 
        _pid( getpid() )
    {

    }

    ~PidScopeGuard() {
        if ( _pid != getpid() ) {
            // something other than EXIT_FAILURE
            exit( 5 );
        }
    }

private:
    const pid_t _pid;
};

void
zero_exit_status()
{
    // path to some binary that will always exist
    const std::string path = "/bin/true";
    const bool managed = true;
    std::string error;
    int pipe = 0;
    const pid_t pid = Exec::fexec( pipe, path, error, managed );

    // both pid and pipe should be non-zero
    BOOST_ASSERT( pid != 0u );
    BOOST_ASSERT( pipe != 0u );

    int status;
    const int options = 0;
    waitpid( pid, &status, options );
    BOOST_ASSERT( WEXITSTATUS(status) == 0u );
}

void
bad_exe_path()
{
    const PidScopeGuard foo;
    (void)foo;

    // path to some binary that does not exist
    const std::string path = "/foo/bar/baz";
    const bool managed = true;
    std::string error;
    int pipe = 0;
    const pid_t pid = Exec::fexec( pipe, path, error, managed );
    BOOST_ASSERT( pid == -1 );
}

void
bad_log_file_path()
{
    // path to some binary that does exist
    const std::string path = "/bin/true";
    const bool managed = true;
    std::string error;
    int pipe = 0;
    const std::string logfile = "/proc/self";
    const pid_t pid = Exec::fexec( pipe, path, error, managed, logfile );

    BOOST_ASSERT( pid == -1 );
}

int
main( int argc, char** argv )
{
    namespace po = boost::program_options;
    po::options_description options;
    options.add_options()
        ("help,h", boost::program_options::bool_switch(), "this help text")
        ;

    bgq::utility::Properties::ProgramOptions propertiesOptions;
    propertiesOptions.addTo( options );
    bgq::utility::LoggingProgramOptions loggingOptions( "ibm.utility" );
    loggingOptions.addTo( options );

    po::variables_map vm;
    po::store( po::parse_command_line( argc, argv, options), vm );
    po::notify( vm );

    if ( vm["help"].as<bool>() ) {
        std::cout << options << std::endl;
        exit( EXIT_SUCCESS );
    }

    bgq::utility::Properties::Ptr properties;
    if ( !propertiesOptions.getFilename().empty() ) {
        properties = bgq::utility::Properties::create( propertiesOptions.getFilename() );
    } else {
        properties = bgq::utility::Properties::create();
    }

    bgq::utility::initializeLogging( *properties, loggingOptions, "ibm.utility" );

    zero_exit_status();

    bad_exe_path();
    
    bad_log_file_path();
}

