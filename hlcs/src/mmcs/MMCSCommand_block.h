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
 * \file MMCSCommand_block.h
 */

#ifndef _MMCSCOMMAND_BLOCK_H
#define _MMCSCOMMAND_BLOCK_H

#include "MMCSCommand.h"

/*!
//  create_block
//  syntax:
//     create_block <bgqblock.xml> [ options ] - define a block of nodes to be controlled by mmcs
//       options ::= outfile=<filename>        - direct the mailbox output to a file.
//                   tee                       - direct the mailbox out to both the file and stdout
*/
class MMCSCommand_create_block: public MMCSCommand
{
public:
    MMCSCommand_create_block(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "create_block <bgqblock.xml> [ options ]";}
    static  MMCSCommand_create_block* build();	// factory method
    static  std::string cmdname() { return "create_block"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 ConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() == 0) return false; else return true;}
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};

/*!
//  connect
//  syntax:
//     [<target>] connect - connect to a set of the block's resources
*/
class MMCSCommand_connect: public MMCSCommand
{
public:
    MMCSCommand_connect(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "[<target>] connect [ options ]";}
    static  MMCSCommand_connect* build();	// factory method
    static  std::string cmdname() { return "connect"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 ConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>&) { return true; }
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};

/*!
//  disconnect
//  syntax:
//     [<target>] disconnect - disconnect from the block's resources
*/
class MMCSCommand_disconnect: public MMCSCommand
{
public:
    MMCSCommand_disconnect(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "disconnect";}
    static  MMCSCommand_disconnect* build();	// factory method
    static  std::string cmdname() { return "disconnect"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 ConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};

#endif
