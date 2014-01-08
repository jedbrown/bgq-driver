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

#ifndef BGQDB_JOB_TYPES_H_
#define BGQDB_JOB_TYPES_H_

#include <string>

#include <stdint.h>

/*! \namespace BGQDB::job
 *
 * \brief Job stuff.
 *
 * To use these functions, you first need to call BGQDB::job::init().
 *
 */

namespace BGQDB {
namespace job {

typedef uint64_t Id; //!< Type for job IDs.

/*! \brief Job status values. */
namespace status {

/*! \brief Job status values. */
enum Value {
    Cleanup,
    Debug,
    Error,
    Loading,
    Running,
    Setup,
    Starting,
    Terminated
};

} // namespace BGQDB::job::status


/*! \brief Job status codes (the value in the DB). */
namespace status_code {

extern const std::string Cleanup;
extern const std::string Debug;
extern const std::string Error;
extern const std::string Loading;
extern const std::string Running;
extern const std::string Setup;
extern const std::string Starting;
extern const std::string Terminated;

} // namespace BGQDB::job::status_code


/*! \brief Convert the status value to the status code. */
const std::string& valueToCode( status::Value value );

/*! \brief Convert the status code to the status value. */
status::Value codeToValue( const std::string& code );

/*! \brief Return true iff code is a valid code. */
bool isCode( const std::string& code );


} } // namespace BGQDB::job


#endif
