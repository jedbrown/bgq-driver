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

#include <bgsched/Dimension.h>
#include <bgsched/Shape.h>

#include "bgsched/ShapeImpl.h"

using namespace bgsched;

namespace bgsched {

Shape::Shape(
        Pimpl impl
        ) :
    _impl(impl)
{

}

Shape::Pimpl
Shape::getPimpl() const
{
    return _impl;
}

Shape::Shape(
        const Shape& shape
        ) :
    _impl(new Shape::Impl(*shape._impl))
{

}

uint32_t
Shape::getMidplaneSize(
        const Dimension& dimension
        ) const
{
    return _impl->getMidplaneSize(dimension);
}

uint32_t
Shape::getMidplaneCount() const
{
    return _impl->getMidplaneCount();
}

uint32_t
Shape::getNodeSize(
        const Dimension& dimension
        ) const
{
    return _impl->getNodeSize(dimension);
}

uint32_t
Shape::getNodeCount() const
{
    return _impl->getNodeCount();
}

bool
Shape::isSmall() const
{
    return _impl->isSmall();
}

bool
Shape::isLarge() const
{
    return _impl->isLarge();
}

bool
Shape::isFull() const
{
    return _impl->isFull();
}

bool
Shape::operator ==(
        const Shape& other
        ) const
{
    for (Dimension i = Dimension::A; i != Dimension(); ++i) {
        if (getNodeSize(i) != other.getNodeSize(i)) {
            return false;
        }
    }
    return true;
}

bool
Shape::operator !=(
        const Shape& other
        ) const
{
    for (Dimension i = Dimension::A; i != Dimension(); ++i) {
        if (getNodeSize(i) != other.getNodeSize(i)) {
            return true;
        }
    }
    return false;
}

} // namespace bgsched
