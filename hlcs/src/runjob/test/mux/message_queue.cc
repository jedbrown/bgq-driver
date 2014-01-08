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
#define BOOST_TEST_MODULE message_queue_test
#include <boost/test/unit_test.hpp>

#include "common/defaults.h"

#include "mux/client/Id.h"
#include "mux/client/MessageQueue.h"

#include "mux/Options.h"

#include "test/mux/Fixture.h"
#include "test/make_argv.h"

using namespace runjob::mux;

BOOST_FIXTURE_TEST_SUITE( message_queue_test, Fixture )

BOOST_AUTO_TEST_CASE( buffer_size_properties )
{
    const bgq::utility::Properties properties( "good.properties" );
    const char* argv[] = {"foo", "--properties", "good.properties"};
    const Options options( 3, const_cast<char**>(argv) );
    const client::Id id(1);
    client::MessageQueue queue( options, id );
    const size_t expected = 
        boost::lexical_cast<size_t>( properties.getValue("runjob.mux", "client_output_buffer_size") )
        * 1024;
    BOOST_CHECK_EQUAL(
            queue.getMaximumSize(),
            expected
            );
}

BOOST_AUTO_TEST_CASE( garbage_buffer_size_properties )
{
    const bgq::utility::Properties properties( "garbage.properties" );
    const char* argv[] = {"foo", "--properties", "garbage.properties"};
    const Options options( 3, const_cast<char**>(argv) );
    const client::Id id(1);
    client::MessageQueue queue( options, id );
    BOOST_CHECK_EQUAL(
            queue.getMaximumSize(),
            runjob::defaults::MuxClientBufferSize * 1024
            );
}

BOOST_AUTO_TEST_SUITE_END()
