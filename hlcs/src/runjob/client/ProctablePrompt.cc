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
#include "client/ProctablePrompt.h"

#include "client/MuxConnection.h"

#include "common/message/StartJob.h"

#include "common/logging.h"

#include <utility/include/ScopeGuard.h>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <iostream>

#include <unistd.h>

LOG_DECLARE_FILE( runjob::client::log );

namespace runjob {
namespace client {

ProctablePrompt::Ptr
ProctablePrompt::create(
        boost::asio::io_service& io_service,
        const boost::weak_ptr<MuxConnection>& mux,
        BGQDB::job::Id id,
        Job::Status& status
        )
{
    const Ptr result(
            new ProctablePrompt( io_service, mux, id, status )
            );

    try {
        // stdin has to be duplicated here because a stream_descriptor assumes ownership, and we
        // need to read it later after the job starts for forwarding stdin to the job
        const int input = dup( STDIN_FILENO );
        bgq::utility::ScopeGuard closeGuard( boost::bind(&close, input) );

        result->_input.assign( input );
        closeGuard.dismiss();
    } catch ( const boost::system::system_error& e ) {
        // assume stdin is not a tty
        LOG_WARN_MSG( "standard input is not a tty, ignoring proctable query" );
        return ProctablePrompt::Ptr();
    }

    return result;
}

ProctablePrompt::ProctablePrompt(
        boost::asio::io_service& io_service,
        const boost::weak_ptr<MuxConnection>& mux,
        const BGQDB::job::Id id,
        Job::Status& status
        ) :
    _input( io_service ),
    _buffer(),
    _mux( mux ),
    _id( id ),
    _status( status ),
    _proctable()
{

}

ProctablePrompt::~ProctablePrompt()
{
    LOG_TRACE_MSG( __FUNCTION__ );

    const boost::shared_ptr<MuxConnection> mux( _mux.lock() );
    if ( !mux ) return;

    _status = Job::Starting;
    const message::StartJob::Ptr start( new message::StartJob() );
    start->setJobId( _id );
    mux->write( start );
}

void
ProctablePrompt::start(
        const message::Proctable::ConstPtr& proctable
        )
{
    BOOST_ASSERT( proctable );
    _proctable = proctable;
    const size_t ranks = 
        std::count_if(
                _proctable->_proctable.begin(),
                _proctable->_proctable.end(),
                boost::bind(
                    std::not_equal_to<Uci>(),
                    boost::bind(
                        &runjob::tool::Rank::io,
                        _1
                        ),
                    Uci()
                    )
                );
    std::cout << "tool started on " << _proctable->_io.size() << " I/O nodes for " << ranks << " ranks. " << std::endl;
    std::cout << "Enter a rank to see its associated I/O node's IP address, or press enter to start the job: " << std::endl;
    this->read();
}

void
ProctablePrompt::stop()
{
    boost::system::error_code error;
    _input.cancel( error );
    if ( error ) {
        LOG_WARN_MSG( "could not cancel: " << boost::system::system_error(error).what() );
    }
}

void
ProctablePrompt::read()
{
    boost::asio::async_read_until(
            _input,
            _buffer,
            "\n",
            boost::bind(
                &ProctablePrompt::readHandler,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
                )
            );
}

void
ProctablePrompt::readHandler(
        const boost::system::error_code& error,
        const size_t length
        )
{
    if ( error == boost::asio::error::operation_aborted ) {
        // asked to terminate
        return;
    } else if ( error ) {
        LOG_ERROR_MSG( "could not read: " << boost::system::system_error(error).what() );
    } else {
        LOG_TRACE_MSG( "read " << length << " bytes" );
    }

    std::istream is( &_buffer );
    std::string line;
    std::getline( is, line );
    LOG_TRACE_MSG( "line: " << line );

    if ( line.empty() ) {
        // done reading
        return;
    }

    // keep reading if the user entered anything, even invalid input
    const bgq::utility::ScopeGuard readGuard( 
            boost::bind(
                &ProctablePrompt::read,
                this
                )
            );

    // attempt to convert to rank
    try {
        const int rank = boost::lexical_cast<int>( line );
        if ( rank < 0 ) {
            std::cerr << "ranks must be positive." << std::endl;
            return;
        }

        if ( static_cast<unsigned>(rank) >= _proctable->_proctable.size() ) {
            std::cerr << "invalid rank, must be less than " << _proctable->_proctable.size() << "." << std::endl;
            return;
        }

        if ( _proctable->_proctable[rank].io() == Uci() ) {
            std::cerr << "rank " << rank << " is not participating in the tool" << std::endl;
            return;
        }

        const auto io = _proctable->_io.find( _proctable->_proctable[rank].io() );
        if ( io == _proctable->_io.end() ) {
            std::cerr << "could not find I/O node associated with rank " << rank << "." << std::endl;
            return;
        }

        std::cout << "rank " << rank << " uses I/O node " << io->first << " at IP address " << io->second << std::endl;
    } catch ( const boost::bad_lexical_cast& e ) {
        std::cerr << "invalid rank, must be numeric." << std::endl;
    }
}

} // client
} // runjob
