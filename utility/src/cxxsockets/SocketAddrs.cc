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
#include "cxxsockets/SocketTypes.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

using namespace CxxSockets;

LOG_DECLARE_FILE( "utility.cxxsockets" );

bool SockAddr::Addrinf(struct addrinfo*& addrinf, unsigned short family, std::string nodename, std::string service) {
    struct addrinfo hints;
    bzero(&hints, sizeof(addrinfo));

    hints.ai_family = family;
    char* node;
    if(nodename.size() == 0) {
        node = NULL;
        hints.ai_flags = AI_PASSIVE;
    }
    else {
        node = (char*)(nodename.c_str());
    }

    // Note:  This is TCP only.
    hints.ai_socktype = SOCK_STREAM;
    LOG_TRACE_MSG("Getting addr info for " << nodename << ":" << service);
    int retval = getaddrinfo(node, service.c_str(), &hints, &addrinf);
    if(retval != 0) {
        std::ostringstream msg;
        msg << "failed to get address info: " << gai_strerror(retval) << " for addr " << nodename;
        LOG_INFO_MSG(msg.str());
        if(addrinf)
            freeaddrinfo(addrinf);
        throw CxxSockets::SockHardError(errno, msg.str());
    }
    
    char buff[128];
    LOG_TRACE_MSG("Addr Info: " << inet_ntop(family, &((struct sockaddr_in*)(addrinf->ai_addr))->sin_addr.s_addr, buff, 128));
    if(!addrinf) {
        std::ostringstream msg;
        msg << "Unable to resolve address: " << strerror(errno);
        LOG_INFO_MSG(msg.str());
        throw CxxSockets::SockHardError(errno, msg.str());
    }
    return true;
};

std::string
SockAddr::getServiceName()
{ 
    char svc_buf[NI_MAXSERV];
    int rc = getnameinfo((sockaddr*)(this), sizeof(sockaddr_storage), NULL, 0,
            svc_buf, NI_MAXSERV, 0);
    if(rc != 0) {
        std::ostringstream msg;
        msg << "getServiceName error: " << gai_strerror(rc);
        throw CxxSocketInternalError(rc, msg.str());
    }
    std::string sname(svc_buf);
    LOG_TRACE_MSG("Service Name " << sname);
    return sname;
}

int
SockAddr::getServicePort()
{
    char svc_buf[NI_MAXSERV];
    int rc = getnameinfo((sockaddr*)(this), sizeof(sockaddr_storage), NULL, 0,
            svc_buf, NI_MAXSERV, NI_NUMERICSERV);
    if(rc != 0) {
        std::ostringstream msg;
        msg << "getServiceName error: " << gai_strerror(rc);
        throw CxxSocketInternalError(rc, msg.str());
    }
    LOG_TRACE_MSG("Service port " << svc_buf);
    int retval = atoi(svc_buf);
    return retval;
}

std::string
SockAddr::getHostName()
{
    char host_buf[NI_MAXHOST];
    int rc = getnameinfo((sockaddr*)(this), sizeof(sockaddr_storage), host_buf, sizeof(host_buf),
            0, 0, 0);
    if(rc != 0) {
        std::ostringstream msg;
        msg << "getHostName error: " << gai_strerror(rc);
        throw SockSoftError(rc, msg.str());
    }
    std::string sname(host_buf);
    LOG_TRACE_MSG("Host Name " << sname);
    return sname;
}

std::string
SockAddr::getHostAddr()
{
    char host_buf[NI_MAXHOST];
    int size = 0;
    if(family() == AF_INET)
        size = sizeof(sockaddr_in);
    else
        size = sizeof(sockaddr_in6);

    int error = getnameinfo((sockaddr*)(this), size, host_buf, sizeof(host_buf),
                            0, 0, NI_NUMERICHOST);
    if(error != 0) {
        std::ostringstream msg;
        LOG_INFO_MSG("Unable to find host address: " << gai_strerror(error));
        msg << "Unable to find host address: " << gai_strerror(error);
        LOG_INFO_MSG(msg.str());
        throw CxxSockets::CxxSocketInternalError(0, msg.str());
    }
    std::string sname(host_buf);
    LOG_TRACE_MSG("host addr: " << sname);
    return sname;
}

SockAddr::SockAddr(sockaddr* sa) {
    assignSa(sa);
}

void SockAddr::assignSa(sockaddr* sa) {
    // Make sure there's no junk in the base object.
    bzero(this, sizeof(SockAddr));

    // Overwrite the base object with the new one.
    int size = 0;
    if(sa->sa_family == AF_INET) {
        size = sizeof(sockaddr_in);
    } else if(sa->sa_family == AF_INET6) {
        size = sizeof(sockaddr_in6);
    }
    else if(sa->sa_family == AF_LOCAL) {
        size = SUN_LEN((sockaddr_un *)sa);
    }
    else {
         std::ostringstream msg;
         msg << "invalid address family: " << sa->sa_family;
         LOG_ERROR_MSG(msg.str());
         throw CxxSockets::SockSoftError(EINVAL, msg.str());
    }

    //    memcpy(this, sa, sizeof(sockaddr_storage)); 
    memcpy(this, sa, size); 
}

SockAddr& SockAddr::operator=(sockaddr* sa) {
    assignSa(sa);
    return *this;
}

SockAddr& SockAddr::operator=(sockaddr& sa) {
    assignSa(&sa);
    return *this;
}

SockAddr::SockAddr(unsigned short family, std::string nodename , std::string service) {
    bzero(this, sizeof(sockaddr_storage));
    struct addrinfo* addrinf = 0;
    Addrinf(addrinf, family, nodename, service);
    if(addrinf) {
        SockAddr sa(addrinf->ai_addr);
        if(family == AF_INET6_ONLY)
            sa.setFamily( AF_INET6_ONLY );
    }
 
    // Copy the addrinfo struct's sockaddr back in to us.
    int size = 0;
    if(addrinf->ai_family == AF_INET) {
        size = sizeof(sockaddr_in);
    } else {
        size = sizeof(sockaddr_in6);
    }
    
    memcpy(this, addrinf->ai_addr, size);

    freeaddrinfo(addrinf);
}

SockAddrList::SockAddrList(unsigned short family, std::string nodename, std::string service) {
    if(service.size() == 0 && nodename.size() == 0) {
        std::ostringstream msg;
        msg << "both service and node cannot be zero";
        LOG_ERROR_MSG(msg.str());
        throw CxxSockets::CxxSocketUserError(-1, msg.str());
    }

    struct addrinfo* addrinf = 0;
    SockAddr::Addrinf(addrinf, family, nodename, service);
    const boost::shared_ptr<struct addrinfo> guard( addrinf, boost::bind(&freeaddrinfo, _1) );
    int addrinfs = 0;
    while (addrinf) {
        ++addrinfs;
        SockAddr sa(addrinf->ai_addr);
        if(family == AF_INET6_ONLY)
            sa.setFamily( AF_INET6_ONLY );
        if(sa.family() == AF_INET)
            push_back(sa);
        else if(sa.family() == AF_INET6) // v6 goes first
            push_front(sa);
        addrinf = addrinf->ai_next;
    }
}

SockAddrList::SockAddrList(unsigned short family, std::vector<std::string>& nodenames, std::string service) {
    struct addrinfo* addrinf = 0;
    for (std::vector<std::string>::iterator it = nodenames.begin(); it != nodenames.end(); ++it) {
        SockAddr::Addrinf(addrinf, family, (*it), service);
        while (addrinf) {
            SockAddr sa(addrinf->ai_addr);
            if(family == AF_INET6_ONLY)
                sa.setFamily( AF_INET6_ONLY );
            if(sa.family() == AF_INET)
                push_back(sa);
            else if(sa.family() == AF_INET6) // v6 goes first
                push_front(sa);
            addrinf = addrinf->ai_next;
        }
        freeaddrinfo(addrinf);
    }
}

std::vector<std::string>& SockAddrList::getIPs(std::vector<std::string>& ips){
    for(iterator i = begin(); i != end(); ++i) {
        LOG_DEBUG_MSG("host ip: " << (*i).getHostAddr());
        ips.push_back((*i).getHostAddr());
    }
    return ips;
}

void SockAddrList::ReverseVersionOrder() {
    // Reverses the list. IPv6 addresses are at the front by default.
    for (unsigned int count = 0; count < size(); ++count) {
        std::deque<SockAddr>::iterator end = _sockque.end() - 1;
        insert(begin() + count, *end);
        pop_back();
    }
}
