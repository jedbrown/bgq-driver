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
#include "SocketTypes.h"  // The single include necessary 
#include "Log2.h"

int main(unsigned int argc, const char** argv) {

    bgq::utility::Properties::Ptr props;

    for (unsigned int i = 1; i < argc; ++i) {
        if (!strcasecmp(argv[i], "--properties")) {
            if (argc == ++i) {
                std::cerr << "please give a file name after " << argv[i-1] << std::endl;
                exit(EXIT_FAILURE);
            }
            props = bgq::utility::Properties::create(argv[i]);
        }
    }

    // Properties should be initialized if we are using security.
    if (!props) {
        props = bgq::utility::Properties::create();
    }

    // Initialize global properties.
    CxxSockets::setProperties(props);
    bgq::utility::initializeLogging(*props);

    // Create a socket address list.  Family of "0" means I don't care what I get.
    CxxSockets::SockAddrList salist(AF_UNSPEC, "", "30004");
    
    // Make a set of listeners (which could contain only one) based on the socket addr list.
    CxxSockets::ListenerSetPtr listener(new CxxSockets::ListenerSet(salist, SOMAXCONN, CxxSockets::SECURE));
    
    // Create a new socket so that we can do an accept.
    CxxSockets::TCPSocketPtr sock(new CxxSockets::SecureTCPSocket(CxxSockets::SECURE));
    
    // Accept it.
    CxxSockets::Message msg;
    try {
        listener->AcceptNew(sock);
        CxxSockets::SocketPtr p = sock;
//        std::cout << "remote uid=" << sock->getUserData()->getUid() << std::endl;
        // Now wait for a message
        sock->Receive(msg);
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << msg.str() << std::endl;
    sleep(100);
}
