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
// Test class to make cxxsockets work with xml serializable objects
// This will need to:
// * create a 'client' and a 'server'
// * instantiate a TCP connection
// * instantiate a serializable object.
// * send a class name, a space, and the object
// * read the class name and the object on the receiving side.

// Client side code
#include "SocketTypes.h"
#include <xml/include/library/XML.h>

// Generated from StoreProtocolSpec.xml
#include "StoreProtocolSpec.h"

int main(unsigned int argc, const char** argv) {

    bgq::util::Properties::Ptr props;

    for (unsigned int i = 1; i < argc; ++i) {
        if (!strcasecmp(argv[i], "--properties")) {
            if (argc == ++i) {
                std::cerr << "please give a file name after " << argv[i-1] << std::endl;
                exit(EXIT_FAILURE);
            }
            props = bgq::util::Properties::create(argv[i]);
        }
    }

    // Properties should be initialized if we are using security.
    if (!props) {
        props = bgq::util::Properties::create();
    }

    // Initialize global properties.
    CxxSockets::setProperties(props);
    bgq::util::initializeLogging(*props);

    std::string host;
    std::cout << "Enter remote host: ";
    std::cin >> host;
    // Create remote socket addr.  Specifying zero means either.
    CxxSockets::SockAddr remote(AF_INET, host, "30004");
    CxxSockets::TCPSocketPtr sock(new CxxSockets::TCPSocket(remote.family(), 0, CxxSockets::INSECURE));
    // Connect to the server.
    sock->Connect(remote);
    // Now build and send the message.  (We need something new or old and simple);
    CxxSockets::Message requestMessage;

    // Build the request
    std::string tss = "my shopping list";
    StoreProtocolSpec::StoreListRequest tm(tss);
    tm._books.push_back("moby dick");
    tm._books.push_back("cryptonomicon");

    tm._songs.push_back("In-A-Gadda-Da-Vida");

    // Serialize it.
    tm.write(requestMessage);

    // Build a message to send the class name
    CxxSockets::Message classNameMessage(tm.getClassName());

    // Send the class name first
    sock->Send(classNameMessage);

    // Now send the request
    sock->Send(requestMessage);

    // Now we need to send a store inventory request
    StoreProtocolSpec::StoreInventoryRequest inventoryRequest;
    CxxSockets::Message invreq;
    inventoryRequest.write(invreq);
    CxxSockets::Message irclassname(inventoryRequest.getClassName());
    sock->Send(irclassname);
    sock->Send(invreq);

    // And wait for the reply
    CxxSockets::Message replyClassName;
    CxxSockets::Message replyMessage;
    try {
        sock->Receive(replyClassName);
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    if(replyClassName.str() != "StoreInventoryReply") {
        std::cout << "Bad reply: " << replyClassName.str() << std::endl;
        exit(1);
    }

    sock->Receive(replyMessage);
    
    StoreProtocolSpec::StoreInventoryReply reply;
    std::istringstream is(replyMessage.str());
    reply.read(is);
    
    // Now print out the inventory
    std::cout << std::endl << "inventory of requested books" << std::endl;
    for(std::vector<StoreProtocolSpec::StoreInventoryReply::Book>::iterator i = reply._books.begin();
        i != reply._books.end(); ++i) {
        std::cout << "title=" << (*i)._title
                  << " author=" << (*i)._author
                  << " isbns=";
        for(std::vector<unsigned>::iterator j = (*i)._isbns.begin();
            j != (*i)._isbns.end(); ++j) {
            std::cout << *j << ",";
        }
        std::cout << " number available=" << (*i)._count << std::endl;
    }

    std::cout << std::endl << "inventory of requested music" << std::endl;
    for(std::vector<StoreProtocolSpec::StoreInventoryReply::MP3>::iterator i = reply._mp3s.begin();
        i != reply._mp3s.end(); ++i) {
        std::cout << "song name=" << (*i)._name
                  << " artist=" << (*i)._artist
                  << " id numbers=";
        for(std::vector<unsigned>::iterator j = (*i)._ids.begin();
            j != (*i)._ids.end(); ++j) {
            std::cout << *j << ",";
        }
        std::cout << " number available=" << (*i)._count << std::endl << std::endl;;
    }
}
