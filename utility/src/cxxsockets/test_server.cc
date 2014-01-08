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
#include <fstream>
#include <iostream>
#include <string>
#include "SocketTypes.h"
#include "servermacros.h"
#include "Log.h"

using namespace std;

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
        props = bgq::utility::Properties::create();
    }

    // Initialize global properties.
    CxxSockets::setProperties(props);
    bgq::utility::initializeLogging(*props);
    
    // Side protocol connection first:
    CxxSockets::SockAddrList side_salist(AF_INET, "", "34543");
    CxxSockets::ListenerSetPtr side_listener(new CxxSockets::ListenerSet(side_salist));
    CxxSockets::TCPSocketPtr side_sock(new CxxSockets::TCPSocket);
    side_listener->AcceptNew(side_sock);
    {
        CxxSockets::Message side_msg("READY");
        side_sock->Send(side_msg);
        side_msg.str().clear();
    }
    std::string testcase;

    testcase = "LTC1";
    // Listener test
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        // Listen on v4 localhost
        CxxSockets::SockAddr local(AF_INET, "localhost", "30002");
        CxxSockets::ListeningSocketPtr listener(new CxxSockets::ListeningSocket(local));
        PBREADY();
        PBDONE();
    } catch(CxxSockets::SockHardError e) {
        std::cout << testcase << ": Listen only on v4" << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    testcase = "LTC2";
    // Listener test
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        // Listen on v4 localhost
        CxxSockets::SockAddr local(AF_INET, "localhost", "30002");
        CxxSockets::ListeningSocketPtr listener(new CxxSockets::ListeningSocket(local));
        PBREADY();
        PBDONE();
    } catch(CxxSockets::SockHardError e) {
        std::cout << testcase << ": Listen only on v4" << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    testcase = "LTC3";
    // Listener test
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        // Socket LIST because we get two with AF_UNSPEC
        CxxSockets::SockAddrList local(AF_UNSPEC, "", "30002");
        CxxSockets::ListenerSetPtr listener(new CxxSockets::ListenerSet(local));
        PBREADY();
        PBDONE();
    } catch(CxxSockets::SockHardError e) {
        std::cout << testcase << ": Listen on v4 and v6" << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    testcase = "LTC4";
    // Listener test
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        // Socket LIST because we get two with AF_UNSPEC
        CxxSockets::SockAddrList local(AF_UNSPEC, "", "30002");
        CxxSockets::ListenerSetPtr listener(new CxxSockets::ListenerSet(local));
        PBREADY();
        PBDONE();
    } catch(CxxSockets::SockHardError e) {
        std::cout << testcase << ": Listen on v4 and v6" << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    testcase = "LTC5";
    // Listener test
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        // Socket LIST because we get two with AF_UNSPEC
        CxxSockets::SockAddrList local(AF_UNSPEC, "localhost", "30002");
        CxxSockets::ListenerSetPtr listener(new CxxSockets::ListenerSet(local));
        PBREADY();
        PBDONE();
    } catch(CxxSockets::SockHardError e) {
        std::cout << testcase << ": Listen on v4 and v6" << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    testcase = "LTC6";
    // Listener test
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        // Socket LIST because we get two with AF_UNSPEC
        CxxSockets::SockAddrList local(AF_UNSPEC, "localhost", "30002");
        CxxSockets::ListenerSetPtr listener(new CxxSockets::ListenerSet(local));
        PBREADY();
        PBDONE();
    } catch(CxxSockets::SockHardError e) {
        std::cout << testcase << ": Listen on v4 and v6" << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    testcase = "LTC7";
    // Listener test
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        // Listen on v6 localhost
        CxxSockets::SockAddr local(AF_INET6, "", "30002");
        CxxSockets::ListeningSocketPtr listener(new CxxSockets::ListeningSocket(local));
        PBREADY();
        PBDONE();
    } catch(CxxSockets::SockHardError e) {
        std::cout << testcase << ": Listen on v6" << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    testcase = "LTC8";
    // Listener test
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        // Listen on v6 localhost
        CxxSockets::SockAddr local(AF_INET6, "", "30002");
        CxxSockets::ListeningSocketPtr listener(new CxxSockets::ListeningSocket(local));
        PBREADY();
        PBDONE();
    } catch(CxxSockets::SockHardError e) {
        std::cout << testcase << ": Listen on v6" << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    testcase = "LTC9";
    // Listener test
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddrList local(AF_INET6, "localhost", "30002");
        CxxSockets::ListenerSetPtr listener(new CxxSockets::ListenerSet(local));
        PBREADY();
        PBDONE();
    } catch(CxxSockets::SockHardError e) {
        std::cout << testcase << ": Listen on v6 only" << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    testcase = "LTC10";
    // Listener test
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        // Socket LIST because we get two with AF_UNSPEC
        CxxSockets::SockAddrList local(AF_INET6, "localhost", "30002");
        CxxSockets::ListenerSetPtr listener(new CxxSockets::ListenerSet(local));
        PBREADY();
        PBDONE();
    } catch(CxxSockets::SockHardError e) {
        std::cout << testcase << ": Listen on v6 only" << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    // Polling listener/receive test v6 (to get both)
    testcase = "LTC11";
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        // Get multiple sockaddrs so we can do both v4 and v6
        CxxSockets::SockAddrList salist(AF_INET6, "", "30002");
        CxxSockets::PollingListenerSetPtr listener(new CxxSockets::PollingListenerSet(salist));
        CxxSockets::TCPSocketPtr sock(new CxxSockets::TCPSocket);
        PBREADY();
        // Non-blocking polling accept
        bool accepted = false;
        int count = 0;
        while(!accepted && count < 6) {
            std::cout << "." << std::flush;
            accepted = listener->AcceptNew(sock);
            usleep(500);
            ++count;
        }
        PBDONE();
    } catch(CxxSockets::SockHardError e) {
        std::cout << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    // Open multiple specified listeners (must be performed on system with more than one IP address)
    testcase = "LTC12";
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    PBREADY();
    try {
        // Get multiple sockaddrs so we can do both v4 and v6
        std::vector<std::string> ips;
        std::string msg;
        while(true) {
            PBWAITSTRING(msg);
            if(msg != "FINI") {
                ips.push_back(msg);
            } else break;
        } 

        CxxSockets::SockAddrList salist(AF_UNSPEC, ips, "30002");
        CxxSockets::PollingListenerSetPtr listener(new CxxSockets::PollingListenerSet(salist));
        PBREADY();
        PBDONE();
    } catch(CxxSockets::SockHardError e) {
        std::cout << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    // Open multiple specified listeners (must be performed on system with more than one IP address)
    testcase = "LTC13";
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    PBREADY();
    try {
        // Get multiple sockaddrs so we can do both v4 and v6
        std::vector<std::string> ips;
        std::string msg;
        while(true) {
            PBWAITSTRING(msg);
            if(msg != "FINI") {
                ips.push_back(msg);
            } else break;
        } 

        CxxSockets::SockAddrList salist(0, ips, "30002");
        CxxSockets::PollingListenerSetPtr listener(new CxxSockets::PollingListenerSet(salist));
        PBREADY();
        PBDONE();
    } catch(CxxSockets::SockHardError e) {
        std::cout << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }


    // Polling listener/receive test v6 (to get both)
    testcase = "LTC14";
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        // Get multiple sockaddrs so we can do both v4 and v6
        CxxSockets::SockAddrList salist(AF_INET6, "", "30002");
        CxxSockets::EpollingListenerSetPtr listener(new CxxSockets::EpollingListenerSet(salist));
        CxxSockets::TCPSocketPtr sock(new CxxSockets::TCPSocket);
        PBREADY();
        // Non-blocking polling accept
        int accepted = false;
        int count = 0;
        while(!accepted && count < 6) {
            std::cout << "." << std::flush;
            accepted = listener->AcceptNew(sock);
            usleep(500);
            ++count;
        }
        PBDONE();
    } catch(CxxSockets::SockHardError e) {
        std::cout << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    // Open multiple specified listeners (must be performed on system with more than one IP address)
    testcase = "LTC15";
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    PBREADY();
    try {
        // Get multiple sockaddrs so we can do both v4 and v6
        std::vector<std::string> ips;
        std::string msg;
        while(true) {
            PBWAITSTRING(msg);
            if(msg != "FINI") {
                ips.push_back(msg);
            } else break;
        } 

        CxxSockets::SockAddrList salist(AF_UNSPEC, ips, "30002");
        CxxSockets::EpollingListenerSetPtr listener(new CxxSockets::EpollingListenerSet(salist));
        PBREADY();
        PBDONE();
    } catch(CxxSockets::SockHardError e) {
        std::cout << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    // Open multiple specified listeners (must be performed on system with more than one IP address)
    testcase = "LTC16";
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    PBREADY();
    try {
        // Get multiple sockaddrs so we can do both v4 and v6
        std::vector<std::string> ips;
        std::string msg;
        while(true) {
            PBWAITSTRING(msg);
            if(msg != "FINI") {
                ips.push_back(msg);
            } else break;
        } 

        CxxSockets::SockAddrList salist(0, ips, "30002");
        CxxSockets::EpollingListenerSetPtr listener(new CxxSockets::EpollingListenerSet(salist));
        PBREADY();
        PBDONE();
    } catch(CxxSockets::SockHardError e) {
        std::cout << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    testcase = "SRTC1";
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        // Testing a managed send
        CxxSockets::SockAddr local(AF_INET6, "localhost", "30022");
        CxxSockets::ListeningSocketPtr listener(new CxxSockets::ListeningSocket(local));
        CxxSockets::TCPSocketPtr sock(new CxxSockets::TCPSocket);
        PBREADY();
        listener->AcceptNew(sock);
        CxxSockets::Message msg;
        sock->Receive(msg);
        if(msg.str() == "Short Message") {
            PBSUCCESS();
        }
        else {
            PBFAIL();
        }
        PBDONE();
    } catch(CxxSockets::SockHardError e) {
        std::cout << testcase << ": Listen on v6" << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    testcase = "SRTC2";
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        // Testing a managed send
        CxxSockets::SockAddr local(AF_INET6, "localhost", "30002");
        CxxSockets::ListeningSocketPtr listener(new CxxSockets::ListeningSocket(local));
        CxxSockets::TCPSocketPtr sock(new CxxSockets::TCPSocket);
        PBREADY();
        listener->AcceptNew(sock);
        CxxSockets::Message msg;
        sock->Receive(msg);
        if(msg.str().length() == 10000000) {
            std::cout << "OK: msg size=" << msg.str().length() << std::endl;
            PBSUCCESS();
        }
        else {
            std::cout << "FAIL: msg size=" << msg.str().length() << std::endl;
            std::string fname = "/tmp/testout.srvr";
            std::fstream f(fname.c_str(), std::ios_base::out);
            f << msg.str();
            PBFAIL();
        }
        PBDONE();
    } catch(CxxSockets::SockHardError e) {
        std::cout << testcase << ": Listen on v6" << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    testcase = "SRTC3";
    std::cout << testcase << " starting" << std::endl;
    PBSTART();

    try {
        // Testing an unmanaged send
        CxxSockets::SockAddr local(AF_INET6, "localhost", "30002");
        CxxSockets::ListeningSocketPtr listener(new CxxSockets::ListeningSocket(local));
        CxxSockets::TCPSocketPtr sock(new CxxSockets::TCPSocket);
        PBREADY();
        listener->AcceptNew(sock);
        CxxSockets::Message msg;
        sock->ReceiveUnManaged(msg, 5);
        std::cout << "received message=" << msg.str() << std::endl;
        std::cout << "received message len=" << msg.str().length() << std::endl;
        if(msg.str() == "BRIEF") {
            std::cout << "OK" << std::endl;
            PBSUCCESS();
        }
        else {
            std::cout << "NOK" << std::endl;
            PBFAIL();
        }
        PBDONE();
    } catch(CxxSockets::CxxError e) {
        std::cout << testcase << ": Listen on v6" << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    // Polling receive
    testcase = "RTC1";
    std::cout << testcase << " starting" << std::endl;
    PBSTART();

    try {
        // Testing an unmanaged send
        CxxSockets::SockAddr local(AF_INET6, "localhost", "30002");
        CxxSockets::ListeningSocketPtr listener(new CxxSockets::ListeningSocket(local));
        PBREADY();
        // We're going to get five connections
        CxxSockets::PollingTCPSocketSetPtr pollset(new CxxSockets::PollingTCPSocketSet);
        for(int i = 0 ; i < 5 ; ++i) {
            CxxSockets::TCPSocketPtr sock(new CxxSockets::TCPSocket);
            listener->AcceptNew(sock);
            std::cout << "ACCEPTED A SOCKET" << std::endl;
            CxxSockets::SocketPtr p = std::tr1::static_pointer_cast<CxxSockets::Socket>(sock);
            pollset->AddSock(p);
        }
        std::cout << "WAITING FOR MESSAGES" << std::endl;
        // Now wait a bit for the other side to send to all of our connections
        usleep(500);
        // And do a polled receive and see if we get data on all of them.
        CxxSockets::MsgMap msgs;
        pollset->PolledReceive(msgs);
        if(msgs.size() != 5) {
            std::cout << "NOT ENOUGH MSGS!!" << std::endl;
            PBFAIL();
        } else {  // Go on to check the messages.
            // Each message should consist of "BRIEF" followed by an integer
            // one to five.
            int badones = 0;
            for(CxxSockets::MsgMap::iterator it = msgs.begin();
                it != msgs.end(); ++it) {
                if((*it).second.find("BRIEF") == std::string::npos &&
                   ((*it).second.find("0") == std::string::npos &&
                    (*it).second.find("1") == std::string::npos &&
                    (*it).second.find("2") == std::string::npos &&
                    (*it).second.find("3") == std::string::npos &&
                    (*it).second.find("4") == std::string::npos)) {
                    std::cout << "Bad message \"" << (*it).second << "\" found." 
                              << std::endl;
                    ++badones;
                }
            }

            if(badones == 0) {
                PBSUCCESS();
            } else {
                PBFAIL();
            }
        }
        PBDONE();
    } catch(CxxSockets::CxxError e) {
        std::cout << testcase << ": Polling receive error: " << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    // Polling receive
    testcase = "RTC2";
    std::cout << testcase << " starting" << std::endl;
    PBSTART();

    try {
        // Testing an unmanaged send
        CxxSockets::SockAddr local(AF_INET6, "localhost", "30002");
        CxxSockets::ListeningSocketPtr listener(new CxxSockets::ListeningSocket(local));
        PBREADY();
        // We're going to get five connections
        CxxSockets::EpollingTCPSocketSetPtr pollset(new CxxSockets::EpollingTCPSocketSet);
        for(int i = 0 ; i < 5 ; ++i) {
            CxxSockets::TCPSocketPtr sock(new CxxSockets::TCPSocket);
            listener->AcceptNew(sock);
            CxxSockets::SocketPtr p = std::tr1::static_pointer_cast<CxxSockets::Socket>(sock);
            pollset->AddSock(p);
        }
        // Now wait a bit for the other side to send to all of our connections
        usleep(500);
        // And do a polled receive and see if we get data on all of them.
        CxxSockets::MsgMap msgs;
        pollset->PolledReceive(msgs);
        if(msgs.size() != 5) {
            PBFAIL();
        } else {  // Go on to check the messages.
            // Each message should consist of "BRIEF" followed by an integer
            // one to five.
            int badones = 0;
            for(CxxSockets::MsgMap::iterator it = msgs.begin();
                it != msgs.end(); ++it) {
                if((*it).second.find("RTC2") == std::string::npos &&
                   ((*it).second.find("0") == std::string::npos &&
                    (*it).second.find("1") == std::string::npos &&
                    (*it).second.find("2") == std::string::npos &&
                    (*it).second.find("3") == std::string::npos &&
                    (*it).second.find("4") == std::string::npos)) {
                    std::cout << "Bad message \"" << (*it).second << "\" found." 
                              << std::endl;
                    ++badones;
                }
            }

            if(badones == 0) {
                PBSUCCESS();
            } else {
                PBFAIL();
            }
        }
        PBDONE();
    } catch(CxxSockets::CxxError e) {
        std::cout << testcase << ": Epolling receive error: " << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    testcase = "RTC3";
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr local(AF_INET6, "", "30002");
        CxxSockets::ListeningSocketPtr listener(new CxxSockets::ListeningSocket(local));
        CxxSockets::TCPSocketPtr sock(new CxxSockets::TCPSocket);
        PBREADY();
        listener->AcceptNew(sock);
        CxxSockets::Message msg;
        sock->Receive(msg);
        bool successful = false;
        if(msg.str() == "Short Message") {
            // Got the first one, now see what happens if we
            // shut down receive side and try again.
            CxxSockets::Message msg;
            try {
                sock->Shutdown(CxxSockets::Socket::RECEIVE);
                sock->Receive(msg);
            } catch (CxxSockets::SockSoftError e) {
                string error(e.what());
                if(error.find("side closed")) {
                    PBSUCCESS();
                    successful = true;
                } else successful = false;
            }
        }
        if(!successful)
            PBFAIL();
        PBDONE();
    } catch(CxxSockets::SockHardError e) {
        std::cout << testcase << ": Listen on v6" << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }
        
    testcase = "RTC4";
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr local(AF_INET6, "", "30002");
        CxxSockets::ListeningSocketPtr listener(new CxxSockets::ListeningSocket(local));
        CxxSockets::TCPSocketPtr sock(new CxxSockets::TCPSocket);
        PBREADY();
        listener->AcceptNew(sock);
        CxxSockets::Message msg;
        sock->Receive(msg);
        bool successful = true;
        if(msg.str() == "Short Message") {
            // Got the first one, now see what happens if we
            // shut down receive side and try again.
            CxxSockets::Message msg("Short Reply");
            try {
                sock->Shutdown(CxxSockets::Socket::RECEIVE);
                // Make sure we can still send when receive is down.
                sock->Send(msg);
            } catch (CxxSockets::SockSoftError e) {
                string error(e.what());
                PBFAIL();
                successful = false;
            }
        }
        if(successful)
            PBSUCCESS();
        PBDONE();
    } catch(CxxSockets::SockHardError e) {
        std::cout << testcase << ": Listen on v6" << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    testcase = "STC1";
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr local(AF_INET6, "localhost", "30002");
        CxxSockets::ListeningSocketPtr listener(new CxxSockets::ListeningSocket(local));
        CxxSockets::TCPSocketPtr sock(new CxxSockets::TCPSocket);
        PBREADY();
        listener->AcceptNew(sock);
        CxxSockets::Message msg;
        sock->Receive(msg);
        bool successful = true;
        if(msg.str() == "First Message") {
            // Got the first one, now see what happens if we
            // shut down receive side and try again.
            CxxSockets::Message msg("First Reply");
            try {
                sock->Send(msg);
            } catch (CxxSockets::SockSoftError e) {
                string error(e.what());
                std::cout << testcase << " Abnormal failure!! " << error << std::endl;
                PBFAIL();
                successful = false;
            }
        } else {  // Bogus message
            successful = false;
            std::cout << testcase << " Abnormal failure!!  Bogus message!" << std::endl;
            PBFAIL();
        }
        if(successful) {
            PBSUCCESS();
        }
        PBDONE();
    } catch(CxxSockets::SockHardError e) {
        std::cout << testcase << ": Listen on v6" << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    testcase = "STC2";
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        CxxSockets::SockAddr local(AF_INET6, "localhost", "30002");
        CxxSockets::ListeningSocketPtr listener(new CxxSockets::ListeningSocket(local));
        CxxSockets::TCPSocketPtr sock(new CxxSockets::TCPSocket);
        PBREADY();
        listener->AcceptNew(sock);
        CxxSockets::Message msg;
        sock->Receive(msg);
        bool successful = true;
        if(msg.str() == "First Message") {
            // Got the first one.  Send a reply
            CxxSockets::Message msg("First Reply");
            try {
                sock->Send(msg);
            } catch (CxxSockets::SockSoftError e) {
                string error(e.what());
                std::cout << testcase << " Abnormal failure!! " << error << std::endl;
                PBFAIL();
                successful = false;
            }

            // Try another one.
            CxxSockets::Message msg2("Second Reply");
            try {
                sock->Send(msg2);
            } catch (CxxSockets::SockSoftError e) {
                string error(e.what());
                std::cout << testcase << " Abnormal failure!! " << error << std::endl;
                PBFAIL();
                successful = false;
            }

        } else {  // Bogus message
            successful = false;
            std::cout << testcase << " Abnormal failure!!  Bogus message! \"" << msg.str() << "\"" << std::endl;
            PBFAIL();
        }
        if(successful) {
            PBSUCCESS();
        }
        PBDONE();
    } catch(CxxSockets::SockHardError e) {
        std::cout << testcase << ": Listen on v6" << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    testcase = "SATC6";
    // Just listen to allow a connection.
    std::cout << testcase << " starting" << std::endl;
    PBSTART();
    try {
        // Listen on v4 localhost
        CxxSockets::SockAddr local(AF_INET, "localhost", "30002");
        CxxSockets::ListeningSocketPtr listener(new CxxSockets::ListeningSocket(local));
        std::cout << "listening" << std::endl;
        PBREADY();
        PBDONE();
    } catch(CxxSockets::SockHardError e) {
        std::cout << testcase << ": Listen only on v4" << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }


    testcase = "PTC1";
    std::cout << testcase << " starting" << std::endl;
    PBSTART();

    try {
        CxxSockets::SockAddr local(AF_INET, "localhost", "30002");
        CxxSockets::ListeningSocketPtr listener(new CxxSockets::ListeningSocket(local));
        PBREADY();
        // We're going to get five connections
        CxxSockets::EpollingTCPSocketSetPtr pollset(new CxxSockets::EpollingTCPSocketSet);
        std::vector<CxxSockets::TCPSocketPtr> sockvec;
        for(int i = 0 ; i < 5 ; ++i) {
            CxxSockets::TCPSocketPtr sock(new CxxSockets::TCPSocket);
            listener->AcceptNew(sock);
            CxxSockets::SocketPtr p = std::tr1::static_pointer_cast<CxxSockets::Socket>(sock);
            pollset->AddSock(p);
            sockvec.push_back(sock);
        }
        // Now wait a bit for the other side to send to all of our connections
        usleep(500);
        // And do a polled receive and see if we get data on all of them.
        CxxSockets::MsgMap msgs;
        pollset->ProtectedPoll();
        // Now go get all of the messages
        for(std::vector<CxxSockets::TCPSocketPtr>::iterator it = sockvec.begin();
            it != sockvec.end(); ++it) {
            CxxSockets::Message message;
            (*it)->Receive(message);
            std::pair<CxxSockets::TCPSocketPtr, std::string> element((*it),message.str());
            msgs.insert(element);
        }
        if(msgs.size() != 5) {
            PBFAIL();
        } else {  // Go on to check the messages.
            // Each message should consist of "BRIEF" followed by an integer
            // one to five.
            int badones = 0;
            for(CxxSockets::MsgMap::iterator it = msgs.begin();
                it != msgs.end(); ++it) {
                if((*it).second.find("BRIEF") == std::string::npos &&
                   ((*it).second.find("0") == std::string::npos &&
                    (*it).second.find("1") == std::string::npos &&
                    (*it).second.find("2") == std::string::npos &&
                    (*it).second.find("3") == std::string::npos &&
                    (*it).second.find("4") == std::string::npos)) {
                    std::cout << "Bad message \"" << (*it).second << "\" found." 
                              << std::endl;
                    ++badones;
                }
            }

            if(badones == 0) {
                PBSUCCESS();
            } else {
                PBFAIL();
            }
        }
        PBDONE();
    } catch(CxxSockets::CxxError e) {
        std::cout << testcase << ": Polling receive error: " << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    testcase = "PTC2";
    std::cout << testcase << " starting" << std::endl;
    PBSTART();

    try {
        CxxSockets::SockAddr local(AF_INET, "localhost", "30002");
        CxxSockets::ListeningSocketPtr listener(new CxxSockets::ListeningSocket(local));
        PBREADY();
        // We're going to get five connections
        CxxSockets::PollingTCPSocketSetPtr pollset(new CxxSockets::PollingTCPSocketSet);
        std::vector<CxxSockets::TCPSocketPtr> sockvec;
        for(int i = 0 ; i < 5 ; ++i) {
            CxxSockets::TCPSocketPtr sock(new CxxSockets::TCPSocket);
            listener->AcceptNew(sock);
            CxxSockets::SocketPtr p = std::tr1::static_pointer_cast<CxxSockets::Socket>(sock);
            pollset->AddSock(p);
            sockvec.push_back(sock);
        }
        // Now wait a bit for the other side to send to all of our connections
        usleep(500);
        // And do a polled receive and see if we get data on all of them.
        CxxSockets::MsgMap msgs;
        pollset->ProtectedPoll();
        // Now go get all of the messages
        for(std::vector<CxxSockets::TCPSocketPtr>::iterator it = sockvec.begin();
            it != sockvec.end(); ++it) {
            CxxSockets::Message message;
            (*it)->Receive(message);
            std::pair<CxxSockets::TCPSocketPtr, std::string> element((*it),message.str());
            msgs.insert(element);
        }
        if(msgs.size() != 5) {
            PBFAIL();
        } else {  // Go on to check the messages.
            // Each message should consist of "BRIEF" followed by an integer
            // one to five.
            int badones = 0;
            for(CxxSockets::MsgMap::iterator it = msgs.begin();
                it != msgs.end(); ++it) {
                if((*it).second.find("BRIEF") == std::string::npos &&
                   ((*it).second.find("0") == std::string::npos &&
                    (*it).second.find("1") == std::string::npos &&
                    (*it).second.find("2") == std::string::npos &&
                    (*it).second.find("3") == std::string::npos &&
                    (*it).second.find("4") == std::string::npos)) {
                    std::cout << "Bad message \"" << (*it).second << "\" found." 
                              << std::endl;
                    ++badones;
                }
            }

            if(badones == 0) {
                PBSUCCESS();
            } else {
                PBFAIL();
            }
        }
        PBDONE();
    } catch(CxxSockets::CxxError e) {
        std::cout << testcase << ": Polling receive error: " << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    testcase = "PTC3";
    std::cout << testcase << " starting" << std::endl;
    PBSTART();

    try {
        CxxSockets::SockAddr local(AF_INET, "localhost", "30002");
        CxxSockets::ListeningSocketPtr listener(new CxxSockets::ListeningSocket(local));
        PBREADY();
        // We're going to get five connections
        CxxSockets::EpollingTCPSocketSetPtr pollset(new CxxSockets::EpollingTCPSocketSet);
        std::vector<CxxSockets::TCPSocketPtr> sockvec;
        for(int i = 0 ; i < 5 ; ++i) {
            CxxSockets::TCPSocketPtr sock(new CxxSockets::TCPSocket);
            listener->AcceptNew(sock);
            CxxSockets::SocketPtr p = std::tr1::static_pointer_cast<CxxSockets::Socket>(sock);
            pollset->AddSock(p);
            sockvec.push_back(sock);
        }
        // Now wait a bit for the other side to send to all of our connections
        usleep(500);
        // And do a polled receive and see if we get data on all of them.
        CxxSockets::MsgMap msgs;
        pollset->UnprotectedPoll();
        // Now go get all of the messages
        for(std::vector<CxxSockets::TCPSocketPtr>::iterator it = sockvec.begin();
            it != sockvec.end(); ++it) {
            CxxSockets::Message message;
            (*it)->Receive(message);
            std::pair<CxxSockets::TCPSocketPtr, std::string> element((*it),message.str());
            msgs.insert(element);
        }
        if(msgs.size() != 5) {
            PBFAIL();
        } else {  // Go on to check the messages.
            // Each message should consist of "BRIEF" followed by an integer
            // one to five.
            int badones = 0;
            for(CxxSockets::MsgMap::iterator it = msgs.begin();
                it != msgs.end(); ++it) {
                if((*it).second.find("BRIEF") == std::string::npos &&
                   ((*it).second.find("0") == std::string::npos &&
                    (*it).second.find("1") == std::string::npos &&
                    (*it).second.find("2") == std::string::npos &&
                    (*it).second.find("3") == std::string::npos &&
                    (*it).second.find("4") == std::string::npos)) {
                    std::cout << "Bad message \"" << (*it).second << "\" found." 
                              << std::endl;
                    ++badones;
                }
            }

            if(badones == 0) {
                PBSUCCESS();
            } else {
                PBFAIL();
            }
        }
        PBDONE();
    } catch(CxxSockets::CxxError e) {
        std::cout << testcase << ": Polling receive error: " << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    testcase = "PTC4";
    std::cout << testcase << " starting" << std::endl;
    PBSTART();

    try {
        CxxSockets::SockAddr local(AF_INET, "localhost", "30002");
        CxxSockets::ListeningSocketPtr listener(new CxxSockets::ListeningSocket(local));
        PBREADY();
        // We're going to get five connections
        CxxSockets::PollingTCPSocketSetPtr pollset(new CxxSockets::PollingTCPSocketSet);
        std::vector<CxxSockets::TCPSocketPtr> sockvec;
        for(int i = 0 ; i < 5 ; ++i) {
            CxxSockets::TCPSocketPtr sock(new CxxSockets::TCPSocket);
            listener->AcceptNew(sock);
            CxxSockets::SocketPtr p = std::tr1::static_pointer_cast<CxxSockets::Socket>(sock);
            pollset->AddSock(p);
            sockvec.push_back(sock);
        }
        // Now wait a bit for the other side to send to all of our connections
        usleep(500);
        // And do a polled receive and see if we get data on all of them.
        CxxSockets::MsgMap msgs;
        pollset->UnprotectedPoll();
        // Now go get all of the messages
        for(std::vector<CxxSockets::TCPSocketPtr>::iterator it = sockvec.begin();
            it != sockvec.end(); ++it) {
            CxxSockets::Message message;
            (*it)->Receive(message);
            std::pair<CxxSockets::TCPSocketPtr, std::string> element((*it),message.str());
            msgs.insert(element);
        }
        if(msgs.size() != 5) {
            PBFAIL();
        } else {  // Go on to check the messages.
            // Each message should consist of "BRIEF" followed by an integer
            // one to five.
            int badones = 0;
            for(CxxSockets::MsgMap::iterator it = msgs.begin();
                it != msgs.end(); ++it) {
                if((*it).second.find("BRIEF") == std::string::npos &&
                   ((*it).second.find("0") == std::string::npos &&
                    (*it).second.find("1") == std::string::npos &&
                    (*it).second.find("2") == std::string::npos &&
                    (*it).second.find("3") == std::string::npos &&
                    (*it).second.find("4") == std::string::npos)) {
                    std::cout << "Bad message \"" << (*it).second << "\" found." 
                              << std::endl;
                    ++badones;
                }
            }

            if(badones == 0) {
                PBSUCCESS();
            } else {
                PBFAIL();
            }
        }
        PBDONE();
    } catch(CxxSockets::CxxError e) {
        std::cout << testcase << ": Polling receive error: " << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    testcase = "PTC5";
    std::cout << testcase << " starting" << std::endl;
    PBSTART();

    try {
        CxxSockets::SockAddr local(AF_INET, "localhost", "30002");
        CxxSockets::ListeningSocketPtr listener(new CxxSockets::ListeningSocket(local));
        PBREADY();
        // We're going to get five connections
        CxxSockets::PollingTCPSocketSetPtr pollset(new CxxSockets::PollingTCPSocketSet);
        CxxSockets::FileSetPtr fs(new CxxSockets::FileSet);

        for(int i = 0 ; i < 5 ; ++i) {
            CxxSockets::TCPSocketPtr sock(new CxxSockets::TCPSocket);
            listener->AcceptNew(sock);
            CxxSockets::SocketPtr p = std::tr1::static_pointer_cast<CxxSockets::Socket>(sock);
            pollset->AddSock(p);
        }

        // Now wait a bit for the other side to send to all of our connections
        usleep(500);
        // And do a polled receive and see if we get data on all of them.
        CxxSockets::MsgMap msgs;
        pollset->ProtectedPoll(fs);
        // Now go get all of the messages
        for(CxxSockets::FileSet::iterator it = fs->begin();
            it != fs->end(); ++it) {
            CxxSockets::Message message;
            CxxSockets::TCPSocketPtr sockp = 
                std::tr1::static_pointer_cast<CxxSockets::TCPSocket>((*it));
            sockp->Receive(message);
            std::pair<CxxSockets::TCPSocketPtr, std::string> element(sockp,message.str());
            msgs.insert(element);
        }
        if(msgs.size() != 5) {
            PBFAIL();
        } else {  // Go on to check the messages.
            // Each message should consist of "BRIEF" followed by an integer
            // one to five.
            int badones = 0;
            for(CxxSockets::MsgMap::iterator it = msgs.begin();
                it != msgs.end(); ++it) {
                if((*it).second.find("BRIEF") == std::string::npos &&
                   ((*it).second.find("0") == std::string::npos &&
                    (*it).second.find("1") == std::string::npos &&
                    (*it).second.find("2") == std::string::npos &&
                    (*it).second.find("3") == std::string::npos &&
                    (*it).second.find("4") == std::string::npos)) {
                    std::cout << "Bad message \"" << (*it).second << "\" found." 
                              << std::endl;
                    ++badones;
                }
            }

            if(badones == 0) {
                PBSUCCESS();
            } else {
                PBFAIL();
            }
        }
        PBDONE();
    } catch(CxxSockets::CxxError e) {
        std::cout << testcase << ": Polling receive error: " << "Socket error detected: " << e.what() << std::endl;
        exit(1);
    }

    if(securetest == true) {

        // We're going to do secure test cases
        testcase = "SSTC1";
        std::cout << testcase << " starting" << std::endl;
        PBSTART();
        try {
            // Create a socket address list.  Family of "0" means I don't care what I get.
            CxxSockets::SockAddrList salist(AF_UNSPEC, "", "30004");
    
            // Make a set of listeners (which could contain only one) based on the socket addr list.
            CxxSockets::ListenerSetPtr listener(new CxxSockets::ListenerSet(salist, SOMAXCONN));

            PBREADY();    
            // Create a new socket so that we can do an accept.
            CxxSockets::SecureTCPSocketPtr sock(new CxxSockets::SecureTCPSocket(CxxSockets::SECURE, CxxSockets::CERTIFICATE));
    
            // Accept it.
            CxxSockets::Message msg;

            bgq::utility::ServerPortConfiguration port_config(0);
            port_config.setProperties(props, "");
            port_config.notifyComplete();
            listener->AcceptNew(sock, port_config);
            CxxSockets::SocketPtr p = sock;

            // Now wait for a message
            sock->Receive(msg);
            if(msg.str() != "Hello world!") {
                PBFAIL();
            }
            else {
                PBSUCCESS();
            }
            PBDONE();
        } catch (const std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }

        // We're going to do secure test cases
        testcase = "SSTC2";
        std::cout << testcase << " starting" << std::endl;
        PBSTART();
        try {
            // Create a socket address list.  Family of "0" means I don't care what I get.
            CxxSockets::SockAddrList salist(AF_INET, "", "30004");
    
            // Make a set of listeners (which could contain only one) based on the socket addr list.
            CxxSockets::ListenerSetPtr listener(new CxxSockets::ListenerSet(salist, SOMAXCONN));

            PBREADY();    
            // Create a new socket so that we can do an accept.
            CxxSockets::SecureTCPSocketPtr sock(new CxxSockets::SecureTCPSocket(CxxSockets::SECURE, CxxSockets::CERTIFICATE));
    
            // Accept it.
            CxxSockets::Message msg;

            bgq::utility::ServerPortConfiguration port_config(0);
            port_config.setProperties(props, "");
            port_config.notifyComplete();
            listener->AcceptNew(sock, port_config);
            CxxSockets::SocketPtr p = sock;

            // Now wait for a message
            sock->Receive(msg);
            if(msg.str() != "Hello world!") {
                PBFAIL();
            }
            else {
                std::cout << "received " << msg.str() << " successfully." << std::endl;
                PBSUCCESS();
            }
            PBDONE();
        } catch (const std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }

        if(usertest) {
            testcase = "SSTC3";
            std::cout << testcase << " starting" << std::endl;
            PBSTART();
            try {
                // Create a socket address list.  Family of "0" means I don't care what I get.
                CxxSockets::SockAddrList salist(AF_UNSPEC, "", "30004");
    
                // Make a set of listeners (which could contain only one) based on the socket addr list.
                CxxSockets::ListenerSetPtr listener(new CxxSockets::ListenerSet(salist, SOMAXCONN, CxxSockets::SECURE));

                PBREADY();    
                // Create a new socket so that we can do an accept.
                CxxSockets::SecureTCPSocketPtr sock(new CxxSockets::SecureTCPSocket(CxxSockets::SECURE, CxxSockets::CRYPTUID));
    
                // Accept it.
                CxxSockets::Message msg;

                bgq::utility::ServerPortConfiguration port_config(0);
                port_config.setProperties(props, "");
                port_config.notifyComplete();
                listener->AcceptNew(sock, port_config);
                CxxSockets::SocketPtr p = sock;
                std::string remote_uid = sock->getUserId().getUser();
                // Now wait for a message
                sock->Receive(msg);

                std::cout << "uid passed as object=" << remote_uid;
                std::cout << " uid passed as message= " << msg.str() << std::endl;
 
                // Compare expected uid with actual uid
                if(msg.str() != remote_uid) {
                    std::cerr << "uid " << remote_uid << " does not match " 
                              << msg.str() << std::endl;
                    PBFAIL();
                }
                else {
                    PBSUCCESS();
                }
                PBDONE();
            } catch (const std::runtime_error& e) {
                std::cerr << e.what() << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        // CRYPTALL
        testcase = "SSTC4";
        std::cout << testcase << " starting" << std::endl;
        PBSTART();
        try {
            // Create a socket address list.  Family of "0" means I don't care what I get.
            CxxSockets::SockAddrList salist(AF_UNSPEC, "", "30004");
    
            // Make a set of listeners (which could contain only one) based on the socket addr list.
            CxxSockets::ListenerSetPtr listener(new CxxSockets::ListenerSet(salist, SOMAXCONN, CxxSockets::SECURE));

            PBREADY();    
            // Create a new socket so that we can do an accept.
            CxxSockets::SecureTCPSocketPtr sock(new CxxSockets::SecureTCPSocket(CxxSockets::SECURE, CxxSockets::CRYPTALL));
    
            // Accept it.
            CxxSockets::Message msg;


            bgq::utility::ServerPortConfiguration port_config(0);
            port_config.setProperties(props, "");
            port_config.notifyComplete();
            listener->AcceptNew(sock, port_config);
            CxxSockets::SocketPtr p = sock;
            //      std::cout << "remote uid=" << sock->getUserData()->getUid() << std::endl;
            // Now wait for a message
            sock->Receive(msg);
            if(msg.str() != "Hello world!") {
                PBFAIL();
            }
            else {
                PBSUCCESS();
            }
            PBDONE();
        } catch (const std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }

        // Backwards compat with old certificate-only secure sockets
        testcase = "SSTC5";
        std::cout << testcase << " starting" << std::endl;
        PBSTART();
        try {
            // Create a socket address list.  Family of "0" means I don't care what I get.
            CxxSockets::SockAddrList salist(AF_UNSPEC, "", "30004");
    
            // Make a set of listeners (which could contain only one) based on the socket addr list.
            CxxSockets::ListenerSetPtr listener(new CxxSockets::ListenerSet(salist, SOMAXCONN, CxxSockets::SECURE));

            PBREADY();    
            // Create a new socket so that we can do an accept.
            CxxSockets::TCPSocketPtr 
                sock(new CxxSockets::TCPSocket(CxxSockets::SECURE));
    
            // Accept it.
            CxxSockets::Message msg;

            listener->AcceptNew(sock);
            CxxSockets::SocketPtr p = sock;
            //      std::cout << "remote uid=" << sock->getUserData()->getUid() << std::endl;
            // Now wait for a message
            sock->Receive(msg);
            if(msg.str() != "Hello world!") {
                PBFAIL();
            }
            else {
                PBSUCCESS();
            }
            PBDONE();
        } catch (const std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
        testcase = "SSTC6";
        // Listener test
        std::cout << testcase << " starting" << std::endl;
        PBSTART();
        try {
            // Socket LIST because we get two with AF_UNSPEC
            CxxSockets::SockAddrList local(AF_UNSPEC, "", "30005");
            CxxSockets::ListenerSetPtr listener(new CxxSockets::ListenerSet(local));
            CxxSockets::SecureTCPSocketPtr sock(new CxxSockets::SecureTCPSocket(CxxSockets::SECURE, CxxSockets::CRYPTALL));
            PBREADY();
            
            bgq::utility::ServerPortConfiguration port_config(0);
            port_config.setProperties(props, "");
            port_config.notifyComplete();
            listener->AcceptNew(sock, port_config);

            PBDONE();
        } catch(CxxSockets::SockHardError e) {
            std::cout << testcase << ": Listen on v4 and v6" << "Socket error detected: " << e.what() << std::endl;
            exit(1);
        }

    } // end secure tests
}
