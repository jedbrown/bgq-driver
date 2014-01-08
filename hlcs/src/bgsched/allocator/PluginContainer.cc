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

#include "bgsched/allocator/PluginContainer.h"

#include "bgsched/utility.h"

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include <boost/assert.hpp>
#include <boost/foreach.hpp>

using namespace bgsched;
using namespace bgsched::allocator;
using namespace std;

LOG_DECLARE_FILE("bgsched");

namespace bgsched {
namespace allocator {

PluginContainer::PluginContainer() :
    _plugins()
{
    this->init();
}

void
PluginContainer::init()
{
    // Get Properties file
    const bgq::utility::Properties& properties = *(getProperties());

    try {
        // Get list of plug-ins from bg.properties file
        typedef bgq::utility::Properties::Section Section;
        const Section& plugins = properties.getValues("bgsched.allocator.plugins");
        if (plugins.empty()) {
            THROW_EXCEPTION(
                    Exception,
                    Errors::NoPluginsFound,
                    "Empty or missing plug-ins section");
        }
        LOG_DEBUG_MSG("Found " << plugins.size() << " plug-in(s) in bg.properties file");

        // Iterate through section results
        for (Section::const_iterator i = plugins.begin(); i != plugins.end(); ++i) {
            const string& name = i->first;
            const string& path = i->second;
            try {
                // Instantiate new PluginHandle object
                LOG_DEBUG_MSG("Creating handle for " << name);
                Handle::Ptr handle(new Handle(path));

                // Insert into map
                BOOST_ASSERT(_plugins.insert(Map::value_type(name, handle)).second);
                LOG_INFO_MSG("Added plug-in " << name);
            } catch (const std::invalid_argument& e) {
                LOG_WARN_MSG(e.what());
                LOG_WARN_MSG("Ignoring plug-in " << name);
            }
        }
    } catch (const std::invalid_argument& e) {
        LOG_WARN_MSG(e.what());
        THROW_EXCEPTION(
                Exception,
                Errors::NoPluginsFound,
                "Empty or missing plug-ins section");
    }
}

PluginContainer::Handle::Ptr
PluginContainer::getPlugin(const string& name) const
{
    Map::const_iterator result = _plugins.find(name);
    if (result == _plugins.end()) {
        THROW_EXCEPTION(
                Exception,
                Errors::PluginNotFound,
                "Plug-in " << name << " was not found");
    }

    return result->second;
}

void
PluginContainer::getPlugins(
        vector<PluginContainer::Map::key_type>& plugins
        ) const
{
    BOOST_FOREACH( const Map::value_type& handle, _plugins ) {
        plugins.push_back( handle.first );
    }
}

string
PluginContainer::Errors::toString(Value /*v*/, const string& what)
{
    return what;
}

} // namespace bgsched::allocator
} // namespace bgsched
