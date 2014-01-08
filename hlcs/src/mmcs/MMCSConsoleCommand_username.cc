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
 * \file MMCSConsoleCommand_username.cc
 * \brief Set the username.
 */

#include "ConsoleController.h"
#include "MMCSConsoleCommand_username.h"
#include "MMCSCommandReply.h"
#include "MMCSCommandProcessor.h"

#include <db/include/api/tableapi/gensrc/DBTJob.h>

#include <boost/foreach.hpp>

using namespace std;

MMCSConsoleCommand_username*
MMCSConsoleCommand_username::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsConsoleCommand(true);
    commandAttributes.internalAuth(true);
    commandAttributes.helpCategory(DEFAULT);
    return new MMCSConsoleCommand_username("username", "username [-v]", commandAttributes);
}

void
MMCSConsoleCommand_username::execute(
        deque<string> args,
        MMCSCommandReply& reply,
        ConsoleController* pController,
        BlockControllerTarget* pTarget
        )
{
    // add username
    if ( args.size() == 0 ) {
        reply << OK << pController->getUser().getUser();
    } else if ( args.size() == 1 ) {
        // add secondary groups
        reply << OK << pController->getUser().getUser() << ";";
        if ( args[0] == "-v" ) {
            reply << ";secondary groups:";
            BOOST_FOREACH( const bgq::utility::UserId::Group& group, pController->getUser().getGroups() ) {
                reply << ";" << group.second;
            }
        } else {
            reply << FAIL << "args? " << usage;
        }
    } else {
        reply << FAIL << "args? " << usage;
    }

    reply << DONE;
}

void
MMCSConsoleCommand_username::help(
        deque<string> args,
        MMCSCommandReply& reply)
{
    reply << OK << description() << ";Print current username.";
    reply << ";";
    reply << ";\toptions";
    reply << ";\t -v    verbose mode, show secondary groups too";
    reply << DONE;
}

