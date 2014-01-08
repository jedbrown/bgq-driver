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

#include <fstream>

#include <fcntl.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>


using namespace std;


namespace mmcs {
namespace console {
namespace command {


void DumpMachine::sendCommand(
            std::deque<std::string>& command,
            std::deque<std::string>& args,
            mmcs_client::CommandReply& reply,
            common::ConsoleController* pController
        )
{
    struct stat filestat;

    // Stat the file to see if it exists (and who owns it)
    if(stat(args[0].c_str(), &filestat) == 0) {
        // Then, we check to see if we own it
        if(filestat.st_uid != getuid()) {
            reply << mmcs_client::FAIL << "generation of " << args[0] << " failed.  User does not own file. " << mmcs_client::DONE;
            return;
        }
    }
    else { // stat fails
        if(errno != ENOENT) {  // if something other than nonexisting file...
            reply << mmcs_client::FAIL << "generation of " << args[0] << " failed.  File error: " << strerror(errno) << mmcs_client::DONE;
            return;
        }
    }

    int openfd = open(args[0].c_str(),O_RDWR|O_CREAT,S_IRUSR|S_IWUSR);
    if(openfd == -1) {
        reply << mmcs_client::FAIL << "Could not create file " << args[0].c_str() << " " << strerror(errno) << mmcs_client::DONE;
        return;
    }

    mmcs_client::CommandReply freply(openfd, pController->getReplyFormat(), true);

    // send the dump_machine command
    MMCSCommandProcessor mmcsCommandProcessor(pController->getCommandProcessor()->getCommandMap()); // MMCS command processor
    mmcsCommandProcessor.logFailures(false); // don't clutter the console with log messages
    mmcsCommandProcessor.execute(command, freply, pController);
    if (freply.getStatus() != mmcs_client::CommandReply::STATUS_OK) {
        std::string freply_str = freply.str();
        reply << mmcs_client::FAIL << freply_str << mmcs_client::DONE;
        return;
    }

    freply.sync();

    close(openfd);

    // We don't get to manipulate the data
    // until it has actually been written to file
    // because of our basic command reply design.
    // So, we've got to re-read the file, delete
    // the OK and then re-write it.
    ifstream ifile;
    ifile.open(args[0].c_str());
    char* temp_buff;
    ifile.seekg (0, ios::end);
    int length = ifile.tellg();
    ifile.seekg (0, ios::beg);

    temp_buff = new char [length];
    ifile.read(temp_buff, length);
    ifile.close();

    std::string outstring(temp_buff, length);
    outstring.erase(0,3); // actually cuts the 'ok' and the newline
    ofstream ofile(args[0].c_str());
    ofile << outstring;
    ofile.flush();
    ofile.close();
    delete [] temp_buff;
    reply << mmcs_client::OK << mmcs_client::DONE;
}

/*
** dump_machine <file.xml>
** Export a machine from the database into <file.xml>
*/
DumpMachine*
DumpMachine::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.mmcsConsoleCommand(true);
    commandAttributes.internalAuth(true);              // Anybody can run the console version because auth is on the server
    commandAttributes.helpCategory(common::ADMIN);

    return new DumpMachine("dump_machine", "dump_machine <file.xml>", commandAttributes);
}

void
DumpMachine::execute(deque<string> args,
                    mmcs_client::CommandReply& reply,
                    common::ConsoleController* pController,
                    server::BlockControllerTarget* /*pTarget*/)
{
    deque<string> mmcs_dump_machine = MMCSCommandProcessor::parseCommand("mmcs_server_cmd dump_machine_server");
    sendCommand(mmcs_dump_machine, args, reply, pController);
}

void
DumpMachine::help(deque<string> ,//args,
                 mmcs_client::CommandReply& reply)
{
    reply << mmcs_client::OK << description()
      << ";Export a machine from the database into <file.xml>"
      << mmcs_client::DONE;
}

} } } // namespace mmcs::console::command
