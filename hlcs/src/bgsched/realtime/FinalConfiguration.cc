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
#include "FinalConfiguration.h"

#include <bgsched/realtime/ConfigurationException.h>

#include "bgsched/utility.h"

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include <memory>
#include <string>

using bgq::utility::ClientPortConfiguration;
using bgq::utility::Properties;

using std::exception;
using std::string;

LOG_DECLARE_FILE( "bgsched" );

namespace bgsched {
namespace realtime {

//---------------------------------------------------------------------
// class FinalConfiguration

const string FinalConfiguration::DEFAULT_SERVICE_NAME("32061");

const string FinalConfiguration::CONFIG_FILE_REALTIME_SECTION_NAME("bgsched");


FinalConfiguration::FinalConfiguration(
        const ClientConfiguration& client_configuration
    ) :
        _port_config(
                DEFAULT_SERVICE_NAME,
                "realtime",
                "real-time",
                ClientPortConfiguration::ConnectionType::Administrative
            )
{
    try {
        _port_config.setPorts( client_configuration.getHost() );
        _port_config.setProperties( getProperties(), CONFIG_FILE_REALTIME_SECTION_NAME );
        _port_config.notifyComplete();
    } catch ( std::exception& e ) {
        THROW_EXCEPTION(
                ConfigurationException,
                ConfigurationErrors::InvalidHost,
                "Error configuring the real-time server that the real-time client will connect to,"
                " '" << e.what() << "'. The real-time API cannot be initialized."
            );
    }
}


} // namespace bgsched::realtime
} // namespace bgsched
