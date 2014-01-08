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
 * \file MMCSCommand_list.h
 */

#ifndef _MMCSCOMMAND_LIST_H
#define _MMCSCOMMAND_LIST_H

#include "MMCSCommand.h"

/*!
 ** list_blocks
 ** Lists allocated blocks. Output includes user, number of consoles started, and if output is redirected to console.
 */
class MMCSCommand_list_blocks: public MMCSCommand
{
    public:
        MMCSCommand_list_blocks(const char* name, const char* description, const MMCSCommandAttributes& attributes)
            : MMCSCommand(name,description,attributes) { usage = "list_blocks"; }
        static  MMCSCommand_list_blocks* build();    // factory method
        static  std::string cmdname() { return "list_blocks"; }
        void execute(std::deque<std::string> args,
                MMCSCommandReply& reply,
                DBConsoleController* pController,
                BlockControllerTarget* pTarget,
                std::vector<std::string>* validnames);
        void execute(std::deque<std::string> args,
                MMCSCommandReply& reply,
                DBConsoleController* pController,
                BlockControllerTarget* pTarget);
        std::vector<std::string> getBlockObjects(std::deque<std::string>& cmdString, DBConsoleController* pController);
        void help(std::deque<std::string> args,
                MMCSCommandReply& reply);
};

/*!
 ** list_users
 ** List mmcs users.  Output includes thread number, block ID and if output is redirected to console.
 */
class MMCSCommand_list_users: public MMCSCommand
{
    public:
        MMCSCommand_list_users(const char* name, const char* description, const MMCSCommandAttributes& attributes)
            : MMCSCommand(name,description,attributes) { usage = "list_users";}
        static  MMCSCommand_list_users* build();    // factory method
        static  std::string cmdname() { return "list_users"; }
        void execute(std::deque<std::string> args,
                MMCSCommandReply& reply,
                DBConsoleController* pController,
                BlockControllerTarget* pTarget=NULL);
        void help(std::deque<std::string> args,
                MMCSCommandReply& reply);
};

/*!
 ** list <db2table> [<value>]
 ** Print the contents of <db2table>.
 ** Optional <value> specifies contents of a field within that table.
 ** Following are the only valid table / field combinations that can be used.
 ** bgqnode / location
 ** bgqnodecard / location
 ** bgqprocessorcard / location
 ** bgqmidplane / location
 ** bgqmachine / alias
 ** bgqblock / blockid (wild card with %)
 ** bgqproducttype / productid
 ** bgqeventlog / block
 */
class MMCSCommand_list: public MMCSCommand
{
    public:
        MMCSCommand_list(const char* name, const char* description, const MMCSCommandAttributes& attributes)
            : MMCSCommand(name,description,attributes) { usage = "list <db2table> [<value>]"; }
        static  MMCSCommand_list* build();    // factory method
        static  std::string cmdname() { return "list"; }
        void execute(std::deque<std::string> args,
                MMCSCommandReply& reply,
                ConsoleController* pController,
                BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { if (args.size() < 1) return false; else return true; }
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};


/*!
 ** list_midplanes
 ** Prints midplane information, including blocks that are booted on that midplane.
 */
class MMCSCommand_list_midplanes: public MMCSCommand
{
    public:
        MMCSCommand_list_midplanes(const char* name, const char* description, const MMCSCommandAttributes& attributes)
            : MMCSCommand(name,description,attributes) { usage = "list_midplanes"; }
        static  MMCSCommand_list_midplanes* build();    // factory method
        static  std::string cmdname() { return "list_midplanes"; }
        void execute(std::deque<std::string> args,
                MMCSCommandReply& reply,
                ConsoleController* pController,
                BlockControllerTarget* pTarget=NULL);
        void help(std::deque<std::string> args,
                MMCSCommandReply& reply);
};


/*!
 ** list_io
 ** Prints IO drawer information, including blocks that are booted on that drawer.
 */
class MMCSCommand_list_io: public MMCSCommand
{
    public:
        MMCSCommand_list_io(const char* name, const char* description, const MMCSCommandAttributes& attributes)
            : MMCSCommand(name,description,attributes) { usage = "list_io"; }
        static  MMCSCommand_list_io* build();    // factory method
        static  std::string cmdname() { return "list_io"; }
        void execute(std::deque<std::string> args,
                MMCSCommandReply& reply,
                ConsoleController* pController,
                BlockControllerTarget* pTarget=NULL);
        bool checkArgs(std::deque<std::string>& args) { return true; }
        void help(std::deque<std::string> args,
                MMCSCommandReply& reply);
};

/*!
 ** list_io_links
 ** Prints IO link information for a specific block.
 */
class MMCSCommand_list_io_links: public MMCSCommand
{
    public:
        MMCSCommand_list_io_links(const char* name, const char* description, const MMCSCommandAttributes& attributes)
            : MMCSCommand(name,description,attributes) { usage = "list_io_links <blockId>"; }
        static  MMCSCommand_list_io_links* build();    // factory method
        static  std::string cmdname() { return "list_io_links"; }
        void execute(
                std::deque<std::string> args,
                MMCSCommandReply& reply,
                ConsoleController* pController,
                BlockControllerTarget* pTarget,
                std::vector<std::string>* validnames
                );
        void execute(
                std::deque<std::string> args,
                MMCSCommandReply& reply,
                ConsoleController* pController,
                BlockControllerTarget* pTarget
                );
        std::vector<std::string> getBlockObjects(std::deque<std::string>& cmdString, DBConsoleController* pController);
        bool checkArgs(std::deque<std::string>& args) { if ( args.size() != 1) return false; return true; }
        void help(
                std::deque<std::string> args,
                MMCSCommandReply& reply
                );
};


/*!
 ** get_block_info [<blockId>]
 ** Prints boot information, boot mode and status for specified block.
 */
class MMCSCommand_get_block_info: public MMCSCommand
{
    public:
        MMCSCommand_get_block_info(const char* name, const char* description, const MMCSCommandAttributes& attributes)
            : MMCSCommand(name,description,attributes) { usage = "get_block_info <blockId>"; }
        static  MMCSCommand_get_block_info* build();    // factory method
        static  std::string cmdname() { return "get_block_info"; }
        void execute(std::deque<std::string> args,
                MMCSCommandReply& reply,
                ConsoleController* pController,
                BlockControllerTarget* pTarget=NULL,
                std::vector<std::string>* validnames = 0);
    std::vector<std::string> getBlockObjects(std::deque<std::string>& cmdString, DBConsoleController* pController);
    bool checkArgs(std::deque<std::string>& args) { if (args.size() != 1) return false; else return true; }
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

#endif
