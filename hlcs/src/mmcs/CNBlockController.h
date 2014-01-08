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

#ifndef CN_BLOCK_CONTROLLER_H
#define CN_BLOCK_CONTROLLER_H

#include "BlockControllerBase.h"

#include <deque>

class CNBlockController : public BlockControllerBase
{
    friend class MailboxMonitor;
    friend class MailboxListener;
    map<std::string, RackBitset>   _rackbits;                      // Bitset map rack location to corresponding bitset
    map<std::string, IOBoardBitset>   _ioboardbits;                // Bitset map for connected IO
    bool _io_notified;

public:
    /*!
     * \brief ctor.
     */
    CNBlockController(
            BGQMachineXML* machine,         //!< [in] machine description
            const std::string& username,    //!< [in] user name
            const std::string& blockname,    //!< [in] block name
            bool delete_machine
            );

    bool                    process_midplane(BGQMidplaneNodeConfig& midplane, MMCSCommandReply& reply, bool passthru = false);
    void                    create_block(deque<string> args, MMCSCommandReply& reply, std::istream* blockStreamXML=NULL);
    virtual void            boot_block(deque<string> args, MMCSCommandReply& reply, const NodeCustomization& ncfg = NodeCustomization(), PerformanceCounters::Timer::Ptr = PerformanceCounters::Timer::Ptr()  );
    virtual void            build_shutdown_req(MCServerMessageSpec::ShutdownBlockRequest& mcShutdownBlockRequest,
                                               MCServerMessageSpec::ShutdownBlockReply& mcShutdownBlockReply,
                                               MMCSCommandReply& reply);
    virtual void            reboot_nodes(BlockControllerTarget* pTarget, deque<string> args, MMCSCommandReply& reply);
    bool notify_io(MCServerMessageSpec::NotifyIoRequest& mcNotifyIoRequest, MCServerMessageSpec::NotifyIoReply& mcNotifyIoReply,
                   MMCSCommandReply& reply, std::vector<BCNodeInfo*>& targetargs);
    virtual void            shutdown_block(MMCSCommandReply& reply, BlockControllerTarget* target, bool no_sysrq);
    virtual void            show_barrier(MMCSCommandReply& reply);
    virtual void            connect(deque<string> args, MMCSCommandReply& reply, BlockControllerTarget* pTarget, bool add_targets = false);
    virtual void            disconnect(deque<string>& args);
    virtual bool            processRASMessage(RasEvent& rasEvent);
    virtual void            setReconnected() { _isstarted = true; _io_notified = true; }
};

typedef boost::shared_ptr<CNBlockController> CNBlockPtr;

#endif
