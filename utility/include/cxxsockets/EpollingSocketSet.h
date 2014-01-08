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
#ifndef _EPOLLINGSOCKETSET_H
#define _EPOLLINGSOCKETSET_H

#include <sys/epoll.h>

namespace CxxSockets {

    class EpollingSocketSet : public EpollingFileSet {
        friend class EpollingTCPSocketSet;
    protected:
        virtual void pAddSock(SocketPtr sock, PollType p = RECV);
    private:
    public:
        EpollingSocketSet(int count = 25) {  _epfd = epoll_create(count); }
        EpollingSocketSet(std::vector<SocketPtr>& socks) {  
            _epfd = epoll_create(socks.size()); 
            AddSocks(socks);
        }

        ~EpollingSocketSet() { ::close(_epfd); }

        //! \brief Add a socket to the set
        //! 
        //! \param sock Socket to add
        //! \param p
        virtual void AddSock(SocketPtr sock, PollType p = RECV);

        //! \brief Add several sockets to the set        
        //! 
        //! \param socks vector of sockets to add
        //! \param p
        void AddSocks(std::vector<SocketPtr>& socks, PollType p = RECV);

        //! \brief Remove a socket from the set
        //! 
        //! \param sock Socket to remove from the set
        virtual void RemoveSock(SocketPtr sock);
    };

    class EpollingTCPSocketSet : public EpollingSocketSet {
    public:
        EpollingTCPSocketSet(int timeout = 0) {
            _timeout = timeout;
        }

        void PolledSend(Message& msg);

        //! \brief Poll all sockets in the set, get messages for all of the ones
        //! that have data, then return those messages.  
        //!
        //! This assumes
        //! CxxSockets on the other end.  This is important because this
        //! will NOT return until a complete message is received from EVERY
        //! socket with ANY data.
        //!
        //! Note: This is useful for small numbers of managed connections 
        //! but will =not= scale efficiently.  This is because it blocks
        //! serially receiving on each polled socket and you'll then need
        //! to loop through the MsgMap to consume all that data when done.
        //!
        //! \param msgmap Map of messages to sockets to return.
        //! \param flags
        void PolledReceive(CxxSockets::MsgMap& msgmap, int flags = 0);
    };
}
#endif
