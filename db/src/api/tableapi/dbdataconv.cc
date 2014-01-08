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

#include "tableapi/dbdataconv.h"

#include <iostream>

#include <stdio.h>

using std::string;

/* function to see SQL_C_BINARY in HEX string format  */
/* used in dump() functions                           */
string
bitDataToChar(
        const unsigned char *d,
        int len
)
{
    string s;
    char buf[3];
    for (int i=0; i<len; i++) {
        sprintf(buf,"%02X",d[i]);
        s+=buf;
    }
    return s;
}

string
bitDataToChar64(
        const unsigned char *d,
        int len
)
{
    string s;
    char buf[3];
    int newlen;
    if (len>64) {
        newlen = 64;
    } else {
        newlen = len;
    }
    for (int i=0; i<newlen; i++) {
        sprintf(buf,"%02X",d[i]);
        s+=buf;
    }
    if (len>64) {
        s+= string(" (truncated)");
    }
    return s;
}

/* function to map a string into SQL_C_BINARY - unsigned char  */
