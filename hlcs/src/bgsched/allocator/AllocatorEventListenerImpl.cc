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

#include "bgsched/allocator/AllocatorEventListenerImpl.h"

#include "bgsched/BlockImpl.h"

#include <utility/include/Log.h>

LOG_DECLARE_FILE("bgsched");

using namespace std;

namespace bgsched {
namespace allocator {

AllocatorEventListener::BlockEvent::Impl::Impl(
        const realtime::ClientEventListener::BlockStateChangedEventInfo::Impl& chg,
        const std::string& error_msg
    ) :
        _block_name(chg.getBlockName()),
        _status(chg.getStatus()),
        _seq_id(chg.getSequenceId()),
        _successful(false),
        _error_msg(error_msg)
{
    // Nothing to do.
}

const string&
AllocatorEventListener::BlockEvent::Impl::getBlockName() const
{
    return _block_name;
}

Block::Status
AllocatorEventListener::BlockEvent::Impl::getStatus() const
{
    return _status;
}

SequenceId
AllocatorEventListener::BlockEvent::Impl::getSequenceId() const
{
    return _seq_id;
}

bool
AllocatorEventListener::BlockEvent::Impl::successful() const
{
    return _successful;
}

const string&
AllocatorEventListener::BlockEvent::Impl::getErrorMessage() const
{
    return _error_msg;
}

AllocatorEventListener::AllocateCompleted::Impl::Impl(
        const realtime::ClientEventListener::BlockStateChangedEventInfo::Impl& chg,
        const std::string& error_msg
    ) :
        BlockEvent::Impl(chg, error_msg)
{
   if (chg.getStatus() == Block::Initialized) {
       //LOG_INFO_MSG("Setting allocate complete to success for block " << chg.getBlockName());
      _successful = true;
   } else {
      //LOG_INFO_MSG("Setting allocate complete to failure for block " << chg.getBlockName());
      _successful = false;
   }
}

AllocatorEventListener::DeallocateCompleted::Impl::Impl(
        const realtime::ClientEventListener::BlockStateChangedEventInfo::Impl& chg,
        const std::string& error_msg
    ) :
        BlockEvent::Impl(chg, error_msg)
{
    if (chg.getStatus() == Block::Free) {
        //LOG_INFO_MSG("Setting deallocate complete to success for block " << chg.getBlockName());
       _successful = true;
    } else {
        //LOG_INFO_MSG("Setting deallocate complete to failure for block " << chg.getBlockName());
       _successful = false;
    }
}

} // namespace bgsched::allocator
} // namespace bgsched
