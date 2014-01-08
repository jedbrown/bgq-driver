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
#ifndef RUNJOB_COMMANDS_ERROR_H
#define RUNJOB_COMMANDS_ERROR_H
/*!
 * \file runjob/commands/error.h
 * \brief error codes and free functions to convert them.
 * \ingroup command_protocol
 */

namespace runjob {
namespace commands {
namespace error {

/*!
 * \brief all possible return codes from commands.
 * \ingroup command_protocol
 */
enum rc {
    success             = 0,
    block_invalid,
    block_not_found,
    config_file_invalid,
    database_error,
    logging_level_invalid,
    job_not_found,
    job_status_invalid,
    job_already_dying,
    maximum_tool_count,
    permission_denied,
    rank_invalid,
    tool_not_found,
    tool_path_invalid,
    tool_subset_invalid,

    unknown_failure     = 255
};

/*!
 * \brief Convert an error code into a string.
 * \ingroup command_protocol
 */
inline const char* toString(
        rc e    //!< [in]
        )
{
    switch (e) {
        case success:               return "Success";
        case block_invalid:         return "Block invalid";
        case block_not_found:       return "Block not found";
        case config_file_invalid:   return "Config file invalid";
        case database_error:        return "Database error";
        case logging_level_invalid: return "Logging level invalid";
        case job_not_found:         return "Job not found";
        case job_status_invalid:    return "Job status invalid";
        case job_already_dying:     return "Job already dying";
        case maximum_tool_count:    return "Maximum tool count";
        case permission_denied:     return "Permission denied";
        case rank_invalid:          return "Invalid rank";
        case tool_not_found:        return "Tool not found";
        case tool_path_invalid:     return "Tool path invalid";
        case tool_subset_invalid:   return "invalid tool subset";
        case unknown_failure:       // fall through 
        default:                    return "Unknown failure";
    }
}

} // error
} // commands
} // runjob

#endif
