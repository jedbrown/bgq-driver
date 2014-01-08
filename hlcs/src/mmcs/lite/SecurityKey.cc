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

#include "SecurityKey.h"

#include <utility/include/Log.h>

#include <openssl/blowfish.h>

#include <boost/foreach.hpp>

#include <cstring>
#include <iomanip>
#include <sstream>


LOG_DECLARE_FILE( "mmcs.lite" );


namespace mmcs {
namespace lite {


SecurityKey::SecurityKey(
        const fw_uint8_t* key,
        const std::string& block
        ) :
    _clear(),
    _encrypted()
{
    LOG_TRACE_MSG( "generating" );
    (void)block;
    (void)key;

    BF_KEY blowfishKey;
    BF_set_key( &blowfishKey, PERSONALITY_LEN_SECKEY, key );

    // ivec should be all zeros since both sides will need to agree on it
    unsigned char ivec[8] = {0};

    // encrypt the block name
    memcpy( _clear.c_array(), block.c_str(), block.size() );

    // do the encryption
    BF_cbc_encrypt(
            _clear.c_array(),
            _encrypted.c_array(),
            64,
            &blowfishKey,
            ivec,
            BF_ENCRYPT
            );

    std::ostringstream os;
    for ( unsigned i = 0; i < PERSONALITY_LEN_SECKEY; ++i ) {
        os << std::hex << i;
    }
    LOG_TRACE_MSG( "key: " << os.str() );

    os.str( "" );
    BOOST_FOREACH( const unsigned char i, _encrypted ) {
        os << std::setfill('0') << std::setw(2) << std::hex << static_cast<unsigned>(i);
    }
    LOG_TRACE_MSG( "encrypted: " << os.str() );
}

} } // namespace mmcs::lite
