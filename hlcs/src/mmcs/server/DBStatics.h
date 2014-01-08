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
/* (C) Copyright IBM Corp.  2011, 2011                              */
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

#ifndef MMCS_SERVER_DB_STATICS_H_
#define MMCS_SERVER_DB_STATICS_H_

#include <bgq_util/include/Location.h>

#include <string>
#include <vector>

namespace mmcs {
namespace server {

namespace DBStatics {
    enum Status { AVAILABLE, ERROR, MISSING, SOFTWARE_FAILURE };

    bool setLocationStatus(
            const std::vector<std::string>& locations, 
            const Status status, 
            const bgq::util::Location::Type type
            );

    /*! \brief Given a hardware state return as a string. */
    std::string hardwareStatusToString(const Status status);
}

} } // namespace mmcs::server

#endif
