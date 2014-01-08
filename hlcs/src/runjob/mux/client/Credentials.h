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
#ifndef RUNJOB_MUX_CLIENT_CREDENTIALS_H_
#define RUNJOB_MUX_CLIENT_CREDENTIALS_H_

#include "mux/client/fwd.h"

#include <utility/include/UserId.h>

#include <sys/types.h>

namespace runjob {
namespace mux {
namespace client {

/*!
 * \brief Obtain user credentials from a local UNIX socket.
 */
class Credentials
{
public:
    /*!
     * \brief ctor.
     *
     * \throws std::runtime_error if setsockopt fails to disable SO_PASSCRED
     * \throws std::runtime_error if getsockopt fails to get SO_PEERCRED
     */
    explicit Credentials(
            const Id& id,           //!< [in]
            const int descriptor    //!< [in]
            );

    /*!
     * \brief Get pid.
     */
    pid_t getPid() const { return _pid; }

    /*!
     * \brief Get client UserId credentials.
     */
    const bgq::utility::UserId::Ptr& getUid() const { return _uid; }

private:
    const Id& _id;                                                  //!<
    pid_t _pid;                                                     //!< pid of runjob process
    bgq::utility::UserId::Ptr _uid;                                 //!< credentials of runjob process
};

} // client
} // mux
} // runjob

#endif
