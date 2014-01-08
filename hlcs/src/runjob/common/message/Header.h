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
#ifndef RUNJOB_MESSAGE_HEADER_H
#define RUNJOB_MESSAGE_HEADER_H

#include <utility/include/version.h>

#include <cstring>

#include <stdint.h>

namespace runjob {
namespace message {

/*!
 * \brief Header used to describe the message that follows.
 */
class Header
{
public:
    /*!
     * \brief ctor.
     */
    Header() :
        _type( 0 ),
        _length( 0 ),
        _version( bgq::utility::SvnRevision ),
        _reserved( )
    {
    
    }

    uint32_t _type;             //!< message type
    uint32_t _length;           //!< length of message
    const uint32_t _version;    //!< version of message
    uint8_t _reserved[4];       //!< reserved bytes
};

} // message
} // runjob

#endif
