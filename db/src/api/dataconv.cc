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

#include <boost/assert.hpp>

LOG_DECLARE_FILE( "database" );

namespace BGQDB {

bool
hexchar2bitdata(
        unsigned char* dest,
        const unsigned length,
        const std::string& source
        )
{
    BOOST_ASSERT( dest );
    memset( dest, 0, length );

    LOG_DEBUG_MSG( __FUNCTION__ << "() converting source " << source << " with " << source.size() << " characters" );
    char convunit[3] = {0};  // a string of length 2 to give only 2 digits to strtol

    if ( source.size() % 2 ) {
        LOG_WARN_MSG( __FUNCTION__ << "() source " << source << " does not have an even number of digits (" << source.size() << ")" );
        return false;
    }

    if ( source.size() / 2 > length ) {
        LOG_WARN_MSG( __FUNCTION__ << "() destination length of " << length << " is smaller than converted source (" << source.size() / 2 << ")" );
        return false;
    }

    for (unsigned i = 0; i < length * 2; i += 2) {
        if ( !isxdigit(source[i]) ) {
            LOG_ERROR_MSG( __FUNCTION__ << "() character at source[" << i << "] is not hexadecimal: " << source[i] );
            return false;
        }
        if ( !isxdigit(source[i+1]) ) {
            LOG_ERROR_MSG( __FUNCTION__ << "() character at source[" << i+1 << "] is not hexadecimal: " << source[i+1] );
            return false;
        }
        convunit[0] = source[i];
        convunit[1] = source[i+1];

        unsigned result = strtoul(convunit, NULL, 16);
        LOG_DEBUG_MSG( "converted " << convunit << " to 0x" << std::hex << result );

        *(dest++) = static_cast<unsigned char>(result);
    }

    return true;
}

void
trim_right_spaces(
        char *input
        )
{
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
    if ( len == 0 ) return;
    
    char *ptr = input + len - 1;
    while (*ptr == ' ' && ptr >= input ) {
        *ptr = '\0';
        ptr--;
    }
}

} // namespace BGQDB
