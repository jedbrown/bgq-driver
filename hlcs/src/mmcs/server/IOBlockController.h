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
 * \file IOBlockController.h
 */

#ifndef MMCS_SERVER_IO_BLOCK_CONTROLLER_H_
#define MMCS_SERVER_IO_BLOCK_CONTROLLER_H_

#include "BlockControllerBase.h"

#include "RackBitset.h"

#include "types.h"

#include <deque>
#include <map>
#include <string>

namespace mmcs {
namespace server {

class IOBlockController : public BlockControllerBase
{
public:
    /*!
     * \brief ctor.
     */
    IOBlockController(
            BGQMachineXML* machine,         //!< [in] machine description
            const std::string& userName,    //!< [in] user name
            const std::string& blockName,   //!< [in] block name
            bool delete_machine
            );

    ~IOBlockController();

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
            MCServerMessageSpec::ShutdownBlockRequest& mcShutdownBlockRequest
            );

    void reboot_nodes(
            BlockControllerTarget* pTarget,
            std::deque<std::string> args,
            mmcs_client::CommandReply& reply
            );

    void shutdown_block(
            mmcs_client::CommandReply& reply,
            const std::deque<std::string>& args
            );

    void show_barrier(
            mmcs_client::CommandReply& reply
            );

    void connect(
            std::deque<std::string> args,
            mmcs_client::CommandReply& reply,
            const BlockControllerTarget* pTarget
            );

    void disconnect(
            const std::deque<std::string>& args,
            mmcs_client::CommandReply& reply
            );

    bool processRASMessage(
            RasEvent& rasEvent
            );

    void setReconnected() { _isstarted = true; }

private:
    void calculateRatios();

private:
    std::map<std::string, IOBoardBitset> _rackbits;
    std::string _bootOptions;
};

} } // namespace mmcs::server

#endif
