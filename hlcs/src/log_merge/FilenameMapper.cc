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


#include "FilenameMapper.hpp"


namespace fs = boost::filesystem;

using std::string;


namespace log_merge {


const FilenameMapper FilenameMapper::Default( MapFilenames::Disabled );


FilenameMapper::FilenameMapper(
        MapFilenames::Value map_filenames
    ) :
        _map_filenames(map_filenames)
{
    // Nothing to do.
}


FilenameMapper::_PatternToName FilenameMapper::_buildPatternToName()
{
    _PatternToName ret;

    ret.push_back( _ReName( boost::regex( ".*-bgagentd\\.log" ),         "bgagentd      " ) );
    ret.push_back( _ReName( boost::regex( ".*-bgmaster_server\\.log" ),  "bgmaster      " ) );
    ret.push_back( _ReName( boost::regex( ".*-mc_server\\.log" ),        "mc_server     " ) );
    ret.push_back( _ReName( boost::regex( ".*-mmcs_server\\.log" ),      "mmcs_server   " ) );
    ret.push_back( _ReName( boost::regex( ".*-runjob_mux\\.log" ),       "runjob_mux    " ) );
    ret.push_back( _ReName( boost::regex( ".*-runjob_server\\.log" ),    "runjob_server " ) );
    ret.push_back( _ReName( boost::regex( ".*subnet_mc.*\\.log" ),       "subnet_mc     " ) );
    ret.push_back( _ReName( boost::regex( ".*-bgws_server\\.log" ),      "bgws_server   " ) );
    ret.push_back( _ReName( boost::regex( ".*-realtime_server\\.log" ),  "realtime      " ) );

    return ret;
}


const FilenameMapper::_PatternToName FilenameMapper::_pattern_to_name(_buildPatternToName());


std::string FilenameMapper::map( const boost::filesystem::path& file_path ) const
{
    if ( _map_filenames == MapFilenames::Disabled ) {
#if BOOST_FILESYSTEM_VERSION == 3
        return file_path.leaf().native();
#else
        return file_path.leaf();
#endif
    }

    boost::smatch m;

    for ( _PatternToName::const_iterator i(_pattern_to_name.begin()) ; i != _pattern_to_name.end() ; ++i ) {
        if ( regex_match( file_path.string(), m, i->first ) )  return i->second;
    }

    fs::path path2(file_path.leaf());
    path2.replace_extension( "" );

    return path2.string();
}


} // namespace log_merge
