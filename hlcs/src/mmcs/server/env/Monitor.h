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

#ifndef MMCSEnvMonitor_H
#define MMCSEnvMonitor_H

#include "common/Thread.h"

#include <boost/asio/io_service.hpp>

namespace mmcs {
namespace server {
namespace env {

class Monitor : public common::Thread
{
public:
    Monitor();
    void* threadStart();

private:
    unsigned calculateThreadPool();

private:
    boost::asio::io_service _io_service;
};

} } } // namespace mmcs::server::env

#endif
