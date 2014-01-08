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
 * \file MMCSCommand_version.cc
 */

#include "MMCSCommand_version.h"
#include "MMCSCommandReply.h"
#include "MMCSProperties.h"

using namespace std;

MMCSCommand_version*
MMCSCommand_version::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.internalAuth(true);
    commandAttributes.helpCategory(DEFAULT);
    return new MMCSCommand_version("version", "version", commandAttributes);
}


void
MMCSCommand_version::execute(
        deque<string> args,
        MMCSCommandReply& reply,
        ConsoleController* pController,
        BlockControllerTarget* pTarget
        )
{
    reply << OK << "BG/Q " << MMCSProperties::getProperty(MMCS_PROCESS)
        << " " << MMCSProperties::getProperty(MMCS_VERSION)
        << DONE;
}

void
MMCSCommand_version::help(
        deque<string> args,
        MMCSCommandReply& reply
        )
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
        << ";Displays the mmcs version"
        << DONE;
}

