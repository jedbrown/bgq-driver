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

#ifndef MASTER_ALIAS_WAITER_H
#define MASTER_ALIAS_WAITER_H

#include "common/Thread.h"

#include "master/lib/BGMasterClient.h"

#include <control/include/mcServer/MCServerRef.h>

#include <string>

namespace mmcs {
namespace server {
namespace master {

// Waits for an alias to start
class AliasWaiter : public common::Thread
{
public:
    AliasWaiter(const std::string& alias, bool update_mc = false);
    void* threadStart();

    static bool sendInitialize(
            MCServerRefPtr ref,
            std::vector<std::string>& hw_to_unmark,
            MCServerMessageSpec::FailoverReply& failrep
            );

    // This guy gets everything in the fail reply and removes it from the booted hardware
    // and puts it in the idle hardware.
    static bool actuallySendMessages(
            MCServerRefPtr ref,
            const MCServerMessageSpec::FailoverReply& failrep,
            std::vector<std::string>& markAvail
            );

    static bool buildFailover(
            MCServerMessageSpec::FailoverRequest& failreq,
            const std::string& subnet_mc,
            const BinaryId& binid
            );

    void updateMcServer(
            const std::string& subnet_mc,
            std::vector<std::string>& blocks_to_free,
            std::vector<std::string>& error_list,
            const BinaryId& binid
            );

    const BinaryId& getBinId() const { return _binid; }

private:
    const std::string _alias;
    const bool _update_mc;
    BinaryId _binid;
};

} } } // namespace mmcs::server::master

#endif
