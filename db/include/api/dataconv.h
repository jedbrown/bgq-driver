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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#ifndef _DATACONV_H_
#define _DATACONV_H_

void trim_right_spaces(char *input);
void trim_right_spaces(char *input, int len);
void char2bitdata(unsigned char *dest, unsigned destLen, const char *source);
void hexchar2bitdata(unsigned char *dest, unsigned destLen, const char *source);
void bitdata2hexchar(char *dest, unsigned destLen, const unsigned char *source, unsigned sourceLen);
bool isSerialnumberString(const char* str);
char* makeChars(char* target, const char* source, int len);



#endif
