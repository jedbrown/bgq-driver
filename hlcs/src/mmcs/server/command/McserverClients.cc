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

#include "McserverClients.h"

#include "../BlockControllerBase.h"

#include "common/ConsoleController.h"

#include <control/include/mcServer/MCServerRef.h>

#include <boost/foreach.hpp>

using namespace std;

namespace mmcs {
namespace server {
namespace command {

McserverClients*
McserverClients::build() {
    Attributes commandAttributes;
    commandAttributes.internalCommand(true);          // this is an internal use command
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(common::ADMIN);
    commandAttributes.bgadminAuth(true);
    return new McserverClients("mcserver_clients", "mcserver_clients", commandAttributes);
}

void
McserverClients::execute(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    if ( !args.empty() ) {
        reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
        return;
    }

    using namespace MCServerMessageSpec;

    // get a connection to mcServer
    MCServerRef* temp;
    BlockControllerBase::mcserver_connect(temp, pController->getUser().getUser(), reply);
    const boost::scoped_ptr<MCServerRef> mcServer( temp );

    if ( reply.getStatus() ) {
        return;
    }

    MCServerMessageSpec::ListClientsRequest mcRequest;
    MCServerMessageSpec::ListClientsReply   mcReply;
    mcServer->listClients(mcRequest, mcReply);

    reply << mmcs_client::OK;
    BOOST_FOREACH( const std::string& i, mcReply._clients ) {
        reply << i << "\n";
    }
    reply << mmcs_client::DONE;
}

void
McserverClients::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Display mcServer clients"
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
