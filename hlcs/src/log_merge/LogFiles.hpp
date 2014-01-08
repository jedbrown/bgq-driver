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


#ifndef LOGFILES_HPP_
#define LOGFILES_HPP_


#include "File.hpp"
#include "FilenameMapper.hpp"
#include "Interval.hpp"
#include "LogFile.hpp"
#include "StreamPool.hpp"
#include "types.hpp"

#include <boost/filesystem.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <queue>
#include <set>
#include <string>
#include <vector>


namespace log_merge {


class LogFiles
{
public:

    LogFiles(
            Interval::ConstPtr interval_ptr = Interval::DefaultPtr, //!< [in]
            const FilenameMapper& filename_mapper = FilenameMapper::Default //!< [ref]
        );

    explicit LogFiles(
            const FilenameMapper& filename_mapper //!< [ref]
        );

    void add(
            const boost::filesystem::path& path,
            bool* no_records_out
        );

    void setPath(
            const boost::filesystem::path& file_path,
            off_t position
        );

    void updatePaths(
            const Paths& paths,
            Interval::ConstPtr interval_ptr
        );

    bool hasRecord() const  { return (! _log_file_ptrs.empty()); }

    const LogFile& getLogWithOldestRecord() const;

    void advance();

    CurrentPositions getCurrentPositions(
            const Paths& paths
        ) const;


private:


    // Compares two Log Files for the priority queue order, the one with the earliest current log record is first.
    struct _Cmp : public std::binary_function<LogFile::ConstPtr, LogFile::ConstPtr, bool> {
        bool operator()( const LogFile::ConstPtr& lhs, const LogFile::ConstPtr& rhs ) {
            return (lhs->getCurrentRecord().getTimestamp() > rhs->getCurrentRecord().getTimestamp());
        }
    };

    typedef std::priority_queue<LogFile::Ptr, std::vector<LogFile::Ptr>, _Cmp > _LogsPriorityQueue;

    typedef std::map<boost::filesystem::path,LogFile::Ptr> _AllFiles;


    Interval::ConstPtr _interval_ptr;
    const FilenameMapper& _filename_mapper;

    StreamPool _stream_pool;
    _LogsPriorityQueue _log_file_ptrs;
    _AllFiles _all_files;


    void _addDirectory(
            const boost::filesystem::path& directory_path
        );

    void _addFile(
            const boost::filesystem::path& file_path,
            bool* no_records_out
        );

    bool _alreadyContains( const boost::filesystem::path& file_path ) const;
};

} // namespace log_merge

#endif
