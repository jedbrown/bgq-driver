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

#ifndef MASTER_SUBNET_WAITER_H
#define MASTER_SUBNET_WAITER_H

#include "common/Thread.h"

#include "master/lib/BGMasterClient.h"
#include "master/common/BinaryController.h"

#include <string>

namespace mmcs {
namespace server {
namespace master {

// Waits for a specific subnet_mc to end
class SubnetWaiter : public common::Thread
{
public:
    SubnetWaiter() : Thread() {}
    void* threadStart();
    void setBin(BinaryControllerPtr bin) { _bin_to_wait = bin; }
    void setAlias(const std::string& alias) { _alias = alias; }
private:
    BinaryControllerPtr _bin_to_wait;
    std::string _alias;
};

} } } // namespace mmcs::server::master

#endif
