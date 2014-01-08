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
 * \file MMCSCommand_mcserver.h
 * Commands for the mcServer.
 */

#ifndef MMCSCOMMAND_MCSERVER_H
#define MMCSCOMMAND_MCSERVER_H

#include "MMCSCommand.h"

class MMCSCommand_mcserver_status : public MMCSCommand
{
public:
    MMCSCommand_mcserver_status(const char* name, const char* description, const MMCSCommandAttributes& attributes)
        : MMCSCommand(name,description,attributes) { usage = "mcserver_status [<regexp>]";}
    static MMCSCommand_mcserver_status* build();
    static std::string cmdname() { return "mcserver_status"; }
    void execute(std::deque<std::string> args,
            MMCSCommandReply& reply,
            ConsoleController* pController,
            BlockControllerTarget* pTarget = NULL);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    void help(std::deque<std::string> args,
            MMCSCommandReply& reply);
};

class MMCSCommand_mcserver_clients : public MMCSCommand
{
public:
    MMCSCommand_mcserver_clients(const char* name, const char* description, const MMCSCommandAttributes& attributes)
        : MMCSCommand(name,description,attributes) { usage = "mcserver_clients"; }
    static MMCSCommand_mcserver_clients* build();
    static std::string cmdname() { return "mcserver_clients"; }
    void execute(std::deque<std::string> args,
            MMCSCommandReply& reply,
            ConsoleController* pController,
            BlockControllerTarget* pTarget = NULL);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    void help(std::deque<std::string> args,
            MMCSCommandReply& reply);
};

#endif
