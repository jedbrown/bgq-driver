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
#include "server/job/Load.h"

#include "server/block/IoNode.h"

#include "server/cios/Message.h"

#include "server/job/SubNodePacing.h"

#include "common/defaults.h"
#include "common/logging.h"
#include "common/JobInfo.h"

#include "server/Job.h"

#include <boost/numeric/conversion/cast.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {

void
Load::create(
        const Job::Ptr& job
        )
{
    job->status().set( Status::Loading, job );

    const Load::Ptr result( new Load(job) );
}

Load::Load(
        const Job::Ptr& job
        ) :
    _job( job ),
    _message( cios::Message::create(bgcios::jobctl::LoadJob, _job->id()) )
{
    this->populate();

    BOOST_FOREACH( IoNode::Map::value_type& i, _job->io() ) {
        IoNode& node = i.second;
        if ( _job->pacing() ) {
            _job->pacing()->add( _message, _job );
        } else {
            node.writeControl( _message );
        }
    }
}

void
Load::populate()
{
    const JobInfo& info = _job->info();

    const boost::shared_ptr<bgcios::jobctl::LoadJobMessage> message = _message->as<bgcios::jobctl::LoadJobMessage>();

    // add executable
    int offset = snprintf( message->arguments, sizeof(message->arguments), "%s%c", info.getExe().c_str(), '\0' );
    message->numArguments = 1;
    LOG_TRACE_MSG( "loading job: " << info.getExe() );

    // add arguments
    LOG_TRACE_MSG(
            info.getArgs().size() << 
            (info.getArgs().size() == 1 ? " argument" : " arguments")
            );
    BOOST_FOREACH( const std::string& arg, info.getArgs() ) {
        offset += snprintf( message->arguments + offset, sizeof(message->arguments) - offset, "%s%c", arg.c_str(), '\0' );
        LOG_TRACE_MSG( "adding arg " << arg );
        message->numArguments++;
    }
    try {
        message->argumentSize = boost::numeric_cast<uint16_t>(offset);
    } catch ( const boost::bad_numeric_cast& e ) {
        LOG_WARN_MSG( "argument size: " << e.what() );
        message->argumentSize = std::numeric_limits<uint16_t>::max();
    }
    LOG_TRACE_MSG( 
            "arg size: " << 
            message->argumentSize << 
            " bytes for " << 
            message->numArguments << 
            (message->numArguments == 1 ? " argument" : " arguments")
            );

    // add user environment variables
    offset = 0;
    LOG_TRACE_MSG( 
            info.getEnvs().size() <<
            " environment " <<
            (info.getEnvs().size() == 1 ? "variable" : "variables")
            );
    BOOST_FOREACH( const std::string& env, info.getEnvs() ) {
        offset += snprintf( message->variables + offset, sizeof(message->variables) - offset, "%s%c", env.c_str(), '\0' );
        LOG_TRACE_MSG( "adding env " << env );
        message->numVariables++;
    }
    try {
        message->variableSize = boost::numeric_cast<uint16_t>(offset);
    } catch ( const boost::bad_numeric_cast& e ) {
        LOG_WARN_MSG( "variable size: " << e.what() );
        message->variableSize = std::numeric_limits<uint16_t>::max();
    }
    LOG_TRACE_MSG(
            "env size: " <<
            message->variableSize <<
            " bytes for " << 
            message->numVariables <<
            (message->numVariables == 1 ? " variable" : " variables")
            );

    // set cwd
    strncpy( message->workingDirectoryPath, info.getCwd().c_str(), sizeof(message->workingDirectoryPath) );

    // set stdin rank
    try {
        message->rankForStdin = boost::numeric_cast<uint32_t>( info.getStdinRank() );
    } catch ( const boost::bad_numeric_cast& e ) {
        LOG_WARN_MSG( "stdin rank: " << e.what() );
        message->rankForStdin = defaults::ClientStdinRank;
    }
    LOG_TRACE_MSG( "stdin rank: " << message->rankForStdin );

    // set strace rank
    if ( info.getStrace().getScope() == Strace::None ) {
        message->rankForStrace = bgcios::jobctl::DisableStrace;
        LOG_TRACE_MSG( "strace disabled" );
    } else {
        try {
            message->rankForStrace = boost::numeric_cast<int32_t>( info.getStrace().getRank() );
        } catch ( const boost::bad_numeric_cast& e ) {
            LOG_WARN_MSG( "strace: " << e.what() );
            message->rankForStrace = bgcios::jobctl::DisableStrace;
        }
        LOG_TRACE_MSG( "strace rank: " << message->rankForStrace );
    }

    // set umask
    message->umask = info.getUmask();
    LOG_TRACE_MSG( "umask: " << std::setw(4) << std::setfill('0') << std::hex << message->umask );

    this->addUserCredentials( message );
}

void
Load::addUserCredentials(
    const boost::shared_ptr<bgcios::jobctl::LoadJobMessage>& message
    )
{
    const JobInfo& info = _job->info();
    const bgq::utility::UserId::ConstPtr uid = info.getUserId();
    message->userId = uid->getUid();
    LOG_TRACE_MSG( "uid " << uid->getUid() );

    message->numSecondaryGroups = 0;
    BOOST_FOREACH( const bgq::utility::UserId::Group& i, uid->getGroups() ) {
        const gid_t gid = i.first;
        const std::string& name = i.second;

        if ( message->numSecondaryGroups == 0 ) {
            // primary group is the first entry in the container
            message->groupId = gid;
            LOG_TRACE_MSG( "primary gid " << gid );
            message->numSecondaryGroups = 1;
        } else if ( message->numSecondaryGroups <= bgcios::jobctl::MaxGroups ) {
            // everything else is a secondary group
            // we use the numSecondaryGroups as an index here
            message->secondaryGroups[ message->numSecondaryGroups - 1 ] = gid;
            LOG_TRACE_MSG(
                    "secondary gid " << message->numSecondaryGroups << " of " << uid->getGroups().size() - 1 << " " 
                    << message->secondaryGroups[ message->numSecondaryGroups - 1 ]
                    );
            message->numSecondaryGroups++;
        } else {
            LOG_WARN_MSG( 
                    "ignoring group " << name << " (" << gid << ") due to maximum group limit of " << bgcios::jobctl::MaxGroups 
                    );
        }
    }

    if ( message->numSecondaryGroups > 0 ) {
        // subtract one since the primary group is not included in the secondary group array
        message->numSecondaryGroups--;
        LOG_TRACE_MSG( "added " << message->numSecondaryGroups << " secondary groups" );
    } else {
        LOG_WARN_MSG( "no groups defined" );
    }
}

} // job
} // server
} // runjob
