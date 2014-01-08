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
/* (C) Copyright IBM Corp.  2011, 2011                              */
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
#include "server/job/SubNodePacing.h"

#include "server/block/IoNode.h"

#include "server/cios/Connection.h"
#include "server/cios/Message.h"

#include "server/job/IoNode.h"

#include "server/Job.h"

#include "common/logging.h"
#include "common/SubBlock.h"

#include <boost/assert.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {

SubNodePacing::SubNodePacing() :
    _mutex( ),
    _outbox( )
{

}

void
SubNodePacing::add(
        const cios::Message::Ptr& message,
        const Job::Ptr& job
        )
{
    BOOST_ASSERT(
            // validate this is a sub-node job
            job->info().getSubBlock().corner().isCoreSet()
            );

    BOOST_ASSERT(
            // validate this is one of the messages that needs to be paced
            message->header()->type == bgcios::jobctl::SetupJob ||
            message->header()->type == bgcios::jobctl::LoadJob ||
            message->header()->type == bgcios::jobctl::StartJob ||
            message->header()->type == bgcios::jobctl::CleanupJob ||
            message->header()->type == bgcios::jobctl::SignalJob
            );

    boost::mutex::scoped_lock lock( _mutex );

    _outbox.push_back(
            std::make_pair(job, message)
            );
    LOG_TRACE_MSG( "added to queue size: " << _outbox.size() );

    if ( _outbox.size() == 1 ) {
        this->write();
    }
}

void
SubNodePacing::next()
{
    boost::mutex::scoped_lock lock( _mutex );

    if ( _outbox.empty() ) {
        LOG_WARN_MSG( "outbox is empty" );
        return;
    }

    // remove the front of the queue and send the next message
    _outbox.pop_front();
    LOG_TRACE_MSG( "removed from queue size: " << _outbox.size() );
    if ( _outbox.empty() ) return;
    this->write();
}

void
SubNodePacing::write()
{
    const Entry& entry = _outbox.front();
    const Job::Ptr& job = entry.first.lock();
    if ( !job ) return;

    IoNode& io = job->io().begin()->second;
    const cios::Message::Ptr& message = entry.second;

    // only jobctl messages need to be paced
    io.writeControl( message );
}

} // job
} // server
} // runjob
