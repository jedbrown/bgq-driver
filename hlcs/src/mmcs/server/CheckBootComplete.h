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
 * \file CheckBootCompleteThread.h
 */

#ifndef MMCS_SERVER_CHECK_BOOT_COMPLETE_H_
#define MMCS_SERVER_CHECK_BOOT_COMPLETE_H_

#include "PerformanceCounters.h"

#include "common/Thread.h"
#include "types.h"

#include "libmmcs_client/CommandReply.h"

#include <set>
#include <string>
#include <vector>

namespace mmcs {
namespace server {

class CheckBootComplete : public common::Thread
{
    friend class DBBlockController;
public:
    CheckBootComplete(
            DBBlockController* b
            );

    void* threadStart();

private:
    PerformanceCounters::Container::Timer::Ptr startCounter();

    void markNodes(
            const std::set<std::string>& all_nodes
            );
private:
    DBBlockController* const _block;
    BlockControllerTarget* _target;
};

} } // namespace mmcs::server

#endif
