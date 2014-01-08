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

#ifndef _MMCSSERVERCOMMAND_DISCONNECT_H
#define _MMCSSERVERCOMMAND_DISCONNECT_H

#include "MMCSCommand.h"


/*!
//  disconnect
//  syntax:
//     [<target>] disconnect - disconnect from the block's resources
*/
class MMCSServerCommand_disconnect: public MMCSCommand
{
public:
    MMCSServerCommand_disconnect(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "disconnect";}
    static  MMCSServerCommand_disconnect* build();	// factory method
    static  std::string cmdname() { return "disconnect"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 ConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};

#endif
