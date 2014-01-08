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
#ifndef RUNJOB_SERVER_HANDLERS_TOOL_STATUS_H
#define RUNJOB_SERVER_HANDLERS_TOOL_STATUS_H

#include "common/commands/ToolStatus.h"

#include "server/CommandHandler.h"
#include "server/fwd.h"

#include <boost/enable_shared_from_this.hpp>

namespace runjob {
namespace server {
namespace handlers {

/*!
 * \brief Handles the start_tool command.
 */
class ToolStatus : public CommandHandler, public boost::enable_shared_from_this<ToolStatus>
{
public:
    /*!
     * \brief ctor.
     */
    ToolStatus(
            const boost::shared_ptr<Server>& server          //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~ToolStatus();

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
    bgq::utility::portConfig::UserType::Value getUserType() const { return bgq::utility::portConfig::UserType::Normal; }

private:
    void lookupJob();

    void findHandler(
            const boost::shared_ptr<Job>& job
            );

private:
    runjob::commands::request::ToolStatus::Ptr _request;
    const runjob::commands::response::ToolStatus::Ptr _response;
    boost::shared_ptr<CommandConnection> _connection;
    boost::weak_ptr<Job> _job;
};

} // handlers
} // server
} // runjob

#endif
