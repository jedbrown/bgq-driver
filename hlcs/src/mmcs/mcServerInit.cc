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
// Blocks Active = Blocks not in FREE state in the BGQ DB
// X = Don't Care (These cases are misconfigurations that will cause
// mmcs_server to not end with an error reading the properties file.)
//
// Subnets Home | Reconnect | Bringup | Blocks Active | Request |
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
#include "ReconnectBlocks.h"
#include "DefaultControlEventListener.h"
#include "MMCSSubnet.h"

#include <control/include/mcServer/MCServer_errno.h>
#include <control/include/mcServer/MCServerRef.h>

using namespace MCServerMessageSpec;

LOG_DECLARE_FILE("mmcs");

extern MCServerMessageType message_type;
extern bool subnets_home;

void
mcServerInit(
        const vector<string>& bringup_options, 
        MMCSCommandReply& reply, 
        const bool blocks_are_active
        )
{
    DefaultControlEventListener* defaultRasListener = DefaultControlEventListener::getDefaultControlEventListener();

    // Determine what kind of message(s) will eventually get sent to mc_server.

    //
    // establish a socket connection to mcServer
    //
    while(true) {
        if (!defaultRasListener->getBase()->isConnected()) {
            defaultRasListener->getBase()->mcserver_connect(reply);
            if (reply.getStatus() != 0)
                {
                    LOG_WARN_MSG( "mcServerInit mcserver_connect() failed: " << reply.str() );
                    sleep(1);
                }
            else break; // Got a connection
        } else break; // already connected
    }

    //
    // Start a generic RAS listener
    //
    if (!defaultRasListener->getBase()->isMailboxStarted())
    {
	defaultRasListener->getBase()->startMailbox(reply);
	if (reply.getStatus() != 0)
	{
	    delete defaultRasListener;
	    defaultRasListener = NULL;
	    return;
	}
    }

    subnets_home = true;
    // Check to see if subnets are on their primary SSNs.  If they are, send
    // BU request.  Otherwise, let reconnect/failover logic have its way.
    BOOST_FOREACH(MMCSSubnet& curr_subnet, MMCSProperties::_subnets) {
        if(curr_subnet._home == false) {
            subnets_home = false;
            break;
        }
    }

    LOG_DEBUG_MSG("Subnets home is " << subnets_home);

    if(subnets_home == false && 
       MMCSProperties::getProperty(RECONNECT_BLOCKS) == "false" &&
       MMCSProperties::getProperty(BRINGUP) == "false" &&
       blocks_are_active == false) {
        // H=0,R=0,B=0,A=0 x
        message_type = FAILOVER_MSG;
    } else if(subnets_home == false && 
              MMCSProperties::getProperty(RECONNECT_BLOCKS) == "true" &&
              MMCSProperties::getProperty(BRINGUP) == "false") {
        // H=0,R=1,B=0,A=0 x
        // H=0,R=1,B=0,A=1 x
        message_type = FAILOVER_MSG;
    } else if(subnets_home == true && 
              MMCSProperties::getProperty(RECONNECT_BLOCKS) == "true" &&
              MMCSProperties::getProperty(BRINGUP) == "false" &&
              blocks_are_active == true) {
        // H=1,R=1,B=0,A=1 x
        message_type = FAILOVER_MSG;
    } else if(subnets_home == false && 
              MMCSProperties::getProperty(RECONNECT_BLOCKS) == "false" &&
              blocks_are_active == true) {
        // H=0,R=0,B=0,A=1 x
        // H=0,R=0,B=1,A=1 x
        message_type = DIE_MSG;
    } else if(subnets_home == true && 
              MMCSProperties::getProperty(RECONNECT_BLOCKS) == "false" &&
              blocks_are_active == true) {
        // H=1,R=0,B=0,A=1 x
        // H=1,R=0,B=1,A=1 x
        message_type = DIE_MSG;
    } else if(MMCSProperties::getProperty(BRINGUP) == "true" &&
              MMCSProperties::getProperty(RECONNECT_BLOCKS) == "false" &&
              blocks_are_active == false) {
        // H=0,R=0,B=1,A=0 x
        // H=1,R=0,B=1,A=0 x
        message_type = NO_MSG;
    } else if(subnets_home == true &&
              MMCSProperties::getProperty(BRINGUP) == "false" &&
              blocks_are_active == false) {
        // H=1,R=0,B=0,A=0 x
        // H=1,R=1,B=0,A=0 x
        message_type = BRINGUP_MSG;
    }       

    // Send a bringup message to mcServer
    //
    BringupRequest mcBringupRequest;
    mcBringupRequest._killMcServerIfHwStarted = true;
    BringupReply   mcBringupReply;
    if(message_type == BRINGUP_MSG) {
        // Don't send a bringup message if it has been done automatically by mc
        // add bringup options
        for (vector<string>::const_iterator it = bringup_options.begin(); it != bringup_options.end(); ++it)
        {
            if (!mcBringupRequest._bringupOptions.empty())
                mcBringupRequest._bringupOptions.append(",");
            mcBringupRequest._bringupOptions.append(*it);
        }
        // send a BringupRequest to mcserver
        try
            {
                defaultRasListener->getBase()->getMCServer()->bringup(mcBringupRequest, mcBringupReply);
            }
        catch (exception &e)
            {
                mcBringupReply._rc = -1;
                mcBringupReply._rt = e.what();
            }
        if (mcBringupReply._rc)
            {
                reply << FAIL << "mcServer bringup: " << mcBringupReply._rt << DONE;
                delete defaultRasListener;
                defaultRasListener = NULL;
                return;
            }
        LOG_INFO_MSG("Bringup request complete");
    } 

    reply << OK << DONE;
}

void
mcServerTerm(
        MMCSCommandReply& reply
        )
{
    DefaultControlEventListener* defaultRasListener = DefaultControlEventListener::getDefaultControlEventListener();
    defaultRasListener->disconnect();
    reply << OK << DONE;
}
