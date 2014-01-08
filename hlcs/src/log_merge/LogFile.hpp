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


#ifndef LOGFILE_HPP_
#define LOGFILE_HPP_


#include "File.hpp"
#include "FilenameMapper.hpp"
#include "Interval.hpp"
#include "LogRecord.hpp"
#include "types.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <fstream>
#include <string>

#include <sys/types.h>


namespace log_merge {


class LogFile : boost::noncopyable
{
public:


    typedef boost::shared_ptr<LogFile> Ptr;
    typedef boost::shared_ptr<const LogFile> ConstPtr;


    // Opens file and locates current record for static logs
    LogFile(
            const boost::filesystem::path& file_path,
            StreamPool& stream_pool, // [ref]
            Interval::ConstPtr interval_ptr = Interval::DefaultPtr,
            const FilenameMapper& filename_mapper = FilenameMapper::Default
        );

    // opens file at position for live logs
    LogFile(
            const boost::filesystem::path& file_path,
            off_t position,
            StreamPool& stream_pool, // [ref]
            Interval::ConstPtr interval_ptr = Interval::DefaultPtr,
            const FilenameMapper& filename_mapper = FilenameMapper::Default
        );


    void update(
            Interval::ConstPtr interval_ptr
        );

    bool hasRecord() const { return _has_record; }

    void advance();

    const File& getFile() const  { return *_file_ptr; }
    const std::string& getLabel() const  { return _label; }
    const LogRecord& getCurrentRecord() const  { return _log_record; }

    off_t getCurrentPosition() const  { return _current_position; }


private:

    File::Ptr _file_ptr;
    Interval::ConstPtr _interval_ptr;
    std::string _label;

    bool _has_record;
    LogRecord _log_record;

    bool _eof;
    std::string _next_line;
    boost::posix_time::ptime _next_time;
    off_t _current_position;


    void _positionToFirstRecordInInterval();

};

} // namespace log_merge

#endif
