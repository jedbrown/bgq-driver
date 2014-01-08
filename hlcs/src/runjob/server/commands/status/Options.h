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
#ifndef RUNJOB_SERVER_COMMANDS_STATUS_OPTIONS_H
#define RUNJOB_SERVER_COMMANDS_STATUS_OPTIONS_H

#include "common/commands/Options.h"
#include "common/commands/ServerStatus.h"

#include <utility/include/BoolAlpha.h>

namespace runjob {
namespace server {
namespace commands {
namespace status {

/*!
 * \brief Program options specific to the runjob_server_status command.
 */
class Options: public runjob::commands::Options
{
public:
    /*!
     * \brief ctor.
     */
    Options(
            unsigned int argc,      //!< [in]
            char** argv             //!< [in]
            );

    /*!
     * \copydoc runjob::commands::Options::doHandle
     */
    void doHandle(
            const runjob::commands::Response::Ptr&   //!< [in]
            ) const;

private:
    void doHelp(
            std::ostream& os
            ) const;
   
    const char* description() const { return "display status information from the Blue Gene runjob server."; }

    void displayConnectionPool(
            const runjob::commands::response::ServerStatus::Ptr& response
            ) const;

    void displayCiosProtocol(
            const runjob::commands::response::ServerStatus::Ptr& response
            ) const;

    void displayPerformanceCounters(
            const runjob::commands::response::ServerStatus::Ptr& response
            ) const;

    void displayConnections(
            const runjob::commands::response::ServerStatus::Ptr& response
            ) const;

    void displayJobs(
            const runjob::commands::response::ServerStatus::Ptr& response
            ) const;
    
    void displayBlocks(
            const runjob::commands::response::ServerStatus::Ptr& response
            ) const;
    
    void displayIoLinks(
            const runjob::commands::response::ServerStatus::Ptr& response
            ) const;

private:
    boost::program_options::options_description _options;   //!<
    bgq::utility::BoolAlpha _ioDetails;
};

} // status
} // commands
} // server
} // runjob

#endif
