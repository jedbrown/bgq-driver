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


#ifndef FILENAMEMAPPER_HPP_
#define FILENAMEMAPPER_HPP_


#include "types.hpp"

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include <string>
#include <vector>


namespace log_merge {


class FilenameMapper
{
public:


    static const FilenameMapper Default;


    explicit FilenameMapper(
            MapFilenames::Value map_filenames
        );

    std::string map( const boost::filesystem::path& file_path ) const;


private:

    typedef std::pair<boost::regex,std::string> _ReName;
    typedef std::vector<_ReName> _PatternToName;


    static _PatternToName _buildPatternToName();


    static const _PatternToName _pattern_to_name;


    MapFilenames::Value _map_filenames;
};

} // namespace log_merge

#endif
