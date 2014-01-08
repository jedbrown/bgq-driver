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


#ifndef LOG_MERGE_LIVE_MONITOR_HPP_
#define LOG_MERGE_LIVE_MONITOR_HPP_


#include "Configuration.hpp"
#include "DirectoryInfo.hpp"
#include "FilenameMapper.hpp"
#include "LogFiles.hpp"
#include "types.hpp"

#include <utility/include/Inotify.h>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include <iosfwd>
#include <vector>


namespace log_merge {


class LiveMonitor
{
public:

    LiveMonitor(
            std::ostream &os,
            const Configuration& configuration,
            const FilenameMapper& filename_mapper
        );

    void run();


private:

    typedef std::vector<bgq::utility::Inotify::Watch> _Watches;


    std::ostream &_os;
    LogFiles _log_files;
    Paths _paths;
    boost::shared_ptr<DirectoryInfo> _cur_di_ptr;

    boost::asio::io_service _io_service;
    boost::asio::deadline_timer _timer;

    bgq::utility::Inotify _inotify;
    _Watches _watches;
    bgq::utility::Inotify::Events _inotify_events;

    bool _waiting;
    unsigned _no_events_count;


    void _checkFiles();

    void _handleInotify(
            const boost::system::error_code& error
        );

    void _startWaiting( const boost::posix_time::time_duration& duration );

};


} // namespace log_merge


#endif
