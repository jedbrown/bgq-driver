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
#ifndef RUNJOB_MUX_CLIENT_RUNJOB_H_
#define RUNJOB_MUX_CLIENT_RUNJOB_H_

#include "common/commands/MuxStatus.h"

#include "mux/client/fwd.h"
#include "mux/client/Id.h"
#include "mux/client/Plugin.h"
#include "mux/client/Server.h"
#include "mux/client/Status.h"
#include "mux/client/Timers.h"

#include "mux/server/fwd.h"

#include "mux/fwd.h"

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/weak_ptr.hpp>

namespace runjob {

class Message;

namespace mux {
namespace client {

/*!
 * \brief manages a single %runjob %client.
 */
class Runjob : public boost::enable_shared_from_this<Runjob>
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<Runjob> Ptr;

    /*!
     * \brief weak pointer type.
     */
    typedef boost::weak_ptr<Runjob> WeakPtr;

    /*!
     * \brief Callback type for status requests.
     */
    typedef boost::function<void (const runjob::commands::response::MuxStatus::Ptr&)> StatusCallback;

public:
    /*!
     * \brief ctor.
     */
    Runjob(
            const boost::shared_ptr<Multiplexer>& mux,  //!< [in]
            const Id& id                                //!< [in]
            );

    /*!
     * \brief dtor
     */
    ~Runjob();

    /*!
     * \brief Start servicing this client.
     */
    void start();

    /*!
     * \brief Handle a message from the runjob_server.
     */
    void handle(
            const boost::shared_ptr<runjob::Message>& msg   //!< [in]
            );

    /*!
     * \brief Get client ID.
     */
    const Id& getId() const { return _id; }

    /*!
     * \brief Get Connection.
     */
    const boost::shared_ptr<Connection>& getConnection() { return _connection; }

    /*!
     * \brief Add our status to the response.
     */
    void status(
            const runjob::commands::response::MuxStatus::Ptr& response,    //!< [in]
            const StatusCallback& handler                                  //!< [in]
            );

private:
    void serverCallback(
            const boost::system::error_code& error
            );

    void handlePlugin(
            const runjob::mux::Plugin::WeakPtr& plugin //!< [in]
            );

    void handleImpl(
            const boost::shared_ptr<runjob::Message>& msg
            );

    void handleInserting(
            const boost::shared_ptr<runjob::Message>& msg   //!< [in]
            );

    void handleStarting(
            const boost::shared_ptr<runjob::Message>& msg   //!< [in]
            );

    void handleDebug(
            const boost::shared_ptr<runjob::Message>& msg   //!< [in]
            );

    void handleDying(
            const boost::shared_ptr<runjob::Message>& msg   //!< [in]
            );

    void handleRunning(
            const boost::shared_ptr<runjob::Message>& msg   //!< [in]
            );

    void handleRequest(
            const boost::shared_ptr<runjob::Message>& message,
            const boost::system::error_code& error
            );

    void statusImpl(
            const runjob::commands::response::MuxStatus::Ptr& response,
            const StatusCallback& handler
            );

    void timeout(
            const boost::system::error_code& error
            );

    void clientDisconnected(
            const boost::system::error_code& error
            );
private:
    boost::asio::deadline_timer _timer;                             //!<
    boost::asio::strand _strand;                                    //!<
    Status _status;                                                 //!<
    const Id _id;                                                   //!<
    boost::shared_ptr<Connection> _connection;                      //!<
    Server _server;                                                 //!<
    Timers _timers;                                                 //!<
    boost::shared_ptr<Plugin> _plugin;                              //!<
    boost::shared_ptr<Credentials> _credentials;                    //!<
    uint64_t _jobId;                                                //!<
    boost::weak_ptr<Multiplexer> _mux;                              //!<
    bool _startTool;                                                //!<
};

} // client
} // mux
} // runjob

#endif
