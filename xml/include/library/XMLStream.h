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

#ifndef XMLSTREAM_HEADER
#define XMLSTREAM_HEADER

#include <iostream>
#include <string>

#include "xml/include/library/TCPStream.h"
#include "xml/include/library/XML.h"

namespace XML {

  class Ostream : public TCPOstream {
  public:
    Ostream(int fd = -1, bool shouldClose = true) 
      : TCPOstream(fd, shouldClose) {}

    void write(const std::string& className, const XML::Serializable& ser) {
      *this << className << " " << ser << std::endl;
    }

  protected:
    
  };

  class Istream : public TCPIstream {
  public:
    Istream(int fd = -1, bool shouldClose = true) 
      : TCPIstream(fd, shouldClose) {}

    std::string getClassName() {
      std::string result("UDEF_TYPE");
      *this >> result;
      return result;
    }

    bool readObject(XML::Serializable& ser) {
      return ser.read(*this);
    }

  protected:

  };

  class IOstream : public TCPIOstream {
  public:
    IOstream(int fd = -1, bool shouldClose = true) 
      : TCPIOstream(fd, shouldClose) {}

    void write(const std::string& className, const XML::Serializable& ser) {
      *this << className << " " << ser << std::endl;
    }

    std::string getClassName() {
      std::string result("UDEF_TYPE");
      *this >> result;
      return result;
    }

    bool readObject(XML::Serializable& ser) {
      return ser.read(*this);
    }

  protected:
    
  };

  class Sstream : public std::stringstream {
  public:
    Sstream() 
      : std::stringstream() {}

    std::string getClassName() {
      std::string result("UDEF_TYPE");
      *this >> result;
      return result;
    }

    bool readObject(XML::Serializable& ser) {
      return ser.read(*this);
    }

  protected:

  };


}; // namespace




#endif
