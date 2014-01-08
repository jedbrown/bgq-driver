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
 * \file bgsched/allocator/PluginContainer.h
 * \brief PluginContainer class definition.
 */

#ifndef BGSCHED_ALLOCATOR_PLUGINCONTAINER_H_
#define BGSCHED_ALLOCATOR_PLUGINCONTAINER_H_

#include <bgsched/allocator/Plugin.h>

#include <bgsched/Exception.h>

#include <utility/include/PluginHandle.h>

#include <map>
#include <string>
#include <vector>

namespace bgsched {
namespace allocator {

/*!
 * \brief Container of Plugin objects.
 */
class PluginContainer
{
public:

    /*!
     * \brief Error codes.
     * \addtogroup Exceptions
     * @{
     */
    struct Errors
    {
        /*!
         * \brief Enumerated error codes.
         */
        enum Value
        {
            NoPluginsFound = 0,
            PluginNotFound
        };

        /*!
         * \brief Error message string.
         *
         * \return Error message string.
         */
        static std::string toString(
                Value v,
                const std::string& what
                );
    };

    /*!
     * \brief PluginContainer exception type.
     */
    typedef RuntimeError<Errors> Exception;
    //!< @}

    /*!
     * \brief PluginHandle type.
     */
    typedef bgq::utility::PluginHandle<Plugin> Handle;

    /*!
     * \brief Plugin map.
     */
    typedef std::map<std::string, Handle::Ptr> Map;

    /*!
     * \brief
     *
     * \throws Exception
     */
    PluginContainer();

    /*!
     * \brief Get a specific plug-in.
     *
     * \throws Exception if the plug-in is not found.
     */
    Handle::Ptr getPlugin(
            const std::string& name //!< [in] name of the plug-in to get
            ) const;

    /*!
     * \brief Get a list of all plug-ins.
     */
    void getPlugins(
            std::vector<PluginContainer::Map::key_type>& plugins //!< [out] Vector of plug-ins.
            ) const;

private:

    Map _plugins; //!< Mapping of names to Plugin objects

private:

    /*!
     * \brief Initialize plug-ins based on properties file settings.
     */
    void init();

};

} // namespace bgsched::allocator
} // namespace bgsched

#endif
