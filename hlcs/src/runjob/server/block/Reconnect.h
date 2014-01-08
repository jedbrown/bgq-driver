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
#ifndef RUNJOB_SERVER_BLOCK_RECONNECT_H
#define RUNJOB_SERVER_BLOCK_RECONNECT_H

#include "common/error.h"

#include "server/block/fwd.h"

#include "server/fwd.h"

#include <bgsched/types.h>
#include <db/include/api/cxxdb/cxxdb.h>

#include <boost/asio/strand.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <vector>
#include <string>

class BGQMachineXML;

namespace runjob {
namespace server {
namespace block {

/*!
 * \brief %Reconnect to initialized blocks by querying the database.
 *
 * \see Container for more information about why a single machine XML description is used 
 * for all reconnected blocks
 *
 * \note noncopyable because destructor does actual work
 */
class Reconnect : public boost::enable_shared_from_this<Reconnect>, private boost::noncopyable
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<Reconnect> Ptr;

    /*!
     * \brief creation callback type.
     */
    typedef boost::function<void(const bgsched::SequenceId)> Callback;
    
    /*!
     * \brief Factory.
     *
     * Callback will be invoked after loading the machine XML and querying
     * for the highest squence ID in the bgqblock table.
     */
    static void create(
            const boost::shared_ptr<Server>& server,    //!< [in]
            const Callback& callback                    //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~Reconnect();

private:
    Reconnect(
            const boost::shared_ptr<Server>& server
            );

    void start();

    void createCallback(
            error_code::rc error,
            const std::string& message,
            const std::string& name,
            size_t numionodes,
            size_t numcnodes,
            const std::string& status
            );

    void findCallback(
            const boost::shared_ptr<block::Compute>& block,
            const std::string& name,
            size_t numcnodes
            );
    
    void initializedCallback(
            const std::string& name,
            error_code::rc error,
            const std::string& message
            );
    
    void nodeSoftwareFailures(
            const boost::shared_ptr<block::Compute>& block,
            const cxxdb::ResultSetPtr& results
            );
    
    void midplaneSoftwareFailures(
            const boost::shared_ptr<block::Compute>& block
            );

    void loadMachineCallback(
            const boost::shared_ptr<BGQMachineXML>& machine,
            const error_code::rc error,
            const Callback& callback
            );

private:
    boost::weak_ptr<Server> _server;
    const cxxdb::ConnectionPtr _connection;
    cxxdb::ResultSetPtr _results;
    boost::asio::io_service::strand _strand;        //!< protect access to database queries
    cxxdb::QueryStatementPtr _smallBlock;
    cxxdb::QueryStatementPtr _largeBlockNodes;
    cxxdb::QueryStatementPtr _largeBlockMidplanes;
    boost::shared_ptr<BGQMachineXML> _machine;
    size_t _ioBlockCount;
    size_t _computeBlockCount;
    boost::posix_time::ptime _startTime;
    int64_t _sequence;
};

} // block
} // server
} // runjob

#endif
