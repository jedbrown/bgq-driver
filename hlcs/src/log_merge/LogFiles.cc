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


#include "LogFiles.hpp"

#include <utility/include/Log.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <stdexcept>
#include <string>

#include <unistd.h>


LOG_DECLARE_FILE( "log_merge" );


using boost::lexical_cast;

using std::string;


namespace log_merge {


LogFiles::LogFiles(
        Interval::ConstPtr interval_ptr,
        const FilenameMapper& filename_mapper
    ) :
      _interval_ptr(interval_ptr),
      _filename_mapper(filename_mapper)
{
    // Nothing to do.
}


LogFiles::LogFiles(
        const FilenameMapper& filename_mapper
    ) :
        _interval_ptr(Interval::DefaultPtr),
        _filename_mapper(filename_mapper)
{
    // Nothing to do.
}


void LogFiles::add(
        const boost::filesystem::path& path,
        bool* no_records_out
    )
{
    // if filename is a directory, then add all the files in the directory.

    boost::filesystem::file_status fs(boost::filesystem::status( path ));

    if ( boost::filesystem::is_directory( fs )  ) {
        // it's a directory.

        _addDirectory( path );

        *no_records_out = false; // not sure how to handle this??

        return;
    }

    if ( boost::filesystem::is_regular( fs ) ) {
        _addFile(
                path,
                no_records_out
            );

        return;
    }

    // Don't know how to handle this type of file.

    if ( ! boost::filesystem::exists( fs ) ) {
        BOOST_THROW_EXCEPTION( std::runtime_error(
                "failed to open '" + lexical_cast<string>(path) + "', error is 'No such file or directory'" )
            );
    }

    BOOST_THROW_EXCEPTION( std::runtime_error(
            "could not open '" + lexical_cast<string>(path) + "' because is not a regular file" )
        );
}


void LogFiles::setPath(
        const boost::filesystem::path& file_path,
        off_t position
    )
{
    LOG_DEBUG_MSG( "Setting '" << file_path << " @ " << position );

    _all_files[file_path] = LogFile::Ptr( new LogFile(
            file_path,
            position,
            _stream_pool,
            _interval_ptr,
            _filename_mapper
        ) );
}


void LogFiles::updatePaths(
        const Paths& paths,
        Interval::ConstPtr interval_ptr
    )
{
    _interval_ptr = interval_ptr;

    BOOST_FOREACH( const Paths::value_type& file_path, paths ) {
        _AllFiles::iterator i(_all_files.find( file_path ));

        LogFile::Ptr log_file_ptr;

        if ( i == _all_files.end() ) { // it's a new file.
            log_file_ptr.reset( new LogFile(
                    file_path,
                    0,
                    _stream_pool,
                    _interval_ptr,
                    _filename_mapper
                ) );

            _all_files[file_path] = log_file_ptr;
        } else { // it's not a new file.
            log_file_ptr = i->second;
        }

        // set the interval on the file, check if the log file has a record now, add it if it does.

        log_file_ptr->update( _interval_ptr );

        if ( log_file_ptr->hasRecord() ) {
            _log_file_ptrs.push( log_file_ptr );
        }
    }
}


const LogFile& LogFiles::getLogWithOldestRecord() const
{
    return *(_log_file_ptrs.top());
}


void LogFiles::advance()
{
    LogFile::Ptr log_file_ptr(_log_file_ptrs.top());

    _log_file_ptrs.pop();

    log_file_ptr->advance();

    // If no more records then discard the log file.

    if ( ! log_file_ptr->hasRecord() ) {
        return;
    }

    // otherwise, put it back on the queue.

    _log_file_ptrs.push( log_file_ptr );
}


CurrentPositions LogFiles::getCurrentPositions(
        const Paths& paths
    ) const
{
    CurrentPositions ret;

    BOOST_FOREACH( const Paths::value_type& path, paths ) {
        ret[path] = _all_files.find( path )->second->getCurrentPosition();
    }

    return ret;
}


void LogFiles::_addDirectory(
        const boost::filesystem::path& directory_path
    )
{
    LOG_DEBUG_MSG( "Adding directory " << directory_path );

    boost::filesystem::directory_iterator di_end;
    for ( boost::filesystem::directory_iterator i( directory_path ) ;
          i != di_end ;
          ++i )
    {
        // skip non-regular files.
        if ( ! boost::filesystem::is_regular( i->status() ) ) {
            LOG_DEBUG_MSG( "skipping non-regular file " << i->path() );
            continue;
        }

        // skip any files with a .gz extension.
        if ( i->path().extension() == ".gz" ) {
            LOG_DEBUG_MSG( "skipping compressed file " << i->path() );
            continue;
        }

        // it's a regular file.

        try {
            bool no_records;

            _addFile(
                    i->path(),
                    &no_records
                );
        } catch ( std::runtime_error& rte ) {
            std::cout << "WARNING, " << rte.what() << ".\n";
        }
    }
}


void LogFiles::_addFile(
        const boost::filesystem::path& file_path,
        bool* no_records_out
    )
{
    // Don't add the file if it's already in the list of files (i.e., specified twice)
    if ( _alreadyContains( file_path ) ) {
        *no_records_out = false;
        return;
    }

    LogFile::Ptr ptr( new LogFile(
            file_path,
            _stream_pool,
            _interval_ptr,
            _filename_mapper
        ) );

    if ( ! ptr->hasRecord() ) {
        // This file contains no log records, so don't add it.
        *no_records_out = true;
        return;
    }

    *no_records_out = false;
    _log_file_ptrs.push( ptr );
    _all_files[file_path] = ptr;
}


bool LogFiles::_alreadyContains( const boost::filesystem::path& file_path ) const
{
    // Go through all the current log files, if any are the same file then return true.

    for ( _AllFiles::const_iterator i(_all_files.begin()) ; i != _all_files.end() ; ++i ) {
        if ( i->second->getFile().isSameFileAs( file_path ) ) {
            return true;
        }
    }

    return false;
}

} // namespace log_merge
