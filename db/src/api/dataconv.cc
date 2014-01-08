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

#include "dataconv.h"

#include <utility/include/Log.h>

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

LOG_DECLARE_FILE( "database" );

char*
makeChars(
        char* target,
        const char* source,
        int len
)
{
    int cpyLen;                   // length to copy

    cpyLen = strlen(source);      // calc length to copy
    if (len <= cpyLen) {
        cpyLen = len;
    } else {
        memset(target, ' ', len);     // blank fill
    }
    memcpy(target, source, cpyLen);
    return target;
}


// copy each byte and pad with spaces
void
char2bitdata(
        unsigned char *dest,
        unsigned destLen,
        const char *source
)
{
    unsigned  i;
    assert(destLen >= strlen(source));

    for (i = 0; i < strlen(source); i++)
        dest[i] = source[i];

    // padding with spaces
    for (; i < destLen; i++)
        dest[i] = 0x20;
}

// convert hex digits to binary
void
hexchar2bitdata(
        unsigned char *dest,
        unsigned destLen,
        const char *source
)
{
    char convunit[3];  // a string of length 2 to give only 2 digits to strtol
    unsigned i;
    unsigned result;

    assert((strlen(source) % 2) == 0); // assuming an even number of digits
    assert((strlen(source) / 2) <= destLen);

    convunit[2] = 0;

    for (i = 0; i < destLen; i += 2) {
        if (!isxdigit(source[i]) || !isxdigit(source[i+1])) {
            LOG_ERROR_MSG(__FUNCTION__ << " Invalid hex digit");
        }
        assert(isxdigit(source[i]));
        assert(isxdigit(source[i+1]));
        convunit[0] = source[i];
        convunit[1] = source[i+1];

        result = strtoul(convunit, 0, 16);

        assert(result < 256);
        *(dest++) = (unsigned char) result;
    }
}

// convert binary to hex digits
void
bitdata2hexchar(
        char *dest,
        unsigned destLen,
        const unsigned char *source,
        unsigned sourceLen
)
{
    unsigned i;

    assert(destLen > (2 * sourceLen));

    for (i = 0; i < sourceLen; i++) {
        sprintf(dest, "%02x", source[i]);
        dest += 2;
    }
}

void
trim_right_spaces(
        char *input
)
{
    // scanning from right to left replace all spaces with '\0' until it finds a non-space
    // character
    trim_right_spaces( input, strlen(input) );
}

void
trim_right_spaces(
        char *input,
        int len
)
{
    // scanning from right to left replace all spaces with '\0' until it finds a non-space
    // character
    if( len == 0 ) { // nothing to trim
        return;
    }
    char *ptr = input + len - 1;
    while(*ptr == ' ' && ptr >= input ) {
        *ptr = '\0';
        ptr--;
    }
}

bool
isSerialnumberString(
        const char* str
)
{
#define SERIALNUMBER_LENGTH 48
    int size = strlen(str);
    if (size != SERIALNUMBER_LENGTH) {
        return false;
    }
    for (int i=0; i < size; ++i) {
        if (!isxdigit(str[i])) {
            return false;
        }
    }
    return true;
}
