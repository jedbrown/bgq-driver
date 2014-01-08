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

#ifndef BGAGENT_MASTER_CONNECTION_H
#define BGAGENT_MASTER_CONNECTION_H

#include <utility/include/portConfiguration/PortConfiguration.h>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

class Agent;

class MasterConnection : public boost::enable_shared_from_this<MasterConnection>
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<MasterConnection> Ptr;

public:
    /*!
     * \brief Factory.
     */
    static void create(
            boost::asio::io_service& io_service,                    //!< [in]
            const bgq::utility::PortConfiguration::Pairs& ports,    //!< [in]
            Agent* const agent                                      //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~MasterConnection();

private:
    MasterConnection(
            boost::asio::io_service& io_service,
            const bgq::utility::PortConfiguration::Pairs& ports,
            Agent* const agent
            );

    void impl(
            const bgq::utility::PortConfiguration::Pairs::const_iterator& port,
            const unsigned attempts
            );

    void read();

    void handleRead(
            const boost::system::error_code& error
            );

    void handleWait(
            const boost::system::error_code& error,
            bgq::utility::PortConfiguration::Pairs::const_iterator port,
            unsigned attempts
            );

private:
    const bgq::utility::PortConfiguration::Pairs _ports;
    Agent* const _agent;
    boost::asio::posix::stream_descriptor _master;
    boost::asio::deadline_timer _timer;
};

#endif
