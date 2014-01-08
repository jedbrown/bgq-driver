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

#ifndef BGWS_DYNAMIC_CONFIGURATION_HPP_
#define BGWS_DYNAMIC_CONFIGURATION_HPP_


#include "capena-http/http/uri/Path.hpp"

#include <hlcs/include/security/Enforcer.h>

#include <utility/include/Properties.h>

#include <boost/shared_ptr.hpp>

#include <boost/filesystem.hpp>

#include <string>

#include <stdint.h>


namespace bgws {


class DynamicConfiguration
{
public:

    typedef boost::shared_ptr<DynamicConfiguration> Ptr;
    typedef boost::shared_ptr<const DynamicConfiguration> ConstPtr;


    static const std::string PROPERTIES_SECTION_NAME;
    static const capena::http::uri::Path DEFAULT_PATH_BASE;


    static ConstPtr create(
            bgq::utility::Properties::ConstPtr properties_ptr
        );

    const std::string& getPropertiesFilename() const  { return _properties_filename; }
    const bgq::utility::Properties::ConstPtr& getPropertiesPtr() const  { return _properties_ptr; }
    const capena::http::uri::Path& getPathBase() const  { return _path_base; }
    unsigned getSessionTimeoutSeconds() const  { return _session_timeout_seconds; }
    const std::string& getMachineName() const  { return _machine_name; }
    const std::string& getMeasurementSystem() const  { return _measurement_system; }

    const boost::filesystem::path& getCheckUserAdminExecutable() const  { return _check_user_admin_exe_path; }
    const boost::filesystem::path& getDiagnosticsExecutable() const  { return _diagnostics_executable; }
    const boost::filesystem::path& getUserAuthExecutable() const  { return _user_auth_exe_path; }
    const boost::filesystem::path& getServiceActionExecutable() const  { return _service_action_exe_path; }
    const boost::filesystem::path& getTealCloseAlertExecutable() const  { return _teal_close_alert_exe_path; }
    const boost::filesystem::path& getTealRemoveAlertExecutable() const  { return _teal_remove_alert_exe_path; }

    const hlcs::security::Enforcer& getSecurityEnforcer() const  { return *_security_enforcer_ptr; }


private:

    DynamicConfiguration(
            bgq::utility::Properties::ConstPtr properties_ptr
        );


    std::string _properties_filename;

    bgq::utility::Properties::ConstPtr _properties_ptr;

    capena::http::uri::Path _path_base;
    uint32_t _session_timeout_seconds;
    std::string _machine_name;
    std::string _measurement_system;
    boost::filesystem::path _check_user_admin_exe_path;
    boost::filesystem::path _diagnostics_executable;
    boost::filesystem::path _user_auth_exe_path;
    boost::filesystem::path _service_action_exe_path;
    boost::filesystem::path _teal_close_alert_exe_path;
    boost::filesystem::path _teal_remove_alert_exe_path;

    boost::shared_ptr<hlcs::security::Enforcer> _security_enforcer_ptr;
};

} // namespace bgws


#endif
