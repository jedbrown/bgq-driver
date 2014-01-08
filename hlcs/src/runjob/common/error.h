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
#ifndef RUNJOB_COMMON_ERROR_H
#define RUNJOB_COMMON_ERROR_H

namespace runjob {
namespace error_code {

/*!
 * \brief All possible error values.
 */
enum rc {
    success = 0,
    block_already_exists,
    block_busy,
    block_invalid,
    block_not_found,
    block_unavailable,
    compute_node_invalid,
    corner_invalid,
    database_error,
    handshake_failure,
    io_node_failure,
    job_failed_to_cleanup,
    job_failed_to_start,
    job_not_found,
    job_np_invalid,
    kill_timeout,
    mapping_file_invalid,
    permission_denied,
    runjob_server_restart,
    runjob_server_shutdown,
    runjob_server_unavailable,
    shape_invalid,
    tool_invalid,

    num_rc
};

/*!
 * \brief Convert an rc into a string.
 */
inline const char* toString(
        const rc error    //!< [in]
        )
{
    static const char* names[] = {
        "success",
        "block already exists",
        "block is busy",
        "block definition is invalid",
        "block could not be found",
        "block is unavailable due to a previous failure",
        "compute node is invalid",
        "invalid sub-block corner location",
        "database error",
        "handshake failure",
        "I/O node failure",
        "job failed to cleanup",
        "job failed to start",
        "job not found",
        "number of ranks exceeds block size",
        "killing the job timed out",
        "mapping file is invalid",
        "permission denied",
        "runjob_server has been restarted",
        "runjob_server has been stopped",
        "runjob_server is unavailable",
        "invalid sub-block shape",
        "tool did not start"
    };

    if (error >= success && error < num_rc) {
        return names[error];
    } else {
        return "invalid error code";
    }
}

} // error_code
} // runjob

#endif
