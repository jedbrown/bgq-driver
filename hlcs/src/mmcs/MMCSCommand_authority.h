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

#ifndef _MMCSCOMMAND_AUTHORITY_H
#define _MMCSCOMMAND_AUTHORITY_H

#include "MMCSCommand.h"
#include <vector>

// These are the only commands that 'do' their own security instead
// of going through the command processor.  While they operate
// on security objects, blocks, they don't use CRUDE actions.

/*!
 * Grant authority to security objects
*/
class MMCSCommand_grant_block_authority: public MMCSCommand
{
public:
    MMCSCommand_grant_block_authority(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "grant_block_authority < block > < user > < actions >";}
    static  MMCSCommand_grant_block_authority* build();	// factory method
    static  std::string cmdname() { return "grant_block_authority"; }
    void execute(std::deque<std::string> args,
                 MMCSCommandReply& reply,
                 DBConsoleController* pController,
                 BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() != 3) return false; else return true;}
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};

/*!
 * Revoke authority to security objects
*/
class MMCSCommand_revoke_block_authority: public MMCSCommand
{
public:
    MMCSCommand_revoke_block_authority(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "revoke_block_authority < block > < user > < action >";}
    static  MMCSCommand_revoke_block_authority* build();	// factory method
    static  std::string cmdname() { return "revoke_block_authority"; }
    void execute(std::deque<std::string> args,
                 MMCSCommandReply& reply,
                 DBConsoleController* pController,
                 BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() != 3) return false; else return true;}
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};

/*!
 * List authority to security objects
*/
class MMCSCommand_list_block_authority: public MMCSCommand
{
public:
    MMCSCommand_list_block_authority(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "list_block_authority < block >";}
    static  MMCSCommand_list_block_authority* build();	// factory method
    static  std::string cmdname() { return "list_block_authority"; }
    void execute(std::deque<std::string> args,
                 MMCSCommandReply& reply,
                 DBConsoleController* pController,
                 BlockControllerTarget* pTarget=NULL,
                 std::vector<std::string>* validnames = 0);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() != 1) return false; else return true;}
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};

#endif
