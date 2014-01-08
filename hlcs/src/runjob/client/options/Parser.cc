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
#include "client/options/Parser.h"

#include "client/options/Debug.h"
#include "client/options/Job.h"
#include "client/options/Label.h"
#include "client/options/Miscellaneous.h"
#include "client/options/ParseEnvironment.h"
#include "client/options/Resource.h"

#include "common/ExportedEnvironment.h"
#include "common/logging.h"
#include "common/properties.h"

#include <boost/spirit/home/phoenix/bind/bind_member_function.hpp>

#include <boost/spirit/home/phoenix/core.hpp>
#include <boost/spirit/home/phoenix/statement.hpp>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <algorithm>
#include <functional>

LOG_DECLARE_FILE( runjob::client::log );

namespace runjob {
namespace client {
namespace options {

Parser::Parser(
        unsigned int argc,
        char** argv
        ) :
    AbstractOptions( argc, argv, runjob::client::log ),
    _info(),
    _tool(),
    _descriptions(),
    _abstractOptions(),
    _startTime( boost::posix_time::microsec_clock::local_time() ),
    _mpmdArgc( 0 )
{
    // create all options descriptions
    _descriptions.push_back(
            Description::Ptr( new Job(_info) )
            );
    _descriptions.push_back(
            Description::Ptr( new Resource(_info) )
            );
    _descriptions.push_back(
            Description::Ptr( new Debug(_info, _tool) )
            );
    _descriptions.push_back(
            Description::Ptr( new Miscellaneous(_info) )
            );

    this->findMpmdSentinel();

    this->add( _abstractOptions );

    // combine all options descriptions with positional arguments into a single
    // options descriptions so we can parse them
    boost::program_options::options_description options;
    options.add( _abstractOptions );
    BOOST_FOREACH( const DescriptionPtr& i, _descriptions ) {
        i->combine( options );
    }

    {
        typedef std::vector<ExportedEnvironment> ExportedEnvironmentVector;

        // hidden options are not shown in help text
        options.add_options()
        (
         "exp_env", // backwards compatible with --exp_env
         boost::program_options::value<ExportedEnvironmentVector>()
         ->multitoken()
         ->composing()
         ->notifier( boost::bind(&JobInfo::addExportedEnvironmentVector, boost::ref(_info), _1) )
        )
        (
         "env_all", // backwards compatible with --env_all
         boost::program_options::bool_switch()
         ->notifier(
             // lambda (unnamed) function to check if the parameter value is true
             // if so, it invokes the JobInfo method to do so
             boost::phoenix::if_( boost::phoenix::arg_names::arg1 )
             [
             boost::phoenix::bind( &JobInfo::addAllEnvironment, boost::phoenix::ref(_info) )
             ]
            )
        )
        (
         "socket", // this is hidden because it typically comes from bg.properties
         boost::program_options::value<Miscellaneous::SocketString>()->
         default_value(runjob::defaults::MuxLocalSocket),
         "runjob_mux listen socket"
        )
        ;
    }

    this->parse( options );

    {
        ParseEnvironment( options, _vm );
    }

    this->notify();

    this->mpmdSyntax();

    LOG_DEBUG_MSG( this->hostname() );
}

Parser::~Parser()
{
    LOG_DEBUG_MSG( __FUNCTION__ );
}

void
Parser::help(
        std::ostream& stream
        ) const
{
    stream << _argv[0] << " [options] : exe arg1 arg2 ... argn" << std::endl;
    stream << std::endl;
    BOOST_FOREACH( const DescriptionPtr& i, _descriptions ) {
        i->help( stream );
    }
    stream << _abstractOptions << std::endl;
    stream << std::endl;
    stream << "for more information, consult the man page" << std::endl;
    stream << std::endl;
}

std::string
Parser::getSocket() const
{
    std::string result;

    if ( !_vm["socket"].defaulted() ) {
        // given specific arg, use it
        result = _vm["socket"].as<Miscellaneous::SocketString>();
    } else {
        // look in properties
        try {
            result = this->getProperties()->getValue(runjob::mux::PropertiesSection, "local_socket");
            LOG_INFO_MSG( "set local socket to " << result << " from properties file" );
        } catch ( const std::invalid_argument& e ) {
            // this isn't fatal, we'll use the default
            LOG_WARN_MSG(
                    "missing local_socket key from " << runjob::mux::PropertiesSection << " section in properties file" 
                    );
            result = _vm["socket"].as<std::string>();
        }
    }

    return result;
}

options::Job::Timeout::Type
Parser::getTimeout() const
{
    if ( !_vm["timeout"].empty() ) {
        return _vm["timeout"].as<options::Job::Timeout>();
    } else {
        return 0;
    }
}

const Label&
Parser::getLabel() const
{
    return _vm["label"].as<Label>();
}

bool
Parser::getRaise() const
{
    return _vm["raise"].as<bool>();
}

boost::posix_time::time_duration
Parser::getDuration() const
{
    boost::posix_time::ptime now(
            boost::posix_time::microsec_clock::local_time()
            );

    return now - _startTime;
}

void
Parser::findMpmdSentinel()
{
    // look for first : token
    char** token = std::find_if(
            _argv,
            _argv + _argc,
            boost::bind(
                &strcmp, ":", _1
                ) == 0
            );

    if ( token == _argv + _argc ) {
        return;
    }

    _mpmdArgc = _argc;
    _argc = static_cast<unsigned>(token - _argv);
}

void
Parser::mpmdSyntax()
{
    if ( !_mpmdArgc ) {
        // warn about deprecated exe and args syntax   

        return;
    }

    unsigned int i = _argc + 1;
    LOG_TRACE_MSG( "parsing MPMD syntax starting at token " << i );
    LOG_TRACE_MSG( "argc: " << _mpmdArgc );

    // first argument is executable
    if ( i == _mpmdArgc ) {
        BOOST_THROW_EXCEPTION(
                std::invalid_argument( "missing executable after : token" )
                );
    }

    // ensure --exe was not provided
    if ( !_info.getExe().empty() ) {
        BOOST_THROW_EXCEPTION(
                std::invalid_argument( "both --exe and : exe provided" )
                );
    }
        
    _info.setExe( _argv[i] );
    LOG_TRACE_MSG( "MPMD executable: " << _argv[i] );

    // ensure --args were not provided
    if ( !_info.getArgs().empty() ) {
        BOOST_THROW_EXCEPTION(
                std::invalid_argument( "both --args and : exe provided" )
                );
    }

    std::vector<std::string> args;
    while ( ++i != _mpmdArgc ) {
        args.push_back( _argv[i] );
        LOG_TRACE_MSG( "MPMD argv[" << i - _argc - 2 << "] " << _argv[i] );
    }
    _info.setArgs( args );
}

} // options
} // client
} // runjob
