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
#include <utility/include/BoolAlpha.h>

#include <iostream>
#include <stdexcept>

namespace bgq {
namespace utility {

BoolAlpha::BoolAlpha(
        bool value
        ) :
    _value( value )
{

}

std::ostream&
operator<<(
        std::ostream& stream,
        const BoolAlpha& b
        )
{
    stream << std::boolalpha << b._value;
    return stream;
}

std::istream&
operator>>(
        std::istream& stream,
        BoolAlpha& b
        )
{
    stream >> std::boolalpha >> b._value;

    return stream;
}

} // utility
} // bgq
