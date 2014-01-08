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
#ifndef RUNJOB_SERVER_HANDLERS_KILL_JOB_H
#define RUNJOB_SERVER_HANDLERS_KILL_JOB_H

#include "server/CommandHandler.h"

#include <hlcs/include/runjob/commands/KillJob.h>

#include <boost/enable_shared_from_this.hpp>

namespace runjob {
namespace server {

class CommandConnection;
class Job;
class Server;

namespace handlers {

/*!
 * \brief Handles the kill_job command.
 */
class KillJob : public CommandHandler, public boost::enable_shared_from_this<KillJob>
{
public:
    /*!
     * \brief ctor.
     */
    KillJob(
            const boost::shared_ptr<Server>& server    //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~KillJob();

    /*!
     * \brief Handle the command.
     */
    void handle(
            const runjob::commands::Request::Ptr& request,         //!< [in]
            const boost::shared_ptr<CommandConnection>& connection //!< [in]
            );

    /*!
     * \brief Get the user type.
     */
    bgq::utility::portConfig::UserType::Value getUserType() const { return bgq::utility::portConfig::UserType::Normal; }

private:
    void findJobCallback(
            const boost::shared_ptr<Job>& job
            );

    void callback(
            const boost::shared_ptr<Job>& job,
            runjob::commands::error::rc error,
            const std::string& message
            );

    void lookupJob();

private:
    runjob::commands::request::KillJob::Ptr _request;
    const runjob::commands::response::KillJob::Ptr _response;
    boost::shared_ptr<CommandConnection> _connection;
};

} // handlers
} // server
} // runjob

#endif
