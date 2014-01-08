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
 * \file MMCSCommand_free.h
 */

#ifndef _MMCSCOMMAND_FREE_H
#define _MMCSCOMMAND_FREE_H

#include "MMCSCommand.h"

/*!
** Release a specified block.  Drops the ido connections and marks a block as free in the BGQBLOCK table.
*/
class MMCSCommand_free: public MMCSCommand
{
public:
    MMCSCommand_free(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "free <blockid>";}
    static  MMCSCommand_free* build();	// factory method
    static  std::string cmdname() { return "free"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 DBConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 DBConsoleController* pController,
                         BlockControllerTarget* pTarget,
                         std::vector<std::string>* validnames);
    bool checkArgs(std::deque<std::string>& args) { if (args.size() != 1) return false; else return true; }
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};

/*!
** Release a selected block.  An alternate form of free that operates on the currently selected block.
*/
class MMCSCommand_free_block: public MMCSCommand
{
public:
    MMCSCommand_free_block(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "free_block";}
    static  MMCSCommand_free_block* build();	// factory method
    static  std::string cmdname() { return "free_block"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 DBConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};


/*!
 *
 */
class MMCSCommand_free_all: public MMCSCommand
{
    std::vector<std::string> _blocks;
public:
    MMCSCommand_free_all(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "free_all"; }
    static  MMCSCommand_free_all* build();	// factory method
    static  std::string cmdname() { return "free_all"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 DBConsoleController* pController,
                         BlockControllerTarget* pTarget = 0);
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 DBConsoleController* pController,
                         BlockControllerTarget* pTarget,
                         std::vector<std::string>* validnames);
    bool checkArgs(std::deque<std::string>& args) { if (args.size() > 1) return false; else return true; }
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
    std::vector<std::string> getBlockObjects(std::deque<std::string>& cmdString, DBConsoleController* pController);
};

#endif
