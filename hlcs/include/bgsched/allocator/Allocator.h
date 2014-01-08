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
 * \file bgsched/allocator/Allocator.h
 * \brief Allocator class definition.
 */

#ifndef BGSCHED_ALLOCATOR_ALLOCATOR_H_
#define BGSCHED_ALLOCATOR_ALLOCATOR_H_

#include <bgsched/allocator/LiveModel.h>
#include <bgsched/allocator/Model.h>
#include <bgsched/allocator/ResourceSpec.h>

#include <bgsched/Block.h>
#include <bgsched/Midplane.h>
#include <bgsched/types.h>

#include <boost/shared_ptr.hpp>

#include <ostream>
#include <string>
#include <vector>

namespace bgsched {

class Shape;

namespace allocator {

/*!
 * \brief High-level class containing objects related to resource allocation.
 *
 * A scheduler will instantiate this class first.
 *
 * The allocator is the highest-level object. It contains one LiveModel and a plug-in container.
 *
 * The LiveModel represents a snapshot of the hardware and state updates are done by calling
 * the syncState() method of the LiveModel.
 */
class Allocator : boost::noncopyable
{
public:

    /*!
     * \brief Dump verbosity level.
     */
    struct DumpVerbosity {
        enum Value {
            BlockShapes  //!< Dump list of base shapes
        };
    };

    typedef std::vector<Shape> Shapes;   //!< Collection of Shapes

    /*!
     * \brief
     *
     * \throws bgsched::allocator::RuntimeException with values:
     * - bgsched::allocator::RuntimeErrors::PipeError - if unexpected error creating pipe
     * - bgsched::allocator::RuntimeErrors::ThreadError - if unexpected error creating thread
     */
    Allocator();

    /*!
     * \brief
     */
    ~Allocator();

    /*!
     * \brief Get reference to the live model.
     *
     * \return Reference to the live model. All calls to this method return a reference to the same object.
     */
    LiveModel& getLiveModel() const;

    /*!
     * \brief Get a list of all installed plug-ins.
     *
     * \return Vector of the successfully loaded plug-ins.
     */
    std::vector<std::string> getPluginList() const;

    /*!
     * \brief Get a list of possible shapes for the given number of compute nodes.
     *
     * Note: Number of compute nodes will be rounded up to match the nearest valid shape if necessary.
     *
     * \throws bgsched::allocator::InputException with value:
     * - bgsched::allocator::InputErrors::InvalidComputeNodeSize - if number of compute nodes given exceeds machine compute nodes
     *
     * \throws bgsched::allocator::InternalException with value:
     * - bgsched::allocator::InternalErrors::UnexpectedError - if unexpected error occurs
     *
     * \return Vector of valid shapes for the given number of compute nodes.
     */
    Shapes getShapesForSize(
            const uint32_t size           //!< [in] Number of compute nodes
            ) const;

    /*!
     * \brief Get list of all possible large shapes for the machine.
     *
     * \return Vector with all possible large shapes for the machine.
     */
    Shapes getShapes() const;

    /*!
     * \brief Get list of valid rotated shapes.
     *
     * Note: Small block shapes can not be rotated so the same shape is returned.
     *
     * \return Vector of valid rotated shapes.
     */
    Shapes getRotations(
            const Shape& shape      //!< [in] Shape to rotate
            ) const;

    /*!
     * \brief Create large shape based on midplane dimensions.
     *
     * Only valid shapes for this machine are returned.
     *
     * \throws bgsched::allocator::ShapeException with value:
     * - bgsched::allocator::ShapeErrors::InvalidMidplaneDimension - if the size in any dimension exceeds machine size
     *
     * \return Large shape based on given midplane dimensions.
     */
    Shape createMidplaneShape(
            const uint32_t a,     //!< [in] Midplane size in A dimension
            const uint32_t b,     //!< [in] Midplane size in B dimension
            const uint32_t c,     //!< [in] Midplane size in C dimension
            const uint32_t d      //!< [in] Midplane size in D dimension
            ) const;

    /*!
     * \brief Create a small shape with 32, 64, 128 or 256 compute nodes.
     *
     * There is a predetermined set of sub-midplane (32, 64, 128 or 256 compute nodes) shapes.
     * The shape returned may have more compute nodes than the size given.  The size is increased
     * to the next largest valid sub-midplane shape.
     *
     * \throws bgsched::allocator::ShapeException with value:
     * - bgsched::allocator::ShapeErrors::InvalidSize - if the compute node size exceeds the maximum small shape size
     *
     * \return Small shape based on given compute node size.
     */
    Shape createSmallShape(
            const uint32_t size       //!< [in] Number of compute nodes
            ) const;

    /*!
     * \brief Find available resources to satisfy the compute block resource specification by calling the
     * specified plug-in on the specified model.
     *
     * No resources are allocated by this method, the model remains unchanged.
     *
     * Resources can be allocated by passing the block object to the prepare method.
     *
     * The compute block returned will have hardware resources set, but the user should set the block name (Id),
     * and if desired any of the other settable block fields.
     *
     * Some plug-in providers may set a default block name (Id) and block description.
     *
     * \throws bgsched::allocator::InputException with value:
     * - bgsched::allocator::InputErrors::PluginNotFound - the plug-in name does not match a loaded plug-in
     *
     * \throws bgsched::allocator::Plugin::Exception with value:
     * - bgsched::allocator::Plugin::Errors::FindBlockResourcesFailed - unexpected error finding block resources
     *
     * \return Block object which satisfies the resource specification or a NULL Block::Ptr
     * if no resources are available matching resource specification.
     */
    Block::Ptr findBlockResources(
            const Model& model,                 //!< [in] Model to search for available resources
            const ResourceSpec& resource,       //!< [in] Compute block resource requirements
            const std::string& plugin           //!< [in] Name of plug-in to use
            ) const;

    /*!
     * \brief Reserve the hardware resources specified by the compute block object, marking them as being in use.
     *
     * The compute block may be obtained from the findBlockResources() method implemented by
     * a plug-in or by using one of the static Block::create() methods.
     *
     * For the live model, registered listeners will be notified when the block is available to run a job or if the
     * block boot fails.
     *
     * \throws bgsched::allocator::InputException with value:
     * - bgsched::allocator::InputErrors::PluginNotFound - the plug-in name does not match a loaded plug-in
     *
     * \throws bgsched::allocator::ResourceAllocationException with value:
     * - bgsched::allocator::ResourceAllocationErrors::BlockResourcesUnavailable - block resources are not available for allocation
     */
    void prepare(
            Model& model,                      //!< [in] Model to update with allocation
            const std::string& blockName,      //!< [in] Compute block to prepare
            const std::string& plugin          //!< [in] Name of plug-in to use
            );

    /*!
     * \brief Deallocates the hardware resources specified by the compute block object.
     *
     * For the live model, registered listeners will be notified when the compute block resources are available
     * to be used by another request.
     *
     * \throws bgsched::allocator::InputException with value:
     * - bgsched::allocator::InputErrors::PluginNotFound - the plug-in name does not match a loaded plug-in
     *
     * \throws bgsched::allocator::ResourceAllocationException with value:
     * - bgsched::allocator::ResourceAllocationErrors::BlockDeallocateFailed - block could not be deallocated (e.g. the block was not found)
     */
    void release(
            Model& model,                     //!< [in] Model to update with deallocation
            const std::string& blockName,     //!< [in] Compute block to release
            const std::string& plugin         //!< [in] Name of plug-in to use
            );

    /*!
     * \brief Dump the allocator information to a stream (typically for debugging usage).
     */
   void dump(
           std::ostream& os,                                      //!< [in/out] Stream to write to
           EnumWrapper<Allocator::DumpVerbosity::Value> verbosity //!< [in] Verbosity level of output
           ) const;

    /*!
     * \brief Implementation type.
     */
    class Impl;

    /*!
     * \brief Pointer to implementation type.
     */
    typedef boost::shared_ptr<Impl> Pimpl;

protected:

    Pimpl _impl;

};

} // namespace bgsched::allocator
} // namespace bgsched

#endif
