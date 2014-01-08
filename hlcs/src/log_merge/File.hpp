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


#ifndef FILE_HPP_
#define FILE_HPP_


#include "StreamPool.hpp"

#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <boost/filesystem/fstream.hpp>

#include <sys/types.h>

#include <iosfwd>
#include <fstream>
#include <string>


namespace log_merge {


class File : boost::noncopyable
{
public:

    typedef boost::shared_ptr<File> Ptr;


    static Ptr create(
            const boost::filesystem::path& file_path,
            StreamPool& stream_pool, // [ref]
            off_t position = 0
        )
    { return Ptr( new File( file_path, stream_pool, position ) ); }


    File(
            const boost::filesystem::path& file_path,
            StreamPool& stream_pool, // [ref]
            off_t position = 0
        );


    void reset();

    const boost::filesystem::path& getPath() const  { return _path; }
    off_t getSize() const  { return _file_size; }

    bool hasCurrentLine() const  { return _has_current_line; }
    void advance();
    const std::string& getCurrentLine() const  { return _current_line; }

    // set the current line to the first line in the file, sets hasCurrentLine.
    void toFirstLine();

    // set the current line to the last line in the file, sets hasCurrentLine.
    void toLastLine();

    // set the current line to the previous line in the file, sets hasCurrentLine.
    void toPreviousLine();

    // Jump to the given position, will advance to the next line after that position, sets hasCurrentLine.
    void jumpTo( off_t position );

    bool isSameFileAs( const boost::filesystem::path& file_path ) const;

    off_t getCurrentPosition() const  { return _line_start; }


private:

    boost::filesystem::path _path;
    StreamPool &_stream_pool;

    StreamWkPtr _stream_wk_ptr;

    off_t _file_size;
    off_t _line_start, _position;

    bool _has_current_line;
    std::string _current_line;


    StreamPtr _getStream();

    void _advance( StreamPtr stream_ptr );
};

} // namespace log_merge

#endif
