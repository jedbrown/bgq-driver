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
#ifndef RUNJOB_SERVER_BLOCK_SECURITY_KEY_H
#define RUNJOB_SERVER_BLOCK_SECURITY_KEY_H

#include <firmware/include/personality.h>

#include <string>

#include <ramdisk/include/services/JobctlMessages.h>

#include <boost/array.hpp>

namespace runjob {
namespace server {
namespace block {

/*!
 * \brief
 */
class SecurityKey
{
public:
    /*!
     * \brief Container type.
     */
    typedef boost::array<unsigned char, bgcios::jobctl::EncryptedDataSize> Container;

public:
    /*!
     * \brief ctor.
     */
    SecurityKey(
            const fw_uint8_t* key,      //!< assumed to be of length PERSONALITY_LEN_SECKEY
            const std::string& block    //!< block name
            );

    /*!
     * \brief
     */
    const Container& clear() const { return _clear; }

    /*!
     * \brief
     */
    const Container& encrypted() const { return _encrypted; }

private:
    Container _clear;
    Container _encrypted;
};

} // block
} // server
} // runjob

#endif
