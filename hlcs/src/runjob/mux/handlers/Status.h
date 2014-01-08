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
#ifndef RUNJOB_MUX_HANDLERS_STATUS_H
#define RUNJOB_MUX_HANDLERS_STATUS_H

#include "common/ConnectionContainer.h"

#include "mux/client/Container.h"

#include "common/commands/MuxStatus.h"

#include "common/fwd.h"

#include "mux/CommandHandler.h"
#include "mux/fwd.h"
#include "mux/Plugin.h"

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <boost/enable_shared_from_this.hpp>

namespace runjob {
namespace mux {
namespace handlers {

/*!
 * \brief Handles the runjob_mux_status command.
 */
class Status : public CommandHandler, public boost::enable_shared_from_this<Status>
{
public:
    /*!
     * \brief ctor.
     */
    Status(
            const boost::weak_ptr<Multiplexer>& mux //!< [in]
            );

    /*!
     * \brief Handle the command.
     */
    void handle(
            const commands::Request::Ptr& request,                 //!< [in]
            const boost::shared_ptr<CommandConnection>& connection //!< [in]
            );
    /*!
     * \brief Get the user type.
     */
    bgq::utility::portConfig::UserType::Value getUserType() const { return bgq::utility::portConfig::UserType::Administrator; }

private:
    void loadHandler(
            const boost::posix_time::ptime& start,
            const boost::shared_ptr<CommandConnection>& connection,
            const runjob::commands::response::MuxStatus::Ptr& response
            );

    void connectionCallback(
            const boost::shared_ptr<CommandConnection>& connection,
            const runjob::commands::response::MuxStatus::Ptr& response,
            const ConnectionContainer::Container& connections
            );

    void getClientsCallback(
            const boost::shared_ptr<CommandConnection>& connection,
            const runjob::commands::response::MuxStatus::Ptr& response,
            const client::Container::Map& clients
            );

    void clientStatusCallback(
            const boost::shared_ptr<CommandConnection>& connection,
            const runjob::commands::response::MuxStatus::Ptr& response,
            client::Container::Map::const_iterator& client
            );

    void serverStatusCallback(
            const boost::shared_ptr<CommandConnection>& connection,
            const runjob::commands::response::MuxStatus::Ptr& response
            );

    void pluginCallback(
            const boost::shared_ptr<CommandConnection>& connection,
            const runjob::commands::response::MuxStatus::Ptr& response,
            const Plugin::WeakPtr& plugin
            );

private:
    boost::weak_ptr<Multiplexer> _mux;
    client::Container::Map _clientMap;
};

} // handlers
} // mux
} // runjob

#endif
