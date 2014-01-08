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
#include <bgsched/allocator/InternalException.h>
#include <bgsched/allocator/LiveModel.h>
#include <bgsched/allocator/Model.h>
#include <bgsched/allocator/Plugin.h>
#include <bgsched/allocator/ShapeException.h>

#include "bgsched/allocator/AllocatorImpl.h"
#include "bgsched/allocator/PluginContainer.h"

#include <bgsched/bgsched.h>
#include <bgsched/ComputeHardware.h>
#include <bgsched/SchedUtil.h>

#include "bgsched/ComputeHardwareImpl.h"
#include "bgsched/ShapeImpl.h"
#include "bgsched/utility.h"

#include <bgsched/core/core.h>

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>
#include <boost/thread/shared_mutex.hpp>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <iterator>

using namespace bgsched;
using namespace bgsched::allocator;
using namespace std;

LOG_DECLARE_FILE("bgsched");

namespace bgsched {
namespace allocator {

Allocator::Impl::Impl() :
    // Initialize hardware from the database. Synchronized from the LiveModel syncState() method.
    _hardware(ComputeHardware::Pimpl(new ComputeHardware::Impl())),
    _plugins(),
    _liveModel(LiveModel::Pimpl(new LiveModel::Impl(this, _hardware))),
    _validShapes(),
    _liveModelApi(_liveModel),
    _mutex()
{
    // Build list of valid shapes
    buildShapesList();
}

LiveModel&
Allocator::Impl::getLiveModel()
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain shared read lock");
    // Lock mutex as shared read by multiple threads, mutex is automatically unlocked when stack is unwound
    boost::shared_lock<boost::shared_mutex> lock(_mutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained shared read lock");

    return _liveModelApi;
}

LiveModel::Pimpl
Allocator::Impl::getLiveModelImpl()
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain shared read lock");
    // Lock mutex as shared read by multiple threads, mutex is automatically unlocked when stack is unwound
    boost::shared_lock<boost::shared_mutex> lock(_mutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained shared read lock");

    return _liveModel;
}

void
Allocator::Impl::dump(
        ostream& os,
        EnumWrapper<Allocator::DumpVerbosity::Value> verbosity
        )
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock");
    // Lock mutex as single writer across multiple threads, mutex is automatically unlocked when stack is unwound
    boost::unique_lock<boost::shared_mutex> lock(_mutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock");

    // Check verbosity level on what to dump
    switch (verbosity.toValue()) {
      case Allocator::DumpVerbosity::BlockShapes:
        // Dump list of base shapes for allocator
        dumpShapesList(os);
        break;
      default:
        LOG_WARN_MSG("Unknown dump verbosity level");
        break;
    }
}

void
Allocator::Impl::getPluginList(
        vector<PluginContainer::Map::key_type>& plugins
        )
{
    return _plugins.getPlugins(plugins);
}

PluginContainer::Map::mapped_type
Allocator::Impl::getPlugin(
        const string& name
        )
{
    try {
        return _plugins.getPlugin(name);
    } catch(const PluginContainer::Exception& pce) {
        THROW_EXCEPTION(
                bgsched::allocator::InputException,
                bgsched::allocator::InputErrors::PluginNotFound,
                pce.what()
                );
    }
}

Block::Ptr
Allocator::Impl::findBlockResources(
        const Allocator& allocator,
        const Model& model,
        const ResourceSpec& resourceSpec,
        const string& pluginName
        )
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock");
    // Lock mutex as single writer across multiple threads, mutex is automatically unlocked when stack is unwound
    boost::unique_lock<boost::shared_mutex> lock(_mutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock");

    Block::Ptr block;
    LOG_DEBUG_MSG("Calling findBlockResources for plug-in: " << pluginName);
    PluginContainer::Handle::Ptr handle;
    try {
        handle = getPlugin(pluginName);
    } catch(...) { // Catch any exceptions getting the plug-in
        throw; // Rethrow the exception
    }

    try {
        block = handle->getPlugin()->findBlockResources(allocator, model, resourceSpec);
    } catch (const bgsched::allocator::Plugin::Exception& e) {
        // Rethrow the plug-in exception
        throw;
    }

    return block;
}

void
Allocator::Impl::prepare(
        const Allocator& allocator,
        Model& model,
        const string& blockName,
        const string& pluginName
        )
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock");
    // Lock mutex as single writer across multiple threads, mutex is automatically unlocked when stack is unwound
    boost::unique_lock<boost::shared_mutex> lock(_mutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock");

    LOG_DEBUG_MSG("Calling prepare for plug-in: " << pluginName);
    PluginContainer::Handle::Ptr handle;
    try {
        handle = getPlugin(pluginName);
    } catch(...) { // Catch any exceptions getting the plug-in
        throw; // Rethrow the exception
    }

    try {
        handle->getPlugin()->prepare(allocator, model, blockName);
    } catch(const bgsched::allocator::Plugin::Exception& e) {
        THROW_EXCEPTION(
                bgsched::allocator::ResourceAllocationException,
                bgsched::allocator::ResourceAllocationErrors::BlockResourcesUnavailable,
                e.what()
                );
    }
}

void
Allocator::Impl::release(
        const Allocator& allocator,
        Model& model,
        const string& blockName,
        const string& pluginName
        )
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock");
    // Lock mutex as single writer across multiple threads, mutex is automatically unlocked when stack is unwound
    boost::unique_lock<boost::shared_mutex> lock(_mutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock");

    LOG_DEBUG_MSG( "Calling release for plug-in: " << pluginName );
    PluginContainer::Handle::Ptr handle;
    try {
        handle = getPlugin(pluginName);
    } catch(...) { // Catch any exceptions getting the plug-in
        throw; // Rethrow the exception
    }

    try {
        handle->getPlugin()->release(allocator, model, blockName);
    } catch(const bgsched::allocator::Plugin::Exception& e) {
        THROW_EXCEPTION(
                bgsched::allocator::ResourceAllocationException,
                bgsched::allocator::ResourceAllocationErrors::BlockDeallocateFailed,
                e.what());
    }
}


vector<Shape::Pimpl>
Allocator::Impl::getShapesForSize(
        const uint32_t size
        )
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain shared read lock");
    // Lock mutex as shared read by multiple threads, mutex is automatically unlocked when stack is unwound
    boost::shared_lock<boost::shared_mutex> lock(_mutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained shared read lock");

    vector<Shape::Pimpl> shapes;
    uint32_t midplaneCount = 0;
    uint32_t computeNodeCount = 0;

    // Small block (32, 64, 128 or 256 compute nodes)
    // Only certain dimensions allowed for small blocks
    if (size <= Midplane::Half) {
        Shape::Pimpl newShape = createSmallShape(size);
        shapes.push_back(newShape);
        return shapes;
    } else { // Large block (512 compute nodes or multiples of 512 compute nodes)
        // Validate size fits in machine
        Coordinates machineSize(0,0,0,0);
        machineSize = core::getMachineSize();
        uint32_t machineComputeNodes = ((((machineSize[Dimension::A] * machineSize[Dimension::B]) * machineSize[Dimension::C]) * machineSize[Dimension::D]) * Midplane::NodeCount);
        if (size > machineComputeNodes) {
            THROW_EXCEPTION(
                    bgsched::allocator::InputException,
                    bgsched::allocator::InputErrors::InvalidComputeNodeSize,
                    "Number of compute nodes specified exceeds machine compute nodes."
            );
        }

        try {
            // Need to round up the number of midplanes needed
            midplaneCount = boost::lexical_cast<uint32_t>(ceil((double)size/(double)Midplane::NodeCount));
        } catch (const boost::bad_lexical_cast& e) {
            THROW_EXCEPTION(
                    bgsched::allocator::InternalException,
                    bgsched::allocator::InternalErrors::UnexpectedError,
                    e.what()
            );
        }

        // The available shapes map is keyed by compute node size so convert to that
        computeNodeCount = midplaneCount * Midplane::NodeCount;

        bool foundShapes = false;
        // Loop until valid shapes are found
        while (foundShapes == false) {
            //LOG_DEBUG_MSG("Trying to find a shape for " << computeNodeCount << " compute nodes");
            // Get shapes from map of previously calculated shapes
            ValidShapes::const_iterator shapesIter = _validShapes.find(computeNodeCount);
            // Verify a shape matching the size was found
            if (shapesIter == _validShapes.end()) {
                // No matching shape was found so try next valid compute node size
                computeNodeCount = computeNodeCount + Midplane::NodeCount;
            } else {
                foundShapes = true;
                // Return the list of valid shapes for the size
                shapes = shapesIter->second;
            }
        }
        return shapes;
    }
}


vector<Shape::Pimpl>
Allocator::Impl::getShapes()
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain shared read lock");
    // Lock mutex as shared read by multiple threads, mutex is automatically unlocked when stack is unwound
    boost::shared_lock<boost::shared_mutex> lock(_mutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained shared read lock");

    vector<Shape::Pimpl> shapes;
    for (ValidShapes::const_iterator iter = _validShapes.begin(); iter != _validShapes.end(); ++iter) {
        // Get vector of all large shapes
        vector<Shape::Pimpl> shapesVector = iter->second;
        for (vector<Shape::Pimpl>::const_iterator iter2 = shapesVector.begin(); iter2 != shapesVector.end(); ++iter2) {
            shapes.push_back(*iter2);
        }
    }
    return shapes;
}

void
Allocator::Impl::buildShapesList()
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock");
    // Lock mutex as single writer across multiple threads, mutex is automatically unlocked when stack is unwound
    boost::unique_lock<boost::shared_mutex> lock(_mutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock");

    // For each dimension add the shape
    for ( uint32_t a = 1; a <= _hardware->getMachineSize(Dimension::A); ++a ) {
        for ( uint32_t b = 1; b <= _hardware->getMachineSize(Dimension::B); ++b ) {
            for ( uint32_t c = 1; c <= _hardware->getMachineSize(Dimension::C); ++c ) {
                for ( uint32_t d = 1; d <= _hardware->getMachineSize(Dimension::D); ++d ) {
                    // Shape is in midplanes
                    addShape( a, b, c, d );
                }
            }
        }
    }
}

void
Allocator::Impl::dumpShapesList(ostream& os)
{
    os << "Available large block shapes for machine:" << endl;
    for (ValidShapes::const_iterator iter = _validShapes.begin(); iter != _validShapes.end(); ++iter) {
        // Get list of shapes
        vector<Shape::Pimpl> shapesVector = iter->second;
        for (vector<Shape::Pimpl>::const_iterator iter2 = shapesVector.begin(); iter2 != shapesVector.end(); ++iter2) {
            Shape::Pimpl shape = *iter2;
            os << "Size (in compute nodes):" << iter->first << " Shape in midplanes - A:" << shape->getMidplaneSize(Dimension::A)
            << " B:" << shape->getMidplaneSize(Dimension::B) << " C:" << shape->getMidplaneSize(Dimension::C)
            << " D:" << shape->getMidplaneSize(Dimension::D) << endl;
        }
    }
}

void
Allocator::Impl::addShape(
        const uint32_t a,
        const uint32_t b,
        const uint32_t c,
        const uint32_t d
        )
{
    // Create the shape
    Shape::Pimpl newShape(new Shape::Impl( a, b, c, d));
    _validShapes[newShape->getNodeCount()].push_back( newShape );
}

Shape::Pimpl
Allocator::Impl::createMidplaneShape(
        const uint32_t a,
        const uint32_t b,
        const uint32_t c,
        const uint32_t d
        )
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain shared read lock");
    // Lock mutex as shared read by multiple threads, mutex is automatically unlocked when stack is unwound
    boost::shared_lock<boost::shared_mutex> lock(_mutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained shared read lock");

    // Create the shape
    Shape::Pimpl newShape(new Shape::Impl(a, b, c, d));
    if (validateShape( newShape )) {
        return newShape;
    } else {
        ostringstream os;
        os << "Midplane dimensions " <<  a << ", " << b << ", " << c << ", " << d << " exceeds the system dimensions.";
        THROW_EXCEPTION(
                bgsched::allocator::ShapeException,
                bgsched::allocator::ShapeErrors::InvalidMidplaneDimension,
                os.str()
                );
    }
}

Shape::Pimpl
Allocator::Impl::createSmallShape(
        const uint32_t size
        )
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain shared read lock");
    // Lock mutex as shared read by multiple threads, mutex is automatically unlocked when stack is unwound
    boost::shared_lock<boost::shared_mutex> lock(_mutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained shared read lock");

    if (size > Midplane::Half) {
        ostringstream os;
        os << "Compute node size " << size << " exceeds the maximum small shape size of " << Midplane::Half;
        THROW_EXCEPTION(
                bgsched::allocator::ShapeException,
                bgsched::allocator::ShapeErrors::InvalidSize,
                os.str()
                );
    }

    // Create the shape
    if (size <= NodeBoard::NodeCount) {
        return Shape::Pimpl( new Shape::Impl(Shape::Impl::NodeBoard));
    } else if ( size <= Midplane::Eighth ) {
        return Shape::Pimpl( new Shape::Impl(Shape::Impl::Eighth));
    } else if ( size <= Midplane::Quarter ) {
        return Shape::Pimpl( new Shape::Impl(Shape::Impl::Quarter));
    } else {
        return Shape::Pimpl( new Shape::Impl(Shape::Impl::Half));
    }
}

bool
Allocator::Impl::validateShape (
        const Shape::Pimpl shape
        )
{
    // Make sure shape dimensions fit into the machine
    for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim) {
        if (shape->getMidplaneSize(dim) > _hardware->getMachineSize(dim)) {
            return false;
        }
    }
    return true;
}

vector<Shape::Pimpl>
Allocator::Impl::getRotations(
        const Shape::Pimpl shape
        )
{
    uint32_t shapeDims[Dimension::MidplaneDims];
    for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim) {
        shapeDims[dim] = shape->getMidplaneSize(dim);
    }
    sort(shapeDims, shapeDims+Dimension::MidplaneDims);

    vector<Shape::Pimpl> shapes;
    Shape::Pimpl shapeImpl(new Shape::Impl(shapeDims));
    if (validateShape(shapeImpl)) {
         shapes.push_back(shapeImpl);
    }

    while (next_permutation(shapeDims, shapeDims+Dimension::MidplaneDims)) {
        Shape::Pimpl shapeImpl(new Shape::Impl(shapeDims));
        if (validateShape(shapeImpl)) {
            shapes.push_back(shapeImpl);
        }
    }
    return shapes;
}

void
Allocator::Impl::updateHardware(
        const ComputeHardware::Pimpl hardware
        )
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock");
    // Lock mutex as single writer across multiple threads, mutex is automatically unlocked when stack is unwound
    boost::unique_lock<boost::shared_mutex> lock(_mutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock");

    LOG_DEBUG_MSG("Updating allocator hardware resources");
    // Reference the new hardware state
    _hardware = hardware;
}

} // namespace bgsched::allocator
} // namespace bgsched
