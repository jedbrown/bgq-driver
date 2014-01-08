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
 * \file MMCSCommand_allocate.h
 */

#ifndef _MMCSCOMMAND_ALLOCATE_H
#define _MMCSCOMMAND_ALLOCATE_H

#include "MMCSCommand.h"
#include <vector>

/*!
** For specified <blockId>, marks block as allocated but does not boot the block.
** Ido connections are established through the proxy.
** The block is marked as allocated in the database and owner is changed to issuer of the allocate_block.
** The command completes as soon as the proxy connections are established.
** If virtual_node_mode is not specified, mode will default to coprocessor.
** Block will revert to coprocessor mode each time it is freed.
*/
class MMCSCommand_allocate_block: public MMCSCommand
{
public:
    MMCSCommand_allocate_block(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "allocate_block <blockId> [ options ]";}
    static  MMCSCommand_allocate_block* build();	// factory method
    static  std::string cmdname() { return "allocate_block"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 DBConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 DBConsoleController* pController,
                         BlockControllerTarget* pTarget,
                         std::vector<std::string>* validnames);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() == 0) return false; else return true;}
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};

/*!
** For specified <blockId>, performs select_block, allocate_block, boot_block and wait_boot.
** <blockId> identifies a block in the BGQBLOCK table.
** This command can be used to initialize a block in free state.
** If virtual_node_mode is not specified, mode will default to coprocessor.
** Block will revert to coprocessor mode each time it is freed.
*/
class MMCSCommand_allocate: public MMCSCommand
{
public:
    MMCSCommand_allocate(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "allocate <blockId> "; }
    static  MMCSCommand_allocate* build();	// factory method
    static  std::string cmdname() { return "allocate"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 DBConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 DBConsoleController* pController,
                         BlockControllerTarget* pTarget,
                         std::vector<std::string>* validnames);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() != 1) return false; else return true;}
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
private:
    void recoverL3Major(std::vector<std::string>& failingNodes,
			std::deque<std::string> allocateArgs,
			MMCSCommandReply& reply,
			DBConsoleController* pController);
};

/*!
** boot_block
** Boots a previously allocated block.
** If a block is in an error state, reset the block first using free_block and allocate_block.
*/
class MMCSServerCommand_boot_block: public MMCSCommand
{
public:
    MMCSServerCommand_boot_block(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "boot_block [update] [<options>]";}
    static  MMCSServerCommand_boot_block* build();	// factory method
    static  std::string cmdname() { return "boot_block"; }
    void execute(std::deque<std::string> args,
		 MMCSCommandReply& reply,
		 DBConsoleController* pController,
		 BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true;}
    void help(std::deque<std::string> args,
	      MMCSCommandReply& reply);
};

/*!
** wait_boot
** Wait for the selected block to complete booting.
** The command does not complete until the block is fully initialized to the point that mmcs can communicate with ciod on all of the I/O nodes.
*/
class MMCSCommand_wait_boot: public MMCSCommand
{
public:
    MMCSCommand_wait_boot(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "wait_boot [<minutes>]";}
    static  MMCSCommand_wait_boot* build();	// factory method
    static  std::string cmdname() { return "wait_boot"; }
    void execute(std::deque<std::string> args,
		 MMCSCommandReply& reply,
		 DBConsoleController* pController,
		 BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() > 1) return false; else return true;}
    void help(std::deque<std::string> args,
	      MMCSCommandReply& reply);
};

#endif
