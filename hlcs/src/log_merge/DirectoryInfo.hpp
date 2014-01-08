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


#ifndef LOG_MERGE_DIRECTORY_INFO_HPP_
#define LOG_MERGE_DIRECTORY_INFO_HPP_


#include "types.hpp"

#include <boost/filesystem.hpp>

#include <iosfwd>
#include <map>
#include <vector>

#include <sys/types.h>


namespace log_merge {


class DirectoryInfo {
public:


    static Paths calcChanges(
            const DirectoryInfo& d1,
            const DirectoryInfo& d2
        );


    explicit DirectoryInfo(
            const Paths& paths = Paths()
        );


    void set(
            const boost::filesystem::path& file_path,
            off_t position
        );

    void update(
            const CurrentPositions& positions
        );

    bool operator==( const DirectoryInfo& other ) const;


private:

    typedef std::map<boost::filesystem::path,off_t> _PathToPosition;

    _PathToPosition _path_to_position;


    _PathToPosition::const_iterator _findEquivalent( const boost::filesystem::path& file_path ) const;


    friend std::ostream& operator<<( std::ostream& os, const DirectoryInfo& di );
};


std::ostream& operator<<( std::ostream& os, const DirectoryInfo& di );

} // namespace log_merge

#endif
