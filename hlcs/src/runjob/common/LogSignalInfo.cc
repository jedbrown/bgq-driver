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
#include "common/LogSignalInfo.h"

#include "common/logging.h"

#include <utility/include/Symlink.h>
#include <utility/include/UserId.h>

#include <boost/filesystem.hpp>

LOG_DECLARE_FILE( runjob::log );

namespace runjob {

LogSignalInfo::LogSignalInfo(
        const siginfo_t& info
        )
{
    LOG_WARN_MSG( "received signal " << info.si_signo );

    // log why the signal was sent
    std::string why_sent;
    switch ( info.si_code ) {
        case SI_USER:       why_sent = "USER"; break;
        case SI_KERNEL:     why_sent = "KERNEL"; break;
        default:            why_sent = "UNKNOWN"; break;
    }
    LOG_WARN_MSG( "signal sent from " << why_sent );

    if ( info.si_pid ) {
        // log pid of sender
        LOG_WARN_MSG( "sent from pid " << info.si_pid );

        // log process name of sender
        boost::filesystem::path path;
        try {
            path = path / "/proc" / boost::lexical_cast<std::string>(info.si_pid) / "exe";
            LOG_WARN_MSG( std::string(bgq::utility::Symlink(path)) );
        } catch ( const boost::system::system_error& e ) {
            LOG_WARN_MSG( "could not read " << path );
            LOG_WARN_MSG( e.what() );
        }
    }

    // log uid of sender
    try {
        bgq::utility::UserId uid( info.si_uid );
        LOG_WARN_MSG( "sent from uid " << uid.getUid() << " (" << uid.getUser() << ")" );
    } catch ( const std::runtime_error& e ) {
        LOG_WARN_MSG( "sent from uid " << info.si_uid );
        LOG_WARN_MSG( "could not get username" );
    }
}

} // runjob
