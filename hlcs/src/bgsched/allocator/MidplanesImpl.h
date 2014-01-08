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
 * \file bgsched/allocator/MidplanesImpl.h
 * \brief Midplanes::Impl class definition.
 */

#ifndef BGSCHED_ALLOCATOR_MIDPLANES_IMPL_H_
#define BGSCHED_ALLOCATOR_MIDPLANES_IMPL_H_

#include <bgsched/allocator/Midplanes.h>

#include <bgsched/Coordinates.h>

#include "bgsched/ComputeHardwareImpl.h"

namespace bgsched {
namespace allocator {

/*!
 * \brief Represents a set of midplanes.
 */
class Midplanes::Impl
{
public:

    /*!
     * \brief
     */
    Impl(
            const ComputeHardware::Pimpl hardware  //!< [in] Reference to hardware
            );

    /*!
     * \brief Get midplane at specific coordinate.
     *
     * \throws bgsched::allocator::InputException with value:
     * - bgsched::allocator::InputErrors::InvalidMidplaneCoordinates - if coordinates are invalid
     *
     * \return Midplane at specific coordinate.
     */
    Midplane::ConstPtr get(
            const bgsched::Coordinates& coordinates     //!< [in] Coordinates of midplane to retrieve
            ) const;

    /*!
     * \brief Get the machine size in midplanes for each dimension.
     *
     * \return Machine size in midplanes for each dimension.
     */
    const bgsched::Coordinates getMachineSize() const;

protected:

    const ComputeHardware::Impl::MidplaneArray& _midplanes;   //!< Reference to midplanes
    bgsched::Coordinates                        _coordinates; //!< Machine coordinates

};

} // namespace bgsched::allocator
} // namespace bgsched

#endif
