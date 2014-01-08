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


#include "portConfiguration/ServerPortConfiguration.h"

#include "portConfiguration/SslConfiguration.h"

#include "Log.h"

#include <boost/bind.hpp>

#include <sstream>


using boost::bind;

using boost::asio::ip::tcp;

using std::ostringstream;
using std::string;


LOG_DECLARE_FILE( "utility" );


namespace bgq {
namespace utility {


const std::string ServerPortConfiguration::DefaultHostname( "localhost" );
const std::string ServerPortConfiguration::OptionName( "listen-port" );
const std::string ServerPortConfiguration::HiddenOptionName(OptionName + "s");
const std::string ServerPortConfiguration::PropertyName( "listen_ports" );


ServerPortConfiguration::ServerPortConfiguration(
        uint32_t default_tcp_port,
        ConnectionType::Value connection_type
    ) :
        PortConfiguration( default_tcp_port ),
        _connection_type(connection_type)
{
    // Nothing to do.
}


ServerPortConfiguration::ServerPortConfiguration(
        const std::string& default_service_name,
        ConnectionType::Value connection_type
    ) :
        PortConfiguration( default_service_name ),
        _connection_type(connection_type)
{
    // Nothing to do.
}


ServerPortConfiguration::ServerPortConfiguration(
        const std::string& default_service_name,
        const std::string& port_type_name,
        const std::string& port_type_description,
        ConnectionType::Value connection_type
    ) :
        PortConfiguration(
                default_service_name
            ),
        _name(port_type_name),
        _description(port_type_description),
        _connection_type(connection_type)
{
    // Nothing to do.
}


void ServerPortConfiguration::addTo(
        boost::program_options::options_description& visible_options,
        boost::program_options::options_description& hidden_options
    )
{
    namespace po = boost::program_options;

    string desc_text(string() + "Ports to accept" + (_description.empty() ? "" : string() + " " + _description) + " connections on");

    string visible_option_name((_name.empty() ? "" : string() + _name + "-") + OptionName);

    visible_options.add_options()
            ( visible_option_name.c_str(),
              po::value<Strings>()->notifier(
                      boost::bind(
                              (void(PortConfiguration::*)(const Strings&)) &PortConfiguration::setPorts,
                              this,
                              _1
                          )
                  ),
              desc_text.c_str()
            )
        ;

    string hidden_option_name((_name.empty() ? "" : string() + _name + "-") + HiddenOptionName);

    hidden_options.add_options()
            ( hidden_option_name.c_str(),
              po::value<Strings>()->notifier(
                      boost::bind(
                              (void(PortConfiguration::*)(const Strings&)) &PortConfiguration::setPorts,
                              this,
                              _1
                          )
                  ),
              desc_text.c_str()
            )
        ;
}


SslConfiguration ServerPortConfiguration::createSslConfiguration() const
{
    LOG_DEBUG_MSG( "creating ssl configuration with connection type " << _connection_type );
    SslConfiguration ssl_config(
            SslConfiguration::Use::Server,
            _connection_type == ConnectionType::Optional ? SslConfiguration::Certificate::Optional : SslConfiguration::Certificate::Administrative,
            _properties_ptr
        );

    return ssl_config;
}


std::string ServerPortConfiguration::_getPropertyName() const
{
    return (string() + (_name.empty() ? "" : string() + _name + "_") + PropertyName);
}


ServerPortConfiguration::Pairs ServerPortConfiguration::_getDefault() const
{
    Pairs pairs;

    pairs.push_back( Pair( DefaultHostname, getDefaultServiceName() ) );

    return pairs;
}


} // namespace bgq::utility
} // namespace bgq
