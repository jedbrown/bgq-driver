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

#include "utility.hpp"

#include <utility/include/Log.h>

#include <openssl/evp.h>

#include <vector>


using std::string;


LOG_DECLARE_FILE( "bgws.command" );


namespace bgws {
namespace command {

namespace utility {


std::string base64Encode( const std::string& bytes )
{
    // use openssl to base-64 encode the bytes.

    std::vector<char> b64_chars( ((bytes.size() + 2) / 3 * 4) + 1 );

    int bytes_written(EVP_EncodeBlock(
            reinterpret_cast<unsigned char*>( b64_chars.data() ),
            reinterpret_cast<const unsigned char*>( bytes.data() ),
            bytes.size()
        ));

    string ret( b64_chars.begin(), b64_chars.begin() + bytes_written );

    return ret;
}


std::string formatTimestamp( const std::string& timestamp_str )
{
    return (timestamp_str.substr( 0, 10 ) + " " + timestamp_str.substr( 11 ));
}


} // namespace utility

} } // namespace bgws::command
