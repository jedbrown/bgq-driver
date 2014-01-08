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

#include <bgsched/allocator/InputException.h>

#include "bgsched/allocator/ResourceSpecImpl.h"

#include <bgsched/Coordinates.h>
#include <bgsched/InternalException.h>
#include <bgsched/Midplane.h>

#include "bgsched/ShapeImpl.h"
#include "bgsched/utility.h"

#include <bgsched/core/core.h>

#include <utility/include/Log.h>

using namespace bgsched;
using namespace bgsched::allocator;
using namespace std;

LOG_DECLARE_FILE("bgsched");

namespace {
    // Message strings
    const string Invalid_Dimension_Str("Dimension must be A, B, C or D.");
    const string Invalid_Mesh_Connectivity_Str("Connectivity Mesh specified for a dimension that requires Torus.");
    const string Invalid_Torus_Connectivity_Str("Connectivity Torus specified for a dimension that requires Mesh.");
    const string DB_Access_Error_Str("Error occurred while accessing database.");
} // anonymous namespace

namespace bgsched {
namespace allocator {

ResourceSpec::Impl::Impl(
        const Shape& shape,
        bool  canRotateShape,
        bool  canUsePassthrough
        ) :
    _shape(new Shape(shape)), // makes its own copy of the shape
    _canRotateShape(canRotateShape),
    _canUsePassthrough(canUsePassthrough),
    _connectivitySpec(),
    _extendedOptions(),
    _drainedMidplanes(),
    _drainedMidplaneMutex()
{
    // Set default connectivity spec dimensions
    for( Dimension dimension = Dimension::A; dimension <= Dimension::D; ++dimension )
        _connectivitySpec[dimension] = ConnectivitySpec::Either;
}

ResourceSpec::Impl::Impl(
        const Impl& impl
        ) :
    _shape(new Shape(*(impl._shape))),
    _canRotateShape(impl._canRotateShape),
    _canUsePassthrough(impl._canUsePassthrough),
    _connectivitySpec(),
    _extendedOptions(),
    _drainedMidplanes(),
    _drainedMidplaneMutex()
{
    // Copy connectivity dimensions
    for( Dimension dimension = Dimension::A; dimension <= Dimension::D; ++dimension )
        _connectivitySpec[dimension] = impl._connectivitySpec[dimension];

    // Copy extended options map
    _extendedOptions = impl._extendedOptions;

    // Copy drained midplanes
    _drainedMidplanes = impl._drainedMidplanes;
}

bgsched::Shape::ConstPtr
ResourceSpec::Impl::getShape() const
{
    return _shape;
}

bool
ResourceSpec::Impl::canRotateShape() const
{
    // Shape rotation is for large shapes (blocks) only
    if (_shape->isSmall()) {
        return false;
    } else {
        return _canRotateShape;
    }
}

bool
ResourceSpec::Impl::canUsePassthrough() const
{
    // Pass-through is for large shapes (blocks) only
    if (_shape->isSmall()) {
        return false;
    } else {
        return _canUsePassthrough;
    }
}

uint32_t
ResourceSpec::Impl::getNodeCount() const
{
    return _shape->getNodeCount();
}

ResourceSpec::DrainedMidplanes
ResourceSpec::Impl::getDrainedMidplanes()
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain shared read lock");
    // Lock mutex as shared read by multiple threads, mutex is automatically unlocked when stack is unwound
    boost::shared_lock<boost::shared_mutex> lock(_drainedMidplaneMutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained shared read lock");
    return _drainedMidplanes;
}

void
ResourceSpec::Impl::addDrainedMidplane(
        const string& midplaneLocation
        )
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock");
    // Lock mutex as single writer across multiple threads, mutex is automatically unlocked when stack is unwound
    boost::unique_lock<boost::shared_mutex> lock(_drainedMidplaneMutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock");

    // Add the midplane location to the drain list
    if (! _drainedMidplanes.insert(midplaneLocation).second) {
        LOG_WARN_MSG("Midplane " <<  midplaneLocation << " already exists in drain list");
    } else {
        LOG_TRACE_MSG("Added midplane " << midplaneLocation << " to resource spec drain list");
    }
}

void
ResourceSpec::Impl::removeDrainedMidplane(
        const string& midplaneLocation
        )
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock");
    // Lock mutex as single writer across multiple threads, mutex is automatically unlocked when stack is unwound
    boost::unique_lock<boost::shared_mutex> lock(_drainedMidplaneMutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock");

    // Remove the midplane location from the drain list
    if (_drainedMidplanes.erase(midplaneLocation) == 0) {
        LOG_WARN_MSG("Midplane " << midplaneLocation << " does not exist in drain list");
    } else {
        LOG_TRACE_MSG("Removed midplane " << midplaneLocation << " from resource spec drain list");
    }
}

ResourceSpec::ExtendedOptions
ResourceSpec::Impl::getExtendedOptions() const
{
    return _extendedOptions;
}

void
ResourceSpec::Impl::addExtendedOption(
        const string& key,
        const string& value
        )
{
    ExtendedOptions::iterator iter;
    iter = _extendedOptions.find(key);
    // Check if extended option is already in the map
    if (iter == _extendedOptions.end()) {
        // Not in map so just add it
        _extendedOptions.insert(make_pair(key,value));
    } else {
        // Already in map so remove original entry and replace with new entry
        _extendedOptions.erase(iter);
        _extendedOptions.insert(make_pair(key,value));
    }
}

void
ResourceSpec::Impl::removeExtendedOption(
        const string& key
        )
{
    ExtendedOptions::iterator iter;
    iter = _extendedOptions.find(key);
    // Check if extended option is really in the map
    if (iter != _extendedOptions.end()) {
        // In the map so erase it
        _extendedOptions.erase(iter);
    }
}

ResourceSpec::ConnectivitySpec::Value
ResourceSpec::Impl::getConnectivitySpec(
        const bgsched::Dimension& i
        ) const
{
    if ((i == Dimension(Dimension::A)) || (i == Dimension(Dimension::B)) || (i == Dimension(Dimension::C)) || (i == Dimension(Dimension::D))) {
        return _connectivitySpec[i];
    } else {
        THROW_EXCEPTION(
                bgsched::allocator::InputException,
                bgsched::allocator::InputErrors::InvalidDimension,
                Invalid_Dimension_Str
        );
    }
}

void
ResourceSpec::Impl::setConnectivitySpec(
        const bgsched::Dimension& i,
        ConnectivitySpec::Value connectivitySpec
        )
{
    if (_shape->isLarge()) {
        if ((i == Dimension(Dimension::A)) || (i == Dimension(Dimension::B)) || (i == Dimension(Dimension::C)) || (i == Dimension(Dimension::D))) {
            // Can't be Mesh if single midplane in the dimension
            if (_shape->getMidplaneSize(i) == 1) {
                if (connectivitySpec == ConnectivitySpec::Mesh) {
                    LOG_ERROR_MSG("Dimension " << string(i) << " is size 1 but the connectivity is Mesh");
                    THROW_EXCEPTION(
                            bgsched::allocator::InputException,
                            bgsched::allocator::InputErrors::InvalidConnectivity,
                            Invalid_Mesh_Connectivity_Str
                    );
                } else {
                    // Connectivity specified is Torus or Either so set the connectivity
                    _connectivitySpec[i] = connectivitySpec;
                    return;
                }
            }

            // Get the machine size
            Coordinates coordinates(0,0,0,0);
            try {
                coordinates = bgsched::core::getMachineSize();
            } catch (...) {
                THROW_EXCEPTION(
                        bgsched::InternalException,
                        bgsched::InternalErrors::UnexpectedError,
                        DB_Access_Error_Str
                );
            }

            // Check if Shape of resource spec represents full machine block (must be Torus in all dimensions)
            if (coordinates[Dimension::A] == _shape->getMidplaneSize(Dimension::A) &&
                    coordinates[Dimension::B] == _shape->getMidplaneSize(Dimension::B) &&
                    coordinates[Dimension::C] == _shape->getMidplaneSize(Dimension::C) &&
                    coordinates[Dimension::D] == _shape->getMidplaneSize(Dimension::D)) {
                // Can't be Mesh if full machine block
                if (connectivitySpec == ConnectivitySpec::Mesh) {
                    THROW_EXCEPTION(
                            bgsched::allocator::InputException,
                            bgsched::allocator::InputErrors::InvalidConnectivity,
                            Invalid_Mesh_Connectivity_Str
                    );
                } else {
                    // Connectivity specified is Torus or Either so set the connectivity
                    _connectivitySpec[i] = connectivitySpec;
                    return;
                }
            }

            // Check if shape dimension is equal to the size of the machine in that dimension (must be Torus)
            if (coordinates[i] == _shape->getMidplaneSize(i)) {
                // Can't be Mesh if dimension size equals machine size in the dimension
                if (connectivitySpec == ConnectivitySpec::Mesh) {
                    THROW_EXCEPTION(
                            bgsched::allocator::InputException,
                            bgsched::allocator::InputErrors::InvalidConnectivity,
                            Invalid_Mesh_Connectivity_Str
                    );
                } else {
                    // Connectivity specified is Torus or Either so set the connectivity
                    _connectivitySpec[i] = connectivitySpec;
                    return;
                }
            }

            // Check if given shape dimension is less than the size of the machine in that dimension.
            if (_shape->getMidplaneSize(i) < coordinates[i] && connectivitySpec == ConnectivitySpec::Torus) {
                // Torus can't be specified in this case unless pass-through is allowed
                if (canUsePassthrough() == false) {
                    // Connectivity must be Mesh for dimension
                    THROW_EXCEPTION(
                            bgsched::allocator::InputException,
                            bgsched::allocator::InputErrors::InvalidConnectivity,
                            Invalid_Torus_Connectivity_Str
                    );
                }
            }
            // Passed all the rules so set the connectivity
            _connectivitySpec[i] = connectivitySpec;
        } else {
            THROW_EXCEPTION(
                    bgsched::allocator::InputException,
                    bgsched::allocator::InputErrors::InvalidDimension,
                    Invalid_Dimension_Str
            );
        }
    } else {
        // This is a small shape so just ignore any request to set connectivity since connectivity is
        // predetermined for small shapes.
        LOG_WARN_MSG( "Request to set connectivity for resource spec representing a small block is ignored.");
    }
}

} // namespace bgsched::allocator
} // namespace bgsched
