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

#include <sstream>
#include "MMCSCommandReply.h"
#include "MMCSCommandProcessor.h"
#include "ConsoleController.h"
#include "MMCSConsolePort.h"
#include "MMCSProperties.h"
#include <Log.h>

#include <editline/readline.h>

using namespace std;

extern int signal_number;

LOG_DECLARE_FILE( "mmcs" );
#define LOGGING_DECLARE_USER_ID_MDC \
    log4cxx::MDC _userid_mdc_( "user", std::string("[") + _user.getUser() + "] " );

volatile int ConsoleController::signal_number = 0;

ConsoleController::ConsoleController(
        MMCSCommandProcessor *mmcsCommandProcessor,
        const bgq::utility::UserId& user,
        CxxSockets::UserType utype,
        BlockHelperPtr blockController
        ) :
    _blockController(blockController),
    _quit( -1),
    _commandProcessor(mmcsCommandProcessor),
    _consolePort(NULL),
    _user(user),
    _utype(utype),
    _redirecting(false),
    _fd(STDOUT_FILENO),
    _replyFormat(0),
    _midplaneControllerMutex(PTHREAD_MUTEX_ERRORCHECK_NP)
{
    // start reading commands from standard input
    _inputs.push(stdin);
}

ConsoleController::~ConsoleController()
{
    if (_consolePort)
    {
      //shutdown(_consolePort->getPortDescriptor(), SHUT_RDWR);
        delete _consolePort;
    }

    if (_fd != STDOUT_FILENO)
    {
        close(_fd);
    }
}

const bgq::utility::UserId&
ConsoleController::getUser() const
{
    PthreadMutexHolder mutex;
    mutex.Lock(&_midplaneControllerMutex); // unlocked via destructor
    return _user;
}

void
ConsoleController::run()
{
    LOGGING_DECLARE_USER_ID_MDC;
    if (_commandProcessor)
    {
        while ( this->quit() == -1 && signal_number == 0)
        {
            serviceCommands();
        }
    }
    else
    {
        LOG_FATAL_MSG("ConsoleController::run() internal error: empty command list");
    }
}

bool
ConsoleController::pushInput(FILE* f)
{
    LOGGING_DECLARE_USER_ID_MDC;
    if (_inputs.size() == 10)
    return false;
    _inputs.push(f);
    return true;
}

void
ConsoleController::serviceCommands()
{
    LOGGING_DECLARE_USER_ID_MDC;
    bool eof = false;
    char buf[4096];
    MMCSCommandProcessorStatus::procstat cmdExecStatus = MMCSCommandProcessorStatus::CMD_NOT_FOUND;
    try
    {
        if (_inputs.size() == 0)
        { // no input streams
            if (!freopen("/dev/tty", "a+", stdin))
            {
                quit(EXIT_SUCCESS);
                return;
            }
            _inputs.push(stdin);
        }

        bool input_is_tty = isatty(fileno(_inputs.top()));
        if (input_is_tty)
        {
            // keyboard input stream - use "readline" library, to allow editing
            char *line = readline("mmcs$ ");
            if (line)
            {
                add_history(line);
                this->writeHistoryFile();
                strncpy(buf, line, sizeof(buf));
                buf[sizeof(buf) - 1] = 0;
                free(line);
            }
            else
            {
                eof = true;
            }
        }
        else
        { // file input stream - no editing
            if (fgets(buf, sizeof(buf), _inputs.top()))
            {
                buf[strlen(buf) - 1] = 0; // trim off trailing "\n"
            }
            else
            {
                eof = true;
                if (MMCSProperties::getProperty(FILE_EXIT) == "true")
                {
                    quit(EXIT_SUCCESS);
                }
            }
        }

        if (eof)
        {
            fclose(_inputs.top());
            _inputs.pop();
            return;
        }

        // parse the command
        MMCSCommandReply reply(1, 1, true);    // send reply to stdout

        reply.setFd(getFd());

        deque<string> cmdStr = MMCSCommandProcessor::parseCommand(buf);
        if (cmdStr.size() == 0)
        {
            return;
        }

        // if the input device is not a tty, log input and failures
        if (input_is_tty)
        {
            _commandProcessor->logFailures(false);
        }
        else
        {
            _commandProcessor->logFailures(true);
            MMCSCommandProcessor::logCommand(cmdStr);
        }

        //
        // Execute command
        //
        cmdExecStatus = _commandProcessor->execute(cmdStr, reply, this);

        //
        // Write the output to the console
        //
        reply.sync();  // send the reply to the client

        // if the cmd failed, check the "exit on failure" option
        if (
                (cmdExecStatus == MMCSCommandProcessorStatus::CMD_NOT_FOUND || cmdExecStatus == MMCSCommandProcessorStatus::CMD_INVALID || reply.getStatus() != MMCSCommandReply::STATUS_OK) &&
                (!MMCSProperties::getProperty("cmd_failure_exit").empty()) &&
                (MMCSProperties::getProperty("cmd_failure_exit") != "false")
                )
        {
            quit(EXIT_FAILURE);
        }

        // for input coming from a non-tty we don't need to retain history
        // since the file or pipe already has the command history!
        if ( input_is_tty ) {
        }
    }
    catch (exception &e)
    {
        LOG_ERROR_MSG("ConsoleController(): " << e.what());
        quit(EXIT_SUCCESS);
    }
    return;
}

std::string
ConsoleController::getHistoryFile() const
{
    // use process name for unique history file
    std::string result = "." + MMCSProperties::getProperty(MMCS_PROCESS) + "_history";
    if ( getenv("HOME") ) {
        std::string home( getenv("HOME") );
        home.append( "/" );
        home.append( result );
        result = home;
    }

    LOG_TRACE_MSG( "using '" << result << "' for editline history" );
    return result;
}


void
ConsoleController::writeHistoryFile() const
{
    const std::string file( this->getHistoryFile() );
    if ( write_history( file.c_str() ) != 0 ) {
        LOG_WARN_MSG( "could not write history file '" << file << "' " << strerror(errno) );
    }
}
