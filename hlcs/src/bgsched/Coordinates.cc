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

#include "bgsched/CoordinatesImpl.h"

namespace bgsched {

Coordinates::Coordinates(
        const uint32_t A,
        const uint32_t B,
        const uint32_t C,
        const uint32_t D,
        const uint32_t E
) :
    _impl(new Impl(A, B, C, D, E))
{
}

Coordinates::Coordinates(
        const uint32_t A,
        const uint32_t B,
        const uint32_t C,
        const uint32_t D
) :
    _impl(new Impl(A, B, C, D))
{
}

uint32_t&
Coordinates::operator[](
        Dimension::Value element
)
{
    return _impl->operator[](element);
}

uint32_t&
Coordinates::operator[](
        uint32_t element
)
{
    return _impl->operator[](element);
}

uint32_t
Coordinates::operator[](
        Dimension::Value element
) const
{
    return _impl->operator[](element);
}

uint32_t
Coordinates::operator[](
        uint32_t element
) const
{
    return _impl->operator[](element);
}

Coordinates&
Coordinates::operator=(
        const Coordinates& coord
)
{
    if (this != &coord) // Protect against invalid self-assignment
    {
        _impl->operator=(coord);
    }
    return *this;
}

bool
Coordinates::operator==(
        const Coordinates& coord
) const
{
    return _impl->operator==(coord);
}

bool
Coordinates::operator!=(
        const Coordinates& coord
) const
{
    return _impl->operator!=(coord);
}

bool
Coordinates::operator<=(
        const Coordinates& coord
) const
{
    return _impl->operator<=(coord);
}

bool
Coordinates::operator>=(
        const Coordinates& coord
) const
{
    return _impl->operator>=(coord);
}

const uint32_t*
Coordinates::data() const
{
    return _impl->data();
}

} // namespace bgsched
