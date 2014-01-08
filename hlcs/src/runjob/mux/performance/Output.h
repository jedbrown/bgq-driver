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
#ifndef RUNJOB_MUX_PERFORMANCE_OUTPUT_H
#define RUNJOB_MUX_PERFORMANCE_OUTPUT_H

#include <utility/include/Log.h>

#include "common/message/PerfCounters.h"

#include "common/logging.h"

#include "mux/server/Connection.h"

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

namespace runjob {
namespace mux {
namespace performance {

/*!
 * \brief Implements the \link bgq::utility::performance::StatisticSet StatisticSet\endlink log output policy.
 *
 * Sends all output to the runjob_server.
 */
class Output
{
public:
    /*!
     * \brief Set the server Connection.
     */
    void setConnection(
            const boost::shared_ptr<server::Connection>& server
            )
    {
        _server = server;
    }

protected:
    /*!
     * \brief ctor.
     */
    Output() :
        _msg(),
        _server()
    {

    }

    /*!
     * \brief Output the container.
     *
     * \pre _server has been set with setConnection.
     */
    template <typename StoragePolicy>
    void output(
            const std::string& /* type */,      //!< [in]
            StoragePolicy& storage              //!< [in]
            ) 
    {
        // get logger
        log4cxx::LoggerPtr log_logger_ = runjob::mux::getLogger();

        if ( !_server ) {
            LOG_WARN_MSG( "ServerConnection is not set" );
            return;
        }

        // create message if we don't have a previous one
        if ( !_msg ) {
            _msg.reset( new message::PerfCounters() );
        } else {
            LOG_TRACE_MSG( "using previous message with " << _msg->getData().size() << " counters" );
        }

        // get storage container
        typename StoragePolicy::Container container;
        storage.get( container );
        
        // iterate through container
        BOOST_FOREACH( const typename StoragePolicy::Container::value_type& i, container ) {
            // add to message
            _msg->addDataPoint( i );
        }

        // send to server
        if ( !container.empty() ) {
            LOG_TRACE_MSG( "sending " << container.size() << " counters" );
            _server->write(
                    _msg,
                    boost::bind(
                        &Output::writeHandler,
                        this,
                        _1
                        )
                    );
        }
    }

    /*!
     * \brief dtor.
     */
    ~Output()
    {

    }

private:
    void writeHandler(
            const boost::system::error_code& error
            ) 
    {
        // get logger
        log4cxx::LoggerPtr log_logger_ = runjob::mux::getLogger();

        if ( error ) {
            LOG_DEBUG_MSG( "server connection down" );
            return;
        }

        LOG_TRACE_MSG( "sent " << _msg->getData().size() << " counters to server" );

        // erase message since we sent it
        _msg.reset();
    }

private:
    message::PerfCounters::Ptr _msg;                    //!<
    boost::shared_ptr<server::Connection> _server;      //!<
};

} // performance
} // mux
} // runjob

#endif
