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

#include "ConsoleController.h"

#include "Properties.h"

#include "../MMCSCommandProcessor.h"

#include <utility/include/Log.h>

#include <editline/readline.h>

using namespace std;

LOG_DECLARE_FILE( "mmcs.common" );

namespace mmcs {
namespace common {

ConsoleController::ConsoleController(
        MMCSCommandProcessor *mmcsCommandProcessor,
        const bgq::utility::UserId& user,
        CxxSockets::UserType utype
        ) :
    _blockController(),
    _quit( -1),
    _commandProcessor(mmcsCommandProcessor),
    _consolePort(NULL),
    _user(user),
    _utype(utype),
    _redirecting(false)
{
    // start reading commands from standard input
    _inputs.push(stdin);
}

ConsoleController::~ConsoleController()
{
    if (_consolePort) {
        delete _consolePort;
    }
}

const bgq::utility::UserId&
ConsoleController::getUser() const
{
    return _user;
}

void
ConsoleController::run()
{
    if (_commandProcessor) {
        while ( this->quit() == -1) {
            serviceCommands();
        }
    } else {
        LOG_FATAL_MSG(__FUNCTION__ << " Empty command list");
    }
}

bool
ConsoleController::pushInput(FILE* f)
{
    if (_inputs.size() == 10) {
        return false;
    }
    _inputs.push(f);
    return true;
}

void
ConsoleController::serviceCommands()
{
    try {
        bool eof = false;
        char buf[4096];
        if (_inputs.size() == 0) { // no input streams
            if (!freopen("/dev/tty", "a+", stdin)) {
                quit(EXIT_SUCCESS);
                return;
            }
            _inputs.push(stdin);
        }

        const bool input_is_tty = isatty(fileno(_inputs.top()));
        if (input_is_tty) {
            // keyboard input stream - use "readline" library, to allow editing
            char *line = readline("mmcs$ ");
            if (line) {
                add_history(line);
                this->writeHistoryFile();
                strncpy(buf, line, sizeof(buf));
                buf[sizeof(buf) - 1] = 0;
                free(line);
            } else  {
                eof = true;
            }
        } else { // file input stream - no editing
            if (fgets(buf, sizeof(buf), _inputs.top())) {
                buf[strlen(buf) - 1] = 0; // trim off trailing "\n"
            } else {
                eof = true;
                if (Properties::getProperty(FILE_EXIT) == "true") {
                    quit(EXIT_SUCCESS);
                }
            }
        }

        if (eof) {
            fclose(_inputs.top());
            _inputs.pop();
            return;
        }

        // parse the command
        mmcs_client::CommandReply reply(1, 1, true);    // send reply to stdout

        deque<string> cmdStr = MMCSCommandProcessor::parseCommand(buf);
        if (cmdStr.size() == 0) {
            return;
        }

        // if the input device is not a tty, log input and failures
        if (input_is_tty) {
            _commandProcessor->logFailures(false);
        } else {
            _commandProcessor->logFailures(true);
            MMCSCommandProcessor::logCommand(cmdStr);
        }

        // Execute command
        const procstat cmdExecStatus = _commandProcessor->execute(cmdStr, reply, this);

        // Write the output to the console
        reply.sync();  // send the reply to the client

        // if the cmd failed, check the "exit on failure" option
        if (
                (cmdExecStatus == CMD_NOT_FOUND || cmdExecStatus == CMD_INVALID || reply.getStatus() != mmcs_client::CommandReply::STATUS_OK) &&
                (!Properties::getProperty("cmd_failure_exit").empty()) &&
                (Properties::getProperty("cmd_failure_exit") != "false")
           )
        {
            quit(EXIT_FAILURE);
        }

        // for input coming from a non-tty we don't need to retain history
        // since the file or pipe already has the command history!
        if ( input_is_tty ) {
            // Do nothing
        }
    } catch (const exception& e) {
        LOG_ERROR_MSG("ConsoleController(): " << e.what());
        quit(EXIT_SUCCESS);
    }
    return;
}

std::string
ConsoleController::getHistoryFile() const
{
    // use process name for unique history file
    std::string result = "." + Properties::getProperty(MMCS_PROCESS) + "_history";
    if ( getenv("HOME") ) {
        std::string home( getenv("HOME") );
        home.append( "/" );
        home.append( result );
        result = home;
    }

    return result;
}

void
ConsoleController::writeHistoryFile() const
{
    const std::string file( this->getHistoryFile() );
    if ( write_history( file.c_str() ) != 0 ) {
        LOG_WARN_MSG( "Could not write history file '" << file << "' " << strerror(errno) );
    }
}

} } // namespace mmcs::common
