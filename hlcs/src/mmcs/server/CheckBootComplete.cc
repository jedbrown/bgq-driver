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

#include "CheckBootComplete.h"

#include "BCNodeInfo.h"
#include "BlockControllerBase.h"
#include "BlockControllerTarget.h"
#include "DBBlockController.h"
#include "DBStatics.h"

#include "common/Properties.h"

#include <utility/include/Log.h>

LOG_DECLARE_FILE("mmcs.server");

namespace mmcs {
namespace server {

CheckBootComplete::CheckBootComplete(
        DBBlockController* b
        ) :
    Thread(),
    _block(b),
    _target(0)
{
    // Need to join this thread
    this->setJoinable( true );
    this->setDeleteOnExit( false );
}

PerformanceCounters::Timer::Ptr
CheckBootComplete::startCounter()
{
    const BlockPtr block = _block->getBase();

    PerformanceCounters& counters = _block->counters();
    const PerformanceCounters::Timer::Ptr result = counters.create()
        ->id( block->getBlockName() )
        ->mode( bgq::utility::performance::Mode::Value::Basic )
        ->function( "boot" )
        ->subFunction( "verify kernel ready" )
        ;

    return result;
}

void
CheckBootComplete::markNodes(
        const std::set<std::string>& good_locs
        )
{
    if (!_block->getBase()->isIoBlock()) {
        return;
    }

    std::vector<std::string> good( good_locs.begin(), good_locs.end() );

    DBStatics::setLocationStatus(good, DBStatics::AVAILABLE, bgq::util::Location::ComputeCardOnIoBoard);
}

void*
CheckBootComplete::threadStart()
{
    void* returnp = 0;
    const BlockPtr block_p = _block->getBase();

    // Start measuring how long this operation takes, we dismiss the counter
    // until a success is encountered
    PerformanceCounters::Timer::Ptr counter = this->startCounter();
    counter->dismiss();

    std::vector<std::string> bad_node_locs;
    std::set<std::string> good_nodes;
    log4cxx::MDC _blockid_mdc_( "blockId", std::string("{") + _block->getBlockName() + "} " );
    log4cxx::MDC _block_user_mdc_( "user", std::string("[") + block_p->getUserName() + "] " );
    unsigned nodecount = 0;

    // Build a boot check message
    MCServerMessageSpec::VerifyKernelReadyRequest bootreq;
    if (_target == 0) {
        for (unsigned int i = 0; i < block_p->getNodes().size(); ++i) {
            BCNodeInfo* ni = block_p->getNodes()[i];
            if (ni->_linkio == false) {
                bootreq._location.push_back(ni->_location);
                ++nodecount;
            }
        }
    } else {
        BOOST_FOREACH(const BCNodeInfo* node, _target->getNodes()) {
            bootreq._location.push_back(node->_location);
            ++nodecount;
        }
    }

    while (isThreadStopping() == false) {
        if (block_p->checkComplete(bad_node_locs, good_nodes, bootreq)) {
            // Check boot completion status
            if (block_p->isDisconnecting()) {
                // This can occur for a KERNEL_FATAL RAS event or deallocate by another console thread
                LOG_WARN_MSG( block_p->disconnectReason() );
                return returnp;
            }

            if ( block_p->_rebooting ) {
                markNodes(good_nodes);
            } else {
                counter->dismiss( false );
            }

            // Success, stop and un-dismiss the counter
            counter->stop();

            // Store counter and output all counters associated with this boot
            counter.reset();
            _block->counters().output( block_p->_bootCookie );
            break;
        }

        sleep(1);
    }

    if (bad_node_locs.size() < nodecount) {
        block_p->_block_shut_down = false; // At least some nodes are running.
    }

    if (block_p->isIoBlock() && common::Properties::getProperty(FREE_IO_TARGETS) == "true") {
        const bool redirecting = block_p->_redirectSock;
        // Free the targets.
        const std::deque<std::string> args( 1, "no_shutdown");
        LOG_TRACE_MSG("Freeing I/O targets for I/O block " << _block->getBlockName());
        mmcs_client::CommandReply reply;
        block_p->disconnect(args, reply);
        if (redirecting) {
            // Redirecting?  Reopen the targets as RAAW.
            LOG_INFO_MSG("Reopening targets in monitor mode for RAAW locks for I/O block " << _block->getBlockName());
            const std::deque<std::string> args( 1, "mode=monitor" );
            const BlockControllerTarget target(block_p, "{*}", reply);
            block_p->connect(args, reply, &target);
        }
    }

    if ( _block->setBlockStatus(BGQDB::INITIALIZED) != BGQDB::OK) {
        // If we can't set the state to INITIALIZED, it's because another thread
        // has done something to it.  Usually, that means there's been a free.
        LOG_WARN_MSG("Setting block status to INITIALIZED failed for block " << _block->getBlockName() << ". Reason: " << block_p->disconnectReason());
        return returnp;
    }

    return returnp;
}

} } // namespace mmcs::server
