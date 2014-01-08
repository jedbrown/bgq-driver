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
#ifndef RUNJOB_SERVER_JOB_CONTAINER_H
#define RUNJOB_SERVER_JOB_CONTAINER_H

#include "common/Uci.h"

#include "server/block/fwd.h"

#include "server/database/fwd.h"

#include "server/job/Create.h"

#include "server/mux/fwd.h"

#include "server/performance/Counters.h"

#include "server/fwd.h"

#include <db/include/api/job/types.h>

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

#include <stdint.h>

namespace runjob {
namespace server {
namespace job {

/*!
 * \brief container of Job objects.
 *
 * \see job
 */
class Container
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Container> Ptr;

    /*!
     * \brief Job container
     */
    typedef boost::unordered_map< BGQDB::job::Id, boost::shared_ptr<Job> > Jobs;

    /*!
     * \brief Callback for find method.
     */
    typedef boost::function< void( const boost::shared_ptr<Job>& ) > FindCallback;

    /*!
     * \brief Callback for the get method.
     */
    typedef boost::function< void( const Jobs& blocks ) > GetCallback;

public:
    /*!
     * \brief ctor.
     */
    Container(
            const boost::shared_ptr<Server>& server     //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~Container();

    /*!
     * \brief Add a job.
     */
    void add(
            const boost::shared_ptr<Job>& job,                                  //!< [in]
            const Create::Callback& callback,                                   //!< [in]
            size_t nodesUsed,                                                   //!< [in]
            const performance::Counters::JobContainer::Timer::Ptr& arbitration, //!< [in]
            const performance::Counters::JobContainer::Timer::Ptr& security     //!< [in]
            );

    /*!
     * \brief remove an entry.
     */
    void remove(
            BGQDB::job::Id id   //!< [in]
            );

    /*!
     * \brief Find a job.
     */
    void find(
            BGQDB::job::Id id,              //!< [in]
            const FindCallback& callback    //!< [in]
            );

    /*!
     * \brief
     */
    void get(
            const GetCallback& callback  //!< [in]
            );

    /*!
     * \brief Handle end-of-file from an I/O node.
     */
    void eof(
            const Uci& location    //!< [in]
            );

    /*!
     * \brief
     */
    void eof(
            const boost::shared_ptr<block::Compute>& block  //!< [in]
            );

private:
    void addImpl(
            const boost::shared_ptr<Job>& job,
            const Create::Callback& callback,
            size_t nodesUsed,
            const performance::Counters::JobContainer::Timer::Ptr& arbitration,
            const performance::Counters::JobContainer::Timer::Ptr& security
            );
    
    void removeImpl(
            BGQDB::job::Id id
            );

    void findImpl(
            BGQDB::job::Id id,
            const FindCallback& callback
            );

    void getImpl(
            const GetCallback& callback
            );

    void blockEofImpl(
            const boost::shared_ptr<block::Compute>& block
            );

    void ioEofImpl(
            const Uci& location
            );

private:
    const Options& _options;                                        //!< program options
    boost::asio::io_service& _io_service;                           //!< asio service
    boost::asio::strand _strand;                                    //!< asio strand for container access
    Jobs _jobs;                                                     //!< container
    boost::shared_ptr<database::Init> _database;                    //!<
    const performance::Counters::JobContainer::Ptr _counters;       //!<
};

} // job
} // server
} // runjob

#endif
