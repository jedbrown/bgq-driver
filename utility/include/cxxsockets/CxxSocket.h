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
 * \file CxxSocket.h
 */

#ifndef CXXSOCKET_H
#define CXXSOCKET_H

#include <utility/include/portConfiguration/ClientPortConfiguration.h>
#include <utility/include/portConfiguration/ServerPortConfiguration.h>

// Provides thread-safe sockets abstractions
// Throws exceptions for errors containing errnos
// Supports:
//    blocking and non-blocking sockets (not yet)
//    TCP and UDP and can be extended to other protocols
//    provides data buffer management

namespace CxxSockets {
    
struct FileLocker {
    PthreadMutexHolder _all;
    PthreadMutexHolder _send;
    PthreadMutexHolder _receive;
};

class File {
    friend class Authorization;
    friend class FileSet;
    friend class PollingSocketSet;
    friend class PollingFileSet;
    friend class PollingTCPSocketSet;
    friend class PollingSecureTCPSocketSet;
    friend class PollingListenerSet;
protected:
    PthreadMutex _fileLock;
    int _fileDescriptor;
    SocketReceiveSidePtr _receiver;
    SocketSendSidePtr _sender;
    int _fcntlFlags;

    //! \brief Mutual exclusion for all operations on this file.
    int LockFile(FileLocker& locker);
    //! \brief Lock send side only
    int LockSend(PthreadMutexHolder& mutex); 
    //! \brief Lock receive side only
    int LockReceive(PthreadMutexHolder& mutex);
    void Initialize();

public:
    File();

    //! construct object with pre-existing descriptor
    File(int descriptor);

    //! \brief Close the file
    int Close() { return(::close(_fileDescriptor)); }

    //! \brief Close on exec
    void CloseOnExec() { 
        fcntl(_fileDescriptor, F_SETFD, FD_CLOEXEC);
        _fcntlFlags = fcntl(_fileDescriptor, F_GETFL);
        if(_fcntlFlags < 0) {
            throw SockHardError(errno, "Cannot perform fcntl.");
        }
        return;
    }
};

//! \brief Base class for all types of sockets.  
//!
//! Locking strategy.
//! 1)  There are three classes of socket lock operations:
//!     a) sending
//!     b) receiving
//!     c) global socket ops
//! 2)  Sending locks provide mutual exclusion on sending and 
//!     polling for sending operations
//! 3)  Receiving locks provide mutual exclusion on receiving
//!     and polling for receiving operations
//! 4)  Acquisition of BOTH locks is required for operations
//!     like socket(), bind(), connect() and close().
class Socket : public File {
    friend class SocketReceiveSide;
    friend class SocketSendSide;
protected:
    Socket() : File() {}
    bool _nonagle;

    //! \brief Protected, non-locking bind.  Called from locking ops.
    void pBind(SockAddr& addr);
    bool internal_getSockName(SockAddr& sa);
    bool internal_getPeerName(SockAddr& sa);

public:
    //! \brief copy constructor
    Socket(Socket& sock);

    //! \brief Which side of the connection to shut down
    enum ShutDownSide { RECEIVE, SEND };

    //! \brief Make sure we close on destruction
    virtual ~Socket()  = 0;

    //! \brief bind() the socket
    void Bind(SockAddr& addr);

    //! \brief Wraps get getsockname
    bool getSockName(SockAddr& sa);

    //! \brief Wraps get getpeername
    bool getPeerName(SockAddr& sa);

    //! \brief Shutdown one side of the connection.
    bool Shutdown(ShutDownSide rw);
};

//! \brief Receive side of the socket.  Just need it for shutdown locks.
class SocketReceiveSide {
    friend class File;
    friend class Socket;
    friend class TCPSocket;
    PthreadMutex _recvLock;   // Scope lock for receiving
    int Unlock() { return(_recvLock.Unlock()); }
    PthreadMutex& getLock() { return _recvLock; }

public:
    SocketReceiveSide() {}
};

//! \brief Send side of the socket.  Just need it for shutdown locks.
class SocketSendSide {
    friend class File;
    friend class Socket;
    friend class TCPSocket;
    PthreadMutex _sendLock;   // Scope lock for sending
    int Unlock() { return(_sendLock.Unlock()); }
    PthreadMutex& getLock() { return _sendLock; }

public:
    SocketSendSide() {}
};

class TCPSendFunctor {
    int error;
public:
    int operator() (int _fileDescriptor, const void* msg, int length, int flags);
};

class SecureTCPSendFunctor {
    int error;
    SSL* _ssl;
public:
    SecureTCPSendFunctor(SSL* ssl) : _ssl(ssl) {}
    int operator() (int _fileDescriptor, const void* msg, int length, int flags);
};

class TCPReceiveFunctor {
    int error;
public:
    int operator() (int _fileDescriptor, const void* msg, int length, int flags);
};

class SecureTCPReceiveFunctor {
    int error;
    SSL* _ssl;
public:
    SecureTCPReceiveFunctor(SSL* ssl) : _ssl(ssl) {}
    int operator() (int _fileDescriptor, const void* msg, int length, int flags);
};

//! \brief Socket on top of TCP.  
//!
//! Address family is determined by sockaddr or param
//! depending upon ctor used.
class TCPSocket : public Socket {
    friend class ListeningSocket;
protected:
    std::string _connection_data;

    //! \brief Actually perform the tcp connect
    void performConnect(SockAddr& local, SockAddr& remote);

    //! \brief Internal non-locked, insecure method
    void mConnect(SockAddr& remote_sa);

    void replaceFd(int fd);

    template<class sendfunctor>
    inline int InternalSend(Message& msg, int flags, sendfunctor& f);

    template<class sendfunctor>
    inline int InternalSendUnmanaged(Message& msg, int flags, sendfunctor& f);

    template<class receivefunctor>
    inline int InternalReceive(Message& msg, int flags, receivefunctor& f);

    template<class receivefunctor>
    inline int InternalReceiveUnManaged(Message& msg, unsigned int bytes, int flags, receivefunctor& f);
public:

    //! \brief Default ctor.  This one does NOT acquire an FD.  It gets a v4 socket by default
    TCPSocket() { 
        _fileDescriptor = socket(AF_INET, SOCK_STREAM, 0); 
        CloseOnExec();
        _nonagle = true;
        _connection_data = "";
    }

    //! \brief Build a socket based on either a family or pre-acquired file
    //! descriptor.  Socket sets will generally use this.
    TCPSocket(int family, int fd) {
        if(fd == 0 && family == 0) {
            throw CxxSocketUserError(0, "Cannot create socket. Family and fd are null.");
        }
        if(fd) // We already have a file descriptor
            _fileDescriptor = fd;
        else // We need a new fd.
            _fileDescriptor = socket(family, SOCK_STREAM, 0);
        CloseOnExec();
        _nonagle = true;
        _connection_data = "";
    }

    //! \brief Build a socket based on a sockaddr structure.
    //!
    //! Use this for initiating a client connection
    TCPSocket(SockAddr& addr) {
        if(addr.family() == 0) {
            std::ostringstream msg;
            msg << "Cannot create socket.  Client socket null family is invalid";
            throw CxxSocketUserError(0, msg.str());
        }
        _fileDescriptor = socket(addr.family(), SOCK_STREAM, 0);
        CloseOnExec();
        _nonagle = true;
        _connection_data = "";
    }

    //! \brief Connect and just specify a remote address
    //!
    //! \param remote_sa SockAddr object specifying the remote address to connect
    void Connect(SockAddr& remote_sa);

    //! \brief set up "keepalive" to probe for dead peers
    //! \param onoff Turn it on or off.
    //! \param firstprobe How long to wait to send the first probe after last data packet.
    //! \param probeint How often to send a probe.
    //! \param probecount Number of unanswered probes to consider the connection dead.
    void setProbe(bool onoff, int firstprobe = 0, int probeint = 0, int probecount = 0);

    //! \brief switch the nagle algorithm on or off.  It's off by default.
    //! \returns whether it is on or off.
    bool toggleNoDelay(); 

    //! \brief Managed send.  Takes care of byte counting for you.
    //!
    //! \param msg  Message object to send.
    //! \param flags
    //!
    //! This method will send the size of the data before sending the
    //! data.  It's meant to be used with a CxxSocket on the other
    //! side.  This allows the user to send a Message object
    //! atomically.
    int Send(Message& msg, int flags = 0);

    //! \brief Unmanaged send.  User must manage data.
    //!
    //! \param msg Message object to send.
    //! \param flags
    //!
    //! Just send the message.  Use when the other side may not
    //! be using CxxSockets.  Can be blocking or not.
    int SendUnManaged(Message& msg, int flags = 0);

    //! \brief Managed receive.
    //! \param msg Message object to receive.
    //! \param flags
    //!
    //! This method recvs the size of the message first and then
    //! continues to receive data until that amount has arrived.
    //! The assumption is that it is communicating with a CxxSocket.
    int Receive(Message& msg, int flags = 0);

    //! \brief Unmanaged receive
    //! \param msg Message object to receive.
    //! \param bytes
    //! \param flags
    //!
    //! This method just receives whatever is available and returns.
    int ReceiveUnManaged(Message& msg, unsigned int bytes, int flags = 0);
};

class SecureTCPSocket : public TCPSocket {
    friend class ListeningSocket;
    friend class SecureTCPSendFunctor;
    friend class SecureTCPReceiveFunctor;
public:
    SecureTCPSocket() ;

    SecureTCPSocket(int family, int fd);

    ~SecureTCPSocket();

    //! \brief Connect and just specify a remote address
    //!
    //! \param remote_sa SockAddr object specifying the remote address to connect
    //! \param port_config
    void Connect(SockAddr& remote_sa, const bgq::utility::ClientPortConfiguration& port_config);

    //! \brief Make this connection a secure one!  You'll only want to call this
    // if you passed a previously connected file descriptor to the constructor.
    // Otherwise, you get this for free.
    void MakeSecure(const bgq::utility::ClientPortConfiguration& port_config);
    void MakeSecure(const bgq::utility::ServerPortConfiguration& port_config);

    //! \brief Managed send.  Takes care of byte counting for you.
    //!
    //! \param msg  Message object to send.
    //! \param flags
    //!
    //! This method will send the size of the data before sending the
    //! data.  It's meant to be used with a CxxSocket on the other
    //! side.  This allows the user to send a Message object
    //! atomically.
    int Send(Message& msg, int flags = 0);

    //! \brief Unmanaged send.  User must manage data.
    //!
    //! \param msg Message object to send.
    //! \param flags
    //!
    //! Just send the message.  Use when the other side may not
    //! be using CxxSockets.  Can be blocking or not.
    int SendUnManaged(Message& msg, int flags = 0);

    //! \brief Managed receive.
    //! \param msg Message object to receive.
    //! \param flags
    //!
    //! This method recvs the size of the message first and then
    //! continues to receive data until that amount has arrived.
    //! The assumption is that it is communicating with a CxxSocket.
    int Receive(Message& msg, int flags = 0);

    //! \brief Unmanaged receive
    //! \param msg Message object to receive.
    //! \param bytes
    //! \param flags
    //!
    //! This method just receives whatever is available and returns.
    int ReceiveUnManaged(Message& msg, unsigned int bytes, int flags = 0);

    const bgq::utility::UserId& getUserId() const { return *_uid_ptr; }
    UserType getUserType() const { return _utype; }
    static std::string printSSLError(SSL* ssl, int rc);
private:
    int ClientHandshake(const bgq::utility::ClientPortConfiguration& port_config);
    int ServerHandshake(const bgq::utility::ServerPortConfiguration& port_config);
    void SetupCredentials(const bgq::utility::SslConfiguration& sslconfig);
    void SetupContext(const bgq::utility::SslConfiguration& sslconfig);
    bgq::utility::UserId::ConstPtr _uid_ptr;
    BIO* _cnnbio;
    SSL* _ssl;
    SSL_CTX* _ctx;
    UserType _utype;
};

//! \brief Generic v4/v6 listening socket.  
//!
//! This is version specified
//! by the sockaddr structure used to initialize it.  ListenerSets
//! allow initilializing multiple listeners.
class ListeningSocket : public Socket {
    int Accept();
public:
    //! \brief Constructor. 
    ListeningSocket() { _nonagle = true; }

    //! \brief Performs a bind() and listen() for a single sockaddr
    //!
    //! \param addr SockAddr object representing address to listen on
    //! \param backlog Maximum number of pending connections to allow
    //! \param p
    ListeningSocket(SockAddr& addr, int backlog = SOMAXCONN);

    //! \brief This gets a new socket
    //! 
    //! \param sock Pass in a new socket object to fill
    bool AcceptNew(TCPSocketPtr& sock);
    bool AcceptNew(SecureTCPSocketPtr& sock, const bgq::utility::ServerPortConfiguration& port_config);
};

}

#endif
