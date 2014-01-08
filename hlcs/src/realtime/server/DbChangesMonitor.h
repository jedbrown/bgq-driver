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

#ifndef DBCHANGESMONITOR_H_
#define DBCHANGESMONITOR_H_

#include "Client.h"

namespace realtime {
namespace server {

class Status;

/*! \brief Checks for changes to the database and passes them on to clients.
 *
 */
class DbChangesMonitor
{
public:

    struct State {
        enum Value {
            Idle, //!< No clients have connected.
            Monitoring, //!< Monitoring DB for changes.
            MaxXact //!< Max xact condition.
        };
    };

    virtual void addClient(
            Client::Ptr client_ptr
        ) =0;

    virtual void removeClient( Client::Ptr client_ptr )  =0;

    virtual ~DbChangesMonitor()  { /* Nothing to do */ }

    void setServerStatus( Status *server_status_p )  { _server_status_p = server_status_p; }

protected:

    void _setState( State::Value state );

private:
    Status *_server_status_p;
};

} // namespace realtime::server
} // namespace realtime

#endif
