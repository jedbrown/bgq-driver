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

/*!
 * \file DiagWait.cc
 */

#include "DiagWait.h"

#include "../BlockControllerBase.h"
#include "../BlockControllerTarget.h"
#include "../HardwareBlockList.h"

#include "common/ConsoleController.h"
#include "common/Properties.h"

#include "libmmcs_client/CommandReply.h"

#include <control/include/mcServer/MCServerRef.h>

#include <boost/thread.hpp>
#include <cstdio>
#include <csignal>
#include <sstream>

#include <fcntl.h>
#include <sched.h>
#include <sys/stat.h>
#include <sys/wait.h>

using namespace std;

namespace mmcs {
namespace server {
namespace command {

DiagWait*
DiagWait::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(true); // does require a BlockController object
    commandAttributes.requiresConnection(false); // does not require  mc_server connections
    commandAttributes.requiresTarget(true); // does require a BlockControllerTarget object
    commandAttributes.internalCommand(true); // this is an internal use command
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(common::SPECIAL);
    Attributes::AuthPair hardwareread(hlcs::security::Object::Hardware, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(hardwareread);
    return new DiagWait("diag_wait", "diag_wait [<seconds> [waitall]]", commandAttributes);
}

void
DiagWait::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    if (HardwareBlockList::list_size() != 0) {
        reply << mmcs_client::FAIL << "Subnet_mc failover in progress, try again later." << mmcs_client::DONE;
        return;
    }

    const BlockPtr pBlock = pTarget->getBlockController(); // get selected block

    // If we are not in the mode where we are accepting mailbox messages
    // then we can't ever see this message...
    if (!pBlock->isMailboxStarted()) {
        reply << mmcs_client::FAIL << "Programs are not running" << mmcs_client::DONE;
        return;
    }

    bool waitall = false;

    if ( args.size() > 1 ) {
        if ( args[1] == "waitall" ) {
            waitall = true;
        } else {
            reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
            return;
        }
    }

    int timeout = 0;
    if ( args.size() > 0 ) {
        try {
            timeout = boost::lexical_cast<int>( args[0] );
            if ( timeout <= 0 ) {
                reply << mmcs_client::FAIL << "Timeout value must be positive" << mmcs_client::DONE;
                return;
            }
        } catch ( const boost::bad_lexical_cast& e ) {
            reply << mmcs_client::FAIL << "Bad timeout value: " << args[0] << mmcs_client::DONE;
            return;
        }
    }

    time_t starttime, now;
    time(&starttime);
    double elapsed_time;
    for (
            unsigned slept = 0;
            (abs(pBlock->terminatedNodes()) != static_cast<int>(pBlock->numNodesStarted())) && (waitall || (pBlock->terminatedNodes() >= 0));
            ++slept
            )
    {
        // Check if we are disconnecting
        if (pBlock->peekDisconnecting()) {
            reply << mmcs_client::FAIL << pBlock->disconnectReason() << mmcs_client::DONE;
            return;
        }
        sleep(1);
        time(&now);
        elapsed_time = difftime(now, starttime);
        if (elapsed_time >= timeout) {
            break;
        }
    }

    if (pBlock->terminatedNodes() == static_cast<int>(pBlock->numNodesStarted())) {
        reply << mmcs_client::OK << mmcs_client::DONE;
    } else {
        if (pBlock->terminatedNodes() < 0) {
            reply << mmcs_client::FAIL << "Node returned FAIL message" << mmcs_client::DONE;
        } else {
            reply << mmcs_client::FAIL << "Timeout reached" << mmcs_client::DONE;
        }
    }
}

void
DiagWait::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description() << ";Wait for diagnostic test to terminate or time out" << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
