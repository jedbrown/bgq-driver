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

#include <bgsched/NodeBoard.h>

#include "bgsched/NodeBoardImpl.h"

using namespace std;

namespace bgsched {

NodeBoard::NodeBoard(
        Pimpl impl
        ) :
    Hardware(impl)
{
    // Nothing to do
}

bool
NodeBoard::isInUse() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->isInUse();
}

const string&
NodeBoard::getComputeBlockName() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getComputeBlockName();
}

SequenceId
NodeBoard::getSequenceId() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getSequenceId();
}

EnumWrapper<NodeBoard::Quadrant>
NodeBoard::getQuadrant() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getQuadrant();
}

const Coordinates
NodeBoard::getMidplaneCoordinates() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getMidplaneCoordinates();
}

const string
NodeBoard::getMidplaneLocation() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getMidplaneLocation();
}

uint32_t
NodeBoard::getAvailableNodeCount() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getAvailableNodeCount();
}

bool
NodeBoard::isMetaState() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->isMetaState();
}

} // namespace bgsched
