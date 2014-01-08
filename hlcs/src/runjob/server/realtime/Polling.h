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
#ifndef RUNJOB_SERVER_REALTIME_POLLING_H
#define RUNJOB_SERVER_REALTIME_POLLING_H

#include "common/error.h"

#include "server/block/Container.h"
#include "server/fwd.h"

#include <hlcs/include/bgsched/types.h>

#include <db/include/api/cxxdb/fwd.h>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/utility.hpp>
#include <boost/weak_ptr.hpp>

namespace runjob {
namespace server {
namespace realtime {

/*!
 * \brief Poll for block status changes that have happened since a provided sequence ID
 *
 * \see EventHandler
 *
 * \note noncopyable because destructor does actual work
 */
class Polling : public boost::enable_shared_from_this<Polling>, private boost::noncopyable
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Polling> Ptr;

    /*!
     * \brief
     */
    static const unsigned int DefaultInterval;

    /*!
     * \brief
     */
    typedef boost::function<void(bgsched::SequenceId)> Callback;

public:
    /*!
     * \brief Factory.
     */
    static Ptr create(
            const boost::shared_ptr<Server>& server,    //!< [in]
            bgsched::SequenceId sequence                //!< [in]
            );

    /*!
     * \brief
     */
    void stop(
            const Callback& callback    //!< [in]
            );

    /*!
     * \brief
     */
    void wait();

    /*!
     * \brief dtor.
     */
    ~Polling();

private:
    Polling(
            const boost::shared_ptr<Server>& server,
            bgsched::SequenceId sequence
           );

    cxxdb::QueryStatementPtr prepareSequenceQuery(
            const cxxdb::ConnectionPtr& database
            );

    cxxdb::QueryStatementPtr prepareDeletionQuery(
            const cxxdb::ConnectionPtr& database
            );

    void impl(
            const boost::system::error_code& error
            );

    void getBlocksCallback(
            const block::Container::Blocks& blocks,
            const cxxdb::ConnectionPtr& database
            );

    void createCallback(
            const std::string& block,
            const std::string& status,
            error_code::rc error,
            const std::string& message
            );
    
    void removeCallback(
            const std::string& block,
            error_code::rc error,
            const std::string& message
            ) const;
    
    void initializedCallback(
            const std::string& block,
            error_code::rc error,
            const std::string& message
            ) const;

private:
    const boost::weak_ptr<Server> _server;
    boost::asio::deadline_timer _timer;
    bool _stopped;
    bgsched::SequenceId _sequence;
    Callback _callback;
};

} // realtime
} // server
} // runjob

#endif
