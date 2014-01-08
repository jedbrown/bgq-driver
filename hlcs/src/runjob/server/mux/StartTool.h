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
#ifndef RUNTOOL_SERVER_MUX_START_TOOL_H
#define RUNTOOL_SERVER_MUX_START_TOOL_H

#include "common/message/Result.h"
#include "common/message/StartTool.h"

#include "server/mux/fwd.h"
#include "server/fwd.h"

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <string>

namespace runjob {
namespace server {
namespace mux {

/*!
 * \brief
 */
class StartTool
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<StartTool> Ptr;

    /*!
     * \brief Factory.
     */
    static void create(
            const boost::shared_ptr<Server>& server,            //!< [in]
            const boost::shared_ptr<runjob::Message>& message,  //!< [in]
            const boost::shared_ptr<Connection>& mux            //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~StartTool();

private:
    StartTool(
            const boost::shared_ptr<Server>& server,
            const boost::shared_ptr<runjob::Message>& message,
            const boost::shared_ptr<Connection>& mux
         );

    BGQDB::job::Id lookupJob() const;

    void findJobHandler(
            const boost::shared_ptr<Job>& job
            );

private:
    const boost::weak_ptr<Server> _server;
    const message::StartTool::Ptr _request;
    const message::Result::Ptr _response;
    const boost::shared_ptr<Connection> _mux;
};

} // mux
} // server
} // runjob

#endif
