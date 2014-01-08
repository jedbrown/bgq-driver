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


#include "DirectoryInfo.hpp"

#include <utility/include/Log.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <stdexcept>
#include <string>


LOG_DECLARE_FILE( "log_merge" );


namespace fs = boost::filesystem;

using boost::lexical_cast;

using std::runtime_error;
using std::string;


namespace log_merge {


Paths DirectoryInfo::calcChanges(
        const DirectoryInfo& d1,
        const DirectoryInfo& d2
    )
{
    Paths ret;

    BOOST_FOREACH( const _PathToPosition::value_type& val, d2._path_to_position ) {
        const fs::path& p(val.first);

        _PathToPosition::const_iterator d1_i(d1._path_to_position.find( p ));
        if ( d1_i == d1._path_to_position.end() ) {
            // Didn't find it in d1, so it's new, return the path.
            ret.push_back( p );
            continue;
        }

        if ( d1_i->second == val.second ) {
            continue;
        }

        ret.push_back( p );
    }

    return ret;
}


DirectoryInfo::DirectoryInfo(
        const Paths& paths
    )
{
    BOOST_FOREACH( const Paths::value_type& path, paths ) {

        // if have an equivalent file already, then skip.

        _PathToPosition::const_iterator ptp_i(_findEquivalent( path ));
        if ( ptp_i != _path_to_position.end() ) {
            LOG_DEBUG_MSG( "path " << path << " equivalent to " << ptp_i->first );
            continue;
        }

        // Don't have the path already.

        fs::file_status file_status(fs::status( path ));

        if ( fs::is_regular( file_status ) ) {
            LOG_DEBUG_MSG( "Adding file " << path );

            _path_to_position[path] = fs::file_size( path );

            continue;
        }

        if ( fs::is_directory( file_status ) ) {
            LOG_DEBUG_MSG( "Reading directory " << path );

            fs::directory_iterator end_i;
            for ( fs::directory_iterator d_i( path ) ; d_i != end_i ; ++d_i ) {
                if ( ! fs::is_regular_file( d_i->symlink_status() ) ) {
                    LOG_DEBUG_MSG( "skipping " << d_i->path() << " because is not regular file" );
                    continue;
                }

                _PathToPosition::const_iterator ptp_i(_findEquivalent( path ));
                if ( ptp_i != _path_to_position.end() ) {
                    LOG_DEBUG_MSG( "path " << path << " in directory equivalent to " << ptp_i->first );
                    continue;
                }

                _path_to_position[d_i->path()] = fs::file_size( d_i->path() );
            }

            continue;
        }

        LOG_DEBUG_MSG( "Skipping " << path << " because not regular or directory." );
    }
}


void DirectoryInfo::set(
        const boost::filesystem::path& file_path,
        off_t position
    )
{
    _path_to_position[file_path] = position;
}


void DirectoryInfo::update(
        const CurrentPositions& positions
    )
{
    BOOST_FOREACH( const CurrentPositions::value_type& pos, positions ) {
        _path_to_position[pos.first] = pos.second;
    }
}


DirectoryInfo::_PathToPosition::const_iterator DirectoryInfo::_findEquivalent( const boost::filesystem::path& path ) const
{
    for ( _PathToPosition::const_iterator i(_path_to_position.begin()) ; i != _path_to_position.end() ; ++i ) {
        if ( fs::equivalent( i->first, path ) )  return i;
    }

    return _path_to_position.end();
}


bool DirectoryInfo::operator==( const DirectoryInfo& other ) const
{
    _PathToPosition::const_iterator i1(_path_to_position.begin()), i2(other._path_to_position.begin());

    for ( ; i1 != _path_to_position.end() && i2 != other._path_to_position.end() ; ++i1, ++i2 ) {
        if ( *i1 != *i2 )  return false;
    }

    return (i1 == _path_to_position.end() && i2 == other._path_to_position.end());
}


std::ostream& operator<<( std::ostream& os, const DirectoryInfo& di )
{
    os << "{DI:";
    BOOST_FOREACH( const DirectoryInfo::_PathToPosition::value_type& val, di._path_to_position ) {
        os << val.first << "->" << val.second;
    }
    os << "}";
    return os;
}


} // namespace log_merge
