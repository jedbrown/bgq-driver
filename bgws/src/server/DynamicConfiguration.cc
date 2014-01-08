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

#include "DynamicConfiguration.hpp"

#include "dbConnectionPool.hpp"

#include <utility/include/Log.h>

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <stdint.h>

#include <stdexcept>


using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {


const std::string DynamicConfiguration::PROPERTIES_SECTION_NAME( "bgws" );
const capena::http::uri::Path DynamicConfiguration::DEFAULT_PATH_BASE(capena::http::uri::Path() / "bg");


DynamicConfiguration::ConstPtr DynamicConfiguration::create(
        bgq::utility::Properties::ConstPtr properties_ptr
    )
{
    return ConstPtr( new DynamicConfiguration( properties_ptr ) );
}


DynamicConfiguration::DynamicConfiguration(
        bgq::utility::Properties::ConstPtr properties_ptr
    ) :
        _properties_ptr(properties_ptr)
{
    _properties_filename = _properties_ptr->getFilename();

    try {
        string path_base_str(_properties_ptr->getValue( PROPERTIES_SECTION_NAME, "path_base" ));

        _path_base = capena::http::uri::Path(path_base_str);

        LOG_INFO_MSG( "Path base in config file is '" << path_base_str << "' -> " << _path_base );
    } catch ( std::exception& e ) {
        LOG_INFO_MSG( "Failed to get path base from configuration file. Will use the default. The exception message is '" << e.what() << "'." );
        _path_base = DEFAULT_PATH_BASE;
    }

    try {
        string session_timeout_str(_properties_ptr->getValue( PROPERTIES_SECTION_NAME, "session_timeout" ));

        if ( session_timeout_str.empty() || session_timeout_str[0] == '-' ) {
            BOOST_THROW_EXCEPTION( std::runtime_error( "session timeout must be a number > 0" ) );
        }

        _session_timeout_seconds = boost::lexical_cast<uint32_t>( session_timeout_str );
        if ( _session_timeout_seconds == 0 ) {
             BOOST_THROW_EXCEPTION( std::runtime_error( "session timeout must be > 0" ) );
        }
        LOG_INFO_MSG( "Session timeout in config file is " << _session_timeout_seconds << " seconds." );
    } catch ( std::exception& e ) {
        LOG_INFO_MSG( "Failed to get session timeout from configuration file. Will use the default. The exception message is '" << e.what() << "'." );
        _session_timeout_seconds = 3600;
    }

    try {
        _machine_name = _properties_ptr->getValue( PROPERTIES_SECTION_NAME, "machine_name" );

        LOG_INFO_MSG( "Machine name in configuration file is '" << _machine_name << "'" );

    } catch ( std::exception& e ) {
        LOG_INFO_MSG( "Failed to get machine name from configuration file. Will use the default. The exception message is '" << e.what() << "'." );
        _machine_name = boost::asio::ip::host_name();
    }


    try {
        _measurement_system = _properties_ptr->getValue( PROPERTIES_SECTION_NAME, "measurement_system" );

        if ( _measurement_system == "US" || _measurement_system == "SI" ) {
            LOG_INFO_MSG( "Measurement system in configuration file is '" << _measurement_system << "'" );
        } else {
            LOG_WARN_MSG( "Invalid measurement system value in configuration file. Will use the default. The measurement system in the configuration file is '" << _measurement_system << "'" );
            _measurement_system = "";
        }

    } catch ( std::exception& e ) {
        LOG_INFO_MSG( "Failed to get measurement system from configuration file. Will use the default. The exception message is '" << e.what() << "'." );
        _measurement_system = "";
    }


    try {
        _check_user_admin_exe_path = _properties_ptr->getValue( PROPERTIES_SECTION_NAME, "check_user_admin_exe" );

        LOG_INFO_MSG( "Check user administrator executable path in configuration file is '" << _check_user_admin_exe_path << "'" );

    } catch ( std::exception& e ) {
        LOG_WARN_MSG( "Failed to get check user administrator executable path from configuration file. No users will be administrators. The exception message is '" << e.what() << "'." );
    }

    try {
        _diagnostics_executable = _properties_ptr->getValue( PROPERTIES_SECTION_NAME, "diagnostics_exe" );

        LOG_INFO_MSG( "Diagnostics executable path in configuration file is '" << _diagnostics_executable << "'" );

    } catch ( std::exception& e ) {
        LOG_INFO_MSG( "Failed to get diagnostics executable path from configuration file. Will use the default. The exception message is '" << e.what() << "'." );
        _diagnostics_executable = "/bgsys/drivers/ppcfloor/diags/bin/rundiags.py";
    }

    try {
        _user_auth_exe_path = _properties_ptr->getValue( PROPERTIES_SECTION_NAME, "user_authentication_exe" );

        LOG_INFO_MSG( "User authentication executable path in configuration file is '" << _user_auth_exe_path << "'" );

    } catch ( std::exception& e ) {
        LOG_INFO_MSG( "Failed to get user authentication executable path from configuration file. Will use the default. The exception message is '" << e.what() << "'." );
        _user_auth_exe_path = "/usr/local/libexec/pwauth";
    }

    try {
        _service_action_exe_path = _properties_ptr->getValue( PROPERTIES_SECTION_NAME, "service_action_exe" );

        LOG_INFO_MSG( "Service action executable path in configuration file is '" << _service_action_exe_path << "'" );

    } catch ( std::exception& e ) {
        LOG_INFO_MSG( "Failed to get service action executable path from configuration file. Will use the default. The exception message is '" << e.what() << "'." );
        _service_action_exe_path = "/bgsys/drivers/ppcfloor/baremetal/bin/ServiceAction";
    }


    try {
        _teal_close_alert_exe_path = _properties_ptr->getValue( PROPERTIES_SECTION_NAME, "teal_chalert_exe" );

        LOG_INFO_MSG( "TEAL close alert executable path in configuration file is '" << _teal_close_alert_exe_path << "'" );

    } catch ( std::exception& e ) {
        LOG_INFO_MSG( "Failed to get TEAL close alert executable path from configuration file. Will use the default. The exception message is '" << e.what() << "'." );
        _teal_close_alert_exe_path = "/opt/teal/bin/tlchalert";
    }

    try {
        _teal_remove_alert_exe_path = _properties_ptr->getValue( PROPERTIES_SECTION_NAME, "teal_rmalert_exe" );

        LOG_INFO_MSG( "TEAL remove alert executable path in configuration file is '" << _teal_remove_alert_exe_path << "'" );

    } catch ( std::exception& e ) {
        LOG_INFO_MSG( "Failed to get TEAL remove alert executable path from configuration file. Will use the default. The exception message is '" << e.what() << "'." );
        _teal_remove_alert_exe_path = "/opt/teal/bin/tlrmalert";
    }

    _security_enforcer_ptr.reset( new hlcs::security::Enforcer(
            _properties_ptr,
            dbConnectionPool::getConnection()
        ) );
}


} // namespace bgws
