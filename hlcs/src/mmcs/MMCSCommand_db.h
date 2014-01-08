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
 * \file MMCSCommand_db.h
 */

#ifndef _MMCSCOMMAND_DB_H
#define _MMCSCOMMAND_DB_H

#include "MMCSCommand.h"

class MMCSCommand_kill_midplane_jobs: public MMCSCommand
{
public:
    MMCSCommand_kill_midplane_jobs(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "kill_midplane_jobs <location> [kill|list]";}
    static  MMCSCommand_kill_midplane_jobs* build();    // factory method
    static  string cmdname() { return "kill_midplane_jobs"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             DBConsoleController* pController,
             BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { if (args.size() < 1 || args.size() > 2) return false; else return true; }
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};


class MMCSCommand_complete_io_service: public MMCSCommand
{
public:
    MMCSCommand_complete_io_service(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "complete_io_service <location>";}
    static  MMCSCommand_complete_io_service* build();    // factory method
    static  string cmdname() { return "complete_io_service"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             DBConsoleController* pController,
             BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { if (args.size() != 1) return false; else return true;}
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

/*!
** delete <db2table> <id>
** Delete record <id> from <db2table>
** db2table is one of the following:
**    bgqblock
*/
class MMCSCommand_delete: public MMCSCommand
{
public:
    MMCSCommand_delete(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "delete <db2table> <id>";}
    static  MMCSCommand_delete* build();    // factory method
    static  std::string cmdname() { return "delete"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { if (args.size() != 2) return false; else return true; }
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

/*!
** sql <sql-string>
** execute <sql-string>
** This command cannot be used to run any sql statement that returns output (i.e., queries).
*/
class MMCSCommand_sql: public MMCSCommand
{
public:
    MMCSCommand_sql(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "sql <sql-string>";}
    static  MMCSCommand_sql* build();    // factory method
    static  std::string cmdname() { return "sql"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { if (args.size() == 0) return false; else return true; }
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

#endif
