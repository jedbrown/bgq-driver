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
 * \file bgsched/Midplane.h
 * \brief Midplane class definition.
 */

#ifndef BGSCHED_MIDPLANE_H_
#define BGSCHED_MIDPLANE_H_

#include <bgsched/Coordinates.h>
#include <bgsched/Dimension.h>
#include <bgsched/Hardware.h>
#include <bgsched/NodeBoard.h>
#include <bgsched/Switch.h>

#include <boost/shared_ptr.hpp>

#include <string>

namespace bgsched {

/*!
 * \brief Represents a midplane and its hardware.
 */
class Midplane : public Hardware
{
public:

    typedef boost::shared_ptr<Midplane> Ptr;             //!< Pointer type.
    typedef boost::shared_ptr<const Midplane> ConstPtr;  //!< Const pointer type.

    typedef std::vector<Ptr> Ptrs;                       //!< Collection of pointers.
    typedef std::vector<ConstPtr> ConstPtrs;             //!< Collection of const pointers.

    /*!
     * \brief Midplane in use indicator.
     *
     * When a midplane is in use by a large block, use the midplane getComputeBlockName() method
     * to obtain the allocated block. If the midplane is in use by one or more small blocks then
     * the node board getComputeBlockName() must be used to obtain the block names.
     */
    enum InUse {
        NotInUse = 0,    //!< Midplane not in use
        AllInUse,        //!< Midplane "in use" by large block
        SubdividedInUse  //!< Midplane "in use" by one or more small blocks
    };

    static const uint32_t Eighth = NodeBoard::NodeCount * 2;                 //!< Number of compute nodes on an eighth of a midplane or two node boards
    static const uint32_t Quarter = Eighth * 2;                              //!< Number of compute nodes on a quarter of a midplane or four node boards
    static const uint32_t Half = Quarter * 2;                                //!< Number of compute nodes on a half of a midplane or eight node boards
    static const uint32_t NodeCount = Half * 2;                              //!< Number of compute nodes on a full midplane or 16 node boards
    static const uint32_t MaxNodeBoards = NodeCount / NodeBoard::NodeCount;  //!< Number of nodes boards in a midplane

public:

    /*!
     * \brief Get midplane sequence ID.
     *
     * \return Midplane sequence ID.
     */
    SequenceId getSequenceId() const;

    /*!
     * \brief Get the compute node memory size in megabytes.
     *
     * \return Compute node memory size in megabytes.
     */
    uint32_t getComputeNodeMemory() const;

    /*!
     * \brief Get the number of available node boards.
     *
     * \return Number of available node boards.
     */
    uint32_t getAvailableNodeBoardCount() const;

    /*!
     * \brief Get a specific node board position.
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidNodeBoardPosition - if the position is not 0-15
     *
     * \return Specific node board position.
     */
    NodeBoard::ConstPtr getNodeBoard(
            const uint32_t position              //!< [in] Integer position (0-15) of node board
            ) const;

    /*!
     * \brief Get a specific switch in the dimension given.
     *
     * \throws bgsched::InputException with values:
     * - bgsched::InputErrors::InvalidDimension - if dimension is not A, B, C or D
     *
     * \return Switch at the given dimension.
     */
    Switch::ConstPtr getSwitch(
            const Dimension& dimension   //!< [in] Dimension of switch to return, A-D
            ) const;

    /*!
     * \brief Get the machine coordinates of this midplane.
     *
     * \return Machine coordinates of the midplane.
     */
    const Coordinates& getCoordinates() const;

    /*!
     * \brief Get the midplane "in use" indicator.
     *
     * \return Midplane "in use" indicator.
     */
    EnumWrapper<Midplane::InUse> getInUse() const;

    /*!
     * \brief Get the "in use" compute block name.
     *
     * \return "In use" compute block name. Returns empty string if midplane is not is use.
     */
    const std::string& getComputeBlockName() const;

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
    explicit Midplane(
            Pimpl impl      //!< [in] Pointer to implementation
            );

    /*!
     * \brief Get pointer to implementation.
     *
     * \return Pointer to implementation.
     */
    Pimpl getPimpl();
};

} // namespace bgsched

#endif
