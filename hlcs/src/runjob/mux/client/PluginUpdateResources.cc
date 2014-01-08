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
#include "mux/client/PluginUpdateResources.h"

#include "common/JobInfo.h"

#include <boost/numeric/conversion/cast.hpp>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

namespace runjob {
namespace mux {
namespace client {

PluginUpdateResources::PluginUpdateResources(
        JobInfo& job,
        const bgsched::runjob::Verify& data
        )
{
    job.getSubBlock().corner().setBlockCoordinates(
            Coordinates(
                data.corner().coordinates().a(),
                data.corner().coordinates().b(),
                data.corner().coordinates().c(),
                data.corner().coordinates().d(),
                data.corner().coordinates().e()
                )
            );
    if ( !data.corner().location().empty() ) {
        job.getSubBlock().corner().setLocation( data.corner().location() );
    }

    if ( data.shape().valid() ) {
        job.getSubBlock().setShape(
                boost::lexical_cast<std::string>( data.shape().a() ) + "x" +
                boost::lexical_cast<std::string>( data.shape().b() ) + "x" +
                boost::lexical_cast<std::string>( data.shape().c() ) + "x" +
                boost::lexical_cast<std::string>( data.shape().d() ) + "x" +
                boost::lexical_cast<std::string>( data.shape().e() )
                );

        // only set core if it's not the default
        if ( data.shape().core() != bgq::util::Location::ComputeCardCoresOnBoard - 1 ) {
            job.getSubBlock().corner().setCore(
                    boost::numeric_cast<uint8_t>( data.shape().core() )
                    );
        }
    } else {
        job.getSubBlock().setShape( data.shape().value() );
    }

    job.setBlock( data.block() );
    job.setSchedulerData( data.scheduler_data() );
    job.setExe( data.exe() );
    job.setArgs( data.args() );

    // convert plugin environment variables into a type we understand
    JobInfo::EnvironmentVector envs;
    BOOST_FOREACH( const bgsched::runjob::Environment& i, data.envs() ) {
        const Environment env( i.getKey(), i.getValue() );
        envs.push_back( env );
    }

    // clear envs and add them again
    job.getEnvs().clear();
    job.addEnvironmentVector( envs );
}

} // client
} // mux
} // runjob

