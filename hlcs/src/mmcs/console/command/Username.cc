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
 * \file Username.cc
 * \brief Set the username.
 */

#include "Username.h"

#include "common/ConsoleController.h"

#include <boost/foreach.hpp>

using namespace std;

namespace mmcs {
namespace console {
namespace command {

Username*
Username::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.bgConsoleCommand(true);
    commandAttributes.internalAuth(true);
    commandAttributes.helpCategory(common::DEFAULT);
    return new Username("username", "username [-v]", commandAttributes);
}

void
Username::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        server::BlockControllerTarget* pTarget
        )
{
    // add username
    if ( args.size() == 0 ) {
        reply << mmcs_client::OK << pController->getUser().getUser();
    } else if ( args.size() == 1 ) {
        // add secondary groups
        reply << mmcs_client::OK << pController->getUser().getUser() << ";";
        if ( args[0] == "-v" ) {
            reply << ";secondary groups:";
            BOOST_FOREACH( const bgq::utility::UserId::Group& group, pController->getUser().getGroups() ) {
                reply << ";" << group.second;
            }
        } else {
            reply << mmcs_client::FAIL << "args? " << _usage;
        }
    } else {
        reply << mmcs_client::FAIL << "args? " << _usage;
    }

    reply << mmcs_client::DONE;
}

void
Username::help(
        deque<string> args,
        mmcs_client::CommandReply& reply)
{
    reply << mmcs_client::OK << description() << ";Print current username.";
    reply << ";options";
    reply << ";  -v    verbose mode, show secondary groups too";
    reply << mmcs_client::DONE;
}


} } } // namespace mmcs::console::command
