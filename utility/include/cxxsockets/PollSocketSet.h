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
 * \file PollSocketSet.h
 */
#ifndef POLLSOCKETSET_H
#define POLLSOCKETSET_H

namespace CxxSockets {

//! \brief Set of sockets to poll
class PollingSocketSet : public PollingFileSet {
protected:
    //! \brief vector of polling objects.  Vectors are
    //! guaranteed to be compatable with C-style arrays.
    void pAddSock(SocketPtr sock, PollType p = RECV); 
public:
    PollingSocketSet(int timeout = 0) { 
        _timeout = timeout; 
        _pollinfo.clear(); 
    }

    //!  \brief Add a socket to the set.  
    void AddSock(SocketPtr sock, PollType p = RECV); 

    //!  \brief Add several sockets to the set
    void AddSocks(std::vector<SocketPtr>& socks);

    //!  \brief Remove a socket
    void RemoveSock(SocketPtr sock);
};

//! Polling TCP Sockets for TCP-specific behaviors
class PollingTCPSocketSet : public PollingSocketSet {
public:
    PollingTCPSocketSet(int timeout = 0) { _timeout = timeout; }

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
    int PolledReceive(MsgMap& msgmap);
};

//! Polling TCP Sockets for TCP-specific behaviors
class PollingSecureTCPSocketSet : public PollingSocketSet {
public:
    PollingSecureTCPSocketSet(int timeout = 0) { _timeout = timeout; }

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
    int PolledReceive(MsgMap& msgmap);
};

}

#endif
