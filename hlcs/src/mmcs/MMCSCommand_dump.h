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
 * \file MMCSCommand_dump.h
 * \brief Diagnostic commands for reading and writing scom register.
 */

#ifndef _MMCSCOMMAND_DUMP_H
#define _MMCSCOMMAND_DUMP_H

#include "MMCSCommand.h"

/*!
** dump_machine
** dump_machine
** Print the BGQMachine configuration object
*/
class MMCSCommand_dump_machine: public MMCSCommand
{
public:
    MMCSCommand_dump_machine(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "dump_machine"; }
    static  MMCSCommand_dump_machine* build();	// factory method
    static  std::string cmdname() { return "dump_machine"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 ConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};

/*!
** dump_block
** dump_block
** Print the BGQBlock configuration object
*/
class MMCSCommand_dump_block: public MMCSCommand
{
public:
    MMCSCommand_dump_block(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "dump_block";}
    static  MMCSCommand_dump_block* build();	// factory method
    static  std::string cmdname() { return "dump_block"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 ConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};

/*!
** dump_personalities
** [<target>] dump_personalities
** Print the node personality objects
*/
class MMCSCommand_dump_personalities: public MMCSCommand
{
public:
    MMCSCommand_dump_personalities(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "[<target>] dump_personalities [unformatted]"; }
    static  MMCSCommand_dump_personalities* build();	// factory method
    static  std::string cmdname() { return "dump_personalities"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 ConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

/*!
** get_block_size
** get_block_size [ a | b | c | | d | e | * ]
** Retrieve the block dimensions of selected block.
** Specifiying a, b, c, d, or e retrieves that one dimension.
** Specifying * retrieves all 5 dimensions.
*/
class MMCSCommand_get_block_size: public MMCSCommand
{
public:
    MMCSCommand_get_block_size(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "get_block_size [ a | b | c | d | e | * ]";}
    static  MMCSCommand_get_block_size* build();	// factory method
    static  std::string cmdname() { return "get_block_size"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 ConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() != 1) return false; else return true; }
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
private:
        void compute(
                const std::deque<std::string>& args,
                const BGQBlockNodeConfig* block,
                MMCSCommandReply& reply
                );

        void io(
                const std::deque<std::string>& args,
                const BGQBlockNodeConfig* block,
                MMCSCommandReply& reply
                );
};

#endif
