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

#include "Monitor.h"

#include "AliasWaiter.h"
#include "SubnetWaiter.h"
#include "WaiterList.h"

#include "../BlockControllerBase.h"

#include "common/Properties.h"

#include "libmmcs_client/CommandReply.h"

#include "master/lib/exceptions.h"

#include <control/include/mcServer/MCServerRef.h>

#include <utility/include/Log.h>

#include <utility/include/cxxsockets/exception.h>

#include <utility/include/portConfiguration/ClientPortConfiguration.h>

#include <boost/thread.hpp>

using mmcs::common::Properties;

LOG_DECLARE_FILE("mmcs.server");

namespace mmcs {
namespace server {
namespace master {

// Static object instantiations
WaiterList Monitor::_waitlist;
MCServerRefPtr Monitor::_server;
std::vector<std::string> Monitor::_bringup_options;
std::map<std::string,BinaryId> Monitor::_alias_binary_map;
boost::condition_variable Monitor::_startup_notifier;
boost::mutex Monitor::_startup_lock;
bool Monitor::_started = false;

bool
Monitor::connect_mc_server()
{
    mmcs_client::CommandReply reply;
    const std::string monstr = "master_monitor";
    MCServerRef* ref;
    BlockControllerBase::mcserver_connect(ref, monstr, reply);
    if (reply.getStatus() != 0) {
        return false;
    }
    _server = ref;
    return true;
}

// Start a waiter for an alias to STOP.
void
Monitor::start_terminate_waiter(
        const AliasWaiter* aw,
        const std::string& alias
        )
{
    if (_waitlist.getStat(alias) == WaiterList::RUNNING) {
        // Binary is running, so we can now wait for it to end.
        SubnetWaiter* const mw = new SubnetWaiter;
        const std::string null;
        const BinaryControllerPtr bptr(new BinaryController(aw->getBinId(), null, alias, null));
        mw->setBin(bptr);
        mw->setAlias(alias);
        mw->setJoinable(true);
        mw->setDeleteOnExit(true);
        mw->start();
    } else {
        // It's not running yet.
        LOG_DEBUG_MSG("Alias " << alias << " not yet running. " << _waitlist.getStat(alias));
        sleep(1);
    }
}

void*
Monitor::threadStart()
{
    // Need to wait on all SubnetMcs.  If any fail we let mc_server know.
    // So we'll get all of the running binaries and all of the configured subnet_mc aliases.
    // If any aliases aren't running, we'll wait for them to start. Concurrently,
    // we'll create waiters for each binary.

    // Initialize the wait list.
    _waitlist.init();

    // First get all stats
    bgq::utility::ClientPortConfiguration port_config(32042);
    port_config.setProperties(Properties::getProperties(), "master.client");
    port_config.notifyComplete();
    BGMasterClient client;
    const bgq::utility::PortConfiguration::Pairs portpairs = port_config.getPairs();
    if (portpairs[0].first.empty()) {
        LOG_FATAL_MSG("No port pairs or invalid port pairs specified.");
        exit(1);
    }

    LOG_INFO_MSG("Attempting to connect to bgmaster_server ...");
    while (isThreadStopping() == false) {
        try {
            client.connectMaster(Properties::getProperties(), portpairs);
            LOG_INFO_MSG("Connected to bgmaster_server successfully.");
            break;
        } catch (const CxxSockets::Error& e) {
            LOG_ERROR_MSG("Socket error detected: " << e.errcode << " " << e.what());
        } catch (const exceptions::CommunicationError& e) {
            LOG_ERROR_MSG("Communication error detected: " << e.errcode << " " << e.what());
        }
        sleep(1);
    }

    // Get all of the defined subnets
    std::vector<std::string> subnets;
    while (true) {
        const std::string subnet = "Name." + boost::lexical_cast<std::string>(subnets.size());
        const std::string subnet_name = Properties::getProperty(subnet);
        if (subnet_name.empty())
            break;
        LOG_DEBUG_MSG("Subnet " << subnet_name << " added to list");
        subnets.push_back(subnet_name);
    }
    LOG_INFO_MSG("Found " << subnets.size() << " subnets.");

    // iterate through list of subnets from bg.properties, starting a thread per subnet
    while ( !subnets.empty() && !isThreadStopping() ) {
        // get status from bgmaster_server
        typedef std::map<BinaryId, BinaryControllerPtr, Id::Comp> BinaryMap;
        BinaryMap mm;
        try {
            client.status(mm);
        } catch (const exceptions::BGMasterError& e) {
            LOG_ERROR_MSG("Unable to retrieve status from BGmaster.");
            sleep(5);
            continue;
        }

        for ( std::vector<std::string>::iterator i = subnets.begin(); i != subnets.end(); ) {
            // Find this subnet in the status returned from bgmaster_server
            const BinaryMap::const_iterator subnet = std::find_if(
                    mm.begin(),
                    mm.end(),
                    boost::bind(
                        std::equal_to<std::string>(),
                        *i,
                        boost::bind(
                            &BinaryController::get_alias_name,
                            boost::bind(
                                &BinaryMap::value_type::second,
                                _1
                                )
                            )
                        )
                    );
            if ( subnet != mm.end() ) {
                // Found a subnet.  First wait for it to start.
                const std::string alias = *i;
                AliasWaiter* aw = new AliasWaiter(alias);
                aw->start();
                while (!isThreadStopping()) {
                    aw->wait();
                    if (_waitlist.getStat(alias) == WaiterList::RUNNING) {
                        break;
                    } else if (_waitlist.getStat(alias) == WaiterList::ERROR) {
                        delete aw;
                        aw = new AliasWaiter(alias);
                        aw->start();
                    }
                }
                start_terminate_waiter(aw, alias);
                delete aw;
                i = subnets.erase(i);
            } else {
                LOG_INFO_MSG( "Did not find subnet " << *i);
                ++i;
            }
        }

        if ( subnets.empty() ) {
            break;
        }
        LOG_INFO_MSG( "Waiting for " << subnets.size() << " subnet" << (subnets.size() == 1 ? "" : "s") << " to start." );
        sleep(5);
    }

    // This is start point to wait for in mmcs_server
    _started = true;
    Monitor::_startup_notifier.notify_all();

    // Attempt to connect to mc_server
    while (!isThreadStopping() && !connect_mc_server()) {
        sleep(1);
    }

    // Now we just periodically check the waiter list to see if anybody is stopped.
    while (!isThreadStopping()) {
        sleep(1);
        std::string stopped_alias = _waitlist.getStopped();
        if (stopped_alias.empty()) {
            stopped_alias = _waitlist.getErrored();
        }
        if (!stopped_alias.empty()) {
            // At this point, we should be in either ERROR or STOPPED state, not WAIT_FOR_START or RUNNING.
            // Wait for it to start back up.
            AliasWaiter* aw = new AliasWaiter(stopped_alias, true);
            aw->start();
            aw->wait();
            // This joins the thread so that we can be assured
            // that we have a started subnet before we try to wait for it to end.
            // Once it starts, we'll do a termination waiter again.
            start_terminate_waiter(aw, stopped_alias);
            delete aw;
        }
    }

    return 0;
}

} } } // namespace mmcs::server::master
