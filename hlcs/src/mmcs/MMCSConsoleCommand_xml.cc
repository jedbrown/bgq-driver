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
 * \file MMCSConsoleCommand_xml.cc
 */

#include "ConsoleController.h"
#include "MMCSConsoleCommand_xml.h"
#include "MMCSCommandReply.h"
#include "MMCSCommand_mmcs_server_connect.h"
#include "MMCSCommandProcessor.h"

#include <db/include/api/BGQDBlib.h>

#include <fstream>

#include <sys/stat.h>
#include <sys/types.h>

#include <pwd.h>

using namespace std;

void sendCommand(deque<std::string>& command, std::deque<std::string>& args, MMCSCommandReply& reply, ConsoleController* pController) {

    struct stat filestat;

    // Stat the file to see if it exists (and who owns it)
    if(stat(args[0].c_str(), &filestat) == 0) {
        // Then, we check to see if we own it
        if(filestat.st_uid != getuid()) {
            reply << FAIL << "generation of " << args[0] << " failed.  User does not own file. " << DONE;
            return;
        }
    }
    else { // stat fails
        if(errno != ENOENT) {  // if something other than nonexisting file...
            reply << FAIL << "generation of " << args[0] << " failed.  File error: " << strerror(errno) << DONE;
            return;
        }
    }

    int openfd = open(args[0].c_str(),O_RDWR|O_CREAT,S_IRUSR|S_IWUSR);
    if(openfd == -1) {
        reply << FAIL << "Could not create file " << args[0].c_str() << " " << strerror(errno) << DONE;
        return;
    }

    MMCSCommandReply freply(openfd, pController->getReplyFormat(), true);

    // send the dump_machine command
    MMCSCommandProcessor mmcsCommandProcessor(pController->getCommandProcessor()->getCommandMap()); // MMCS command processor
    mmcsCommandProcessor.logFailures(false); // don't clutter the console with log messages
    mmcsCommandProcessor.execute(command, freply, pController);
    if (freply.getStatus() != MMCSCommandReply::STATUS_OK) {
        std::string freply_str = freply.str();
        reply << FAIL << freply_str << DONE;
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
    reply << OK << DONE;
}

/*
** dump_machine <file.xml>
** Export a machine from the database into <file.xml>
*/
MMCSConsoleCommand_dump_machine*
MMCSConsoleCommand_dump_machine::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.mmcsConsoleCommand(true);
    commandAttributes.internalAuth(true);              // Anybody can run the console version because auth is on the server
    commandAttributes.helpCategory(ADMIN);

    return new MMCSConsoleCommand_dump_machine("dump_machine", "dump_machine <file.xml>", commandAttributes);
}

void
MMCSConsoleCommand_dump_machine::execute(deque<string> args,
                    MMCSCommandReply& reply,
                    ConsoleController* pController,
                    BlockControllerTarget* pTarget)
{
    deque<string> mmcs_dump_machine = MMCSCommandProcessor::parseCommand("mmcs_server_cmd dump_machine_server");
    sendCommand(mmcs_dump_machine, args, reply, pController);
}

void
MMCSConsoleCommand_dump_machine::help(deque<string> args,
                 MMCSCommandReply& reply)
{
    reply << OK << description()
      << ";Export a machine from the database into <file.xml>"
      << DONE;
}

/*
** dump_block <file.xml> <blockId>
** Export a block <blockId> from the database into <file.xml>
*/
MMCSConsoleCommand_dump_block*
MMCSConsoleCommand_dump_block::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.mmcsConsoleCommand(true);
    commandAttributes.mmcsServerCommand(false);
    commandAttributes.internalAuth(true);
    commandAttributes.helpCategory(ADMIN);
    return new MMCSConsoleCommand_dump_block("dump_block", "dump_block <file.xml> <blockId>", commandAttributes);
}

void
MMCSConsoleCommand_dump_block::execute(deque<string> args,
                    MMCSCommandReply& reply,
                    ConsoleController* pController,
                    BlockControllerTarget* pTarget)
{
    deque<string> mmcs_dump_block = MMCSCommandProcessor::parseCommand("mmcs_server_cmd dump_block_server " + args[1]);
    sendCommand(mmcs_dump_block, args, reply, pController);
}

std::vector<std::string>
MMCSConsoleCommand_dump_block::getBlockObjects(std::deque<std::string>& cmdString, DBConsoleController* pController) {
    std::vector<std::string> retvec;
    retvec.push_back(cmdString[1]);
    return retvec;
}

void
MMCSConsoleCommand_dump_block::help(deque<string> args,
                 MMCSCommandReply& reply)
{
    reply << OK << description()
      << ";Export a block <blockId> from the database into <file.xml>"
      << DONE;
}
