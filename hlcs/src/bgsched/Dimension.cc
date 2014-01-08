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
#include <bgsched/RuntimeException.h>

#include "bgsched/utility.h"

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>

#include <iostream>

using namespace std;

LOG_DECLARE_FILE("bgsched");

namespace bgsched {

Dimension::Dimension(
        Value v
        ) :
    _value(v)
{
    if (_value > Dimension::E) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidDimension,
                boost::lexical_cast<string>(_value) + " is not a valid Dimension");
    }
}

Dimension::Dimension(
        const uint32_t dimension
        ) :
    _value(Dimension::A)
{
    if (dimension > Dimension::E) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidDimension,
                boost::lexical_cast<string>(dimension) + " is not a valid Dimension");
    }
    _value = Value(dimension);
}

Dimension::Dimension() :
    _value(InvalidDimension)
{
}

Dimension&
Dimension::operator++()
{
    if (_value == Dimension::E || _value == InvalidDimension) {
        _value = InvalidDimension;
    } else {
        _value = Value(_value + 1);
    }

    return *this;
}

Dimension
Dimension::operator++(
        const int
        )
{
    Dimension temp = *this;
    ++*this;
    return temp;
}

Dimension&
Dimension::operator--()
{
    if (_value == Dimension::A || _value == InvalidDimension) {
        _value = InvalidDimension;
    } else {
        _value = Value(_value - 1);
    }

    return *this;
}

Dimension
Dimension::operator--(
        const int
        )
{
    Dimension temp = *this;
    --*this;
    return temp;
}

bool
Dimension::operator==(
        const Dimension& other
        ) const
{
    return _value == other._value;
}

bool
Dimension::operator!=(
        const Dimension& other
        ) const
{
    return _value != other._value;
}

Dimension::operator string() const
{
    static const char* label[] = {"A","B","C","D","E"};
    if (_value <= Dimension::E) {
        return label[_value];
    } else {
        return "INVALID";
    }
}

Dimension::operator uint32_t() const
{
    return _value;
}

} // namespace bgsched
