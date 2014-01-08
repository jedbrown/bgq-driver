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


#ifndef BGQ_UTILITY_SSL_CONFIGURATION_H_
#define BGQ_UTILITY_SSL_CONFIGURATION_H_

/*! \file
 *
 *  \brief Class for SSL configuration.
 */

#include <utility/include/Properties.h>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/asio/ssl.hpp>

#include <string>


namespace bgq {
namespace utility {

/*! \brief Class for SSL configuration.
 *
 * SSL options are read from the Properties.
 *
 */
class SslConfiguration
{
public:

    typedef boost::asio::ssl::context Context;
    typedef boost::shared_ptr<Context> ContextPtr;

    typedef boost::shared_ptr<std::string> StringPtr;


    struct Use {
        enum Value {
            Client, //!< Used for client connections.
            Server  //!< Used for server connections.
        };
    };

    struct Certificate {
        enum Value {
            Administrative, //!< Present the administrative certificate
            Command,        //!< Present the command certificate.
            Optional        //!< Client certificate is optional
        };
    };


    static const std::string AdministratorCertificateSectionName;
    static const std::string CommandCertificateSectionName;
    static const std::string CASectionName;


    /*! \brief Constructor */
    SslConfiguration(
            Use::Value use, //!< Used for client or server.
            Certificate::Value certificate, //!< Certificate to use.
            Properties::ConstPtr properties_ptr //!< Properties pointer.
        );


    Use::Value getUse() const  { return _use; }
    const std::string& getMyCertFilename() const  { return _my_cert_filename; }
    const std::string& getMyPrivateKeyFilename() const  { return _my_private_key_filename; }
    StringPtr getCaCertificatesPath() const  { return _ca_certificates_path; }
    StringPtr getCaCertificateFilename() const  { return _ca_certificate_filename; }
    bool getCaUseDefaultPaths() const  { return _use_default_paths; }


    /*! \brief Create a context.
     *
     */
    ContextPtr createContext(
            boost::asio::io_service& io_service
        );


private:

    const Use::Value _use;
    const Certificate::Value _certificate;
    std::string _my_cert_filename, _my_private_key_filename;
    StringPtr _ca_certificates_path;
    StringPtr _ca_certificate_filename;
    bool _use_default_paths;
};

}
}

#endif
