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
 * \file MMCSServerCommand_xml.cc
 */

#include "ConsoleController.h"
#include "DBBlockController.h"
#include "MMCSServerCommand_xml.h"
#include "MMCSCommandReply.h"

#include <db/include/api/BGQDBlib.h>

#include <fstream>

#include <sys/stat.h>
#include <sys/types.h>

#include <pwd.h>

using namespace std;

/*
** dump_machine <file.xml>
** Export a machine from the database into <file.xml>
*/
MMCSServerCommand_dump_machine*
MMCSServerCommand_dump_machine::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsConsoleCommand(false);
    commandAttributes.helpCategory(SPECIAL);
    MMCSCommandAttributes::AuthPair hardwareread(hlcs::security::Object::Hardware, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(hardwareread);

    return new MMCSServerCommand_dump_machine("dump_machine_server", "dump_machine <file.xml>", commandAttributes);
}

void
MMCSServerCommand_dump_machine::execute(deque<string> args,
                    MMCSCommandReply& reply,
                    ConsoleController* pController,
                    BlockControllerTarget* pTarget)
{
    std::ostringstream machineStreamXML;
    if (BGQDB::getMachineXML(machineStreamXML) == 0) {
        reply << OK << machineStreamXML.str() << DONE;
        return;
    }
    reply << FAIL << "generation of machine xml failed." << DONE;
}

void
MMCSServerCommand_dump_machine::help(deque<string> args,
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
MMCSServerCommand_dump_block*
MMCSServerCommand_dump_block::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.requiresObjNames(true);
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsConsoleCommand(false);
    MMCSCommandAttributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    commandAttributes.helpCategory(SPECIAL);
    return new MMCSServerCommand_dump_block("dump_block_server", "dump_block <file.xml> <blockId>", commandAttributes);
}

void
MMCSServerCommand_dump_block::execute(deque<string> args,
                    MMCSCommandReply& reply,
                    ConsoleController* pController,
                    BlockControllerTarget* pTarget)
{
    std::vector<std::string> validnames;
    validnames.push_back(args[0]);
    execute(args, reply, pController, pTarget, &validnames);
}

void
MMCSServerCommand_dump_block::execute(deque<string> args,
                    MMCSCommandReply& reply,
                    ConsoleController* pController,
                    BlockControllerTarget* pTarget,
                    std::vector<std::string>* validnames)
{
    //write XML file into stream object
    std::ostringstream blockStreamXML;
    BGQDB::STATUS result = BGQDB::OK;
    try {
        result = BGQDB::getBlockXML(blockStreamXML, validnames->at(0));
        if ( result == BGQDB::OK ) {
            reply << OK << blockStreamXML.str() << DONE;
            return;
        }
    } catch(...) {
        reply << FAIL << "Database error.  Could not dump block " << validnames->at(0) << ". " << DONE;
        return;
    }

    reply
        << FAIL <<
        "generation of XML failed for block " << validnames->at(0) << ": " <<
        DBBlockController::strDBError(result) <<
        DONE
        ;
}

std::vector<std::string>
MMCSServerCommand_dump_block::getBlockObjects(std::deque<std::string>& cmdString, DBConsoleController* pController) {
    std::vector<std::string> retvec;
    retvec.push_back(cmdString[0]);
    return retvec;
}

void
MMCSServerCommand_dump_block::help(deque<string> args,
                 MMCSCommandReply& reply)
{
    reply << OK << description()
      << ";Export a block <blockId> from the database into <file.xml>"
      << DONE;
}
