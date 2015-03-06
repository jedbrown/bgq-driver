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
 * \file common/socklinux/GenericDevicePlatform.h
 * \brief ???
 */

#ifndef __common_socklinux_GenericDevicePlatform_h__
#define __common_socklinux_GenericDevicePlatform_h__

#include "components/atomic/native/NativeCounter.h"
typedef PAMI::Counter::Native GenericDeviceCounter;
//#include "components/atomic/pthread/Pthread.h"
//typedef PAMI::Counter::Pthread GenericDeviceCounter;

#include "util/queue/Queue.h"
#include "util/queue/MultiQueue.h"

/// \brief Queue type to use for messages
#define GENDEVMSG_SEND_QNUM	0
#define GENDEVMSG_COMPL_QNUM	1
#define GENDEVMSG_NUM_QUEUES	2
typedef PAMI::MultiQueue<GENDEVMSG_NUM_QUEUES,GENDEVMSG_COMPL_QNUM>	GenericDeviceCompletionQueue;
typedef PAMI::MultiQueue<GENDEVMSG_NUM_QUEUES,GENDEVMSG_SEND_QNUM>	GenericDeviceMessageQueue;

/// \brief Queue type to use for threads (work)
#include "util/queue/GccThreadSafePostingQueue.h"
#include "util/queue/MutexedQueue.h"
#include "components/atomic/native/NativeMutex.h"
typedef PAMI::GccThreadSafePostingQueue<PAMI::MutexedQueue<PAMI::Mutex::Native> >	GenericDevicePostingQueue;
typedef PAMI::Queue					GenericDeviceWorkQueue;

#endif // __common_socklinux_GenericDevicePlatform_h__
