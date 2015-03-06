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

#include <bgsched/allocator/Allocator.h>

#include "bgsched/allocator/AllocatorImpl.h"

#include <utility/include/PluginHandle.h>

#include <boost/thread/locks.hpp>
#include <boost/foreach.hpp>

using namespace bgsched;
using namespace bgsched::allocator;
using namespace std;

namespace bgsched {
namespace allocator {

Allocator::Allocator() :
    _impl(new Allocator::Impl())
{
    // Nothing to do
}

Allocator::~Allocator()
{
    // Nothing to do
}

LiveModel&
Allocator::getLiveModel() const
{
    return _impl->getLiveModel();
}

vector<string>
Allocator::getPluginList() const
{
    Allocator::Impl::Plugins plugins;

    _impl->getPluginList(plugins);

    vector<string> names;
    BOOST_FOREACH(PluginContainer::Map::key_type& name, plugins) {
        names.push_back(name);
    }

    return names;
}

Allocator::Shapes
Allocator::getShapesForSize(
        const uint32_t size
        ) const
{
    Shapes shapes;

    vector<Shape::Pimpl> shapePimpls = _impl->getShapesForSize(size);
    for (vector<Shape::Pimpl>::const_iterator iter = shapePimpls.begin(); iter != shapePimpls.end(); ++iter) {
         shapes.push_back(Shape(*iter));
    }

    return shapes;
}

Allocator::Shapes
Allocator::getShapes() const
{
    Shapes shapes;

    vector<Shape::Pimpl> shapePimpls = _impl->getShapes();
    for (vector<Shape::Pimpl>::const_iterator iter = shapePimpls.begin(); iter != shapePimpls.end(); ++iter) {
         shapes.push_back(Shape(*iter));
    }

    return shapes;
}

Allocator::Shapes
Allocator::getRotations(
        const Shape& shape
) const
{
    Shapes shapes;

    // Return the same shape if small shape
    if (shape.isSmall()) {
        shapes.push_back(shape);
    } else {
        vector<Shape::Pimpl> shapePimpls = _impl->getRotations(shape.getPimpl());
        for (vector<Shape::Pimpl>::iterator iter = shapePimpls.begin(); iter != shapePimpls.end(); ++iter) {
            Shape newShape = Shape(*iter);
            // Don't return shapes that match original shape
            if (newShape != shape) {
                shapes.push_back(newShape);
            }
        }
    }

    return shapes;
}

Shape
Allocator::createMidplaneShape(
        const uint32_t a,
        const uint32_t b,
        const uint32_t c,
        const uint32_t d
        ) const
{
    return Shape(_impl->createMidplaneShape(a, b, c, d));
}

Shape
Allocator::createSmallShape(
        const uint32_t size
        ) const
{
    return Shape(_impl->createSmallShape(size));
}

Block::Ptr
Allocator::findBlockResources(
        const Model& model,
        const ResourceSpec& resourceSpec,
        const string& pluginHandle
        ) const
{
    Block::Ptr block;

    try {
        block = _impl->findBlockResources(*this, model, resourceSpec, pluginHandle);
    } catch(...) {
        throw; // Rethrow the plug-in exception
    }

    // Note: Block pointer could be NULL which means no resources were found
    return block;
}

void
Allocator::prepare(
        Model& model,
        const string& blockName,
        const string& pluginHandle
        )
{
    try {
        _impl->prepare(*this, model, blockName, pluginHandle);
    } catch(...) {
        throw; // Rethrow the exception
    }
}

void
Allocator::release(
        Model& model,
        const string& blockName,
        const string& pluginHandle
        )
{
    try {
        _impl->release(*this, model, blockName, pluginHandle);
    } catch(...) {
        throw; // Rethrow the exception
    }
}

void
Allocator::dump(
        ostream& os,
        EnumWrapper<Allocator::DumpVerbosity::Value> verbosity
        ) const
{
    _impl->dump(os, verbosity);
}

} // namespace bgsched::allocator
} // namespace bgsched

