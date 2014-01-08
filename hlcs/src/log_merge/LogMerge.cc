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


#include "LogMerge.hpp"

#include "Configuration.hpp"
#include "FilenameMapper.hpp"
#include "Interval.hpp"
#include "LiveMonitor.hpp"

#include <utility/include/Log.h>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>

#include <unistd.h>


namespace fs = boost::filesystem;

using boost::lexical_cast;

using std::cout;
using std::runtime_error;
using std::string;


LOG_DECLARE_FILE( "log_merge" );


namespace log_merge {


void LogMerge::validatePaths( const Paths& paths )
{
    BOOST_FOREACH( const Paths::value_type& path, paths ) {
        fs::file_status file_status(fs::status( path ));

        if ( fs::is_regular( file_status ) ) {
            continue;
        }

        if ( fs::is_directory( file_status ) ) {
            continue;
        }

        if ( ! fs::exists( file_status ) ) {
            BOOST_THROW_EXCEPTION( runtime_error( "cannot open " + lexical_cast<string>(path) + ", no such file or directory" ) );
        }

        BOOST_THROW_EXCEPTION( runtime_error( "cannot open " + lexical_cast<string>(path) + ", not a regular file or directory" ) );
    }
}


LogMerge::LogMerge()
    : _os_p(&cout)
{
    // Nothing to do.
}


void LogMerge::run()
{
    FilenameMapper filename_mapper( _configuration.getMapFilenames() );

    if ( _configuration.isLive() ) {
        _runLive( filename_mapper );
    } else {
        _runImmediate( filename_mapper );
    }
}


void LogMerge::_runImmediate( const FilenameMapper& filename_mapper )
{
    LOG_DEBUG_MSG( "interval=" << _configuration.getInterval() );

    LogFiles log_files(
            Interval::ConstPtr( new Interval( _configuration.getInterval() ) ),
            filename_mapper
        );

    BOOST_FOREACH( const Configuration::Paths::value_type& path_str, _configuration.getPaths() ) {
        try {
            bool no_records;

            log_files.add( path_str, &no_records );

            if ( no_records && (! _configuration.getInterval().isSet()) ) {
                (*_os_p) << "WARNING, '" << path_str << "' did not contain any log records.\n";
            }
        } catch ( const runtime_error& rte ) {
            (*_os_p) << "WARNING, " << rte.what() << ".\n";
        }
    }

    for (  ; log_files.hasRecord() ; log_files.advance()  ) {
        const LogFile &log_file(log_files.getLogWithOldestRecord());
        (*_os_p) << log_file.getLabel() << ": " << log_file.getCurrentRecord().getText() << "\n";
    }
}


void LogMerge::_runLive( const FilenameMapper& filename_mapper )
{
    LiveMonitor live_monitor(
            *_os_p,
            _configuration,
            filename_mapper
        );

    live_monitor.run();
}


} // namespace log_merge
