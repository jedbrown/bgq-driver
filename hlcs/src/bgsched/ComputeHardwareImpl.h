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
 * \file bgsched/ComputeHardwareImpl.h
 * \brief ComputeHardware::Impl class definition.
 */

#ifndef BGSCHED_COMPUTE_HARDWARE_IMPL_H_
#define BGSCHED_COMPUTE_HARDWARE_IMPL_H_

#include <bgsched/Coordinates.h>
#include <bgsched/Shape.h>

#include "bgsched/MidplaneImpl.h"

#include <boost/multi_array.hpp>

#include <map>
#include <ostream>
#include <string>

namespace bgsched {

/*!
 * \brief Represents the physical compute hardware and its state.
 */
class ComputeHardware::Impl
{
public:

    /*!
     * \brief Container of midplanes by coordinate.
     */
    typedef boost::multi_array<Midplane::Pimpl, Dimension::MidplaneDims> MidplaneArray;

    /*!
     * \brief Initialize the compute hardware structure for this machine.
     *
     * \throws bgsched::DatabaseException with values:
     * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
     * - bgsched::DatabaseErrors::ConnectionError - if error occurs connecting to the database
     * - bgsched::DatabaseErrors::UnexpectedError - if unexpected database error
     * - bgsched::DatabaseErrors::DataNotFound - if required data was not found in the database
     *
     * \throws bgsched::InternalException with values:
     * - bgsched::InternalErrors::XMLParseError - if error occurs parsing XML data
     * - bgsched::InternalErrors::InconsistentDataError - if inconsistent data found in the database
     */
    Impl();

    /*!
     * \brief Copy ctor.
     */
    Impl(
            const ComputeHardware::Pimpl fromComputeHardware //!< [in] From ComputeHardware
        );

    /*!
     * \brief Dump the compute hardware to stream.
     */
    void dump(
           std::ostream& os  //!< [in/out] Stream to write to
           );

    /*!
     * \brief Get the state of the hardware at the location given.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidLocationString - if the location string isn't valid
     *
     * \return State of the hardware at the location.
     */
    EnumWrapper<Hardware::State> getState(
            const std::string& location     //!< [in] Hardware location
            ) const;

    /*!
     * \brief Set hardware state for a midplane, node board or switch location.
     *
     * Note: The hardware state can not be updated if the hardware is "in use".
     *
     * \throws bgsched::RuntimeException with value:
     * - bgsched::RuntimeErrors::HardwareInUseError - if trying to update state when hardware is "in use"
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidLocationString - if the location string isn't valid
     */
    void setState(
            const std::string& location,    //!< [in] Hardware location
            Hardware::State state,          //!< [in] State to set the hardware to
            bool fromDatabase               //!< [in] Indicator if state update originated from database change
            );

    /*!
     * \brief Get the hardware object representing the location.
     *
     * \return Hardware object representing the location.
     */
    Hardware::Pimpl getHardware(
            const std::string& location     //!< [in] Hardware location
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
            const Dimension& i      //!< [in] Dimension to get the machine size
            ) const;

    /*!
     * \brief Get the coordinates of the last midplane.
     *
     * \return Coordinates of the last midplane.
     */
    const Coordinates& getMachineExtent() const;

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
     * \brief Validate the shape fits into the machine hardware.
     */
    bool validateShape(
            const Shape& shape      //!< [in] Shape to validate
            ) const;

    /*!
     * \brief Get midplane at machine coordinates given.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidMidplaneCoordinates if the coordinates are invalid.
     *
     * \return Midplane at the machine coordinates given.
     */
    Midplane::Pimpl getMidplane(
            const Coordinates& coords     //!< [in] Midplane coordinates
            ) const;

    /*!
     * \brief Get the midplane at the location given.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidLocationString if the location string is not valid.
     *
     * \return Midplane at the location given.
     */
    Midplane::Pimpl getMidplane(
            const bgq::util::Location& location     //!< [in] Midplane location
            ) const;

    /*!
     * \brief Get the midplane at the string location given.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidLocationString if the location string is not valid.
     *
     * \return Midplane at the string location given.
     */
    Midplane::Pimpl getMidplane(
            const std::string& location     //!< [in] Midplane location string (e.g. R00-M0)
            ) const;

    /*!
     * \brief Get container of machine midplanes.
     *
     * \return Machine midplanes.
     */
    const ComputeHardware::Impl::MidplaneArray& getMidplanes() const;

protected:

    /*!
     * \brief Initialize hardware from database.
     *
     * \throws bgsched::DatabaseException with values:
     * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
     * - bgsched::DatabaseErrors::ConnectionError - if error occurs connecting to the database
     * - bgsched::DatabaseErrors::UnexpectedError - if unexpected database error
     * - bgsched::DatabaseErrors::DataNotFound - if required data was not found in the database
     *
     * \throws bgsched::InternalException with values:
     * - bgsched::InternalErrors::XMLParseError - if error occurs parsing XML data
     * - bgsched::InternalErrors::InconsistentDataError - if inconsistent data found in the database
     */
    void initializeDatabase();

protected:

    typedef std::map<std::string, Midplane::Pimpl> MidplaneMap; //!< Collection of midplanes.

protected:

    MidplaneArray         _midplanes;     //!< Midplanes by coordinate
    MidplaneMap           _midplaneMap;   //!< Map the location string to the midplane
    Coordinates           _extent;        //!< Machine dimensions
    uint32_t              _rows;          //!< Number of compute rows
    uint32_t              _columns;       //!< Number of compute columns

};

} // namespace bgsched

#endif
