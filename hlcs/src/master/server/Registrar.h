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

#ifndef MASTER_REGISTRAR_H_
#define MASTER_REGISTRAR_H_


#include <utility/include/cxxsockets/types.h>

#include <utility/include/portConfiguration/PortConfiguration.h>

#include <boost/thread.hpp>

#include <pthread.h>


//! \brief Registrar joins new BGAgents and clients
//! Registrar process new clients, validates them and
//! starts new threads within the client manager
class Registrar
{
public:
    Registrar();
    ~Registrar();
    void run(bool agent);
    bool get_failed() { return _failed; }
    void cancel();
private:
    //! \brief Waits for new incoming connections.
    void listenForNew(const bgq::utility::PortConfiguration::Pairs& portpairs);

    //! \brief Validate a new connection and put it in the agent vector.
    void processNew(CxxSockets::TCPSocketPtr sock);

    //! \brief Listen for new agents in this thread.
    boost::thread _listenerThread;

    //! \brief listener for new connections
    CxxSockets::ListenerSetPtr _registrationListener;
    bool _end;
    bool _failed;
    pthread_t _my_tid;
};


#endif
