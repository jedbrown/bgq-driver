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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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
 * \file MailboxMonitor.h
 */

#ifndef CHECK_BOOT_COMPLETE_H_
#define CHECK_BOOT_COMPLETE_H_

#include "PerformanceCounters.h"
#include "MMCSThread.h"
#include "BlockControllerTarget.h"

class DBBlockController;

class CheckBootComplete : public MMCSThread
{
    friend class DBBlockController;
public:
    CheckBootComplete(DBBlockController* b);
    void* threadStart();

private:
    PerformanceCounters::Container::Timer::Ptr startCounter();
    void markNodes(BlockPtr pBlock, MMCSCommandReply& reply,
                   std::vector<std::string>& bad_node_locs,
                   std::vector<std::string>& all_nodes);
private:
    DBBlockController* const pBlock;
    BlockControllerTarget* _target;
};

#endif
