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


#ifndef LOG_MERGE_STREAM_POOL_H_
#define LOG_MERGE_STREAM_POOL_H_


#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <boost/filesystem/fstream.hpp>

#include <iosfwd>
#include <map>
#include <deque>

#include <sys/types.h>


namespace log_merge {


typedef boost::shared_ptr<boost::filesystem::ifstream> StreamPtr;
typedef boost::weak_ptr<boost::filesystem::ifstream> StreamWkPtr;


class StreamPool
{
public:

    explicit StreamPool( unsigned size = 20 );

    StreamPtr getStream(
            const boost::filesystem::path& file_path,
            off_t offset = 0
        );

    void remove( const boost::filesystem::path& file_path );


private:

    typedef std::map<boost::filesystem::path,StreamPtr> _PathToStreamPtr;
    typedef std::deque<boost::filesystem::path> _Lru;

    unsigned _size;
    _PathToStreamPtr _path_to_stream_ptr;
    _Lru _lru;

};

} // namespace log_merge

#endif
