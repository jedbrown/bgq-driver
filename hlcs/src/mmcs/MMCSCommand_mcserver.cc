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
 * \file MMCSCommand_mcserver.cc
 */

#include "BlockControllerTarget.h"
#include "ConsoleController.h"
#include "MMCSCommandReply.h"
#include "MMCSCommand_mcserver.h"

#include <control/include/mcServer/MCServerRef.h>

#include <boost/foreach.hpp>
#include <boost/scoped_ptr.hpp>

MMCSCommand_mcserver_status*
MMCSCommand_mcserver_status::build() {
    MMCSCommandAttributes commandAttributes;
    commandAttributes.internalCommand(true);          // this is an internal use command
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(ADMIN);
    MMCSCommandAttributes::AuthPair hardwareread(hlcs::security::Object::Hardware, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(hardwareread);
    return new MMCSCommand_mcserver_status("mcserver_status", "mcserver_status [<regexp>]", commandAttributes);
}

void
MMCSCommand_mcserver_status::execute(
        std::deque<std::string> args,
        MMCSCommandReply& reply,
        ConsoleController* pController,
        BlockControllerTarget* pTarget)
{
    using namespace MCServerMessageSpec;

    // get a connection to mcServer
    MCServerRef* temp;
    BlockControllerBase::mcserver_connect(temp, pController->getUser().getUser(), reply);
    const boost::scoped_ptr<MCServerRef> mcServer( temp );

    if ( reply.getStatus() ) return;

    MCServerMessageSpec::StatusRequest mcStatusRequest;
    if ( args.empty() ) {
        mcStatusRequest._expression.push_back( ".*" );
    } else if ( args.size() == 1 ) {
        mcStatusRequest._expression.push_back( args[0] );
    } else {
        reply << FAIL << "args? " << usage << DONE;
        return;
    }

    MCServerMessageSpec::StatusReply   mcStatusReply;
    mcServer->status(mcStatusRequest, mcStatusReply);

    if (mcStatusReply._rc)
    {
        reply << FAIL << mcStatusReply._rt << DONE;
        return;
    }

    // return the status by midplane
    reply << OK;
    for (vector<StatusReply::Midplane>::iterator midplane = mcStatusReply._midplanes.begin();
            midplane != mcStatusReply._midplanes.end(); ++midplane)
    {
        for (vector<string>::iterator status= (*midplane)._statusStrings.begin();
                status != (*midplane)._statusStrings.end(); ++status)
        {
            reply << (*status) << "\n";
        }
    }
    reply << DONE;
}

void
MMCSCommand_mcserver_status::help(
        deque<string> args,
        MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
        << ";display mcServer status"
        << DONE;
}

MMCSCommand_mcserver_clients*
MMCSCommand_mcserver_clients::build() {
    MMCSCommandAttributes commandAttributes;
    commandAttributes.internalCommand(true);          // this is an internal use command
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(ADMIN);
    commandAttributes.bgadminAuth(true);
    return new MMCSCommand_mcserver_clients("mcserver_clients", "mcserver_clients", commandAttributes);
}

void
MMCSCommand_mcserver_clients::execute(
        std::deque<std::string> args,
        MMCSCommandReply& reply,
        ConsoleController* pController,
        BlockControllerTarget* pTarget)
{
    if ( !args.empty() ) {
        reply << FAIL << "args? " << usage << DONE;
        return;
    }

    using namespace MCServerMessageSpec;

    // get a connection to mcServer
    MCServerRef* temp;
    BlockControllerBase::mcserver_connect(temp, pController->getUser().getUser(), reply);
    const boost::scoped_ptr<MCServerRef> mcServer( temp );

    if ( reply.getStatus() ) return;

    MCServerMessageSpec::ListClientsRequest mcRequest;
    MCServerMessageSpec::ListClientsReply   mcReply;
    mcServer->listClients(mcRequest, mcReply);

    reply << OK;
    BOOST_FOREACH( const std::string& i, mcReply._clients ) {
        reply << i << "\n";
    }
    reply << DONE;
}

void
MMCSCommand_mcserver_clients::help(
        deque<string> args,
        MMCSCommandReply& reply)
{
    reply << OK << description()
        << ";display mcServer clients"
        << DONE;
}
