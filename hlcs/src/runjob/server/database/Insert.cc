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
#include "server/database/Insert.h"

#include "common/error.h"
#include "common/Exception.h"
#include "common/logging.h"
#include "common/JobInfo.h"

#include "server/Job.h"

#include <bgq_util/include/Location.h>

#include <control/include/bgqconfig/BGQTopology.h>

#include <boost/numeric/conversion/cast.hpp>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace database {

Insert::Insert(
        const boost::shared_ptr<BGQDB::job::Operations>& operations
        ) :
    _operations( operations ),
    _nodesPerMidplane( bgq::util::Location::ComputeCardsOnNodeBoard * bgq::util::Location::NodeBoardsOnMidplane)
{

}

void
Insert::execute(
        const Job::Ptr& job,
        const size_t nodesUsed
        )
{
    const JobInfo& job_info( job->info() );
    BGQDB::job::InsertInfo db_info;

    db_info.setBlock( job_info.getBlock() );
    db_info.setExe( job_info.getExe() );
    db_info.setArgs( job_info.getArgs() );

    BGQDB::job::InsertInfo::Envs envs( job_info.getEnvs().begin(), job_info.getEnvs().end() );
    db_info.setEnvs( envs );

    db_info.setCwd( job_info.getCwd() );
    db_info.setMapping( job_info.getMapping().value() );
    db_info.setUserName( job_info.getUserId()->getUser() );
    db_info.setProcessesPerNode( 
            static_cast<uint16_t>( job_info.getRanksPerNode() )
            );
    db_info.setNp( job_info.getNp() );
    if ( job_info.getSubBlock().isValid() ) {
        const SubBlock& subBlock = job_info.getSubBlock();
        const Shape& shape = subBlock.shape();
        const Corner& corner = subBlock.corner();

        db_info.setShape(
                BGQDB::job::Shape(
                    static_cast<uint16_t>( shape.a() ),
                    static_cast<uint16_t>( shape.b() ),
                    static_cast<uint16_t>( shape.c() ),
                    static_cast<uint16_t>( shape.d() ),
                    static_cast<uint16_t>( shape.e() ),
                    corner.isCoreSet() ? 1 : bgq::util::Location::ComputeCardCoresOnBoard - 1
                    )
                );
        db_info.setCorner( boost::lexical_cast<std::string>(subBlock.corner()) );

        BGQDB::job::InsertInfo::NodeBoardPositions nodeBoards;

        // only add node boards if the entire midplane is not used by the job
        if ( nodesUsed != _nodesPerMidplane ) {
            BOOST_FOREACH( const Shape::Nodes::value_type& i, shape.nodes() ) {
                nodeBoards.push_back(
                        BGQTopology::nodeCardNameFromPos( i.first )
                        );
            }
            db_info.setNodeBoardPositions( nodeBoards );
        }
    }

    db_info.setSchedulerData( job_info.getSchedulerData() );
    db_info.setHostname( job_info.getHostname() );
    db_info.setPid( job_info.getPid() );
    db_info.setClient( boost::numeric_cast<unsigned>(job->client()) );

    try {
        BGQDB::job::Id id;
        _operations->insert( db_info, &id );
        job->_id = id;
    } catch ( const std::exception& e ) {
        LOG_RUNJOB_EXCEPTION( runjob::error_code::database_error, e.what() );
    }
}

} // database
} // server
} // runjob
