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
#ifndef RUNJOB_SERVER_SHUTDOWN_H
#define RUNJOB_SERVER_SHUTDOWN_H

#include "server/block/Container.h"

#include "server/cios/Connection.h"

#include "server/job/Container.h"

#include "server/fwd.h"

#include <boost/enable_shared_from_this.hpp>

#include <vector>

namespace runjob {
namespace server {

/*!
 * \brief
 */
class Shutdown : public boost::enable_shared_from_this<Shutdown>
{
public:
    /*!
     * \brief Container of I/O links
     */
    typedef std::vector<cios::Connection::Ptr> IoLinks;

public:
    /*!
     * \brief Factory.
     */
    static void create(
            const boost::shared_ptr<Server>& server
            );

private:
    Shutdown(
            const boost::shared_ptr<Server>& server
            );

    void getBlocksHandler(
            const block::Container::Blocks& blocks
            );

    void getJobsHandler(
            const job::Container::Jobs& jobs
            );

    void stopLinkHandler();

    void removeJobHandler();

    void stopIoService();

private:
    const boost::weak_ptr<Server> _server;
    IoLinks _links;
    IoLinks::iterator _currentLink;
    job::Container::Jobs _jobs;
    job::Container::Jobs::iterator _currentJob;
};

} // server
} // runjob

#endif
