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
 * \file MMCSCommand_lite.h
 * \brief Definition of commands specific to mmcs_lite.
 */

#ifndef MMCSCOMMAND_LITE_H
#define MMCSCOMMAND_LITE_H

#include "common/AbstractCommand.h"


namespace mmcs {
namespace lite {


/*!
//  gen_block
//  syntax:
//     gen_block <block> <midplane> <cnodes> <nodecard> [ options ] - define a block of nodes to be controlled by mmcs
*/
class MMCSCommand_gen_block : public common::AbstractCommand
{
public:
    MMCSCommand_gen_block(const char* name, const char* description, const Attributes& attributes)
      : common::AbstractCommand(name,description,attributes) { _usage = "gen_block <blockid> <midplane>  <cnodes> <nodecard> [ node ] [ options ]";}
    static MMCSCommand_gen_block* build();    // factory method
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             common::ConsoleController* pController,
             server::BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    void help(std::deque<std::string> args,
              mmcs_client::CommandReply& reply);
};


/*!
//  gen_io_block
//  syntax:
//     gen_io_block <block> <drawer> <ionodes> <node> [ options ] - define a block of nodes to be controlled by mmcs
*/
class MMCSCommand_gen_io_block : public common::AbstractCommand
{
public:
    MMCSCommand_gen_io_block(const char* name, const char* description, const Attributes& attributes)
      : common::AbstractCommand(name,description,attributes) { _usage = "gen_io_block <blockid> <drawer> <ionodes> <node> [ options ]";}
    static MMCSCommand_gen_io_block* build();    // factory method
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             common::ConsoleController* pController,
             server::BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    void help(std::deque<std::string> args,
              mmcs_client::CommandReply& reply);
};

/*!
//  dump_ras
//  syntax:
//     dump_ras [filename]
//
*/
class MMCSCommand_dump_ras : public common::AbstractCommand
{
public:
    MMCSCommand_dump_ras(const char* name, const char* description, const Attributes& attributes)
      : common::AbstractCommand(name,description,attributes) { _usage = "dump_ras [filename]";}
    static MMCSCommand_dump_ras* build();    // factory method
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             common::ConsoleController* pController,
             server::BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    void help(std::deque<std::string> args,
              mmcs_client::CommandReply& reply);
};


/*!
//  show_envs
//  syntax:
//     show_envs <location>
//
*/
class MMCSCommand_show_envs : public common::AbstractCommand
{
public:
    MMCSCommand_show_envs(const char* name, const char* description, const Attributes& attributes)
      : common::AbstractCommand(name,description,attributes) { _usage = "show_envs <type> <location>";}
    static MMCSCommand_show_envs* build();    // factory method
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             common::ConsoleController* pController,
             server::BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    void help(std::deque<std::string> args,
              mmcs_client::CommandReply& reply);
};


/*!
//  bringup
//  syntax:
//     bringup [options]
//
*/
class MMCSCommand_bringup : public common::AbstractCommand
{
public:
    MMCSCommand_bringup(const char* name, const char* description, const Attributes& attributes)
      : common::AbstractCommand(name,description,attributes) { _usage = "bringup [options]";}
    static MMCSCommand_bringup* build();    // factory method
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             common::ConsoleController* pController,
             server::BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    void help(std::deque<std::string> args,
              mmcs_client::CommandReply& reply);
};


/*!
//  wait_for_terminate
//  syntax:
//     wait_for_terminate [seconds]
//
*/
class MMCSCommand_wait_for_terminate : public common::AbstractCommand
{
public:
    MMCSCommand_wait_for_terminate(const char* name, const char* description, const Attributes& attributes)
      : common::AbstractCommand(name,description,attributes) { _usage = "wait_for_terminate [seconds]";}
    static MMCSCommand_wait_for_terminate* build();    // factory method
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             common::ConsoleController* pController,
             server::BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    void help(std::deque<std::string> args,
              mmcs_client::CommandReply& reply);
};

//

// this is an alias for disconnect
class MMCSCommand_d : public common::AbstractCommand
{
public:
    MMCSCommand_d(const char* name, const char* description, const Attributes& attributes)
      : common::AbstractCommand(name,description,attributes) { _usage = "d";}
    static MMCSCommand_d* build();    // factory method
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             common::ConsoleController* pController,
             server::BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    void help(std::deque<std::string> args,
              mmcs_client::CommandReply& reply);
};

/*!
** delete_block
** Perform a disconnect on all the connections associated with the current block
** and delete the current block
*/
class MMCSCommand_delete_block : public common::AbstractCommand
{
public:
    MMCSCommand_delete_block(const char* name, const char* description, const Attributes& attributes)
      : common::AbstractCommand(name,description,attributes) { _usage = "delete_block";}
    static MMCSCommand_delete_block* build();    // factory method
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             common::ConsoleController* pController,
             server::BlockControllerTarget* pTarget=NULL);
    void help(std::deque<std::string> args,
              mmcs_client::CommandReply& reply);
};

class MMCSCommand_boot_block : public common::AbstractCommand
{
public:
    MMCSCommand_boot_block(const char* name, const char* description, const Attributes& attributes)
      : common::AbstractCommand(name,description,attributes) { _usage = "boot_block [ options ]";}
    static MMCSCommand_boot_block* build();	// factory method
    void execute(std::deque<std::string> args,
		 mmcs_client::CommandReply& reply,
		 common::ConsoleController* pController,
		 server::BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true;}
    void help(std::deque<std::string> args,
	      mmcs_client::CommandReply& reply);
};


} } // namespace mmcs::lite


#endif
