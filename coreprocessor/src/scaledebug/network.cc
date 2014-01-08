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
/* (C) Copyright IBM Corp.  2011, 2011                              */
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
#include "network.h"

int number_of_clients = 0;

#if USE_SECURE_CERTIFICATE
std::vector<CxxSockets::SecureTCPSocketPtr> SocketVector;
#else
std::vector<CxxSockets::TCPSocketPtr> SocketVector;
#endif

void findIPs(std::vector<string>& IPs) 
{    
    // Discover IP addresses on the machine for later use
    struct ifaddrs* ifa;
    struct ifaddrs* base_ifa;
    if(getifaddrs(&ifa) < 0) {
        perror("No valid interface IPs");
        exit(1);
    }
    
    base_ifa = ifa;
    char buff[128];
    bool skip = false;
    for (ifa = base_ifa; ifa != NULL; ifa = ifa->ifa_next) {
        bzero(buff, 128);
        if(ifa) {
            if((strncmp(ifa->ifa_name, "ib", 2) != 0) &&
               (strncmp(ifa->ifa_name, "et", 2) != 0))
                skip = true;
            if(ifa->ifa_addr->sa_family == AF_INET) {
                inet_ntop(ifa->ifa_addr->sa_family,
                          &((struct sockaddr_in*)(ifa->ifa_addr))->sin_addr.s_addr,
                          buff, 128);
            } else if(ifa->ifa_addr->sa_family == AF_INET6) {
                inet_ntop(ifa->ifa_addr->sa_family,
                          &((struct sockaddr_in6*)(ifa->ifa_addr))->sin6_addr.s6_addr,
                          buff, 128);
                skip = true; // \todo v6 scoped addressing architecture sucks.
            } else skip = true;
            std::ostringstream addr;
            addr << buff;
            if(ifa->ifa_addr->sa_family == AF_INET6 && addr.str() != "::1") {
                addr << "%" << ifa->ifa_name;
            }
            if(!skip)
            {
                //              cout << "IP Address: " << addr.str() << endl;                
                IPs.push_back(addr.str());
            }
            skip = false;
        }
    }
    
    freeifaddrs(base_ifa);
}

int connectToTool(int argc, char* argv[])
{
    bgq::utility::Properties::Ptr props;
    if (!props) {
        props = bgq::utility::Properties::create();
    }
    bgq::utility::initializeLogging(*props);
    
    try 
    {
        CxxSockets::SockAddrList side_salist(AF_INET, "", "34543");
        CxxSockets::ListenerSetPtr side_listener(new CxxSockets::ListenerSet(side_salist));

#if USE_SECURE_CERTIFICATE
        CxxSockets::SecureTCPSocketPtr sock(new CxxSockets::SecureTCPSocket());
        bgq::utility::ServerPortConfiguration port_config(0);
        port_config.setProperties(props, "");
        port_config.notifyComplete();
        side_listener->AcceptNew(sock, port_config);
#else
        CxxSockets::TCPSocketPtr sock = (CxxSockets::TCPSocketPtr)new CxxSockets::TCPSocket();
        side_listener->AcceptNew(sock);
        {
            CxxSockets::Message side_msg("READY");
            sock->Send(side_msg);
            side_msg.str().clear();
            SocketVector.push_back(sock);
        }
#endif
    }
    catch(CxxSockets::Error& err)
    {
        return -1;
    }
    return 0;
}

int sendCommand(const char* str)
{
    CxxSockets::Message side_msg(str);
#if USE_SECURE_CERTIFICATE
    for(std::vector<CxxSockets::SecureTCPSocketPtr>::iterator iter = SocketVector.begin(); iter != SocketVector.end(); ++iter)
#else
    for(std::vector<CxxSockets::TCPSocketPtr>::iterator iter = SocketVector.begin(); iter != SocketVector.end(); ++iter)
#endif
    {
        (*iter)->Send(side_msg);
    }
    side_msg.str().clear();
}

int receiveReply(MsgMap& msgs)
{
#if USE_SECURE_CERTIFICATE
    for(std::vector<CxxSockets::SecureTCPSocketPtr>::iterator iter = SocketVector.begin(); iter != SocketVector.end(); ++iter)
#else
    for(std::vector<CxxSockets::TCPSocketPtr>::iterator iter = SocketVector.begin(); iter != SocketVector.end(); ++iter)
#endif
    {
        CxxSockets::Message side_msg;
        (*iter)->Receive(side_msg);
#if USE_SECURE_CERTIFICATE
        std::pair<CxxSockets::SecureTCPSocketPtr, std::string> element(*iter,side_msg.str());
#else
        std::pair<CxxSockets::TCPSocketPtr, std::string> element(*iter,side_msg.str());
#endif
        
        cout << side_msg.str() << endl;  // temporary hack until real parser is inplace
        
        msgs.insert(element);
    }
}
