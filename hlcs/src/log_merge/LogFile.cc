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


#include "LogFile.hpp"

#include "FilenameMapper.hpp"
#include "LogLineParser.hpp"

#include <utility/include/Log.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>
#include <stdexcept>
#include <string>


using std::string;


LOG_DECLARE_FILE( "log_merge" );


namespace log_merge {


LogFile::LogFile(
        const boost::filesystem::path& file_path,
        StreamPool& stream_pool, // [ref]
        Interval::ConstPtr interval_ptr,
        const FilenameMapper& filename_mapper
    ) :
        _file_ptr(File::create( file_path, stream_pool, 0 ) ),
        _interval_ptr(interval_ptr),
        _label(filename_mapper.map( _file_ptr->getPath() )),
        _has_record(true),
        _eof(false),
        _current_position(0)
{
    LOG_DEBUG_MSG( "opening " << _file_ptr->getPath() << " size=" << _file_ptr->getSize() << ". Looking for first log record." );

    // Find the first log record.

    while ( _file_ptr->hasCurrentLine() ) {
        _next_line = _file_ptr->getCurrentLine();

        bool has_time;
        string remaining;

        LogLineParser::getInstance().parse( _next_line, &has_time, &_next_time, &remaining );

        if ( has_time ) { // read a log record.
            LOG_DEBUG_MSG( "Got the first log record in " << _file_ptr->getPath() << " time=" << _next_time );
            break;
        }

        _file_ptr->advance();
    }

    // if no log record then reject this file.
    if ( ! _file_ptr->hasCurrentLine() ) {
        LOG_DEBUG_MSG( "Didn't find any log records in " << _file_ptr->getPath() );
        _has_record = false;
        return;
    }

    // if the first log record is in the interval, then this log file is good.

    if ( _interval_ptr->includes( _next_time ) ) {

        LOG_DEBUG_MSG( "for file " << _file_ptr->getPath() << ", first record in interval" );

        advance();
        return;
    }

    // if the first log record is after the interval, then reject this log file.

    if ( _interval_ptr->after( _next_time ) ) {
        _has_record = false;

        LOG_DEBUG_MSG( "rejected " << _file_ptr->getPath() << ", first record after interval" );

        return;
    }

    // otherwise check to see if the last log file isn't before the log file.

    _file_ptr->toLastLine();

    while ( _file_ptr->hasCurrentLine() ) {
        _next_line = _file_ptr->getCurrentLine();

        bool has_time;
        string remaining;

        LogLineParser::getInstance().parse( _next_line, &has_time, &_next_time, &remaining );

        if ( has_time )  break;

        _file_ptr->toPreviousLine();
    }

    if ( ! _file_ptr->hasCurrentLine() ) { // this shouldn't happen.
        _has_record = false;
        return;
    }

    // if the last record is before the interval then this log file doesn't contain any records in the interval.

    if ( _interval_ptr->before( _next_time ) ) {
        _has_record = false;

        LOG_DEBUG_MSG( "rejected " << _label << " (" << _file_ptr->getSize() << ") last record before interval" );

        return;
    }

    _positionToFirstRecordInInterval();

    LOG_DEBUG_MSG( "opened " << _label << " (" << _file_ptr->getSize() << ") with log record." );
}


LogFile::LogFile(
        const boost::filesystem::path& file_path,
        off_t position,
        StreamPool& stream_pool, // [ref]
        Interval::ConstPtr interval_ptr,
        const FilenameMapper& filename_mapper
    ) :
        _file_ptr(File::create( file_path, stream_pool, position ) ),
        _interval_ptr(interval_ptr),
        _label(filename_mapper.map( _file_ptr->getPath() )),
        _has_record(false),
        _eof(false),
        _current_position(position)
{
    // Nothing to do.
}


void LogFile::update(
        Interval::ConstPtr interval_ptr
    )
{
    _interval_ptr = interval_ptr;

    _file_ptr->reset();

    _eof = false;
    _has_record = false;

    // Look for a record from the current position.

    while ( _file_ptr->hasCurrentLine() ) {
        _next_line = _file_ptr->getCurrentLine();

        LOG_DEBUG_MSG( "update got line " << _next_line );

        bool has_time;
        string remaining;

        LogLineParser::getInstance().parse( _next_line, &has_time, &_next_time, &remaining );

        if ( has_time ) {
            _has_record = true;
            break;
        }

        _file_ptr->advance();
    }

    // Didn't find a record.
    if ( ! _has_record ) {
        return;
    }

    // if the record is outside the interval then exit.
    if ( ! _interval_ptr->includes( _next_time ) ) {

        LOG_DEBUG_MSG( "update outside interval!" );

        _has_record = false;
        return;
    }

    LOG_DEBUG_MSG( "update inside interval!" );

    // Otherwise read the record!
    advance();
}


void LogFile::advance()
{
    if ( _eof ) {
        _has_record = false;
        _current_position = _file_ptr->getCurrentPosition();
        return;
    }

    if ( ! _has_record ) {
        return;
    }

    _current_position = _file_ptr->getCurrentPosition();

    string cur_line(_next_line);
    boost::posix_time::ptime cur_time(_next_time);

    while ( true ) {
        _file_ptr->advance();

        if ( ! _file_ptr->hasCurrentLine() ) {
            _eof = true;
            break;
        }

        _next_line = _file_ptr->getCurrentLine();

        bool has_time;
        string remaining;

        LogLineParser::getInstance().parse( _next_line, &has_time, &_next_time, &remaining );

        if ( has_time ) { // it's a log line, so this is the next log line in the file.
            break;
        }

        cur_line += "\n";
        cur_line += _next_line;
    }

    _log_record = LogRecord( cur_time, cur_line );

    // check if next log record is outside of the interval.

    if ( ! _interval_ptr->includes( _next_time ) ) {
        _eof = true;
    }
}



void LogFile::_positionToFirstRecordInInterval()
{
    // otherwise need to find the first record in the interval.

    _file_ptr->toFirstLine();

    off_t start_search_pos(_file_ptr->getCurrentPosition());
    off_t end_search_pos(_file_ptr->getSize());

    while ( (end_search_pos - start_search_pos) > (10*1024) ) {
        off_t test_pos((start_search_pos + end_search_pos)/2);
        _file_ptr->jumpTo( test_pos );

        LOG_DEBUG_MSG( "Searching " << start_search_pos << " - " << end_search_pos );

        while ( _file_ptr->hasCurrentLine() ) {
            _next_line = _file_ptr->getCurrentLine();

            bool has_time;
            string remaining;

            LogLineParser::getInstance().parse( _next_line, &has_time, &_next_time, &remaining );

            if ( has_time )  break; // read a log record

            _file_ptr->advance();
        }

        if ( ! _file_ptr->hasCurrentLine() || (! _interval_ptr->before( _next_time )) ) {
            end_search_pos = test_pos;
            continue;
        }

        start_search_pos = _file_ptr->getCurrentPosition();
    }

    _file_ptr->jumpTo( start_search_pos );

    while ( _file_ptr->hasCurrentLine() ) {
        _next_line = _file_ptr->getCurrentLine();

        bool has_time;
        string remaining;

        LogLineParser::getInstance().parse( _next_line, &has_time, &_next_time, &remaining );

        if ( has_time && _interval_ptr->includes( _next_time ) )  break; // read a log record in the interval.

        _file_ptr->advance();
    }

    if ( ! _file_ptr->hasCurrentLine() ) { // this shouldn't happen.
        _has_record = false;
        return;
    }

    advance();
}

} // namespace log_merge
