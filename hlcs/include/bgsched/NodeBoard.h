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
 * \file bgsched/NodeBoard.h
 * \brief NodeBoard class definition.
 */

#ifndef BGSCHED_NODEBOARD_H_
#define BGSCHED_NODEBOARD_H_

#include <bgsched/Coordinates.h>
#include <bgsched/Hardware.h>
#include <bgsched/types.h>

#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>

namespace bgsched {

/*!
 * \brief Represents a node board.
 */
class NodeBoard : public Hardware
{
public:

    /*!
     * \brief Node board quadrant identifier.
     */
    enum Quadrant {
        Q1,              //!< Quadrant 1 comprising node boards N00..N03
        Q2,              //!< Quadrant 2 comprising node boards N04..N07
        Q3,              //!< Quadrant 3 comprising node boards N08..N11
        Q4               //!< Quadrant 4 comprising node boards N12..N15
    };

    static const uint32_t NodeCount = 32;                //!< Number of compute nodes on a node board

    typedef boost::shared_ptr<NodeBoard> Ptr;            //!< Pointer type.
    typedef boost::shared_ptr<const NodeBoard> ConstPtr; //!< Const pointer type.

    typedef std::vector<Ptr> Ptrs;                       //!< Collection of pointers.
    typedef std::vector<ConstPtr> ConstPtrs;             //!< Collection of const pointers.

    /*!
     * \brief Get node board sequence ID.
     *
     * \return Node board sequence ID.
     */
    SequenceId getSequenceId() const;

    /*!
     * \brief Get quadrant of node board on the midplane.
     *
     * \return Quadrant of node board on the midplane.
     */
    EnumWrapper<NodeBoard::Quadrant> getQuadrant() const;

    /*!
     * \brief Node board "in use" indicator.
     *
     * \return true if node board is "in use".
     */
    bool isInUse() const;

    /*!
     * \brief Get the "in use" compute block name.
     *
     * \return "In use" compute block name. Returns empty string if node board is not is use.
     */
    const std::string& getComputeBlockName() const;

    /*!
     * \brief Get the machine coordinates of the midplane the node board is located on.
     *
     * \throws bgsched::DatabaseException with value:
     * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
     *
     * \return Machine coordinates of the midplane the node board is located on.
     */
    const Coordinates getMidplaneCoordinates() const;

    /*!
     * \brief Get the midplane location the node board is on.
     *
     * \return Midplane location the node board is on.
     */
    const std::string getMidplaneLocation() const;

    /*!
     * \brief Get the number of available compute nodes.
     *
     * \return Number of available compute nodes.
     */
    uint32_t getAvailableNodeCount() const;

    /*!
     * \brief Meta-state indicator. If "true" the hardware state value of the node board represents a meta-state,
     * which indicates that one or more compute nodes are not "Available". A "false" value means the hardware state
     * is the actual representation of the node board hardware.
     *
     * \return true if node board hardware state represents a meta-state.
     */
    bool isMetaState() const;

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
    explicit NodeBoard(
            Pimpl impl    //!< [in] Pointer to implementation
            );
};

} // namespace bgsched

#endif
