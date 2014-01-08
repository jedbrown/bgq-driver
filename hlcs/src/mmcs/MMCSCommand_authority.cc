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
 * \file MMCSCommand_authority.cc
 */

#include <db/include/api/BGQDBlib.h>
#include <hlcs/include/security/privileges.h>
#include <hlcs/include/security/exception.h>
#include <ras/include/RasEventImpl.h>
#include <ras/include/RasEventHandlerChain.h>
#include "MMCSCommand_authority.h"
#include "MMCSCommandReply.h"
#include "DBConsoleController.h"
#include "DBBlockController.h"
#include "MMCSProperties.h"
#include "MMCSCommandProcessor.h"
#include "ras.h"
#include "utility/include/Log.h"

LOG_DECLARE_FILE( "mmcs.command" );

MMCSCommand_grant_block_authority*
MMCSCommand_grant_block_authority::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.requiresObjNames(true);
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(USER);
    commandAttributes.internalAuth(true);
    commandAttributes.specialAuthString("Administrative or block ownership");
    return new MMCSCommand_grant_block_authority("grant_block_authority", "grant_block_authority < blockid > < user > < action >", commandAttributes);
}

void
MMCSCommand_grant_block_authority::execute(deque<string> args,
                                           MMCSCommandReply& reply,
                                           DBConsoleController* pController,
                                           BlockControllerTarget* pTarget)
{
    std::string block = args[0];
    std::string newuser = args[1];
    std::string action = args[2];
    // An administrator can do anything, anybody else has to be the block owner
    std::string blockowner;

    if(BGQDB::getBlockOwner(block, blockowner) != BGQDB::OK) {
        reply << FAIL << "Cannot get block owner for " << block
              << " from database.  Is it a valid block?" << DONE;
        return;
    }

    if(pController->getUserType() == CxxSockets::Administrator ||
       blockowner == pController->getUser().getUser()) {
        hlcs::security::Action::Type action_type;
        if(action == "read")
            action_type = hlcs::security::Action::Read;
        else if(action == "update")
            action_type = hlcs::security::Action::Update;
        else if(action == "delete")
            action_type = hlcs::security::Action::Delete;
        else if(action == "execute")
            action_type = hlcs::security::Action::Execute;
        else {
            reply << FAIL << action << " is not a valid security action type." << DONE;
            return;
        }
        try {
            hlcs::security::Object blockobj(hlcs::security::Object::Block, block);
            hlcs::security::Authority auth(newuser, action_type);
            hlcs::security::grant(blockobj, auth, pController->getUser());
        } catch(hlcs::security::exception::ObjectNotFound& e) {
            reply << FAIL << "Database block object error for " << block << " : " << e.what() << DONE;
            return;
        } catch(hlcs::security::exception::DatabaseError& e) {
            reply << FAIL << "Database error detected: " << e.what() << DONE;
            return;
        } catch(std::invalid_argument& e) {
            reply << FAIL << "Object " << block << " is not a block." << e.what() << DONE;
            return;
        } catch(std::logic_error& e) {
            reply << FAIL << "Action " << action << " not valid for object "
                  << block << ": " << e.what() << DONE;
            return;
        }
        reply << OK << DONE;
    } else {
        RasEventImpl event(MMCSOps_0301);
        event.setDetail("USER", pController->getUser().getUser());
        event.setDetail("COMMAND", this->name());
        RasEventHandlerChain::handle(event);
        BGQDB::putRAS(event);
        reply << FAIL << event.getDetail(RasEvent::MESSAGE) << DONE;
    }
}

void
MMCSCommand_grant_block_authority::help(deque<string> args,
                                        MMCSCommandReply& reply)
{
    reply << OK << description()
          << ";Grants authority to perform < action > to < user > on < blockid > object."
          << ";Actions may be 'read', 'update', 'delete' or 'execute'."
          << ";This does NOT affect special authorities created in the properties file."
          << DONE;
}

MMCSCommand_revoke_block_authority*
MMCSCommand_revoke_block_authority::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.requiresObjNames(true);
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(USER);
    commandAttributes.internalAuth(true);
    commandAttributes.specialAuthString("Administrative or block ownership");
    return new MMCSCommand_revoke_block_authority("revoke_block_authority", "revoke_block_authority < blockid > < user > < actions >", commandAttributes);
}

void
MMCSCommand_revoke_block_authority::execute(deque<string> args,
                                            MMCSCommandReply& reply,
                                            DBConsoleController* pController,
                                            BlockControllerTarget* pTarget)
{
    std::string block = args[0];
    std::string newuser = args[1];
    std::string action = args[2];
    // An administrator can do anything, anybody else has to be the block owner
    std::string blockowner;
    if(BGQDB::getBlockOwner(block, blockowner) != BGQDB::OK) {
        reply << FAIL << "Cannot get block owner for " << block << " from database." << DONE;
        return;
    }

    if(pController->getUserType() == CxxSockets::Administrator ||
       blockowner == pController->getUser().getUser()) {
        hlcs::security::Action::Type action_type;
        if(action == "read")
            action_type = hlcs::security::Action::Read;
        else if(action == "update")
            action_type = hlcs::security::Action::Update;
        else if(action == "delete")
            action_type = hlcs::security::Action::Delete;
        else if(action == "execute")
            action_type = hlcs::security::Action::Execute;
        else {
            reply << FAIL << action << " is not a valid security action type." << DONE;
            return;
        }
        try {
            hlcs::security::Object blockobj(hlcs::security::Object::Block, block);
            hlcs::security::Authority auth(newuser, action_type);
            hlcs::security::revoke(blockobj, auth, pController->getUser());
        } catch(hlcs::security::exception::ObjectNotFound& e) {
            reply << FAIL << "Database block object error for " << block << " : " << e.what() << DONE;
            return;
        } catch(hlcs::security::exception::DatabaseError& e) {
            reply << FAIL << "Database error detected: " << e.what() << DONE;
            return;
        } catch(std::invalid_argument& e) {
            reply << FAIL << "Object " << block << " is not a block." << e.what() << DONE;
            return;
        } catch(std::logic_error& e) {
            reply << FAIL << "Action " << action << " not valid for object "
                  << block << ": " << e.what() << DONE;
            return;
        }
        reply << OK << DONE;
    } else {
        RasEventImpl event(MMCSOps_0301);
        event.setDetail("USER", pController->getUser().getUser());
        event.setDetail("COMMAND", this->name());
        RasEventHandlerChain::handle(event);
        BGQDB::putRAS(event);
        reply << FAIL << event.getDetail(RasEvent::MESSAGE) << DONE;
    }
}

void
MMCSCommand_revoke_block_authority::help(deque<string> args,
                                         MMCSCommandReply& reply)
{
    reply << OK << description()
          << ";Revokes authority to < action > from < user > on < blockid > object."
          << ";Actions may be 'read', 'update', 'delete' or 'execute'."
          << ";This does NOT affect special authorities created in the properties file."
          << DONE;
}

MMCSCommand_list_block_authority*
MMCSCommand_list_block_authority::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.requiresObjNames(true);
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(DEFAULT);
    commandAttributes.internalAuth(false);
    MMCSCommandAttributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new MMCSCommand_list_block_authority("list_block_authority", "list_block_authority < blockid >", commandAttributes);
}

void
MMCSCommand_list_block_authority::execute(deque<string> args,
			      MMCSCommandReply& reply,
			      DBConsoleController* pController,
                              BlockControllerTarget* pTarget,
                              std::vector<std::string>* validnames)
{
    if(!validnames) {
        reply << FAIL << "Internal error.  Invalid command." << DONE;
        return;
    }

    std::string blockid = validnames->at(0);

    using namespace hlcs::security;
    Object blockobj(Object::Block, blockid);
    Authorities authlist = hlcs::security::list(*(MMCSProperties::getProperties().get()), blockobj);
    Authority::Container authcont = authlist.get();
    reply << OK;
    std::string lastuser;
    bool first = true;
    for(Authority::Container::iterator it = authcont.begin();
        it != authcont.end(); ++it) {
        std::string curuser = it->user();
        if(curuser == lastuser) {
            reply << ", " << it->action();
        }
        else {
            if(!first)
                reply << std::endl;
            reply << curuser << ": " << it->action();
        }
        std::string source;
        if(it->source() == Authority::Source::Granted)
            source = "granted";
        else if(it->source() == Authority::Source::Properties)
            source = "properties";
        else {
            reply << ABORT << "Internal security error.  Invalid authorization type." << DONE;
            return;
        }
        reply << ":" << source;
        lastuser = it->user();
        first = false;
    }
    reply << DONE;
    return;
}

void
MMCSCommand_list_block_authority::help(deque<string> args,
                                       MMCSCommandReply& reply)
{
    reply << OK << description()
          << ";Shows authorities associated with specified < blockid > object"
          << DONE;
}
