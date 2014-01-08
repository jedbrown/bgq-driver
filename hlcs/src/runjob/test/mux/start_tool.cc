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
#define BOOST_TEST_MODULE start_tool
#include <boost/test/unit_test.hpp>

#include "mux/commands/start_tool/Debugger.h"
#include "mux/commands/start_tool/Options.h"

#include "test/mux/Fixture.h"

#include "test/make_argv.h"

#include <ramdisk/include/services/JobctlMessages.h>

#include <boost/program_options/errors.hpp>

using namespace runjob::mux::commands;

extern volatile int MPIR_being_debugged;
extern char MPIR_executable_path[256];
extern char MPIR_server_arguments[1024];
extern char MPIR_server_arguments[1024];
extern MPIR_PROCDESC* MPIR_proctable;
extern int MPIR_proctable_size;

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );

BOOST_FIXTURE_TEST_SUITE( start_tool_test, Fixture )

BOOST_AUTO_TEST_CASE( missing_id_token )
{
    _argv = make_argv("--id", _argc);
    BOOST_REQUIRE_THROW(
            start_tool::Options(_argc, _argv),
            boost::program_options::invalid_command_line_syntax
            );
}

BOOST_AUTO_TEST_CASE( missing_id )
{
    _argv = make_argv("", _argc);
    const start_tool::Options options(_argc, _argv);
    BOOST_REQUIRE_THROW(
            options.validate(),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( missing_tool_token )
{
    _argv = make_argv("--id 123 --tool", _argc);
    BOOST_REQUIRE_THROW(
            start_tool::Options(_argc, _argv),
            boost::program_options::invalid_command_line_syntax
            );
}

BOOST_AUTO_TEST_CASE( missing_tool )
{
    _argv = make_argv("--id 123", _argc);
    const start_tool::Options options(_argc, _argv);
    BOOST_REQUIRE_THROW(
            options.validate(),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( tool_path )
{
    _argv = make_argv("--id 123 --tool hello", _argc);
    const start_tool::Options options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.toolDaemon().getExecutable(),
            "hello"
            );
}

BOOST_AUTO_TEST_CASE( tool_args )
{
    const char* argv[] = { "foo", "--id", "123", "--tool", "hello", "--args", "how are you doing" };
    const start_tool::Options options( 7, const_cast<char**>(argv) );
    BOOST_REQUIRE_EQUAL(
            options.toolDaemon().getExecutable(),
            "hello"
            );
    BOOST_REQUIRE_EQUAL(
            options.toolDaemon().getArguments().size(),
            4
            );
    BOOST_REQUIRE_EQUAL(
            options.toolDaemon().getArguments()[0],
            "how"
            );
    BOOST_REQUIRE_EQUAL(
            options.toolDaemon().getArguments()[1],
            "are"
            );
    BOOST_REQUIRE_EQUAL(
            options.toolDaemon().getArguments()[2],
            "you"
            );
    BOOST_REQUIRE_EQUAL(
            options.toolDaemon().getArguments()[3],
            "doing"
            );
}

BOOST_AUTO_TEST_CASE( tool_args_with_no_path )
{
    _argv = make_argv("--id 123 --args hello", _argc);
    BOOST_REQUIRE_THROW(
            start_tool::Options(_argc, _argv),
            boost::program_options::error
            );
}

BOOST_AUTO_TEST_CASE( tool_subset_with_no_path )
{
    _argv = make_argv("--id 123 --subset 0-$max", _argc);
    BOOST_REQUIRE_THROW(
            start_tool::Options(_argc, _argv),
            boost::program_options::error
            );
}

BOOST_AUTO_TEST_CASE( path_too_long )
{
    const std::string executable( "hello" );
    size_t size = executable.size();
    std::string args;
    for ( ; size < static_cast<unsigned>(bgcios::jobctl::MaxArgumentSize) - 2; ++size ) {
        args.append( boost::lexical_cast<std::string>(size % 10 ) );
    }
    _argv = make_argv( "--tool " + executable + " --args " + args, _argc );

    BOOST_REQUIRE_THROW(
            start_tool::Options(_argc, _argv),
            boost::program_options::error
            );
}

BOOST_AUTO_TEST_CASE( debugger_attach_then_detach )
{
    start_tool::Debugger foo;

    // attach
    MPIR_being_debugged = 1;
    BOOST_CHECK_EQUAL( foo.attached(), true );

    // detach
    MPIR_being_debugged = 0;
    BOOST_CHECK_EQUAL( foo.attached(), false );
}

BOOST_AUTO_TEST_CASE( debugger_server_path )
{
    start_tool::Debugger debugger;

    const std::string tool( "/bin/foo" );
    MPIR_being_debugged = 1;
    strcpy( MPIR_executable_path, tool.c_str() );
    BOOST_CHECK_EQUAL(
            debugger.tool().getExecutable(),
            tool
            );
    BOOST_CHECK_EQUAL(
            debugger.tool().getArguments().size(),
            0
            );

    BOOST_CHECK_EQUAL( debugger.attached(), true );
}

BOOST_AUTO_TEST_CASE( debugger_arguments_without_embedded_null )
{
    start_tool::Debugger debugger;
    
    const std::string tool( "/bin/foo" );
    const std::string args( "hello world how are you" );
    MPIR_being_debugged = 1;
    strcpy( MPIR_executable_path, tool.c_str() );
    strcpy( MPIR_server_arguments, args.c_str() );
    BOOST_CHECK_EQUAL(
            debugger.tool().getExecutable(),
            tool
            );
    BOOST_CHECK_EQUAL(
            debugger.tool().getArguments().size(),
            1u
            );
    BOOST_CHECK_EQUAL(
            debugger.tool().getArguments().at(0),
            args
            );
}

BOOST_AUTO_TEST_CASE( debugger_arguments_with_embedded_null )
{
    start_tool::Debugger debugger;

    const std::string tool( "/bin/foo" );
    memset( MPIR_server_arguments, 0, sizeof(MPIR_server_arguments) );
    char* args = MPIR_server_arguments;
    int offset = sprintf( args, "hello" );
    args += offset + 1;
    offset += sprintf( args, "world" );

    MPIR_being_debugged = 1;
    strcpy( MPIR_executable_path, tool.c_str() );
    BOOST_CHECK_EQUAL(
            debugger.tool().getExecutable(),
            tool
            );
    BOOST_CHECK_EQUAL(
            debugger.tool().getArguments().size(),
            2u
            );
    BOOST_CHECK_EQUAL(
            debugger.tool().getArguments().at(0),
            "hello"
            );
    BOOST_CHECK_EQUAL(
            debugger.tool().getArguments().at(1),
            "world"
            );
}

BOOST_AUTO_TEST_CASE( debugger_proctable )
{
    start_tool::Debugger debugger;

    const runjob::message::Proctable::Ptr proctable(
            new runjob::message::Proctable
            );

    const runjob::Uci io_one( "R00-IC-J00" );
    const runjob::Uci io_two( "R00-IC-J01" );
    BOOST_CHECK(
            proctable->_io.insert(
                std::make_pair( io_one, "127.0.0.1" )
                ).second == true
            );
    BOOST_CHECK(
            proctable->_io.insert(
                std::make_pair( io_two, "127.0.0.1" )
                ).second == true
            );

    // add four ranks to proctable, two for each I/O node
    unsigned pid = 0;
    proctable->_proctable.push_back(
            runjob::tool::Rank( io_one, 0, ++pid )
            );
    proctable->_proctable.push_back(
            runjob::tool::Rank( io_one, 1, ++pid )
            );
    proctable->_proctable.push_back(
            runjob::tool::Rank( io_two, 2, ++pid )
            );
    proctable->_proctable.push_back(
            runjob::tool::Rank( io_two, 3, ++pid )
            );

    BOOST_CHECK_EQUAL(
            proctable->_proctable.size(), 4u
            );

    const std::string executable( "/bin/false" );

    debugger.fillProctable(
            executable,
            proctable
            );

    BOOST_CHECK_EQUAL(
            MPIR_proctable_size, 4u
            );

    for ( int i = 0; i < MPIR_proctable_size; ++i ) {
        MPIR_PROCDESC* p = &MPIR_proctable[i];
        BOOST_CHECK(
                !strcmp( p->host_name, "127.0.0.1" )
                );
        BOOST_CHECK_EQUAL(
                p->executable_name, executable
                );

        // pid's should be unique per I/O node
        switch( i ) {
            case 0:
                BOOST_CHECK_EQUAL( p->pid, 0 ); break;
            case 1:
                BOOST_CHECK_EQUAL( p->pid, 1 ); break;
            case 2:
                BOOST_CHECK_EQUAL( p->pid, 0 ); break;
            case 3:
                BOOST_CHECK_EQUAL( p->pid, 1 ); break;
            default:
                BOOST_CHECK_EQUAL( false, true ); break;
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
