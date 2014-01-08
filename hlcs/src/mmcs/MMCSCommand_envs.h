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
 * \file MMCSCommand_envs.h
 * Commands for polling the hardware
 */

#ifndef _MMCSCOMMAND_ENVS_H
#define _MMCSCOMMAND_ENVS_H

#include "MMCSCommand.h"

class MMCSCommand_start_hw_polling : public MMCSCommand
{
public:
    MMCSCommand_start_hw_polling(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "start_hw_polling <type> <location> <seconds>";}
    static MMCSCommand_start_hw_polling* build();
    static std::string cmdname() { return "start_hw_polling"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget = NULL);
    bool checkArgs(std::deque<std::string>& args) { if (args.size() != 3) return false; else return true; }
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

class MMCSCommand_stop_hw_polling : public MMCSCommand
{
public:
    MMCSCommand_stop_hw_polling(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "stop_hw_polling <location>";}
    static MMCSCommand_stop_hw_polling* build();
    static std::string cmdname() { return "stop_hw_polling"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget = NULL);
    bool checkArgs(std::deque<std::string>& args) { if (args.size() != 1) return false; else return true; }
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

class MMCSCommand_list_hw_polling : public MMCSCommand
{
public:
    MMCSCommand_list_hw_polling(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "list_hw_polling";}
    static MMCSCommand_list_hw_polling* build();
    static std::string cmdname() { return "list_hw_polling"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget = NULL);
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

#endif
