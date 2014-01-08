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
#include "mux/client/Timers.h"

#include "mux/client/Id.h"

#include "common/logging.h"

#include <boost/lexical_cast.hpp>

LOG_DECLARE_FILE( runjob::mux::log );

namespace runjob {
namespace mux {
namespace client {

namespace {
    const std::string FunctionName( "mux" );
}

Timers::Timers(
        const Id& id,
        performance::Counters::Container::Ptr counters
        ) :
    _id( id ),
    _counters( counters ),
    _init(),
    _plugin_verify(),
    _plugin_started(),
    _argument_parsing()
{

}

void
Timers::start_init()
{
    _init = _counters->create()
        ->function( FunctionName )
        ->subFunction( "initialize" )
        ->mode( bgq::utility::performance::Mode::Value::Basic )
        ->id( _id )
        ;
    _init->dismiss();
}

void
Timers::start_plugin_started()
{
    _plugin_started = _counters->create()
        ->function( FunctionName )
        ->id( _id )
        ->subFunction( "scheduler plugin started" )
        ->mode( bgq::utility::performance::Mode::Value::Extended )
        ;
}

void
Timers::start_plugin_verify()
{
    _plugin_verify= _counters->create()
        ->function( FunctionName )
        ->id( _id )
        ->subFunction( "scheduler plugin verify" )
        ->mode( bgq::utility::performance::Mode::Value::Extended )
        ;
    _plugin_verify->dismiss();
}

void
Timers::update(
        BGQDB::job::Id id
        )
{
    if ( _init ) {
        _init->id( id )->dismiss( false );
        _init.reset();
    }

    if ( _plugin_verify ) {
        _plugin_verify->id( id )->dismiss( false );
        _plugin_verify.reset();
    }

    if ( _plugin_started ) {
        _plugin_started->id( id )->dismiss( false );
        _plugin_started.reset();
    }

    // create a data point for argument parsing
    bgq::utility::performance::DataPoint dataPoint(
            "client",
            boost::lexical_cast<std::string>( id ),
            _argument_parsing
            );
    dataPoint.setSubFunction( "argument parsing" );
    dataPoint.setMode( bgq::utility::performance::Mode::Value::Basic );
    _counters->add( dataPoint );
}

void
Timers::stop_init()
{
    _init->stop();
}

void
Timers::stop_plugin_verify()
{
    _plugin_verify->stop();
}

void
Timers::set_argument_parsing(
        const boost::posix_time::time_duration& duration
        )
{
    _argument_parsing = duration;
}

} // client
} // mux
} // runjob
