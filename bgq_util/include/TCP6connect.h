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


#ifndef TCP6CONNECT_H
#define TCP6CONNECT_H

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

#include <iostream>
#include <string>

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
    const std::string cnn(connectionName);
    const std::string remotePort = cnn.substr(cnn.find_last_of(':') + 1, cnn.length());
    const std::string remoteHostName = cnn.substr(0, cnn.find_last_of(':'));

    struct addrinfo* addrs = 0;
    struct addrinfo hints;

    bzero(&hints, sizeof(addrinfo));
    hints.ai_socktype = SOCK_STREAM;  // TCP
    struct sockaddr* remote_sockaddr;
    unsigned short ip_family = AF_INET;
    int connectionFd = -1;
    const int retval = getaddrinfo(remoteHostName.c_str(), remotePort.c_str(), &hints, &addrs);
    if (retval != 0) {
        if (verbose)
            std::cerr <<"failed to get address info: " << gai_strerror(retval) << " for address " << remoteHostName << std::endl;;
        return -1;
    }

    unsigned addrcount = 0;
    struct addrinfo* curr_addr = addrs;
    bool connected = false;
    while (curr_addr) {
        ++addrcount;
        ip_family = curr_addr->ai_family;
        remote_sockaddr = curr_addr->ai_addr;        

        // create a socket
        connectionFd = socket(ip_family, SOCK_STREAM, 0);
        if (connectionFd == -1) {
            const int save_errno = errno;
            if (verbose) printf("\ntcp socket error: %s\n", strerror(save_errno));
            errno = save_errno;
            if (addrs) {
                freeaddrinfo(addrs);
                addrs = 0;
            }

            return -1;
        }

        // make it non-blocking
        if (fcntl(connectionFd, F_SETFL, fcntl(connectionFd, F_GETFL) | O_NONBLOCK)) {
            const int save_errno = errno;
            close(connectionFd);
            if (verbose) printf("\ntcp fcntl error: %s\n", strerror(save_errno));
            errno = save_errno;
            if (addrs) {
                freeaddrinfo(addrs);
                addrs = 0;
            }
            return -1;
        }

        // attempt to connect it to a host
        time_t start = time(0);
        int size = AF_INET;
        if (ip_family == AF_INET)
            size = sizeof(sockaddr_in);
        else size = sizeof(sockaddr_in6);
        int connect_rc = 0;
        while (true) {
            connect_rc = connect(connectionFd, remote_sockaddr, size);
            if (connect_rc == 0 || errno == EISCONN) break;

            if (verbose) printf("\ntcp connect: still trying (%s)\n", strerror(errno));

            if (timeout == 0 || (time(0) - start < timeout)) {
                // keep waiting
                usleep(100*1000);
                continue;
            }

            // preserve "host unreachable" and "connection refused" errors,
            // but relabel errors due to non-blocking io as "timeouts"
            //
            if (errno == EINPROGRESS || errno == EALREADY)
                errno = ETIMEDOUT;
        }

        if (connect_rc != 0) {  // failed
            const int save_errno = errno;
            close(connectionFd);
            if (verbose) printf("\ntcp connect error: %s\n", strerror(save_errno));
            errno = save_errno;
            curr_addr = curr_addr->ai_next;
            continue;
        } else {
            connected = true;
            break;
        }
    }
    
    if (connected) {
        // restore blocking io
        if (fcntl(connectionFd, F_SETFL, fcntl(connectionFd, F_GETFL) & ~O_NONBLOCK)) {
            const int save_errno = errno;
            close(connectionFd);
            if (verbose) printf("tcp fcntl error: %s\n", strerror(save_errno));
            errno = save_errno;
            if (addrs) {
                freeaddrinfo(addrs);
                addrs = 0;
            }

            return -1;
        }

        // force data written to socket to be sent immediately, rather than delaying in order to coalesce packets
        const int enable = 1;
        if (setsockopt(connectionFd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable)) == -1) {
            const int save_errno = errno;
            close(connectionFd);
            if (verbose) printf("tcp socket option error: %s\n", strerror(save_errno));
            errno = save_errno;
            if (addrs) {
                freeaddrinfo(addrs);
                addrs = 0;
            }

            return -1;
        }
    }
    
    if (addrs) {
        freeaddrinfo(addrs);
        addrs = 0;
    }

    return connectionFd;
}

#endif /* TCPCONNECT_H */
