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
#ifndef RUNJOB_SERVER_COMMAND_HANDLER_H
#define RUNJOB_SERVER_COMMAND_HANDLER_H

#include "server/fwd.h"

#include <db/include/api/cxxdb/fwd.h>

#include <db/include/api/job/types.h>

#include <hlcs/include/runjob/commands/Request.h>

#include <utility/include/portConfiguration/types.h>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace runjob {
namespace server {

/*!
 * \brief Handles commands sent to the runjob_server.
 */
class CommandHandler
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<CommandHandler> Ptr;

public:
    /*!
     * \brief factory method.
     */
    static void create(
            const boost::shared_ptr<Server>& server,                //!< [in]
            const runjob::commands::Request::Ptr& request,          //!< [in]
            const boost::shared_ptr<CommandConnection>& connection  //!< [in]
            );

    /*!
     * \brief dtor.
     */
    virtual ~CommandHandler();

    /*!
     * \brief Handle the command.
     */
    virtual void handle(
            const runjob::commands::Request::Ptr& request,          //!< [in]
            const boost::shared_ptr<CommandConnection>& connection  //!< [in]
            ) = 0;

    /*!
     * \brief Get the type of user this command requires.
     */
    virtual bgq::utility::portConfig::UserType::Value getUserType() const = 0;

protected:
    /*!
     * \brief ctor.
     */
    CommandHandler(
            const boost::shared_ptr<Server>& server //!< [in]
            );

    /*!
     * \brief Get a job ID using a pid and hostname
     */
    BGQDB::job::Id getJob(
            const cxxdb::ConnectionPtr& connection,
            pid_t pid,
            const std::string& host
            );

protected:
    const Options& _options;                //!< program options
    const boost::weak_ptr<Server> _server;  //!< server
};

} // server
} // runjob

#endif
