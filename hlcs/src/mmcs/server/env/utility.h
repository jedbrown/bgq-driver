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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#ifndef MMCS_ENV_UTILITY_H
#define MMCS_ENV_UTILITY_H

#include "types.h"

#include <db/include/api/cxxdb/fwd.h>

class MCServerRef;

namespace mmcs {
namespace server {
namespace env {

/*!
 * \brief Get a list of racks in the system
 */
Racks
getRacks(
        const cxxdb::ConnectionPtr& connection  //!< [in]
        );

/*!
 * \brief Get a list of I/O drawers in the system
 */
IoDrawers
getIoDrawers(
        const cxxdb::ConnectionPtr& connection  //!< [in]
        );

/*!
 * \brief Get a list of I/O racks in the system
 */
IoDrawers
getIoRacks(
        const cxxdb::ConnectionPtr& connection  //!< [in]
        );

/*!
 * \brief Calculate the number of connections to mc_server to use
 */
unsigned
calculateConnectionSize(
        const Racks& racks  //!< [in]
        );

} } } // namespace mmcs::server::env

#endif
