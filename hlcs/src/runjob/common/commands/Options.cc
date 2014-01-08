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
#include "common/commands/Options.h"

#include "common/commands/convert.h"

#include "common/defaults.h"
#include "common/logging.h"
#include "common/properties.h"

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include <iostream>

LOG_DECLARE_FILE( runjob::log );

namespace po = boost::program_options;

namespace runjob {
namespace commands {

Options::~Options()
{

}

Options::Options(
        const std::string& service,
        const std::string& log,
        runjob::commands::Message::Tag::Type tag,
        unsigned int argc, 
        char** argv
        ) :
    AbstractOptions( argc, argv, log ),
    _job( 0 ),
    _tag( tag ),
    _options( "Options" ),
    _portConfiguration( service, bgq::utility::ClientPortConfiguration::ConnectionType::Command ),
    _request(),
    _timeout( runjob::defaults::CommandConnectTimeout )
{
    // create args
    _portConfiguration.addTo( _options );

    // add generic args
    AbstractOptions::add( _options );

    // add options
    _options.add_options()
        (
         "wait-for-server,w",
         boost::program_options::value(&_timeout)
         ->implicit_value( 5 ),
         "if the server is down, wait this number\nof seconds before retrying to connect"
        )
        ;
}

void
Options::validate() const
{
    try {
        this->doValidate();
    } catch ( const boost::program_options::error& e ) {
        std::cerr << e.what() << std::endl;
        throw;
    }
}

error::rc
Options::handle(
        const runjob::commands::Response::Ptr& response
        ) const
{
    if ( response->getError() ) {
        std::cerr << runjob::commands::Message::toString(_tag) << " failed: ";
        std::cerr << runjob::commands::error::toString( response->getError() ) << std::endl;
        std::cerr << response->getMessage() << std::endl;
        return response->getError();
    }

    // let command display results
    this->doHandle(response);

    return error::success;
}

void
Options::add(
        const std::string& section_name,
        boost::program_options::options_description& options,
        AbstractOptions::ExtraParser* extra
        )
{
    BOOST_FOREACH( const boost::shared_ptr<boost::program_options::option_description> option, _options.options() ) {
        options.add( option );
    }

    // parse
    this->parse( options, extra );

    // notify
    this->notify();

    // log version information
    std::ostringstream version;
    this->version( version );
    LOG_INFO_MSG( version.str() );

    // log host name
    LOG_INFO_MSG( this->hostname() );

    // set properties file in port configuration
    _portConfiguration.setProperties( this->getProperties(), section_name );
    _portConfiguration.notifyComplete();

    // create request
    switch ( _tag ) {
        case runjob::commands::Message::Tag::ChangeCiosConfig:
            _request.reset( new runjob::commands::request::ChangeCiosConfig );
            break;
        case runjob::commands::Message::Tag::DumpProctable:
            _request.reset( new runjob::commands::request::DumpProctable );
            break;
        case runjob::commands::Message::Tag::EndTool:
            _request.reset( new runjob::commands::request::EndTool );
            break;
        case runjob::commands::Message::Tag::JobAcl:
            _request.reset( new runjob::commands::request::JobAcl );
            break;
        case runjob::commands::Message::Tag::JobStatus:
            _request.reset( new runjob::commands::request::JobStatus );
            break;
        case runjob::commands::Message::Tag::KillJob:
            _request.reset( new runjob::commands::request::KillJob );
            break;
        case runjob::commands::Message::Tag::LocateRank:
            _request.reset( new runjob::commands::request::LocateRank );
            break;
        case runjob::commands::Message::Tag::LogLevel:
            _request.reset( new runjob::commands::request::LogLevel );
            break;
        case runjob::commands::Message::Tag::MuxStatus:
            _request.reset( new runjob::commands::request::MuxStatus );
            break;
        case runjob::commands::Message::Tag::RefreshConfig:
            _request.reset( new runjob::commands::request::RefreshConfig );
            break;
        case runjob::commands::Message::Tag::ServerStatus:
            _request.reset( new runjob::commands::request::ServerStatus );
            break;
        case runjob::commands::Message::Tag::ToolStatus:
            _request.reset( new runjob::commands::request::ToolStatus );
            break;
        default:
            LOG_FATAL_MSG( _tag );
            LOG_FATAL_MSG( runjob::commands::Message::toString( _tag ) );
            BOOST_ASSERT(!"unhandled request type");
    }
}

void
Options::help(
        std::ostream& os
        ) const
{
    os << _argv[0] << " " << this->usage() << std::endl;
    os << std::endl;
    os << this->description() << std::endl;
    os << std::endl;
    this->doHelp( os );
    os << std::endl;
}

std::string
Options::usage() const
{
    const unsigned total = _positionalArgs.max_total_count();
    if ( total == std::numeric_limits<unsigned>::max() ) {
        return std::string();
    }

    std::ostringstream os;
    for ( unsigned i = 0; i < total; ++i ) {
        if ( i != 0 ) {
            os << " ";
        }
        os << _positionalArgs.name_for_position( i );
    }

    return os.str();
}

void
Options::doHelp(
        std::ostream& os
        ) const
{
    os << _options;
}

} // commands
} // runjob
