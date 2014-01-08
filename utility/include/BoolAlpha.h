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
/*!
 * \file utility/include/BoolAlpha.h
 */
#ifndef UTILITY_BOOL_ALPHA_H
#define UTILITY_BOOL_ALPHA_H

#include <iosfwd>

namespace bgq {
namespace utility {

/*!
 * \brief Wrapper around std::boolalpha
 *
 * Example usage
 *
 * \include test/BoolAlpha/sample.cc
 *
 * doing this allows your program to accept boolean arguments with implicit
 * or explicit values in the form of true/false. Example
 *
\verbatim
./a.out --flag true
./a.out --flag false
./a.out --flag
\endverbatim
 */
class BoolAlpha
{
public:
    /*!
     * \brief ctor.
     *
     * \throws std::logic_error 
     *
     * \note purposefully implicit ctor to allow implicit conversions.
     */
    BoolAlpha(
            bool value = false  //!< [in]
            );

    bool _value;
};

/*!
 * \brief extraction operator.
 */
std::ostream&
operator<<(
        std::ostream& stream,       //!< [in]
        const BoolAlpha& b          //!< [in]
        );

/*!
 * \brief insertion operator.
 */
std::istream&
operator>>(
        std::istream& stream,       //!< [in]
        BoolAlpha& b                //!< [in]
        );

} // utility
} // bgq

#endif
