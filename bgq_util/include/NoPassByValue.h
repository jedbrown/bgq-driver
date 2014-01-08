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

#ifndef _NO_PASS_BY_VALUE_H
#define _NO_PASS_BY_VALUE_H

// A base for classes which must not be passed by value
// because they contain file descriptors, sockets, or pointers
// to new'd objects, and thus for which the default (compiler-generated)
// bitwise copy constructor would be inappropriate.
//
class NoPassByValue
   {
public:
   NoPassByValue() {}
private:
   // private, to provoke compiler error on attempted use
   NoPassByValue(const NoPassByValue&);
   NoPassByValue& operator=(const NoPassByValue&);
   };


#endif
