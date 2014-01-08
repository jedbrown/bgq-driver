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
#ifndef RUNJOB_SERVER_HANDLERS_DUMP_PROCTABLE_H
#define RUNJOB_SERVER_HANDLERS_DUMP_PROCTABLE_H

#include "server/CommandHandler.h"
#include "server/fwd.h"

#include "common/commands/DumpProctable.h"

#include <boost/enable_shared_from_this.hpp>

namespace runjob {
namespace server {
namespace handlers {

/*!
 * \brief Handles the start_tool command.
 */
class DumpProctable : public CommandHandler, public boost::enable_shared_from_this<DumpProctable>
{
public:
    /*!
     * \brief ctor.
     */
    DumpProctable(
            const boost::shared_ptr<Server>& server          //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~DumpProctable();

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
    void findHandler(
            const boost::shared_ptr<Job>& job
            );

    void validateRequest(
            const boost::shared_ptr<Job>& job
            );

    void addBoilerPlate(
            const boost::shared_ptr<Job>& job
            );

    void addIo(
            const boost::shared_ptr<Job>& job
            );

    void addProctable(
            const boost::shared_ptr<Job>& job
            );

private:
    runjob::commands::request::DumpProctable::Ptr _request;
    const runjob::commands::response::DumpProctable::Ptr _response;
    boost::shared_ptr<CommandConnection> _connection;
    runjob::commands::error::rc _error;
    std::ostringstream _message;
};

} // handlers
} // server
} // runjob

#endif
