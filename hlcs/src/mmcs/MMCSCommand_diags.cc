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
 * \file MMCSCommand_diags.cc
 */

#include "BlockControllerTarget.h"
#include "ConsoleController.h"
#include "MMCSCommandReply.h"
#include "MMCSCommand_diags.h"
#include "MMCSProperties.h"
#include "McServerLocker.h"
#include "HardwareBlockList.h"

#include <control/include/mcServer/MCServerRef.h>

#include <utility/include/Exec.h>
#include <boost/thread.hpp>
#include <cstdio>
#include <csignal>
#include <sstream>

#include <fcntl.h>
#include <sched.h>
#include <sys/stat.h>
#include <sys/wait.h>

using namespace std;

MMCSCommand_diag_wait*
MMCSCommand_diag_wait::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(true); // does require a BlockController object
    commandAttributes.requiresConnection(false); // does not require  mc_server connections
    commandAttributes.requiresTarget(true); // does require a BlockControllerTarget object
    commandAttributes.internalCommand(true); // this is an internal use command
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(SPECIAL);
    MMCSCommandAttributes::AuthPair hardwareread(hlcs::security::Object::Hardware, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(hardwareread);
    return new MMCSCommand_diag_wait("diag_wait", "diag_wait [<seconds> [waitall]]", commandAttributes);
}

void
MMCSCommand_diag_wait::execute(deque<string> args, MMCSCommandReply& reply,
    ConsoleController* pController, BlockControllerTarget* pTarget)
{
    if(HardwareBlockList::list_size() != 0) {
        reply << FAIL << "Subnet_mc failover in progress.  Try again later." << DONE;
        return;
    }

    const BlockPtr pBlock = pTarget->getBlockController(); // get selected block

    // If we are not in the mode where we are accepting mailbox messages
    // then we can't ever see this message...
    if (!pBlock->isMailboxStarted())
    {
        reply << FAIL << "programs are not running" << DONE;
        return;
    }


    bool waitall = false;

    if ( args.size() > 1 ) {
        if ( args[1] == "waitall" ) {
            waitall = true;
        } else {
            reply << FAIL << "args? " << usage << DONE;
            return;
        }
    }

    int timeout = 0;
    if ( args.size() > 0 ) {
        try {
            timeout = boost::lexical_cast<int>( args[0] );
            if ( timeout <= 0 ) {
                reply << FAIL << "timeout must be positive" << DONE;
                return;
            }
        } catch ( const boost::bad_lexical_cast& e ) {
            reply << FAIL << "garbage timeout value: " << args[0] << DONE;
            return;
        }
    }

    time_t starttime, now;
    time(&starttime);
    double elapsed_time;
    for (unsigned slept = 0;
         (abs(pBlock->terminatedNodes()) != static_cast<int>(pBlock->numNodesStarted())) &&
             (waitall || (pBlock->terminatedNodes() >= 0));
         ++slept) {

        // Check if we are disconnecting
        if (pBlock->peekDisconnecting())
            {
                reply << FAIL << pBlock->disconnectReason() << DONE;
                return;
            }
        sleep(1);
        time(&now);
        elapsed_time = difftime(now, starttime);
        if(elapsed_time >= timeout)
            break;
    }

    if (pBlock->terminatedNodes() == static_cast<int>(pBlock->numNodesStarted())) {
        reply << OK << DONE;
    } else {
        if (pBlock->terminatedNodes() < 0) {
            reply << FAIL << "Node returned FAIL message" << DONE;
        } else {
            reply << FAIL << "Timeout reached" << DONE;
        }
    }
}

void
MMCSCommand_diag_wait::help(deque<string> args, MMCSCommandReply& reply)
{
    reply << OK << description() << ";Wait for diagnostic test to terminate or time out" << DONE;
}
