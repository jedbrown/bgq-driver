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
#define BOOST_TEST_MODULE convert
#include <boost/test/unit_test.hpp>

#include "common/message/convert.h"
#include "common/message/Header.h"

#include "common/Message.h"

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

using namespace runjob;
using namespace runjob::message;

struct Fixture {
    Fixture() :
        _header(),
        _msg(),
        _buf(),
        _os(&_buf)
    {
        bzero(&_header, sizeof(_header));
        bgq::utility::initializeLogging( *bgq::utility::Properties::create() );
    }

    Message::Ptr setHeader() {
        _msg->serialize(_os);
        _header._type = _msg->getType();

        return runjob::message::convert( _header, _buf );
    }
    
    Header _header;
    Message::Ptr _msg;
    boost::asio::streambuf _buf;
    std::ostream _os;
};

BOOST_FIXTURE_TEST_SUITE( convert, Fixture )

BOOST_AUTO_TEST_CASE( insert_job )
{
    // create empty message
    _msg.reset( new InsertJob() );
    BOOST_CHECK_EQUAL( _msg->getType(), Message::InsertJob );

    // serialize it into a string
    Message::Ptr result = setHeader();

    // ensure it worked
    BOOST_REQUIRE(
            boost::dynamic_pointer_cast<InsertJob>(result) != NULL
            );
}

BOOST_AUTO_TEST_CASE( start_job )
{
    // create empty message
    _msg.reset( new StartJob() );
    BOOST_CHECK_EQUAL( _msg->getType(), Message::StartJob );

    // serialize it into a string
    Message::Ptr result = setHeader();

    // ensure it worked
    BOOST_REQUIRE(
            boost::dynamic_pointer_cast<StartJob>(result) != NULL
            );
}

BOOST_AUTO_TEST_CASE( exit_job )
{
    // create empty message
    _msg.reset( new ExitJob() );
    BOOST_CHECK_EQUAL( _msg->getType(), Message::ExitJob );

    // serialize it into a string
    Message::Ptr result = setHeader();

    // ensure it worked
    BOOST_REQUIRE(
            boost::dynamic_pointer_cast<ExitJob>(result) != NULL
            );
}

BOOST_AUTO_TEST_CASE( kill_job )
{
    // create empty message
    _msg.reset( new KillJob() );
    BOOST_CHECK_EQUAL( _msg->getType(), Message::KillJob );

    // serialize it into a string
    Message::Ptr result = setHeader();

    // ensure it worked
    BOOST_REQUIRE(
            boost::dynamic_pointer_cast<KillJob>(result) != NULL
            );
}

BOOST_AUTO_TEST_CASE( perf_counters )
{
    // create empty message
    _msg.reset( new PerfCounters() );
    BOOST_CHECK_EQUAL( _msg->getType(), Message::PerfCounters );

    // serialize it into a string
    Message::Ptr result = setHeader();

    // ensure it worked
    BOOST_REQUIRE(
            boost::dynamic_pointer_cast<PerfCounters>(result) != NULL
            );
}

BOOST_AUTO_TEST_CASE( result )
{
    // create empty message
    _msg.reset( new Result() );
    BOOST_CHECK_EQUAL( _msg->getType(), Message::Result );

    // serialize it into a string
    Message::Ptr result = setHeader();

    // ensure it worked
    BOOST_REQUIRE(
            boost::dynamic_pointer_cast<Result>(result) != NULL
            );
}

BOOST_AUTO_TEST_CASE( stderr )
{
    // create empty message
    _msg.reset( new StdIo() );
    _msg->setType( Message::StdError );
    BOOST_CHECK_EQUAL( _msg->getType(), Message::StdError );

    // serialize it into a string
    Message::Ptr result = setHeader();

    // ensure it worked
    BOOST_REQUIRE(
            boost::dynamic_pointer_cast<StdIo>(result) != NULL
            );
}

BOOST_AUTO_TEST_CASE( stdout )
{
    // create empty message
    _msg.reset( new StdIo() );
    _msg->setType( Message::StdOut );
    BOOST_CHECK_EQUAL( _msg->getType(), Message::StdOut );

    // serialize it into a string
    Message::Ptr result = setHeader();

    // ensure it worked
    BOOST_REQUIRE(
            boost::dynamic_pointer_cast<StdIo>(result) != NULL
            );
}

BOOST_AUTO_TEST_CASE( stdin )
{
    // create empty message
    _msg.reset( new StdIo() );
    _msg->setType( Message::StdIn );
    BOOST_CHECK_EQUAL( _msg->getType(), Message::StdIn );

    // serialize it into a string
    Message::Ptr result = setHeader();

    // ensure it worked
    BOOST_REQUIRE(
            boost::dynamic_pointer_cast<StdIo>(result) != NULL
            );
}

BOOST_AUTO_TEST_CASE( proctable )
{
    // create empty message
    _msg.reset( new Proctable() );
    _msg->setType( Message::Proctable );
    BOOST_CHECK_EQUAL( _msg->getType(), Message::Proctable );

    // serialize it into a string
    Message::Ptr result = setHeader();

    // ensure it worked
    BOOST_REQUIRE(
            boost::dynamic_pointer_cast<Proctable>(result) != NULL
            );
}

BOOST_AUTO_TEST_SUITE_END()
