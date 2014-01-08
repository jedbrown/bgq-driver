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

#include <bgsched/allocator/LiveModel.h>

#include "bgsched/allocator/LiveModelImpl.h"

#include <bgsched/Block.h>
#include <bgsched/Coordinates.h>
#include <bgsched/InputException.h>

#include "bgsched/BlockImpl.h"
#include "bgsched/utility.h"

#include <utility/include/Log.h>

#include <boost/thread/locks.hpp>

using namespace bgsched;
using namespace bgsched::allocator;
using namespace bgq::util;
using namespace std;

LOG_DECLARE_FILE("bgsched");

namespace bgsched {
namespace allocator {

LiveModel::LiveModel(
        Pimpl impl
        ) :
    Model(impl)
{
    // Nothing to do
}

void
LiveModel::allocate(
        const string& blockName
        )
{
    _impl->allocate(blockName);
}

void
LiveModel::deallocate(
        const string& blockName
        )
{
    _impl->deallocate(blockName);
}

const Block::Ptr
LiveModel::getBlock(
        const string& id
        ) const
{
    Block::Pimpl pimpl = _impl->getBlock(id);
    return Block::Ptr(new Block(pimpl));
}

EnumWrapper<Block::Status>
LiveModel::getBlockStatus(
        const string& blockName
        ) const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getBlockStatus(blockName);
}

Block::Ptrs
LiveModel::getFilteredBlocks(
        const BlockFilter& filter
        ) const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getFilteredBlocks(filter);
}

void
LiveModel::addBlock(
        const Block::Ptr block,
        const string& owner
        )
{
    // Verify we got a valid block pointer
    if (!block) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::BlockNotAdded,
                "Block parameter is empty, block not added."
        );
    }
    Block::Pimpl pimpl = block->getPimpl();
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    impl->addBlock(pimpl,owner);
}

void
LiveModel::removeBlock(
        const string& blockName
        )
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    impl->removeBlock(blockName);
}

void
LiveModel::syncState( )
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    impl->syncState();
}

void
LiveModel::dump(
        ostream& os,
        EnumWrapper<Model::DumpVerbosity::Value> verbosity
        ) const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    impl->dump(os, verbosity);
}

void
LiveModel::registerListener(
        AllocatorEventListener& listener
        )
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    impl->registerListener(listener);
}

void
LiveModel::notifyAllocated(
        const string& blockName
        )
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    impl->notifyAllocated(blockName);
}

void
LiveModel::notifyDeallocated(
        const string& blockName
        )
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    impl->notifyDeallocated(blockName);
}

Midplanes::ConstPtr
LiveModel::getMidplanes() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getMidplanes();
}

Midplane::Ptr
LiveModel::getMidplane(
        const Coordinates& coordinates
        ) const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getMidplane(coordinates);
}

Midplane::Ptr
LiveModel::getMidplane(
        const string& location
        ) const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getMidplane(location);
}

Model::DrainedMidplanes
LiveModel::getDrainedMidplanes() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getDrainedMidplanes();
}

void
LiveModel::addDrainedMidplane(
        const string& midplaneLocation
        )
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    impl->addDrainedMidplane(midplaneLocation);
}

void
LiveModel::removeDrainedMidplane(
        const string& midplaneLocation
        )
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    impl->removeDrainedMidplane(midplaneLocation);
}

void
LiveModel::monitorBlockAllocate(
        const string& blockName
        )
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->monitorBlockAllocate(blockName);
}

void
LiveModel::monitorBlockDeallocate(
        const string& blockName
        )
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->monitorBlockDeallocate(blockName);
}

} // namespace bgsched::allocator
} // namespace bgsched
