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
#ifndef __components_devices_generic_Device_h__
#define __components_devices_generic_Device_h__

///  \file components/devices/generic/Device.h
///  \brief Generic Device
///
///  The Generic classes implement a BaseDevice and a Message object
///  to post into the queueing system.  The GI device is currently
///  used to implement barriers, so the Generic device posts a message
///  and uses a interprocess communication sysdep to signal the Generic wire
///

/// \page use_gendev How to use the Generic Device
///
/// This chapter explains the basic requirements and features of the generic device.
///
/// \section use_gendev_basic Basic requirements and features of the Generic Device
///
/// The generic device implements a pair of queues for each context.
///
/// One queue holds thread objects, which each represent a until of work.
/// Each thread on the queue will have it's work function called when
/// the generic device slice (context) is advanced. Depending on the
/// thread status and work function return code, the thread may persist
/// on the queue or be removed. Thread objects are posted using
/// \ref PAMI::Device::Generic::Device::postThread "postThread()".
///
/// The other queue holds message objects. These objects are queued only
/// to be checked for completion. during advance, each object on this queue
/// has it's status checked, and if Done will be dequeued and the completion
/// callback invoked. Message are posted using
/// \ref PAMI::Device::Generic::Device::postMsg "postMsg()".
///
/// A user of the generic device may employ both message and threads,
/// or either one alone.
///
/// A typical usage is to create a message object with common data
/// and the completion callback, and then create one or more thread objects
/// with data members for the specific work assigned to the thread.
/// As each thread completes it's work it updates the message and when
/// last thread completes it will set the message status to Done.
/// The thread objects are posted to different contexts thereby achieving
/// parallelism.
///
/// All devices are given a pointer to the array of generic devices during
/// init(). By saving the pointer, any device may later post work to
/// any slice of the generic device.
///
/// \subsection use_gendev_syn SYNOPSIS
/// <div style="margin-left: 3em">
///
/// \#include "components/devices/generic/Device.h"
///
/// </div>
/// \subsection use_gendev_thr THREAD
/// <div style="margin-left: 3em">
///
/// All objects posted to the generic device via postThread() must
/// inherit from GenericThread. Each thread has a status which determines
/// how it is handled when on a generic device queue. The thread status
/// may change as a result of calling the work fuinction, but also may
/// be changed by third-parties, for example to terminate a thread without
/// the work function being called (again).
///
/// \ref PAMI::Device::ThreadStatus "GenericThread status values"
///
/// \ref PAMI::Device::Generic::GenericThread "class GenericThread"
///
/// \b Provides:
/// <div style="margin-left: 3em">
///
/// \ref PAMI::Device::Generic::GenericThread::getStatus "ThreadStatus getStatus()"
///
/// \ref PAMI::Device::Generic::GenericThread::setStatus "void setStatus(ThreadStatus stat)"
///
/// \ref PAMI::Device::Generic::GenericThread::setFunc "void setFunc(pami_work_function func, void *cookie)"
///
/// </div>
/// </div>
/// \subsection use_gendev_msg MESSAGE
/// <div style="margin-left: 3em">
///
/// All objects posted to the generic device via postMsg() must
/// inherit from GenericMessage.
///
/// \ref PAMI::Device::MessageStatus "GenericMessage status values"
///
/// \ref PAMI::Device::Generic::GenericMessage "class GenericMessage"
///
/// The generic device will check the status of each message on it's queue
/// and if a status is Done then will dequeue the message and call the
/// message completion callback. This also requires that the message support
/// the getQS() method to return the send queue on which the message is
/// (may be) queued. Note, this queue may never be used (always empty) but
/// it must be a valid queue. If the generic device finds another message
/// on this queue, then it will start that message using the postNext() method.
///
/// The message has a private interface known by the model and thread,
/// and possibly device.
///
/// \b Requires:
/// <div style="margin-left: 3em">
///
/// \ref GenericDeviceMessageQueue "typedef GenericDeviceMessageQueue"
///
/// \ref PAMI::Device::Generic::GenericMessage::postNext "virtual pami_context_t postNext(bool devQueued)"
///
/// </div>
/// \b Provides:
/// <div style="margin-left: 3em">
///
/// \ref PAMI::Device::Generic::GenericMessage::setStatus "void setStatus(MessageStatus status)"
///
/// \ref PAMI::Device::Generic::GenericMessage::getStatus "MessageStatus getStatus()"
///
/// \ref PAMI::Device::Generic::GenericMessage::getQS "GenericDeviceMessageQueue *getQS()"
///
/// \ref PAMI::Device::Generic::GenericMessage::getClientId "size_t getClientId()"
///
/// \ref PAMI::Device::Generic::GenericMessage::getContextId "size_t getContextId()"
///
/// \ref PAMI::Device::Generic::GenericMessage::executeCallback "void executeCallback(pami_context_t ctx, pami_result_t err = PAMI_SUCCESS)"
///
/// </div>
/// </div>
/// \subsection use_gendev_dev DEVICE
/// <div style="margin-left: 3em">
///
/// All devices that post work to the generic device must ensure that the
/// messages posted have a QS pointer (in GenericMessage ctor) which
/// points to a valid send queue. The send queue may never be used (always empty)
/// but it must exist.
///
/// The device has a private interface known by the model and message
/// (and possibly thread). The device must, however, implement the
/// \ref PAMI::Device::Interface::FactoryInterface "FactoryInterface"
/// and be instanciated in the
/// \ref PAMI::PlatformDeviceList "PlatformDeviceList"
/// of the client (defined in the context).
///
/// \b Requires:
/// <div style="margin-left: 3em">
///
/// \ref GenericDeviceMessageQueue "typedef GenericDeviceMessageQueue"
///
/// </div>
/// </div>
/// \subsection use_gendev_mdl MODEL
/// <div style="margin-left: 3em">
///
/// The model is typically driven by the defined interface for the model type.
/// For example, a multicombine model will implement the postMulticombine()
/// interface. The model constructor takes a device reference, client ID, and
/// context ID. The device reference may be for a single global device, a
/// device specific to the context, or some other arangement. Since the model
/// is intimately tied to the device, the model knows how to interact with
/// the device.
///
/// All details of the Message, Device, and Thread are hidden from the user of
/// the model, except that a reference to a (meaningful) device must be passed
/// to the model ctor, and the model must expose an integer constant "sizeof_msg"
/// which is the number of bytes required for the Message object.
///
/// </div>
/// \subsection use_gendev_use HOW TO
/// <div style="margin-left: 3em">
///
/// Steps to create a new Model/Device/Message/Thread set.
///
/// TBD...
///
/// </div>
/// <HR>

#include "components/memory/MemoryManager.h"
#include "WakeupManager.h"
#include "Global.h"
#include "components/devices/BaseDevice.h"
#include "components/devices/generic/AdvanceThread.h"
#include "components/devices/generic/Message.h"
#include "components/devices/FactoryInterface.h"
#include "components/atomic/MutexInterface.h"
#include <pami.h>

#ifndef PAMI_MAX_NUM_CLIENTS
/** \todo PAMI_MAX_NUM_CLIENTS needs to be setup by pami.h */
#define PAMI_MAX_NUM_CLIENTS  4
#endif // !PAMI_MAX_NUM_CLIENTS

#include "GenericDevicePlatform.h"

namespace PAMI {
namespace Device {
namespace Generic {

///  \brief A Device implmentation of a Generic Work Queuesystem
///
///  This class implements a queue system. The user posts work to the queue
///  and calls to advance will execute the work.
///
class Device {

public:
	/// \brief standard Device::Factory API
	class Factory : public Interface::FactoryInterface<Factory,Device,Device> {
	public:
		/// \brief Generate an array of devices for a client
		/// \param[in] client Client ID
		/// \param[in] num_ctx  Number of contexts being created in client
		/// \param[in] mm Memory manager (for shmem alloc, if needed)
		/// \return Array of devices
		static inline Device *generate_impl(size_t client, size_t num_ctx, Memory::MemoryManager & mm, PAMI::Device::Generic::Device *devices) {                    (void)mm;(void)devices;
			size_t x;
			Device *gds;
			pami_result_t rc;
			rc = __global.heap_mm->memalign((void **)&gds, 16, sizeof(*gds) * num_ctx);
			PAMI_assertf(rc == PAMI_SUCCESS, "alloc failed for generics[%zu], errno=%d\n", num_ctx, errno);
			for (x = 0; x < num_ctx; ++x) {
				new (&gds[x]) PAMI::Device::Generic::Device(client, x, num_ctx);
			}
			return gds;
		}
		/// \brief Initialize a specific device for client/context
		/// \param[in] devs   Device array returned by generate call
		/// \param[in] client   Client ID
		/// \param[in] contextId  Context ID
		/// \param[in] clt    Client
		/// \param[in] ctx    Context
		/// \param[in] sd   SysDep
		/// \param[in] devices    Generic Device array (same as devs in this case)
		/// \return Error code
		static inline pami_result_t init_impl(Device *devs, size_t client, size_t contextId, pami_client_t clt, pami_context_t ctx, PAMI::Memory::MemoryManager *mm, PAMI::Device::Generic::Device *devices) {
                        (void)clt;(void)client;
			return getDevice_impl(devs, client, contextId).init(ctx, client, contextId, mm, devices);
		}
		/// \brief Advance this device for client/context
		/// \param[in] devs Device array returned by generate call
		/// \param[in] client Client ID
		/// \param[in] context  Context ID
		/// \return Events processed
		static inline size_t advance_impl(Device *devs, size_t client, size_t context) {
			return getDevice_impl(devs, client, context).advance();
		}
		/// \brief Get reference to specific device given client and context
		/// \param[in] devs Device array returned by generate call
		/// \param[in] client Client ID
		/// \param[in] context  Context ID
		/// \return Reference to a device
		static inline Device & getDevice_impl(Device *devs, size_t client, size_t context) {
                        (void)client;
			return devs[context];
		}
	}; // class Factory

	/// \brief  Constructor for generic device
	///
	/// \param[in] client   Client ID
	/// \param[in] contextId  Context ID
	/// \param[in] num_ctx    Number of contexts for client
	///
	inline Device(size_t client, size_t contextId, size_t num_ctx) :
#if 0
	__GenericQueue(),
	__Threads(),
#endif
	__ThrIter(),
	__clientId(client),
	__contextId(contextId),
	__nContexts(num_ctx)
	{
	}

	/// \brief Initialize the generic device slice
	/// \param[in] ctx  Context
	/// \param[in] client Client ID
	/// \param[in] context  Context ID
	/// \return Error code
	inline pami_result_t init(pami_context_t ctx, size_t client, size_t context, PAMI::Memory::MemoryManager *mm, PAMI::Device::Generic::Device *devices) {
		// GenericThread must be able to enqueue on both Queues...
		COMPILE_TIME_ASSERT(sizeof(PAMI::Queue::Element) >=
					sizeof(GenericDevicePostingQueue::Element));
		COMPILE_TIME_ASSERT(sizeof(pami_work_t) >= sizeof(GenericThread));
		__context = ctx;
		__mm = mm;
		__allGds = devices;

		__queues = NULL;
		char key[PAMI::Memory::MMKEYSIZE];
		int n = sprintf(key, "/proc%zd-clt%zd-ctx%zd-gd-",
			__global.mapping.task(),
			client, context);
		pami_result_t rc;
		PAMI::Memory::MemoryManager *qmm;
#if defined(__pami_target_bgq__) && defined(USE_COMMTHREADS)
		qmm = __global._wuRegion_mm;
#else // ! __pami_target_bgq__
		qmm = __global.heap_mm;
#endif // ! __pami_target_bgq__
		PAMI_assertf(__queues->__Threads.checkCtorMm(qmm) &&
				__queues->__Posted.checkCtorMm(qmm) &&
				__queues->__GenericQueue.checkCtorMm(qmm),
			"Queue memory incompatible with embedded atomics");
		// process-private allocation...
		rc = qmm->memalign((void **)&__queues,
					GenericDevicePostingQueue::ALIGNMENT,
					sizeof(*__queues));
		PAMI_assertf(rc == PAMI_SUCCESS, "Out of memory allocating generic device queues");
		new (&__queues->__Threads) PAMI::Queue();
		new (&__queues->__Posted) GenericDevicePostingQueue();
		new (&__queues->__GenericQueue) GenericDeviceCompletionQueue();
		key[n] = 't';
		key[n + 1] = '\0';
		PAMI_assertf(__queues->__Threads.checkDataMm(mm) &&
				__queues->__Posted.checkDataMm(mm) &&
				__queues->__GenericQueue.checkDataMm(mm),
			"supplied MemoryManager incompatible with queue atomics");
		__queues->__Threads.init(mm, key);
		key[n] = 'p';
		__queues->__Posted.init(mm, key);
		key[n] = 'm';
		__queues->__GenericQueue.init(mm, key);
		__queues->__Threads.iter_init(&__ThrIter);
		__queues->__Posted.iter_init(&__PstIter);
		return PAMI_SUCCESS;
	}

	/// \brief Advance routine for (one channel of) the generic device.
	///
	/// This advances all units of work on this context's queue, and
	/// checks the message queue for completions. It also calls the
	/// advanceRecv routine for all devices.
	///
	/// \return number of events processed
	///
	inline size_t advance() {
		int events = 0;
		//+ Need to ensure only one of these runs per core
		//+ (even if multi-threads per core)
		//+ if (core_mutex.tryAcquire()) {

		// could check the queues here and return if empty, but it
		// probably takes just as much as the for loops would, and
		// just further delay the advance of real work when present.

		//if (!__Threads.mutex()->tryAcquire()) continue;
		GenericThread *thr;

		// We do the persistent queue first to avoid calling new work twice.
		// We want to avoid enqueueing onto persistent if it completes on
		// the first call.

		// This queue has a single produce/consumer - same thread does both.
		// i.e. it is completely single-threaded and private.
		for (__queues->__Threads.iter_begin(&__ThrIter);
				__queues->__Threads.iter_check(&__ThrIter);
				__queues->__Threads.iter_end(&__ThrIter)) {
			thr = (GenericThread *)__queues->__Threads.iter_current(&__ThrIter);
			if (likely(thr->getStatus() == PAMI::Device::Ready)) {
				++events;
				pami_result_t rc = thr->executeThread(__context);
				if (unlikely(rc != PAMI_EAGAIN)) {
					// thr must not be accessed - it might be re-used
					// or freed!
					// thr->setStatus(PAMI::Device::Complete);
					__queues->__Threads.iter_remove(&__ThrIter);
					continue;
				}
			}
		}

		// All work is removed from queue, by definition.
		// The queue iterator implements this.
		__queues->__Posted.iter_begin(&__PstIter);
		for (; __queues->__Posted.iter_check(&__PstIter); __queues->__Posted.iter_end(&__PstIter)) {
			thr = (GenericThread *)__queues->__Posted.iter_current(&__PstIter);
			// By definition, a posting queue removes all elements as
			// we work the iterator. In that case the following remove()
			// is a NO-OP. However, a conventional queue might be useable
			// in come cases and so we call remove() in an attempt to be
			// compatible with both.
			__queues->__Posted.iter_remove(&__PstIter);
			if (thr->getStatus() == PAMI::Device::Ready) {
				++events;
				pami_result_t rc = thr->executeThread(__context);
				if (rc == PAMI_EAGAIN) {
					__queues->__Threads.enqueue((PAMI::Queue::Element *)thr);
				}
			} else if (thr->getStatus() != PAMI::Device::Complete) {
				__queues->__Threads.enqueue((PAMI::Queue::Element *)thr);
			}
		}
		//__queues->__Threads.mutex()->release();

		//+ core_mutex.release();

		// Now check everything on the completion queue...
		GenericMessage *msg, *nxtmsg, *nxt;
		for (msg = (GenericMessage *)__queues->__GenericQueue.peekHead(); msg; msg = nxtmsg) {
			nxtmsg = (GenericMessage *)__queues->__GenericQueue.nextElem(msg);
			if (msg->getStatus() == Done) {
				++events;
				__queues->__GenericQueue.deleteElem(msg);
				GenericDeviceMessageQueue *qs = msg->getQS();
				// replace dequeue by remove to support multiple active messages in queue
				// qs->dequeue(); // assert return == msg
				qs->remove(msg);
				nxt = (PAMI::Device::Generic::GenericMessage *)qs->peek();
				if (nxt) nxt->postNext(true); // virtual function
				msg->executeCallback(__context);
			}
		}
		return events;
	}

	/// \brief     Advance routine for the generic device.
	///
	/// Currently not used, since subdevices have to be polled for recvs.
	///
	/// \return Boolean indicating if device needs advancing
	///
	inline bool isAdvanceNeeded();

	/// \brief     Post a thread object on a generic device slice's queue
	///
	/// Used this to post a thread of work. The function must return
	/// either PAMI_SUCCESS to indicate completion, or PAMI_EGAIN to
	/// be called again on the next pass through advance. The function
	/// must not re-queue itself using the same GenericThread object,
	/// as it may already be queued and re-queueing would cause an
	/// endless loop in a linked list queue.
	///
	/// \param[in] thr  Thread object to post for advance work
	///
	inline void postThread(GenericThread *thr) __attribute__((__always_inline__)) {
		__queues->__Posted.enqueue((GenericDevicePostingQueue::Element *)thr);
	}

	/// \brief Post a message to the generic-device queuing system
	///
	/// \param[in] msg  Message to be queued/completed.
	///
	inline void postMsg(GenericMessage *msg) {
		__queues->__GenericQueue.enqueue((GenericDeviceCompletionQueue::Element *)msg);
	}

	/// \brief accessor for the context-id associated with generic device slice
	/// \return context ID
	inline size_t contextId() { return __contextId; }
	inline size_t clientId() { return __clientId; }

	/// \brief accessor for the total number of contexts in this client
	/// \return number of contexts/generic device slices
	inline size_t nContexts() { return __nContexts; }

	/// \brief accessor for the context associated with generic device slice
	/// \return context handle
	inline pami_context_t getContext() { return __context; }

	inline Memory::MemoryManager *getMM() { return __mm; }

	inline PAMI::Device::Generic::Device *getAllDevs() { return __allGds; }

	inline void dump(const char *str) {
		__queues->__Posted.iter_dump(str, &__PstIter);
		__queues->__Threads.iter_dump(str, &__ThrIter);
	}

private:
	struct GenericDeviceQueues {
		/// \brief Storage for the queue of threads (a.k.a. work units)
		GenericDevicePostingQueue __Posted; // often alignment critical
		PAMI::Queue __Threads;

		/// \brief Storage for the queue for message completion
		///
		/// Queue[1] is used by the Generic::Device to enqueue messages for completion.
		/// By convention, queue[0] is used for attaching messages to a sub-device.
		///
		GenericDeviceCompletionQueue __GenericQueue;
	};

	GenericDeviceQueues *__queues;
	PAMI::Queue::Iterator __ThrIter;
	GenericDevicePostingQueue::Iterator __PstIter;

	pami_context_t __context; ///< context handle for this generic device
	size_t __clientId;    ///< client ID for context
	size_t __contextId;   ///< context ID
	size_t __nContexts;   ///< number of contexts in client
	Memory::MemoryManager *__mm;
	Device *__allGds;
}; /* class Device */

}; /* namespace Generic */
}; /* namespace Device */
}; /* namespace PAMI */

#endif /* __components_devices_generic_device_h__ */

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=java --indent=force-tab=8 --indent-preprocessor
// astyle options --indent-col1-comments --max-instatement-indent=79
// astyle options --min-conditional-indent=2 --pad-oper --unpad-paren
// astyle options --pad-header --add-brackets --keep-one-line-blocks
// astyle options --keep-one-line-statements --align-pointer=name --lineend=linux
//
