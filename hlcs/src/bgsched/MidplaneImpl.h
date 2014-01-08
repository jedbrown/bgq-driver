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
 * \file bgsched/MidplaneImpl.h
 * \brief Midplane::Impl class definition.
 */

#ifndef BGSCHED_MIDPLANE_IMPL_H_
#define BGSCHED_MIDPLANE_IMPL_H_

#include <bgsched/Coordinates.h>
#include <bgsched/ComputeHardware.h>
#include <bgsched/Exception.h>
#include <bgsched/Midplane.h>
#include <bgsched/types.h>

#include "bgsched/HardwareImpl.h"
#include "bgsched/NodeBoardImpl.h"
#include "bgsched/SwitchImpl.h"

#include <bgq_util/include/Location.h>

#include <boost/array.hpp>
#include <boost/assert.hpp>
#include <boost/shared_ptr.hpp>

#include <string>

namespace bgsched {

/*!
 * \brief Represents a midplane.
 */
class Midplane::Impl : public Hardware::Impl
{
public:

    /*!
     * \brief
     *
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::XMLParseError - if error parsing XML
     */
    Impl(
            const std::string& location,    //!< [in] Midplane location
            const XMLEntity* XMLEntityPtr   //!< [in] XML describing the midplane
            );

    /*!
     * \brief Copy ctor.
     *
     * Creates a copy of a midplane.
     */
    Impl(
            const Midplane::Pimpl fromMidplane //!< [in] From midplane
            );

    /*!
     * \brief Dump the midplane to stream.
     */
    void dump(
           std::ostream& os  //!< [in/out] Stream to write to
           );

    /*!
     * \brief Get the midplane "in use" indicator.
     *
     * \return Midplane "in use" indicator.
     */
    EnumWrapper<Midplane::InUse> getInUse() const;

    /*!
     * \brief Set the midplane "in use" indicator.
     */
    void setInUse(
            Midplane::InUse inUse //!< [in] Midplane "in use" indicator
            );

    /*!
     * \brief Get the "in use" compute block name.
     *
     * \return "In use" compute block name.
     */
    const std::string& getComputeBlockName() const;

    /*!
     * \brief Set the "in use" compute block name.
     */
    void setComputeBlockName(
            const std::string& computeBlockName  //!< [in] Compute block
            );

    /*!
     * \brief Get the compute node memory size in megabytes.
     *
     * \return Compute node memory size in megabytes.
     */
    uint32_t getComputeNodeMemory() const;

    /*!
     * \brief Get the number of available node boards.
     *
     * \return Count of available node boards.
     */
    uint32_t getAvailableNodeBoardCount() const;

    /*!
     * \brief Get a specific node board position.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidNodeBoardPosition - if the position is not 0-15
     *
     * \return Specific node board position.
     */
    NodeBoard::Pimpl getNodeBoard(
            const uint32_t position      //!< [in] Integer position (0-15) of node board
            ) const;

    /*!
     * \brief Get a specific switch in the dimension given.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidDimension - if dimension is not A, B, C or D
     *
     * \return Switch at the given dimension.
     */
    Switch::Pimpl getSwitch(
            const Dimension& s  //!< [in] Switch dimension to get
            ) const;

    /*!
     * \brief Get a specific switch in the dimension given.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidDimension - if dimension is not A, B, C or D
     *
     * \return Switch at the given dimension.
     */
    Switch::Pimpl getSwitch(
            const bgq::util::Location::Dimension::Value s  //!< [in] Switch dimension to get
            ) const;

    /*!
     * \brief Get the machine coordinates of this midplane.
     *
     * \return Machine coordinates of the midplane.
     */
    const Coordinates& getCoordinates( ) const;

    /*!
     * \brief Add node board to midplane.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidNodeBoardPosition - if the position is not 0-15
     */
    void addNodeBoard(
            const uint32_t position,      //!< [in] Position
            const NodeBoard::Pimpl n      //!< [in] Pointer to node board implementation to be added
            );

    /*!
     * \brief Add switch to midplane.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidDimension - if dimension is not A, B, C or D
     */
    void addSwitch(
            const uint32_t dimension,  //!< [in] Dimension
            const Switch::Pimpl s      //!< [in] Pointer to switch implementation to be added
            );

protected:

    Midplane::InUse                                       _inUse;              //!< Midplane "in use" indicator
    std::string                                           _computeBlockName;   //!< "In use" compute block
    boost::array<Switch::Pimpl, Dimension::MidplaneDims>  _switches;           //!< Midplane switches
    boost::array<NodeBoard::Pimpl, MaxNodeBoards>         _nodeBoards;         //!< Midplane node boards
    Coordinates                                           _machineCoordinates; //!< Midplane coordinates
    uint32_t                                              _computeNodeMemory;  //!< Midplane compute node size in megabytes

};

/*!
 * \brief ostream operator.
 */
std::ostream&
operator<<(std::ostream& os,                        //!< [in/out] Stream to write to
        const Coordinates& coordinates              //!< [in] Midplane coordinates
        );

} // namespace bgsched

#endif
