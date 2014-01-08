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

#ifndef BASE64_HEADER
#define BASE64_HEADER

#include <iostream>

#if XML_FIXES
void encode64_new(std::ostream& os, const std::string& buf);
std::string decode64_new(const char *buf, unsigned buflen);

#else
void encode64(std::ostream &os,	const char *source, unsigned ssize, bool newlines);
void decode64(std::ostream &os, const char *source, unsigned ssize);
void encode64(std::ostream &os, const std::string& source, bool newlines);
void decode64(std::ostream &os, const std::string& source);
#endif

#endif
