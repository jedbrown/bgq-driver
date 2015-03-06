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

#ifndef MMCS_ENV_TYPES_H
#define MMCS_ENV_TYPES_H

#include <string>
#include <vector>

namespace mmcs {
namespace server {
namespace env {

typedef std::vector<std::string> Racks;

typedef std::vector<std::string> IoDrawers;

const unsigned NORMAL_POLLING_PERIOD = 10;  // 10 seconds to check for threads being killed
const unsigned ENVS_POLLING_PERIOD   = 300; // 5 minutes to check cards for new environmental data

// these come from mc.h
const int CARD_NOT_PRESENT           = 52;  // special value returned in the _error field
const int CARD_NOT_UP                = 11;  // special value returned in the _error field
const int UNEXPECTED_DEVICE          = 19;  // special value returned in the _error field

} } } // namespace mmcs::server::env

#endif
