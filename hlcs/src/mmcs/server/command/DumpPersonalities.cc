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

#include "DumpPersonalities.h"

#include "../BCLinkchipInfo.h"
#include "../BCNodeInfo.h"
#include "../BlockControllerTarget.h"

namespace mmcs {
namespace server {
namespace command {

DumpPersonalities*
DumpPersonalities::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(true);            // does require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(common::ADMIN);
    Attributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new DumpPersonalities("dump_personalities", "[<target>] dump_personalities [unformatted]", commandAttributes);
}

void
DumpPersonalities::execute(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    bool formatted = true;

    if (args.size() > 0) {
        if (args[0].compare("unformatted") == 0) {
            formatted = false;
        } else {
            reply << mmcs_client::FAIL <<  "args? " << _usage << mmcs_client::DONE;
            return;
        }
    }

    reply << mmcs_client::OK;
    for (unsigned i = 0; i < pTarget->getNodes().size(); ++i) {
        BCNodeInfo *nodeInfo = pTarget->getNodes()[i];
        reply << "{" << nodeInfo->_locateId << "} " << "Dumping personality:";
        if (!formatted) {
            uint8_t numPersonalityWords = nodeInfo->personality().PersonalitySizeWords;
            unsigned* personalityWord = (unsigned*) &nodeInfo->personality();

            for (unsigned i = 0; i < numPersonalityWords; ++i) {
                if (i % 4 == 0) {
                    reply << "\n" << std::hex << std::setw(8) << std::setfill('0') << i*4 << ":";
                }
                reply << " " << std::hex << std::setw(8) << std::setfill('0') << *personalityWord;
                ++personalityWord;
            }
            reply << std::dec;
        } else {
            reply << "\n" << nodeInfo->personality() << "\n";
        }
    }

    for (unsigned i = 0; i < pTarget->getLinkchips().size(); ++i) {
        BCLinkchipInfo *bllInfo = pTarget->getLinkchips()[i];
        reply << "{" << bllInfo->_locateId << "} " << "Dumping personalities:";
    }
    reply << mmcs_client::DONE;
}

void
DumpPersonalities::help(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description() << ";Print the node personality objects" << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
