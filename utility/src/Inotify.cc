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


#include "Inotify.h"

#include <utility/include/Log.h>

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/throw_exception.hpp>

#include <boost/system/error_code.hpp>

#include <sstream>

#include <errno.h>


using boost::bind;

using std::ostringstream;
using std::string;


LOG_DECLARE_FILE( "utility" );


namespace bgq {
namespace utility {


//---------------------------------------------------------------------
// class Inotify::WatchImpl


Inotify::WatchImpl::WatchImpl(
        int id,
        uint32_t mask,
        Inotify& inotify
    ) :
        _id(id),
        _mask(mask),
        _inotify(inotify)
{
    // Nothing to do.
}


Inotify::WatchImpl::~WatchImpl()
{
    if ( _id == -1 ) {
        // already invalidated.
        return;
    }

    try {
        // Notify the _inotify.
        _inotify._endWatch( _id );
    } catch ( const boost::system::system_error& e ) {
        LOG_WARN_MSG( e.what() );
    }
}


//---------------------------------------------------------------------
// class Inotify

string
Inotify::formatMask(
        uint32_t mask
        )
{
    ostringstream oss;

    oss << "|";
    if ( mask & IN_ACCESS )  oss << "ACCESS|";
    if ( mask & IN_ATTRIB )  oss << "ATTRIB|";
    if ( mask & IN_CLOSE_WRITE )  oss << "CLOSE_WRITE|";
    if ( mask & IN_CLOSE_NOWRITE )  oss << "CLOSE_NO_WRITE|";
    if ( mask & IN_CREATE )  oss << "CREATE|";
    if ( mask & IN_DELETE )  oss << "DELETE|";
    if ( mask & IN_DELETE_SELF )  oss << "DELETE_SELF|";
    if ( mask & IN_MODIFY )  oss << "MODIFY|";
    if ( mask & IN_MOVE_SELF )  oss << "MOVE_SELF|";
    if ( mask & IN_MOVED_FROM )  oss << "MOVED_FROM|";
    if ( mask & IN_MOVED_TO )  oss << "MOVED_TO|";
    if ( mask & IN_OPEN ) oss << "OPEN|";
    if ( mask & IN_IGNORED )  oss << "IGNORED|";
    if ( mask & IN_ISDIR )  oss << "ISDIR|";
    if ( mask & IN_Q_OVERFLOW )  oss << "Q_OVERFLOW|";
    if ( mask & IN_UNMOUNT )  oss << "UNMOUNT|";

    return oss.str();
}


Inotify::Inotify(
        boost::asio::io_service& io
        ) :
    boost::asio::posix::stream_descriptor( io )
{
#ifdef IN_CLOEXEC
    this->assign( inotify_init1(IN_CLOEXEC) );
#else
    this->assign( inotify_init() );
    
    // enable close on exec
    (void)fcntl(
            this->native(),
            F_SETFD,
            fcntl(this->native(), F_GETFD) | FD_CLOEXEC
            );
#endif
}


Inotify::Watch Inotify::watch(
        const boost::filesystem::path& path,
        uint32_t mask
    )
{
    int watch_id(inotify_add_watch( native(),
#if BOOST_FILESYSTEM_VERSION == 3
                path.native().c_str(),
#else
                path.file_string().c_str(),
#endif
                mask));

    if ( watch_id == -1 ) {
        BOOST_THROW_EXCEPTION( boost::system::system_error( boost::system::errc::make_error_code( boost::system::errc::errc_t( errno ) ) ) );
    }

    // Add the Watch to my map of watches.

    Watch watch_ptr(boost::make_shared<WatchImpl>( watch_id, mask, boost::ref(*this) ));

    _watches[watch_id] = watch_ptr;

    return watch_ptr;
}


void
Inotify::read( 
        Events& events_out, 
        boost::system::error_code& err_out
        )
{
    read_some( boost::asio::null_buffers(), err_out ); // will block if there's nothing to read.

    if ( err_out ) {
        return;
    }

    _readEvents( events_out, err_out );
}


void
Inotify::async_read( 
        Events& events_out, 
        ReadHandler handler
        )
{
    events_out.clear();

    async_read_some(
            boost::asio::null_buffers(),
            bind( &Inotify::_readHandler, this,
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred(),
                  boost::ref( events_out ),
                  handler
              )
        );
}


void 
Inotify::_readEvents( 
        Events& events_out, 
        boost::system::error_code& err_out
        )
{
    bytes_readable bytes_readable;

    io_control( bytes_readable, err_out );

    if ( err_out ) {
        return;
    }

    LOG_DEBUG_MSG( "read: bytes_available=" << bytes_readable.get() );

    std::vector<uint8_t> buf( bytes_readable.get() );

    read_some( boost::asio::buffer( buf ), err_out );

    if ( err_out ) {
        return;
    }

    events_out.clear();

    _parseEvents( buf, events_out );
}


void
Inotify::_parseEvents( 
        const Buffer& buf, 
        Events& events_out
        )
{
    const struct inotify_event *begin(reinterpret_cast<const struct inotify_event*> ( &(buf[0]) ));
    const struct inotify_event *end(reinterpret_cast<const struct inotify_event*> ( &(buf[buf.size()]) ));

    const struct inotify_event *cur_event_struct(begin);

    // make sure cur_event_struct isn't off the end of the buffer...
    while ( cur_event_struct < end ) {
        Event event;
        const struct inotify_event *next_event_struct;

        if ( _parseEvent( cur_event_struct, event, next_event_struct ) ) {
            events_out.push_back( event );
        }

        cur_event_struct = next_event_struct;
    }
}


bool
Inotify::_parseEvent(
        const struct inotify_event* event_struct,
        Event& event_out,
        const struct inotify_event*& next_event_struct_out
    )
{
    next_event_struct_out =
            reinterpret_cast<const struct inotify_event*> (
                    reinterpret_cast<const uint8_t*>(event_struct) + (sizeof ( struct inotify_event ) + event_struct->len)
                );

    LOG_TRACE_MSG( "event: wd=" << event_struct->wd << " mask=" << formatMask( event_struct->mask ) << " cookie=" << event_struct->cookie << " len=" << event_struct->len );

    if ( event_struct->mask & IN_Q_OVERFLOW ) {
        event_out.watch_ref = WatchRef();
        event_out.mask = event_struct->mask;
        event_out.name = "";
        return true;
    }

    Watches::iterator watch_i(_watches.find( event_struct->wd ));
    if ( watch_i == _watches.end() ) {
        // Ignore because not watching.
        return false;
    }

    event_out.watch_ref = watch_i->second;

    Watch ptr(event_out.watch_ref.lock());
    if ( ptr == Watch() ) {
        // The watch pointer was invalid so ignore.
        return false;
    }

    uint32_t watch_mask(ptr->getMask());

    event_out.mask = event_struct->mask;

    if ( event_struct->len > 0 ) {
        event_out.name = string( event_struct->name );
    }

    if ( (watch_mask & IN_ONESHOT) || (event_struct->mask & IN_IGNORED) ) {
        ptr->invalidate();
        _watches.erase( event_struct->wd  );
    }

    return true;
}


void
Inotify::_readHandler(
        const boost::system::error_code& error,
        std::size_t /* bytes_transferred */,
        Events& events_out,
        ReadHandler read_handler
    )
{
    if ( error ) {
        read_handler( error );
        return;
    }

    boost::system::error_code err2;

    _readEvents( events_out, err2 );

    read_handler( err2 );
}


void
Inotify::_endWatch( 
        int id
        )
{
    if ( ! _watches.erase( id ) ) {
        // Wasn't even watching.
        return;
    }

    int rc(inotify_rm_watch( native(), id ));

    if ( rc == -1 ) {
        LOG_ERROR_MSG( "endWatch failed with errno=" << errno );
        BOOST_THROW_EXCEPTION( boost::system::system_error( boost::system::errc::make_error_code( boost::system::errc::errc_t( errno ) ) ) );
    }
}


} // namespace bgq::utility
} // namespace bgq
