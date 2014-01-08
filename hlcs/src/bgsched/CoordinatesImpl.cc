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
#include <bgsched/InputException.h>

#include "bgsched/CoordinatesImpl.h"
#include "bgsched/utility.h"

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>

using namespace std;

LOG_DECLARE_FILE("bgsched");

namespace bgsched {

Coordinates::Impl::Impl(
        const uint32_t A,
        const uint32_t B,
        const uint32_t C,
        const uint32_t D,
        const uint32_t E
)
{
    _coords[Dimension::A] = A;
    _coords[Dimension::B] = B;
    _coords[Dimension::C] = C;
    _coords[Dimension::D] = D;
    _coords[Dimension::E] = E;
    _size = Dimension::NodeDims;
}

Coordinates::Impl::Impl(
        const uint32_t A,
        const uint32_t B,
        const uint32_t C,
        const uint32_t D
)
{
    _coords[Dimension::A] = A;
    _coords[Dimension::B] = B;
    _coords[Dimension::C] = C;
    _coords[Dimension::D] = D;
    _coords[Dimension::E] = 0;
    _size = Dimension::MidplaneDims;
}

uint32_t&
Coordinates::Impl::operator[](
        Dimension::Value element
)
{
    if ((element < Dimension::A) || (element > boost::numeric_cast<int>(_size))) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidDimension,
                boost::lexical_cast<string>(element) + " is not a valid Dimension");
    } else {
        return _coords[element];
    }
}

uint32_t&
Coordinates::Impl::operator[](
        uint32_t element
)
{
    if (element > _size) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidDimension,
                boost::lexical_cast<string>(element) + " is not a valid Dimension");
    } else {
        return _coords[element];
    }
}

uint32_t
Coordinates::Impl::operator[](
        Dimension::Value element
) const
{
    if ((element < Dimension::A) || (element > boost::numeric_cast<int>(_size))) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidDimension,
                boost::lexical_cast<string>(element) + " is not a valid Dimension");
    } else {
        return _coords[element];
    }
}

uint32_t
Coordinates::Impl::operator[](
        uint32_t element
) const
{
    if (element > _size) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidDimension,
                boost::lexical_cast<string>(element) + " is not a valid Dimension");
    } else {
        return _coords[element];
    }
}

void
Coordinates::Impl::operator=(
        const Coordinates& coord
)
{
    for (Dimension i = Dimension::A; i < Dimension::NodeDims; ++i) {
        _coords[i] = coord[i];
    }
}

bool
Coordinates::Impl::operator==(
        const Coordinates& coord
) const
{
    for (Dimension i = Dimension::A; i < _size; ++i) {
        if (_coords[i] != coord[i]) {
            return false;
        }
    }
    return true;
}

bool
Coordinates::Impl::operator!=(
        const Coordinates& coord
) const
{
    for (Dimension i = Dimension::A; i < _size; ++i) {
        if (_coords[i] != coord[i]) {
            return true;
        }
    }
    return false;
}

bool
Coordinates::Impl::operator<=(
        const Coordinates& coord
) const
{
    uint32_t left = 0;
    uint32_t right = 0;
    if (_size == Dimension::MidplaneDims) {
        // Calculate for 4D coordinate
        left = (_coords[Dimension::A] * 1000) + (_coords[Dimension::B] * 100) +
        (_coords[Dimension::C] * 10) + _coords[Dimension::D] ;
        right = (coord[Dimension::A] * 1000) + (coord[Dimension::B] * 100) +
        (coord[Dimension::C] * 10) + coord[Dimension::D] ;
    } else {
        // Calculate for 5D coordinate
        left = (_coords[Dimension::A] * 10000) + (_coords[Dimension::B] * 1000) +
        (_coords[Dimension::C] * 100) + (_coords[Dimension::D] * 10) + _coords[Dimension::E] ;
        right = (coord[Dimension::A] * 10000) + (coord[Dimension::B] * 1000) +
        (coord[Dimension::C] * 100) + (coord[Dimension::D] * 10) + coord[Dimension::E] ;

    }
    if (left <= right) {
        return true;
    }
    return false;
}

bool
Coordinates::Impl::operator>=(
        const Coordinates& coord
) const
{
    uint32_t left = 0;
    uint32_t right = 0;
    if (_size == Dimension::MidplaneDims) {
        // Calculate for 4D coordinate
        left = (_coords[Dimension::A] * 1000) + (_coords[Dimension::B] * 100) +
        (_coords[Dimension::C] * 10) + _coords[Dimension::D] ;
        right = (coord[Dimension::A] * 1000) + (coord[Dimension::B] * 100) +
        (coord[Dimension::C] * 10) + coord[Dimension::D] ;
    } else {
        // Calculate for 5D coordinate
        left = (_coords[Dimension::A] * 10000) + (_coords[Dimension::B] * 1000) +
        (_coords[Dimension::C] * 100) + (_coords[Dimension::D] * 10) + _coords[Dimension::E] ;
        right = (coord[Dimension::A] * 10000) + (coord[Dimension::B] * 1000) +
        (coord[Dimension::C] * 100) + (coord[Dimension::D] * 10) + coord[Dimension::E] ;

    }
    if (left >= right) {
        return true;
    }
    return false;
}

} // namespace bgsched
