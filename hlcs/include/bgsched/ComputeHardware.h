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
 * \file bgsched/ComputeHardware.h
 * \brief ComputeHardware class definition.
 */

#ifndef BGSCHED_COMPUTE_HARDWARE_H_
#define BGSCHED_COMPUTE_HARDWARE_H_

#include <bgsched/Coordinates.h>
#include <bgsched/EnumWrapper.h>
#include <bgsched/Midplane.h>

#include <boost/shared_ptr.hpp>

#include <string>

namespace bgsched {

/*!
 * \brief Represents the physical compute hardware and its state.
 *
 * This object provides methods to query compute hardware configuration and state.
 */
class ComputeHardware
{
public:

    typedef boost::shared_ptr<ComputeHardware> Ptr;             //!< Pointer type.
    typedef boost::shared_ptr<const ComputeHardware> ConstPtr;  //!< Const pointer type.

    /*!
     * \brief Get the state of the hardware (midplane, node board, switch) at the location given.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidLocationString - if the location string isn't valid
     *
     * \return State of hardware at given location.
     */
    EnumWrapper<Hardware::State> getState(
            const std::string& location      //!< [in] Hardware location
    ) const;

    /*!
     * \brief Get the machine size in midplanes for the given dimension.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidDimension - if not a valid Dimension (must be A-D)
     *
     * \return Size in midplanes in the given dimension.
     */
    uint32_t getMachineSize(
            const Dimension dimension              //!< [in] Dimension (must be A-D)
    ) const;

    /*!
     * \brief Get the midplane size in compute nodes for the given dimension.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidDimension - if not a valid Dimension (must be A-E)
     *
     * \return Size in compute nodes in the given dimension.
     */
    uint32_t getMidplaneSize(
            const Dimension dimension              //!< [in] Dimension (must be A-E)
    ) const;

    /*!
     * \brief Get the machine size in rack rows.
     *
     * \return Machine size in rack rows.
     */
    uint32_t getMachineRows() const;

    /*!
     * \brief Get the machine size in rack columns.
     *
     * \return Machine size in rack columns.
     */
    uint32_t getMachineColumns() const;

    /*!
     * \brief Get midplane at machine coordinates given.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidMidplaneCoordinates if the coordinates are invalid.
     *
     * \return Midplane at the machine coordinates given.
     */
    Midplane::ConstPtr getMidplane(
            const Coordinates& coordinates     //!< [in] Midplane coordinates
    ) const;

    /*!
     * \brief Get the midplane at the location given.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidLocationString if the location string is not valid.
     *
     * \return Midplane at the location given.
     */
    Midplane::ConstPtr getMidplane(
            const std::string& location //!< [in] Midplane location (e.g. R00-M0)
    ) const;

    /*!
     * \brief Implementation type.
     */
    class Impl;

    /*!
     * \brief Pointer to implementation type.
     */
    typedef boost::shared_ptr<Impl> Pimpl;

    /*!
     * \brief
     */
    explicit ComputeHardware(
            Pimpl impl //!< [in] Pointer to implementation
    );

protected:

    Pimpl _impl;

};

} // namespace bgsched

#endif
