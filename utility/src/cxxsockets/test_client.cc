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
#include <iostream>
#include <string>
#include <ifaddrs.h>
#include <boost/lexical_cast.hpp>
#include "SocketTypes.h"
#include "clientmacros.h"
#include "Log.h"

// Message protocol:
// Server Sends a "READY" message on start
// Client sends a "START" message to tell server to run its end of the test
// Client sends a "DONE" message when the test is done
// Server sends a "READY" message when it gets a "DONE"
// Client sends a "QUIT" message when it is finished.

using namespace std;

void findIPs(std::vector<string>& IPs) {

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
    while (ifa->ifa_next) {
        bzero(buff, 128);
        if(ifa) {
            if(ifa->ifa_addr->sa_family == AF_INET) {
                inet_ntop(ifa->ifa_addr->sa_family,
                          &((struct sockaddr_in*)(ifa->ifa_addr))->sin_addr.s_addr,
                          buff, 128);
            } else if(ifa->ifa_addr->sa_family == AF_INET6) {
                inet_ntop(ifa->ifa_addr->sa_family,
                          &((struct sockaddr_in6*)(ifa->ifa_addr))->sin6_addr.s6_addr,
                          buff, 128);
                skip = true; // Not doing v6 scoped addresses.
            } else skip = true;
            std::ostringstream addr;
            addr << buff;
            if(ifa->ifa_addr->sa_family == AF_INET6 && addr.str() != "::1") {
                addr << "%" << ifa->ifa_name;
            }
            if(!skip)
                IPs.push_back(addr.str());
            skip = false;
        }
        ifa = ifa->ifa_next;
    }

    freeifaddrs(base_ifa);
}

int main(int argc, const char** argv) {

    bgq::utility::Properties::Ptr props;

    bool securetest = false;
    bool usertest = false;

    for (int i = 1; i < argc; ++i) {
        if (!strcasecmp(argv[i], "--properties")) {
            if (argc == ++i) {
                std::cerr << "please give a file name after " << argv[i-1] << std::endl;
                exit(EXIT_FAILURE);
            }
            props = bgq::utility::Properties::create(argv[i]);
        }
        if(!strcasecmp(argv[i], "--secure")) {
            securetest = true;
        }
        if(!strcasecmp(argv[i], "--userid")) {
            usertest = true;
        }
    }

    // Properties should be initialized if we are using security.
    if (!props) {
        std::cout << "setting up properties from envvar" << std::endl;
        props = bgq::utility::Properties::create();
    }

    // Initialize global properties.
    CxxSockets::setProperties(props);
    bgq::utility::initializeLogging(*props);

    std::vector<std::string> ipas;
    findIPs(ipas);
    std::cout << "IPs found on this machine: " << std::endl;
    for(std::vector<std::string>::iterator it = ipas.begin();
        it != ipas.end(); ++it)
        std::cout << (*it) << std::endl;

    unsigned int failed = 0;
    unsigned int completed = 0;

    // Set up the side protocol
    CxxSockets::SockAddr side_remote(AF_INET, "localhost", "34543");
    CxxSockets::SockAddrList side_local(AF_INET, "localhost", "");
    CxxSockets::TCPSocketSetPtr side_sockets(new CxxSockets::TCPSocketSet(side_local));
    CxxSockets::FourTuple sft(side_local.front(), side_remote);
    CxxSockets::TCPSocketPtr side_sock = side_sockets->Retrieve();
    side_sock->Connect(sft);
    
    {
        CxxSockets::Message side_msg;
        side_sock->Receive(side_msg);
        std::cout << side_msg.str() << std::endl;
        assert(side_msg.str() == "READY");
        side_msg.str().clear();
    }
    // End side protocol block

    std::string testcase;

    testcase = "LTC1";

    // Listener on AF_INET.  Should only connect to that
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr remote(AF_INET, "localhost", "30002");
        CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
        socket->Connect(remote);
        std::cout << testcase << ": SUCCESS" << std::endl;
    } catch (CxxSockets::SockHardError e) {
        std::cout << testcase << ": Socket error detected: " << e.what() << std::endl;
    }

    PBDONE();

    testcase = "LTC2";

    // Listener on AF_INET.  Should fail to connect to that.
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr remote(AF_INET6, "::1", "30002");
        CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
        socket->Connect(remote);
        std::cout << testcase << " FAILED: connected" << std::endl;
        ++failed;
    } catch (CxxSockets::SockHardError e) {
        std::cout << testcase << " SUCCESS: Socket error detected: " << e.what() << std::endl;
    }

    PBDONE();

    testcase = "LTC3";

    // Listener on AF_UNSPEC.  Should connect to that
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr remote(AF_INET, "localhost", "30002");
        CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
        socket->Connect(remote);
        std::cout << testcase << ": SUCCESS" << std::endl;
    } catch (CxxSockets::SockHardError e) {
        std::cout << testcase << " FAILED: Socket error detected: " << e.what() << std::endl;
        ++failed;
    }

    PBDONE();

    testcase = "LTC4";

    // Listener on AF_UNSPEC.  Should connect to that
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr remote(AF_INET6, "::1", "30002");
        CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
        socket->Connect(remote);
        std::cout << testcase << ": SUCCESS" << std::endl;
    } catch (CxxSockets::SockHardError e) {
        std::cout << testcase << " FAILED: Socket error detected: " << e.what() << std::endl;
        ++failed;
    }

    PBDONE();

    testcase = "LTC5";
    // Listener on AF_UNSPEC.  Should connect to that
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr remote(AF_INET, "localhost", "30002");
        CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
        socket->Connect(remote);
        std::cout << testcase << ": SUCCESS" << std::endl;
    } catch (CxxSockets::SockHardError e) {
        ++failed;
        std::cout << testcase << ": FAILED: Socket error detected: " << e.what() << std::endl;
    }

    PBDONE();

    testcase = "LTC6";
    // Listener on AF_UNSPEC.  Should connect to that
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr remote(AF_INET6, "localhost", "30002");
        CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
        socket->Connect(remote);
        std::cout << testcase << ": SUCCESS" << std::endl;
    } catch (CxxSockets::SockHardError e) {
        std::cout << testcase << " FAILED: Socket error detected: " << e.what() << std::endl;
        ++failed;
    }

    PBDONE();

    testcase = "LTC7";
    // Listener on AF_INET.  Should connect to that
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr remote(AF_INET, "localhost", "30002");
        CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
        socket->Connect(remote);
        std::cout << testcase << ": SUCCESS" << std::endl;
    } catch (CxxSockets::SockHardError e) {
        std::cout << testcase << " FAILED: Socket error detected: " << e.what() << std::endl;
        ++failed;
    }

    PBDONE();

    testcase = "LTC8";

    // Listener on AF_UNSPEC.  Should connect to that
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr remote(AF_INET6, "localhost", "30002");
        CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
        socket->Connect(remote);
        std::cout << testcase << ": SUCCESS" << std::endl;
    } catch (CxxSockets::SockHardError e) {
        std::cout << testcase << " FAILED: Socket error detected: " << e.what() << std::endl;
        ++failed;
    }

    PBDONE();

    testcase = "LTC9";

    // Listener on AF_INET6_ONLY.  Should fail to connect to that
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr remote(AF_INET, "localhost", "30002");
        CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
        socket->Connect(remote);
        std::cout << testcase << " FAILED: connection completed" << std::endl;
        ++failed;
    } catch (CxxSockets::SockHardError e) {
        std::cout << testcase << ": SUCCESS: Socket error detected: " << e.what() << std::endl;
    }

    PBDONE();
    testcase = "LTC10";

    // Listener on AF_UNSPEC.  Should connect to that
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr remote(AF_INET6, "localhost", "30002");
        CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
        socket->Connect(remote);
        std::cout << testcase << ": SUCCESS" << std::endl;
    } catch (CxxSockets::SockHardError e) {
        std::cout << testcase << " FAILED: Socket error detected: " << e.what() << std::endl;
        ++failed;
    }

    PBDONE();

    testcase = "LTC11";
    // Listener is polling, just connect
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr remote(AF_INET, "localhost", "30002");
        CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
        socket->Connect(remote);
        std::cout << testcase << ": SUCCESS" << std::endl;
    } catch (CxxSockets::SockHardError e) {
        std::cout << testcase << " FAILED: Socket error detected: " << e.what() << std::endl;
        ++failed;
    }
    PBDONE();

    testcase = "LTC12";
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    for(std::vector<std::string>::iterator it = ipas.begin(); 
        it != ipas.end(); ++it)
        PBSENDSTRING((*it));
    PBSENDSTRING("FINI");
    PBREADYWAIT();
    unsigned int conncount = 0;
    for (std::vector<std::string>::iterator it = ipas.begin(); it != ipas.end(); ++it) {
        try {
            CxxSockets::SockAddr remote(CxxSockets::findFamily(*it), (*it), "30002");
            CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
            socket->Connect(remote);
            ++conncount;
        } catch (CxxSockets::SockHardError e) {
            std::cout << testcase << " FAILED: Socket error detected: " << e.what() << std::endl;
            ++failed;
        }
    }
    if(conncount == ipas.size())
        std::cout << testcase << ": SUCCESS connected to " << conncount << " ips" << std::endl;
    
    PBDONE();

    testcase = "LTC13";
    // Listener is polling, just connect
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    for (std::vector<std::string>::iterator it = ipas.begin(); it != ipas.end(); ++it) {
        PBSENDSTRING((*it));
    }
    PBSENDSTRING("FINI");
    PBREADYWAIT();
    // Put a bad one in the mix which should fail
    ipas.push_back("mybogushost");
    conncount = 0;
    for (std::vector<std::string>::iterator it = ipas.begin(); it != ipas.end(); ++it) {
        try {
            CxxSockets::SockAddr remote(CxxSockets::findFamily(*it), (*it), "30002");
            CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
            socket->Connect(remote);
            ++conncount;
        } catch (CxxSockets::SockHardError e) {
            std::cout << testcase << " Socket error detected: " << e.what() << std::endl;
        }
    }
    if(conncount == ipas.size() - 1)
        std::cout << testcase << ": SUCCESS connected to " << conncount << " ips" << std::endl;
    else {
        std::cout << testcase << ": FAILED connected to " << conncount << " ips" << std::endl;
        ++failed;
    }
    // Take "bogus" back out.
    ipas.pop_back();

    PBDONE();

    testcase = "LTC14";
    // Listener is polling, just connect
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr remote(AF_INET, "localhost", "30002");
        CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
        socket->Connect(remote);
        std::cout << testcase << ": SUCCESS" << std::endl;
    } catch (CxxSockets::SockHardError e) {
        std::cout << testcase << " FAILED: Socket error detected: " << e.what() << std::endl;
        ++failed;
    }
    PBDONE();

    testcase = "LTC15";
    std::cout << testcase << " starting" << std::endl;
    PBSTART();

    for(std::vector<std::string>::iterator it = ipas.begin(); 
        it != ipas.end(); ++it)
        PBSENDSTRING((*it));
    PBSENDSTRING("FINI");
    PBREADYWAIT();
    conncount = 0;
    for (std::vector<std::string>::iterator it = ipas.begin(); it != ipas.end(); ++it) {
        try {
            CxxSockets::SockAddr remote(CxxSockets::findFamily(*it), (*it), "30002");
            CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
            socket->Connect(remote);
            ++conncount;
        } catch (CxxSockets::SockHardError e) {
            std::cout << testcase << " FAILED: Socket error detected: " << e.what() 
                      << " for IP Address " << (*it) << std::endl;
            ++failed;
        }
    }
    if(conncount == ipas.size())
        std::cout << testcase << ": SUCCESS connected to " << conncount << " ips" << std::endl;
    
    PBDONE();

    testcase = "LTC16";
    // Listener is polling, just connect
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    for (std::vector<std::string>::iterator it = ipas.begin(); it != ipas.end(); ++it) {
        PBSENDSTRING((*it));
    }
    PBSENDSTRING("FINI");
    PBREADYWAIT();
    // Put a bad one in the mix which should fail
    ipas.push_back("mybogushost");
    conncount = 0;
    for (std::vector<std::string>::iterator it = ipas.begin(); it != ipas.end(); ++it) {
        try {
            CxxSockets::SockAddr remote(CxxSockets::findFamily(*it), (*it), "30002");
            CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
            socket->Connect(remote);
            ++conncount;
        } catch (CxxSockets::SockHardError e) {
            std::cout << testcase << " Socket error detected: " << e.what() << std::endl;
        }
    }
    if(conncount == ipas.size() - 1)
        std::cout << testcase << ": SUCCESS connected to " << conncount << " ips" << std::endl;
    else {
        std::cout << testcase << ": FAILED connected to " << conncount << " ips" << std::endl;
        ++failed;
    }
    // Take "bogus" back out.
    ipas.pop_back();

    PBDONE();

    testcase = "SRTC1";
    // Connect to listener and send a short message.
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr remote(AF_INET6, "localhost", "30022");
        CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
        socket->Connect(remote);
        CxxSockets::Message msg("Short Message");
        socket->Send(msg);
        PBRESULT();
        std::cout << testcase << ": SUCCESS" << std::endl;
    } catch (CxxSockets::SockHardError e) {
        std::cout << testcase << " FAILED: Socket error detected: " << e.what() << std::endl;
        ++failed;
    }
    PBDONE();

    testcase = "SRTC2";
    // Connect to listener and send a short message.
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr remote(AF_INET6, "localhost", "30002");
        CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
        socket->Connect(remote);
        // Create a big data chunk to send
        CxxSockets::Message msg;
        for(unsigned int i = 0; i < 10000000; ++i)
            msg << i % 10;
        socket->Send(msg);
        PBRESULT();
        std::cout << testcase << ": SUCCESS" << std::endl;
    } catch (CxxSockets::SockHardError e) {
        std::cout << testcase << " FAILED: Socket error detected: " << e.what() << std::endl;
        ++failed;
    }
    PBDONE();

    testcase = "SRTC3";
    // Connect to listener and send a short message, unmanaged.
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr remote(AF_INET6, "localhost", "30002");
        CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
        socket->Connect(remote);
        CxxSockets::Message msg("BRIEF");
        socket->SendUnManaged(msg);
        PBRESULT();
        std::cout << testcase << ": SUCCESS" << std::endl;
    } catch (CxxSockets::CxxError e) {
        std::cout << testcase << " FAILED: Socket error detected: " << e.what() << std::endl;
        ++failed;
    }
    PBDONE();

    testcase = "RTC1";
    // Connect to the listener several times and send several messages.
    // All should be received on the correct connections.
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr remote(AF_INET6, "localhost", "30002");
        for(int i = 0; i < 5 ; ++i) {
            CxxSockets::Message msg;
            CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
            socket->Connect(remote);
            msg << "BRIEF" << i;
            socket->Send(msg);
        }
        PBRESULT();
        std::cout << testcase << ": SUCCESS" << std::endl;
    } catch (CxxSockets::CxxError e) {
        std::cout << testcase << " FAILED: Socket error detected: " << e.what() << std::endl;
        ++failed;
    }
    PBDONE();

    testcase = "RTC2";
    // Connect to the listener several times and send several messages.
    // All should be received on the correct connections.
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr remote(AF_INET6, "localhost", "30002");
        for(int i = 0; i < 5 ; ++i) {
            CxxSockets::Message msg;
            CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
            socket->Connect(remote);
            msg << "RTC2" << i;
            socket->Send(msg);
        }
        PBRESULT();
        std::cout << testcase << ": SUCCESS" << std::endl;
    } catch (CxxSockets::CxxError e) {
        std::cout << testcase << " FAILED: Socket error detected: " << e.what() << std::endl;
        ++failed;
    }
    PBDONE();

    testcase = "RTC3";
    // Connect to listener and send a short message.
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr remote(AF_INET6, "localhost", "30002");
        CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
        socket->Connect(remote);
        CxxSockets::Message msg("Short Message");
        socket->Send(msg);
        PBRESULT();
        std::cout << testcase << ": SUCCESS" << std::endl;
    } catch (CxxSockets::SockHardError e) {
        std::cout << testcase << " FAILED: Socket error detected: " << e.what() << std::endl;
        ++failed;
    }
    PBDONE();

    testcase = "RTC4";
    // Connect to listener and send a short message.
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr remote(AF_INET6, "localhost", "30002");
        CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
        socket->Connect(remote);
        CxxSockets::Message msg("Short Message");
        socket->Send(msg);
        CxxSockets::Message rcvmsg;
        socket->Receive(rcvmsg);
        PBRESULT();
        if(rcvmsg.str() == "Short Reply") {
            std::cout << testcase << ": SUCCESS" << std::endl;
        } else {
            std::cout << testcase << " FAILED.  Bogus reply from server." << std::endl;
            ++failed;
        }
    } catch (CxxSockets::SockHardError e) {
        std::cout << testcase << " FAILED: Socket error detected: " << e.what() << std::endl;
        ++failed;
    }
    PBDONE();

    testcase = "STC1";
    // Connect to listener and send a short message.
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr remote(AF_INET6, "localhost", "30002");
        CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
        socket->Connect(remote);
        // Send a message and get a reply to confirm health
        CxxSockets::Message msg("First Message");
        socket->Send(msg);
        CxxSockets::Message rcvmsg;
        socket->Receive(rcvmsg);
        if(rcvmsg.str() == "First Reply") {
        } else {
            std::cout << testcase << " FAILED.  Bogus reply from server." << std::endl;
            ++failed;
        }

        // Shutdown send side and try to send
        socket->Shutdown(CxxSockets::Socket::SEND);
        try {
            socket->Send(msg);
        } catch (CxxSockets::SockSoftError e) {
            string error(e.what());
            if(error.find("send side closed") == 0) {
                std::cout << testcase << " FAILED: Socket error detected: " << e.what() << std::endl;
                ++failed;
            } else {
                std::cout << testcase << " :SUCCESS.  Correctly failed to send to closed socket. " << e.what() << std::endl;
            }
        }

    } catch (CxxSockets::SockHardError e) {
        std::cout << testcase << " FAILED: Socket error detected: " << e.what() << std::endl;
        ++failed;
    }
    PBRESULT();
    PBDONE();

    testcase = "STC2";
    // Connect to listener and send a short message.
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr remote(AF_INET6, "localhost", "30002");
        CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
        socket->Connect(remote);
        // Send a message and get a reply to confirm health
        CxxSockets::Message msg("First Message");
        socket->Send(msg);
        CxxSockets::Message rcvmsg;
        socket->Receive(rcvmsg);
        if(rcvmsg.str() == "First Reply") {
        } else {
            std::cout << testcase << " FAILED.  Bogus reply from server." << std::endl;
            ++failed;
        }

        // Shutdown send side and try to receive
        // another message.
        socket->Shutdown(CxxSockets::Socket::SEND);
        CxxSockets::Message rcvmsg2;
        socket->Receive(rcvmsg2);
        if(rcvmsg2.str() == "Second Reply") {
            std::cout << testcase << " SUCCESS.  Correctly received message \"" << rcvmsg2.str() << "\"" << std::endl;
        } else {
            std::cout << testcase << " FAILED.  Bogus reply from server: " << rcvmsg2.str() << std::endl;
            ++failed;
        }
    } catch (CxxSockets::SockHardError e) {
        std::cout << testcase << " FAILED: Socket error detected: " << e.what() << std::endl;
        ++failed;
    }
    PBRESULT();
    PBDONE();

    // SockAddr tests
    // getHostAddr() test
    testcase = "SATC1";
    std::cout << testcase << " starting" << std::endl;
    try {
        CxxSockets::SockAddrList salist(AF_UNSPEC, ipas, "30002");
        std::vector<string> values;
        for(CxxSockets::SockAddrList::iterator it = salist.begin();
            it != salist.end(); ++it) {
            values.push_back((*it).getHostAddr());
        }

        unsigned int foundone = 0;
        for(std::vector<string>::iterator it = values.begin();
            it != values.end(); ++it) {
            std::vector<string>::iterator one = 
                find(ipas.begin(), ipas.end(), (*it));
            if(one != ipas.end()) {
                ++foundone;
            }
        }

        if(foundone == values.size()) {
            ++completed;
            std::cout << testcase << " SUCCESS" << std::endl;
        } else {
            ++failed;
            std::cout << testcase << " FAILED" << std::endl;
        }
    } catch(CxxSockets::CxxError e) {
        std::cout << testcase << " FAILED.  Unexpected error " << e.what() << std::endl;
    }

    // getHostName() test
    testcase = "SATC2";
    std::cout << testcase << " starting" << std::endl;
    try {
        CxxSockets::SockAddrList salist(AF_UNSPEC, ipas, "30002");
        std::vector<string> values;
        for(CxxSockets::SockAddrList::iterator it = salist.begin();
            it != salist.end(); ++it) {
            std::string name = (*it).getHostName();
            if(name != "")
                values.push_back(name);
        }
        if(values.size() == ipas.size()) {
            std::cout << testcase << " SUCCESS" << std::endl;
            ++completed;
        }
        else {
            std::cout << testcase << " FAILED" << std::endl;
            ++failed;
        }
    } catch(CxxSockets::CxxError e) {
        ++failed;
        std::cout << testcase << " FAILED.  Unexpected error " << e.what() << std::endl;
    }

    // getServiceName() test
    testcase = "SATC3";
    std::cout << testcase << " starting" << std::endl;
    try {
        CxxSockets::SockAddr sa(AF_INET, "localhost", "23");
        std::string name = sa.getServiceName();
        if(name == "telnet") {
            std::cout << testcase << " SUCCESS" << std::endl;
            ++completed;
        }
        else {
            std::cout << testcase << " FAILED" << std::endl;
            ++failed;
        }
    } catch(CxxSockets::CxxError e) {
        ++failed;
        std::cout << testcase << " FAILED.  Unexpected error " << e.what() << std::endl;
    }

    // getServicePort() test
    testcase = "SATC4";
    std::cout << testcase << " starting" << std::endl;
    try {
        CxxSockets::SockAddr sa(AF_INET, "localhost", "telnet");
        int port = sa.getServicePort();
        if(port == 23) {
            std::cout << testcase << " SUCCESS" << std::endl;
            ++completed;
        }
        else {
            std::cout << testcase << " FAILED" << std::endl;
            ++failed;
        }
    } catch(CxxSockets::CxxError e) {
        ++failed;
        std::cout << testcase << " FAILED.  Unexpected error " << e.what() << std::endl;
    }

    // getSockName() test
    testcase = "SATC5";
    std::cout << testcase << " starting" << std::endl;
    try {
        // Create a new socket
        CxxSockets::SockAddr sa(AF_UNSPEC, ipas[1], "30002");
        CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(sa.family(), 0));
        socket->Bind(sa);

        // Now send an empty sockaddr to fill out and get the name
        CxxSockets::SockAddr ta;
        socket->getSockName(ta);
        
        if(ta.getServicePort() == boost::lexical_cast<int>("30002") &&
           (ta.getHostAddr() == ipas[1])) {
            std::cout << testcase << " SUCCESS" << std::endl;
            ++completed;
        } else {
            std::cout << testcase << " FAILED." << std::endl;
            ++failed;
        }
    } catch(CxxSockets::CxxError e) {
        std::cout << testcase << " FAILED.  Unexpected error " << e.what() << std::endl;
        ++failed;
    }

    // getPeerName() test
    testcase = "SATC6";
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        // Create a new socket
        CxxSockets::SockAddr sa(AF_INET, "localhost", "30002");
        CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(sa.family(), 0));
        socket->Connect(sa);

        // Now send an empty sockaddr to fill out and get the name
        CxxSockets::SockAddr ta;
        socket->getPeerName(ta);
        
        if(ta.getServicePort() == sa.getServicePort() &&
           ta.getHostAddr() == sa.getHostAddr()) {
            std::cout << testcase << " SUCCESS" << std::endl;
            ++completed;
        } else {
            std::cout << testcase << " FAILED." << std::endl;
            ++failed;
        }
    } catch(CxxSockets::CxxError e) {
        std::cout << testcase << " FAILED.  Unexpected error " << e.what() << std::endl;
        ++failed;
    }

    PBDONE();

    testcase = "PTC1";
    // Connect to the listener several times and send several messages.
    // All should be received on the correct connections.
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr remote(AF_INET, "localhost", "30002");
        for(int i = 0; i < 5 ; ++i) {
            CxxSockets::Message msg;
            CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
            socket->Connect(remote);
            msg << "BRIEF" << i;
            socket->Send(msg);
        }
        PBRESULT();
        std::cout << testcase << ": SUCCESS" << std::endl;
    } catch (CxxSockets::CxxError e) {
        std::cout << testcase << " FAILED: Socket error detected: " << e.what() << std::endl;
        ++failed;
    }
    PBDONE();


    testcase = "PTC2";
    // Connect to the listener several times and send several messages.
    // All should be received on the correct connections.
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr remote(AF_INET, "localhost", "30002");
        for(int i = 0; i < 5 ; ++i) {
            CxxSockets::Message msg;
            CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
            socket->Connect(remote);
            msg << "BRIEF" << i;
            socket->Send(msg);
        }
        PBRESULT();
        std::cout << testcase << ": SUCCESS" << std::endl;
    } catch (CxxSockets::CxxError e) {
        std::cout << testcase << " FAILED: Socket error detected: " << e.what() << std::endl;
        ++failed;
    }
    PBDONE();

    testcase = "ASTC1";
    std::cout << testcase << " starting" << std::endl;
    try {
        std::vector<CxxSockets::SocketPtr> socks;
        for(int i = 0; i < 5 ; ++i) {
            CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(AF_INET, 0));
            socks.push_back(socket);
        }
        CxxSockets::SocketSet ss;
        ss.AddSocks(socks);
        if(ss.size() == 5) {
            std::cout << testcase << ": SUCCESS" << std::endl;
            ++completed;
        } else {
            std::cout << testcase << " FAILED: Could not add all sockets. " << std::endl;
            ++failed;
        }
    } catch (CxxSockets::CxxError e) {
        std::cout << testcase << " FAILED: Socket error detected: " << e.what() << std::endl;
        ++failed;
    }

    testcase = "PTC3";
    // Connect to the listener several times and send several messages.
    // All should be received on the correct connections.
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr remote(AF_INET, "localhost", "30002");
        for(int i = 0; i < 5 ; ++i) {
            CxxSockets::Message msg;
            CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
            socket->Connect(remote);
            msg << "BRIEF" << i;
            socket->Send(msg);
        }
        PBRESULT();
        std::cout << testcase << ": SUCCESS" << std::endl;
    } catch (CxxSockets::CxxError e) {
        std::cout << testcase << " FAILED: Socket error detected: " << e.what() << std::endl;
        ++failed;
    }
    PBDONE();


    testcase = "PTC4";
    // Connect to the listener several times and send several messages.
    // All should be received on the correct connections.
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr remote(AF_INET, "localhost", "30002");
        for(int i = 0; i < 5 ; ++i) {
            CxxSockets::Message msg;
            CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
            socket->Connect(remote);
            msg << "BRIEF" << i;
            socket->Send(msg);
        }
        PBRESULT();
        std::cout << testcase << ": SUCCESS" << std::endl;
    } catch (CxxSockets::CxxError e) {
        std::cout << testcase << " FAILED: Socket error detected: " << e.what() << std::endl;
        ++failed;
    }
    PBDONE();

    testcase = "PTC5";
    // Connect to the listener several times and send several messages.
    // All should be received on the correct connections.
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr remote(AF_INET, "localhost", "30002");
        for(int i = 0; i < 5 ; ++i) {
            CxxSockets::Message msg;
            CxxSockets::TCPSocketPtr socket(new CxxSockets::TCPSocket(remote.family(), 0));
            socket->Connect(remote);
            msg << "BRIEF" << i;
            socket->Send(msg);
        }
        PBRESULT();
        std::cout << testcase << ": SUCCESS" << std::endl;
    } catch (CxxSockets::CxxError e) {
        std::cout << testcase << " FAILED: Socket error detected: " << e.what() << std::endl;
        ++failed;
    }
    PBDONE();

    if(securetest == true) {

        // We're going to do secure test cases
        testcase = "SSTC1";
        PBSTART();
        std::cout << testcase << " starting" << std::endl;
        try {
            // Create remote socket addr.  Specifying zero means either.
            CxxSockets::SockAddr remote(AF_INET, "localhost", "30004");
            CxxSockets::SecureTCPSocketPtr sock(new CxxSockets::SecureTCPSocket(remote.family(), 0, CxxSockets::SECURE, CxxSockets::CERTIFICATE));
            // Connect to the server.
            bgq::utility::ClientPortConfiguration port_config(0);
            port_config.setProperties(props, "");
            port_config.notifyComplete();
            sock->Connect(remote, port_config);

            // Send a message
            CxxSockets::Message msg("Hello world!");
            sock->Send(msg);
            PBRESULT();
            std::cout << testcase << ": SUCCESS" << std::endl;
        } catch (CxxSockets::CxxError e) {
            std::cout << testcase << " FAILED: Error detected " << e.what() << std::endl;
            ++failed;
        }
        PBDONE();

        // We're going to do secure test cases
        testcase = "SSTC2";
        PBSTART();
        std::cout << testcase << " starting" << std::endl;
        try {
            // Create remote socket addr.  Specifying zero means either.
            CxxSockets::SockAddr remote(AF_INET, "localhost", "30004");
            int fd = ::socket(AF_INET, SOCK_STREAM, 0);
            CxxSockets::SockAddr local(AF_INET, "", "");
            int rc = ::bind(fd, (sockaddr*)(&local), sizeof(sockaddr_in));
            if(rc < 0) {
                perror("SSTC2 failed to bind");
                exit(EXIT_FAILURE);
            }

            rc = ::connect(fd, (sockaddr*)(&remote), sizeof(sockaddr_in));
            if(rc < 0) {
                perror("SSTC2 failed to connect");
                exit(EXIT_FAILURE);
            }

            CxxSockets::SecureTCPSocketPtr sock(new CxxSockets::SecureTCPSocket(remote.family(), fd, CxxSockets::SECURE, CxxSockets::CERTIFICATE));
            // Connect to the server.
            bgq::utility::ClientPortConfiguration port_config(0);
            port_config.setProperties(props, "");
            port_config.notifyComplete();
            sock->MakeSecure(port_config);

            // Send a message
            CxxSockets::Message msg("Hello world!");
            sock->Send(msg);
            ::close(fd);
            PBRESULT();
            std::cout << testcase << ": SUCCESS" << std::endl;
        } catch (CxxSockets::CxxError e) {
            std::cout << testcase << " FAILED: Error detected " << e.what() << std::endl;
            ++failed;
        }
        PBDONE();

        if(usertest) {
            testcase = "SSTC3";
            PBSTART();
            std::cout << testcase << " starting" << std::endl;
            try {
                // Create remote socket addr.  Specifying zero means either.
                CxxSockets::SockAddr remote(AF_INET, "localhost", "30004");
                CxxSockets::SecureTCPSocketPtr 
                    sock(new CxxSockets::SecureTCPSocket(remote.family(), 0, CxxSockets::SECURE, CxxSockets::CRYPTUID));

                // Connect to the server.
                bgq::utility::ClientPortConfiguration port_config(0);
                port_config.setProperties(props, "");
                port_config.notifyComplete();
                sock->Connect(remote, port_config);

                // Send a message
                bgq::utility::UserId myuid;
                CxxSockets::Message msg(myuid.getUser());
                sock->Send(msg);
                PBRESULT();
                std::cout << testcase << ": SUCCESS" << std::endl;
            } catch (CxxSockets::CxxError e) {
                std::cout << testcase << " FAILED: Error detected " << e.what() << std::endl;
                ++failed;
            }
            PBDONE();
        }

        // We're going to do secure test cases
        testcase = "SSTC4";
        PBSTART();
        std::cout << testcase << " starting" << std::endl;
        try {
            // Create remote socket addr.  Specifying zero means either.
            CxxSockets::SockAddr remote(AF_INET, "localhost", "30004");
            CxxSockets::SecureTCPSocketPtr 
                sock(new CxxSockets::SecureTCPSocket(remote.family(), 0, CxxSockets::SECURE, CxxSockets::CRYPTALL));
            // Connect to the server.
            bgq::utility::ClientPortConfiguration port_config(0);
            port_config.setProperties(props, "");
            port_config.notifyComplete();
            sock->Connect(remote, port_config);

            // Send a message
            CxxSockets::Message msg("Hello world!");
            sock->Send(msg);
            PBRESULT();
            std::cout << testcase << ": SUCCESS" << std::endl;
        } catch (CxxSockets::CxxError e) {
            std::cout << testcase << " FAILED: Error detected " << e.what() << std::endl;
            ++failed;
        }
        PBDONE();

        // Backwards compatability test with old certificate-only sockets
        testcase = "SSTC5";
        PBSTART();
        std::cout << testcase << " starting" << std::endl;
        try {
            // Create remote socket addr.  Specifying zero means either.
            CxxSockets::SockAddr remote(AF_INET, "localhost", "30004");
            CxxSockets::TCPSocketPtr sock(new CxxSockets::TCPSocket(remote.family(), 0, CxxSockets::SECURE));
            // Connect to the server.
            sock->Connect(remote);

            // Send a message
            CxxSockets::Message msg("Hello world!");
            sock->Send(msg);
            PBRESULT();
            std::cout << testcase << ": SUCCESS" << std::endl;
        } catch (CxxSockets::CxxError e) {
            std::cout << testcase << " FAILED: Error detected " << e.what() << std::endl;
            ++failed;
        }
        PBDONE();

        testcase = "SSTC6";

        // Listener on AF_UNSPEC.  Should connect to that
        std::cout << testcase << " starting" << std::endl;
        PBSTART();
        try {
            CxxSockets::SockAddr remote(AF_INET6, "::1", "30005");
            CxxSockets::SecureTCPSocketPtr socket(new CxxSockets::SecureTCPSocket(remote.family(), 0));
            bgq::utility::ClientPortConfiguration port_config(0);
            port_config.setProperties(props, "");
            port_config.notifyComplete();
            socket->Connect(remote, port_config);
            std::cout << testcase << ": SUCCESS" << std::endl;
        } catch (CxxSockets::SockHardError e) {
            std::cout << testcase << " FAILED: Socket error detected: " << e.what() << std::endl;
            ++failed;
        }

        PBDONE();

    }

    // File I/O test cases
    testcase = "FTC1";
    std::cout << testcase << " starting" << std::endl;
    try {
        // Construct a unique "temp" file
        CxxSockets::FilePtr myfile(new CxxSockets::File("/tmp/test_client_FTC1_XXXXXX", true));
        CxxSockets::Message msg;
        msg << "This is my test file output";
        myfile->Write(msg);
	myfile->Lseek(0, L_SET);
	CxxSockets::Message readmsg;
	myfile->Read(readmsg, 100);
        if(readmsg.str() != msg.str()) {
            std::cout << testcase << " FAILED: File incorrect. Written msg: " << msg.str() << " Read msg: " << readmsg.str() << std::endl;
            ++failed;
        }
        std::cout << testcase << ": SUCCESS" << std::endl;
        //	myfile->Delete();
    } catch (CxxSockets::CxxError e) {
        std::cout << testcase << " FAILED: File IO error detected: " << e.what() << std::endl;
        ++failed;
    }
    ++completed;

    // Mixed file set test
    testcase = "FSTC2";
    std::cout << testcase << " starting" << std::endl;
    try {
        CxxSockets::PollingFileSetPtr fs(new CxxSockets::PollingFileSet);
        CxxSockets::TCPSocketPtr sock(new CxxSockets::TCPSocket);
        CxxSockets::FilePtr myfile(new CxxSockets::File("/tmp/test_client_FSTC2_XXXXXX", true));
        fs->push_back(std::tr1::dynamic_pointer_cast<CxxSockets::File>(sock));
        fs->push_back(myfile);
        myfile->Delete();
        std::cout << testcase << ": SUCCESS" << std::endl;
    } catch (CxxSockets::CxxError e) {
        std::cout << testcase << " FAILED: File IO error detected: " << e.what() << std::endl;
        ++failed;
    } catch (...) {
        std::cout << "caught mystery exception" << std::endl;
    }
    ++completed;

    // Conclusion:
    std::cout << std::endl << completed << " test(s) completed.  " << failed << " case(s) failed." << std::endl;
}
