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

#include "McserverStatus.h"

#include "../BlockControllerBase.h"

#include "common/ConsoleController.h"

#include <control/include/mcServer/MCServerRef.h>

using namespace std;

namespace mmcs {
namespace server {
namespace command {

McserverStatus*
McserverStatus::build() {
    Attributes commandAttributes;
    commandAttributes.internalCommand(true);          // this is an internal use command
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(common::ADMIN);
    Attributes::AuthPair hardwareread(hlcs::security::Object::Hardware, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(hardwareread);
    return new McserverStatus("mcserver_status", "mcserver_status [<regexp>]", commandAttributes);
}

void
McserverStatus::execute(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    using namespace MCServerMessageSpec;

    // get a connection to mcServer
    MCServerRef* temp;
    BlockControllerBase::mcserver_connect(temp, pController->getUser().getUser(), reply);
    const boost::scoped_ptr<MCServerRef> mcServer( temp );

    if ( reply.getStatus() ) {
        return;
    }

    MCServerMessageSpec::StatusRequest mcStatusRequest;
    if ( args.empty() ) {
        mcStatusRequest._expression.push_back( ".*" );
    } else if ( args.size() == 1 ) {
        mcStatusRequest._expression.push_back( args[0] );
    } else {
        reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
        return;
    }

    MCServerMessageSpec::StatusReply   mcStatusReply;
    mcServer->status(mcStatusRequest, mcStatusReply);

    if (mcStatusReply._rc) {
        reply << mmcs_client::FAIL << mcStatusReply._rt << mmcs_client::DONE;
        return;
    }

    // return the status by midplane
    reply << mmcs_client::OK;
    for (
            vector<StatusReply::Midplane>::iterator midplane = mcStatusReply._midplanes.begin();
            midplane != mcStatusReply._midplanes.end();
            ++midplane
        )
    {
        for (
                vector<string>::iterator status= (*midplane)._statusStrings.begin();
                status != (*midplane)._statusStrings.end();
                ++status
            )
        {
            reply << (*status) << "\n";
        }
    }
    reply << mmcs_client::DONE;
}

void
McserverStatus::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Display mcServer status"
          << mmcs_client::DONE;
}


} } } // namespace mmcs::server::command
