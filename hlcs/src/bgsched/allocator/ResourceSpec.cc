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

#include "bgsched/allocator/ResourceSpecImpl.h"

using namespace bgsched;
using namespace bgsched::allocator;
using namespace std;

namespace bgsched {
namespace allocator {

// Constructor based on a Shape
ResourceSpec::ResourceSpec(
        const bgsched::Shape& shape,
        bool  canRotateShape,
        bool  canUsePassthrough
        ):
    _impl(new ResourceSpec::Impl(shape, canRotateShape, canUsePassthrough))
{
    // Nothing to do
}

// Constructor from Pimpl
ResourceSpec::ResourceSpec(
        Pimpl impl
        ) :
    _impl(impl)
{
    // Nothing to do
}

// Copy constructor
ResourceSpec::ResourceSpec(
        const ResourceSpec& resourceSpec
        ) :
    _impl(new ResourceSpec::Impl(*resourceSpec._impl))
{
    // Nothing to do
}

Shape::ConstPtr
ResourceSpec::getShape() const
{
    return _impl->getShape();
}

bool
ResourceSpec::canRotateShape() const
{
    return _impl->canRotateShape();
}

bool
ResourceSpec::canUsePassthrough() const
{
    return _impl->canUsePassthrough();
}

uint32_t
ResourceSpec::getNodeCount() const
{
    return _impl->getNodeCount();
}

ResourceSpec::ConnectivitySpec::Value
ResourceSpec::getConnectivitySpec(
        const Dimension& dimension
        ) const
{
    return _impl->getConnectivitySpec(dimension);
}

void
ResourceSpec::setConnectivitySpec(
         const Dimension& dimension,
         ResourceSpec::ConnectivitySpec::Value connectivitySpec
         )
{
    _impl->setConnectivitySpec(dimension, connectivitySpec);
}

ResourceSpec::DrainedMidplanes
ResourceSpec::getDrainedMidplanes() const
{
    return _impl->getDrainedMidplanes();
}

void
ResourceSpec::addDrainedMidplane(
        const string& midplaneLocation
        )
{
    _impl->addDrainedMidplane(midplaneLocation);
}

void
ResourceSpec::removeDrainedMidplane(
        const string& midplaneLocation
        )
{
    _impl->removeDrainedMidplane(midplaneLocation);
}

ResourceSpec::ExtendedOptions
ResourceSpec::getExtendedOptions() const
{
    return _impl->getExtendedOptions();
}

void
ResourceSpec::addExtendedOption(
        const string& key,
        const string& value
        )
{
    _impl->addExtendedOption(key, value);
}

void
ResourceSpec::removeExtendedOption(
        const string& key
        )
{
    _impl->removeExtendedOption(key);
}

//-----------------------------------------------
// class ConnectivitySpec

const string&
ResourceSpec::ConnectivitySpec::toString(
        Value connectivity
        )
{
    static const string strings[] = {
        "Torus",
        "Mesh",
        "Either"
    };

    static const string InvalidStr("INVALID");

    if (connectivity <= ResourceSpec::ConnectivitySpec::Either ) {
        return strings[connectivity];
    }

    return InvalidStr;
}

} // namespace bgsched::allocator
} // namespace bgsched

