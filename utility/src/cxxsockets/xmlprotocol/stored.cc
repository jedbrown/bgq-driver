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

// Server side code
#include <map>
#include "SocketTypes.h"
#include <xml/include/library/XML.h>

#include "xml/include/c_api/StoreProtocolSpec.h"

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

    // Maps to hold our list of available materials
    // No MP3s in stock, though
    std::map<std::string, StoreProtocolSpec::StoreInventoryReply::Book> booklist;
    std::map<std::string, StoreProtocolSpec::StoreInventoryReply::MP3> mp3list;

    // Fill the lists
    StoreProtocolSpec::StoreInventoryReply::Book nBook;
    nBook._title = "moby dick";
    nBook._author = "mellville";
    // We'll give it two isbns for variety
    nBook._isbns.push_back(1234);
    nBook._isbns.push_back(4321);
    nBook._count = 2;
    booklist[nBook._title] = nBook;

    StoreProtocolSpec::StoreInventoryReply::Book oBook;
    oBook._title = "cryptonomicon";
    oBook._author = "stephenson";
    // We'll give it two isbns for variety
    oBook._isbns.push_back(5678);
    oBook._isbns.push_back(8765);
    nBook._count = 5;

    booklist[oBook._title] = oBook;

    StoreProtocolSpec::StoreInventoryReply::MP3 mp3;
    mp3._name = "In-A-Gadda-Da-Vida";
    mp3._artist = "Iron Butterfly";
    mp3._ids.push_back(9087);
    mp3._count = 36;

    mp3list[mp3._name] = mp3;

    // Create a socket address list.  Family of "0" means I don't care what I get.
    CxxSockets::SockAddrList salist(AF_UNSPEC, "", "30004");
    
    // Make a set of listeners (which could contain only one) based on the socket addr list.
    CxxSockets::ListenerSetPtr listener(new CxxSockets::ListenerSet(salist, SOMAXCONN, CxxSockets::INSECURE));
    
    // Create a new socket so that we can do an accept.
    CxxSockets::TCPSocketPtr sock(new CxxSockets::TCPSocket(CxxSockets::INSECURE));
    // Accept it.
    CxxSockets::Message msg;
    try {
        listener->AcceptNew(sock);
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    bool done = false;

    // We're going to need a list to operate on.
    StoreProtocolSpec::StoreListRequest tr;

    while(!done) {
        // Now wait for a message
        CxxSockets::Message classNameMessage;
        sock->Receive(classNameMessage);
        sock->Receive(msg);
        // This prints the serialized message and class name
        std::cout << classNameMessage.str() << std::endl;
        std::cout << msg.str() << std::endl;


        if(classNameMessage.str() == "StoreListRequest") {
            // We have a target request
            std::istringstream is(msg.str());
            tr.read(is);
        } else if (classNameMessage.str() == "StoreInventoryRequest") {
            // Want to know if the items in the target are in stock
            StoreProtocolSpec::StoreInventoryReply ireply;

            // Loop through everything in the request and add an element to the reply.
            // First, the books.
            for(std::vector<std::string>::iterator i = tr._books.begin(); 
                i != tr._books.end(); ++i) {
                // If the requested item is in the list, put it in the reply
                if(booklist.find(*i) != booklist.end()) {
                    ireply._books.push_back(booklist[*i]);
                }
            }

            // Same thing with MP3s
            for(std::vector<std::string>::iterator i = tr._songs.begin(); 
                i != tr._songs.end(); ++i) {
                // If the requested item is in the list, put it in the reply
                if(mp3list.find(*i) != mp3list.end()) {
                    ireply._mp3s.push_back(mp3list[*i]);
                }
            }

            ireply._rc = 0;

            // Now send back the reply
            CxxSockets::Message replyClassName(ireply.getClassName());
            sock->Send(replyClassName);
            CxxSockets::Message replyMessage;
            ireply.write(replyMessage);
            sock->Send(replyMessage);
            done = true;
        } else {
            std::cerr << "Bad class name " << classNameMessage.str() << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    sleep(2);
}
