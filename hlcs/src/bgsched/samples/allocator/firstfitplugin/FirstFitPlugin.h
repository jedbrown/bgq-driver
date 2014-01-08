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

#ifndef IBM_FIRST_FIT_PLUGIN_H_
#define IBM_FIRST_FIT_PLUGIN_H_

#include <bgsched/Block.h>
#include <bgsched/Midplane.h>

#include <bgsched/allocator/Model.h>
#include <bgsched/allocator/Plugin.h>

#include <iosfwd>
#include <map>

namespace IBMFirstFit {

/*!
 * \brief IBM First Fit plug-in implementation.
 *
 * Provides implementation of findBlockResources() with simple first fit behavior.
 */
class FirstFitPlugin : public bgsched::allocator::Plugin
{
public:

    /*!
     * \brief
     */
    FirstFitPlugin();

    /*!
     * \brief
     */
    ~FirstFitPlugin();

    /*!
     * \brief Get plug-in description.
     */
    void description(
            std::ostream& os //!< [in] Stream to write the description to
            );

    /*!
     * \brief Find available resources to satisfy the compute block resource specification.
     *
     * This implementation will perform a first fit based on the resource requirements.
     *
     * No resources are allocated by this method, the model remains unchanged.
     *
     * Resources can be allocated by passing the block object to the prepare method.
     *
     * The block returned will have hardware resources set as well as a default block name and
     * block description. The caller can modify any of the settable block fields.
     *
     * Note: Pass-through blocks are not supported by the First Fit plug-in and the resource spec setting
     * to allow pass-through is ignored.
     *
     * \throws bgsched::allocator::Plugin::Exception with the following value:
     * - bgsched::allocator::Plugin::Errors::FindBlockResourcesFailed - unexpected error finding block resources
     *
     * \return Block object which satisfies the resource specification or a NULL Block::Ptr
     * if no resources are available matching resource specification.
     */
    bgsched::Block::Ptr findBlockResources(
            const bgsched::allocator::Allocator& allocator,      //!< [in] Allocator
            const bgsched::allocator::Model& model,              //!< [in] Model to search for available resources
            const bgsched::allocator::ResourceSpec& resourceSpec //!< [in] Compute block resource requirements
            );

    /*!
     * \brief Reserve the hardware resources specified by the compute block object, marking them as being in use.
     *
     * The compute block may be obtained from the findBlockResources() method implemented by
     * a plug-in or by using one of the static Block::create() methods.
     *
     * For the live model, registered listeners will be notified when the block is available to run jobs, or if there is a failure.
     *
     * \throws bgsched::allocator::InputException with value:
     * - bgsched::allocator::InputErrors::PluginNotFound - the plug-in name does not match a loaded plug-in
     *
     * \throws bgsched::allocator::Plugin::Exception with value:
     * - bgsched::allocator::Plugin::Errors::BlockResourcesUnavailable - block resources are not available for allocation
     */
    void prepare(
            const bgsched::allocator::Allocator& allocator, //!< [in] Allocator
            bgsched::allocator::Model& model,               //!< [in] Model to update with allocation
            const std::string& blockName                    //!< [in] Compute block to prepare
            );

    /*!
     * \brief Deallocates the hardware resources specified by the compute block object.
     *
     * For the live model, registered listeners will be notified when the compute block resources are available to be
     * allocated to another request, or if there is a failure.
     *
     * \throws bgsched::allocator::InputException with value:
     * - bgsched::allocator::InputErrors::PluginNotFound - the plug-in name does not match a loaded plug-in
     *
     * \throws bgsched::allocator::Plugin::Exception with value:
     * - bgsched::allocator::Plugin::Errors::BlockDeallocateFailed - block could not be deallocated (e.g. the block was not found)
     */
    void release(
            const bgsched::allocator::Allocator& allocator,  //!< [in] Allocator
            bgsched::allocator::Model& model,                //!< [in] Model to update with deallocation
            const std::string& blockName                     //!< [in] Compute block to release
            );
};

} // namespace IBMFirstFit


#endif
