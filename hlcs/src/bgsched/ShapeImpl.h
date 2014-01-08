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
 * \file bgsched/ShapeImpl.h
 * \brief Shape::Impl class definition.
 */

#ifndef SHAPE_IMPL_H_
#define SHAPE_IMPL_H_

#include <bgsched/Dimension.h>
#include <bgsched/Shape.h>
#include <bgsched/types.h>

#include <boost/array.hpp>

namespace bgsched {

/*!
 * \brief Represents a five-dimensional shape.
 */
class Shape::Impl
{
public:

    static const Shape::Impl NodeBoard;   //!< Shape representing a single node board
    static const Shape::Impl Eighth;      //!< Shape representing an eighth of a midplane or two node boards
    static const Shape::Impl Quarter;     //!< Shape representing a quarter of a midplane or four node boards
    static const Shape::Impl Half;        //!< Shape representing a half of a midplane or eight node boards
    static const Shape::Impl Midplane;    //!< Shape representing a full midplane or 16 node boards

    /*!
     * \brief ctor for a size given in compute nodes.
     */
    Impl(
            const uint32_t a,     //!< [in] Compute node size in A dimension
            const uint32_t b,     //!< [in] Compute node size in B dimension
            const uint32_t c,     //!< [in] Compute node size in C dimension
            const uint32_t d,     //!< [in] Compute node size in D dimension
            const uint32_t e      //!< [in] Compute node size in E dimension
            );

    /*!
     * \brief ctor for a size given in midplanes.
     */
    Impl(
            const uint32_t a,     //!< [in] Midplane size in A dimension
            const uint32_t b,     //!< [in] Midplane size in B dimension
            const uint32_t c,     //!< [in] Midplane size in C dimension
            const uint32_t d      //!< [in] Midplane size in D dimension
            );

    /*!
     * \brief ctor for a size given in midplanes.
     */
    Impl(
            const uint32_t [Dimension::MidplaneDims]  //!< [in] Midplane size dimensions
            );

    /*!
     * \brief Copy ctor.
     */
    Impl(
            const Impl& impl   //!< [in] From shape
            );

    /*!
     * \brief Get size in midplanes for a given dimension.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidDimension - if not a valid Dimension (must be A-D)
     *
     * \return For large shapes the size in midplanes for a given dimension. If small shape then size
     * returned is always 1.
     */
    uint32_t getMidplaneSize(
            const Dimension dimension      //!< [in] Dimension (A-D) to return the size of
            ) const;

    /*!
     * \brief Get size in compute nodes for a given dimension.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidDimension - if not a valid Dimension (must be A-E)
     *
     * \return Size in compute nodes for a given dimension.
     */
    uint32_t getNodeSize(
            const Dimension dimension      //!< [in] Dimension (A-E) to return the size of
            ) const;

    /*!
     * \brief Get number of midplanes.
     *
     * This is the number of midplanes actually required for the full shape.
     *
     * \return For large shapes the number of midplanes. For small shapes the midplane count is always 1.
     */
    uint32_t getMidplaneCount() const;

    /*!
     * \brief Get number of compute nodes.
     *
     * This is the number of compute nodes actually required for the full shape, since the dimensions
     * are rounded up to full midplanes or to a specific dimension for small blocks.
     *
     * \return Number of compute nodes.
     */
    uint32_t getNodeCount() const;

    /*!
     * \brief Returns true if sub-midplane (32, 64, 128 or 256 compute nodes) sized shape.
     *
     * \return true if sub-midplane (32, 64, 128 or 256 compute nodes) sized shape.
     */
    bool isSmall() const;

    /*!
     * \brief Return true if midplane (512 compute nodes) or larger sized shape.
     *
     * \return true if midplane (512 compute nodes) or larger sized shape.
     */
    bool isLarge() const;

    /*!
     * \brief Return true if shape represents the full system (all midplanes).
     *
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors:UnexpectedError - if error accessing database occurs
     *
     * \return true if shape represents the full system (all midplanes).
     */
    bool isFull() const;

private:

    /*!
     * \brief Common code for constructors
     */
    void initMidplanes(
            const uint32_t a,     //!< [in] Midplane size in A dimension
            const uint32_t b,     //!< [in] Midplane size in B dimension
            const uint32_t c,     //!< [in] Midplane size in C dimension
            const uint32_t d      //!< [in] Midplane size in D dimension
            );

    void initialize(
            const uint32_t a,     //!< [in] Compute node size in A dimension
            const uint32_t b,     //!< [in] Compute node size in B dimension
            const uint32_t c,     //!< [in] Compute node size in C dimension
            const uint32_t d,     //!< [in] Compute node size in D dimension
            const uint32_t e      //!< [in] Compute node size in E dimension
            );

    /*!
     * \brief Set compute node count for shape.
     */
    void setSize();

    /*!
     * \brief Initialize midplane count from compute node count.
     *
     * This has the effect of increasing the "real" shape to fill full
     * midplanes, so compute node count will reflect the full size.
     */
    void setMidplaneDimensions();

protected:

    boost::array<uint32_t, Dimension::NodeDims>     _dims;          //!< Node dimensions
    boost::array<uint32_t, Dimension::MidplaneDims> _midplaneDims;  //!< Midplane dimensions
    uint32_t                                        _size;          //!< Shape size in compute nodes

};

} // namespace bgsched

#endif
