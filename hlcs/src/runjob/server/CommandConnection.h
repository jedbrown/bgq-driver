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
#ifndef RUNJOB_SERVER_COMMAND_CONNECTION_H_
#define RUNJOB_SERVER_COMMAND_CONNECTION_H_

#include "server/Connection.h"
#include "server/Server.h"

#include <hlcs/include/runjob/commands/Message.h>

#include <hlcs/include/runjob/commands/Response.h>

#include <utility/include/portConfiguration/Acceptor.h>

#include <boost/enable_shared_from_this.hpp>

#include <deque>

namespace runjob {
namespace server {

/*!
 * \brief connection to a runjob_server command.
 */
class CommandConnection : public Connection, public boost::enable_shared_from_this<CommandConnection>
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<CommandConnection> Ptr;

public:
    /*!
     * \brief ctor.
     */
    CommandConnection(
            const bgq::utility::portConfig::SocketPtr& socket,      //!< [in]
            const bgq::utility::UserId::ConstPtr& user,             //!< [in]
            bgq::utility::portConfig::UserType::Value user_type,    //!< [in]
            const Server::Ptr& server                               //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~CommandConnection();

    /*!
     * \brief Add message to outgoing message queue.
     */
    void write(
            const runjob::commands::Message::Ptr& msg   //!< [in]
            );

    /*!
     * \brief Get user type.
     */
    bgq::utility::portConfig::UserType::Value getUserType() const { return _user_type; }

    /*!
     * \brief Get user name.
     */
    const bgq::utility::UserId::ConstPtr& getUser() const { return _user; }

private:
    Connection::Ptr getShared() { return this->shared_from_this(); }
    
    void readHeader();

    void readHeaderHandler(
            const boost::system::error_code& error,
            const size_t bytesTransferred
            );

    void readBodyHandler(
            const boost::system::error_code& error,
            const size_t bytesTransferred
            );
    
    void writeImpl(
            const runjob::commands::Message::Ptr& msg
            );

    void write();

    void writeHandler(
            const boost::system::error_code& error,
            const size_t bytesTransferred
            );

private:
    boost::asio::strand _strand;                                //!<
    runjob::commands::Header _incomingHeader;                   //!<
    std::deque<runjob::commands::Message::Ptr> _outbox;         //!<
    runjob::commands::Header _outgoingHeader;                   //!<
    const bgq::utility::UserId::ConstPtr _user;                 //!<
    const bgq::utility::portConfig::UserType::Value _user_type; //!<
};

} // server
} // runjob

#endif
