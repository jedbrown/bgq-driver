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


#include "GlobalFixture.hpp"

#include "CurlHandle.hpp"

#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>

#include <boost/bind.hpp>

#include <iostream>
#include <string>

#include <stdint.h>


GlobalFixture *GlobalFixture::_fixture_p = 0;


CurlHandlePtr GlobalFixture::createCurlHandle()
{
    CurlHandlePtr ret( new CurlHandle(
            get().endpoint,
            *get().properties_ptr
        ) );

    return ret;
}


GlobalFixture::GlobalFixture()
{
    properties_ptr = bgq::utility::Properties::create();

    bgq::utility::LoggingProgramOptions logging_program_options( "ibm.capena-http" );

    bgq::utility::LoggingProgramOptions::Strings lpo_args;

#if 0
    lpo_args.push_back( "ibm.capena-http.server.Connection=D" );
    lpo_args.push_back( "ibm.capena-htp.server=D" );
    lpo_args.push_back( "ibm.utility=D" );
#endif


    logging_program_options.notifier( lpo_args );

    bgq::utility::initializeLogging(
            *properties_ptr,
            logging_program_options
        );


    const uint32_t default_port(0); // If port is 0 then the server will pick one.

    bgq::utility::ServerPortConfiguration port_configuration(
            default_port,
            bgq::utility::ServerPortConfiguration::ConnectionType::Optional
        );

    const std::string section_name("");
    port_configuration.setProperties( properties_ptr, section_name );

    port_configuration.notifyComplete();


    _server_ptr = capena::server::Server::create(
            _io_service,
            port_configuration,
            boost::bind( &GlobalFixture::_createResponder, this, _1, _2 )
        );


    _server_ptr->setListeningCallback( boost::bind( &GlobalFixture::_serverListeningCb, this, _1 ) );


    _server_ptr->start();


    // Start a thread does io_service.run().
    _io_service_thread_ptr.reset( new boost::thread( boost::bind( &boost::asio::io_service::run, &_io_service ) ) );

    // Wait until server is listening.
    {
        boost::unique_lock<boost::mutex> listening_lock( _listening_mtx );
        while ( endpoint == boost::asio::ip::tcp::endpoint() ) {
            _listening_cond.wait( listening_lock );
        }
    }

    _fixture_p = this;
}


GlobalFixture::~GlobalFixture()
{
    std::cout << "Cleaning up, telling server to stop.\n";

    _server_ptr->stop();

    std::cout << "Cleaning up, waiting for server thread to complete.\n";

    _io_service_thread_ptr->join();

    std::cout << "Cleaning up, Done!\n";
}


void GlobalFixture::_serverListeningCb(
        const capena::server::Server::Endpoints& endpoints
    )
{
    boost::unique_lock<boost::mutex> listening_lock( _listening_mtx );

    assert( ! endpoints.empty() );

    endpoint = endpoints.front();

    _listening_cond.notify_all();
}


capena::server::ResponderPtr GlobalFixture::_createResponder(
        capena::server::RequestPtr request_ptr,
        bgq::utility::portConfig::UserType::Value conn_user_type
    )
{
    last_createResponder_user_type = conn_user_type;

    capena::server::ResponderPtr responder_ptr = _create_responder_fn( request_ptr, conn_user_type );

    return responder_ptr;
}
