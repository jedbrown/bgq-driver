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

#include "job/Operations.h"

#include "sqlStrings.gen.h"

#include "job/exception.h"

#include "cxxdb/cxxdb.h"

#include "tableapi/DBConnectionPool.h"

#include "tableapi/gensrc/bgqtableapi.h"

#include <bgq_util/include/Location.h>

#include <utility/include/Log.h>

#include <boost/assign.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <stdexcept>
#include <string>
#include <vector>

using boost::lexical_cast;

using std::string;

LOG_DECLARE_FILE( "database" );

namespace BGQDB {
namespace job {

Operations::Operations()
{
    LOG_INFO_MSG( "Initializing BGQDB::job::Operations" );

    _insert_conn_ptr = DBConnectionPool::instance().getConnection();

    _insert_query_block_status_stmt_ptr = _insert_conn_ptr->prepareQuery(
            sql::INSERT_QUERY_BLOCK_STATUS,
            sql::INSERT_QUERY_BLOCK_STATUS_PARAMETER_NAMES
        );

    _insert_query_sub_node_user_stmt_ptr = _insert_conn_ptr->prepareQuery(
            sql::INSERT_QUERY_SUB_NODE_USER,
            sql::INSERT_QUERY_SUB_NODE_USER_PARAMETER_NAMES
        );

    // Prepare the SELECT-INSERT statement
    // allows getting the generated job ID when do the insert.

    _insert_job_stmt_ptr = _insert_conn_ptr->prepareQuery(
            sql::INSERT_JOB,
            sql::INSERT_JOB_PARAMETER_NAMES
        );

    _insert_job_map_stmt_ptr = _insert_conn_ptr->prepareUpdate(
            "INSERT INTO TBGQJobMap (" +
            DBTJobmap::ID_COL + "," +
            DBTJobmap::NODEBOARD_COL + "," +
            DBTJobmap::MIDPLANE_COL +
            ") VALUES (?,?,?)",
            boost::assign::list_of
                ( DBTJobmap::ID_COL )
                ( DBTJobmap::NODEBOARD_COL )
                ( DBTJobmap::MIDPLANE_COL )
        );

    _update_conn_ptr = DBConnectionPool::instance().getConnection();

    _update_stmt_ptr = _update_conn_ptr->prepareUpdate(
            string() + "UPDATE " + DBTJob().getTableName() + " SET " + DBTJob::STATUS_COL + " = ? WHERE " + DBTJob::ID_COL + " = ?",
            boost::assign::list_of( DBTJob::STATUS_COL )( DBTJob::ID_COL )
        );

    _remove_conn_ptr = DBConnectionPool::instance().getConnection();

    _remove_insert_job_history_stmt_ptr = _remove_conn_ptr->prepareUpdate(
            sql::REMOVE_INSERT_HISTORY,
            sql::REMOVE_INSERT_HISTORY_PARAMETER_NAMES
        );

    _remove_delete_job_stmt_ptr = _remove_conn_ptr->prepareUpdate( string() +
            "DELETE FROM " + DBTJob().getTableName() + " WHERE " + DBTJob::ID_COL + " = ?",
            boost::assign::list_of( DBTJob::ID_COL )
        );

    LOG_DEBUG_MSG( "Finished initializing db::job::Operations" );
}


void Operations::insert(
        const InsertInfo& info,
        Id *id_out
    )
{
    boost::lock_guard<boost::mutex> lg(_insert_mtx);

    LOG_DEBUG_MSG(
            "Inserting new job"
            " on '" << info.getBlock() << "'"
            " for '" << info.getUserName() << "'"
        );

    // use a transaction to query the block table so we can ensure the status
    // is initialized prior to inserting the job
    cxxdb::Transaction tx( *_insert_conn_ptr );

    Shape block_shape;

    std::string qualifier;

    _checkBlockInitialized(
            info.getBlock(),
            block_shape,
            qualifier
        );

    _checkSubNodeUser(
            info.getUserName(),
            info.getCorner()
            );

    bool args_truncated(false), envs_truncated(false);

    _insertJob(
            info,
            block_shape,
            qualifier,
            id_out,
            &args_truncated,
            &envs_truncated
        );

    _insertJobMap(
            *id_out,
            info
        );

    _insert_conn_ptr->commit();

    LOG_INFO_MSG(
            "Inserted job " << *id_out <<
            " on '" << info.getBlock() << "'" <<
            " with qualifier '" << qualifier << "'" <<
            " for '" << info.getUserName() << "'" <<
            (args_truncated ? " (arguments truncated)" : "") <<
            (envs_truncated ? " (envs truncated)" : "")
        );
}


void Operations::update(
        Id id,
        status::Value new_status
    )
{
    boost::lock_guard<boost::mutex> lg(_update_mtx);

    if ( ! (new_status == status::Cleanup || new_status == status::Debug || new_status == status::Loading || new_status == status::Running || new_status == status::Starting || new_status == status::Setup) ) {
        BOOST_THROW_EXCEPTION( exception::InvalidStatus( string() +
                "cannot set job status to " + lexical_cast<string>(int(new_status))
            ) );
    }

    const string &status_code(
            new_status == status::Cleanup ? status_code::Cleanup :
            new_status == status::Debug ? status_code::Debug :
            new_status == status::Loading ? status_code::Loading :
            new_status == status::Running ? status_code::Running :
            new_status == status::Setup ? status_code::Setup :
            status_code::Starting
        );

    _update_stmt_ptr->parameters()[DBTJob::STATUS_COL].set( status_code );

    _update_stmt_ptr->parameters()[DBTJob::ID_COL].cast( id );

    unsigned affected_rows(0);

    _update_stmt_ptr->execute( &affected_rows );

    if ( affected_rows != 1 ) {
        BOOST_THROW_EXCEPTION( exception::JobNotFound( string() +
                "couldn't update status for job " + lexical_cast<string>( id ) + " because it doesn't exist."
            ) );
    }

    LOG_DEBUG_MSG( "Set status for " << id << " to " << status_code );
}


void Operations::remove(
        Id job_id,
        const RemoveInfo& info
    )
{
    boost::lock_guard<boost::mutex> lg(_remove_mtx);

    LOG_INFO_MSG( "Removing job " << job_id );

    // use a transaction to delete from TBGQJob and insert into TBGQJob_history
    cxxdb::Transaction tx( *_remove_conn_ptr );
    bool truncate_error_text = false;

    if ( info.isTerminated() ) {
        _remove_insert_job_history_stmt_ptr->parameters()[ DBTJob_history::STATUS_COL ].set( status_code::Terminated );
        _remove_insert_job_history_stmt_ptr->parameters()[ DBTJob_history::EXITSTATUS_COL ].set( info.getExitStatus() );
        if ( info.getMessage().empty() ) {
            _remove_insert_job_history_stmt_ptr->parameters()[ DBTJob_history::ERRTEXT_COL ].setNull();
        } else {
            _remove_insert_job_history_stmt_ptr->parameters()[ DBTJob_history::ERRTEXT_COL ].set( info.getMessage(), &truncate_error_text );
        }
    } else {
        _remove_insert_job_history_stmt_ptr->parameters()[ DBTJob_history::STATUS_COL ].set( status_code::Error );
        _remove_insert_job_history_stmt_ptr->parameters()[ DBTJob_history::EXITSTATUS_COL ].setNull();
        _remove_insert_job_history_stmt_ptr->parameters()[ DBTJob_history::ERRTEXT_COL ].set( info.getMessage(), &truncate_error_text );
    }

    _remove_insert_job_history_stmt_ptr->parameters()[ DBTJob::ID_COL ].cast( job_id );

    unsigned affected_rows(0);

    _remove_insert_job_history_stmt_ptr->execute( &affected_rows );

    if ( affected_rows != 1 ) {
        BOOST_THROW_EXCEPTION( exception::JobNotFound( string() +
                "could not remove job " + lexical_cast<string>( job_id ) + " because the job doesn't exist"
            ) );
    }


    _remove_delete_job_stmt_ptr->parameters()[ DBTJob::ID_COL ].cast( job_id );

    _remove_delete_job_stmt_ptr->execute( &affected_rows );

    if ( affected_rows != 1 ) {
        BOOST_THROW_EXCEPTION( exception::JobNotFound( string() +
                "could not remove job " + lexical_cast<string>( job_id ) + " because the job doesn't exist"
            ) );
    }

    if ( truncate_error_text ) {
        LOG_WARN_MSG( "error text truncated" );
    }
    _remove_conn_ptr->commit();
}


void Operations::_checkBlockInitialized(
        const std::string& block_id,
        Shape& block_shape_out,
        std::string& qualifier_out
    )
{
    LOG_DEBUG_MSG( "Check if block '" + block_id + "' has correct state for new job." );

    _insert_query_block_status_stmt_ptr->parameters()[DBTBlock::BLOCKID_COL].set( block_id );

    cxxdb::ResultSetPtr rs_ptr(_insert_query_block_status_stmt_ptr->execute());

    if ( ! rs_ptr->fetch() ) {
        BOOST_THROW_EXCEPTION( exception::BlockNotFound( "could not insert job because there's no block '" + block_id + "'" ) );
    }

    const string block_status_code(rs_ptr->columns()[DBTBlock::STATUS_COL].getString());

    if ( block_status_code != BLOCK_INITIALIZED ) {
        BOOST_THROW_EXCEPTION( exception::BlockNotInitialized( string() +
                "could not insert job because '" + block_id + "' has status " + block_status_code + " rather than I"
            ) );
    }
    
    const string block_action_code(rs_ptr->columns()[DBTBlock::ACTION_COL].getString());
    if ( !block_action_code.empty() && block_action_code != BLOCK_NO_ACTION ) {
        BOOST_THROW_EXCEPTION( exception::BlockActionNotEmpty( string() +
                "could not insert job because '" + block_id + "' has action " + block_action_code
            ) );
    }

    block_shape_out = Shape(
            rs_ptr->columns()[DBTBlock::SIZEA_COL].as<uint16_t>(),
            rs_ptr->columns()[DBTBlock::SIZEB_COL].as<uint16_t>(),
            rs_ptr->columns()[DBTBlock::SIZEC_COL].as<uint16_t>(),
            rs_ptr->columns()[DBTBlock::SIZED_COL].as<uint16_t>(),
            rs_ptr->columns()[DBTBlock::SIZEE_COL].as<uint16_t>()
        );

    if ( rs_ptr->columns()[DBTBlock::QUALIFIER_COL].isNull() ) {
        qualifier_out.clear();
    } else {
        qualifier_out = rs_ptr->columns()[DBTBlock::QUALIFIER_COL].getString();
    }

    // everything's OK.
    LOG_DEBUG_MSG( "Block exists and status is correct." );
}

void Operations::_checkSubNodeUser(
        const std::string& user,
        const std::string& corner
        )
{
    if ( corner.empty() ) return;

    // validate corner location is a sub-node core
    const bgq::util::Location location( corner );
    if ( location.getType() != bgq::util::Location::ComputeCardCoreOnNodeBoard ) {
        LOG_DEBUG_MSG( "corner '" << corner << "' is not a compute card core" );
        return;
    }

    // corner is in the format Rxx-Mx-Nxx-Jxx-Cxx
    // we want to strip off the Cxx
    const std::string like_corner(
            corner.substr(0, 15) + "%"
            );
    _insert_query_sub_node_user_stmt_ptr->parameters()[DBTJob::USERNAME_COL].set( user );
    _insert_query_sub_node_user_stmt_ptr->parameters()[DBTJob::CORNER_COL].set( like_corner );

    cxxdb::ResultSetPtr rs_ptr(_insert_query_sub_node_user_stmt_ptr->execute());

    if ( ! rs_ptr->fetch() ) {
        // everything's OK.
        LOG_DEBUG_MSG( "Users other than '" << user << "' are not running on '" << like_corner << "'" );
        return;
    }

    BOOST_THROW_EXCEPTION(
            exception::SubNodeJobUserConflict(
                string() +
                "could not insert job because user '" +
                rs_ptr->columns()[DBTJob::USERNAME_COL].getString() +
                "' has a job running on core '" +
                rs_ptr->columns()[DBTJob::CORNER_COL].getString() +
                "'"
                )
            );
}


void Operations::_insertJob(
        const InsertInfo& info,
        const Shape& block_shape,
        const std::string& qualifier,
        Id *id_out,
        bool *args_truncated_out,
        bool *envs_truncated_out
    )
{

    const Shape &shape(info.getShape() == Shape::Block ? block_shape : info.getShape()); // use block shape if info says to use that.

    _insert_job_stmt_ptr->parameters()[ DBTJob::ARGS_COL ].set( InsertInfo::format( info.getArgs() ), args_truncated_out ); // it's ok to truncate args in the db.

    _insert_job_stmt_ptr->parameters()[ DBTJob::BLOCKID_COL ].set( info.getBlock() );
    if ( info.getCorner().empty() ) {
        _insert_job_stmt_ptr->parameters()[ DBTJob::CORNER_COL ].setNull();
    } else {
        _insert_job_stmt_ptr->parameters()[ DBTJob::CORNER_COL ].set( info.getCorner() );
    }

    _insert_job_stmt_ptr->parameters()[ DBTJob::ENVS_COL ].set( InsertInfo::format( info.getEnvs() ), envs_truncated_out ); // truncate environmentals, too.

    _insert_job_stmt_ptr->parameters()[ DBTJob::EXECUTABLE_COL ].set( info.getExe() );
    _insert_job_stmt_ptr->parameters()[ DBTJob::HOSTNAME_COL ].set( info.getHostname() );
    _insert_job_stmt_ptr->parameters()[ DBTJob::MAPPING_COL ].set( info.getMapping() );
    _insert_job_stmt_ptr->parameters()[ DBTJob::NODESUSED_COL ].cast( shape.getSize() );
    _insert_job_stmt_ptr->parameters()[ DBTJob::PID_COL ].set( info.getPid() );
    _insert_job_stmt_ptr->parameters()[ DBTJob::PROCESSESPERNODE_COL ].cast( info.getProcessesPerNode() );
    _insert_job_stmt_ptr->parameters()[ DBTJob::NP_COL ].cast( info.getNp() );
    _insert_job_stmt_ptr->parameters()[ DBTJob::CLIENT_COL ].cast( info.getClient() );

    if ( info.getSchedulerData().empty() ) {
        _insert_job_stmt_ptr->parameters()[ DBTJob::SCHEDULERDATA_COL ].setNull();
    } else {
        _insert_job_stmt_ptr->parameters()[ DBTJob::SCHEDULERDATA_COL ].set( info.getSchedulerData() );
    }

    _insert_job_stmt_ptr->parameters()[ DBTJob::SHAPEA_COL ].cast( shape.getA() );
    _insert_job_stmt_ptr->parameters()[ DBTJob::SHAPEB_COL ].cast( shape.getB() );
    _insert_job_stmt_ptr->parameters()[ DBTJob::SHAPEC_COL ].cast( shape.getC() );
    _insert_job_stmt_ptr->parameters()[ DBTJob::SHAPED_COL ].cast( shape.getD() );
    _insert_job_stmt_ptr->parameters()[ DBTJob::SHAPEE_COL ].cast( shape.getE() );

    _insert_job_stmt_ptr->parameters()[ DBTJob::USERNAME_COL ].set( info.getUserName() );
    _insert_job_stmt_ptr->parameters()[ DBTJob::WORKINGDIR_COL ].set( info.getCwd() );

    if ( !qualifier.empty() ) {
        _insert_job_stmt_ptr->parameters()[ DBTJob::QUALIFIER_COL ].set( qualifier );
    } else {
        _insert_job_stmt_ptr->parameters()[ DBTJob::QUALIFIER_COL ].setNull();
    }

    cxxdb::ResultSetPtr rs_ptr(_insert_job_stmt_ptr->execute());

    // get the job ID
    if ( ! rs_ptr->fetch() ) {
        BOOST_THROW_EXCEPTION( std::runtime_error( "didn't get the job ID" ) );
    }

    *id_out = rs_ptr->columns()[ DBTJob::ID_COL ].as<uint64_t>();

    if ( rs_ptr->fetch() ) {
        BOOST_THROW_EXCEPTION( std::runtime_error( "got multiple job IDs??" ) );
    }

}


void Operations::_insertJobMap(
        Id id,
        const InsertInfo& info
    )
{
    if ( info.getCorner().empty() ) {
        return;
    }

    _insert_job_map_stmt_ptr->parameters()[ DBTJobmap::ID_COL ].cast( id );
    _insert_job_map_stmt_ptr->parameters()[ DBTJobmap::MIDPLANE_COL ].set( info.getCorner().substr( 0, 6 ) );

    if ( info.getNodeBoardPositions().empty() ) {
        _insert_job_map_stmt_ptr->parameters()[ DBTJobmap::NODEBOARD_COL ].setNull();

        _insert_job_map_stmt_ptr->execute();

        return;
    }

    BOOST_FOREACH( const string &nb_position, info.getNodeBoardPositions() ) {
        _insert_job_map_stmt_ptr->parameters()[ DBTJobmap::NODEBOARD_COL ].set( nb_position );
        _insert_job_map_stmt_ptr->execute();
    }
}


} } // namespace BGQDB::job
