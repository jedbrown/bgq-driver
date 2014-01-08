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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

#include "DumpMachine.h"

#include "common/ConsoleController.h"

#include "../../MMCSCommandProcessor.h"

#include <utility/include/ScopeGuard.h>

#include <boost/bind.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

using namespace std;

namespace mmcs {
namespace console {
namespace command {

void
DumpMachine::sendCommand(
        const std::deque<std::string>& command,
        const std::deque<std::string>& args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController
        )
{
    // We want to buffer output and eventually put it in the output file rather than stdout
    mmcs_client::CommandReply freply(
            STDOUT_FILENO,
            1,      // reply format
            false  // writing disabled
            );

    // Send the dump_machine command
    MMCSCommandProcessor mmcsCommandProcessor(pController->getCommandProcessor()->getCommandMap());
    mmcsCommandProcessor.logFailures(false); // don't clutter the console with log messages
    mmcsCommandProcessor.execute(command, freply, pController);
    if (freply.getStatus() != mmcs_client::CommandReply::STATUS_OK) {
        reply << mmcs_client::FAIL << freply.str() << mmcs_client::DONE;
        return;
    }

    // use O_EXCL flag to ensure file will be created
    const int openfd = open(args[0].c_str(),O_RDWR|O_CREAT|O_EXCL,S_IRUSR|S_IWUSR);
    if (openfd == -1) {
        reply << mmcs_client::FAIL << "Could not create file " << args[0].c_str() << " " << strerror(errno) << mmcs_client::DONE;
        return;
    }
    bgq::utility::ScopeGuard closeGuard(
            boost::bind( &::close, openfd )
            );

    const std::string results( freply.str() );

    const size_t rc = write( openfd, results.c_str(), results.size() );
    if ( rc != results.size() ) {
        reply << mmcs_client::FAIL << "Wrote " << rc << " bytes to file " << args[0] << " instead of expected " << results.size() << mmcs_client::DONE;
        return;
    }

    reply << mmcs_client::OK << mmcs_client::DONE;
}

DumpMachine*
DumpMachine::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.bgConsoleCommand(true);
    commandAttributes.internalAuth(true);              // Anybody can run the console version because auth is on the server
    commandAttributes.helpCategory(common::ADMIN);

    return new DumpMachine("dump_machine", "dump_machine <file.xml>", commandAttributes);
}

void
DumpMachine::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        server::BlockControllerTarget* /*pTarget*/
        )
{
    BOOST_ASSERT( args.size() == 1 );
    if ( boost::filesystem::exists( args[0]) ) {
        reply << mmcs_client::FAIL << args[0] << " already exists" << mmcs_client::DONE;
        return;
    }

    const deque<string> mmcs_dump_machine = MMCSCommandProcessor::parseCommand("mmcs_server_cmd dump_machine_server");
    sendCommand(mmcs_dump_machine, args, reply, pController);
}

void
DumpMachine::help(
        deque<string> ,//args,
        mmcs_client::CommandReply& reply
        )
{
    reply << mmcs_client::OK << description()
      << ";Export the machine from the database into <file.xml>"
      << ";The command will fail if file.xml already exists."
      << mmcs_client::DONE;
}

} } } // namespace mmcs::console::command
