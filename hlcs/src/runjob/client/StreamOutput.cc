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
#include "client/StreamOutput.h"

#include "client/options/Label.h"
#include "client/options/Parser.h"

#include "common/logging.h"

#include <boost/bind.hpp>

#include <unistd.h>

LOG_DECLARE_FILE( runjob::client::log );

namespace runjob {
namespace client {

StreamOutput::StreamOutput(
        const boost::shared_ptr<const options::Parser>& options,
        boost::asio::io_service& io_service,
        const int fd,
        const char* label
        ) :
    Output(options, label),
    _output( io_service )
{
    LOG_DEBUG_MSG( _label << " " << fd );

    if ( fd == STDOUT_FILENO || fd == STDERR_FILENO ) {
        // duplicate descriptor since stream descriptor will assume ownership
        int new_fd = dup( fd );
        if ( new_fd == -1 ) {
            LOG_FATAL_MSG( "could not duplicate descriptor " << fd << ": " << strerror(errno) );
            BOOST_THROW_EXCEPTION(
                    std::runtime_error( "dup" )
                    );
        }
        _output.assign( new_fd );
    } else {
        _output.assign( fd );
    }
}

void
StreamOutput::write()
{
    const message::StdIo::ConstPtr msg = _outbox[0];
    LOG_TRACE_MSG( "message with " << msg->getData().size() << " bytes" );

    std::vector<boost::asio::const_buffer> buffers;

    if ( _options->getLabel().getScope() == options::Label::None ) {
        // not labeling output, just write it and be done
        buffers.push_back(
                boost::asio::buffer( msg->getData().c_str(), msg->getData().size() )
                );

        this->writeImpl( buffers );

        return;
    }

    // labeling output
    _rank = boost::lexical_cast<std::string>( msg->getRank() );

    // loop through entire message block looking for new line characters so we
    // can properly label each line
    std::string::size_type start = 0;
    std::string::size_type newLine = std::string::npos;
    do {
        if ( _addLabel ) {
            // only add a label prefix if we need one on this line
            switch ( _options->getLabel().getScope() ) {
                case options::Label::Long:
                    buffers.push_back( boost::asio::buffer(_label, strlen(_label)) );
                    buffers.push_back( boost::asio::buffer(_longPrefix, 1) );
                    buffers.push_back( boost::asio::buffer(_rank) );
                    buffers.push_back( boost::asio::buffer(_longSuffix, 3) );
                    break;
                case options::Label::Short:
                    buffers.push_back( boost::asio::buffer(_rank) );
                    buffers.push_back( boost::asio::buffer(_shortSuffix, 2) );
                    break;
                default:
                    BOOST_ASSERT( !"unhandled label scope" );
                    break;
            }
        }

        newLine = msg->getData().find_first_of( '\n', start );

        std::size_t size;
        if ( newLine == std::string::npos ) {
            // no new line found, print the rest of the message
            size = msg->getData().size() - start;
            _addLabel = false;
        } else {
            // found a new line so we only want to print a subset of the message
            size = newLine - start + 1; 
            _addLabel = true;
        }

        buffers.push_back(
                boost::asio::buffer( msg->getData().c_str() + start, size )
                );
        LOG_TRACE_MSG( "buffer at " << start << " with " << size << " bytes" );

        if ( newLine == msg->getData().size() ) {
            break;
        }

        // look for next new line
        start = newLine + 1;
    } while ( newLine != std::string::npos && start != msg->getData().size() );
    
    this->writeImpl( buffers );
}

void
StreamOutput::writeImpl(
        const std::vector<boost::asio::const_buffer>& buffers
        )
{
    LOG_TRACE_MSG( "writing " << buffers.size() << " buffers" );

    boost::asio::async_write(
            _output,
            buffers,
            boost::bind(
                &StreamOutput::writeHandler,
                Output::shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
                )
            );

}

} // client
} // runjob
