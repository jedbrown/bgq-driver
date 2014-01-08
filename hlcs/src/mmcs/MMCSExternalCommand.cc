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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

#include <sys/types.h>
#include <sys/wait.h>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include <utility/include/Exec.h>
#include <utility/include/ScopeGuard.h>
#include "MMCSExternalCommand.h"
#include "MMCSProperties.h"
#include "ConsoleOptions.h"

LOG_DECLARE_FILE("mmcs");

std::string MMCSExternalCommand::_propfile = "";

MMCSExternalCommand* MMCSExternalCommand::build(std::string& name, std::string& description)
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsConsoleCommand(true);
    commandAttributes.externalCommand(true);
    commandAttributes.helpCategory(DEFAULT);
    return new MMCSExternalCommand(name.c_str(), description.c_str(), commandAttributes);
}

void MMCSExternalCommand::runcmd(MMCSCommandReply& reply, const std::ostringstream& cmd_and_args, const std::string& executable) {
    LOG_DEBUG_MSG("Running command " << executable);

    std::ostringstream command;

    // Get the paths to search for the executable
    std::string pathstring = MMCSProperties::getProperty("command_path");
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

    // Loop through all paths supplied and
    // see if our executable exists in any of them.
    boost::char_separator<char> sep(":");
    tokenizer pathtok(pathstring, sep);
    bool found = false;
    for(tokenizer::iterator dir = pathtok.begin(); dir != pathtok.end(); ++dir) {
        std::string filestring = *dir + "/" + executable;
        if(boost::filesystem::exists(filestring)) {
            found = true;
            command << *dir << "/";
            break;
        }
    }

    if(!found) {
        reply << ABORT << "Command " << executable << " not found in command path." << DONE;
        return;
    }

    command << cmd_and_args.str();

    int pipefd;
    const bgq::utility::ScopeGuard logGuard(boost::bind(&::close, boost::ref(pipefd)));
    std::string errorstring = "";
    const pid_t child_pid = Exec::fexec(pipefd, command.str(), errorstring, true, "", MMCSExternalCommand::_propfile);

    std::ostringstream monitorstream;

    typedef boost::shared_ptr<Exec::ChildMonitor> ChildMonitorPtr;
    const ChildMonitorPtr cm(new Exec::ChildMonitor);

    boost::thread childthread = boost::thread(&Exec::ChildMonitor::monitor_child, cm, pipefd, &monitorstream);

    int exit_status = 0;
    if(child_pid != 0)
        waitpid(child_pid, &exit_status, 0);

    bool failed = false;
    std::ostringstream errormsg;
    if(WIFEXITED(exit_status) == false) {
        failed = true;
        int signo = 0;
        if(WIFSIGNALED(exit_status)) {
            signo = WTERMSIG(exit_status);
            errormsg << "  Exit signal " << signo << " received.";
            if(WCOREDUMP(exit_status)) {
                errormsg << "  Core dumped.";
            }
        }
    } else {
        int statnum = WEXITSTATUS(exit_status);
        if(statnum) {
            failed = true;
            errormsg << "Command failed with status " << statnum << ".";
        }
    }

    cm->end();
    childthread.join();
    if(failed)
        reply << FAIL << monitorstream.str() << "\n" << errormsg.str() << DONE;
    else
        reply << OK << monitorstream.str() << DONE;
}

void MMCSExternalCommand::execute(deque<string> args,
			      MMCSCommandReply& reply,
			      ConsoleController* pController,
			      BlockControllerTarget* pTarget)
{
    std::ostringstream command;

    LOG_DEBUG_MSG("Executing external command " << args[0]);

    for(std::deque<std::string>::const_iterator it = args.begin();
        it != args.end(); ++it) {
        command << *it << " ";
    }

    LOG_DEBUG_MSG("command line: " << command.str());

    runcmd(reply, command, args[0]);
}

void MMCSExternalCommand::help(deque<string> args,
			   MMCSCommandReply& reply)
{
    std::ostringstream runstr;
    runstr << args[0] << " --help";
    runcmd(reply, runstr, args[0]);
}
