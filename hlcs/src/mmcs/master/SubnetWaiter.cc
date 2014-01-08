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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

#include "HardwareBlockList.h"
#include "master/Monitor.h"
#include "master/SubnetWaiter.h"
#include "master/WaiterList.h"
#include "MMCSProperties.h"

#include "../../master/lib/exceptions.h"

#include <control/include/mcServer/MCServerRef.h>
#include <utility/include/Log.h>

LOG_DECLARE_FILE( "mmcs" );

namespace mmcs {
namespace master {

void*
SubnetWaiter::threadStart()
{
    // Open a client API connect to bgmaster_server
    // Needs to get master location from properties and command line
    bgq::utility::PortConfiguration::Pairs portpairs;
    LOG_INFO_MSG("Starting subnet_mc termination waiter for " << _alias);
    bgq::utility::ClientPortConfiguration port_config(32042);
    port_config.setProperties(MMCSProperties::getProperties(), "master.client");
    port_config.notifyComplete();
    portpairs = port_config.getPairs();
    if(portpairs[0].first.length() == 0) {
        LOG_FATAL_MSG("No port pairs or invalid port pairs specified");
        exit(1);
    }

    BGMasterClient client;
    bgq::utility::Properties::Ptr tprop = MMCSProperties::getProperties();
    client.initProperties(tprop);
    while(isThreadStopping() == false) {
        try {
            client.connectMaster(portpairs);
            LOG_DEBUG_MSG("Connected to bgmaster_server");
        } catch(CxxSockets::CxxError& e) {
            LOG_ERROR_MSG("Socket error detected: " << e.errcode << " " << e.what());

            // If we're spinning, do so slowly.
            sleep(1);
            continue;
        } catch (exceptions::CommunicationError& e) {
            LOG_ERROR_MSG("Communication error detected: " << e.errcode << " " << e.what());
            sleep(1);
            continue;
        }

        break; // We're connected and it is started, so we're done with this loop.
    }

    // At this point, the alias is known to be started.
    Monitor::_waitlist.setStat(_alias, WaiterList::RUNNING);

    bool abend = false;
    try {
        // So wait for it to end.
        if(client.wait_for_terminate(_bin_to_wait->get_binid()) < 0) {
            // Abnormally ended!  bgmaster_server might have gone
            // down.  We'll need to start waiting again.
            LOG_WARN_MSG("Wait for " << _alias << " ended prematurely."
                         << "  bgmaster_server may have gone down.  Will make hardware"
                         << " unavailable for new boots until bgmaster_server returns.");
            Monitor::_waitlist.setStat(_alias, WaiterList::ERROR);
            abend = true;
        }
    } catch(exceptions::BGMasterError& e) {
        if(e.errcode == exceptions::INFO) {
            // Log it, but consider it a failure and restart.
            LOG_DEBUG_MSG("Soft communications error " << e.what() << " detected.");
        }
        LOG_ERROR_MSG("BGmaster connection failed: " << e.what()
                      << " No longer monitoring for " << _alias << " termination.");
        // Set the alias to error...
        Monitor::_waitlist.setStat(_alias, WaiterList::ERROR);
        abend = true;
    }

    if(abend == false) {
        LOG_INFO_MSG("Terminated subnet mc detected");
        Monitor::_waitlist.setStat(_alias, WaiterList::STOPPED);
        Monitor::_waitlist.addBinId(_bin_to_wait->get_binid().str());
    }
    
    // We don't send a message to mc_server when it goes down, but we
    // do when it comes back up.

    // Find out which hardware this subnet is managing and put it
    // in an exclusion list.
    std::string hardware = MMCSProperties::getProperty((char*)(_alias.c_str()));
    LOG_INFO_MSG("Making " << hardware << " unavailable.");
    HardwareBlockList::add_to_list(hardware);

    LOG_INFO_MSG("Ending subnet_mc termination waiter for " << _alias);
    return 0;
}

} // master
} // mmcs
