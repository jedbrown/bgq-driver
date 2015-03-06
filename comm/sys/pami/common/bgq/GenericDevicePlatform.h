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
 * \file common/bgq/GenericDevicePlatform.h
 * \brief ???
 */

#ifndef __common_bgq_GenericDevicePlatform_h__
#define __common_bgq_GenericDevicePlatform_h__

#include "util/queue/Queue.h"
#include "util/queue/MutexedQueue.h"
#include "util/queue/MultiQueue.h"

#include "components/atomic/indirect/IndirectCounter.h"
#include "components/atomic/indirect/IndirectMutex.h"
#include "components/atomic/native/NativeCounter.h"
typedef PAMI::Counter::Native GenericDeviceCounter;

//#include "components/atomic/pthread/Pthread.h"
//typedef PAMI::Counter::Pthread GenericDeviceCounter;

#include "util/queue/Queue.h"
#include "util/queue/MutexedQueue.h"
#include "util/queue/MultiQueue.h"
#include "util/queue/bgq/ArrayBasedQueue.h"

/// \brief Queue type to use for messages
#define GENDEVMSG_SEND_QNUM	0
#define GENDEVMSG_COMPL_QNUM	1
#define GENDEVMSG_NUM_QUEUES	2

/// \brief The type of queue used to implement generic device completion queue(s)
///
/// Must be compatible with GenericDeviceMessageQueue.
typedef PAMI::MultiQueue<GENDEVMSG_NUM_QUEUES,GENDEVMSG_COMPL_QNUM>	GenericDeviceCompletionQueue;

/// \brief The type of queue used to implement device send queue(s)
///
/// Must be compatible with GenericDeviceCompletionQueue.
typedef PAMI::MultiQueue<GENDEVMSG_NUM_QUEUES,GENDEVMSG_SEND_QNUM>	GenericDeviceMessageQueue;

/// \brief Queue type to use for generic device threads (work) queue(s)
typedef PAMI::Queue		GenericDeviceWorkQueue;
#include "components/atomic/bgq/L2Mutex.h"
#if 1

#include "util/queue/bgq/ArrayBasedPostingQueue.h"
typedef PAMI::ArrayBasedPostingQueue<PAMI::Mutex::BGQ::L2, PAMI::Queue>	GenericDevicePostingQueue;

#else

#include "util/queue/GccThreadSafePostingQueue.h"
typedef PAMI::GccThreadSafePostingQueue<PAMI::MutexedQueue<PAMI::Mutex::BGQ::IndirectL2> >	GenericDevicePostingQueue;

#endif

#endif // __common_bgq_GenericDevicePlatform_h__
