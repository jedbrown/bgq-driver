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

#include "ListBlockAuthority.h"

#include "common/Properties.h"

#include "security/privileges.h"
#include "security/exception.h"

using namespace std;

namespace mmcs {
namespace server {
namespace command {

ListBlockAuthority*
ListBlockAuthority::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.requiresObjNames(true);
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(common::DEFAULT);
    commandAttributes.internalAuth(false);
    Attributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new ListBlockAuthority("list_block_authority", "list_block_authority < blockid >", commandAttributes);
}

void
ListBlockAuthority::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    std::vector<std::string> validnames;
    if ( !args.empty() ) {
        validnames.push_back( args[0] );
    }
    this->execute( args, reply, pController, pTarget, &validnames );
}

void
ListBlockAuthority::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget,
        std::vector<std::string>* validnames
)
{
    if (!validnames) {
        reply << mmcs_client::FAIL << "Internal error, invalid command." << mmcs_client::DONE;
        return;
    }

    std::string blockid = validnames->at(0);

    using namespace hlcs::security;
    Object blockobj(Object::Block, blockid);
    Authorities authlist = hlcs::security::list(*(common::Properties::getProperties().get()), blockobj);
    Authority::Container authcont = authlist.get();
    reply << mmcs_client::OK;
    std::string lastuser;
    bool first = true;
    for (Authority::Container::iterator it = authcont.begin(); it != authcont.end(); ++it) {
        std::string curuser = it->user();
        if (curuser == lastuser) {
            reply << ", " << it->action();
        } else {
            if (!first)
                reply << std::endl;
            reply << curuser << ": " << it->action();
        }
        std::string source;
        if (it->source() == Authority::Source::Granted)
            source = "granted";
        else if (it->source() == Authority::Source::Properties)
            source = "properties";
        else {
            reply << mmcs_client::ABORT << "Internal security error, invalid authorization type." << mmcs_client::DONE;
            return;
        }
        reply << ":" << source;
        lastuser = it->user();
        first = false;
    }
    reply << mmcs_client::DONE;
    return;
}

void
ListBlockAuthority::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Shows authorities associated with specified < blockid > object"
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
