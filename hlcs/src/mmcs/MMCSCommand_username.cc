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
 * \file MMCSCommand_username.cc
 * \brief Set the username.
 */

#include "DBConsoleController.h"
#include "MMCSCommand_username.h"
#include "MMCSCommandReply.h"

#include <db/include/api/tableapi/gensrc/DBTJob.h>

#include <boost/foreach.hpp>
using namespace std;

MMCSCommand_username*
MMCSCommand_username::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.internalAuth(true);
    commandAttributes.helpCategory(DEFAULT);
    return new MMCSCommand_username("username", "username", commandAttributes);
}

void
MMCSCommand_username::execute(
        deque<string> args,
        MMCSCommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget
        )
{
    reply << OK << pController->getUser().getUser() << ";";
    reply << ";secondary groups:";
    BOOST_FOREACH( const bgq::utility::UserId::Group& group, pController->getUser().getGroups() ) {
        reply << ";" << group.second;
    }
    reply << DONE;
}

void
MMCSCommand_username::help(
        deque<string> args,
        MMCSCommandReply& reply
        )
{
    reply << OK << description() << ";Print current username." << DONE;
}

