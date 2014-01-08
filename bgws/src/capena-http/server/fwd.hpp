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

#ifndef CAPENA_SERVER_FWD_HPP_
#define CAPENA_SERVER_FWD_HPP_


/*! \mainpage

Capena is a dynamic web application library.
It provides classes to implement an HTTP server and generate dynamic responses.
Capena is named after the Porta Capena in Rome because it's a gateway where
a festival for the Roman god Mercury, known for his amazing speed, is held.

HTTP is defined at:<br>
http://tools.ietf.org/html/rfc2616

The classes are in the \ref capena namespace.

An application using this library does:

-# Define a class extending capena::server::AbstractResponder that generates responses.
-# Create an instance of capena::server::Server,
passing it your boost::asio::io_service and capena::server::ResponderCreatorFn, a function that creates your implementation of capena::server::AbstractResponder.
-# Call start() on your instance of capena::server::Server.

The capena::server::Server registers work with the io_service so
when you run the io_service and the Server does its work.

When a request comes in from the web server,
an instance of your responder will be created and methods will be called on it
to process the request.

Also look at capena::server::exception::Error for a way to generate responses when the application must throw an exception.

 */


#include "../http/http.hpp"

#include <utility/include/portConfiguration/types.h>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include <map>
#include <string>
#include <vector>

#include <stdint.h>


/*! \brief Capena-HTTP library namespace */
namespace capena {

/*! \brief HTTP server. */
namespace server {


typedef std::vector<uint8_t> Bytes;

typedef std::map<std::string,std::string> Headers;

typedef bgq::utility::portConfig::SocketPtr SocketPtr;

class AbstractResponder;
typedef boost::shared_ptr<AbstractResponder> ResponderPtr;

class Request;
typedef boost::shared_ptr<Request> RequestPtr;
typedef boost::shared_ptr<const Request> RequestConstPtr;

class Connection;
typedef boost::shared_ptr<Connection> ConnectionPtr;

class Response;
typedef boost::shared_ptr<Response> ResponsePtr;

class Server;


/*! \brief Create a Responder to generate a response for the request.
 *
 * \param request_ptr will not be null.
 *
 * This function will be called when a new request comes in to the server.
 *
 * The Request passed in to this function has the headers complete,
 * so the URI (method, path, and query), and headers are available.
 *
 * If an exception is thrown then a response will be generated based on the exception.
 *
 * If NULL is returned
 * the response will be <em>501 Internal %Server Error</em>.
 *
 */
typedef boost::function<ResponderPtr ( RequestPtr request_ptr, bgq::utility::portConfig::UserType::Value conn_user_type )> ResponderCreatorFn;


enum class DataContinuesIndicator {
    END_OF_DATA,
    EXPECT_MORE_DATA
};


/*! \brief Container for enum, indicates whether the headers set by the application indicate that the response has a body or not. */
enum class BodyPresense {
    EXPECT_BODY,
    NO_BODY
};


/*! \brief This function is used by the Responder to notify that the headers are complete. */
typedef boost::function<void ( http::Status status, const Headers& headers, BodyPresense expect_body )> NotifyStatusHeadersFn;

/*! \brief This function is used by the Responder to notify data to send and end of data. */
typedef boost::function<void ( const std::string& data, DataContinuesIndicator data_continues )> NotifyDataFn;


} } // namespace capena::server


#endif
