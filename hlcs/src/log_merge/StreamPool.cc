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


#include "StreamPool.hpp"

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <iostream>
#include <stdexcept>
#include <string>


using boost::lexical_cast;

using std::runtime_error;
using std::string;


LOG_DECLARE_FILE( "log_merge" );


namespace log_merge {


StreamPool::StreamPool( unsigned size )
    : _size(size)
{
    // Nothing to do.
}


StreamPtr StreamPool::getStream(
        const boost::filesystem::path& file_path,
        off_t offset
    )
{
    // check if is already in the pool.
    _PathToStreamPtr::iterator i(_path_to_stream_ptr.find( file_path ));

    // if it's in the pool, move it to the front of the LRU list.
    if ( i != _path_to_stream_ptr.end() ) {

        LOG_TRACE_MSG( "Found path in _path_to_stream_ptr. path=" << file_path );

        _lru.erase( std::find( _lru.begin(), _lru.end(), file_path ) );
        _lru.push_front( file_path );
        return i->second;
    }

    // Make room in the cache if necessary.
    while ( _path_to_stream_ptr.size() >= _size ) {
        // Need to get rid of one, get rid of the least-recently used.
        _path_to_stream_ptr.erase( _lru.back() );
        _lru.pop_back();
    }

    LOG_TRACE_MSG( "Opening " << file_path );

    StreamPtr stream_ptr( new boost::filesystem::ifstream( file_path ) );

    if ( ! *stream_ptr ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to open '" + lexical_cast<string>( file_path ) + "'" ) );
    }

    stream_ptr->seekg( offset );

    _path_to_stream_ptr[file_path] = stream_ptr;
    _lru.push_front( file_path );

    LOG_TRACE_MSG( "Added path to _path_to_stream_ptr. path=" << file_path );

    return stream_ptr;
}


void StreamPool::remove( const boost::filesystem::path& file_path )
{
    _PathToStreamPtr::iterator i(_path_to_stream_ptr.find( file_path ));

    if ( i == _path_to_stream_ptr.end() ) {
        return;
    }

    _path_to_stream_ptr.erase( i );
    std::remove( _lru.begin(), _lru.end(), file_path );
}


} // namespace log_merge
