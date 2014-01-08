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
/* (C) Copyright IBM Corp.  2011, 2011                              */
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
#ifndef RUNJOB_SERVER_JOB_SUB_NODE_PACING_H
#define RUNJOB_SERVER_JOB_SUB_NODE_PACING_H

#include "server/cios/Connection.h"

#include "server/block/fwd.h"

#include "server/fwd.h"

#include <boost/thread/mutex.hpp>

#include <queue>

namespace runjob {
namespace server {
namespace job {

/*!
 * \brief
 */
class SubNodePacing
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<SubNodePacing> Ptr;

public:
    /*!
     * \brief ctor.
     */
    explicit SubNodePacing();

    /*!
     * \brief
     */
    void add(
            const boost::shared_ptr<cios::Message>& message,
            const boost::shared_ptr<Job>& job
            );

    /*!
     * \brief
     */
    void next();

private:
    void write();

private:
    typedef std::pair< boost::weak_ptr<Job>, boost::shared_ptr<cios::Message> > Entry;
    typedef std::deque<Entry> Outbox;

private:
    boost::mutex _mutex;
    Outbox _outbox;
};

} // job
} // server
} // runjob

#endif
