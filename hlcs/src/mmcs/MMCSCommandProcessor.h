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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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
 * \file MMCSCommandProcessor.h
 */

#ifndef MMCSCOMMAND_PROCESSOR_H
#define MMCSCOMMAND_PROCESSOR_H

#include <string>
#include <deque>
#include <map>
#include <boost/shared_ptr.hpp>
#include "MMCSCommand.h"
#include "MMCSCommandReply.h"
#include "MMCSProperties.h"
#include "MMCSCommandProcessorStatus.h"

class ConsoleController;
class BlockController;
class BlockControllerTarget;

namespace hlcs {
namespace security {

class Enforcer;

} // security
} // hlcs

// NOTE:  We will probably need to create a locking
// interface around these so we can update them on
// a refresh_config, especially for new security authorizations
// We'll only want a write lock allowing all readers.
typedef std::map<std::string, MMCSCommand*> MMCSCommandMap;
typedef std::map<std::string, std::string> ExternalCommandMap;
typedef boost::shared_ptr<hlcs::security::Enforcer> EnforcerPtr;

/*!
 * \brief Executes MMCSCommand objects.
 */
class MMCSCommandProcessor
{
public:
    /*!
    ** /brief MMCSCommandProcessor constructors
    ** @param mmcsCommands a map of command names and MMCSCommand objects
    */
    MMCSCommandProcessor(MMCSCommandMap* mmcsCommands) : _mmcsCommands(mmcsCommands), _logFailures(true), _bg_console(false) {}
    MMCSCommandProcessor(const MMCSCommandProcessor& c) : _mmcsCommands(c._mmcsCommands), _logFailures(c._logFailures), _bg_console(c._bg_console) {}

    /*!
    ** MMCSCommandProcessor destructor
    */
    virtual ~MMCSCommandProcessor() {}

    /*!
    ** /brief should FAIL replies from commands be logged to the MMCS log? (default is true)
    */
    void logFailures(bool tf) { _logFailures = tf; }
    bool logFailures() { return _logFailures; }

    /*!
    ** /brief log the command and parameters.
    */
    static void  logCommand(std::string cmd, std::deque<std::string> args);

    /*!
    ** /brief log the command and parameters.
    */
    static void logCommand(std::deque<std::string> cmd_and_args);

    /*!
    ** /brief break up command line into words.
    ** /param rStr	      null terminated command line
    ** /returns std::deque<std::string> sequence of words
    */
    static std::deque<std::string> parseCommand(const std::string &rStr);

    /*! /brief combine words into a command line
    **  /param std::deque<std::string> sequence of words
    **  /returns string      null terminated command line
    */
    static std::string unparseCommand(const std::deque<std::string>& cmdStr);

    /*!
    ** /brief Perform specific MMCS command
    ** @param cmdStr      the command string as parsed by parse()
    ** @param reply       the command output stream. Refer to class MMCSCommandProcessorReply
    ** @param pController the ConsoleController object that the command is to work on
    ** @returns           execution status, refer the MMCSCommandProcessor::status
    */
    virtual
    MMCSCommandProcessorStatus::procstat execute(std::deque<std::string> cmdStr,
				       MMCSCommandReply& reply,
				       ConsoleController* pController);

    /*!
    ** /brief Perform specific MMCS command
    ** @param cmdName     the command name
    ** @param cmdArgs     the command arguments
    ** @param reply       the command output stream. Refer to class MMCSCommandProcessorReply
    ** @param pController the ConsoleController object that the command is to work on
    ** @returns           execution status, refer the MMCSCommandProcessor::status
    */
    virtual
    MMCSCommandProcessorStatus::procstat execute(std::string cmdName,
				       std::deque<std::string> cmdArgs,
				       MMCSCommandReply& reply,
				       ConsoleController* pController);


    /*!
    ** /brief return a pointer to the command map
    */
    MMCSCommandMap* getCommandMap() { return _mmcsCommands; }

    /*!
    ** /brief create a command map from specified attributes
    ** /parm attr MMCSCommandAttributes matching desired commands
    ** /parm mask MMCSCommandAttributes mask indicating which attributes
    **            to check
    */
    static
    MMCSCommandMap* createCommandMap(MMCSCommandAttributes& attr,
                                     MMCSCommandAttributes& mask,
                                     MMCSProperties::Map* externalCommands = 0);

    static EnforcerPtr     _command_enforcer;

protected:
    virtual MMCSCommandProcessorStatus::procstat validate(std::deque<std::string>& cmdStr,
					MMCSCommandReply& reply,
					ConsoleController* pController,
					BlockControllerTarget** pTarget,
                                        MMCSCommand** pCmd,
                                        std::vector<std::string>* validnames);
    MMCSCommandProcessorStatus::procstat validate_security(deque<string>& cmdStr,
                                                 MMCSCommandReply& reply,
                                                 ConsoleController* pController,
                                                 MMCSCommand** pCmd,
                                                 std::string& cmdName,
                                                 std::vector<std::string>* validnames);

    /*!
    ** /brief invoke the 'execute' method of an MMCSCommand
    ** @param cmdStr      the command string as parsed by parse()
    ** @param reply       the command output stream. Refer to class MMCSCommandProcessorReply
    ** @param pController the ConsoleController object that the command is to work on
    ** @param pTarget     optional target list
    ** @param pCmd        the MMCSCommand object
    ** @param             validate status
    ** @returns           execution status, refer the MMCSCommandProcessor::status
    */
    virtual
    MMCSCommandProcessorStatus::procstat invokeCommand(std::deque<std::string> cmdStr,
					     MMCSCommandReply& reply,
					     ConsoleController* pController,
					     BlockControllerTarget* pTarget,
					     MMCSCommand* pCmd,
					     MMCSCommandProcessorStatus::procstat status,
                                             std::vector<std::string>& validnames);
    MMCSCommandMap* _mmcsCommands;
    bool            _logFailures;
    bool            _bg_console;
};


#ifdef WITH_DB


/*!
 * \brief Executes MMCS commands from mmcs_console.
 */
class MMCSConsoleCommandProcessor: public MMCSCommandProcessor
{
public:
    MMCSConsoleCommandProcessor(MMCSCommandMap* mmcsCommands)
	: MMCSCommandProcessor(mmcsCommands) { logFailures(false); _bg_console = true; }

protected:
    MMCSCommandProcessorStatus::procstat invokeCommand(std::deque<std::string> cmdStr,
					     MMCSCommandReply& reply,
					     ConsoleController* pController,
					     BlockControllerTarget* pTarget,
					     MMCSCommand* pCmd,
					     MMCSCommandProcessorStatus::procstat status,
                                             std::vector<std::string>& validnames);
    //! \brief Execute an external command
    void executeExternal(deque<string>& cmdStr,
                         MMCSCommandReply& reply,
                         ConsoleController* pController,
                         BlockControllerTarget* pTarget);
};

/*!
 * \brief Executes MMCS commands from an MMCS server.
 */
class MMCSServerCommandProcessor: public MMCSCommandProcessor
{
public:
    MMCSServerCommandProcessor(MMCSCommandMap* mmcsCommands);

    /*!
     * \brief
     */
    MMCSCommandProcessorStatus::procstat validate(
            std::deque<std::string>& cmdStr,
            MMCSCommandReply& reply,
            ConsoleController* pController,
            BlockControllerTarget** pTarget,
            MMCSCommand** pCmd,
            std::vector<std::string>* validnames);

protected:
    /*!
     * \breif Call the command processor for the command.
     * @param cmdStr      the command string as parsed by parse()
     * @param reply       the command output stream. Refer to class MMCSCommandProcessorReply
     * @param pController the ConsoleController object that the command is to work on
     * @param pTarget     the BlockControllerTarget list that the command is to work on
     * @param pCmd        the MMCSCommand object to be executed
     * @param status      the validation status, refer to MMCSCommandProcessor::status
     * @returns           execution status, refer to MMCSCommandProcessor::status
     */
    MMCSCommandProcessorStatus::procstat invokeCommand(std::deque<std::string> cmdStr,
					     MMCSCommandReply& reply,
					     ConsoleController* pController,
					     BlockControllerTarget* pTarget,
					     MMCSCommand* pCmd,
					     MMCSCommandProcessorStatus::procstat status,
                                             std::vector<std::string>& validnames);
};
#endif

#endif
