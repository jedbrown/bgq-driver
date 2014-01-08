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

#include "Pipe.hpp"

#include "utility.hpp"

#include <utility/include/Log.h>

#include <boost/throw_exception.hpp>

#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

#include <stdexcept>
#include <string>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>


using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace utility {


Pipe::Pipe(
        boost::asio::io_service &io_service
    )
{
    int fds[2];

    int rc;

    rc = pipe( fds );

    if ( -1 == rc ) {
        int pipe_errno(errno);

        BOOST_THROW_EXCEPTION(
            boost::system::system_error(
                    boost::system::errc::make_error_code(
                            boost::system::errc::errc_t( pipe_errno )
                        ),
                    "pipe()"
                )
            );
    }

    _read_sd_ptr.reset( new boost::asio::posix::stream_descriptor( io_service, fds[0] ) );
    _write_sd_ptr.reset( new boost::asio::posix::stream_descriptor( io_service, fds[1] ) );

    rc = fcntl( fds[0], F_SETFD, FD_CLOEXEC );
    if ( -1 == rc ) {
        int fcntl_errno(errno);
        BOOST_THROW_EXCEPTION(
            boost::system::system_error(
                    boost::system::errc::make_error_code(
                            boost::system::errc::errc_t( fcntl_errno )
                        ),
                    "fcntl()"
                )
            );
    }

    rc = fcntl( fds[1], F_SETFD, FD_CLOEXEC );
    if ( -1 == rc ) {
        int fcntl_errno(errno);
        BOOST_THROW_EXCEPTION(
            boost::system::system_error(
                    boost::system::errc::make_error_code(
                            boost::system::errc::errc_t( fcntl_errno )
                        ),
                    "fcntl()"
                )
            );
    }

}


} // namespace bgws::utility
} // namespace bgws
