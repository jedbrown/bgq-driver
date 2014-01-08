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

#include "GrantBlockAuthority.h"

#include "../DBConsoleController.h"
#include "../ras.h"

#include "security/privileges.h"
#include "security/exception.h"

#include <db/include/api/BGQDBlib.h>

#include <ras/include/RasEventImpl.h>
#include <ras/include/RasEventHandlerChain.h>

using namespace std;

namespace mmcs {
namespace server {
namespace command {

GrantBlockAuthority*
GrantBlockAuthority::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.requiresObjNames(true);
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(common::USER);
    commandAttributes.internalAuth(true);
    commandAttributes.specialAuthString("Administrative or block ownership");
    return new GrantBlockAuthority("grant_block_authority", "grant_block_authority < blockid > < user > < action >", commandAttributes);
}

void
GrantBlockAuthority::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    std::string block = args[0];
    std::string newuser = args[1];
    std::string action = args[2];
    // An administrator can do anything, anybody else has to be the block owner
    std::string blockowner;

    if (BGQDB::getBlockOwner(block, blockowner) != BGQDB::OK) {
        reply << mmcs_client::FAIL << "Cannot get block owner for " << block
              << " from database.  Is it a valid block?" << mmcs_client::DONE;
        return;
    }

    if (pController->getUserType() == CxxSockets::Administrator || blockowner == pController->getUser().getUser()) {
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
            reply << mmcs_client::FAIL << action << " is not a valid security action type." << mmcs_client::DONE;
            return;
        }
        try {
            hlcs::security::Object blockobj(hlcs::security::Object::Block, block);
            hlcs::security::Authority auth(newuser, action_type);
            hlcs::security::grant(blockobj, auth, pController->getUser());
        } catch(const hlcs::security::exception::ObjectNotFound& e) {
            reply << mmcs_client::FAIL << "Database block object error for " << block << " : " << e.what() << mmcs_client::DONE;
            return;
        } catch(const hlcs::security::exception::DatabaseError& e) {
            reply << mmcs_client::FAIL << "Database error detected: " << e.what() << mmcs_client::DONE;
            return;
        } catch(const std::invalid_argument& e) {
            reply << mmcs_client::FAIL << "Object " << block << " is not a block." << e.what() << mmcs_client::DONE;
            return;
        } catch(const std::logic_error& e) {
            reply << mmcs_client::FAIL << "Action " << action << " not valid for object "
                << block << ": " << e.what() << mmcs_client::DONE;
            return;
        }
        reply << mmcs_client::OK << mmcs_client::DONE;
    } else {
        RasEventImpl event(MMCSOps_0301);
        event.setDetail("USER", pController->getUser().getUser());
        event.setDetail("COMMAND", this->name());
        RasEventHandlerChain::handle(event);
        BGQDB::putRAS(event);
        reply << mmcs_client::FAIL << event.getDetail(RasEvent::MESSAGE) << mmcs_client::DONE;
    }
}

void
GrantBlockAuthority::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Grants authority to perform < action > to < user > on < blockid > object."
          << ";Actions may be 'read', 'update', 'delete' or 'execute'."
          << ";This does NOT affect special authorities created in the properties file."
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
