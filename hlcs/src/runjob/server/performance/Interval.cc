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
#include "server/performance/Interval.h"

#include "common/defaults.h"
#include "common/logging.h"
#include "common/properties.h"

#include "server/Options.h"

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace performance {

Interval::Interval(
        const Options& options
        ) :
    _options( options )
{

}

unsigned
Interval::get() const
{
    int interval( defaults::ServerPerfCounterInterval );
    try {
        const std::string& value = _options.getProperties()->getValue( PropertiesSection, "performance_counter_interval" );
        interval = boost::lexical_cast<int>( value );
        if ( interval <= 0 ) {
            LOG_WARN_MSG( "performance_counter_interval value must be positive: " << value );
            interval = defaults::ServerPerfCounterInterval;
            LOG_WARN_MSG( "using default value of " << interval );
        }
    } catch ( const boost::bad_lexical_cast& e ) {
        LOG_WARN_MSG(
                "garbage value in performance_counter_interval key in [" << PropertiesSection << "] " <<
                "section of properties file " << _options.getProperties()->getFilename()
                );
        LOG_WARN_MSG( "using default value of " << interval );
    } catch ( const std::invalid_argument& e ) {
        LOG_WARN_MSG(
                "missing performance_counter_interval key in [" << PropertiesSection << "] " <<
                "section of properties file " << _options.getProperties()->getFilename()
                );
        LOG_WARN_MSG( "using default value of " << interval );
    }

    return interval;
}

} // performance
} // server
} // runjob
