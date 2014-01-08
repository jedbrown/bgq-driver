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
/* (C) Copyright IBM Corp.  2011, 2012                              */
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
#include <boost/foreach.hpp>
#include <control/include/mcServer/MCServerRef.h>
#include "MMCSCommand_shutdown_hardware.h"
#include "HardwareBlockList.h"

MMCSCommand_shutdown_hardware*
MMCSCommand_shutdown_hardware::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.mmcsServerCommand(false);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(ADMIN);
    MMCSCommandAttributes::AuthPair hardware_execute(hlcs::security::Object::Hardware, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(hardware_execute);
    return new MMCSCommand_shutdown_hardware("shutdown_hardware", "shutdown_hardware <subnet_id | all>", commandAttributes);
}

void
MMCSCommand_shutdown_hardware::execute(
        deque<string> args,
        MMCSCommandReply& reply,
        ConsoleController* pController,
        BlockControllerTarget* pTarget
        )
{
    MCServerMessageSpec::ShutdownHardwareRequest hw_request;
    MCServerMessageSpec::ShutdownHardwareReply hw_reply;
    if(args[0] == "all") {
        // Loop through all of the subnets and add them to the request.
        BOOST_FOREACH(MMCSSubnet& sn, MMCSProperties::_subnets) {
            hw_request._subnet.push_back(sn.get_name());
            HardwareBlockList::add_to_list(sn.get_name());
        }
    } else {
        // Check that it's in the subnet list and add it to the request.
        BOOST_FOREACH(MMCSSubnet& sn, MMCSProperties::_subnets) {
            if(sn.get_name() == args[0]) {
                hw_request._subnet.push_back(args[0]);
                HardwareBlockList::add_to_list(args[0]);
            }
        }
    }

    if(hw_request._subnet.size() == 0) {
        reply << FAIL << "No valid subnets specified." << DONE;
    } else {
        // Connect to mc_server and send the request.
        MCServerRef* temp;
        BlockControllerBase::mcserver_connect(temp, pController->getUser().getUser(), reply);
        const boost::scoped_ptr<MCServerRef> mcServer( temp );
        if(reply.getStatus()) return;
        mcServer->shutdownHardware(hw_request, hw_reply);
        if(hw_reply._rc) {
            reply << FAIL << hw_reply._rt << DONE;
            return;
        }
        reply << OK << DONE;
    }
}

void
MMCSCommand_shutdown_hardware::help(
        deque<string> args,
        MMCSCommandReply& reply
        )
{
    reply << OK << description()
        << ";Perform controlled shutdown of BG/Q hardware."
        << DONE;

}
