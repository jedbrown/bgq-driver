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
 * \file bgsched/allocator/ResourceSpec.h
 * \brief ResourceSpec class definition.
 */

#ifndef BGSCHED_ALLOCATOR_RESOURCE_SPEC_H_
#define BGSCHED_ALLOCATOR_RESOURCE_SPEC_H_

#include <bgsched/Dimension.h>
#include <bgsched/EnumWrapper.h>
#include <bgsched/Shape.h>
#include <bgsched/types.h>

#include <boost/shared_ptr.hpp>

#include <map>
#include <set>
#include <string>

namespace bgsched {
namespace allocator {

/*!
 * \brief Contains resource specification parameters for findBlockResources().
 *
 * These correspond to the requirements given for the job to be run on the resources.
 * The methods defined here are available to all plug-ins.
 *
 * \sa Allocator
 * \sa Plugin
 *
 */
class ResourceSpec
{
public:

    typedef boost::shared_ptr<ResourceSpec> Ptr;                  //!< Pointer type.
    typedef boost::shared_ptr<const ResourceSpec> ConstPtr;       //!< Const Pointer type.

    typedef std::map<std::string, std::string> ExtendedOptions;   //!< Map of extended options.
    typedef std::set<std::string> DrainedMidplanes;               //!< Collection of drained midplanes.

    /*!
     * \brief Each dimension can be connected as either a Torus or a Mesh.
     */
    struct ConnectivitySpec
    {
        enum Value {
            Torus = 0,  //!< Block must be Torus in this dimension
            Mesh,       //!< Block must be a Mesh in this dimension
            Either      //!< Prefer a Torus, but if resources are not available, block may be a Mesh in this dimension
        };

        /*!
         *  \brief Convert a ResourceSpec::ConnectivitySpec::Value to a string.
         */
        static const std::string& toString(Value connectivity);
    };

    /*!
     * \brief Returns the resource specification Shape.
     *
     * \return Resource specification Shape.
     */
    bgsched::Shape::ConstPtr getShape() const;

    /*!
     * \brief Indicates if plug-in can rotate the Shape when searching for available resources.
     *
     * Only large block shapes are allowed to be rotated so false is returned for small block shapes.
     *
     * \return True if the Allocator plug-in provider is allowed to rotate the Shape.
     */
    bool canRotateShape() const;

    /*!
     * \brief Indicates if plug-in can use pass-through midplanes to construct a large block.
     *
     * Only large block shapes representing multiple midplanes can use pass-through midplanes
     * so returns false for small block shapes.
     *
     * \return True if the Allocator plug-in provider is allowed to use pass-through midplanes
     * to construct a large block.
     */
    bool canUsePassthrough() const;

    /*!
     * \brief Get the number of compute nodes.
     *
     * \return Number of compute nodes.
     */
    uint32_t getNodeCount() const;

    /*!
     * \brief For large blocks return the desired connectivity for a block in the specified dimension.
     *
     * When set to ConnectivitySpec::Torus, the block returned will be a torus in the specified
     * dimension. If the necessary pass-through switches are not available, the
     * request for resources will fail. When set to ConnectivitySpec::Mesh, the block will be
     * a mesh.  When set to ConnectivitySpec::Either, a torus will be used when available,
     * otherwise a mesh will be returned. For small blocks, there is a fixed
     * set of torus/mesh configurations.
     *
     * \throws bgsched::allocator::InputException with value:
     * - bgsched::allocator::InputErrors::InvalidDimension - if Dimension is not A, B, C or D
     *
     * \return Torus or mesh or either specification.
     */
    ConnectivitySpec::Value getConnectivitySpec(
            const bgsched::Dimension& dimension  //!< [in] Dimension A,B,C,D
            ) const;

    /*!
     * \brief For large blocks set the connectivity specification for a given dimension.
     *
     * For resource specs representing small blocks any attempt to set the connectivity is ignored.
     * Small blocks have specific hardened connectivity that can not be modified.
     *
     * If the shape consists of all the machine midplanes (full block) or a single midplane then the
     * connectivity must be Torus (or Either) for all dimensions. Any dimension with a single midplane
     * must also be Torus (or Either). Any dimension that is equal to the size of the machine in that
     * dimension must also be Torus (or Either).
     *
     * If a given shape dimension is less than the size of the machine in that dimension then Mesh must be specified.
     * The only exception to that requirement is if pass-through is allowed, in that situation Torus connectivity is permitted.
     *
     * When set to Either on findBlockResources(), the block returned will be a torus in this dimension,
     * unless resources were not found to create a torus, in which case, the block returned will be a
     * mesh in this dimension.
     *
     * Note: When rotating the shape is allowed, the corresponding dimension connectivity specification is also rotated.
     *
     * \throws bgsched::allocator::InputException with values:
     * - bgsched::allocator::InputErrors::InvalidDimension - if dimension is not A, B, C or D
     * - bgsched::allocator::InputErrors::InvalidConnectivity - if connectivity Mesh is specified for a dimension that requires Torus
     *   or if Torus is specified for a dimension that requires Mesh
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::UnexpectedError - if error accessing database occurs
     */
    void setConnectivitySpec(
            const bgsched::Dimension& dimension,      //!< [in] Dimension to set
            ConnectivitySpec::Value connectivitySpec  //!< [in] Connectivity spec (Torus, Mesh, Either) to set
    );

    /*!
     * \brief Get list of drained midplanes. Midplanes in the drain list are listed by location.
     *
     * \return List of drained midplanes.
     */
    ResourceSpec::DrainedMidplanes getDrainedMidplanes() const;

    /*!
     * \brief Add drained midplane to the resource spec. The drain list consists of midplane locations to exclude
     * when searching for compute block resources.
     */
    void addDrainedMidplane(
            const std::string& midplaneLocation   //!< [in] Midplane location to add to drain list
    );

    /*!
     * \brief Remove drained midplane from the resource spec. The drain list consists of midplane locations to exclude
     * when searching for compute block resources.
     */
    void removeDrainedMidplane(
            const std::string& midplaneLocation   //!< [in] Midplane location to remove from drain list
    );

    /*!
     * \brief Return a map of extended options (key/value pairs).
     *
     * Extended options are provided as a mechanism to communicate additional information to plug-in providers.
     * No standard set of extended options exist so an Allocator plug-in may simply choose to ignore these.
     *
     * \return Map of extended options (key/value pairs).
     */
    ExtendedOptions getExtendedOptions() const;

    /*!
     * \brief Add an extended option (key/value pair) to the resource spec.
     *
     * Extended options are provided as a mechanism to communicate additional information to plug-in providers.
     * No standard set of extended options exist so an Allocator plug-in may simply choose to ignore these.
     *
     * If an extended option with the same key exists in the resource spec it is replaced.
     */
    void addExtendedOption(
            const std::string& key,     //!< [in] Extended option key (string)
            const std::string& value    //!< [in] Extended option value (string)
    );

    /*!
     * \brief Remove an extended option (key/value pair) from the resource spec.
     *
     * Extended options are provided as a mechanism to communicate additional information to plug-in providers.
     * No standard set of extended options exist so an Allocator plug-in may simply choose to ignore these.
     *
     * If the extended option is not found in the resource spec the request is ignored.
     */
    void removeExtendedOption(
            const std::string& key     //!< [in] Extended option key (string)
    );

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
     *
     * Note: When rotating the shape is allowed, the corresponding dimension connectivity specification is also rotated.
     *
     * Note: Shape rotation and pass-through usage settings are ignored when constructing resource spec for small blocks.
     */
    explicit ResourceSpec(
            const bgsched::Shape& shape,   //!< [in] Shape in compute nodes (small block) or midplanes (large block)
            bool  canRotateShape = false,  //!< [in] Indication if can rotate Shape (large blocks only)
            bool  canUsePassthrough = true //!< [in] Indication if can use pass-through midplanes (large blocks only)
            );

    /*!
     * \brief Copy constructor.
     */
    explicit ResourceSpec(
            const ResourceSpec& resourceSpec  //!< [in] ResourceSpec to copy from
            );

    /*!
     * \brief
     */
    explicit ResourceSpec(
            Pimpl impl //!< [in] Pointer to implementation
            );

protected:

    Pimpl _impl;

};

} // namespace bgsched::allocator
} // namespace bgsched

#endif
