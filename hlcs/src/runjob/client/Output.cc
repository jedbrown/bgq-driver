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
#include "client/Output.h"

#include "common/message/StdIo.h"

#include "client/options/Parser.h"

#include "client/FileOutput.h"
#include "client/StreamOutput.h"

#include "common/logging.h"

#include <boost/bind.hpp>

LOG_DECLARE_FILE( runjob::client::log );

namespace runjob {
namespace client {

Output::Ptr
Output::create(
        const boost::shared_ptr<const options::Parser>& options,
        boost::asio::io_service& io_service,
        const int fd,
        const char* label
        )
{
    Ptr result;

    try {
        // create a stream output since this way we can use Boost.Asio stream_descriptors
        result.reset(
                new StreamOutput( options, io_service, fd, label )
                );
    } catch ( const boost::system::system_error& e ) {
        LOG_TRACE_MSG( "could not create StreamOutput for fd " << fd );
        LOG_TRACE_MSG( e.what() );
        LOG_TRACE_MSG( "assuming fd " << fd << " is a file" );
        result.reset(
                new FileOutput( options, io_service, fd, label )
                );
    }
    LOG_TRACE_MSG( IOV_MAX << " IOV_MAX" );

    return result;
}

Output::Output(
        const boost::shared_ptr<const options::Parser>& options,
        const char* label
        ) :
    _outbox(),
    _options( options ),
    _label( label ),
    _longPrefix( "[" ),
    _longSuffix( "]: " ),
    _shortSuffix( ": " ),
    _rank(),
    _addLabel( true )
{

}

Output::~Output()
{
    LOG_DEBUG_MSG( __FUNCTION__ );
}

void
Output::write(
        const message::StdIo::ConstPtr& msg
        )
{
    // add to queue
    _outbox.push_back( msg );

    // let previous message finish sending
    if ( _outbox.size() > 1 ) {
        LOG_TRACE_MSG( "added message to queue, size " << _outbox.size() );
    } else {
        // send
        this->write();
    }
}

void
Output::writeHandler(
        const boost::system::error_code& error,
        const size_t length
        )
{
    LOG_TRACE_MSG( "write handler" );

    // remove previous message from queue
    _outbox.pop_front();

    // handle errors
    if ( error == boost::asio::error::operation_aborted ) {
        LOG_TRACE_MSG( "done writing output" );
    } else if ( error ) {
        LOG_ERROR_MSG( "could not write: " << boost::system::system_error(error).what() );
        return;
    } else {
        LOG_TRACE_MSG( "wrote " << length << " bytes" );
    }

    // start another send if outbox is not empty
    if ( !_outbox.empty() ) {
        this->write();
    }
}

} // client
} // runjob
