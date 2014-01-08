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
#include "Symlink.h"

#include <utility/include/Log.h>

#include <boost/system/error_code.hpp>

#include <boost/scoped_array.hpp>
#include <boost/throw_exception.hpp>

LOG_DECLARE_FILE( "utility" );

namespace bgq {
namespace utility {

Symlink::Symlink(
        const boost::filesystem::path& path
        ) :
    _link()
{
    ssize_t bufSize = 16;
    boost::scoped_array<char> buf( new char[bufSize] );

    while (1) {
        ssize_t linkSize = readlink(
#if BOOST_FILESYSTEM_VERSION == 3
                path.native().c_str(),
#else
                path.file_string().c_str(),
#endif
                buf.get(), static_cast<size_t>(bufSize)
                );
        if ( linkSize == -1 ) {
            BOOST_THROW_EXCEPTION(
                    boost::system::system_error(
                        boost::system::errc::make_error_code(
                            boost::system::errc::errc_t( errno )
                            )
                        )
                    );
        } else if ( linkSize == bufSize ) {
            bufSize *= 2;
            LOG_TRACE_MSG( "increased buffer size to " << bufSize );
            buf.reset( new char[bufSize] );
        } else {
            _link = std::string( buf.get(), static_cast<size_t>(linkSize) );
            LOG_TRACE_MSG( path << " links to " << _link );
            break;
        }
    }
}

} // utility
} // bgq
