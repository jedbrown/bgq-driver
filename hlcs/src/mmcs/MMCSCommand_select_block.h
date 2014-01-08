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
 * \file MMCSCommand_select_block.h
 * select_block - make a specified block the current block
 */

#ifndef _MMCSCOMMAND_SELECT_BLOCK_H
#define _MMCSCOMMAND_SELECT_BLOCK_H

#include "MMCSCommand.h"

class MMCSCommand_select_block: public MMCSCommand
{
public:
    MMCSCommand_select_block(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "select_block <blockId>";}
    static  MMCSCommand_select_block* build();	// factory method
    static  std::string cmdname() { return "select_block"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 DBConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 DBConsoleController* pController,
                         BlockControllerTarget* pTarget,
                         std::vector<std::string>* validnames);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() != 1) return false; else return true; }
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};

class MMCSCommand_list_selected_block: public MMCSCommand
{
public:
    MMCSCommand_list_selected_block(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "list_selected_block"; }
    static  MMCSCommand_list_selected_block* build();	// factory method
    static  std::string cmdname() { return "list_selected_block"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 DBConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 DBConsoleController* pController,
                         BlockControllerTarget* pTarget,
                         std::vector<std::string>* validnames);
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};

class MMCSCommand_deselect_block: public MMCSCommand
{
public:
    MMCSCommand_deselect_block(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "deselect_block"; }
    static  MMCSCommand_deselect_block* build();	// factory method
    static  std::string cmdname() { return "deselect_block"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 DBConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};

#endif
