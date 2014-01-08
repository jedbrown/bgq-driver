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


#ifndef LOG_MERGE_TYPES_HPP_
#define LOG_MERGE_TYPES_HPP_


#include <boost/filesystem.hpp>

#include <map>

#include <sys/types.h>


namespace log_merge {


struct MapFilenames
{
    enum Value {
        Enabled,
        Disabled
    };
};


typedef std::vector<boost::filesystem::path> Paths;

typedef std::map<boost::filesystem::path,off_t> CurrentPositions;


} // namespace log_merge

#endif
