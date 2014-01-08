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


#include "LiveMonitor.hpp"

#include "LogMerge.hpp"

#include <utility/include/Log.h>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <iostream>


LOG_DECLARE_FILE( "log_merge" );


namespace log_merge {


LiveMonitor::LiveMonitor(
        std::ostream &os,
        const Configuration& configuration,
        const FilenameMapper& filename_mapper
    ) :
        _os( os ),
        _log_files( filename_mapper ),
        _timer( _io_service ),
        _inotify( _io_service ),
        _waiting(false),
        _no_events_count(0)
{
    std::copy(
            configuration.getPaths().begin(),
            configuration.getPaths().end(),
            std::back_inserter( _paths )
        );
}


void LiveMonitor::run()
{
    LogMerge::validatePaths( _paths ); // will throw if any problems in the paths.

    _cur_di_ptr.reset( new DirectoryInfo( _paths ) );

    // When start monitoring logs, need to initialize all the log files to the end...

    BOOST_FOREACH( const Paths::value_type& path, _paths ) {
        if ( boost::filesystem::is_directory( path )  ) {

            boost::filesystem::directory_iterator di_end;
            for ( boost::filesystem::directory_iterator i( path ) ;
                  i != di_end ;
                  ++i )
            {
                // skip non-regular files.
                if ( ! boost::filesystem::is_regular( i->status() ) ) {
                    LOG_DEBUG_MSG( "skipping non-regular file " << i->path() );
                    continue;
                }

                // it's a regular file.

                off_t end_of_file( boost::filesystem::file_size( i->path() ) );
                _log_files.setPath( i->path(), end_of_file );

            }

        } else {
            off_t end_of_file( boost::filesystem::file_size( path ) );
            _log_files.setPath( path, end_of_file );
        }
    }


    _startWaiting( boost::posix_time::seconds( 2 ) ); // wait a couple seconds before first check.

    BOOST_FOREACH( const Paths::value_type &path, _paths ) {
        _watches.push_back( _inotify.watch( path, IN_CREATE | IN_MODIFY ) );
    }

    _inotify.async_read(
            _inotify_events,
            boost::bind( &LiveMonitor::_handleInotify, this, _1 )
        );

    _io_service.run();
}


void LiveMonitor::_checkFiles()
{
    // Look at the directories again.

    DirectoryInfo new_di( _paths );

    // Figure out what files have changed from the current info.
    Paths changed_paths(DirectoryInfo::calcChanges( *_cur_di_ptr, new_di ));

    // Notify log_files to update the files that have changed and the new interval to use (to 1 second ago)

    _log_files.updatePaths(
            changed_paths,
            Interval::create(
                    boost::posix_time::min_date_time,
                    boost::posix_time::second_clock::local_time() - boost::posix_time::seconds(1)
               )
        );

    bool any_records(false);

    for (  ; _log_files.hasRecord() ; _log_files.advance()  ) {
        const LogFile &log_file(_log_files.getLogWithOldestRecord());
        _os << log_file.getLabel() << ": " << log_file.getCurrentRecord().getText() << "\n";

        any_records = true;
    }

    CurrentPositions current_positions(_log_files.getCurrentPositions( changed_paths ));

    _cur_di_ptr->update( current_positions );


    if ( any_records ) {
        // Call again in 1 sec.

        _no_events_count = 0;

        LOG_DEBUG_MSG( "Got events this time" );

        _startWaiting( boost::posix_time::seconds( 1 ) ); // Wait a second for new log records to appear.

    } else {
        // No records, stop waiting.

        ++_no_events_count;

        LOG_DEBUG_MSG( "Got no events " << _no_events_count << " times" );

        if ( _no_events_count == 2 ) {
            // Haven't seen any updates for a while, wait for notification.
            _waiting = false;
        } else {
            _startWaiting( boost::posix_time::seconds( 2 ) ); // Try again in a couple seconds.
        }
    }
}


void LiveMonitor::_handleInotify(
        const boost::system::error_code& error
    )
{
    LOG_DEBUG_MSG( "Got inotify, events size=" << _inotify_events.size() );

    if ( ! _waiting ) {
        LOG_DEBUG_MSG( "Wasn't waiting, setting timer." );

        _startWaiting( boost::posix_time::seconds( 2 ) ); // Give it a couple of seconds to settle down.
    }

    // keep reading from inotify.
    _inotify.async_read(
            _inotify_events,
            boost::bind( &LiveMonitor::_handleInotify, this, _1 )
        );
}


void LiveMonitor::_startWaiting( const boost::posix_time::time_duration& duration )
{
    _timer.expires_from_now( duration );

    _timer.async_wait(
            boost::bind( &LiveMonitor::_checkFiles, this )
        );

    _waiting = true;
}


} // namespace log_merge
