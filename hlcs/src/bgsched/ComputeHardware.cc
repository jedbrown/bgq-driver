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
#include <bgsched/ComputeHardware.h>
#include <bgsched/InputException.h>

#include "bgsched/ComputeHardwareImpl.h"
#include "bgsched/utility.h"

#include <string>

LOG_DECLARE_FILE("bgsched");

using namespace bgsched;
using namespace std;

namespace bgsched {

ComputeHardware::ComputeHardware(
        Pimpl impl
        ) :
    _impl(impl)
{

}

EnumWrapper<Hardware::State>
ComputeHardware::getState(
        const string& location
        ) const
{
    return _impl->getState(location);
}

uint32_t
ComputeHardware::getMachineSize(
        const Dimension dimension
        ) const
{
    return _impl->getMachineSize(dimension);
}

uint32_t
ComputeHardware::getMidplaneSize(
        const Dimension dimension
        ) const
{
    switch (dimension)
    {
      case Dimension::A:
      case Dimension::B:
      case Dimension::C:
      case Dimension::D:
          return 4;
      case Dimension::E:
          return 2;
      default:
          THROW_EXCEPTION(
                  bgsched::InputException,
                  bgsched::InputErrors::InvalidDimension,
                  "Dimension outside range boundary");
    }
}

Midplane::ConstPtr
ComputeHardware::getMidplane(
        const Coordinates& coords
        ) const
{
    try {
        Midplane::Pimpl impl = _impl->getMidplane( coords );
        Midplane::ConstPtr result(new Midplane(impl));
        return result;
    } catch (...) {
        // Rethrow any exceptions
        throw;
    }
}

Midplane::ConstPtr
ComputeHardware::getMidplane(
        const string& location
    ) const
{
    try {
        Midplane::Pimpl impl(_impl->getMidplane(location));
        Midplane::ConstPtr result(new Midplane(impl));
        return result;
    } catch (...) {
        // Rethrow any exceptions
        throw;
    }
}

uint32_t
ComputeHardware::getMachineRows() const
{
    return _impl->getMachineRows( );
}

uint32_t
ComputeHardware::getMachineColumns() const
{
    return _impl->getMachineColumns( );
}

} // namespace bgsched

