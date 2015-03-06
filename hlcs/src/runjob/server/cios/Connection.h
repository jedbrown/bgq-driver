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
#ifndef RUNJOB_SERVER_CIOS_CONNECTION_H
#define RUNJOB_SERVER_CIOS_CONNECTION_H

#include "common/Uci.h"

#include "server/block/fwd.h"

#include "server/cios/fwd.h"
#include "server/cios/Message.h"

#include "server/job/fwd.h"

#include "server/fwd.h"

#include <db/include/api/job/types.h>

#include <ramdisk/include/services/MessageHeader.h>

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <deque>

namespace runjob {
namespace server {
namespace cios {

/*!
 * \brief a single connection to a daemon running on an I/O node.
 */
class Connection : public boost::enable_shared_from_this<Connection>
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<Connection> Ptr;

    /*!
     * \brief Pointer type for socket objects.
     */
    typedef boost::shared_ptr<boost::asio::ip::tcp::socket> SocketPtr;

    /*!
     * \brief Status callback type.
     */
    typedef boost::function<void (const SocketPtr&)> StatusCallback;

    /*!
     * \brief
     */
    typedef boost::function<void()> StopCallback;
    
public:
    /*!
     * \brief ctor.
     */
    Connection(
            const uint8_t service,                          //!< [in]
            const std::string& location,                    //!< [in]
            const boost::shared_ptr<Server>& server,        //!< [in]
            const boost::shared_ptr<block::Io>& block       //!< [in]
            ); 

    /*!
     * \brief dtor.
     */
    ~Connection();

    /*!
     * \brief Get service type.
     */
    uint8_t getService() const { return _service; }

    /*!
     * \brief Get location string.
     */
    const Uci& getLocation() const { return _location; }

    /*!
     * \brief Get the endpoint.
     */
    const boost::asio::ip::tcp::endpoint& getEndpoint() const { return _endpoint; }

    /*!
     * \brief Start connecting to the I/O node.
     */
    void start(
            const boost::asio::ip::tcp::endpoint& endpoint  //!< [in]
            );

    /*!
     * \brief Stop reading from an I/O node.
     */
    void stop(
            const StopCallback& callback = StopCallback()   //!< [in]
            );

    /*!
     * \brief 
     */
    void status(
            const StatusCallback& callback                         //!< [in]
            );

    /*!
     * \brief Start asynchronously writing a message to the I/O node.
     */
    void write(
            const boost::shared_ptr<Message>& msg        //!< [in]
            );

private:
    void start();

    void read();

    void connectHandler(
            const SocketPtr& socket
            );

    void authenticateHandler(
            const SocketPtr& socket
            );

    void stopImpl(
            const StopCallback& callback
            );

    void statusImpl(
            const StatusCallback& callback 
            );

    void writeImpl(
            const boost::shared_ptr<Message>& msg
            );

    void write();

    void writeHandler(
            const boost::system::error_code& error
            );

    void readHeaderHandler(
            const boost::system::error_code& error
            );
   
    void readDataHandler(
            const boost::system::error_code& error
            );

    void findJobHandler(
            const boost::shared_ptr<Job>& job
            );

    void handleJob(
            const boost::shared_ptr<Job>& job,
            const boost::shared_ptr<cios::Message>& incoming
            );

    void keepAlive(
            const SocketPtr& socket
            );
    
    void pulse();

private:
    typedef std::deque< boost::shared_ptr<Message> > Outbox;

private:
    const boost::weak_ptr<Server> _server;                  //!<
    const uint8_t _service;                                 //!<
    const Options& _options;                                //!<
    SocketPtr _socket;                                      //!<
    boost::asio::strand _strand;                            //!<
    boost::asio::ip::tcp::endpoint _endpoint;               //!<
    Uci _location;                                          //!<
    boost::shared_ptr<ConnectionInterval> _interval;        //!<
    boost::shared_ptr<Authenticate> _authenticate;          //!<
    boost::shared_ptr<job::Container> _jobs;                //!<
    bgcios::MessageHeader _header;                          //!<
    boost::shared_ptr<cios::Message> _incoming;             //!<
    Outbox _outbox;                                         //!< outgoing messages
    boost::weak_ptr<block::Io> _block;                      //!<
    uint32_t _sequence;                                     //!<
    boost::posix_time::ptime _pulse;                        //!<
    uint32_t _authenticateRetryCount;                       //!<
};

} // cios
} // server
} // runjob

#endif
