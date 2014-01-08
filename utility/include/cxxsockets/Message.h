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
#include <sstream>
#include <iostream>
#include <string>

// Accompanying objects defining
// messages for Socket classes to send
// and receive.
namespace CxxSockets {
    class Message {
        // Needs a buffer object and a means 
        // to manipulate the buffer.
        std::ostringstream message_stream;
    public:
        // ctors
        Message() { message_stream << ""; } ; // empty for receiving
        Message(std::ostringstream& data) { message_stream << data; }
        Message(std::string& msgstr) { message_stream << msgstr; }
        Message(const char* c_string) { message_stream << c_string; }
        void Add(std::ostringstream& data) { message_stream << data; }
        void Replace(std::ostringstream& data) { message_stream.str(data.str()); }
        void Add(char* c_string) { message_stream << c_string; }
        void Clear() { message_stream.str(""); }
        int Size() { return message_stream.str().size(); }
        void Print() { std::cout << message_stream.str() << std::endl; }
        std::string String() { return message_stream.str(); }
        char* getBuff(char* buff) { buff = (char*)(message_stream.str().c_str()); return buff; }
    };
}
