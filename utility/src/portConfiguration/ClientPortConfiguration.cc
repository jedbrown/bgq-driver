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


#include "portConfiguration/ClientPortConfiguration.h"

#include "portConfiguration/SslConfiguration.h"

#include "PairConnector.h"

#include "Log.h"

#include <boost/bind.hpp>
#include <boost/throw_exception.hpp>

#include <sstream>


using boost::bind;

using boost::asio::ip::tcp;

using std::ostringstream;
using std::string;


LOG_DECLARE_FILE( "utility" );


namespace bgq {
namespace utility {


const std::string ClientPortConfiguration::DefaultHostname( "" );
const std::string ClientPortConfiguration::OptionName( "host" );
const std::string ClientPortConfiguration::PropertyName( "host" );


ClientPortConfiguration::ClientPortConfiguration(
        uint32_t default_tcp_port,
        ConnectionType::Value connection_type
    ) :
        PortConfiguration( default_tcp_port ),
        _connection_type(connection_type)
{
    // Nothing to do.
}


ClientPortConfiguration::ClientPortConfiguration(
        const std::string& default_service_name,
        ConnectionType::Value connection_type
    ) :
        PortConfiguration( default_service_name ),
        _connection_type(connection_type)
{
    // Nothing to do.
}


ClientPortConfiguration::ClientPortConfiguration(
        const std::string& default_service_name,
        const std::string& port_type_name,
        const std::string& port_type_description,
        ConnectionType::Value connection_type
    ) :
        PortConfiguration( default_service_name ),
        _name(port_type_name),
        _description(port_type_description),
        _connection_type(connection_type)
{
    // Nothing to do.
}


void ClientPortConfiguration::addTo(
        boost::program_options::options_description& options
    )
{
    namespace po = boost::program_options;

    string option_name((_name.empty() ? "" : string() + _name + "-") + OptionName);

    string desc_text(_description.empty() ? "Server to connect to" : string() + "Server to open " + _description + " connection to" );

    options.add_options()
            ( option_name.c_str(),
              po::value<Strings>()->notifier(
                      boost::bind(
                              (void(ClientPortConfiguration::*)(const Strings&)) &ClientPortConfiguration::setPorts,
                              this,
                              _1
                          )
                  ),
              desc_text.c_str()
            )
        ;
}


SslConfiguration ClientPortConfiguration::createSslConfiguration() const
{
    SslConfiguration ssl_config(
            SslConfiguration::Use::Client,
            _connection_type == ClientPortConfiguration::ConnectionType::Command ?
                    SslConfiguration::Certificate::Command : SslConfiguration::Certificate::Administrative,
            _properties_ptr
        );

    return ssl_config;
}


std::string ClientPortConfiguration::_getPropertyName() const
{
    return (string() + (_name.empty() ? "" : string() + _name + "_") + PropertyName);
}


ClientPortConfiguration::Pairs ClientPortConfiguration::_getDefault() const
{
    Pairs pairs;

    pairs.push_back( Pair( DefaultHostname, getDefaultServiceName() ) );

    return pairs;
}

} // namespace bgq::utility
} // namespace bgq

