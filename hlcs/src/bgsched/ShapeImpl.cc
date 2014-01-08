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
#include <bgsched/Dimension.h>
#include <bgsched/InputException.h>
#include <bgsched/InternalException.h>
#include <bgsched/Midplane.h>

#include "bgsched/ShapeImpl.h"
#include "bgsched/utility.h"

#include <bgsched/core/core.h>

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>

#include <cmath>
#include <string>

using namespace bgsched;
using namespace std;

LOG_DECLARE_FILE("bgsched");

namespace {
    // Message strings
    const string DB_Access_Error_Str("Error occurred while accessing database.");
} // anonymous namespace

const Shape::Impl Shape::Impl::NodeBoard(2,2,2,2,2);
const Shape::Impl Shape::Impl::Eighth(2,2,4,2,2);
const Shape::Impl Shape::Impl::Quarter(2,4,4,2,2);
const Shape::Impl Shape::Impl::Half(4,4,4,2,2);
const Shape::Impl Shape::Impl::Midplane(4,4,4,4,2);

namespace bgsched {

Shape::Impl::Impl(
        const uint32_t a,
        const uint32_t b,
        const uint32_t c,
        const uint32_t d,
        const uint32_t e
        )
{
    initialize(a, b, c, d, e);
}

Shape::Impl::Impl(
        const uint32_t a,
        const uint32_t b,
        const uint32_t c,
        const uint32_t d
        )
{
    initMidplanes(a, b, c, d);
}

Shape::Impl::Impl(
        const uint32_t size[Dimension::MidplaneDims]
       )
{
    initMidplanes(size[Dimension::A], size[Dimension::B], size[Dimension::C], size[Dimension::D]);
}

Shape::Impl::Impl(
        const Shape::Impl& impl
        )
{
    initialize(impl._dims[Dimension::A], impl._dims[Dimension::B], impl._dims[Dimension::C], impl._dims[Dimension::D], impl._dims[Dimension::E]);
}

void
Shape::Impl::initialize(
        const uint32_t a,
        const uint32_t b,
        const uint32_t c,
        const uint32_t d,
        const uint32_t e
        )
{
    _dims[Dimension::A] = a;
    _dims[Dimension::B] = b;
    _dims[Dimension::C] = c;
    _dims[Dimension::D] = d;
    _dims[Dimension::E] = e;
    setMidplaneDimensions();
    setSize();
}


uint32_t
Shape::Impl::getMidplaneSize(
        const Dimension dimension
        ) const
{
    if (dimension > Dimension::D) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidDimension,
                "Dimension exceeds range boundary");
    } else {
        return _midplaneDims.at(dimension);
    }
}

uint32_t
Shape::Impl::getNodeSize(
        const Dimension dimension
        ) const
{
    if (dimension > Dimension::E) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidDimension,
                "Dimension exceeds range boundary");
    } else {
        return _dims.at(dimension);
    }
}

uint32_t
Shape::Impl::getNodeCount() const
{
    return _size;
}

bool
Shape::Impl::isSmall() const
{
    return getNodeCount() < Midplane::NodeCount;
}

bool
Shape::Impl::isLarge() const
{
    return getNodeCount() >= Midplane::NodeCount;
}

bool
Shape::Impl::isFull() const
{
    Coordinates machineSize(0,0,0,0);
    // Get the size of the system in each dimension
    try {
        machineSize = core::getMachineSize();
    } catch (...) {
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::UnexpectedError,
                DB_Access_Error_Str
        );
    }
    if ((machineSize[Dimension::A] == _midplaneDims[Dimension::A]) &&
        (machineSize[Dimension::B] == _midplaneDims[Dimension::B]) &&
        (machineSize[Dimension::C] == _midplaneDims[Dimension::C]) &&
        (machineSize[Dimension::D] == _midplaneDims[Dimension::D])) {
        return true;
    } else {
        return false;
    }
}

uint32_t
Shape::Impl::getMidplaneCount() const

{
    // Return 1 midplane if small block
    if (isSmall()) {
        return 1;
    } else {
        // Must be large block so divide compute nodes by midplane compute nodes (512) to get midplane count
        return _size/Midplane::NodeCount;
    }
}

void
Shape::Impl::initMidplanes(
        const uint32_t a,
        const uint32_t b,
        const uint32_t c,
        const uint32_t d
        )
{
    _dims[Dimension::A] = a*4;
    _dims[Dimension::B] = b*4;
    _dims[Dimension::C] = c*4;
    _dims[Dimension::D] = d*4;
    _dims[Dimension::E] = 2;
    _midplaneDims[Dimension::A] = a;
    _midplaneDims[Dimension::B] = b;
    _midplaneDims[Dimension::C] = c;
    _midplaneDims[Dimension::D] = d;

    setSize();
}

void
Shape::Impl::setSize()
{
    _size = _dims[Dimension::A] * _dims[Dimension::B] * _dims[Dimension::C] * _dims[Dimension::D] * _dims[Dimension::E];
}

void
Shape::Impl::setMidplaneDimensions()
{
    for (Dimension i = Dimension::A; i <= Dimension::D; ++i)
    {
        try {
            _midplaneDims[i] = boost::lexical_cast<uint32_t>(ceil(double(_dims[i])/4.0));
        } catch (const boost::bad_lexical_cast& e) {
            THROW_EXCEPTION(
                    bgsched::InternalException,
                    bgsched::InternalErrors::UnexpectedError,
                    e.what()
                    );
        }
    }
}

} // namespace bgsched

