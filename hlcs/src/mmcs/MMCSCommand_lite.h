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

#include "ConsoleController.h"
#include "MMCSCommand.h"

#include <boost/shared_ptr.hpp>

#include <vector>

class BGQMachineXML;
namespace lite {
class Job;
}

/*!
 * \brief sqlite'd version of a BlockController.
 */
class LiteBlockController : public BlockHelper
{
public:
    /*!
     * \brief ctor
     */
    LiteBlockController(
            BGQMachineXML* machine,         //!< [in] machine XML description.
            const std::string& userName,    //!< [in] user name
            const std::string& blockName    //!< [in] block name
            );
    LiteBlockController(BlockPtr ptr);

    /*!
     * \brief handle a RAS event
     */
    int processRASMessage(
                           RasEvent &rasEvent  //!< [in]
                           );

     /*!
     * \brief handle a console event
     */
    void processConsoleMessage(
                               MCServerMessageSpec::ConsoleMessage& consoleMessage
                               );

};

class LiteConsoleController : public ConsoleController
{
public:
    LiteConsoleController(
            MMCSCommandProcessor* commandProcessor,     //!< [in] command processor
            BGQMachineXML* machine,                     //!< [in] machine XML description
            const bgq::utility::UserId& user            //!< [in] user
            );
    void genIOBlockController(deque<string> args, std::string& blockName, std::istream& xml, MMCSCommandReply& reply);
    void genCNBlockController(deque<string> args, std::string& blockName, std::istream& xml, MMCSCommandReply& reply);
    bool blockInitialized() { return _block_initialized; }
    void setBlockInitialized(
            bool tf
            )
    {
        _block_initialized = tf;
    }

    /*!
     * \brief Get the currently active job.
     */
    boost::weak_ptr<lite::Job> getJob() const { return _job; }

    /*!
     * \brief Set the currently active job.
     */
    void setJob(
            const boost::shared_ptr<lite::Job> job    //!< [in]
            )
    {
        _job = job;
    }

private:
    BGQMachineXML* _machine;
    bool _block_initialized;
    boost::weak_ptr<lite::Job> _job;
};

/*!
//  gen_block
//  syntax:
//     gen_block <block> <midplane> <cnodes> <nodecard> [ options ] - define a block of nodes to be controlled by mmcs
*/
class MMCSCommand_gen_block: public MMCSCommand
{
public:
    MMCSCommand_gen_block(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "gen_block <blockid> <midplane>  <cnodes> <nodecard> [ node ] [ options ]";}
    static  MMCSCommand_gen_block* build();    // factory method
    static  std::string cmdname() { return "gen_block"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};


/*!
//  gen_io_block
//  syntax:
//     gen_io_block <block> <drawer> <ionodes> <node> [ options ] - define a block of nodes to be controlled by mmcs
*/
class MMCSCommand_gen_io_block: public MMCSCommand
{
public:
    MMCSCommand_gen_io_block(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "gen_io_block <blockid> <drawer> <ionodes> <node> [ options ]";}
    static  MMCSCommand_gen_io_block* build();    // factory method
    static  std::string cmdname() { return "gen_io_block"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

/*!
//  dump_ras
//  syntax:
//     dump_ras [filename]
//
*/
class MMCSCommand_dump_ras: public MMCSCommand
{
public:
    MMCSCommand_dump_ras(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "dump_ras [filename]";}
    static  MMCSCommand_dump_ras* build();    // factory method
    static  std::string cmdname() { return "dump_ras"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};


/*!
//  show_envs
//  syntax:
//     show_envs <location>
//
*/
class MMCSCommand_show_envs: public MMCSCommand
{
public:
    MMCSCommand_show_envs(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "show_envs <type> <location>";}
    static  MMCSCommand_show_envs* build();    // factory method
    static  std::string cmdname() { return "show_envs"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};


/*!
//  bringup
//  syntax:
//     bringup [options]
//
*/
class MMCSCommand_bringup: public MMCSCommand
{
public:
    MMCSCommand_bringup(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "bringup [options]";}
    static  MMCSCommand_bringup* build();    // factory method
    static  std::string cmdname() { return "bringup"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};


/*!
//  wait_for_terminate
//  syntax:
//     wait_for_terminate [seconds]
//
*/
class MMCSCommand_wait_for_terminate: public MMCSCommand
{
public:
    MMCSCommand_wait_for_terminate(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "wait_for_terminate [seconds]";}
    static  MMCSCommand_wait_for_terminate* build();    // factory method
    static  std::string cmdname() { return "wait_for_terminate"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

//

// this is an alias for disconnect
class MMCSCommand_d: public MMCSCommand
{
public:
    MMCSCommand_d(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "d";}
    static  MMCSCommand_d* build();    // factory method
    static  std::string cmdname() { return "d"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

/*!
** delete_block
** Perform a disconnect on all the connections associated with the current block
** and delete the current block
*/
class MMCSCommand_delete_block: public MMCSCommand
{
public:
    MMCSCommand_delete_block(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "delete_block";}
    static  MMCSCommand_delete_block* build();    // factory method
    static  std::string cmdname() { return "delete_block"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget=NULL);
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

class MMCSCommand_boot_block: public MMCSCommand
{
public:
    MMCSCommand_boot_block(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "boot_block [ options ]";}
    static  MMCSCommand_boot_block* build();	// factory method
    static  std::string cmdname() { return "boot_block"; }
    void execute(std::deque<std::string> args,
		 MMCSCommandReply& reply,
		 ConsoleController* pController,
		 BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true;}
    void help(std::deque<std::string> args,
	      MMCSCommandReply& reply);
};


#endif
