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
#ifndef RUNJOB_SERVER_JOB_TOOL_TOOL_STATUS_H
#define RUNJOB_SERVER_JOB_TOOL_TOOL_STATUS_H

#include "server/fwd.h"

#include "common/commands/ToolStatus.h"

#include <hlcs/include/runjob/commands/error.h>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <sstream>

namespace runjob {
namespace server {
namespace job {
namespace tool {

/*!
 * \brief
 */
class ToolStatus: public boost::enable_shared_from_this<ToolStatus>
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<ToolStatus> Ptr;

public:
    /*!
     * \brief Factory.
     */
    static void create(
            const boost::shared_ptr<Job>& job,                          //!< [in]
            const runjob::commands::request::ToolStatus::Ptr& request,  //!< [in]
            const boost::shared_ptr<CommandConnection>& connection      //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~ToolStatus();

private:
    ToolStatus(
            const boost::shared_ptr<Job>& job,
            const runjob::commands::request::ToolStatus::Ptr& request,
            const boost::shared_ptr<CommandConnection>& connection
            );

    void impl();

private:
    boost::shared_ptr<Job> _job;
    const runjob::commands::request::ToolStatus::Ptr _request;
    const runjob::commands::response::ToolStatus::Ptr _response;
    boost::shared_ptr<CommandConnection> _connection;
    runjob::commands::error::rc _status;
    std::ostringstream _message;
};

} // tool
} // job
} // server 
} // runjob

#endif
