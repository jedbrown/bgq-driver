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

#ifndef MMCS_ENV_HEALTH_CHECK_H
#define MMCS_ENV_HEALTH_CHECK_H

#include "types.h"

#include "common/Thread.h"

#include <set>
#include <string>

namespace mmcs {
namespace server {
namespace env {

class HealthCheck: public common::Thread
{
public:
    typedef std::set<std::string> OpticalContainer;

public:
    HealthCheck();
    void* threadStart();
    unsigned seconds;

private:
    void getBadWireMasks();
    OpticalContainer _skipModules;
};

} } } // namespace mmcs::server::env

#endif
