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

#ifndef BGQ_UTILITY_CLIENT_PORT_CONFIGURATION_H_
#define BGQ_UTILITY_CLIENT_PORT_CONFIGURATION_H_


/*! \file
 *
 *  \brief Class for port configuration for clients.
 */

#include "PortConfiguration.h"

#include <boost/program_options.hpp>

#include <string>
#include <stdint.h>


namespace bgq {
namespace utility {


class SslConfiguration;


/*! \brief Port configuration for clients.
 *

This class provides configuration for the ports that a client
should attempt to connect to.
See Connector for a class that uses this configuration to connect
to the server.

Non-ssl configuration can be done explicitly,
or by taking input from the command line,
a properties file,
or using the default
(in that order).

Usage:

Add a property to bg.properties like this:
<pre>
[mysection]
host = localhost:32061
</pre>

This class uses 'host' as the default property name.
Replace 32061 with the port that your server is listening on.

If the host name is empty (a configuration like ":32061"),
the client will connect to localhost.

Example pseudo-code:

<pre>

namespace po = boost:program_options;

ClientPortConfiguration port_config( MyDefaultPort );

po::options_description desc( "Options" );

port_config.addTo( desc );

-- do program options stuff.

po::store( ... );
po::notify( ... );

Properties::Ptr properties_ptr( Properties::create() ); // use Properties::ProgramOptions

bgq::utility::initializeLogging( *properties_ptr ); // use bgq::utility::LoggingProgramOptions

port_config.setProperties( properties_ptr, MySectionName );

port_config.notifyComplete(); // throws exceptions.

// Use Connector to connect to the server and then use the socket.

</pre>

When you run this program it will get the ports to connect
to from the command line, the properties file, or use the default.

If you have more than one type of port that you connect to
(e.g., connections to different servers),
pass in a port_type_name
and port_type_description
to the constructor.

The port_type_name will be prefixed to the option name,
so the option on the command line will be --<i>port_type_name</i>-host,
and the config file property name will be
<i>port_type_name</i>_host.

The port_type_description will be used in the help text:
\"Server to open <i>description</i> connection to.\"

 *
 */
class ClientPortConfiguration : public PortConfiguration
{
public:


    struct ConnectionType {
        enum Value {
            Command, //!< Command connection, uses command certificate.
            Administrative //!< Administrative connection, uses administrative certificate.
        };
    };


    static const std::string DefaultHostname; //!< The default host name if no ports are given.
    static const std::string OptionName; //!< The name of the command-line option.
    static const std::string PropertyName; //!< The name of the property in the properties file.


    //! \brief Constructor to specify port.
    ClientPortConfiguration(
            uint32_t default_tcp_port,
            ConnectionType::Value connection_type = ConnectionType::Command
        );


    //! \brief Constructor to specify service name.
    ClientPortConfiguration(
            const std::string& default_service_name,
            ConnectionType::Value connection_type = ConnectionType::Command
        );

    //! \brief Constructor to specify port type.
    ClientPortConfiguration(
            const std::string& default_service_name, //!< The service name that will be used if none is provided.
            const std::string& port_type_name, //!< The port name, will be prepended to options
            const std::string& port_type_description, //!< The port description, will be embedded in the option description help text.
            ConnectionType::Value connection_type = ConnectionType::Command //!< [in]
        );


    /*! \brief Add the client port configuration options to the options description.
     *
     *  The option added is --host. The format of this parameter is as described in
     *  parsePortsStr(const std::string&,const std::string&,Pairs& pairs_out) .
     *
     *  The --host option can be supplied multiple times to attempt
     *  to connect to multiple ports.
     */
    void addTo(
            boost::program_options::options_description& options
        );

    ConnectionType::Value getConnectionType() const  { return _connection_type; }

    /*! \brief Create SSL configuration based on PortConfiguration.
     *
     *  Creates the SSL configuration.
     *  Called by Connector.
     *
     *  \pre notifyComplete() called.
     */
    SslConfiguration createSslConfiguration() const;


protected:

    //! \brief The property name is '[<i>name</i>_]host'.
    std::string _getPropertyName() const;

    //! \brief The default is "localhost".
    Pairs _getDefault() const;


private:

    std::string _name, _description;

    ConnectionType::Value _connection_type;

};

} // namespace bgq::utility
} // namespace bgq

#endif
