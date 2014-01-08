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
 * \file MMCSCommand_refresh_config.cc
 */

#include <iostream>
#include <fstream>
#include "MMCSCommand_refresh_config.h"
#include "MMCSCommandReply.h"
#include "MMCSConsolePort.h"
#include "DBConsoleController.h"
#include "DBBlockController.h"
#include "MMCSProperties.h"

using namespace std;

MMCSCommand_refresh_config*
MMCSCommand_refresh_config::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(ADMIN);             // 'help admin'  will include this command's summary
    commandAttributes.bgadminAuth(true);
    return new MMCSCommand_refresh_config("refresh_config", "refresh_config [file]", commandAttributes);
}


void
MMCSCommand_refresh_config::execute(
        deque<string> args,
        MMCSCommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget)
{
    std::string configfile;
    if ( args.size() == 1 ) {
        configfile = args[0];
    } else if ( args.empty() ) {
        configfile = MMCSProperties::getProperties()->getFilename();
    } else {
        reply << FAIL << "args? " << usage <<  DONE;
        return;
    }

    try {
        MMCSProperties::getProperties()->reload( configfile );
        MMCSProperties::reload();
        reply << OK << DONE;
    }
    catch ( const std::exception& e )
    {
        reply << FAIL << e.what() << DONE;
    }
}

void
MMCSCommand_refresh_config::help(deque<string> args,
			   MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
	  << ";Rereads config file."
	  << DONE;
}
