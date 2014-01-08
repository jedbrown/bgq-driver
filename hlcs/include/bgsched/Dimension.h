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
 * \file bgsched/Dimension.h
 * \brief Dimension class definition.
 */

#ifndef BGSCHED_DIMENSION_H_
#define BGSCHED_DIMENSION_H_

#include <stdint.h>
#include <string>

namespace bgsched {

/*!
 * \brief Wrapper class around 5 dimensional values.
 */
class Dimension
{
public:
    /*!
     * \brief Possible dimension values.
     */
    enum Value {
        A = 0,                //!< A dimension
        B = 1,                //!< B dimension
        C = 2,                //!< C dimension
        D = 3,                //!< D dimension
        E = 4,                //!< E dimension
        InvalidDimension = 99 //!< Denotes invalid dimension value
    };

    static const uint32_t MidplaneDims = 4; //!< Number of midplane dimensions.
    static const uint32_t NodeDims = 5;     //!< Number of node dimensions.

    /*!
     * \brief
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidDimension - if v is not a valid Dimension
     */
    Dimension(
            Value v   //!< [in] Dimension value
            );

    /*!
     * \brief
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidDimension - if not a valid Dimension
     */
    Dimension(
            const uint32_t dimension  //!< [in] Dimension value
            );

    /*!
     * \brief Sentinel
     */
    explicit Dimension();

    /*!
     * \brief Prefix increment operator.
     */
    Dimension& operator++();

    /*!
     * \brief Postfix increment operator.
     */
    Dimension operator++(
            const int
            );

    /*!
     * \brief Prefix decrement operator.
     */
    Dimension& operator--();

    /*!
     * \brief Postfix decrement operator.
     */
    Dimension operator--(
            const int
            );

    /*!
     * \brief Equality operator.
     */
    bool operator==(
            const Dimension& other
            ) const;

    /*!
     * \brief Non-equality operator.
     */
    bool operator!=(
            const Dimension& other
            ) const;

    /*!
     * \brief Conversion to std::string operator.
     */
    operator std::string() const;

    /*!
     * \brief Conversion to uint32_t.
     */
    operator uint32_t() const;

protected:

    Value _value;       //!< The actual dimension

};

} // bgsched

#endif
