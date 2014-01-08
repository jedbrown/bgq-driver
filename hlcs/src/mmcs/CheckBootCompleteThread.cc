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

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include <boost/scope_exit.hpp>
#include <algorithm>
#include <db/include/api/BGQDBlib.h>
#include "CheckBootCompleteThread.h"
#include "DBBlockController.h"
#include "BlockControllerBase.h"
#include "BlockHelper.h"
#include "MMCSProperties.h"
#include "DBStatics.h"

LOG_DECLARE_FILE("mmcs");

CheckBootComplete::CheckBootComplete(DBBlockController* b) : MMCSThread(), pBlock(b), _target(0) {}

PerformanceCounters::Timer::Ptr
CheckBootComplete::startCounter() {
    const BlockPtr block = pBlock->getBase();

    PerformanceCounters& counters = pBlock->counters();
    const PerformanceCounters::Timer::Ptr result = counters.create()
        ->id( block->_blockName )
        ->mode( bgq::utility::performance::Mode::Value::Basic )
        ->function( "boot" )
        ->subFunction( "verify kernel ready" )
        ;

    return result;
}

void CheckBootComplete::markNodes(BlockPtr pBlock, MMCSCommandReply& reply,
                                  std::vector<std::string>& bad_node_locs,
                                  std::vector<std::string>& good_locs) {
    LOG_INFO_MSG(__FUNCTION__);
    if(!pBlock->isIOBlock()) return;
    DBStatics::setLocationStatus(good_locs, reply, DBStatics::AVAILABLE, DBStatics::ION);
}

void* CheckBootComplete::threadStart() {
    void* returnp = 0;
    const BlockPtr block_p = pBlock->getBase();
    bool& rebooting = block_p->_rebooting;
    boost::mutex& ifl = block_p->_init_free_lock;
    bool i_locked_it = false;

    BOOST_SCOPE_EXIT( ( &rebooting ) ) {
        rebooting = false;
    } BOOST_SCOPE_EXIT_END;

    // This ensures that if we lock the init/free lock, we let it go.
    // We can't use a scope lock because we don't want to lock out 
    // the entire boot timeout, just the block initialization and
    // target reconnect.
    BOOST_SCOPE_EXIT( ( &ifl) ( &i_locked_it ) ) {
        if(i_locked_it)
            ifl.unlock();
    } BOOST_SCOPE_EXIT_END;

    // start measuring how long this operation takes, we dismiss the counter
    // until a success is encountered
    PerformanceCounters::Timer::Ptr counter = this->startCounter();
    counter->dismiss();

    std::vector<std::string> bad_node_locs, good_nodes;
    log4cxx::MDC _blockid_mdc_( "blockId", std::string("{") + block_p->_blockName + "} " );
    log4cxx::MDC _block_user_mdc_( "user", std::string("[") + block_p->_userName + "] " );
    LOG_INFO_MSG("Starting check complete thread");
    MMCSCommandReply reply;
    unsigned nodecount = 0;

    // Build a boot check message
    MCServerMessageSpec::VerifyKernelReadyRequest bootreq;
    if(_target == 0) {
        for(unsigned int i = 0; i < block_p->getNodes().size(); ++i) {
            BCNodeInfo* ni = block_p->getNodes()[i];
            if(ni->_linkio == false) {
                bootreq._location.push_back(ni->_location);
                ++nodecount;
            }
        }
    } else {
        BOOST_FOREACH(BCNodeInfo* node, _target->getNodes()) {
            bootreq._location.push_back(node->_location);
            ++nodecount;
        }
    }

    std::ostringstream msg;
    bootreq.write(msg);
    LOG_TRACE_MSG(msg.str());
    bool initialized = false;

    while(isThreadStopping() == false) {
        if(block_p->checkComplete(reply, bad_node_locs, good_nodes, bootreq, nodecount) == true) {
            // check boot completion status
            if (block_p->isDisconnecting()) {
                // this can occur for a KERNEL_FATAL ras event or
                // deallocate by another console thread
                reply << FAIL << block_p->disconnectReason() << DONE;
                return returnp;
            }
            BGQDB::BlockInfo bInfo;
            int result = BGQDB::getBlockInfo(block_p->_blockName, bInfo);
            if ((result == BGQDB::OK) &&
                strcmp(bInfo.status, BGQDB::BLOCK_DEALLOCATING) == 0) {
                // this can occur if bgqblock record status is set to 'D' directly
                std::string blockErrorText;
                result = BGQDB::getBlockErrorText(block_p->_blockName, blockErrorText);
                if (result != BGQDB::OK || blockErrorText[0] == '\0')
                    block_p->setDisconnecting(true, "block has been freed by RAS event");
                else
                    block_p->setDisconnecting(true, blockErrorText);
                reply << FAIL << block_p->disconnectReason() << DONE;
                return returnp;
            }

            // Once this lock is acquired, a free cannot be started.  If one is already
            // in progress, when the free completes, we get the lock and the block state
            // change will fail.  This way, we will not attempt to reconnect targets
            // leaving a block in a bad state.
            block_p->_init_free_lock.lock();
            i_locked_it = true;
            markNodes(block_p, reply, bad_node_locs, good_nodes);
            // If we were rebooting, note that we are now done.
            if(block_p->getRebooting()) block_p->_rebooted = true;
            if(strcmp(bInfo.status, BGQDB::BLOCK_INITIALIZED) != 0) {
                if(pBlock->setBlockStatus(block_p->_blockName, BGQDB::INITIALIZED) != BGQDB::OK) {
                    // If we can't set the state to INITIALIZED, it's because another thread
                    // has done something to it.  Usually, that means there's been a free.
                    LOG_WARN_MSG("setBlockStatus(INITIALIZED) failed.  "  << block_p->disconnectReason());
                    reply << FAIL << block_p->disconnectReason() << DONE;
                    return returnp;
                } else 
                    LOG_INFO_MSG("setBlockStatus(INITIALIZED) " << block_p->_blockName << " succeeded.");
            }

            // success, stop and un-dismiss the counter
            counter->stop();
            counter->dismiss( false );

            LOG_INFO_MSG("block initialization successful");
            initialized = true;
            // store counter and output all counters associated with this boot
            counter.reset();
            pBlock->counters().output( block_p->_savedBootCookie );
            LOG_DEBUG_MSG(block_p->_total_barrier_duration << " usecs to do barriers");
            reply << OK << DONE;
            break;
        }
        sleep(1);
    }

    if(bad_node_locs.size() < nodecount)
        block_p->_block_shut_down = false; // At least some nodes are running.

    if(block_p->isIOBlock() &&
       MMCSProperties::getProperty(FREE_IO_TARGETS) == "true") {
        bool redirecting = false;
        if(block_p->_redirectSock != 0) {
            redirecting = true;
        }
        // Free the targets.
        std::deque<std::string> args;
        args.push_back("no_shutdown");
        LOG_INFO_MSG("Freeing IO targets");
        block_p->disconnect(args);
        if(redirecting && initialized) {
            // Redirecting?  Reopen the targets as RAAW.
            LOG_INFO_MSG("Reopening targets in monitor mode for RAAW locks.");
            std::deque<std::string> a;
            a.push_back("mode=monitor");
            std::string targspec = "{*}";
            BlockControllerTarget* target = new BlockControllerTarget(block_p, targspec, reply);
            block_p->connect(a, reply, target);
        }
    }
    LOG_INFO_MSG("Check Boot Complete thread done");
    return returnp;
}
