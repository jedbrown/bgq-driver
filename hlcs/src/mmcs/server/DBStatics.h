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


#include "libmmcs_client/CommandReply.h"

#include <string>
#include <vector>


namespace mmcs {
namespace server {


namespace DBStatics {
    enum Status { AVAILABLE, ERROR, MISSING };
    enum Type { ION, CN, IOCARD, COMPUTECARD, SERVICECARD };
    void setLocationStatus(const std::vector<std::string>& locations, mmcs_client::CommandReply& reply, Status status, Type type);
}

} } // namespace mmcs::server

#endif
