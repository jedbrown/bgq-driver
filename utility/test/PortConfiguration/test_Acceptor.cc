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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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


#include "common.hpp"

#include "portConfiguration/Acceptor.h"
#include "portConfiguration/Connector.h"

#include <boost/bind.hpp>

#include <iostream>


using std::cout;


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE Acceptor
#include <boost/test/unit_test.hpp>


class GlobalFixture
{
public:


    bgq::utility::Properties::Ptr properties_ptr;


    GlobalFixture() :
        properties_ptr( new bgq::utility::Properties() )
    {
        bgq::utility::initializeLogging( *properties_ptr );

        s_p = this;
    }


    static GlobalFixture& get()  { return *s_p; }


private:

    static GlobalFixture *s_p;

};


GlobalFixture *GlobalFixture::s_p = NULL;


BOOST_GLOBAL_FIXTURE( GlobalFixture );


void test_stop_accepted(
        const bgq::utility::Acceptor::AcceptArguments& args,
        bgq::utility::Acceptor::AcceptArguments* args_out,
        bgq::utility::Acceptor& acceptor
    )
{
    cout << "test_Acceptor: simple_accepted() called! status=" << args.status << "\n";

    if ( args.status == bgq::utility::Acceptor::Status::NowAccepting ) {
        *args_out = args;
        acceptor.stop();
    }
}


BOOST_AUTO_TEST_CASE( test_stop )
{
    // Simple test case just makes sure that can start accepting and call stop.

    boost::asio::io_service io_service;

    bgq::utility::ServerPortConfiguration server_port_configuration( 0 );

    server_port_configuration.setProperties( GlobalFixture::get().properties_ptr, "" /* section_name */ );

    server_port_configuration.notifyComplete();

    bgq::utility::Acceptor acceptor(
            io_service,
            server_port_configuration
        );


    bgq::utility::Acceptor::AcceptArguments args( bgq::utility::Acceptor::Status::ResolveError );

    acceptor.start(
            boost::bind( &test_stop_accepted, _1, &args, boost::ref( acceptor ) )
        );

    io_service.run();

    BOOST_CHECK_EQUAL( args.status, bgq::utility::Acceptor::Status::NowAccepting );
}


class TestConnect
{
public:

    TestConnect(
            boost::asio::io_service& io_service
        ) :
            _io_service(io_service),
            _got_ok(false)
    {
        bgq::utility::ServerPortConfiguration server_port_configuration( 0 );

        server_port_configuration.setProperties(
                GlobalFixture::get().properties_ptr,
                "" /* section_name */
            );

        server_port_configuration.notifyComplete();

        _acceptor_ptr.reset( new bgq::utility::Acceptor(
                io_service,
                server_port_configuration
            ) );
    }


    void start()
    {
        _acceptor_ptr->start(
                boost::bind( &TestConnect::_handleAccepted, this, _1 )
            );
    }


    bool gotOk() const  { return _got_ok; }
    bgq::utility::portConfig::UserType::Value getUserType() const  { return _user_type; }
    const bgq::utility::UserId& getUserId() const  { return *_user_id_ptr; }
    const std::string& getClientCn() const  { return _client_cn; }


private:

    boost::asio::io_service &_io_service;
    boost::shared_ptr<bgq::utility::Acceptor> _acceptor_ptr;

    boost::shared_ptr<bgq::utility::Connector> _connector_ptr;

    bool _got_ok;
    bgq::utility::portConfig::UserType::Value _user_type;
    bgq::utility::UserId::ConstPtr _user_id_ptr;
    std::string _client_cn;


    void _handleAccepted(
            const bgq::utility::Acceptor::AcceptArguments& args
        )
    {
        if ( args.status == bgq::utility::Acceptor::Status::NowAccepting ) {

            cout << "TestConnect: listening on " << args.endpoints[0] << "\n";

            bgq::utility::ClientPortConfiguration client_port_configuration(
                    args.endpoints[0].port()
                );

            client_port_configuration.setProperties(
                    GlobalFixture::get().properties_ptr,
                    "" /* section_name */
                );

            client_port_configuration.notifyComplete();

            _connector_ptr.reset( new bgq::utility::Connector(
                    _io_service,
                    client_port_configuration
                ) );

            cout << "TestConnect: connecting...\n";

            _connector_ptr->async_connect(
                    boost::bind( &TestConnect::_handleConnected, this, _1 )
                );
        }


        if ( args.status == bgq::utility::Acceptor::Status::OK ) {
            cout << "TestConnect:_handleAccepted status is OK. user_id=" << args.user_id_ptr->getUser() << " cn=" << args.client_cn << "\n";

            _got_ok = true;
            _user_type = args.user_type;
            _user_id_ptr = args.user_id_ptr;
            _client_cn = args.client_cn;

            _acceptor_ptr->stop();
        }

    }


    void _handleConnected(
            const bgq::utility::Connector::ConnectResult &cr
        )
    {
        cout << "TestConnect::_handleConnected: err=" << cr.error << "\n";
    }

};


BOOST_AUTO_TEST_CASE( test_Connect )
{
    // If have Acceptor config'd to accept Administrative or Command certificates
    // and connect with Connector config'd to use Command (and have access to administrative),
    // The accepted callback is called with
    // status OK,
    // user_type is Administrator,
    // user id is the current user,
    // client cn is the CN from the administrative cert.

    boost::asio::io_service io_service;

    TestConnect test(
            io_service
        );

    test.start();

    io_service.run();

    BOOST_CHECK( test.gotOk() );
    BOOST_CHECK_EQUAL( test.getUserType(), bgq::utility::portConfig::UserType::Administrator );
    BOOST_CHECK_EQUAL( test.getUserId().getUser(), bgq::utility::UserId().getUser() );
    BOOST_CHECK_EQUAL( test.getClientCn(), "Blue Gene administrative" );
}
