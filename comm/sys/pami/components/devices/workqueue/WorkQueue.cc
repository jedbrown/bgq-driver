/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q 5765-PER 5765-PRP                                    */
/*                                                                  */
/* (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/* US Government Users Restricted Rights -                          */
/* Use, duplication, or disclosure restricted                       */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/**
 * \file components/devices/workqueue/WorkQueue.cc
 * \brief This file ensures that the VTABLE exists for anything using these classes...
 */

#include "util/common.h"
#include "components/devices/workqueue/WorkQueue.h"
#include "components/devices/workqueue/SharedWorkQueue.h"
#include "components/devices/workqueue/MemoryWorkQueue.h"

namespace PAMI
{
  namespace Device
  {
    namespace WorkQueue
    {
      WorkQueue::~WorkQueue() { }
      NonVirtWorkQueue::~NonVirtWorkQueue() { }
      SharedWorkQueue::~SharedWorkQueue() { }
      MemoryWorkQueue::~MemoryWorkQueue() { }
    };
  };
};
