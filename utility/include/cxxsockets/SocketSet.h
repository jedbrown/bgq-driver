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
#ifndef _SOCKETSET_H
#define _SOCKETSET_H

#include <vector>
#include <list>
#include <stdexcept>
#include <bgq_util/include/pthreadmutex.h>
#include <poll.h>

#ifndef _SOCKETTYPES_H
#include "SocketTypes.h"
#endif
#ifndef _SOCKETADDRS_H
#include "SocketAddrs.h"
#endif

class Socket;

namespace CxxSockets {

    //! \brief Class for aggregating sockets for the purpose of polling or 
    //! abstracting IP versions.
    class SocketSet : public FileSet {
    protected:
    public:
        //! \brief ctor
        SocketSet(bool blocking = false, Policy p = INSECURE) { _blocking = blocking; }
        virtual ~SocketSet() {}

        //! \brief Add a socket to the set
        //!
        //! \param sock Socket to add
        virtual void AddSock(SocketPtr sock);
        virtual void AddSocks(std::vector<SocketPtr> socks);
        //! \brief Remove a socket from the set. 
        //! 
        //! \param sock Socket to remove from the set
        virtual void RemoveSock(SocketPtr& sock);
    };

    //! \brief This allows us to create a set from a sockaddr list.  Then, we can retrieve
    //! just one and never have to deal with IPvX.    
    class TCPSocketSet : public FileSet {
    public:
        TCPSocketSet(Policy p = INSECURE) { }
        TCPSocketSet(SockAddrList& sal, Policy p = INSECURE);
        const TCPSocketPtr Retrieve() { 
            PthreadMutexHolder mutex;
            LockSet(mutex);
            return std::tr1::static_pointer_cast<TCPSocket>(front()); 
        }
    };
}

#endif
