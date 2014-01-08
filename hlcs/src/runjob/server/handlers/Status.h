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
#ifndef RUNJOB_SERVER_HANDLERS_STATUS_H
#define RUNJOB_SERVER_HANDLERS_STATUS_H

#include "server/block/Container.h"

#include "server/cios/Connection.h"

#include "server/job/Container.h"

#include "common/commands/ServerStatus.h"

#include "common/ConnectionContainer.h"

#include "server/CommandHandler.h"

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <boost/enable_shared_from_this.hpp>

#include <vector>

namespace runjob {
namespace server {

class CommandConnection;
class Server;

namespace handlers {

/*!
 * \brief Handles the runjob_server_status command.
 */
class Status : public CommandHandler, public boost::enable_shared_from_this<Status>
{
public:
    /*!
     * \brief ctor.
     */
    Status(
            const boost::shared_ptr<Server>& server          //!< [in]
          );

    /*!
     * \brief dtor.
     */
    ~Status();

    /*!
     * \brief Handle the command.
     */
    void handle(
            const runjob::commands::Request::Ptr& request,          //!< [in]
            const boost::shared_ptr<CommandConnection>& connection  //!< [in]
            );

    /*!
     * \brief Get the user type.
     */
    bgq::utility::portConfig::UserType::Value getUserType() const { return bgq::utility::portConfig::UserType::Administrator; }

private:
    typedef std::vector<cios::Connection::Ptr> IoLinks;

private:
    void loadHandler(
            const boost::posix_time::ptime& start
            );

    void connectionsHandler(
            const ConnectionContainer::Container& connections
            );

    void jobsHandler(
            const job::Container::Jobs& jobs
            );

    void blocksHandler(
            const block::Container::Blocks& blocks
            );

    void linksHandler(
            const cios::Connection::SocketPtr& socket
            );

private:
    const boost::shared_ptr<IoLinks> _links;
    IoLinks::iterator _link;
    boost::shared_ptr<CommandConnection> _connection;
    const runjob::commands::response::ServerStatus::Ptr _response;
};

} // handlers
} // server
} // runjob

#endif
