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
 * \file common/bgp/GenericDevicePlatform.h
 * \brief ???
 */

#ifndef __common_bgp_GenericDevicePlatform_h__
#define __common_bgp_GenericDevicePlatform_h__

#include "spi/kernel_interface.h"
#include "components/atomic/bgp/LockBoxMutex.h"
#include "components/atomic/bgp/LockBoxCounter.h"
#include "components/atomic/native/NativeCounter.h"
#include "util/queue/Queue.h"
#include "util/queue/MutexedQueue.h"
#include "util/queue/MultiQueue.h"

/// \brief The atomic counter used by threads for tracking completion of message
typedef PAMI::Counter::Native GenericDeviceCounter;
//typedef PAMI::Counter::BGP::LockBoxCounter GenericDeviceCounter;

#define GENDEVMSG_SEND_QNUM	0	///< queue number used for send queue
#define GENDEVMSG_COMPL_QNUM	1	///< queue number used for completion queue
#define GENDEVMSG_NUM_QUEUES	2	///< total number of queues used for a message

/// \brief The type of queue used to implement generic device completion queue(s)
///
/// Must be compatible with GenericDeviceMessageQueue.
typedef PAMI::MultiQueue<GENDEVMSG_NUM_QUEUES,GENDEVMSG_COMPL_QNUM>	GenericDeviceCompletionQueue;

/// \brief The type of queue used to implement device send queue(s)
///
/// Must be compatible with GenericDeviceCompletionQueue.
typedef PAMI::MultiQueue<GENDEVMSG_NUM_QUEUES,GENDEVMSG_SEND_QNUM>	GenericDeviceMessageQueue;

/// \brief Queue type to use for generic device threads (work) queue(s)
#include "util/queue/GccThreadSafePostingQueue.h"
#include "components/atomic/native/NativeMutex.h"
typedef PAMI::GccThreadSafePostingQueue<PAMI::MutexedQueue<PAMI::Mutex::Native> >	GenericDevicePostingQueue;
typedef PAMI::Queue									GenericDeviceWorkQueue;

#endif // __common_bgp_GenericDevicePlatform_h__
