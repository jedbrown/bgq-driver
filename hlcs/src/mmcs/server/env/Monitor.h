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

#include <boost/asio/io_service.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>

namespace mmcs {
namespace server {
namespace env {

class Monitor : public boost::enable_shared_from_this<Monitor>
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<Monitor> Ptr;

public:
    static Ptr create();
    void stop();
    ~Monitor();

private:
    Monitor();
    void start();
    unsigned calculateThreadPool();
    void run();

private:
    boost::asio::io_service _io_service;
    boost::thread_group _threads;
};

} } } // namespace mmcs::server::env

#endif
