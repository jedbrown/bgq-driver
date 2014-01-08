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
#ifndef RUNJOB_SERVER_SERVER_H
#define RUNJOB_SERVER_SERVER_H

#include "common/fwd.h"

#include "server/block/fwd.h"
#include "server/job/fwd.h"
#include "server/mux/fwd.h"
#include "server/realtime/fwd.h"
#include "server/sim/ChildHandler.h"
#include "server/fwd.h"

#include <boost/asio/io_service.hpp>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace runjob {
namespace server {

namespace database {

class Init;

} // database

namespace performance {

class Counters;

} // performance

namespace sim {

class SharedMemory;

} // sim

/*!
 * \brief Server objects used throughout runjob_server.
 */
class Server : public boost::enable_shared_from_this<Server>
{
public:
    /*!
     * \brief Pointer type
     */
    typedef boost::shared_ptr<Server> Ptr;
    
    /*!
     * \brief Pointer type
     */
    typedef boost::weak_ptr<Server> WeakPtr;

    /*!
     * \brief Factory.
     */
    static Ptr create(
            const Options& options                 //!< [in]
            )
    {
        return Ptr( new Server(options) );
    }

public:
    /*!
     * \brief dtor.
     */
    ~Server();

    /*!
     * \brief start server.
     */
    int start();

    /*!
     * \brief Get I/O service.
     */
    boost::asio::io_service& getIoService() { return _io_service; }

    /*!
     * \brief Get Options.
     */
    const Options& getOptions() const { return _options; }

    /*!
     * \brief Get BlockMap.
     */
    const boost::shared_ptr<block::Container>& getBlocks() { return _blocks; }

    /*!
     * \brief Get ConnectionContainer.
     */
    const boost::shared_ptr<ConnectionContainer>& getConnections() { return _connections; }

    /*!
     * \brief Get job container.
     */
    const boost::shared_ptr<job::Container>& getJobs() { return _jobs; }

    /*!
     * \brief Get Database.
     */
    const boost::shared_ptr<database::Init>& getDatabase() { return _database; }

    /*!
     * \brief Get SharedMemory.
     */
    const boost::shared_ptr<sim::SharedMemory>& getSimCounter() { return _simCounter; }

    /*!
     * \brief Get Performance Counters.
     */
    const boost::shared_ptr<performance::Counters>& getPerformanceCounters() { return _performanceCounters; }

    /*!
     * \brief Get Security.
     */
    const boost::shared_ptr<Security>& getSecurity() { return _security; }
   
    /*!
     * \brief Get mux listener.
     */
    const boost::shared_ptr<mux::Listener>& getMuxListener() { return _mux; }

    /*!
     * \brief Get realtime connection.
     */
    const boost::shared_ptr<realtime::Connection>& getRealtimeConnection() { return _realtime; }

    /*!
     * \brief
     */
    bool stopped() const { return _stopped; }

private:
    /*!
     * \brief ctor.
     */
    Server(
            const Options& options                  //!< [in]
          );

private:
    friend class Shutdown;
    boost::asio::io_service _io_service;                            //!<
    bool _stopped;                                                  //!<
    const Options& _options;                                        //!<
    boost::shared_ptr<SignalHandler> _signalHandler;                //!<
    sim::ChildHandler _childHandler;                                //!<
    boost::shared_ptr<CommandListener> _command;                    //!<
    boost::shared_ptr<mux::Listener> _mux;                          //!<
    boost::shared_ptr<database::Init> _database;                    //!<
    boost::shared_ptr<performance::Counters> _performanceCounters;  //!<
    boost::shared_ptr<job::Container> _jobs;                        //!<
    boost::shared_ptr<block::Container> _blocks;                    //!<
    boost::shared_ptr<ConnectionContainer> _connections;            //!<
    boost::shared_ptr<sim::SharedMemory> _simCounter;               //!<
    boost::shared_ptr<Security> _security;                          //!<
    boost::shared_ptr<realtime::Connection> _realtime;              //!<
};

} // server
} // runjob

#endif
