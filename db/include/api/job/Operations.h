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

#ifndef BGQDB_JOB_OPERATIONS_H_
#define BGQDB_JOB_OPERATIONS_H_


#include "types.h"
#include "InsertInfo.h"
#include "Shape.h"
#include "RemoveInfo.h"

#include "../BGQDBlib.h"

#include "../cxxdb/fwd.h"

#include <boost/thread.hpp>

namespace BGQDB {
namespace job {

/*! \brief %Operations on the job tables. */
class Operations
{
public:

    /*! \brief Constructor.
     *
     *  \pre BGQDB::init was successful.
     *
     *  \note Holds three database connections and prepares a few statements.
     */
    Operations();


    /*! \brief Insert a new job.
     *
     *  \throw BGQDB::job::exception::BlockNotFound if the block doesn't exist.
     *  \throw BGQDB::job::exception::BlockNotInitialized if the block is not in Initialized status.
     */
    void insert(
            const InsertInfo& info,
            Id *id_out
        );


    /*! \brief Update the status of the job.
     *
     *  \throw BGQDB::job::exception::JobNotFound job with the id doesn't exist.
     */
    void update(
            Id id,
            status::Value new_status
        );


    /*! \brief Remove a job.
     */
    void remove(
            Id job_id,
            const RemoveInfo& info
        );


private:

    boost::mutex _insert_mtx;
    cxxdb::ConnectionPtr _insert_conn_ptr;
    cxxdb::QueryStatementPtr _insert_query_block_status_stmt_ptr;
    cxxdb::QueryStatementPtr _insert_query_sub_node_user_stmt_ptr;
    cxxdb::QueryStatementPtr _insert_job_stmt_ptr;
    cxxdb::UpdateStatementPtr _insert_job_map_stmt_ptr;

    boost::mutex _update_mtx;
    cxxdb::ConnectionPtr _update_conn_ptr;
    cxxdb::UpdateStatementPtr _update_stmt_ptr;

    boost::mutex _remove_mtx;
    cxxdb::ConnectionPtr _remove_conn_ptr;
    cxxdb::UpdateStatementPtr _remove_insert_job_history_stmt_ptr;
    cxxdb::UpdateStatementPtr _remove_delete_job_stmt_ptr;


    void _checkBlockInitialized(
            const std::string& block_id,
            Shape& block_shape_out,
            std::string& qualifier_out
        );

    void _checkSubNodeUser(
            const std::string& user,
            const std::string& corner
            );

    void _insertJob(
            const InsertInfo& info,
            const Shape& block_shape,
            const std::string& qualifier,
            Id *id_out,
            bool *args_truncated_out,
            bool *envs_truncated_out
        );

    void _insertJobMap(
            Id id,
            const InsertInfo& info
        );

};


} } // namespace BGQDB::job


#endif
