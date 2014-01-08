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


#ifndef BGQ_UTILITY_SERVER_PORT_CONFIGURATION_H_
#define BGQ_UTILITY_SERVER_PORT_CONFIGURATION_H_


/*! \file
 *
 *  \brief Class for port configuration for servers.
 */


#include "PortConfiguration.h"

#include <boost/program_options.hpp>

#include <string>

#include <stdint.h>


namespace bgq {
namespace utility {


class SslConfiguration;


/*! \brief Port configuration for servers.
 *

This class provides configuration for the ports that a server
should accept connections on, and a method to start listening
on those ports.

Configuration can be done explicitly, or by taking input from
the command line, a properties file, or using the default (in that
order).

Usage:

Add a property to bg.properties like this:
<pre>
[mysection]
listen_ports = localhost:32061

</pre>

This class uses 'listen_ports' as the default property name.
Replace 32061 with the port that your server is listening on.

If the host name is empty (a configuration like ":32061"),
the server will accept connections on any interfaces.

Example pseudo-code:

<pre>

namespace po = boost:program_options;

ServerPortConfiguration port_config( MyDefaultPort );

po::options_description all_desc( "Options" );
po::options_description visible_desc( "Options" );

port_config.addTo( visible_desc, all_desc );

-- do program options stuff.

po::store( ... );
po::notify( ... );

Properties::Ptr properties_ptr( Properties::create() ); // use Properties::ProgramOptions

bgq::utility::initializeLogging( *properties_ptr ); // use bgq::utility::LoggingProgramOptions

port_config.setProperties( properties_ptr, MySectionName );

port_config.notifyComplete(); // throws exceptions.

// pass port_config to Acceptors and start accepting.

</pre>

Now, when you run the program it will get the ports to connect
to from the command line, the properties file, or use the default.

If you have more than one type of port that you accept connections on
(e.g., client connections and console connections),
pass in a port_type_name
and port_type_description
to the constructor.

The port_type_name will be prefixed to the option name, so
the option on the command line will be --<i>port_type_name</i>-listen-port
(and -listen-ports),
and the config file property name will be
<i>port_type_name</i>_listen_ports.

The port_type_description will be used in the help text:
\"Ports to accept <i>port_type_description</i> connections on\".

 *
 */
class ServerPortConfiguration : public PortConfiguration
{
public:

    /*! \brief The client certificate types to accept. */
    struct ConnectionType {
        enum Value {
            AdministrativeCommand, //!< Accept both administrative and command certificates.
            AdministrativeOnly,    //!< Accept only administrative certificate.
            Optional               //!< Client certificate is optional, will accept administrative, command, or no certificate.
        };
    };


    static const std::string DefaultHostname; //!< The default host name.
    static const std::string OptionName; //!< The base name of the command-line option.
    static const std::string HiddenOptionName; //!< The base name of the hidden option on the command-line.
    static const std::string PropertyName; //!< The base name of the property in the properties file.


    //! \brief Constructor to specify port.
    ServerPortConfiguration(
            uint32_t default_tcp_port, //!< [in]
            ConnectionType::Value connection_type = ConnectionType::AdministrativeCommand //!< [in]
        );

    //! \brief Constructor to specify service name.
    ServerPortConfiguration(
            const std::string& default_service_name, //!< [in]
            ConnectionType::Value connection_type = ConnectionType::AdministrativeCommand //!< [in]
        );

    //! \brief Constructor to specify port type.
    ServerPortConfiguration(
            const std::string& default_service_name, //!< The service name that will be used if none is provided.
            const std::string& port_type_name, //!< The port name, will be prepended to options
            const std::string& port_type_description, //!< The port description, will be embedded in the option description help text.
            ConnectionType::Value connection_type = ConnectionType::AdministrativeCommand //!< [in]
        );


    /*! \brief Add the server port configuration options to the options description.
     *
     *  The option added is --listen-port. The format of this parameter is as described in
     *  parsePortsStr(const std::string&,const std::string&,Pairs& pairs_out) .
     *
     *  The --listen-ports option is added to the hidden options and is an alias for
     *  --listen-port.
     *
     *  --listen-ports can be supplied multiple times.
     */
    void addTo(
            boost::program_options::options_description& visible_options,
            boost::program_options::options_description& hidden_options
        );

    ConnectionType::Value getConnectionType() const { return _connection_type; }

    /*! \brief Create SSL configuration based on PortConfiguration.
     *
     *  Creates the SSL configuration.
     *  Called by Connector.
     *
     *  \pre notifyComplete() called.
     */
    SslConfiguration createSslConfiguration() const;


protected:

    //! \brief The property name is '[<i>name</i>_]listen_ports'.
    std::string _getPropertyName() const;

    //! \brief The default is '127.0.0.1:<i>default_service_name</i>,[::1]:<i>default_service_name</i>'
    Pairs _getDefault() const;


private:

    std::string _name, _description;

    ConnectionType::Value _connection_type;

};


} // namespace bgq::utility
} // namespace bgq


#endif
