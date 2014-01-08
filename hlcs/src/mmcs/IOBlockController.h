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

#ifndef IO_BLOCK_CONTROLLER_H
#define IO_BLOCK_CONTROLLER_H

#include "BlockControllerBase.h"

#include <deque>

class IOBlockController : public BlockControllerBase
{
    friend class MailboxMonitor;
    friend class MailboxListener;
    map<std::string, IOBoardBitset>   _rackbits;                      // Bitset map rack location to corresponding bitset

public:
    /*!
     * \brief ctor.
     */
    IOBlockController(
            BGQMachineXML* machine,         //!< [in] machine description
            const std::string& userName,    //!< [in] user name
            const std::string& blockName,    //!< [in] block name
            bool delete_machine
            );
    ~IOBlockController();

    void			create_block(deque<string> args, MMCSCommandReply& reply, std::istream* blockStreamXML=NULL);
    void			boot_block(deque<string> args, MMCSCommandReply& reply, const NodeCustomization& ncfg = NodeCustomization(), PerformanceCounters::Timer::Ptr = PerformanceCounters::Timer::Ptr() );
    void                        build_shutdown_req(MCServerMessageSpec::ShutdownBlockRequest& mcShutdownBlockRequest,
                                                   MCServerMessageSpec::ShutdownBlockReply& mcShutdownBlockReply,
                                                   MMCSCommandReply& reply);
    void			reboot_nodes(BlockControllerTarget* pTarget, deque<string> args, MMCSCommandReply& reply);
    void			shutdown_block(MMCSCommandReply& reply, BlockControllerTarget* target, bool no_sysrq);
    void            show_barrier(MMCSCommandReply& reply);
    void			mcserver_connect(MMCSCommandReply& reply) { BlockControllerBase::mcserver_connect(_mcServer, _userName, reply); }
    void                        connect(deque<string> args, MMCSCommandReply& reply, BlockControllerTarget* pTarget, bool add_targets = false);
    void			disconnect(deque<string>& args);
    bool            processRASMessage(RasEvent& rasEvent);
    virtual void            setReconnected() { _isstarted = true; }
protected:

};

typedef boost::shared_ptr<IOBlockController> IOBlockPtr;
#endif

