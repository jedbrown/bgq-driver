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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

// This is the mc_server message truth table.  The request(s)
// that get sent to mc_server are based on this.
//
// Subnets Home = SubnetMc processes are running on their default hosts.
// Reconnect = [mmcs] section's reconnect flag.
// Bringup = [machinecontroller] section's bringup flag.
// Hardware Active = Blocks not in FREE state in the BGQ DB or hardware in Service
// X = Don't Care (These cases are misconfigurations that will cause
// mmcs_server to not end with an error reading the properties file.)
//
// Subnets Home | Reconnect | Bringup |   HW Active   | Request |
// --------------------------------------------------------------
//      0       |    0      |    0    |       0       |   X     |
// --------------------------------------------------------------
//      0       |    0      |    0    |       1       |   X     |
// --------------------------------------------------------------
//      0       |    0      |    1    |       0       | None    |
// --------------------------------------------------------------
//      0       |    0      |    1    |       1       | DIE     |
// --------------------------------------------------------------
//      0       |    1      |    0    |       0       | Failover|
// --------------------------------------------------------------
//      0       |    1      |    0    |       1       | Failover|
// --------------------------------------------------------------
//      0       |    1      |    1    |       0       |   X     |
// --------------------------------------------------------------
//      0       |    1      |    1    |       1       |   X     |
// --------------------------------------------------------------
//      1       |    0      |    0    |       0       |   X     |
// --------------------------------------------------------------
//      1       |    0      |    0    |       1       |   X     |
// --------------------------------------------------------------
//      1       |    0      |    1    |       0       | None    |
// --------------------------------------------------------------
//      1       |    0      |    1    |       1       | DIE     |
// --------------------------------------------------------------
//      1       |    1      |    0    |       0       | Bringup |
// --------------------------------------------------------------
//      1       |    1      |    0    |       1       | Failover|
// --------------------------------------------------------------
//      1       |    1      |    1    |       0       |   X     |
// --------------------------------------------------------------
//      1       |    1      |    1    |       1       |   X     |
// --------------------------------------------------------------
//
// This can be interpreted like this:
//
// H = Home
// R = Reconnect
// B = Bringup
// A = Blocks Active
//
// Failover:
// H=0,R=0,B=0,A=0
// H=0,R=1,B=0,A=0
// H=0,R=1,B=0,A=1
// H=1,R=1,B=0,A=1
// DIE:
// H=0,R=0,B=0,A=1
// H=0,R=0,B=1,A=1
// H=1,R=0,B=0,A=1
// H=1,R=0,B=1,A=1
// None:
// H=0,R=0,B=1,A=0
// H=1,R=0,B=1,A=0
// Bringup:
// H=1,R=0,B=0,A=0
// H=1,R=1,B=0,A=0
//

#include "mcServerInit.h"

#include "BlockControllerBase.h"
#include "DefaultListener.h"
#include "ReconnectBlocks.h"

#include "common/Properties.h"
#include "common/Subnet.h"

#include <control/include/mcServer/MCServer_errno.h>
#include <control/include/mcServer/MCServerRef.h>

using namespace MCServerMessageSpec;

using mmcs::common::Properties;

LOG_DECLARE_FILE("mmcs.server");

namespace mmcs {
namespace server {

MCServerMessageType message_type;
bool subnets_home = true;

void
mcServerInit(
        const std::vector<std::string>& bringup_options,
        mmcs_client::CommandReply& reply,
        const bool blocks_are_active
        )
{
    DefaultListener* defaultRasListener(NULL);
    try {
        defaultRasListener = DefaultListener::get();
    } catch ( const std::runtime_error& e ) {
        reply << mmcs_client::FAIL << e.what() << mmcs_client::DONE;
        return;
    }
    BOOST_ASSERT( defaultRasListener );

    // Determine what kind of message(s) will eventually get sent to mc_server.

    // Establish a socket connection to mcServer
    LOG_INFO_MSG("Attempting to connect to mc_server.");
    while (true) {
        if (!defaultRasListener->getBase()->isConnected()) {
            defaultRasListener->getBase()->mcserver_connect(reply);
            if (reply.getStatus() != 0) {
                sleep(3);
            } else {
                break; // Got a connection
            }
        } else {
            break; // Already connected
        }
    }
    LOG_INFO_MSG("Connected to mc_server successfully.");

    // Start a generic RAS listener
    if (!defaultRasListener->getBase()->isMailboxStarted()) {
        defaultRasListener->getBase()->startMailbox(reply);
        if (reply.getStatus() != 0) {
            delete defaultRasListener;
            defaultRasListener = NULL;
            return;
        }
    }

    subnets_home = true;
    // Check to see if subnets are on their primary SSNs. If they are, send
    // BU request. Otherwise, let reconnect/failover logic have its way.
    BOOST_FOREACH(const common::Subnet& curr_subnet, Properties::_subnets) {
        if (curr_subnet._home == false) {
            subnets_home = false;
            break;
        }
    }

    LOG_DEBUG_MSG("Subnets home is " << subnets_home);

    if (subnets_home == false &&
       Properties::getProperty(RECONNECT_BLOCKS) == "false" &&
       Properties::getProperty(BRINGUP) == "false" &&
       blocks_are_active == false) {
        // H=0,R=0,B=0,A=0 x
        message_type = FAILOVER_MSG;
    } else if(subnets_home == false &&
              Properties::getProperty(RECONNECT_BLOCKS) == "true" &&
              Properties::getProperty(BRINGUP) == "false") {
        // H=0,R=1,B=0,A=0 x
        // H=0,R=1,B=0,A=1 x
        message_type = FAILOVER_MSG;
    } else if(subnets_home == true &&
              Properties::getProperty(RECONNECT_BLOCKS) == "true" &&
              Properties::getProperty(BRINGUP) == "false" &&
              blocks_are_active == true) {
        // H=1,R=1,B=0,A=1 x
        message_type = FAILOVER_MSG;
    } else if(subnets_home == false &&
              Properties::getProperty(RECONNECT_BLOCKS) == "false" &&
              blocks_are_active == true) {
        // H=0,R=0,B=0,A=1 x
        // H=0,R=0,B=1,A=1 x
        message_type = DIE_MSG;
    } else if(subnets_home == true &&
              Properties::getProperty(RECONNECT_BLOCKS) == "false" &&
              blocks_are_active == true) {
        // H=1,R=0,B=0,A=1 x
        // H=1,R=0,B=1,A=1 x
        message_type = DIE_MSG;
    } else if(Properties::getProperty(BRINGUP) == "true" &&
              Properties::getProperty(RECONNECT_BLOCKS) == "false" &&
              blocks_are_active == false) {
        // H=0,R=0,B=1,A=0 x
        // H=1,R=0,B=1,A=0 x
        message_type = NO_MSG;
    } else if(subnets_home == true &&
              Properties::getProperty(BRINGUP) == "false" &&
              blocks_are_active == false) {
        // H=1,R=0,B=0,A=0 x
        // H=1,R=1,B=0,A=0 x
        message_type = BRINGUP_MSG;
    }

    // Send a bringup message to mcServer
    BringupRequest mcBringupRequest;
    mcBringupRequest._killMcServerIfHwStarted = true;
    BringupReply mcBringupReply;
    if (message_type == BRINGUP_MSG) {
        // Don't send a bringup message if it has been done automatically by mc
        // add bringup options
        for (std::vector<std::string>::const_iterator it = bringup_options.begin(); it != bringup_options.end(); ++it) {
            if (!mcBringupRequest._bringupOptions.empty())
                mcBringupRequest._bringupOptions.append(",");
            mcBringupRequest._bringupOptions.append(*it);
        }
        // Send a BringupRequest to mcserver
        try {
            defaultRasListener->getBase()->getMCServer()->bringup(mcBringupRequest, mcBringupReply);
        } catch (const std::exception& e) {
            mcBringupReply._rc = -1;
            mcBringupReply._rt = e.what();
        }

        if (mcBringupReply._rc) {
            reply << mmcs_client::FAIL << "mcServer bringup: " << mcBringupReply._rt << mmcs_client::DONE;
            delete defaultRasListener;
            defaultRasListener = NULL;
            return;
        }
        LOG_INFO_MSG("Bringup request complete.");
    }

    reply << mmcs_client::OK << mmcs_client::DONE;
}

} } // namespace mmcs::server
