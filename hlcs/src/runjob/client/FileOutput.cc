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
#include "client/FileOutput.h"

#include "client/options/Label.h"
#include "client/options/Parser.h"

#include "common/logging.h"

#include <boost/bind.hpp>

LOG_DECLARE_FILE( runjob::client::log );

namespace runjob {
namespace client {

FileOutput::FileOutput(
        const boost::shared_ptr<const options::Parser>& options,
        boost::asio::io_service& io_service,
        const int fd,
        const char* label
        ) :
    Output(options, label),
    _io_service( io_service ),
    _fd( fd )
{
    LOG_DEBUG_MSG( _label << " " << fd );
}

void
FileOutput::write()
{
    const message::StdIo::ConstPtr msg = _outbox[0];
    LOG_TRACE_MSG( "message with " << msg->getData().size() << " bytes" );
    ssize_t rc = 0;
    int error = 0;

    if ( _options->getLabel().getScope() == options::Label::None ) {
        // not labeling output, just write it and be done
        rc = ::write( _fd, msg->getData().c_str(), msg->getData().size() );
        if ( rc == -1 ) {
            error = errno;
        }

        this->writeImpl( rc, error );

        return;
    }

    // we are labeling output
    _rank = boost::lexical_cast<std::string>( msg->getRank() );

    std::vector<struct iovec> buffers;

    // loop through entire message block looking for new line characters so we
    // can properly label each line
    std::string::size_type start = 0;
    std::string::size_type newLine = std::string::npos;
    do {
        struct iovec iov;

       if ( _addLabel ) {
            switch ( _options->getLabel().getScope() ) {
                case options::Label::Long:
                    iov.iov_base = const_cast<char*>(_label); iov.iov_len = strlen(_label);
                    buffers.push_back( iov );
                    iov.iov_base = const_cast<char*>(_longPrefix); iov.iov_len = 1;
                    buffers.push_back( iov );
                    iov.iov_base = const_cast<char*>(_rank.c_str()); iov.iov_len = _rank.size();
                    buffers.push_back( iov );
                    iov.iov_base = const_cast<char*>(_longSuffix); iov.iov_len = 3;
                    buffers.push_back( iov );
                    break;
                case options::Label::Short:
                    iov.iov_base = const_cast<char*>(_rank.c_str()); iov.iov_len = _rank.size();
                    buffers.push_back( iov );
                    iov.iov_base = const_cast<char*>(_shortSuffix); iov.iov_len = 2;
                    buffers.push_back( iov );
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

 
        iov.iov_base = const_cast<char*>(msg->getData().c_str() + start);
        iov.iov_len = size;
        buffers.push_back( iov );
        LOG_TRACE_MSG( "buffer at " << start << " with " << size << " bytes" );

        if ( newLine == msg->getData().size() ) break;

        // look for next new line
        start = newLine + 1;
    } while ( newLine != std::string::npos && start != msg->getData().size() );

    LOG_TRACE_MSG( "writing " << buffers.size() << " buffers" );

    rc = writev( _fd, &buffers[0], static_cast<int>(buffers.size()) );
    if ( rc == -1 ) {
        error = errno;
    }

    this->writeImpl( rc, error );
}

void
FileOutput::writeImpl(
        const ssize_t rc,
        const int error
        )
{
    _io_service.post(
            boost::bind(
                &FileOutput::writeHandler,
                Output::shared_from_this(),
                boost::system::error_code(
                    error,
                    boost::system::get_system_category()
                    ),
                rc
                )
            );
}

} // client
} // runjob
