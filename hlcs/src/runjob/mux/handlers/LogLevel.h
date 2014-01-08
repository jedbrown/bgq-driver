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
#ifndef RUNJOB_MUX_HANDLERS_LOG_LEVEL_H
#define RUNJOB_MUX_HANDLERS_LOG_LEVEL_H

#include "mux/CommandHandler.h"
#include "mux/fwd.h"

namespace runjob {
namespace mux {
namespace handlers {

/*!
 * \brief Handles the log_level command.
 */
class LogLevel : public CommandHandler
{
public:
    /*!
     * \brief ctor.
     */
    LogLevel();

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
};

} // handlers
} // mux
} // runjob

#endif
