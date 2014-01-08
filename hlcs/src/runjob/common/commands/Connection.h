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
#ifndef RUNJOB_SERVER_COMMANDS_CONNECTION_H
#define RUNJOB_SERVER_COMMANDS_CONNECTION_H

#include <hlcs/include/runjob/commands/Request.h>
#include <hlcs/include/runjob/commands/Response.h>

#include <utility/include/portConfiguration/Connector.h>

#include <boost/asio.hpp>

namespace runjob {
namespace commands {

class Options;

/*!
 * \brief A connection for a runjob command.
 */
class Connection
{
public:
    /*!
     * \brief ctor
     */
    Connection(
            const Options& options       //!< [in] program options
            );

    /*!
     * \brief Synchronously send a request and receive a response.
     *
     * \returns the response
     *
     * \throws boost::system::system_error
     */
    runjob::commands::Response::Ptr send(
            const runjob::commands::Request::Ptr& msg          //!< [in]
            );

private:
    /*!
     * \brief Socket type.
     */
    typedef bgq::utility::portConfig::SocketPtr SocketPtr;

private:
    boost::asio::io_service _io_service;    //!< I/O service
    const Options& _options;                //!< program options
    bgq::utility::Connector _connector;     //!< connector
    SocketPtr _socket;                      //!< socket
};

} // commands
} // runjob

#endif

