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
#ifndef RUNJOB_COMMANDS_HEADER_H
#define RUNJOB_COMMANDS_HEADER_H
/*!
 * \file runjob/commands/Header.h
 * \brief runjob::commands::Header definition and implementation.
 * \ingroup command_protocol
 */

#include <stdint.h>

#include <ostream>

namespace runjob {
namespace commands {

/*!
 * \brief Header used to describe the message that follows.
 * \ingroup command_protocol
 */
struct Header
{
    uint8_t _type;          //!< request or response
    uint32_t _tag;          //!< message type
    uint32_t _length;       //!< length of message
    uint8_t _reserved[8];   //!< reserved bytes
};

/*!
 * \brief Output operator.
 * \ingroup command_protocol
 */
inline std::ostream&
operator<<(
        std::ostream& os,                       //!< [in]
        const Header& header  //!< [in]
        )
{
    os << "type: " << static_cast<unsigned>(header._type) << " ";
    os << "tag: " << header._tag << " ";
    os << "length: " << header._length << " ";

    return os;
}

} // commands
} // runjob

#endif
