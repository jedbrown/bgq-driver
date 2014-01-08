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
#ifndef RUNJOB_SERVER_JOB_DESTROY_H
#define RUNJOB_SERVER_JOB_DESTROY_H

#include "server/job/ExitStatus.h"
#include "server/mux/fwd.h"
#include "server/fwd.h"

#include <db/include/api/job/types.h>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace runjob {
namespace server {
namespace job  {

/*!
 * \brief Named parameter idiom to destroy a job
 */
class Destroy : public boost::enable_shared_from_this<Destroy>
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<Destroy> Ptr;

public:
    /*!
     * \brief factory
     */
    static Ptr create(
            const boost::weak_ptr<Server>& server,  //!< [in]
            const ExitStatus&                       //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~Destroy();

    Destroy* mux(
            const boost::weak_ptr<mux::Connection>&   //!< [in]
            );

    Destroy* client_id(
            const uint64_t    //!< [in]
            );
    
    Destroy* started(
            const bool    //!< [in]
            );
    
    Destroy* id(
            const BGQDB::job::Id    //!< [in]
            );

    Destroy* client_disconnected(
            const bool  //!< [in]
            );

private:
    Destroy(
            const boost::weak_ptr<Server>& server,
            const ExitStatus&
           );

    void callback();

private:
    BGQDB::job::Id _id;
    const ExitStatus _exitStatus;
    boost::weak_ptr<mux::Connection> _mux;
    uint64_t _client;
    bool _started;
    const boost::weak_ptr<Server> _server;
    bool _clientDisconnected;
};

} // job
} // server
} // runjob

#endif
