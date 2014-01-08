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
/* (C) Copyright IBM Corp.  2009, 2011                              */
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

#ifndef __INTERNAL_ERROR_H__
#define __INTERNAL_ERROR_H__

#include <exception>
#include <sstream>

//
// exception class based on the common C++ exception class.
//

class InternalError : public std::exception 
{

public:
   /** Takes a character string describing the error.  */
   explicit  InternalError(const std::string&  __arg) 
   : exception()
   , _M_msg(__arg) 
   {}

   explicit  InternalError(const std::string&  __arg, const std::string&  szFile, unsigned nLine) 
   : exception() 
   { 
      std::ostringstream ostr;
      ostr << "! Internal Error: " << __arg << ": " << szFile << "(" << nLine << ")";
      _M_msg = ostr.str();
   }

   virtual  ~InternalError() throw() {}

   /** Returns a C-style character string describing the general cause of
    *  the current error (the same string passed to the ctor).  */
   virtual const char* what() const throw() { return _M_msg.c_str(); }

private:
   std::string _M_msg;
};

#endif

