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
#ifndef RUNJOB_MUX_COMMAND_CONNECTION_H
#define RUNJOB_MUX_COMMAND_CONNECTION_H

#include "common/Connection.h"
#include "common/fwd.h"

#include "mux/fwd.h"

#include <hlcs/include/runjob/commands/Message.h>
#include <hlcs/include/runjob/commands/Response.h>

#include <utility/include/portConfiguration/Acceptor.h>

#include <boost/enable_shared_from_this.hpp>
#include <boost/weak_ptr.hpp>

#include <deque>

namespace runjob {
namespace mux {

/*!
 * \brief connection to a runjob_mux command.
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
     *
     * Creates a new command connection. Start handling requests with CommandConnection::readHeader().
     */
    CommandConnection(
            const bgq::utility::portConfig::SocketPtr& socket,      //!< [in]
            const bgq::utility::UserId::ConstPtr& user,             //!< [in]
            bgq::utility::portConfig::UserType::Value user_type,    //!< [in]
            const boost::weak_ptr<Multiplexer>& mux                 //!< [in]
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
    boost::shared_ptr<Connection> getShared() { return shared_from_this(); };
    
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

    void resolveComplete();

private:
    boost::asio::strand _strand;                                //!<
    runjob::commands::Header _incomingHeader;                   //!<
    std::deque<runjob::commands::Message::Ptr> _outbox;         //!<
    runjob::commands::Header _outgoingHeader;                   //!<
    const bgq::utility::UserId::ConstPtr _user;                 //!<
    const bgq::utility::portConfig::UserType::Value _user_type; //!<
    boost::weak_ptr<Multiplexer> _mux;                          //!<
};

} // mux
} // runjob

#endif
