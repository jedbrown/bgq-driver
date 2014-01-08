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
#include "mux/client/MessageQueue.h"

#include "mux/client/Message.h"

#include "common/defaults.h"
#include "common/logging.h"
#include "common/Message.h"
#include "common/properties.h"

#include "mux/Options.h"

#include <boost/make_shared.hpp>

#include <ostream>

LOG_DECLARE_FILE( runjob::mux::log );

namespace runjob {
namespace mux {
namespace client {

MessageQueue::MessageQueue(
        const Options& options,
        const Id& id
        ) :
    _id( id ),
    _size( 0 ),
    _highWaterMark( defaults::MuxClientBufferSize ),
    _dropped( 0 ),
    _container()
{
    const std::string key( "client_output_buffer_size" );
    try {
        const bgq::utility::Properties::ConstPtr properties = options.getProperties();
        _highWaterMark = boost::lexical_cast<size_t>(
                properties->getValue( PropertiesSection, key)
                );

        // size cannot be zero
        if ( !_highWaterMark ) {
            _highWaterMark = defaults::MuxClientBufferSize;
            LOG_WARN_MSG( "invalid " << key << " key value from [" << PropertiesSection << "] section of properties" );
            LOG_WARN_MSG( "value cannot be zero" );
            LOG_WARN_MSG( "using default value " << _highWaterMark );
        }
    } catch ( const std::invalid_argument& e ) {
        LOG_WARN_MSG( "missing key " << key << " from [" << PropertiesSection << "] section of properties" );
        LOG_WARN_MSG( "using default value " << _highWaterMark );
    } catch ( const boost::bad_lexical_cast& e ) {
        LOG_WARN_MSG( "garbage in key " << key << " from [" << PropertiesSection << "] section of properties" );
        LOG_WARN_MSG( "using default value " << _highWaterMark );
    }

    const size_t BytesPerKilobyte = 1024;
    _highWaterMark *= BytesPerKilobyte;
    LOG_TRACE_MSG( "high water mark " << _highWaterMark << " bytes" );
}

MessageQueue::~MessageQueue()
{
    LOGGING_DECLARE_LOCATION_MDC( _id );
    if ( _dropped ) {
        LOG_WARN_MSG( "dropped " << _dropped << " messages" );
    }
}

bool
MessageQueue::push_back(
        const runjob::Message::Ptr& message
        )
{
    // create entry for queue by serializing message into a buffer
    const runjob::mux::client::Message::Ptr element(
            boost::make_shared<runjob::mux::client::Message>( message )
            );

    // handle high water mark by dropped stdout or stderr messages
    _size += element->_buffer.size();
    if ( _size > _highWaterMark ) {
        switch ( message->getType() ) {
            case runjob::Message::StdError:
            case runjob::Message::StdOut:
                ++_dropped;
                _size -= element->_buffer.size();
                return false;
            default:
                _container.push_back( element );
                break;
        }
    } else {
        _container.push_back( element );
    }

    return true;
}

void
MessageQueue::pop_front()
{
    if ( this->empty() ) return;

    const Message::Ptr front = this->front();
    _size -= front->_buffer.size();

    _container.pop_front();
}

} // client
} // mux
} // runjob
