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
#include <utility/include/Log.h>
#include "SocketTypes.h"

int main(int argc, const char** argv) {

    bgq::utility::Properties::Ptr props;

    for (int i = 1; i < argc; ++i) {
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

    std::string host;
    std::cout << "Enter remote host: ";
    std::cin >> host;
    // Create remote socket addr.  Specifying zero means either.
    CxxSockets::SockAddr remote(AF_INET, host, "30004");
    CxxSockets::SecureTCPSocketPtr sock(new CxxSockets::SecureTCPSocket(remote.family(), 0, CxxSockets::SECURE));
    // Connect to the server.
    bgq::utility::ClientPortConfiguration port_config(0);
    port_config.notifyComplete();
    sock->Connect(remote, port_config);

//     CxxSockets::SocketPtr p = sock;
//     CxxSockets::Authorizer auth(p);
//     auth.send();

    // Send a message
    CxxSockets::Message msg("Hello world!");
    sock->Send(msg);
}
