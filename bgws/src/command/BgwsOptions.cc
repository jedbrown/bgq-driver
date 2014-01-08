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

#include "BgwsOptions.hpp"

#include <boost/bind.hpp>

#include <utility/include/Log.h>


using std::string;


LOG_DECLARE_FILE( "bgws.command" );


namespace bgws {
namespace command {


const capena::http::uri::Uri BgwsOptions::DEFAULT_BASE(
        capena::http::uri::Uri::Security::Secure,
        "localhost:32071",
        capena::http::uri::Path() / "bg"
    );


BgwsOptions::BgwsOptions()
    :
        _desc( "Blue Gene Web Services options" )
{
    namespace po = boost::program_options;

    _bg_properties_program_options.addTo( _desc );
    _desc.add_options()
            ( "base", po::value<string>()->notifier( boost::bind( &BgwsOptions::_setBase, this, _1 ) ), "BGWS server base URL" )
        ;
}


void BgwsOptions::setBgProperties( bgq::utility::Properties::ConstPtr properties_ptr )
{
    if ( ! _base_ptr ) {
        try {
            _base_ptr.reset( new capena::http::uri::Uri(capena::http::uri::Uri::parse( properties_ptr->getValue( "bgws.commands", "base_url" ) )) );
        } catch ( std::exception& e ) {
            LOG_DEBUG_MSG( "Failed to get BGWS server base URL from properties file, will use default. The error is '" << e.what() << "'." );
            _base_ptr.reset( new capena::http::uri::Uri(DEFAULT_BASE) );
        }
    }

    LOG_DEBUG_MSG( "base is " << _base_ptr->calcString() );

    _ssl_configuration_ptr.reset( new bgq::utility::SslConfiguration(
            bgq::utility::SslConfiguration::Use::Client,
            bgq::utility::SslConfiguration::Certificate::Command,
            properties_ptr
        ) );
}


void BgwsOptions::_setBase( const std::string& base_str )
{
    _base_ptr.reset( new capena::http::uri::Uri(capena::http::uri::Uri::parse( base_str )) );
}


} } // namespace bgws::command
