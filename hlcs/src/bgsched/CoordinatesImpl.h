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
 * \file bgsched/CoordinatesImpl.h
 * \brief Coordinates::Impl class definition.
 */

#ifndef BGSCHED_COORDINATES_IMPL_H_
#define BGSCHED_COORDINATES_IMPL_H_

#include <bgsched/Coordinates.h>
#include <bgsched/Dimension.h>

#include <stdint.h>

namespace bgsched {

/*!
 * \brief Represents coordinates.
 */
class Coordinates::Impl
{
public:

    /*!
     * \brief ctor for 5D coordinates (e.g. node board coordinates).
     */
    Impl(
            const uint32_t A,    //!< [in] Coordinate for A dimension
            const uint32_t B,    //!< [in] Coordinate for B dimension
            const uint32_t C,    //!< [in] Coordinate for C dimension
            const uint32_t D,    //!< [in] Coordinate for D dimension
            const uint32_t E     //!< [in] Coordinate for E dimension
            );

    /*!
     * \brief ctor for 4D coordinates (e.g. midplane coordinates or machine size).
     */
    Impl(
            const uint32_t A,    //!< [in] Coordinate for A dimension
            const uint32_t B,    //!< [in] Coordinate for B dimension
            const uint32_t C,    //!< [in] Coordinate for C dimension
            const uint32_t D     //!< [in] Coordinate for D dimension
            );

    /*!
     * \brief Array subscript operator.
     */
    uint32_t &operator[](
            Dimension::Value element //!< [in] Coordinate element
            );

    /*!
     * \brief Array subscript operator.
     */
    uint32_t &operator[](
            uint32_t element //!< [in] Coordinate element
            );

    /*!
     * \brief Array subscript operator.
     */
    uint32_t operator[](
            Dimension::Value element //!< [in] Coordinate element
            ) const;

    /*!
     * \brief Array subscript operator.
     */
    uint32_t operator[](
            uint32_t element //!< [in] Coordinate element
            ) const;

    /*!
     * \brief Assignment operator.
     */
    void operator=(
            const Coordinates& coord //!< [in] Coordinates
            );

    /*!
     * \brief Equality operator.
     */
    bool operator==(
            const Coordinates& coord //!< [in] Coordinates
            ) const;

    /*!
     * \brief Non-equality operator.
     */
    bool operator!=(
            const Coordinates& coord //!< [in] Coordinates
            ) const;

    /*!
     * \brief Less than or equal to operator.
     */
    bool operator<=(
            const Coordinates& coord //!< [in] Coordinates
            ) const;

    /*!
     * \brief Greater than or equal to operator.
     */
    bool operator>=(
            const Coordinates& coord //!< [in] Coordinates
            ) const;

    const uint32_t* data() const  { return _coords; }

protected:

    uint32_t _coords[Dimension::NodeDims];  //!< Coordinate array (used for both 4D and 5D)
    uint32_t _size;                         //!< Indicator for 4D or 5D coordinates

};

} // namespace bgsched

#endif
