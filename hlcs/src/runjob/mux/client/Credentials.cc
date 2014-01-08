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
#include "mux/client/Credentials.h"

#include "mux/client/Id.h"

#include "common/logging.h"

#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

#include <sys/types.h>
#include <sys/socket.h>

#include <cstring>

LOG_DECLARE_FILE( runjob::mux::log );

namespace runjob {
namespace mux {
namespace client {

Credentials::Credentials(
        const Id& id,
        const int descriptor
        ) :
    _id ( id ),
    _pid( 0 ),
    _uid()
{
    LOGGING_DECLARE_LOCATION_MDC( _id );
    
    // disable SO_PASSCRED so peer credentials cannot be sent
    int flag = false;
    int rc = setsockopt(
            descriptor,
            SOL_SOCKET,
            SO_PASSCRED,
            &flag,
            sizeof(flag)
            );

    // ensure it worked
    if ( rc != 0 ) {
        const boost::system::error_code error(errno, boost::system::get_system_category() );
        LOG_ERROR_MSG( "could not disable SO_PASSCRED: " << boost::system::system_error(error).what() );
        BOOST_THROW_EXCEPTION( std::runtime_error("setsockopt") );
    }

    // storage for credentials
    struct ucred peer_credentials;
    bzero( &peer_credentials, sizeof(peer_credentials) );
    socklen_t peer_credentials_size = static_cast<socklen_t>( sizeof(peer_credentials) );

    rc = getsockopt(
            descriptor,
            SOL_SOCKET,
            SO_PEERCRED,
            &peer_credentials,
            &peer_credentials_size
            );

    // ensure it worked
    if ( rc != 0 ) {
        const boost::system::error_code error(errno, boost::system::get_system_category() );
        LOG_ERROR_MSG( "could not get peer credentials: " << boost::system::system_error(error).what() );
        throw std::runtime_error("peer credentials");
    }
    LOG_TRACE_MSG("peer uid " << peer_credentials.uid);
    LOG_TRACE_MSG("peer gid " << peer_credentials.gid);
    LOG_TRACE_MSG("peer pid " << peer_credentials.pid);

    // remember pid
    _pid = peer_credentials.pid;

    // create UserId
    _uid.reset(
            new bgq::utility::UserId( peer_credentials.uid )
            );
    LOG_INFO_MSG( _uid->getUser() << " " << _pid );
}

} // client
} // mux
} // runjob
