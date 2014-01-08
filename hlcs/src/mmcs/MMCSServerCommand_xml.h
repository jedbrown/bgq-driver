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
 * \file MMCSServerCommand_xml.h
 */

#ifndef _MMCSSERVERCOMMAND_XML_H
#define _MMCSSERVERCOMMAND_XML_H

#include "MMCSCommand.h"

/*
** dump_machine <file.xml>
** Export a machine from the database into <file.xml>
*/
class MMCSServerCommand_dump_machine: public MMCSCommand
{
public:
    MMCSServerCommand_dump_machine(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "dump_machine <file.xml>"; }
    static  MMCSServerCommand_dump_machine* build();    // factory method
    static  std::string cmdname() { return "dump_machine"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget=NULL);
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

/*
** dump_block <file.xml> <blockId>
** Export a block <blockId> from the database into <file.xml>
*/
class MMCSServerCommand_dump_block: public MMCSCommand
{
public:
    MMCSServerCommand_dump_block(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "dump_block <file.xml> <blockId>"; }
    static  MMCSServerCommand_dump_block* build();    // factory method
    static  std::string cmdname() { return "dump_block"; }
    void execute(deque<string> args,
                 MMCSCommandReply& reply,
                 ConsoleController* pController,
                 BlockControllerTarget* pTarget);
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget,
             std::vector<std::string>* validnames);
    virtual std::vector<std::string> getBlockObjects(std::deque<std::string>& cmdString, DBConsoleController* pController);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() != 1) return false; else return true;}
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

#endif
