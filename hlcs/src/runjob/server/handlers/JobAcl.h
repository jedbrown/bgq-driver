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
#ifndef RUNJOB_SERVER_HANDLERS_JOB_ACL_H
#define RUNJOB_SERVER_HANDLERS_JOB_ACL_H

#include "common/commands/JobAcl.h"

#include "server/CommandHandler.h"

#include <boost/enable_shared_from_this.hpp>

namespace runjob {
namespace server {

class CommandConnection;
class Server;

namespace handlers {

/*!
 * \brief Handles the job_status command.
 */
class JobAcl : public CommandHandler, public boost::enable_shared_from_this<JobAcl>
{
public:
    /*!
     * \brief ctor.
     */
    JobAcl(
            const boost::shared_ptr<Server>& server    //!< [in]
            );

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
    bool checkOwner(
            const runjob::commands::request::JobAcl::Ptr& request,
            const boost::shared_ptr<CommandConnection>& connection
            );

    void list(
            const runjob::commands::request::JobAcl::Ptr& request,
            const runjob::commands::response::JobAcl::Ptr& response
            );
    
    void grant(
            const runjob::commands::request::JobAcl::Ptr& request,
            const boost::shared_ptr<CommandConnection>& connection
            );

    void revoke(
            const runjob::commands::request::JobAcl::Ptr& request,
            const boost::shared_ptr<CommandConnection>& connection
            );
};

} // handlers
} // server
} // runjob

#endif
