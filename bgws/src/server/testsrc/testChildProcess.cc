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


#include "utility/ChildProcesses.hpp"
#include "utility/ChildProcess.hpp"
#include "utility/EasyChildProcess.hpp"

#include <utility/include/Log.h>
#include <utility/include/Properties.h>
#include <utility/include/SignalHandler.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <iostream>

#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE ChildProcess
#include <boost/test/unit_test.hpp>


using bgq::utility::ExitStatus;

using bgws::utility::ChildProcess;
using bgws::utility::ChildProcesses;
using bgws::utility::EasyChildProcess;

using std::string;


LOG_DECLARE_FILE( "bgws" );


static boost::shared_ptr<boost::asio::io_service> s_io_service_ptr;
static boost::shared_ptr<ChildProcesses> s_child_processes_ptr;
static boost::shared_ptr<bgq::utility::SignalHandler<SIGCHLD> > s_sig_handler_ptr;


static void startWaitForSignal();


static void handleSignal(
        const boost::system::error_code& ec,
        const siginfo_t& sig_info
    )
{
    if ( ec ) {
        LOG_WARN_MSG( "Signal handler got error " << ec );
        return;
    }

    if ( sig_info.si_signo != SIGCHLD ) {
        LOG_WARN_MSG( "Wasn't expecting signal " << sig_info.si_signo );

        // Start up the loop again.
        startWaitForSignal();
        return;
    }

    s_child_processes_ptr->notifySigChld();
}


static void startWaitForSignal()
{
    s_sig_handler_ptr->async_wait(
            boost::bind( &handleSignal, _1, _2 )
        );
}


static void timeout( const boost::system::error_code& e, boost::shared_ptr<boost::asio::deadline_timer> /*timer_ptr*/ )
{
    if ( e ) {

        if ( e == boost::asio::error::operation_aborted ) {
            // Ignore this.
            return;
        }

        std::cerr << "Timeout error\n" << e;
    } else {
        std::cerr << "Timed out.\n";
    }

    s_io_service_ptr->stop();
}


static void gatherOutput( EasyChildProcess::OutputType /*out_type*/, EasyChildProcess::OutputIndicator ind, const std::string& line, std::string& output_in_out )
{
    if ( ind == EasyChildProcess::OutputIndicator::Normal ) {
        output_in_out += line + "\n";
    }
}


static void handleEnded( const ExitStatus& exit_status, ExitStatus& exit_status_out, boost::asio::deadline_timer& timer )
{
    timer.cancel();
    exit_status_out = exit_status;
}


static int initialize_()
{
    bgq::utility::initializeLogging( *(bgq::utility::Properties::create( "" )) );

    s_io_service_ptr.reset( new boost::asio::io_service() );
    s_child_processes_ptr.reset( new ChildProcesses( *s_io_service_ptr ) );
    s_sig_handler_ptr.reset( new bgq::utility::SignalHandler<SIGCHLD>( *s_io_service_ptr ) );

    return 0;
}

static int s_initialized_(initialize_());


static void testExitStatusSuccess( ExitStatus& exit_status_out )
{
    boost::shared_ptr<boost::asio::deadline_timer> timer_ptr( new boost::asio::deadline_timer( *s_io_service_ptr, boost::posix_time::seconds(5) ) );
    timer_ptr->async_wait( boost::bind( &timeout, _1, timer_ptr ) );

    ChildProcess::Ptr process_ptr(s_child_processes_ptr->create( "testExitStatusSuccess", "/bin/true" ));
    process_ptr->start(
            boost::bind( &handleEnded, _1, boost::ref(exit_status_out), boost::ref(*timer_ptr) )
        );
}

BOOST_AUTO_TEST_CASE( test_exit_status_success )
{
    // Can run /bin/true and get the exit status, 0.

    ExitStatus exit_status;

    s_io_service_ptr->post( boost::bind( &testExitStatusSuccess, boost::ref(exit_status) ) );

    startWaitForSignal();

    s_io_service_ptr->reset();
    s_io_service_ptr->run();

    BOOST_CHECK_EQUAL( exit_status, ExitStatus::Normal );
}


static void testExitStatusFailed( ExitStatus& exit_status_out )
{
    boost::shared_ptr<boost::asio::deadline_timer> timer_ptr( new boost::asio::deadline_timer( *s_io_service_ptr, boost::posix_time::seconds(5) ) );
    timer_ptr->async_wait( boost::bind( &timeout, _1, timer_ptr ) );

    ChildProcess::Ptr process_ptr(s_child_processes_ptr->create( "testExitStatusFailed", "/bin/false" ));
    process_ptr->start(
            boost::bind( &handleEnded, _1, boost::ref(exit_status_out), boost::ref(*timer_ptr) )
        );
}

BOOST_AUTO_TEST_CASE( test_exit_status_failed )
{
    // Can run /bin/false and get the exit status, 1.

    ExitStatus exit_status;

    s_io_service_ptr->post( boost::bind( &testExitStatusFailed, boost::ref(exit_status) ) );

    startWaitForSignal();

    s_io_service_ptr->reset();
    s_io_service_ptr->run();

    BOOST_REQUIRE( exit_status.exited() );
    BOOST_CHECK_EQUAL( exit_status.getExitStatus(), 1 );
}


static void testOtherExitStatus( ExitStatus& exit_status_out )
{
    boost::shared_ptr<boost::asio::deadline_timer> timer_ptr( new boost::asio::deadline_timer( *s_io_service_ptr, boost::posix_time::seconds(5) ) );
    timer_ptr->async_wait( boost::bind( &timeout, _1, timer_ptr ) );

    EasyChildProcess::Ptr process_ptr(s_child_processes_ptr->createEasy( "testOtherExitStatus", "/bin/bash" ));
    process_ptr->start(
            boost::bind( &handleEnded, _1, boost::ref(exit_status_out), boost::ref(*timer_ptr) ),
            string( "exit 50\n" )
        );
}

BOOST_AUTO_TEST_CASE( test_other_exit_status )
{
    // Can run bash with "exit 50", and get the exit status, 50.

    ExitStatus exit_status;

    s_io_service_ptr->post( boost::bind( &testOtherExitStatus, boost::ref(exit_status) ) );

    startWaitForSignal();

    s_io_service_ptr->reset();
    s_io_service_ptr->run();

    BOOST_REQUIRE( exit_status.exited() );
    BOOST_CHECK_EQUAL( exit_status.getExitStatus(), 50 );
}


static void testEasyOutput( ExitStatus& exit_status_out, std::string& output_out )
{
    boost::shared_ptr<boost::asio::deadline_timer> timer_ptr( new boost::asio::deadline_timer( *s_io_service_ptr, boost::posix_time::seconds(5) ) );
    timer_ptr->async_wait( boost::bind( &timeout, _1, timer_ptr ) );

    ChildProcess::Args args = { "testEasyOutputLine" };

    EasyChildProcess::Ptr process_ptr(s_child_processes_ptr->createEasy( "testEasyOutput", "/bin/echo", args ) );
    process_ptr->start(
            boost::bind( &handleEnded, _1, boost::ref(exit_status_out), boost::ref(*timer_ptr) ),
            boost::bind( &gatherOutput, _1, _2, _3, boost::ref(output_out) )
        );
}

BOOST_AUTO_TEST_CASE( test_easy_output )
{
    // Can get the output from the program you're running.

    ExitStatus exit_status;
    string output;

    s_io_service_ptr->post( boost::bind( &testEasyOutput, boost::ref(exit_status), boost::ref(output) ) );

    startWaitForSignal();

    s_io_service_ptr->reset();
    s_io_service_ptr->run();

    BOOST_CHECK_EQUAL( exit_status, ExitStatus::Normal );
    BOOST_CHECK_EQUAL( output, "testEasyOutputLine\n" );
}


static void testEasyInput( ExitStatus& exit_status_out, std::string& output_out )
{
    boost::shared_ptr<boost::asio::deadline_timer> timer_ptr( new boost::asio::deadline_timer( *s_io_service_ptr, boost::posix_time::seconds(5) ) );
    timer_ptr->async_wait( boost::bind( &timeout, _1, timer_ptr ) );

    EasyChildProcess::Ptr process_ptr(s_child_processes_ptr->createEasy( "testEasyInput", "/bin/cat" ) );
    process_ptr->start(
            boost::bind( &handleEnded, _1, boost::ref(exit_status_out), boost::ref(*timer_ptr) ),
            boost::bind( &gatherOutput, _1, _2, _3, boost::ref(output_out) ),
            "testEasyInput\n"
        );
}

BOOST_AUTO_TEST_CASE( test_easy_input )
{
    // Can send input to a program.

    ExitStatus exit_status;
    string output;

    s_io_service_ptr->post( boost::bind( &testEasyInput, boost::ref(exit_status), boost::ref(output) ) );

    startWaitForSignal();

    s_io_service_ptr->reset();
    s_io_service_ptr->run();

    BOOST_CHECK_EQUAL( exit_status, ExitStatus::Normal );
    BOOST_CHECK_EQUAL( output, "testEasyInput\n" );
}


static void testNoFile( ExitStatus& exit_status_out )
{
    boost::shared_ptr<boost::asio::deadline_timer> timer_ptr( new boost::asio::deadline_timer( *s_io_service_ptr, boost::posix_time::seconds(5) ) );
    timer_ptr->async_wait( boost::bind( &timeout, _1, timer_ptr ) );

    EasyChildProcess::Ptr process_ptr(s_child_processes_ptr->createEasy( "testNoFile", "/nofile" ) );
    process_ptr->start(
            boost::bind( &handleEnded, _1, boost::ref(exit_status_out), boost::ref(*timer_ptr) )
        );
}

BOOST_AUTO_TEST_CASE( test_no_file )
{
    // If try to execute a program and it doesn't exist then fails with exit status 127.

    ExitStatus exit_status;

    s_io_service_ptr->post( boost::bind( &testNoFile, boost::ref(exit_status) ) );

    startWaitForSignal();

    s_io_service_ptr->reset();
    s_io_service_ptr->run();

    BOOST_REQUIRE( exit_status.exited() );
    BOOST_CHECK_EQUAL( exit_status.getExitStatus(), 127 );
}


static void testNotExecutable( ExitStatus& exit_status_out )
{
    boost::shared_ptr<boost::asio::deadline_timer> timer_ptr( new boost::asio::deadline_timer( *s_io_service_ptr, boost::posix_time::seconds(5) ) );
    timer_ptr->async_wait( boost::bind( &timeout, _1, timer_ptr ) );

    EasyChildProcess::Ptr process_ptr(s_child_processes_ptr->createEasy( "testNotExecutable", "/etc/passwd" ) );
    process_ptr->start(
            boost::bind( &handleEnded, _1, boost::ref(exit_status_out), boost::ref(*timer_ptr) )
        );
}

BOOST_AUTO_TEST_CASE( test_not_executable )
{
    // If try to execute a program and it doesn't exist then fails with exit status 127.

    ExitStatus exit_status;

    s_io_service_ptr->post( boost::bind( &testNotExecutable, boost::ref(exit_status) ) );

    startWaitForSignal();

    s_io_service_ptr->reset();
    s_io_service_ptr->run();

    BOOST_REQUIRE( exit_status.exited() );
    BOOST_CHECK_EQUAL( exit_status.getExitStatus(), 126 );
}


static void setCwdBin()
{
    chdir( "/bin" );
}

static void testPreExecFn( ExitStatus& exit_status_out )
{
    boost::shared_ptr<boost::asio::deadline_timer> timer_ptr( new boost::asio::deadline_timer( *s_io_service_ptr, boost::posix_time::seconds(5) ) );
    timer_ptr->async_wait( boost::bind( &timeout, _1, timer_ptr ) );

    EasyChildProcess::Ptr process_ptr(s_child_processes_ptr->createEasy( "testPreExecFn", "./true" ) );
    process_ptr->setPreExecFn( &setCwdBin );
    process_ptr->start(
            boost::bind( &handleEnded, _1, boost::ref(exit_status_out), boost::ref(*timer_ptr) )
        );
}

BOOST_AUTO_TEST_CASE( test_pre_exec_fn )
{
    // Can pass a pre-exec function that gets run before exec().

    ExitStatus exit_status;

    s_io_service_ptr->post( boost::bind( &testPreExecFn, boost::ref(exit_status) ) );

    startWaitForSignal();

    s_io_service_ptr->reset();
    s_io_service_ptr->run();

    BOOST_REQUIRE( exit_status.exited() );
    BOOST_CHECK_EQUAL( exit_status.getExitStatus(), 0 );
}
