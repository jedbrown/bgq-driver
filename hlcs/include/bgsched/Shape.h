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
 * \file bgsched/Shape.h
 * \brief Shape class definition.
 */

#ifndef BGSCHED_SHAPE_H_
#define BGSCHED_SHAPE_H_

#include <bgsched/types.h>

#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>

namespace bgsched {

class Dimension;

/*!
 * \brief Represents a five-dimensional shape.
 *
 * The only way to construct a shape is through the Allocator. This makes all shapes valid
 * for the machine. The Allocator provides methods to create and rotate shapes.
 *
 * \sa Allocator
 * \sa Allocator::getShapesForSize
 * \sa Allocator::getShapes
 * \sa Allocator::getRotations
 * \sa Allocator::createMidplaneShape
 * \sa Allocator::createSmallShape
 */

class Shape
{
public:

    typedef boost::shared_ptr<Shape> Ptr;               //!< Pointer type.
    typedef boost::shared_ptr<const Shape> ConstPtr;    //!< Const pointer type.

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
            const Dimension& dimension      //!< [in] Dimension (A-D) to return the size of
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
             const Dimension& dimension      //!< [in] Dimension (A-E) to return the size of
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

    /*!
     * \brief Implementation type.
     */
    class Impl;

    /*!
     * \brief Pointer to implementation type.
     */
    typedef boost::shared_ptr<Impl> Pimpl;

    /*!
     * \brief Get pointer to implementation.
     *
     * \return Pointer to implementation.
     */
    Pimpl getPimpl() const;

    /*!
     * \brief
     */
    explicit Shape(
            Pimpl impl      //!< [in] Pointer to implementation
            );

    /*!
     * \brief Copy ctor.
     */
    Shape(
            const Shape& shape  //!< [in] Shape to copy from
            );

    /*!
     * \brief Equal operator.
     */
    bool operator==(
            const Shape& other //!< [in] Shape to compare against
            ) const;

    /*!
     * \brief Not equal operator.
     */
    bool operator!=(
            const Shape& other //!< [in] Shape to compare against
            ) const;

protected:

    Pimpl _impl;

};

} // namespace bgsched

#endif
