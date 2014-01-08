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


#include "portConfiguration/PortConfiguration.h"

#include "portConfiguration/SslConfiguration.h"

#include "Log.h"

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/throw_exception.hpp>
#include <boost/tokenizer.hpp>

#include <stdexcept>
#include <string>


using boost::lexical_cast;

using std::string;


LOG_DECLARE_FILE( "utility" );


namespace bgq {
namespace utility {


PortConfiguration::Pair PortConfiguration::parsePortStr( const std::string& port_str, const std::string& default_service_name )
{
    static const string bracketed_addr_re_str( "\\[([^\\]]*)\\]" ); // matches[1] == bracketed addr
    static const string nonbracketed_addr_re_str( "([^:]*)" ); // matches[2] == nonbracketed addr
    static const string addr_re_str( "(?:(?:" + bracketed_addr_re_str + ")|(?:" + nonbracketed_addr_re_str + "))" );
    static const string port_re_str(":(.*)"); // matches[3] == service name
    static const boost::regex addr_port_re( "(?:" + addr_re_str + ")?" + "(?:" + port_re_str + ")?" );

    boost::smatch matches;
    if ( ! boost::regex_match( port_str, matches, addr_port_re ) ) {
        BOOST_THROW_EXCEPTION( InvalidPortStr( string() + "host:port value '" + port_str + "' is not valid" ) );
    }

    string host_name(matches[1].str().empty() ? matches[2] : matches[1]);
    string service_name(matches[3].str().empty() ? default_service_name : matches[3]);

    return Pair( host_name, service_name );
}


void PortConfiguration::parsePortsStr( const std::string& ports_str, const std::string& default_service_name, Pairs& pairs_out )
{
    typedef boost::tokenizer<boost::escaped_list_separator<char> > Tr;

    // split up the string on ,s
    Tr toks( ports_str );

    for ( Tr::iterator tok_i(toks.begin()); tok_i != toks.end(); ++tok_i ) {
        Pair pair(parsePortStr( *tok_i, default_service_name ));
        pairs_out.push_back( pair );
    }
}


void PortConfiguration::parsePortsStrs( const Strings& ports_strs, const std::string& default_service_name, Pairs& pairs_out )
{
    for ( Strings::const_iterator i(ports_strs.begin()) ; i != ports_strs.end() ; ++i ) {
        parsePortsStr( *i, default_service_name, pairs_out );
    }
}


PortConfiguration::PortConfiguration(
        uint32_t default_tcp_port
    ) :
        _default_service_name(lexical_cast<string>( default_tcp_port ))
{
    // Nothing to do.
}


PortConfiguration::PortConfiguration(
        const std::string& default_service_name
    ) :
        _default_service_name(default_service_name)
{
    // Nothing to do.
}


void PortConfiguration::setProperties(
        Properties::ConstPtr properties_ptr,
        const std::string& section_name
    )
{
    _properties_ptr = properties_ptr;
    _section_name = section_name;
}


const PortConfiguration::Pairs& PortConfiguration::getPairs() const
{
    return _pairs;
}


void PortConfiguration::setPorts( const std::string& ports_str )
{
    _pairs.clear();
    parsePortsStr( ports_str, _default_service_name, _pairs );
}


void PortConfiguration::setPorts( const Strings& ports_strs )
{
    _pairs.clear();
    parsePortsStrs( ports_strs, _default_service_name, _pairs );
}


void PortConfiguration::notifyComplete()
{
    _handlePairsComplete();

    if ( _properties_ptr ) {
        try {
            _administrative_cn = _properties_ptr->getValue( "security.admin", "cn" );
        } catch ( std::exception& e ) {
            LOG_WARN_MSG( "Administrative CN isn't configured in the properties." );
        }

        try {
            _command_cn = _properties_ptr->getValue( "security.command", "cn" );
        } catch ( std::exception& e ) {
            LOG_WARN_MSG( "Command CN isn't configured in the properties." );
        }
    }
}


void PortConfiguration::_handlePairsComplete()
{
    if ( ! _pairs.empty() ) {
        return;
    }

    // If properties has been set, see if can use the value in the properties file.

    if ( _properties_ptr && (! _section_name.empty()) ) {
        try {
            string ports_str(_properties_ptr->getValue( _section_name, _getPropertyName() ));
            parsePortsStr( ports_str, _default_service_name, _pairs );
            return;
        } catch (const std::invalid_argument& e) {
            LOG_WARN_MSG( e.what() );
            // fall through and use default
        }
    }

    // Otherwise, use the default.

    _pairs = _getDefault();
}


} // namespace bgq::utility
} // namespace bgq
