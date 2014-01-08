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


LOG_DECLARE_FILE( "bgws.clients" );


namespace bgws_clients {


const std::string BgwsOptions::DEFAULT_BASE( "https://localhost/bg" );


BgwsOptions::BgwsOptions()
    :
        _use_session_file(true),
        _desc( "Blue Gene Web Service options" )
{
    namespace po = boost::program_options;

    _bg_properties_program_options.addTo( _desc );
    _desc.add_options()
            ( "session-file,s", po::value<string>()->notifier( boost::bind( &BgwsOptions::_notifySessionFile, this, _1 ) ), "Session file" )
            ( "base", po::value( &_base ), "BGWS base URL" )
            ( "session-id", po::value( &_session_id ), "Session ID" )
        ;
}


void BgwsOptions::setBgProperties( bgq::utility::Properties& properties )
{
    if ( _base.empty() ) {
        try {
            _base = properties.getValue( "bgws.clients", "base_url" );
            if ( _base.empty() ) {
                _base = DEFAULT_BASE;
            }
        } catch ( std::exception& e ) {
            LOG_DEBUG_MSG( "Failed to get BGWS base URL from properties file, will use default. The error is '" << e.what() << "'." );
            _base = DEFAULT_BASE;
        }
    }

    // If the last character of base is /, remove the /.
    if ( _base[_base.size() - 1] == '/' ) {
        _base = _base.substr( 0, _base.size() - 1 );
    }

    LOG_DEBUG_MSG( "base is " << _base );


    try {
        _ca_cert_filename = properties.getValue( "bgws.clients", "certificate_authority_certificate" );
    } catch ( std::exception& e ) {
        LOG_DEBUG_MSG( "Failed to get CA certificate filename from properties file, will not use a CA certificate file. The error is '" << e.what() << "'." );
    }
}


void BgwsOptions::_notifySessionFile( const std::string& s )
{
    if ( s.empty() ) {
        _use_session_file = false;
    } else {
        _use_session_file = true;
        _session_filename = s;
    }
}


} // namespace bgws_clients
