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

#include "base64.hpp"

#include <openssl/evp.h>

#include <vector>


using std::string;


namespace bgws {
namespace utility {
namespace base64 {


std::string encode( const Bytes& bytes )
{
    // use openssl to base-64 encode the bytes.

    std::vector<char> b64_chars( ((bytes.size() + 2) / 3 * 4) + 1 );

    int bytes_written(EVP_EncodeBlock(
            reinterpret_cast<unsigned char*>( b64_chars.data() ),
            bytes.data(),
            bytes.size()
        ));

    // LOG_INFO_MSG( "bytes size=" << bytes.size() << " b64_bytes size=" << b64_bytes.size() << " bytes_written=" << bytes_written );

    return string( b64_chars.begin(), b64_chars.begin() + bytes_written );
}


Bytes decode( const std::string& s )
{
    // use openssl to base-64 decode the bytes.

    Bytes ret( s.size() ); // The # bytes will be shorter than the string, will resize.

    int bytes_written(EVP_DecodeBlock(
            ret.data(),
            reinterpret_cast<const unsigned char*>( s.data() ),
            s.size()
        ));

    // LOG_INFO_MSG( "base64Decode: b64str=" << s << " ret size=" << ret.size() << " bytes_written=" << bytes_written );

    if ( bytes_written == -1 ) {
        return Bytes();
    }

    ret.resize( bytes_written );

    return ret;
}


} // namespace bgws::utility::base64
} // namespace bgws::utility
} // namespace bgws
