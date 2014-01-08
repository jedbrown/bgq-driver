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
#ifndef RUNJOB_SERVER_HANDLERS_CHANGE_CIOS_CONFIG_H
#define RUNJOB_SERVER_HANDLERS_CHANGE_CIOS_CONFIG_H

#include "common/commands/ChangeCiosConfig.h"

#include "server/block/Container.h"
#include "server/block/fwd.h"

#include "server/CommandHandler.h"
#include "server/fwd.h"

#include <boost/enable_shared_from_this.hpp>

namespace runjob {
namespace server {
namespace handlers {

/*!
 * \brief Handles the cios_log_level command.
 */
class ChangeCiosConfig : public CommandHandler, public boost::enable_shared_from_this<ChangeCiosConfig>
{
public:
    /*!
     * \brief ctor.
     */
    ChangeCiosConfig(
            const boost::shared_ptr<Server>& server          //!< [in]
          );

    /*!
     * \brief dtor.
     */
    ~ChangeCiosConfig();

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
    void getBlocksHandler(
            const block::Container::Blocks& blocks
            );

    void impl(
            const boost::shared_ptr<Block>& block,
            const block::Container::Blocks& blocks
            );
    
    void impl(
            const boost::shared_ptr<block::IoNode>& node
            );

    void control(
            const boost::shared_ptr<block::IoNode>& node
            );
    
    void data(
            const boost::shared_ptr<block::IoNode>& node
            );
private:
    runjob::commands::request::ChangeCiosConfig::Ptr _request;
    runjob::commands::response::ChangeCiosConfig::Ptr _response;
    boost::shared_ptr<CommandConnection> _connection;
};

} // handlers
} // server
} // runjob

#endif
