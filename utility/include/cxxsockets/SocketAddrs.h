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
#ifndef _SOCKETADDRS_H
#define _SOCKETADDRS_H

#include <iostream>
#include <vector>
#include <deque>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

static log4cxx::LoggerPtr sockaddr_logger(log4cxx::Logger::getLogger( "ibm.utility.cxxsockets.SocketAddrs" ));

namespace CxxSockets {

    class SockAddr;

    //! \brief SockAddr class for representing IP address structures.
    //!
    //! Inherit from sockaddr_storage and provide wrappers
    //! for generic address calls like getnameinfo()
    //! There are no data members.  This allows us to pass a pointer
    //! to this object to the BSD C API.
    class SockAddr : sockaddr_storage {
        friend class SockAddrList;
        static bool Addrinf(struct addrinfo*& addrinf, unsigned short family, std::string nodename, std::string service);
        void assignSa(sockaddr* sa);
    public:
        //! Constructors:
        SockAddr() { bzero(this, sizeof(sockaddr_storage)); }
     
        //! \brief Constructor based on a sockaddr struct
        SockAddr(sockaddr* sa);

        //! \brief Build a SockAddr from passed in data.
        //!
        //! \param family Address family e.g. AF_INET, AF_INET6, AF_UNSPEC
        //! \param nodename hostname or IP address string
        //! \param service port or service name from /etc/services
        SockAddr(unsigned short family, std::string nodename = "", std::string service = "");

        //! \brief assignment operators for sockaddr structures
        SockAddr& operator=(sockaddr* sa);
        SockAddr& operator=(sockaddr& sa);

        //! \brief getnameinfo wrappers
        //! Returns the service associated with the port
        std::string getServiceName() { 
            char svc_buf[NI_MAXSERV];
            int rc = getnameinfo((sockaddr*)(this), sizeof(sockaddr_storage), NULL, 0,
                        svc_buf, NI_MAXSERV, 0);
            if(rc != 0) {
                std::ostringstream msg;
                msg << "getServiceName error: " << gai_strerror(rc);
                throw CxxSockets::CxxSocketInternalError(rc, msg.str());
            }
            std::string sname(svc_buf);
            LOG4CXX_TRACE(sockaddr_logger, "Service Name " << sname);
            return sname;
        }

        //! \brief Returns the service port number (always use for ephemerals)
        int getServicePort() {
            char svc_buf[NI_MAXSERV];
            int rc = getnameinfo((sockaddr*)(this), sizeof(sockaddr_storage), NULL, 0,
                                 svc_buf, NI_MAXSERV, NI_NUMERICSERV);
            if(rc != 0) {
                std::ostringstream msg;
                msg << "getServiceName error: " << gai_strerror(rc);
                throw CxxSockets::CxxSocketInternalError(rc, msg.str());
            }
            LOG4CXX_TRACE(sockaddr_logger, "Service port " << svc_buf);
            int retval = atoi(svc_buf);
            return retval;
        }

        //! \brief Returns the numeric form of the IP address
        std::string getHostAddr();

        //! \brief Returns the host name
        // If there is no associated name, it'll throw a soft error exception.
        std::string getHostName() {
            char host_buf[NI_MAXHOST];
            int rc = getnameinfo((sockaddr*)(this), sizeof(sockaddr_storage), host_buf, sizeof(host_buf),
                                 0, 0, 0);
            if(rc != 0) {
                std::ostringstream msg;
                msg << "getHostName error: " << gai_strerror(rc);
                throw CxxSockets::SockSoftError(rc, msg.str());
            }
            std::string sname(host_buf);
            LOG4CXX_TRACE(sockaddr_logger, "Host Name " << sname);
            return sname;
        }

        //! \brief Returns real address family
        int family() { return ss_family == AF_INET6_ONLY ? AF_INET6 : ss_family; }

        //! \brief Returns real address family OR cached AF_INET6_ONLY value
        int fm() { return ss_family; }

        //! \brief Set the address family
        void setFamily(const int fam) { ss_family = fam; }
    };

    //! \brief List of SockAddrs
    class SockAddrList {
        //! \brief double-ended-queue of SockAddrs
        std::deque<SockAddr> _sockque;
    public:
        typedef std::deque<SockAddr>::iterator iterator;
        typedef std::deque<SockAddr>::const_iterator const_iterator;

        //! \brief Default constructor
        SockAddrList() {}

        //! \brief Constructor
        //!
        //! \param family Address family e.g. AF_INET, AF_INET6, AF_UNSPEC
        //! \param nodename hostname or IP address string
        //! \param service port or service name from /etc/services
        SockAddrList(unsigned short family, std::string nodename = "", std::string service = "");

        //! \brief Constructor
        //!
        //! \param family Address family e.g. AF_INET, AF_INET6, AF_UNSPEC
        //! \param nodenames vector of hostnames or IP address strings
        //! \param service port or service name from /etc/services
        SockAddrList(unsigned short family, std::vector<std::string>& nodenames, std::string service = "");

        //! \brief By default the list is ordered IPv6 first.  This lets you reverse that.
        //! Useful if you know you want to be IPv4 only.
        void ReverseVersionOrder();

        //! \brief Get the IP address strings
        //!
        //! \param ips Pass in an empty vector and it gets filled with the IP addresses
        //! \returns A reference to the value passed in.
        std::vector<std::string>& getIPs(std::vector<std::string>& ips);

        //! forwarded operations
        SockAddr& front() { return _sockque.front(); }
        size_t size() { return _sockque.size(); }
        void push_back(SockAddr& sa) { _sockque.push_back(sa); }
        void pop_back() { _sockque.pop_back(); }
        void push_front(SockAddr& sa) { _sockque.push_front(sa); }
        iterator insert ( iterator position, SockAddr& sa ) { return _sockque.insert(position, sa); }
        iterator begin() { return _sockque.begin(); }                                        
        iterator end() { return _sockque.end(); }                                            
        const_iterator begin() const { return _sockque.begin(); }                            
        const_iterator end() const { return _sockque.end(); }        
    };
}
#endif
