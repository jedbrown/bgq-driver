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


#include "portConfiguration/SslConfiguration.h"

#include "ssl_util.h"

#include "Log.h"

#include <boost/throw_exception.hpp>

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <unistd.h>


using std::runtime_error;
using std::string;


LOG_DECLARE_FILE( "utility" );


namespace bgq {
namespace utility {


const std::string SslConfiguration::AdministratorCertificateSectionName( "security.admin" );
const std::string SslConfiguration::CommandCertificateSectionName( "security.command" );
const std::string SslConfiguration::CASectionName( "security.ca" );


SslConfiguration::SslConfiguration(
        Use::Value use,
        Certificate::Value certificate,
        Properties::ConstPtr properties_ptr
    ) :
        _use(use),
        _certificate(certificate),
        _my_private_key_filename(),
        _ca_certificates_path(),
        _ca_certificate_filename(),
        _use_default_paths( false )
{
    const string admin_cert_file_name(
            properties_ptr->getValue(
                    AdministratorCertificateSectionName,
                    "certificate"
                )
        );

    const string admin_key_file_name(
            properties_ptr->getValue(
                    AdministratorCertificateSectionName,
                    "key_file"
                )
        );

    const string command_cert_file_name(
            properties_ptr->getValue(
                    CommandCertificateSectionName,
                    "certificate"
                )
        );

    const string command_key_file_name(
            properties_ptr->getValue(
                    CommandCertificateSectionName,
                    "key_file"
                )
        );

    if ( _use == Use::Client ) {
        if ( certificate == Certificate::Command ) {
            // If the user has access to the admin key then use the administrative key.
            if ( access( admin_key_file_name.c_str(), R_OK ) == 0 ) {
                LOG_DEBUG_MSG( "Client command and have R access to administrative key file, using administrative certificate." );
                certificate = Certificate::Administrative;
            } else {
                LOG_DEBUG_MSG( "Client command certificate and do not have access to administrative key file, using command certificate." );
            }
        } else {
            LOG_DEBUG_MSG( "Client requested to use administrative certificate." );
        }
    } else { // used for server... always use the administrative certificate
        certificate = Certificate::Administrative;
    }

    if ( certificate == Certificate::Command ) {
        _my_cert_filename = command_cert_file_name;
        _my_private_key_filename = command_key_file_name;
    } else {
        _my_cert_filename = admin_cert_file_name;
        _my_private_key_filename = admin_key_file_name;
    }

    try {
        _ca_certificates_path.reset( new string( properties_ptr->getValue( CASectionName, "certificates_dir" ) ) );
    } catch ( std::exception& e ) {
        // Ignore this error.
    }

    try {
        _ca_certificate_filename.reset( new string( properties_ptr->getValue( CASectionName, "certificate" ) ) );
    } catch ( std::exception& e ) {
        // Ignore this error.
    }

    try {
        std::istringstream is( properties_ptr->getValue( CASectionName, "use_default_paths" ) );
        is >> std::boolalpha >> _use_default_paths;
    } catch ( std::exception& e ) {
        // Ignore error
        _use_default_paths = false;
    }

    if (  (! _ca_certificates_path) &&
          (! _ca_certificate_filename) &&
          (! _use_default_paths) ) {
        BOOST_THROW_EXCEPTION( runtime_error( "the SSl configuration is not valid, no CA certificate is configured" ) );
    }

    LOG_DEBUG_MSG(
            "Configuration:\n"
            "\tCertificate file: '" << _my_cert_filename << "'\n"
            "\tPrivate key file: '" << _my_private_key_filename << "'\n"
            "\tCA certificates path: " << (_ca_certificates_path ? string() + "'" + *_ca_certificates_path + "'" : "not set") << "\n"
            "\tCA certificate file: " <<  (_ca_certificate_filename ? string() + "'" + *_ca_certificate_filename + "'" : "not set") << "\n"
            "\tCA use default paths: " << _use_default_paths
        );
}


SslConfiguration::ContextPtr SslConfiguration::createContext(
            boost::asio::io_service& io_service
        )
{
    string ssl_function_name;

    LOG_DEBUG_MSG( "Creating context." );

    ContextPtr context_ptr;

    try {
        ssl_function_name = "constructor";
        context_ptr.reset( new Context(
                io_service,
                _use == Use::Client ? boost::asio::ssl::context::sslv23_client : boost::asio::ssl::context::sslv23_server
            ) );

        Context &context(*context_ptr);

        ssl_function_name = "set_options";
        context.set_options(
                boost::asio::ssl::context::default_workarounds
            );

        ssl_function_name = "set_verify_mode";
        boost::asio::ssl::context::verify_mode mode = boost::asio::ssl::context::verify_peer;
        if ( _use == Use::Client ) {
            mode |= boost::asio::ssl::context::verify_fail_if_no_peer_cert;
        } else {
            // only enable peer verification if requested
            if ( _certificate != Certificate::Optional ) {
                mode |= boost::asio::ssl::context::verify_fail_if_no_peer_cert;
            }
        }
        LOG_DEBUG_MSG( "verify mode: " << std::hex << mode );
        context.set_verify_mode( mode );

        ssl_function_name = "use_certificate_chain_file";
        context.use_certificate_chain_file(
                _my_cert_filename
            );

        ssl_function_name = "use_private_key_file";
        context.use_private_key_file(
                _my_private_key_filename,
                boost::asio::ssl::context::pem
            );

        // Configure CA stuff.

        if ( _ca_certificates_path ) {
            ssl_function_name = "add_verify_path";
            context.add_verify_path( *_ca_certificates_path );
        }

        if ( _ca_certificate_filename ) {
            ssl_function_name = "load_verify_file";
            context.load_verify_file( *_ca_certificate_filename );
        }
    } catch ( std::exception& e ) {
        THROW_SSL_ERROR( ssl_function_name );
    }

    if ( _use_default_paths ) {
        int ssl_rc(SSL_CTX_set_default_verify_paths( context_ptr->impl() ));
        if ( ssl_rc != 0 ) {
            THROW_SSL_ERROR( ssl_function_name );
        }
    }

    return context_ptr;
}


}
}
