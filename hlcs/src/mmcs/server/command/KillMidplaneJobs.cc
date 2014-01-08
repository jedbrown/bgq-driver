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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

#include "KillMidplaneJobs.h"

#include "../DBConsoleController.h"
#include "../RunJobConnection.h"

#include <db/include/api/BGQDBlib.h>

using namespace std;

namespace mmcs {
namespace server {
namespace command {

KillMidplaneJobs*
KillMidplaneJobs::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false) ;      // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.internalCommand(true);
    commandAttributes.helpCategory(common::SPECIAL);
    commandAttributes.bgadminAuth(true);
    return new KillMidplaneJobs("kill_midplane_jobs", "kill_midplane_jobs <location> [kill|list]", commandAttributes);
}

void
KillMidplaneJobs::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    BGQDB::STATUS result;

    vector<BGQDB::job::Id> jobs;

    bool listOnly = false;

    if (args.size() > 1)   {
        string subcmd( args[1] );
        if (subcmd == string("kill"))  {
            // Nothing to do.
        } else if (subcmd == string("list")) {
            listOnly = true;
        } else {
            // Invalid argument
            reply << mmcs_client::FAIL << "Use kill or list" << mmcs_client::DONE;
            return;
        }
    }

    if (pController->getBlockHelper() != 0)
        pController->deselectBlock();  // If we have a selected block, deselect it.

    // first call to get list (3rd argument is "listOnly")
    result = BGQDB::killMidplaneJobs(args[0].c_str(), &jobs, true);
    if (result != BGQDB::OK) {
        reply << mmcs_client::FAIL << "Error on BGQDB::KillMidplaneJobs" << mmcs_client::DONE;
        return;
    }

    if (listOnly == true) {
        reply << mmcs_client::OK;
        // list was requested, not kill, so just return the list of job IDs to the caller
        for (vector<BGQDB::job::Id>::const_iterator i(jobs.begin()) ; i != jobs.end() ; ++i ) {
            if ( i != jobs.begin() ) {
                reply << ";";
            }
            reply << *i;
        }
    } else {
        const int signal = 9;

        // kill the jobs
        for (vector<BGQDB::job::Id>::const_iterator i(jobs.begin()) ; i != jobs.end() ; ++i ) {
            RunJobConnection::instance().kill(*i, signal, "Delivered due to kill_midplane_jobs " + args[0] );
        }

        // now call to free the blocks
        result = BGQDB::killMidplaneJobs(args[0].c_str(), &jobs, false);
        if (result != BGQDB::OK) {
            reply << mmcs_client::FAIL << "Error on BGQDB::KillMidplaneJobs trying to free blocks" << mmcs_client::DONE;
            return;
        }

        reply << mmcs_client::OK;
    }

    reply << mmcs_client::DONE;
}

void
KillMidplaneJobs::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Internal command, not to be used from console, only to be used internally during service actions. "
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
