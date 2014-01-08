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
 * \file bgsched/NodeBoardImpl.h
 * \brief NodeBoard::Impl class definition.
 */

#ifndef BGSCHED_NODEBOARD_IMPL_H_
#define BGSCHED_NODEBOARD_IMPL_H_

#include <utility/include/XMLEntity.h>

#include <bgsched/NodeBoard.h>

#include "bgsched/HardwareImpl.h"

#include <string>

namespace bgsched {

/*!
 * \brief Represents a node board.
 */
class NodeBoard::Impl : public Hardware::Impl
{
public:

    /*!
     * \brief
     *
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::XMLParseError - if error parsing XML
     */
    Impl(
            const std::string& location,        //!< [in] Location of the node board
            const XMLEntity* XMLEntityPtr       //!< [in] XML describing the node board
    );

    /*!
     * \brief Copy ctor.
     *
     * This creates a copy of a node board.
     */
    Impl(
            const NodeBoard::Pimpl fromNodeBoard    //!< [in] From node board
    );

    /*!
     * \brief Dump the node board to stream.
     */
    void dump(
            std::ostream& os   //!< [in/out] Stream to write to
    );

    /*!
     * \brief Node board "in use" indicator.
     *
     * \return true if node board is "in use".
     */
    bool isInUse() const;

    /* \brief Get the "in use" compute block name.
     *
     * \return "In use" compute block name.
     */
    const std::string& getComputeBlockName() const;

    /*!
     * \brief Get quadrant of node board on the midplane.
     *
     * \return Quadrant of node board on the midplane.
     */
    const EnumWrapper<NodeBoard::Quadrant> getQuadrant() const;

    /*!
     * \brief Set the node board "in use" indicator.
     */
    void setInUse(
            bool inUse    //!< [in] "In use" indicator to set
    );

    /*!
     * \brief Set the "in use" compute block name.
     */
    void setComputeBlockName(
            const std::string& computeBlockName //!< [in] "In use" compute block
    );

    /*!
     * \brief Set the node board quadrant.
     */
    void setQuadrant(
            NodeBoard::Quadrant quadrant   //!< [in] Node board quadrant
    );

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
     * \brief Set the number of available compute nodes.
     */
    void setAvailableNodeCount(
            const uint32_t nodeCount //!< [in] Count of available compute nodes to set
    );

    /*!
     * \brief Meta-state indicator. If "true" the hardware state value of the node board represents a meta-state,
     * which indicates that one or more compute nodes are not "Available". A "false" value means the hardware state
     * is the actual representation of the node board hardware.
     *
     * \return true if node board hardware state represents a meta-state.
     */
    bool isMetaState() const;

    /*!
     * \brief Set the node board meta-state indicator.
     */
    void setMetaState(
            bool metaState    //!< [in] Meta-state indicator to set
    );

protected:

    bool                  _inUse;               //!< Node board "in use" indicator
    std::string           _computeBlockName;    //!< "In use" compute block
    NodeBoard::Quadrant   _quadrant;            //!< Node board quadrant
    bool                  _isMetaState;         //!< Node board meta-state indicator
    uint32_t              _availableNodeCount;  //!< Count of computes nodes with "Available" state

};

/*!
 * \brief ostream operator
 * \param[in] os
 * \param[in] state
 */
std::ostream&
operator<<(
        std::ostream& os,               //!< [in/out] Stream to write to
        NodeBoard::Quadrant quadrant    //!< [in] Node board quadrant
        );

} // namespace bgsched

#endif
