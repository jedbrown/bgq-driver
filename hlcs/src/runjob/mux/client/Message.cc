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
#include "mux/client/Message.h"

#include "common/Message.h"

#include <cstring>
#include <ostream>

namespace runjob {
namespace mux {
namespace client {

Message::Message(
        const runjob::Message::Ptr& message
        ) :
    _header(),
    _buffer()
{
    std::ostream os( &_buffer );
    message->serialize(os);
    _header._type = message->getType();
    _header._length = static_cast<uint32_t>( _buffer.size() );
}

} // client
} // mux
} // runjob
