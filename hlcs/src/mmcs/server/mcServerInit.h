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

/*!
 * \file mcServerInit.h
 * \brief Initialize MMCS' interface with MCServer
 *
 * Establish a generic listener for RAS events that are not handled by any other RAS listener.
 *
 * Send a Bringup command to MCServer to initialize the hardware.
 */

#ifndef MC_SERVER_INIT_H
#define MC_SERVER_INIT_H

#include <vector>
#include <string>

#include "libmmcs_client/CommandReply.h"

namespace mmcs {
namespace server {

void mcServerInit(
        const std::vector<std::string>& bringup_options,
        mmcs_client::CommandReply& reply,
        const bool blocks_are_booted
        );

} } // namespace mmcs::server

#endif
