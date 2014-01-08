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

#include <bgsched/allocator/InputException.h>

#include "bgsched/allocator/MidplanesImpl.h"

#include <bgsched/Coordinates.h>
#include "bgsched/utility.h"

#include <utility/include/Log.h>

using namespace bgsched;
using namespace bgsched::allocator;

namespace {
    // Message strings
    const std::string InvalidMidplaneCoordinates_Str("Invalid midplane coordinates.");
} // anonymous namespace

LOG_DECLARE_FILE("bgsched");

namespace bgsched {
namespace allocator {

Midplanes::Impl::Impl(
        const ComputeHardware::Pimpl hardware
        ) :
    _midplanes(hardware->getMidplanes()),
    _coordinates(0,0,0,0)
{
    _coordinates[Dimension::A]= hardware->getMachineSize(Dimension::A);
    _coordinates[Dimension::B]= hardware->getMachineSize(Dimension::B);
    _coordinates[Dimension::C]= hardware->getMachineSize(Dimension::C);
    _coordinates[Dimension::D]= hardware->getMachineSize(Dimension::D);
}

const Coordinates
Midplanes::Impl::getMachineSize() const
{
    return _coordinates;
}

Midplane::ConstPtr
Midplanes::Impl::get(
        const Coordinates& coords
        ) const
{
    Coordinates machineSize = getMachineSize();
    for ( Dimension dimension = Dimension::A; dimension <= Dimension::D; ++dimension ) {
        if ( coords[dimension] > machineSize[dimension] ) {
            THROW_EXCEPTION(
                    bgsched::allocator::InputException,
                    bgsched::allocator::InputErrors::InvalidMidplaneCoordinates,
                    InvalidMidplaneCoordinates_Str
                    );
        }
    }
    Midplane::Pimpl impl = _midplanes [coords[Dimension::A]] [coords[Dimension::B]] [coords[Dimension::C]] [coords[Dimension::D]];
    Midplane::ConstPtr result(new Midplane(impl));
    return result;
}

} // namespace bgsched::allocator
} // namespace bgsched



