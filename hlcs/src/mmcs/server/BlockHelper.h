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

#ifndef MMCS_SERVER_BLOCK_HELPER_H_
#define MMCS_SERVER_BLOCK_HELPER_H_

#include "types.h"

#include "libmmcs_client/CommandReply.h"

#include <bgq_util/include/pthreadmutex.h>

#include <xml/include/c_api/MCServerMessageSpec.h>

class RasEvent;

namespace mmcs {
namespace server {

class BlockHelper
{
private:
    const BlockPtr _base;
public:
    BlockHelper(BlockControllerBase* b);
    BlockHelper(BlockPtr b);
    const BlockPtr& getBase() const { return _base; }
    PthreadMutex& getMutex();
    virtual void initMachineConfig(mmcs_client::CommandReply& reply) { reply << mmcs_client::OK << mmcs_client::DONE; }
    virtual int processRASMessage(RasEvent& rasEvent) { return 0; };
    virtual void postProcessRASMessage(int recid) { return; };
    virtual void processConsoleMessage(MCServerMessageSpec::ConsoleMessage& consoleMessage) {}
    virtual void setAllocateStartTime(time_t time) {}
    virtual ~BlockHelper() {}
};

} } // namespace mmcs::server

#endif
