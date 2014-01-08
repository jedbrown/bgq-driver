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
/* (C) Copyright IBM Corp.  2005, 2011                              */
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

#include "RefreshConfig.h"

#include "../DBConsoleController.h"

#include "common/Properties.h"

#include "libmmcs_client/CommandReply.h"

#include <ras/include/RasEventHandlerChain.h>

#include <iostream>
#include <fstream>

using namespace std;

using mmcs::common::Properties;

namespace mmcs {
namespace server {
namespace command {

RefreshConfig*
RefreshConfig::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(common::ADMIN);             // 'help admin'  will include this command's summary
    commandAttributes.bgadminAuth(true);
    return new RefreshConfig("refresh_config", "refresh_config [file]", commandAttributes);
}

void
RefreshConfig::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    std::string configfile;
    if ( args.size() == 1 ) {
        configfile = args[0];
    } else if ( args.empty() ) {
        configfile = Properties::getProperties()->getFilename();
    } else {
        reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
        return;
    }

    try {
        Properties::getProperties()->reload( configfile );
        Properties::reload();
        RasEventHandlerChain::reinitialized();
        RasEventHandlerChain::setProperties(Properties::getProperties());
        RasEventHandlerChain::initChain();
        reply << mmcs_client::OK << mmcs_client::DONE;
    } catch ( const std::exception& e ) {
        reply << mmcs_client::FAIL << e.what() << mmcs_client::DONE;
    }
}

void
RefreshConfig::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description() << ";Rereads config file." << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
