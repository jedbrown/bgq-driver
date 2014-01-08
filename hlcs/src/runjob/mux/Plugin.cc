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
#include "mux/Plugin.h"

#include "common/logging.h"
#include "common/properties.h"

#include "mux/Multiplexer.h"
#include "mux/Options.h"

#include <sstream>

namespace runjob {
namespace mux {

LOG_DECLARE_FILE( runjob::mux::log );

Plugin::Plugin(
        const boost::shared_ptr<Multiplexer>& mux
        ) :
    _strand( mux->getIoService() ),
    _options( mux->getOptions() ),
    _handle()
{

}

void
Plugin::start()
{
    _strand.post(
            boost::bind(
                &Plugin::startImpl,
                shared_from_this()
                )
            );
}

void
Plugin::get(
        const GetCallback& callback
        )
{
    _strand.post(
            boost::bind(
                &Plugin::getImpl,
                shared_from_this(),
                callback
                )
            );
}

void
Plugin::startImpl()
{
    if ( _handle ) {
        LOG_INFO_MSG( "unloading plugin '" << _handle->getPath() << "'" );
        _handle.reset();
    }

    const int flags = this->getFlags();

    try {
        // look in properties file for plugin path
        const std::string path = _options.getProperties()->getValue( PropertiesSection, "plugin" );
        LOG_DEBUG_MSG( "using plugin path " << path << " with dlopen flags 0x" << std::hex << std::setw(4) << std::setfill('0') << flags );
        _handle.reset(
                new Handle( path, flags )
                );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( "could not create plugin: " << e.what() );
    } catch ( ... ) {
        LOG_WARN_MSG( "could not create plugin" );
    }
}

int
Plugin::getFlags() const
{
    // default flags if none are found in bg.properties
    int result = RTLD_LAZY;

    const std::string key = "plugin_flags";
    try {
        std::string value = _options.getProperties()->getValue( PropertiesSection, key );
        if ( value.empty() ) {
            LOG_WARN_MSG( "empty value for " << key << " key in [" << PropertiesSection << "] section." );
            return result;
        }

        // strip off leading 0x if present
        if ( value.size() > 2 && value.substr(0,2) == "0x" ) {
            value.erase(0,2);
        }

        std::istringstream is( value );
        is >> std::hex >> result;
        if ( is.fail() || !is.eof() ) {
            LOG_WARN_MSG(
                    "garbage value for " << key << " key in [" << PropertiesSection << "] section: " <<
                    _options.getProperties()->getValue( PropertiesSection, key )
                    );

            result = RTLD_LAZY;
        }
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }

    return result;
}

void
Plugin::getImpl(
        const GetCallback& callback
        )
{
    callback( _handle );
}

} // mux
} // runjob
