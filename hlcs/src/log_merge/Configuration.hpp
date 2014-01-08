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


#ifndef LOG_MERGE_CONFIGURATION_HPP_
#define LOG_MERGE_CONFIGURATION_HPP_

#include "Interval.hpp"
#include "types.hpp"

#include <iosfwd>
#include <string>
#include <vector>


namespace log_merge {


class Configuration
{
public:

    typedef std::vector<std::string> Args;
    typedef std::vector<std::string> Paths;


    static const std::string DefaultBgqLogsDirectory;

    static const Paths DefaultPaths;


    static boost::posix_time::ptime parseTimestamp( const std::string& s, bool is_start );


    Configuration();


    void setInput( std::istream& is )  { _is_p = &is; }
    void setOutput( std::ostream& os )  { _os_p = &os; }


    void setReadPathsNullTerminated( bool read_null_terminated )  { _read_null_terminated = read_null_terminated; }

    void setPaths( const Paths& paths )  { _paths = paths; }

    void setMapFilenames( MapFilenames::Value map_filenames )  { _map_filenames = map_filenames; }

    void setMapFilenamesBool( bool map_filenames )  { setMapFilenames( map_filenames ? MapFilenames::Enabled : MapFilenames::Disabled ); }

    void setStartTime( const boost::posix_time::ptime& start_time )  { _interval.setStart( start_time ); }
    void setStartTimeStr( const std::string& s );

    void setEndTime( const boost::posix_time::ptime& end_time )  { _interval.setEnd( end_time ); }
    void setEndTimeStr( const std::string& s );

    void setLive( bool is_live )  { _is_live = is_live; }


    void configure( const Args& args );

    bool isHelpRequested() const  { return _help_requested; }

    const Paths& getPaths() const  { return _paths; }

    MapFilenames::Value getMapFilenames() const  { return _map_filenames; }

    const Interval& getInterval() const  { return _interval; }

    bool isLive() const  { return _is_live; }


private:


    std::istream *_is_p;
    std::ostream *_os_p;

    bool _help_requested;
    bool _read_null_terminated;
    Paths _paths;
    MapFilenames::Value _map_filenames;
    Interval _interval;
    bool _is_live;


    void _readPathsFromInput();
};

} // namespace log_merge

#endif
