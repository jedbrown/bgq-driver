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

#include "ExternalCommand.h"

#include "common/Properties.h"

#include <utility/include/Exec.h>
#include <utility/include/ExitStatus.h>
#include <utility/include/Log.h>
#include <utility/include/ScopeGuard.h>

#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>

#include <sys/types.h>
#include <sys/wait.h>

LOG_DECLARE_FILE("mmcs.console");

using namespace std;

using mmcs::common::Properties;

namespace mmcs {
namespace console {

ExternalCommand*
ExternalCommand::build(
        const std::string& name, 
        const std::string& description
        )
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.bgConsoleCommand(true);
    commandAttributes.externalCommand(true);
    commandAttributes.helpCategory(common::DEFAULT);
    return new ExternalCommand(name.c_str(), description.c_str(), commandAttributes);
}

void
ExternalCommand::runcmd(
        mmcs_client::CommandReply& reply,
        const std::string& cmd_and_args,
        const std::string& executable
        )
{
    LOG_DEBUG_MSG("Running command " << executable);

    std::ostringstream command;

    // Get the paths to search for the executable
    const std::string pathstring = Properties::getProperty("command_path");
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

    // Loop through all paths supplied and
    // see if our executable exists in any of them.
    tokenizer pathtok(pathstring, boost::char_separator<char>(":"));
    bool found = false;
    for (tokenizer::const_iterator dir = pathtok.begin(); dir != pathtok.end(); ++dir) {
        const std::string filestring = *dir + "/" + executable;
        if (boost::filesystem::exists(filestring)) {
            found = true;
            command << *dir << "/";
            break;
        }
    }

    if (!found) {
        reply << mmcs_client::ABORT << "Command " << executable << " not found in command path." << mmcs_client::DONE;
        return;
    }

    command << cmd_and_args;

    int pipefd;
    const bgq::utility::ScopeGuard logGuard(boost::bind(&::close, boost::ref(pipefd)));
    std::string errorstring;
    LOG_DEBUG_MSG( "Using properties file " << Properties::getProperties()->getFilename() );
    const pid_t child_pid = Exec::fexec(
            pipefd,
            command.str(),
            errorstring,
            true,
            "",
            Properties::getProperties()->getFilename()
            );
    if ( child_pid == -1 ) {
        reply << mmcs_client::FAIL << "Internal error from fexec: " << errorstring << mmcs_client::DONE;
        return;
    }

    while ( 1 ) {
        char buf[1024];
        const int rc = read( pipefd, buf, sizeof(buf) - 1 );
        if ( rc == -1 ) {
            if ( errno == EAGAIN || errno == EINTR ) {
                continue;
            } else {
                reply << mmcs_client::FAIL << "Internal error reading from child process: " << strerror(errno) << mmcs_client::DONE;
                break;
            }
        } else if ( !rc ) {
            break;
        } else {
            buf[rc] = '\0';
            std::cout << buf;
            std::cout.flush();
        }
    }

    int status = 0;
    waitpid(child_pid, &status, 0);
    const bgq::utility::ExitStatus exit_status( status );

    if ( exit_status.signaled() ) {
        reply << mmcs_client::FAIL << "Command failed due to " << exit_status << mmcs_client::DONE;
    } else if ( exit_status.exited() && exit_status.getExitStatus() ) {
        reply << mmcs_client::FAIL << "Command failed with " << exit_status << mmcs_client::DONE;
    } else {
        reply << mmcs_client::OK << mmcs_client::DONE;
    }
}

void
ExternalCommand::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* ,//pController,
        server::BlockControllerTarget* //pTarget
        )
{
    std::ostringstream command;

    LOG_DEBUG_MSG("Executing external command " << args[0]);

    for (std::deque<std::string>::const_iterator it = args.begin(); it != args.end(); ++it) {
        command << *it << " ";
    }

    LOG_DEBUG_MSG("Command line: " << command.str());

    runcmd(reply, command.str(), args[0]);
}

void
ExternalCommand::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
        )
{
    std::ostringstream runstr;
    runstr << args[0] << " --help";
    runcmd(reply, runstr.str(), args[0]);
}

} } // namespace mmcs::console
