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
 * \file bgsched/allocator/AllocatorImpl.h
 * \brief Allocator::Impl class definition.
 */

#ifndef BGSCHED_ALLOCATOR_IMPL_H_
#define BGSCHED_ALLOCATOR_IMPL_H_

#include <bgsched/allocator/Allocator.h>
#include <bgsched/allocator/LiveModel.h>

#include "bgsched/allocator/LiveModelImpl.h"
#include "bgsched/allocator/ModelImpl.h"
#include "bgsched/allocator/PluginContainer.h"

#include <bgsched/Block.h>
#include <bgsched/types.h>

#include <boost/thread/shared_mutex.hpp>

#include <map>
#include <string>
#include <vector>

namespace bgsched {
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
class Allocator::Impl
{
public:

    typedef std::vector<PluginContainer::Map::key_type> Plugins;  //!< Collection of plug-ins

    /*!
     * \brief
     */
    Impl();

    /*!
     * \brief Get reference to the live model.
     *
     * \return Reference to the live model. All calls to this method return a reference to the same object.
     */
    LiveModel& getLiveModel();

    /*!
     * \brief Get the LiveModel implementation.
     *
     * \return Pointer to LiveModel implementation.
     */
    LiveModel::Pimpl getLiveModelImpl();

    /*!
     * \brief Return the requested plug-in.
     */
    PluginContainer::Map::mapped_type getPlugin(
            const std::string& name         //!< [in] Plug-in name
            );

    /*!
     * \brief Get a list of all installed plug-ins.
     *
     * \return Vector of the successfully loaded plug-ins.
     */
    void getPluginList(
            Plugins& plugins  //!< [out] Vector of plug-ins
            );

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
    std::vector<Shape::Pimpl> getShapesForSize(
            const uint32_t size       //!< [in] Number of compute nodes
            );

    /*!
     * \brief Get list of all possible large shapes for the machine.
     *
     * \return Vector with all possible large shapes for the machine.
     */
    std::vector<Shape::Pimpl> getShapes();

    /*!
     * \brief Get list of valid rotated shapes.
     *
     * Note: Small block shapes can not be rotated so the same shape is returned.
     *
     * \return Vector of valid rotated shapes.
     */
    std::vector<Shape::Pimpl> getRotations(
            const Shape::Pimpl shape        //!< [in] Shape to rotate
            );

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
    Shape::Pimpl createMidplaneShape(
            const uint32_t a,     //!< [in] Midplane size in A dimension
            const uint32_t b,     //!< [in] Midplane size in B dimension
            const uint32_t c,     //!< [in] Midplane size in C dimension
            const uint32_t d      //!< [in] Midplane size in D dimension
            );

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
    Shape::Pimpl createSmallShape(
            const uint32_t size   //!< [in] Number of compute nodes
            );

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
            const Allocator& allocator,         //!< [in] Allocator object
            const Model& model,                 //!< [in] Model to search for available resources
            const ResourceSpec& resource,       //!< [in] Compute block resource requirements
            const std::string& plugin           //!< [in] Name of plug-in to use
            );

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
            const Allocator& allocator,        //!< [in] Allocator object
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
            const Allocator& allocator,       //!< [in] Allocator object
            Model& model,                     //!< [in] Model to update with deallocation
            const std::string& blockName,     //!< [in] Compute block to release
            const std::string& plugin         //!< [in] Name of plug-in to use
            );


    /*!
     * \brief Update the allocator hardware state.
     */
    void updateHardware(
            const ComputeHardware::Pimpl hardware   //!< [in] Compute hardware state
            );

    /*!
     * \brief Dump the allocator information to a stream (typically for debugging usage).
     */
   void dump(
           std::ostream& os,                                      //!< [in/out] Stream to write to
           EnumWrapper<Allocator::DumpVerbosity::Value> verbosity //!< [in] Verbosity level of output
           );

private:

	/*!
	 * \brief Builds a list of all the possible large block shapes for this machine.
	 *
	 * This uses the midplane dimensions of the machine to figure out the shapes.
	 */
    void buildShapesList();

    /*!
     * \brief Dumps a list of all the possible large block shapes for this machine.
     */
    void dumpShapesList(std::ostream& os);

	/*!
	 * \brief Add a shape of the specified midplane dimensions to the list of valid shapes for this machine.
	 */
    void addShape(
            const uint32_t a,     //!< [in] Midplane size in A dimension
            const uint32_t b,     //!< [in] Midplane size in B dimension
            const uint32_t c,     //!< [in] Midplane size in C dimension
            const uint32_t d      //!< [in] Midplane size in D dimension
            );

	/*!
	 * \brief Validate that the given shape fits in this machine.
	 *
	 * This does not check whether there are resources available, just whether a block of the given shape
	 * could exist on this machine.
	 */
    bool validateShape (
            const Shape::Pimpl shape  //!< [in] Shape to validate
            );

protected:

    typedef std::map< uint32_t, std::vector<Shape::Pimpl> > ValidShapes; //!< Container of valid shapes

protected:

    ComputeHardware::Pimpl _hardware;     //!< LiveModel compute hardware
    PluginContainer        _plugins;      //!< Plug-in container
    LiveModel::Pimpl       _liveModel;    //!< LiveModel pointer to implementation
    ValidShapes            _validShapes;  //!< A map of valid shapes for this machine, keyed by size
    LiveModel              _liveModelApi; //!< LiveModel object
    boost::shared_mutex    _mutex;        //!< Mutex for thread safety, allows many readers or a single writer

};

} // namespace bgsched::allocator
} // namespace bgsched

#endif
