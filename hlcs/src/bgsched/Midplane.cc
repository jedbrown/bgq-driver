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

#include <bgsched/Coordinates.h>
#include <bgsched/Midplane.h>

#include "bgsched/MidplaneImpl.h"

using namespace bgsched;
using namespace std;

namespace bgsched {

Midplane::Midplane(
        Pimpl impl
        ) :
    Hardware(impl)
{
  // Nothing to do.
}

Midplane::Pimpl
Midplane::getPimpl()
{
    return boost::dynamic_pointer_cast<Impl>(_impl);
}

SequenceId
Midplane::getSequenceId() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getSequenceId();
}

uint32_t
Midplane::getComputeNodeMemory() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getComputeNodeMemory();
}

uint32_t
Midplane::getAvailableNodeBoardCount() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getAvailableNodeBoardCount();
}

NodeBoard::ConstPtr
Midplane::getNodeBoard(
        const uint32_t number
        ) const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    NodeBoard::Pimpl nodeBoard = impl->getNodeBoard(number);
    NodeBoard::ConstPtr result(new NodeBoard(nodeBoard));
    return result;
}

Switch::ConstPtr
Midplane::getSwitch(
        const Dimension& dimension
        ) const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    Switch::Pimpl switchPimpl = impl->getSwitch(dimension);
    Switch::ConstPtr result(new Switch(switchPimpl));
    return result;
}

const Coordinates&
Midplane::getCoordinates() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getCoordinates();
}

EnumWrapper<Midplane::InUse>
Midplane::getInUse() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getInUse();
}

const string&
Midplane::getComputeBlockName() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getComputeBlockName();
}

} // namespace bgsched

