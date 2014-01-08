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
 * \file bgsched/allocator/ModelImpl.h
 * \brief Model::Impl class definition.
 */

#ifndef BGSCHED_ALLOCATOR_MODEL_IMPL_H_
#define BGSCHED_ALLOCATOR_MODEL_IMPL_H_

#include <bgsched/allocator/Allocator.h>
#include <bgsched/allocator/HardwareException.h>
#include <bgsched/allocator/Model.h>
#include <bgsched/allocator/ResourceAllocationException.h>
#include <bgsched/allocator/ResourceSpec.h>

#include <bgsched/Block.h>
#include <bgsched/Coordinates.h>
#include <bgsched/Job.h>
#include <bgsched/Midplane.h>
#include <bgsched/Switch.h>

#include "bgsched/ComputeHardwareImpl.h"
#include "bgsched/BlockImpl.h"

#include <boost/scoped_ptr.hpp>
#include <boost/thread/shared_mutex.hpp>

#include <map>
#include <string>
#include <vector>

namespace bgsched {
namespace allocator {

/*!
 * \brief Represents Blue Gene blocks and hardware.
 */
class Model::Impl
{
public:

    /*!
     * \brief
     */
    Impl(
            Allocator::Impl* allocator,     //!< [in] Pointer to allocator implementation
            ComputeHardware::Pimpl hardware //!< [in] Pointer to compute hardware implementation
    );

    /*!
     * \brief
     */
    virtual ~Impl();

    /*!
     * \brief Initiate the allocating (booting) of a compute block.
     *
     * The compute block may be obtained from the findBlockResources() method implemented
     * by a plug-in or one of the static Block::create methods on the Block class.
     */
    virtual void allocate(
            const std::string& blockName   //!< [in] Compute block to allocate
    ) = 0;

    /*!
     * \brief Initiate the deallocating (freeing) of a compute block.
     */
    virtual void deallocate(
            const std::string& blockName   //!< [in] Compute block to deallocate
    ) = 0 ;

    /*!
     * \brief Add a block to the model, but do not allocate it.
     */
    virtual void addBlock(
            const Block::Pimpl block,  //!< [in] Compute block to add
            const std::string& owner   //!< [in] Compute block owner
    ) = 0;

    /*!
     * \brief Remove a a block from the model.
     */
    virtual void removeBlock(
            const std::string& blockName  //!< [in] Compute block name to remove
    ) = 0;

    /*!
     * \brief Get a compute block from the model.
     *
     * \return Compute block for the given name.
     */
    virtual const Block::Pimpl getBlock (
            const std::string& blockName  //!< [in] Compute block to retrieve info on
    ) = 0;

    /*!
     * \brief Get compute block status from the model.
     *
     * \return Compute block status from the model.
     */
    virtual EnumWrapper<Block::Status> getBlockStatus(
            const std::string& blockName  //!< [in] Compute block to retrieve status on
    ) = 0;

    /*!
     * \brief Synchronize the model hardware state.
     */
    virtual void syncState() = 0;

    /*!
     * \brief Dump the model information to a stream (typically for debugging usage).
     */
    virtual void dump(
            std::ostream& os,                                   //!< [in/out] Stream to write to
            EnumWrapper<Model::DumpVerbosity::Value> verbosity  //!< [in] Verbosity level of output
    ) = 0;

    /*!
     * \brief Get compute hardware resources for the model.
     *
     * \return Compute hardware resources.
     */
    virtual ComputeHardware::Pimpl getComputeHardware();

    /*!
     * \brief Get all midplanes.
     *
     * \return All midplanes.
     */
    virtual Midplanes::Ptr getMidplanes();

    /*!
     * \brief Get midplane at specific coordinate.
     *
     * \return Midplane at specific coordinate.
     */
    virtual Midplane::Ptr getMidplane(
            const bgsched::Coordinates& coordinates  //!< [in] Coordinates to retrieve midplane for
    );

    /*!
     * \brief Get midplane at specific location.
     *
     * \return Midplane at specific location.
     */
    virtual Midplane::Ptr getMidplane(
            const std::string& location  //!< [in] Midplane location like R00-M0
    );

    /*!
     * \brief Get list of drained midplanes for the model. Midplanes in the drain list are listed by location.
     *
     * \return List of drained midplanes for the model.
     */
    virtual Model::DrainedMidplanes getDrainedMidplanes();

    /*!
     * \brief Add drained midplane for the model. The drain list consists of midplane locations to exclude
     * when searching for block resources.
     */
    virtual void addDrainedMidplane(
            const std::string& midplaneLocation   //!< [in] Midplane location to add to drain list
    );

    /*!
     * \brief Remove drained midplane for the model. The drain list consists of midplane locations to exclude
     * when searching for block resources.
     */
    virtual void removeDrainedMidplane(
            const std::string& midplaneLocation   //!< [in] Midplane location to remove from drain list
    );

protected:

    Allocator::Impl*        _allocator;            //!< Reference to owning allocator
    ComputeHardware::Pimpl  _hardware;             //!< Reference to compute hardware
    boost::shared_mutex     _mutex;                //!< Thread safety allows many readers or a single writer
    Model::DrainedMidplanes _drainedMidplanes;     //!< Collection of drained midplanes
    boost::shared_mutex     _drainedMidplaneMutex; //!< Thread safety allows many readers or a single writer

};

} // namespace bgsched::allocator
} // namespace bgsched

#endif
