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
#ifndef RUNJOB_SERVER_SIM_WATCH_H
#define RUNJOB_SERVER_SIM_WATCH_H

#include "server/cios/fwd.h"

#include <utility/include/Inotify.h>

#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

namespace runjob {
namespace server {
namespace sim {

/*!
 * \brief Wrapper around bgq::utility::Inotify::Watch object to correlate it with a
 * connection to a simulated CIOS component.
 */
struct Watch
{
    boost::filesystem::path _path;                      //!< Path to file being watched.
    boost::shared_ptr<cios::Connection> _connection;    //!< CIOS component connection.
    bgq::utility::Inotify::Watch _watch;                //!< Watch indicator.
};

} // sim
} // server
} // runjob

#endif
