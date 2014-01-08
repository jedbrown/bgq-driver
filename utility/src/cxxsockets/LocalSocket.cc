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
#include "SocketTypes.h"

using namespace CxxSockets;

LOG_DECLARE_FILE( "utility.cxxsockets" );

LocalDatagramSocket::LocalDatagramSocket(SockAddr& addr, Policy p)
{
    FileLocker locker;
    LockFile(locker);
    _fileDescriptor = socket(addr.family(), SOCK_DGRAM, 0);
    CloseOnExec();
    if(_fileDescriptor < 1)
        throw CxxSockets::SockHardError(errno, strerror(errno));
    LOG_INFO_MSG("constructing local datagram socket with policy " << p);

    if(unlink(((sockaddr_un *)&addr)->sun_path) != 0) {
        if(errno != ENOENT) { // No worries if path does not exist
            std::ostringstream msg;
            msg << "unlink error: " << strerror(errno);
            LOG_ERROR_MSG(msg.str());
            throw CxxSockets::SockHardError(errno, msg.str());
        }
    }
    pBind(addr);
    LOG_DEBUG_MSG("bound local datagram socket to " << ((sockaddr_un *)&addr)->sun_path);
}

LocalDatagramSocket::~LocalDatagramSocket()
{
    Close();
    SockAddr addr;
    try {
        getSockName(addr);
        (void)unlink(((sockaddr_un *)&addr)->sun_path); // Ignore errors
    } catch (const SockHardError& e) {
        LOG_DEBUG_MSG("path not removed after getSockName error: " << e.what());
    }
}

void LocalDatagramSocket::Connect(SockAddr& dest)
{
    FileLocker locker;
    LockFile(locker);
    if(connect(_fileDescriptor, (sockaddr*)(&dest), SUN_LEN(((sockaddr_un *)&dest))) == -1) {
        std::ostringstream msg;
        msg << "connect error: " << strerror(errno);
        throw CxxSockets::SockHardError(errno, msg.str());
    }
    LOG_INFO_MSG("Connected local datagram socket");
    return;
}

int LocalDatagramSocket::Send(Message& msg, int flags)
{
    SockAddr dest;
    return Send(msg, dest, flags);
}

int LocalDatagramSocket::Send(Message& msg, SockAddr& dest, int flags)
{
    PthreadMutexHolder mutex;
   int lockrc = LockSend(mutex);
   if(lockrc != 0) {
       std::ostringstream msg;
       if(lockrc != -1)
           msg << "Send error.  Socket send side lock error: " << strerror(lockrc);
       else
           msg << "Send error.  Socket send side closed: " << 0;
       LOG_INFO_MSG(msg.str());
       throw CxxSockets::SockSoftError(lockrc, msg.str());
   }

   // Dead simple protocol:  Send a four byte size followed by data.
   // Perhaps we should break up large sends into smaller chunks for
   // memory efficiency.  Probably around 500kB chunks.
   uint32_t size = msg.str().length();
   const uint32_t maxsize = 10000000;
   if(size > maxsize) {
       std::ostringstream errmsg;
       errmsg << "size send error, message too big " 
              << "(MsgSize=" << msg.str().length() << ", maxsize=" << maxsize << ")" 
              << ": " << strerror(EFBIG);
       LOG_ERROR_MSG(errmsg.str());
       throw CxxSockets::SockSoftError(errno, errmsg.str());
   }

   // If a destination is not specified, do not pass the address arguments.
   sockaddr *addr = (sockaddr *)&dest;
   socklen_t addrlen = SUN_LEN(((sockaddr_un *)&dest));
   if((dest.fm() == 0) && (addrlen == sizeof(unsigned short))) {
       addr = NULL;
       addrlen = 0;
   }

   // Send the size.
   uint32_t sizeNbo = htonl(msg.str().length());  // size in network byte order format.
   if(sendto(_fileDescriptor, &sizeNbo, sizeof(uint32_t), flags, addr, addrlen) < 0) {
       std::ostringstream errmsg;
       msg << "size send error: " << strerror(errno);
       LOG_ERROR_MSG(msg.str());
       throw CxxSockets::SockHardError(errno, errmsg.str());
   }

   // Send the data.
   int bytes = 0;
   bytes = sendto(_fileDescriptor, msg.str().c_str(), size, flags, addr, addrlen);
   if(bytes < 0) {
       std::ostringstream errmsg;
       errmsg << "data send error: " << strerror(errno);
       throw CxxSockets::SockHardError(errno, errmsg.str());
   }
   return size;
}

int LocalDatagramSocket::Receive(Message& msg, int flags)
{
    SockAddr src;
    return Receive(msg, src, flags);
}

int LocalDatagramSocket::Receive(Message& msg, SockAddr& src, int flags)
{
    PthreadMutexHolder mutex;
   int lockrc = LockReceive(mutex);

   if(lockrc != 0) {
       std::ostringstream msg;
       if(lockrc != -1)
           msg << "Receive error.  Socket receive side lock error: " << strerror(lockrc);
       else
           msg << "Receive error.  Socket receive side closed: " << 0;
       LOG_INFO_MSG(msg.str());
       throw CxxSockets::SockSoftError(lockrc, msg.str());
   }

   uint32_t size = 0;
   int bytes = recv(_fileDescriptor, &size, sizeof(uint32_t), flags);

   LOG_DEBUG_MSG("Receiving message of size " << bytes << " bytes.");

   if(bytes < 0) {
       std::ostringstream msg;
       msg << "size Receive error: " << strerror(errno);
       LOG_ERROR_MSG(msg.str());
       throw CxxSockets::SockHardError(errno, msg.str());
   }
   else if(bytes == 0) {
       std::ostringstream msg;
       msg << "Receive error: Connection closed: " << strerror(errno);
       LOG_INFO_MSG(msg.str());
       throw CxxSockets::SockCloseUnexpected(errno, msg.str());
   }

   size = ntohl(size);

   const unsigned int BUFFSIZE = 65535;
   char databuff[BUFFSIZE];
   bytes = 0;

   while(msg.str().length() < size) {
       bzero(&databuff, BUFFSIZE);
       socklen_t addrsize = SUN_LEN(((sockaddr_un *)&src));
       bytes = recvfrom(_fileDescriptor, databuff, BUFFSIZE>size?size:BUFFSIZE - 1, flags, (sockaddr *)&src, &addrsize);

       if(bytes < 0) {
           std::ostringstream msg;
           msg << "data Receive error: " << strerror(errno);
           LOG_ERROR_MSG(msg.str());
           throw CxxSockets::SockHardError(errno, msg.str());
       }

       else if(bytes == 0) {
           std::ostringstream msg;
           msg << "Receive error: Connection closed: " << strerror(errno);
           LOG_INFO_MSG(msg.str());
           throw CxxSockets::SockCloseUnexpected(errno, msg.str());
       }
       //        std::cout << "size=" <<  size << " msize=" << msg.str().length() << " bytes=" << bytes << std::endl;
       ////msg << databuff;
       for(int u=0; u < bytes; ++u) 
       {
           msg << databuff[u];
       }
   }
   LOG_DEBUG_MSG("Received complete message of size " << size << " bytes.");
   return size;
}

