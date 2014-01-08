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
 * \file ListenerSets.h
 */
#ifndef LISTENERSETS_H
#define LISTENERSETS_H

namespace CxxSockets {

//! \brief Set of Listeners.
//!
//! Treat multiple listeners as one object for performing accepts.
//! More transparent v4/v6 support. 
class ListenerSet : public FileSet {
    ListenerSet() {}
public:

    //! \brief Constructor.  You need a list of sockaddrs even if you
    //! just want a single element.
    ListenerSet(SockAddrList& sal, int backlog = SOMAXCONN);

    //! \brief This will perform a blocking accept on the first socket
    //! in the list.  
    //!
    //! You want a polling listener set if you might
    //! have more than one listener.
    //!
    //! \param sock TCP socket to accept
    bool AcceptNew(TCPSocketPtr& sock);
    bool AcceptNew(SecureTCPSocketPtr& sock, const bgq::utility::ServerPortConfiguration& port_config);
};

//! \brief Polling set of listeners
class PollingListenerSet : public PollingSocketSet {
public:
    PollingListenerSet() {}

    //! \brief Build the set from a list of socket addresses
    //! 
    //! \param sal socket address list
    //! \param backlog The maximum number of pending connections allowed
    //! \param p
    PollingListenerSet(SockAddrList& sal, int backlog = SOMAXCONN);
    ~PollingListenerSet() { }

    //! \brief Polling AcceptNew()
    //!
    //! Each time a poll() is done, the set of listeners with 
    //! available connections is archived.  If there are any
    //! archived, the accept() is performed on one of them
    //! and a poll() is not performed.  IOW, we only poll when
    //! we need to and we only return one new connection per
    //! operation.
    bool AcceptNew(TCPSocketPtr& sock);
    bool AcceptNew(SecureTCPSocketPtr& sock, const bgq::utility::ServerPortConfiguration& port_config);
private:
    bool AcceptNewCommon(SecureTCPSocketPtr& secure_sock, TCPSocketPtr& sock, const bgq::utility::ServerPortConfiguration& port_config, bool secure);
};

}
#endif
