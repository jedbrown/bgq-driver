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


#ifndef TCPCONNECT_H
#define TCPCONNECT_H

// Simple TCP communications - connect() side.
// 09 Feb 2001 Derek Lieber.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

// Connect to a remote process.
// Taken:    host name and port number to connect to (something like "bg02:4000")
//           how long to wait for connection to be established (0=wait forever)
//           display progress and error messages?
// Returned: connection file descriptor (-1=error)
// See also: TCPlisten()
//
static inline int
TCPconnect(const char *connectionName, time_t timeout = 0, bool verbose = false)
   {
   char           remoteHostName[strlen(connectionName) + 1];
   unsigned short remotePort;

   // parse connection name
   //
   for (const char *cp = connectionName; ; ++cp)
      {
      if (*cp == 0)
         {
         if (verbose)
            printf("tcp connect error: please specify a <host:port> instead of `%s'\n", connectionName);
         errno = EINVAL;
         return -1;
         }
      if (*cp == ':')
         {
         unsigned len = cp - connectionName;
         strncpy(remoteHostName, connectionName, len)[len] = 0;
         remotePort = atol(cp + 1);
         break;
         }
      }
    uint32_t ipaddr;
    int rc;
    #if _AIX
         struct hostent htent;
         struct hostent_data ht_data;
         memset(&htent, 0, sizeof(htent));
         memset(&ht_data, 0, sizeof(ht_data));

        rc = gethostbyname_r(remoteHostName,     // *name, 
                               &htent,         //  *htent,
                               &ht_data);      // *ht_data      
        ipaddr = ht_data.host_addr.s_addr; 
        if (rc != 0)
        {
          errno = ENXIO;
          return -1;
        }

    #else
        enum {BUFFSIZE = 1024};
        char szBuff[BUFFSIZE];                  // buffer to use for strings..
        struct hostent *pHostResult;    // pointer to result
        struct hostent bufHost;         // buffer for host struct...
        in_addr *pHostAddr;
        int h_errnop;
        

        //
        // use the thread safe version...
        //
        rc = gethostbyname_r(remoteHostName,           // *name
                               &bufHost,            // *ret
                               szBuff,              // *buf
                               BUFFSIZE,            // buflen
                               &pHostResult,        // **result
                               &h_errnop);          // *h_errnop
        if ((rc != 0) || (pHostResult == NULL))
        {
          #ifndef _AIX
          if (verbose) printf("tcp connect error: can't resolve \"%s\": %s\n", remoteHostName, hstrerror(h_errno));
          #endif
          errno = ENXIO;
          return -1;
        }
        pHostAddr = (in_addr *)pHostResult->h_addr_list[0];
        ipaddr = pHostAddr->s_addr;
       assert(pHostResult->h_addrtype == AF_INET);
       assert(pHostResult->h_length == sizeof(unsigned));
    #endif        

   // construct network <address:port> of remote host
   //
   sockaddr_in remoteAddress;
   memset(&remoteAddress, 0, sizeof(remoteAddress));
   remoteAddress.sin_family      = AF_INET;                                 // internet domain
   remoteAddress.sin_addr.s_addr = ipaddr; // address of remote host
   remoteAddress.sin_port        = htons(remotePort);                       // port on that host


#if 0
   // lookup remote host info
   //
   struct hostent ret;
   struct hostent *remoteInfo=NULL;
   char buf[1024];
   int gherr;
   int rc=gethostbyname_r (remoteHostName, &ret, buf, sizeof(buf), &remoteInfo, &gherr);
   
   if (rc || remoteInfo==NULL)
      {
      #ifndef _AIX
      if (verbose) printf("tcp connect error: can't resolve \"%s\": %s\n", remoteHostName, hstrerror(h_errno));
      #endif
      errno = ENXIO;
      return -1;
      }

   assert(remoteInfo->h_addrtype == AF_INET);
   assert(remoteInfo->h_length == sizeof(unsigned));

   // construct network <address:port> of remote host
   //
   memset(&remoteAddress, 0, sizeof(remoteAddress));
   remoteAddress.sin_family      = AF_INET;                                 // internet domain
   remoteAddress.sin_addr.s_addr = *(unsigned *)remoteInfo->h_addr_list[0]; // address of remote host
   remoteAddress.sin_port        = htons(remotePort);                       // port on that host
#endif

   // create a socket
   //
   int connectionFd = socket(AF_INET, SOCK_STREAM, 0);
   if (connectionFd == -1)
      {
      int save_errno = errno;
      if (verbose) printf("\ntcp socket error: %s\n", strerror(save_errno));
      errno = save_errno;
      return -1;
      }

   // make it non-blocking
   //
   if (fcntl(connectionFd, F_SETFL, fcntl(connectionFd, F_GETFL) | O_NONBLOCK))
      {
      int save_errno = errno;
      close(connectionFd);
      if (verbose) printf("\ntcp fcntl error: %s\n", strerror(save_errno));
      errno = save_errno;
      return -1;
      }
   
   // attempt to connect it to a host
   //
   time_t start = time(0);
   while (connect(connectionFd, (sockaddr *)&remoteAddress, sizeof(remoteAddress)))
      {
      if (errno == EISCONN)
         break; // AIX - socket is now connected
         
    //if (verbose) printf("\ntcp connect: still trying (%s)\n", strerror(errno));

      if (timeout == 0 || (time(0) - start < timeout))
         { // keep waiting
         usleep(100*1000);
         continue;
         }
         
      // preserve "host unreachable" and "connection refused" errors,
      // but relabel errors due to non-blocking io as "timeouts"
      //
      if (errno == EINPROGRESS || errno == EALREADY)
         errno = ETIMEDOUT;

      int save_errno = errno;
      close(connectionFd);
      if (verbose) printf("\ntcp connect error: %s\n", strerror(save_errno));
      errno = save_errno;
      return -1;
      }

   //   if (verbose)
   //      printf("TCPconnect ok\n");
 
   // restore blocking io
   //
   if (fcntl(connectionFd, F_SETFL, fcntl(connectionFd, F_GETFL) & ~O_NONBLOCK))
      {
      int save_errno = errno;
      close(connectionFd);
      if (verbose) printf("tcp fcntl error: %s\n", strerror(save_errno));
      errno = save_errno;
      return -1;
      }

   // force data written to socket to be sent immediately, rather than delaying in order to coalesce packets
   //
   int enable = 1;
   if (setsockopt(connectionFd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable)) == -1)
      {
      int save_errno = errno;
      close(connectionFd);
      if (verbose) printf("tcp socket option error: %s\n", strerror(save_errno));
      errno = save_errno;
      return -1;
      }

   return connectionFd;
   }

#endif /* TCPCONNECT_H */
