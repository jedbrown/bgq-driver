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


#ifndef GLOBAL_FIXTURE_HPP_
#define GLOBAL_FIXTURE_HPP_


#include "../../server/Server.hpp"

#include <utility/include/Properties.h>

#include <utility/include/portConfiguration/types.h>

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>


class CurlHandle;

typedef boost::shared_ptr<CurlHandle> CurlHandlePtr;


class GlobalFixture
{
public:

    typedef boost::function<capena::server::ResponderPtr (
            capena::server::RequestPtr request_ptr,
            bgq::utility::portConfig::UserType::Value conn_user_type
        )> CreateResponderFn;

    static CurlHandlePtr createCurlHandle();


    bgq::utility::Properties::Ptr properties_ptr;

    boost::asio::ip::tcp::endpoint endpoint;


    bgq::utility::portConfig::UserType::Value last_createResponder_user_type;


    GlobalFixture();


    boost::asio::io_service& io_service()  { return _io_service; }

    static GlobalFixture& get() { return *_fixture_p; }


    void setCreateResponderFn( CreateResponderFn create_responder_fn )
    {
        _create_responder_fn = create_responder_fn;
    }


    ~GlobalFixture();


private:

    typedef boost::shared_ptr<boost::thread> _ThreadPtr;


    static GlobalFixture *_fixture_p;

    boost::asio::io_service _io_service;

    capena::server::Server::Ptr _server_ptr;
    _ThreadPtr _io_service_thread_ptr;


    boost::mutex _listening_mtx;
    boost::condition_variable _listening_cond;


    CreateResponderFn _create_responder_fn;


    void _serverListeningCb(
            const capena::server::Server::Endpoints& endpoints
        );


    capena::server::ResponderPtr _createResponder(
            capena::server::RequestPtr request_ptr,
            bgq::utility::portConfig::UserType::Value conn_user_type
        );

};


// For some reason doesn't work to shut down the capena server, this makes sure it doesn't get cleaned up, process just exits.
class GFHolder
{
public:
    GFHolder()  { _fixture_ptr.reset( new GlobalFixture() ); }
    ~GFHolder()  { _exit( 0 ); /* Prevent destructors from being called. */ }
private:
    boost::scoped_ptr<GlobalFixture> _fixture_ptr;
};


#endif
