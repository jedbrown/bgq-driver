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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#ifndef MMCS_ENV_MC_SERVER_CONNECTION
#define MMCS_ENV_MC_SERVER_CONNECTION

#include <utility/include/portConfiguration/Connector.h>
#include <utility/include/portConfiguration/types.h>

#include <xml/include/library/XML.h>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include <iostream>

namespace mmcs {
namespace server {
namespace env {

class McServerConnection : public boost::enable_shared_from_this<McServerConnection>
{
public:
    /*!
     * \brief Callback type.
     */
    typedef boost::function<void(std::istream&)> SendCallback;
   
    /*!
     * \brief Connect callback type.
     */
    typedef boost::function<void(const bgq::utility::Connector::Error::Type, const std::string&)> ConnectCallback;

    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<McServerConnection> Ptr;

public:
    /*!
     * \brief Factory
     */
    static Ptr create(
            boost::asio::io_service& io_service //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~McServerConnection();

    /*!
     * \brief
     */
    void send(
            const std::string& name,
            const XML::Serializable& request,
            const SendCallback& callback
            );

    /*!
     * \brief
     */
    void start(
            const std::string& user,        //!< [in]
            const ConnectCallback& callback //!< [in] callback
            );

    const bgq::utility::portConfig::SocketPtr& getSocket() const { return _socket; }

private:
    McServerConnection(
            boost::asio::io_service& io_service
            );

    void connectHandler(
            const bgq::utility::Connector::ConnectResult& result,
            const boost::shared_ptr<bgq::utility::Connector>& connector,
            const ConnectCallback& callback,
            const std::string& user
            );

    void connectRequestHandler(
            std::istream& response,
            const ConnectCallback& callback
            );

    void writeHandler(
            const boost::system::error_code& error,
            const size_t bytesTransferred,
            const SendCallback& callback
            );

    void readHeaderSizeHandler(
            const boost::system::error_code& error,
            const size_t bytesTransferred,
            const SendCallback& callback
            );
    
    void readHeaderHandler(
            const boost::system::error_code& error,
            const size_t bytesTransferred,
            const SendCallback& callback
            );
    
    void readMessageSizeHandler(
            const boost::system::error_code& error,
            const size_t bytesTransferred,
            const SendCallback& callback
            );
    
    void readMessageHandler(
            const boost::system::error_code& error,
            const size_t bytesTransferred,
            const SendCallback& callback
            );

private:
    boost::asio::io_service& _io_service;
    bgq::utility::portConfig::SocketPtr _socket;
    std::string _outgoingHeader;
    unsigned _outgoingHeaderSize;
    boost::asio::streambuf _outgoingMessage;
    unsigned _outgoingMessageSize;
    std::vector<char> _incomingHeader;
    unsigned _incomingHeaderSize;
    boost::asio::streambuf _incomingMessage;
    unsigned _incomingMessageSize;
};

} } } // namespace mmcs::server::env

#endif
