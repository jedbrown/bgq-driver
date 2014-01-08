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


#include "File.hpp"

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <iostream>
#include <stdexcept>

#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>


using std::runtime_error;
using std::string;


LOG_DECLARE_FILE( "log_merge" );


namespace log_merge {


File::File(
        const boost::filesystem::path& file_path,
        StreamPool& stream_pool, // [ref]
        off_t position
    ) :
      _path(file_path),
      _stream_pool(stream_pool),
      _position(position),
      _has_current_line(false)
{
    struct stat64 stat_buf;
    int rc(stat64(
#if BOOST_FILESYSTEM_VERSION == 3
                _path.native().c_str(),
#else
                _path.file_string().c_str(),
#endif
                &stat_buf ));

    if ( -1 == rc ) {
        char strerror_buf[128];

        BOOST_THROW_EXCEPTION( runtime_error( string() +
                "failed to open '" +
#if BOOST_FILESYSTEM_VERSION == 3
                _path.native()
#else
                _path.file_string()
#endif
                + "', error is '" + strerror_r( errno, strerror_buf, sizeof ( strerror_buf ) ) + "'"
            ) );
    }

    if ( ! S_ISREG( stat_buf.st_mode ) ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() +
                "file '" +
#if BOOST_FILESYSTEM_VERSION == 3
                _path.native()
#else
                _path.file_string()
#endif
                + "' is not a regular file"
            ) );
    }

    _file_size = stat_buf.st_size;

    advance();
}


void File::reset()
{
    // remove this file's stream from the stream pool so that re-opens next time.
    _stream_pool.remove( _path );

    _position = _line_start;

    advance();
}


void File::advance()
{
    _advance( _getStream() );
}


void File::toFirstLine()
{
    jumpTo( 0 );
}


void File::toLastLine()
{
    StreamPtr stream_ptr(_getStream());

    stream_ptr->clear();

    for ( off_t offset_from_end(2) ; // want to ignore last character because might be \n.
          offset_from_end <= _file_size ;
          ++offset_from_end
        )
    {
        _position = _file_size - offset_from_end;
        stream_ptr->seekg( _position );
        if ( stream_ptr->peek() == '\n' ) {
            stream_ptr->ignore();
            break;
        }
    }

    _advance( stream_ptr );
}


void File::toPreviousLine()
{
    StreamPtr stream_ptr(_getStream());

    stream_ptr->clear();

    if ( _line_start < 2 ) {
        _has_current_line = false;
        return;
    }

    for ( _position = _line_start - 2 ; _position >= 0 ; --_position ) {
        stream_ptr->seekg( _position );

        if ( stream_ptr->peek() == '\n' ) {
            stream_ptr->ignore();
            break;
        }
    }

    _advance( stream_ptr );
}


void File::jumpTo( off_t position )
{
    StreamPtr stream_ptr(_getStream());

    stream_ptr->clear();

    _position = position;

    stream_ptr->seekg( _position );

    _advance( stream_ptr );
}


bool File::isSameFileAs( const boost::filesystem::path& file_path ) const
{
    return boost::filesystem::equivalent( _path, file_path );
}


StreamPtr File::_getStream()
{
    StreamPtr stream_ptr(_stream_wk_ptr.lock());

    if ( ! stream_ptr ) {
        LOG_DEBUG_MSG( "opening " << _path << " @ " << _position );

        stream_ptr = _stream_pool.getStream( _path, _position );
        _stream_wk_ptr = stream_ptr;
    }

    return stream_ptr;
}


void File::_advance( StreamPtr stream_ptr )
{
    _line_start = _position;

    std::getline( *stream_ptr, _current_line );

    if ( ! *stream_ptr ) {
        _has_current_line = false;
        return;
    }

    _has_current_line = true;
    _position = stream_ptr->tellg();

    LOG_TRACE_MSG( "line in " << _path << " is " + _current_line );
}


} // namespace log_merge
