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
#define BOOST_TEST_MODULE client
#define BOOST_TEST_DYN_LINK
#define DBOOST_TEST_MAIN

#include <bgsched/runjob/Client.h>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( arg_valid )
{
    using namespace bgsched::runjob;

    const char* argv[] = {"foo","--exe","hello"};
    BOOST_CHECK_NO_THROW(
            Client( 3, const_cast<char**>(argv) )
            );
}

BOOST_AUTO_TEST_CASE( arg_invalid )
{
    using namespace bgsched::runjob;

    // --exe specific twice should be translated to a std::invalid_argument
    // instead of a boost::program_options exception type
    const char* argv[] = {"foo","--exe","hello","--exe","world"};
    BOOST_CHECK_THROW(
            Client( 5, const_cast<char**>(argv) ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( kill_when_not_started )
{
    using namespace bgsched::runjob;

    const char* argv[] = {"foo","--exe","hello"};
    Client client( 3, const_cast<char**>(argv) );
    BOOST_CHECK_THROW(
            client.kill( 15 ),
            std::logic_error
            );
}

BOOST_AUTO_TEST_CASE( versions )
{
    using namespace bgsched::runjob;

    BOOST_CHECK_EQUAL(
            Client::Version::major,
            1u
            );

    BOOST_CHECK_EQUAL(
            Client::Version::minor,
            0u
            );

    BOOST_CHECK_EQUAL(
            Client::Version::mod,
            0u
            );
}
