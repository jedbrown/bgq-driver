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
 * \file MMCSCommand_reboot.h
 */

#ifndef MMCSCOMMAND_REBOOT_H
#define MMCSCOMMAND_REBOOT_H

#include "MMCSCommand.h"

class MMCSCommand_reboot_nodes: public MMCSCommand
{
public:
    MMCSCommand_reboot_nodes(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "[<target>] reboot_nodes";}
    static  MMCSCommand_reboot_nodes* build();	// factory method
    static  std::string cmdname() { return "reboot_nodes"; }
    void execute(std::deque<std::string> args,
		 MMCSCommandReply& reply,
		 DBConsoleController* pController,
		 BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    void help(std::deque<std::string> args,
	      MMCSCommandReply& reply);
};

#endif
