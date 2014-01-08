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
#ifndef RUNJOB_MUX_CLIENT_MESSAGE_H
#define RUNJOB_MUX_CLIENT_MESSAGE_H

#include "common/message/Header.h"

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

namespace runjob {

class Message;

namespace mux {
namespace client {

/*!
 * \brief Entry in the queue.
 */
class Message
{
public:
    /*!
     * \brief ctor.
     */
    Message(
            const boost::shared_ptr<runjob::Message>& message     //!< [in]
           );

    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Message> Ptr;

    message::Header _header;                       //!< message header.
    boost::asio::streambuf _buffer;                //!< serialized message body.
};

} // client 
} // mux
} // runjob

#endif
