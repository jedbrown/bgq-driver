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
#include "mux/client/Plugin.h"

#include "common/message/Result.h"

#include "common/Coordinates.h"

#include "mux/client/Id.h"
#include "mux/client/PluginUpdateResources.h"
#include "mux/client/Timers.h"

#include "hlcs/src/bgsched/runjob/NodeImpl.h"
#include "hlcs/src/bgsched/runjob/StartedImpl.h"
#include "hlcs/src/bgsched/runjob/TerminatedImpl.h"
#include "hlcs/src/bgsched/runjob/UserIdImpl.h"
#include "hlcs/src/bgsched/runjob/VerifyImpl.h"

#include "common/JobInfo.h"
#include "common/logging.h"

LOG_DECLARE_FILE( runjob::mux::log );

namespace runjob {
namespace mux {
namespace client {

Plugin::Plugin(
        const Id& id,
        Timers& timers,
        const runjob::mux::Plugin::WeakPtr& plugin
        ) :
    _id( id ),
    _timers( timers ),
    _plugin( plugin )
{

}

void
Plugin::verify(
        const pid_t pid,
        JobInfo& info,
        const message::Result::Ptr& result
        )
{
    LOG_TRACE_MSG( __FUNCTION__ );
    const runjob::mux::Plugin::Ptr plugin( _plugin.lock() );
    if ( !plugin ) return;

    LOGGING_DECLARE_LOCATION_MDC( _id );

    // Time this operation
    _timers.start_plugin_verify();

    try {
        // copy environment variables into a type the plugin
        // will understand
        bgsched::runjob::Verify::Environments envs;
        BOOST_FOREACH( const Environment& i, info.getEnvs() ) {
            const bgsched::runjob::Environment env( i.getKey(), i.getValue() );
            envs.push_back( env );
        }

        bgsched::runjob::Corner corner;
        corner.location(
                boost::lexical_cast<std::string>(
                    info.getSubBlock().corner() )
                );
        bgsched::runjob::Shape shape;
        shape.value(
                info.getSubBlock().shape()
                );

        bgsched::runjob::UserId::Pimpl uid(
                new bgsched::runjob::UserId::Impl
                );
        uid->_uid = info.getUserId()->getUid();
        BOOST_FOREACH( const auto& i, info.getUserId()->getGroups() ) {
            uid->_groups.push_back( i.first );
        }

        bgsched::runjob::Verify::Pimpl impl(
                new bgsched::runjob::Verify::Impl
                );
        impl->_pid = pid;
        impl->_block = info.getBlock();
        impl->_corner = corner;
        impl->_shape = shape;
        impl->_exe = info.getExe();
        impl->_args = info.getArgs();
        impl->_envs = envs;
        impl->_user = bgsched::runjob::UserId( uid );

        bgsched::runjob::Verify data( impl );
        plugin->getPlugin()->execute( data );

        // plugin failed the request
        if ( data.deny_job() == bgsched::runjob::Verify::DenyJob::Yes ) {
            LOG_WARN_MSG( "plugin verify failed: '" << data.message() << "'" );
            result->setMessage( "scheduler failed the job with: " + data.message() );
            result->setError( error_code::job_failed_to_start );
            return;
        }

        PluginUpdateResources( info, data );

        LOG_TRACE_MSG( "plugin verify succeeded" );
        _timers.stop_plugin_verify();

        // fall through
    } catch ( const std::exception& e ) {
        LOG_ERROR_MSG( e.what() );
        result->setMessage( e.what() );
        result->setError( error_code::job_failed_to_start );
        return;
    } catch ( ... ) {
        LOG_ERROR_MSG( "caught unknown exception in plugin verify" );
        result->setMessage( "plugin failed verify request" );
        result->setError( error_code::job_failed_to_start );
        return;
    }
}

void
Plugin::started(
        const pid_t pid,
        const BGQDB::job::Id job
        )
{
    LOG_TRACE_MSG( __FUNCTION__ );
    const runjob::mux::Plugin::Ptr plugin( _plugin.lock() );
    if ( !plugin ) return;

    // time this operation
    _timers.start_plugin_started();

    bgsched::runjob::Started::Pimpl impl(
            new bgsched::runjob::Started::Impl()
            );
    impl->pid( pid );
    impl->job( job );

    bgsched::runjob::Started data( impl );
    try {
        plugin->getPlugin()->execute( data );
    } catch ( ... ) {
        LOG_WARN_MSG( "caught unknown exception in plugin started" );
    }
}

void
Plugin::startError(
        const pid_t pid,
        const error_code::rc error,
        const std::string& message
        )
{
    LOG_TRACE_MSG( __FUNCTION__ );
    const runjob::mux::Plugin::Ptr plugin( _plugin.lock() );
    if ( !plugin ) return;

    bgsched::runjob::Terminated::Pimpl impl(
            new bgsched::runjob::Terminated::Impl
            );
    impl->_pid = pid;
    impl->_killTimeout = (error == error_code::kill_timeout);
    impl->_message = message;
    try {
        plugin->getPlugin()->execute( bgsched::runjob::Terminated(impl) );
    } catch ( ... ) {
        LOG_WARN_MSG( "caught unknown exception in plugin terminated" );
    }
}

void
Plugin::terminated(
        const pid_t pid,
        const BGQDB::job::Id job,
        const message::ExitJob::ConstPtr& msg
        )
{
    LOG_TRACE_MSG( __FUNCTION__ );
    const runjob::mux::Plugin::Ptr plugin( _plugin.lock() );
    if ( !plugin ) return;

    bgsched::runjob::Terminated::Pimpl impl(
            new bgsched::runjob::Terminated::Impl
            );

    BOOST_FOREACH( const auto& i, msg->_nodes ) {
        // convert to bgsched coordinates
        bgsched::runjob::Coordinates coordinates;
        coordinates.a( i._coordinates.a() );
        coordinates.b( i._coordinates.b() );
        coordinates.c( i._coordinates.c() );
        coordinates.d( i._coordinates.d() );
        coordinates.e( i._coordinates.e() );

        const bgsched::runjob::Node::Pimpl node_impl(
                new bgsched::runjob::Node::Impl(
                    boost::lexical_cast<std::string>( i._location ),
                    coordinates
                    )
                );
        impl->_nodes.push_back( bgsched::runjob::Node(node_impl) );
    }

    impl->_pid = pid;
    impl->_job = job;
    impl->_status = msg->_status;
    impl->_killTimeout = (msg->_error == error_code::kill_timeout);
    LOG_TRACE_MSG( "pid          : " << impl->_pid );
    LOG_TRACE_MSG( "job          : " << impl->_job );
    LOG_TRACE_MSG( "status       : " << impl->_status );
    LOG_TRACE_MSG( "kill timeout : " << std::boolalpha << impl->_killTimeout );
    LOG_TRACE_MSG( "failed nodes : " << impl->_nodes.size() );
    try {
        plugin->getPlugin()->execute( bgsched::runjob::Terminated(impl) );
    } catch ( ... ) {
        LOG_WARN_MSG( "caught unknown exception in plugin terminated" );
    }
}

} // client
} // mux
} // runjob
