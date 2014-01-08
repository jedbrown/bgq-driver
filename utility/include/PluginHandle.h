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
#ifndef BGQ_UTILITY_PLUGIN_HANDLE_H
#define BGQ_UTILITY_PLUGIN_HANDLE_H
/*!
 * \file utility/include/PluginHandle.h
 * \brief \link bgq::utility::PluginHandle PluginHandle\endlink definition and implementation.
 */

#include <utility/include/Log.h>
#include <utility/include/ThrowException.h>

#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include <string>

#include <dlfcn.h>

namespace bgq {
namespace utility {

/*!
 * \brief Interface to load a plug-in object.
 *
 * \tparam T the plug-in interface, this should be an abstract base class which plug-in types derive from.
 *
 * \section Linking
 *
 * This is a header only library, but you must link to the following libraries:
 *
 * - libbgutility.so for logging purposes
 * - libdl.so for dynamic loading
 *
 * Add the following to your Makefile.
 *
 \verbatim

 LDFLAGS += -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility
 LDFLAGS += -L -ldl
 LDFLAGS += -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib
 \endverbatim
 *
 * Otherwise you will encounter linker errors.
 *
 * \section plugin_handle_overview Overview
 *
 * This class loads a shared library and looks for two symbols
 *
 * - T* create()
 * - void destroy(T*)
 *
 * If either of these symbols are not found, the constructor throws a MissingSymbol exception. The
 * expected usage scenario is for a plug-in author to create an abstract interface, this type should
 * be the PluginHandle template parameter. Plug-in authors should derive from this interface, and
 * include these two methods in their shared library to create and destroy their 
 * plug-in object from a base class pointer.
 */
template <
    typename T
    >
class PluginHandle
{
public:
    static const std::string CreateSymbolName;  //!< name of the create symbol
    static const std::string DestroySymbolName; //!< name of the destroy symbol

public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<PluginHandle> Ptr;

    /*!
     * \brief Plugin type.
     */
    typedef T Plugin;

    /*!
     * \brief Plugin pointer type.
     */
    typedef boost::shared_ptr<T> PluginPtr;

    /*!
     * \brief Invalid path exception.
     */
    struct InvalidPath : public std::invalid_argument
    {
        /*!
         * \brief ctor.
         */
        InvalidPath(
                const std::string& what         //!< [in]
                ) : std::invalid_argument(what)
        {

        }
    };

    /*!
     * \brief Invalid plugin exception.
     */
    struct InvalidPlugin : public std::invalid_argument
    {
        /*!
         * \brief ctor.
         */
        InvalidPlugin(
                const std::string& what         //!< [in]
                ) : std::invalid_argument(what)
        {

        }
    };

    /*!
     * \brief Missing symbol exception.
     */
    struct MissingSymbol : public std::invalid_argument
    {
        /*!
         * \brief ctor.
         */
        MissingSymbol(
                const std::string& what         //!< [in]
                ) : std::invalid_argument(what)
        {

        }
    };

public:
    /*!
     * \brief ctor.
     *
     * loads the plug-in library specified by the path argument. Once the plug-in is loaded
     * it looks for a create and a destroy symbol to create a Plugin object.
     *
     * \throws InvalidPath if the plug-in could not be loaded
     * \throws MissingSymbol if the create or destroy symbols are missing
     * \throws InvalidPlugin if the plugin's create method returns a NULL pointer
     */
    explicit PluginHandle(
            const std::string& path,    //!< [in] path to library
            int flags = RTLD_LAZY       //!< [in] dlopen flags
            ) :
        _create(),
        _destroy(),
        _path( path ),
        _dlHandle(),
        _handle()
    {
        log4cxx::LoggerPtr log_logger_ = log4cxx::Logger::getLogger( LoggerName );
        // load library
        void* handle = dlopen( path.c_str(), flags );
        char* error = dlerror();
        if ( handle == NULL) {
            UTILITY_THROW_EXCEPTION( InvalidPath, "Could not load plug-in: " << error  << " Path is: " << path );
        }
        LOG_DEBUG_MSG( "opened plugin '" << _path << "' successfully at " << handle );
        _dlHandle.reset(
                handle,
                boost::bind( &dlclose, handle )
                );

        // load symbol create
        _create = reinterpret_cast<T*(*)()>(
                dlsym( _dlHandle.get(), CreateSymbolName.c_str() )
                );
        error = dlerror();
        if ( !_create ) {
            UTILITY_THROW_EXCEPTION( MissingSymbol, "Missing " << CreateSymbolName << " symbol: " << error );
        }
        LOG_DEBUG_MSG( "found " << CreateSymbolName << " symbol" );

        // load symbol destroy
        _destroy = reinterpret_cast<void (*)(T*)>(
                dlsym( _dlHandle.get(), DestroySymbolName.c_str() )
                );
        error = dlerror();
        if ( !_destroy ) {
            UTILITY_THROW_EXCEPTION( MissingSymbol, "Missing " << DestroySymbolName << " symbol: " << error );
        }
        LOG_DEBUG_MSG( "found " << DestroySymbolName << " symbol" );

        // create handle, the custom deleter method for the shared_ptr will
        // destroy the plugin
        T* plugin = _create();
        if ( !plugin ) {
            UTILITY_THROW_EXCEPTION( InvalidPlugin, "Could not create plug-in" );
        }
        _handle.reset(
                plugin, 
                boost::bind(_destroy, plugin)
                );
        LOG_DEBUG_MSG( "created handle" );
    }

    ~PluginHandle()
    {
        log4cxx::LoggerPtr log_logger_ = log4cxx::Logger::getLogger( LoggerName );
        LOG_DEBUG_MSG( "closed plugin '" << _path << "'" );
    }

    /*!
     * \brief get underlying Plugin object.
     */
    PluginPtr getPlugin()
    {
        BOOST_ASSERT(_handle);
        return _handle;
    }

    /*!
     * \brief get path to plugin.
     */
    const std::string& getPath() const
    {
        return _path;
    }

private:
    static const std::string LoggerName;

private:
    boost::function<T* (void)> _create;     //!< function pointer to create plugin.
    boost::function<void (T*)> _destroy;    //!< function pointer to destroy plugin.
    const std::string _path;                //!< path to plugin.
    boost::shared_ptr<void> _dlHandle;      //!< handle to dynamic loader
    PluginPtr _handle;                      //!< handle to Plugin object.
};


// storage for static members

template <typename T>
const std::string PluginHandle<T>::LoggerName = "ibm.utility.PluginHandle";

template <typename T>
const std::string PluginHandle<T>::CreateSymbolName = "create";

template <typename T>
const std::string PluginHandle<T>::DestroySymbolName = "destroy";

} // utility
} // bgq

#endif
