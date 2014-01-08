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
#ifndef RUNJOB_MUX_HANDLERS_REFRESH_CONFIG_H
#define RUNJOB_MUX_HANDLERS_REFRESH_CONFIG_H

#include "common/ConnectionContainer.h"

#include "mux/CommandHandler.h"
#include "mux/fwd.h"
#include "mux/Plugin.h"

#include <hlcs/include/runjob/commands/RefreshConfig.h>

#include <boost/enable_shared_from_this.hpp>

namespace runjob {
namespace mux {
namespace handlers {

/*!
 * \brief Handles the runjob_mux_refresh_config command.
 */
class RefreshConfig : public CommandHandler, public boost::enable_shared_from_this<RefreshConfig>
{
public:
    /*!
     * \brief ctor.
     */
    RefreshConfig(
            const boost::weak_ptr<Multiplexer>& mux  //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~RefreshConfig();

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
    boost::weak_ptr<Multiplexer> _mux;
    boost::shared_ptr<CommandConnection> _connection;
    const runjob::commands::response::RefreshConfig::Ptr _response;
};

} // handlers
} // mux
} // runjob

#endif
