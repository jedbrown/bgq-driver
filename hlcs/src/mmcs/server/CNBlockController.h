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
/* (C) Copyright IBM Corp.  2009, 2011                              */
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

/*!
 * \file CNBlockController.h
 */

#ifndef MMCS_SERVER_CN_BLOCK_CONTROLLER_H
#define MMCS_SERVER_CN_BLOCK_CONTROLLER_H


#include "BlockControllerBase.h"

#include "RackBitset.h"

#include "libmmcs_client/CommandReply.h"

#include <boost/shared_ptr.hpp>

#include <deque>
#include <string>
#include <map>


namespace mmcs {
namespace server {


class CNBlockController : public BlockControllerBase
{
    std::map<std::string, RackBitset>   _rackbits;                      // Bitset map rack location to corresponding bitset
    bool _io_notified;

public:
    /*!
     * \brief ctor.
     */
    CNBlockController(
            BGQMachineXML* machine,         //!< [in] machine description
            const std::string& username,    //!< [in] user name
            const std::string& blockname,   //!< [in] block name
            bool delete_machine
            );

    bool process_midplane(
            BGQMidplaneNodeConfig& midplane,
            mmcs_client::CommandReply& reply,
            bool passthru = false
            );

    void create_block(
            std::deque<std::string> args,
            mmcs_client::CommandReply& reply,
            std::istream* blockStreamXML=NULL
            );

    void boot_block(
            std::deque<std::string> args,
            mmcs_client::CommandReply& reply,
            const NodeCustomization& ncfg = NodeCustomization(),
            PerformanceCounters::Timer::Ptr = PerformanceCounters::Timer::Ptr()
            );

    void build_shutdown_req(
            MCServerMessageSpec::ShutdownBlockRequest& mcShutdownBlockRequest,
            MCServerMessageSpec::ShutdownBlockReply& mcShutdownBlockReply,
            mmcs_client::CommandReply& reply
            );

    bool notify_io(
            MCServerMessageSpec::NotifyIoRequest& mcNotifyIoRequest,
            MCServerMessageSpec::NotifyIoReply& mcNotifyIoReply,
            mmcs_client::CommandReply& reply,
            std::vector<BCNodeInfo*>& targetargs
            );

    void shutdown_block(
            mmcs_client::CommandReply& reply,
            BlockControllerTarget* target
            );

    void show_barrier(
            mmcs_client::CommandReply& reply
            );

    void connect(
            std::deque<std::string> args,
            mmcs_client::CommandReply& reply,
            const BlockControllerTarget* pTarget,
            bool add_targets = false
            );

    void disconnect(
            const std::deque<std::string>& args,
            mmcs_client::CommandReply& reply
            );

    bool processRASMessage(
            RasEvent& rasEvent
            );

    void setReconnected() { _isstarted = true; _io_notified = true; }
};

typedef boost::shared_ptr<CNBlockController> CNBlockPtr;


} } // namespace mmcs::server

#endif
