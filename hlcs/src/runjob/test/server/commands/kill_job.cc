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
#define BOOST_TEST_MODULE kill_job
#include <boost/test/unit_test.hpp>

#include "server/commands/kill_job/Options.h"

#include "test/server/commands/Fixture.h"

#include "test/make_argv.h"

#include <boost/program_options/errors.hpp>

#include <boost/foreach.hpp>

using namespace runjob::server::commands;

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );

BOOST_FIXTURE_TEST_SUITE( kill_job_test, Fixture )

BOOST_AUTO_TEST_CASE( list_short )
{
    _argv = make_argv("-l", _argc);
    boost::scoped_ptr<kill_job::Options> options;
    BOOST_REQUIRE_NO_THROW(
            options.reset(
                new kill_job::Options(_argc, _argv)
                )
            );
    
    BOOST_REQUIRE_EQUAL(
            options->getList(),
            true
            );
}

BOOST_AUTO_TEST_CASE( list_long )
{
    _argv = make_argv("--list", _argc);
    boost::scoped_ptr<kill_job::Options> options;
    BOOST_REQUIRE_NO_THROW(
            options.reset(
                new kill_job::Options(_argc, _argv)
                )
            );
    
    BOOST_REQUIRE_EQUAL(
            options->getList(),
            true
            );
}

BOOST_AUTO_TEST_CASE( missing_id )
{
    _argv = make_argv("--id", _argc);
    boost::scoped_ptr<kill_job::Options> options;
    BOOST_REQUIRE_THROW(
            options.reset(
                new kill_job::Options(_argc, _argv)
                ),
            boost::program_options::invalid_command_line_syntax
            );
}

BOOST_AUTO_TEST_CASE( non_numeric_id )
{
    _argv = make_argv("--id foo", _argc);
    boost::scoped_ptr<kill_job::Options> options;
    BOOST_REQUIRE_THROW(
            options.reset(
                new kill_job::Options(_argc, _argv)
                ),
            boost::program_options::invalid_option_value
            );
}

BOOST_AUTO_TEST_CASE( negative_id )
{
    _argv = make_argv("--id -15", _argc);
    boost::scoped_ptr<kill_job::Options> options;
    BOOST_REQUIRE_THROW(
            options.reset(
                new kill_job::Options(_argc, _argv)
                ),
            boost::program_options::invalid_option_value
            );
}

BOOST_AUTO_TEST_CASE( positive_id )
{
    _argv = make_argv("--id 15", _argc);
    boost::scoped_ptr<kill_job::Options> options;
    BOOST_REQUIRE_NO_THROW(
            options.reset(
                new kill_job::Options(_argc, _argv)
                )
            );
    BOOST_REQUIRE_EQUAL(
            options->getJobId(),
            15u
            );
}

BOOST_AUTO_TEST_CASE( positional_id )
{
    _argv = make_argv("15", _argc);
    boost::scoped_ptr<kill_job::Options> options;
    BOOST_REQUIRE_NO_THROW(
            options.reset(
                new kill_job::Options(_argc, _argv)
                )
            );
    BOOST_REQUIRE_EQUAL(
            options->getJobId(),
            15u
            );
}

BOOST_AUTO_TEST_CASE( bogus_string_signal )
{
    _argv = make_argv("-s ASDF --id 15", _argc);
    boost::scoped_ptr<kill_job::Options> options;
    BOOST_REQUIRE_NO_THROW(
            options.reset(
                new kill_job::Options(_argc, _argv)
                )
            );
    BOOST_REQUIRE_THROW(
            options->validate(),
            boost::program_options::invalid_option_value
            );
}

BOOST_AUTO_TEST_CASE( bogus_string_signal_special_arg )
{
    _argv = make_argv("-BOGUS --id 15", _argc);
    boost::scoped_ptr<kill_job::Options> options;
    BOOST_REQUIRE_THROW(
            options.reset(
                new kill_job::Options(_argc, _argv)
                ),
            boost::program_options::unknown_option
            );
}

BOOST_AUTO_TEST_CASE( bogus_numeric_signal_special_arg )
{
    _argv = make_argv("-123 --id 15", _argc);
    boost::scoped_ptr<kill_job::Options> options;
    BOOST_REQUIRE_NO_THROW(
            options.reset(
                new kill_job::Options(_argc, _argv)
                )
            );
    BOOST_REQUIRE_EQUAL(
            options->getSignal(),
            "123"
            );
}

BOOST_AUTO_TEST_CASE( good_string_signals )
{
    // test all valid signal strings
    BOOST_FOREACH( const kill_job::Options::Signal& signal, kill_job::Options::signals ) {
        std::ostringstream args;
        args << "--id 15 -s SIG" << boost::get<0>(signal);
        _argv = make_argv( args.str().c_str(), _argc);
        boost::scoped_ptr<kill_job::Options> options;
        BOOST_REQUIRE_NO_THROW(
                options.reset(
                    new kill_job::Options(_argc, _argv)
                    )
                );
        BOOST_REQUIRE_EQUAL(
                options->getSignal(),
                std::string("SIG") + boost::get<0>(signal)
                );
    }
}

BOOST_AUTO_TEST_CASE( good_string_signals_without_sig_prefix )
{
    // test all valid signal strings
    BOOST_FOREACH( const kill_job::Options::Signal& signal, kill_job::Options::signals ) {
        std::ostringstream args;
        args << "--id 15 -s " << boost::get<0>(signal);
        _argv = make_argv( args.str().c_str(), _argc);
        boost::scoped_ptr<kill_job::Options> options;
        BOOST_REQUIRE_NO_THROW(
                options.reset(
                    new kill_job::Options(_argc, _argv)
                    )
                );
        BOOST_REQUIRE_EQUAL(
                options->getSignal(),
                boost::get<0>(signal)
                );
    }
}

BOOST_AUTO_TEST_CASE( good_string_special_arg )
{
    // test all valid signal strings
    BOOST_FOREACH( const kill_job::Options::Signal& signal, kill_job::Options::signals ) {
        std::ostringstream args;
        args << "-" << boost::get<0>(signal) << " 123";
        _argv = make_argv( args.str().c_str(), _argc);
        boost::scoped_ptr<kill_job::Options> options;
        BOOST_REQUIRE_NO_THROW(
                options.reset(
                    new kill_job::Options(_argc, _argv)
                    )
                );
        BOOST_REQUIRE_EQUAL(
                options->getSignal(),
                boost::get<0>(signal)
                );
    }
}

BOOST_AUTO_TEST_CASE( good_string_special_arg_with_sig_prefix )
{
    // test all valid signal strings
    BOOST_FOREACH( const kill_job::Options::Signal& signal, kill_job::Options::signals ) {
        std::ostringstream args;
        args << "-SIG" << boost::get<0>(signal) << " 123";
        _argv = make_argv( args.str().c_str(), _argc);
        boost::scoped_ptr<kill_job::Options> options;
        BOOST_REQUIRE_NO_THROW(
                options.reset(
                    new kill_job::Options(_argc, _argv)
                    )
                );
        BOOST_REQUIRE_EQUAL(
                options->getSignal(),
                std::string("SIG") + boost::get<0>(signal)
                );
    }
}

BOOST_AUTO_TEST_CASE( numeric_signal_special_arg )
{
    // test all valid signal numbers
    BOOST_FOREACH( const kill_job::Options::Signal& signal, kill_job::Options::signals ) {
        std::ostringstream args;
        args << "-" << boost::get<1>(signal) << " 123";
        _argv = make_argv( args.str().c_str(), _argc);
        boost::scoped_ptr<kill_job::Options> options;
        BOOST_REQUIRE_NO_THROW(
                options.reset(
                    new kill_job::Options(_argc, _argv)
                    )
                );
        BOOST_REQUIRE_EQUAL(
                options->getSignal(),
                boost::lexical_cast<std::string>( boost::get<1>(signal) )
                );
    }
}

BOOST_AUTO_TEST_CASE( duplicate_signal )
{
    _argv = make_argv("-9 -INT -s STOP --signal HUP --id 123", _argc);
    boost::scoped_ptr<kill_job::Options> options;
    BOOST_REQUIRE_THROW(
            options.reset(
                new kill_job::Options(_argc, _argv)
                ),
            boost::program_options::multiple_occurrences
            );
}

BOOST_AUTO_TEST_CASE( duplicate_id )
{
    _argv = make_argv("-9 --id 123 321", _argc);
    boost::scoped_ptr<kill_job::Options> options;
    BOOST_REQUIRE_THROW(
            options.reset(
                new kill_job::Options(_argc, _argv)
                ),
            boost::program_options::multiple_occurrences
            );
}

BOOST_AUTO_TEST_CASE( positive_timeout )
{
    _argv = make_argv("123 --timeout 456", _argc);
    const kill_job::Options options( _argc, _argv );
    BOOST_REQUIRE_EQUAL(
            options.getTimeout(),
            456
            );
}

BOOST_AUTO_TEST_CASE( zero_timeout )
{
    _argv = make_argv("123 --timeout 0", _argc);
    BOOST_REQUIRE_THROW(
            kill_job::Options( _argc, _argv ),
            boost::program_options::invalid_option_value 
            );
}

BOOST_AUTO_TEST_CASE( negative_signal_number )
{
    _argv = make_argv("-s -10 --id 15", _argc);
    kill_job::Options options( _argc, _argv );
    BOOST_REQUIRE_THROW(
            options.validate(),
            boost::program_options::invalid_option_value
            );
}

BOOST_AUTO_TEST_SUITE_END()
