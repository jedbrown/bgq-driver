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
 * \file bgsched/allocator/Plugin.h
 * \brief Plugin class definition.
 */

#ifndef BGSCHED_ALLOCATOR_PLUGIN_H_
#define BGSCHED_ALLOCATOR_PLUGIN_H_

#include <bgsched/allocator/Allocator.h>

#include <bgsched/Block.h>
#include <bgsched/Exception.h>

#include <iostream>
#include <string>

namespace bgsched {
namespace allocator {

class Model;
class ResourceSpec;

/*!
 * \brief Plug-in interface.
 *
 * This interface must be implemented by plug-ins.
 */
class Plugin
{
public:

    /*!
     * \brief Enumerated error codes for plug-in errors.
     */
    struct Errors
    {
        /*!
         * \brief Error codes.
         */
        enum Value
        {
            BlockResourcesUnavailable, //!< The block resources were not available to be allocated
            BlockDeallocateFailed,     //!< The block could not be deallocated
            FindBlockResourcesFailed,  //!< Unexpected error finding block resources
            PluginError                //!< An unhandled exception from the plug-in was caught by the Allocator
        };

        /*!
         * \brief Error message string.
         *
         * \return Error message string.
         */
        static std::string toString(
                Value v,                   //!< [in] Error value
                const std::string& what    //!< [in] Descriptive string
                );

    };

    /*!
     * \brief Exception inherits from scheduler exception.
     *
     * This is the only type of exception thrown by the Allocator call to a plug-in method.
     * All exceptions thrown by a plug-in implementation will be converted to this type.
     */
    typedef RuntimeError<Errors> Exception;

public:

    /*!
     * \brief
     */
    virtual ~Plugin() { };

    /*!
     * \brief Get plug-in description.
     */
    virtual void description(
            std::ostream& os //!< [in/out] Stream to write description to
    ) = 0;

    /*!
     * \brief Find available resources to satisfy the compute block resource specification.
     *
     * No resources are allocated by this method, the model remains unchanged.
     *
     * Resources can be allocated by passing the block object to the prepare method.
     *
     * The plug-in implementation should throw a bgsched::allocator::Plugin::Exception with the following value:
     * - bgsched::allocator::Plugin::Errors::FindBlockResourcesFailed - unexpected error finding block resources
     *
     * \return Block object which satisfies the resource specification or a NULL Block::Ptr
     * if no resources are available matching resource specification.
     */
    virtual Block::Ptr findBlockResources(
            const Allocator& allocator,      //!< [in] Allocator
            const Model& model,              //!< [in] Model to search for available resources
            const ResourceSpec& resourceSpec //!< [in] Compute block resource requirements
    ) = 0;

    /*!
     * \brief Reserve the hardware resources specified by the compute block object, marking them as being in use.
     *
     * The compute block may be obtained from the findBlockResources() method implemented by
     * a plug-in or by using one of the static Block::create() methods.
     *
     * For the live model, this method call is asynchronous. If the LiveModel::allocate method is called, the
     * registered listeners will be notified when the compute block is ready to run jobs. If the plug-in
     * implementation does not call the LiveModel::allocate method, then it must call the LiveModel::notifyAllocated
     * method when the block is ready to run jobs, or if there is a failure.
     *
     * The plug-in implementation should throw a bgsched::allocator::Plugin::Exception with the following value:
     * - bgsched::allocator::Plugin::Errors::BlockResourcesUnavailable - block resources are not available for allocation
     */
    virtual void prepare(
            const Allocator& allocator,   //!< [in] Allocator
            Model& model,                 //!< [in] Model to update with allocation
            const std::string& blockName  //!< [in] Compute block to prepare
    ) = 0;

    /*!
     * \brief Deallocates the hardware resources specified by the compute block object.
     *
     * For the live model, this method call is asynchronous. If the LiveModel::deallocate method is called, the
     * registered listeners will be notified when the compute block is ready to be allocated for another request.
     * If the plug-in implementation does not call the LiveModel::deallocate method, then it must call the
     * LiveModel::notifyDeallocated method when the compute block resources are available to be allocated to
     * another request, or if there is a failure.
     *
     * The plug-in implementation should throw a bgsched::allocator::Plugin::Exception with the following value:
     * - bgsched::allocator::Plugin::Errors::BlockDeallocateFailed - block could not be deallocated (e.g. the block was not found)
     */
    virtual void release(
            const Allocator& allocator,   //!< [in] Allocator
            Model& model,                 //!< [in] Model to update with deallocation
            const std::string& blockName  //!< [in] Compute block to release
    ) = 0;
};

} // namespace bgsched::allocator
} // namespace bgsched

extern "C" {

/*!
 * \brief Declaration of library method to create a Plugin.
 *
 * \attention This symbol must be defined in the plug-in library.
 *
 * \return Pointer to a Plugin object.
 */
bgsched::allocator::Plugin* create();

/*!
 * \brief Declaration of a library method to destroy a Plugin.
 *
 * \attention This symbol must be defined in the plug-in library.
 */
void destroy(
        bgsched::allocator::Plugin* plugin //!< [in] Pointer to the Plugin to destroy
);

} // extern "C"

#endif
