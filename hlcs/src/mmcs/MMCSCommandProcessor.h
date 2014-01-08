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

#ifndef MMCS_MMCS_COMMAND_PROCESSOR_H
#define MMCS_MMCS_COMMAND_PROCESSOR_H

#include "common/AbstractCommand.h"
#include "common/fwd.h"
#include "common/Properties.h"

#include "libmmcs_client/CommandReply.h"

#include "security/Enforcer.h"

#include <boost/shared_ptr.hpp>

#include <deque>
#include <map>
#include <string>

namespace mmcs {

// NOTE:  We will probably need to create a locking interface around these so we can update them on
// a refresh_config, especially for new security authorizations.
// We'll only want a write lock allowing all readers.
typedef std::map<std::string, common::AbstractCommand*> MMCSCommandMap;
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
    ** /brief Should FAIL replies from commands be logged to the MMCS log? (default is true)
    */
    void logFailures(bool tf) { _logFailures = tf; }
    bool logFailures() { return _logFailures; }

    /*!
    ** /brief Log the command and parameters.
    */
    static void  logCommand(const std::string& cmd, const std::deque<std::string>& args);

    /*!
    ** /brief Log the command and parameters.
    */
    static void logCommand(const std::deque<std::string>& cmd_and_args);

    /*!
    ** /brief Break up command line into words.
    ** /param rStr	      null terminated command line
    ** /returns std::deque<std::string> sequence of words
    */
    static std::deque<std::string> parseCommand(const std::string &rStr);

    /*! /brief Combine words into a command line
    **  /param std::deque<std::string> sequence of words
    **  /returns string      null terminated command line
    */
    static std::string unparseCommand(const std::deque<std::string>& cmdStr);

    /*!
    ** /brief Perform specific MMCS command
    ** @param cmdStr      the command string as parsed by parse()
    ** @param reply       the command output stream. Refer to class MMCSCommandProcessorReply
    ** @param pController the common::ConsoleController object that the command is to work on
    ** @returns           execution status, refer the MMCSCommandProcessor::status
    */
    virtual
    procstat execute(std::deque<std::string> cmdStr,
				       mmcs_client::CommandReply& reply,
				       common::ConsoleController* pController);

    /*!
    ** /brief Perform specific MMCS command
    ** @param cmdName     the command name
    ** @param cmdArgs     the command arguments
    ** @param reply       the command output stream. Refer to class MMCSCommandProcessorReply
    ** @param pController the common::ConsoleController object that the command is to work on
    ** @returns           execution status, refer the MMCSCommandProcessor::status
    */
    virtual
    procstat execute(std::string cmdName,
				       std::deque<std::string> cmdArgs,
				       mmcs_client::CommandReply& reply,
				       common::ConsoleController* pController);


    /*!
    ** /brief Return a pointer to the command map
    */
    MMCSCommandMap* getCommandMap() { return _mmcsCommands; }

    /*!
    ** /brief Create a command map from specified attributes
    ** /parm attr common::AbstractCommand::Attributes matching desired commands
    ** /parm mask common::AbstractCommand::Attributes mask indicating which attributes
    **            to check
    */
    static
    MMCSCommandMap* createCommandMap(common::AbstractCommand::Attributes& attr,
                                     common::AbstractCommand::Attributes& mask,
                                     common::Properties::Map* externalCommands = 0);

    static EnforcerPtr     _command_enforcer;

protected:
    virtual procstat validate(std::deque<std::string>& cmdStr,
					mmcs_client::CommandReply& reply,
					common::ConsoleController* pController,
					server::BlockControllerTarget** pTarget,
					common::AbstractCommand** pCmd,
                                        std::vector<std::string>* validnames);
    procstat validate_security(std::deque<std::string>& cmdStr,
                                                 mmcs_client::CommandReply& reply,
                                                 common::ConsoleController* pController,
                                                 common::AbstractCommand** pCmd,
                                                 std::string& cmdName,
                                                 std::vector<std::string>* validnames);

    /*!
    ** /brief Invoke the 'execute' method of an MMCSCommand
    ** @param cmdStr      the command string as parsed by parse()
    ** @param reply       the command output stream. Refer to class MMCSCommandProcessorReply
    ** @param pController the common::ConsoleController object that the command is to work on
    ** @param pTarget     optional target list
    ** @param pCmd        the MMCSCommand object
    ** @param             validate status
    ** @returns           execution status, refer the MMCSCommandProcessor::status
    */
    virtual
    procstat invokeCommand(std::deque<std::string> cmdStr,
					     mmcs_client::CommandReply& reply,
					     common::ConsoleController* pController,
					     server::BlockControllerTarget* pTarget,
					     common::AbstractCommand* pCmd,
					     procstat status,
                                             std::vector<std::string>& validnames);
    MMCSCommandMap* _mmcsCommands;
    bool            _logFailures;
    bool            _bg_console;
};

} // namespace mmcs

#endif
