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
#ifndef RUNJOB_SERVER_JOB_TOOL_STATUS_H
#define RUNJOB_SERVER_JOB_TOOL_STATUS_H

#include <iostream>

namespace runjob {
namespace server {
namespace job {
namespace tool {

/*!
 * \brief
 */
enum class Status {
    Invalid,
    Ending,
    Error,
    Running,
    Starting,
    Terminated
};

/*!
 * \brief
 */
std::ostream&
operator<<(
        std::ostream& os,
        const Status& status
        );

/*!
 * \brief
 */
std::istream&
operator>>(
        std::istream& is,
        Status& status
        );

/*!
 * \brief
 */
const char*
getDatabaseValue(
        Status s    //!< [in]
        );

} // tool
} // job
} // server
} // runjob

#endif
