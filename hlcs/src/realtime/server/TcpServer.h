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


#ifndef TCPSERVER_H_
#define TCPSERVER_H_


#include "Client.h"

#include <utility/include/portConfiguration/Acceptor.h>
#include <utility/include/portConfiguration/ServerPortConfiguration.h>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/asio/ssl.hpp>

#include <vector>


namespace realtime {
namespace server {


class configuration_t;
class DbChangesMonitor;
class Status;


/*! \brief Accepts new connections.
 *
 */
class TcpServer
{
public:

    TcpServer(
            boost::asio::io_service& io_service,
            DbChangesMonitor& db_changes_monitor,
            const bgq::utility::ServerPortConfiguration& port_config,
            Status& status
        );


private:

    bgq::utility::Acceptor _acceptor;

    DbChangesMonitor &_db_changes_monitor;
    Status &_status;


    void _handleAccept(
            const bgq::utility::Acceptor::AcceptArguments& args
        );
};


} } // namespace realtime::server


#endif
