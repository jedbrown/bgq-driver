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

#include <deque>
#include <tr1/memory>
#include <boost/shared_ptr.hpp>
#include <bgq_util/include/pthreadmutex.h>
#include "MMCSCommandReply.h"

#ifndef _BLOCK_HELPER_H
#define _BLOCK_HELPER_H

class BlockControllerBase;
class RasEvent;

namespace MCServerMessageSpec {
class ConsoleMessage;
}

typedef boost::shared_ptr<BlockControllerBase> BlockPtr;

class BlockHelper {
protected:
    BlockPtr _base;
public:
    BlockHelper(BlockControllerBase* b);
    BlockHelper(BlockPtr b);
    void setBase(BlockPtr b);
    BlockPtr& getBase() { return _base; }
    PthreadMutex& getMutex();
    virtual void initMachineConfig(MMCSCommandReply& reply) { reply << OK << DONE; }
    virtual int processRASMessage(RasEvent& rasEvent) { return false; };
    virtual void postProcessRASMessage(int recid) { return; };
    virtual void processConsoleMessage(MCServerMessageSpec::ConsoleMessage& consoleMessage) {}
    virtual void setAllocateStartTime(time_t& time) {}
    virtual ~BlockHelper() {}
};


#endif
