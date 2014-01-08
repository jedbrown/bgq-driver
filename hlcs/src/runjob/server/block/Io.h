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
#ifndef RUNJOB_SERVER_BLOCK_IO_H
#define RUNJOB_SERVER_BLOCK_IO_H

#include "server/block/fwd.h"
#include "server/block/SecurityKey.h"

#include "server/cios/fwd.h"

#include "server/database/fwd.h"

#include "server/job/fwd.h"

#include "server/Block.h"
#include "server/fwd.h"

#include <boost/asio/ip/address.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <string>
#include <vector>

class BGQIOBoardNodeConfig;
class BGQIONodePos;
class BGQNodeConfig;

namespace runjob {
namespace server {
namespace sim {

class Iosd;
class Inotify;

} // sim

namespace block {

/*!
 * \brief I/O block.
 *
 * This class has two modes of operation:
 *
 * - simulation
 * - production
 *
 * During simulation, it forks an I/O services daemon for each node in the block.
 * During production, it skips forking and connects to the daemons on the I/O nodes
 * directly.
 *
 * \see runjob::server::sim::Iosd
 * \see Container
 * \see Compute
 * \see IoNode
 * \see IoLink
 */
class Io : public Block, public boost::enable_shared_from_this<Io>
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<Io> Ptr;

    /*!
     * \brief container of IoLink objects.
     */
    typedef std::vector<IoLink> Links;

    /*!
     * \brief Container of I/O nodes.
     */
    typedef std::vector< boost::shared_ptr<IoNode> > Nodes;

public:
    /*!
     * \brief factory method.
     *
     * Creates an I/O connection to the stdio and jobctl daemons on each I/O node.
     *
     * \throws std::runtime_error if iosd_sim_path or iosd_sim_name are missing from the runjob.server
     * section of the properties file when job simulation is enabled.
     */
    static Ptr create(
            const std::string& name,                            //!< [in] block name
            const boost::shared_ptr<BGQBlockNodeConfig>& block, //!< [in] xml description
            const boost::shared_ptr<Server>& server             //!< [in] configuration objects
      );

    /*!
     * \brief dtor.
     */
    ~Io();

    /*!
     * \brief Get I/O node container.
     */
    const Nodes& getNodes() const { return _nodes; }

    /*!
     * \brief Get security key.
     */
    const SecurityKey& securityKey() const { return _securityKey; }

    /*!
     * \brief
     */
    void initialized();

    /*!
     * \brief
     */
    void terminating();

private:
    Io(
            const std::string& name,
            const boost::shared_ptr<BGQBlockNodeConfig>& block,
            const boost::shared_ptr<Server>& server
      );

    void init();

    void connect(
            const database::IoNodeIpAddress& ip,
            const std::string& location,
            const boost::shared_ptr<cios::Connection>& control,
            const boost::shared_ptr<cios::Connection>& data
            );

    boost::shared_ptr<sim::Inotify> setupSimulation();

    void handleNode(
            BGQIOBoardNodeConfig& board,
            const BGQIONodePos& io
            );

private:
    typedef boost::shared_ptr<sim::Iosd> IosdPtr;
    typedef std::vector<IosdPtr> Daemons;

private:
    boost::asio::io_service& _io_service;                           //!< asio service
    boost::asio::strand _strand;                                    //!< asio strand
    const Options& _options;                                        //!< program options
    boost::shared_ptr<job::Container> _jobs;                        //!< job map
    Nodes _nodes;                                                   //!< node container
    Daemons _daemons;                                               //!< simulated I/O daemons
    SecurityKey _securityKey;                                       //!<
    boost::weak_ptr<sim::Inotify> _iNotify;                         //!<
    std::string _iosdName;                                          //!<
    std::string _iosdPath;                                          //!<
    bool _initialized;                                              //!<
};

} // block
} // server
} // runjob

#endif
