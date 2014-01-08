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
 * \file bgsched/allocator/Model.h
 * \brief Model class definition.
 */

#ifndef BGSCHED_ALLOCATOR_MODEL_H_
#define BGSCHED_ALLOCATOR_MODEL_H_

#include <bgsched/allocator/Midplanes.h>
#include <bgsched/allocator/ResourceSpec.h>

#include <bgsched/Block.h>
#include <bgsched/Coordinates.h>
#include <bgsched/Hardware.h>
#include <bgsched/Job.h>
#include <bgsched/Midplane.h>

#include <boost/shared_ptr.hpp>

#include <map>
#include <set>
#include <string>
#include <vector>


namespace bgsched {

class BlockFilter;

namespace allocator {

/*!
 * \brief Represents Blue Gene blocks and hardware.
 */
class Model
{
public:

    /*!
     * \brief Dump verbosity level.
     */
    struct DumpVerbosity {
        enum Value {
            Hardware = 0,   //!< Dump compute hardware summary
            Blocks,         //!< Dump summary list of blocks in brief format
            BlocksExtended, //!< Dump summary list of blocks in extended format
            DrainList       //!< Dump midplane drain list
        };
    };

    typedef boost::shared_ptr<Model> Ptr;           //!< Pointer type.

    typedef std::set<std::string> DrainedMidplanes; //!< Collection of drained midplanes.

    /*!
     * \brief Initiate the allocating (booting) of a compute block.
     *
     * The compute block object may be obtained from the findBlockResources() method implemented
     * by a plug-in or one of the static Block::create methods on the Block class.
     */
    virtual void allocate(
            const std::string& blockName  //!< [in] Compute block to allocate
            ) = 0;

    /*!
     * \brief Initiate the deallocating (freeing) of a compute block.
     */
    virtual void deallocate(
            const std::string& blockName  //!< [in] Compute block to deallocate
            ) = 0;

    /*!
     * \brief Add a block to the model, but do not allocate it.
     */
    virtual void addBlock(
            const Block::Ptr block,   //!< [in] Compute block to add
            const std::string& owner  //!< [in] Compute block owner
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
    virtual const Block::Ptr getBlock(
            const std::string& blockName //!< [in] Compute block to retrieve info on
            ) const = 0;

    /*!
     * \brief Get compute block status from the model.
     *
     * \return Compute block status from the model.
     */
    virtual EnumWrapper<Block::Status> getBlockStatus(
            const std::string& blockName  //!< [in] Compute block to retrieve status on
            ) const = 0;

    /*!
     * \brief Get all midplanes.
     *
     * \return All midplanes.
     */
    virtual Midplanes::ConstPtr getMidplanes() const = 0;

    /*!
     * \brief Return midplane at specific coordinate.
     *
     * \return Midplane at specific coordinate.
     */
    virtual Midplane::Ptr getMidplane(
            const bgsched::Coordinates& coordinates  //!< [in] Coordinates to retrieve midplane for
            ) const = 0;

    /*!
     * \brief Get midplane at specific location.
     *
     * \return Midplane at specific location.
     */
    virtual Midplane::Ptr getMidplane(
            const std::string& location //!< [in] Midplane location like R00-M0
            ) const = 0;

    /*!
     * \brief Get list of drained midplanes for the model. Midplanes in the drain list are listed by location.
     *
     * \return List of drained midplanes for the model.
     */
    virtual Model::DrainedMidplanes getDrainedMidplanes() const = 0;

    /*!
     * \brief Add drained midplane for the model. The drain list consists of midplane locations to exclude
     * when searching for block resources.
     */
    virtual void addDrainedMidplane(
            const std::string& midplaneLocation   //!< [in] Midplane location to add to drain list
    ) = 0;

    /*!
     * \brief Remove drained midplane for the model. The drain list consists of midplane locations to exclude
     * when searching for block resources.
     */
    virtual void removeDrainedMidplane(
            const std::string& midplaneLocation   //!< [in] Midplane location to remove from drain list
    ) = 0;

    /*!
     * \brief Synchronize the model hardware state.
     */
    virtual void syncState() = 0;

    /*!
     * \brief Dump the model information to a stream (typically for debugging usage).
     */
    virtual void dump(
           std::ostream& os,                                  //!< [in/out] Stream to write to
           EnumWrapper<Model::DumpVerbosity::Value> verbosity //!< [in] Verbosity level of output
           ) const = 0;

    /*!
     * \brief
     */
    virtual ~Model() = 0;

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

protected:

    /*!
     * \brief
     */
    explicit Model(
            Pimpl impl  //!< [in] Pointer to implementation
    );

    Pimpl _impl;

};

} // namespace bgsched::allocator
} // namespace bgsched

#endif
