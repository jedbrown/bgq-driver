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
#define BOOST_TEST_MODULE authority
#include <boost/test/unit_test.hpp>

#include "server/commands/grant_job_authority/Options.h"
#include "server/commands/revoke_job_authority/Options.h"

#include "test/server/commands/Fixture.h"

#include "test/make_argv.h"

#include <db/include/api/tableapi/gensrc/DBTJobsecurity.h>
#include <boost/assign/list_of.hpp>

#include <boost/program_options/errors.hpp>

#include <boost/foreach.hpp>

using namespace runjob::server::commands;

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );

BOOST_FIXTURE_TEST_SUITE( authority, Fixture )

BOOST_AUTO_TEST_CASE( positional )
{
    _argv = make_argv( "1 fred read", _argc );

    BOOST_CHECK_NO_THROW(
            grant_job_authority::Options( _argc, _argv )
            );
    BOOST_CHECK_NO_THROW(
            revoke_job_authority::Options( _argc, _argv )
            );
}

BOOST_AUTO_TEST_CASE( authid_too_large )
{
    const std::string authid( BGQDB::DBTJobsecurity::AUTHID_SIZE + 1, 'a' );
    _argv = make_argv( "1 " + authid + " read", _argc );

    BOOST_CHECK_THROW(
            grant_job_authority::Options( _argc, _argv ),
            boost::program_options::invalid_option_value
            );
    BOOST_CHECK_THROW(
            revoke_job_authority::Options( _argc, _argv ),
            boost::program_options::invalid_option_value
            );
}

BOOST_AUTO_TEST_SUITE_END()
