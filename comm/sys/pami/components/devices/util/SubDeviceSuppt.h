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
#ifndef __components_devices_util_SubDeviceSuppt_h__
#define __components_devices_util_SubDeviceSuppt_h__

#include "components/devices/generic/Device.h"

////////////////////////////////////////////////////////////////////////
///  \file components/devices/util/SubDeviceSuppt.h
///  \brief Generic Device Examples and Frequently-used sub-classes
///
///  Namespace:  PAMI, the messaging namespace.
///
////////////////////////////////////////////////////////////////////////

/// \page use_gendev
///
/// \section use_gendev_suppt How to use the Generic Device with SubDeviceSuppt.h
///
/// This chapter explains what is provided by the header file "SubDeviceSuppt.h"
/// and how to use it.
///
/// \subsection use_gendev_suppt_syn SYNOPSIS
/// <div style="margin-left: 3em">
///
/// \#include "components/devices/util/SubDeviceSuppt.h"
///
/// </div>
/// \subsection use_gendev_suppt_thr THREAD SUPPORT
/// <div style="margin-left: 3em">
///
/// \ref PAMI::Device::Generic::GenericAdvanceThread "class GenericAdvanceThread"
///
/// \b Provides:
/// <div style="margin-left: 3em">
///
/// \ref PAMI::Device::Generic::GenericAdvanceThread::setMsg "void setMsg(GenericMessage *msg)"
///
/// \ref PAMI::Device::Generic::GenericAdvanceThread::getMsg "GenericMessage *getMsg()"
///
/// \ref PAMI::Device::Generic::GenericAdvanceThread::setAdv "void setAdv(pami_work_function advThr)"
///
/// </div>
/// \ref PAMI::Device::Generic::SimpleAdvanceThread "class SimpleAdvanceThread"
///
/// Basic thread object for messages that move data. Adds a "bytes left" field.
/// Inherits from GenericAdvanceThread (and, by definition, GenericThread).
/// Ctor initializes "bytes left" to 0.
///
/// \b Provides:
/// <div style="margin-left: 3em">
///
/// \ref PAMI::Device::Generic::SimpleAdvanceThread::_bytesLeft "size_t _bytesLeft (public data member)"
///
/// </div>
/// </div>
/// \subsection use_gendev_suppt_msg MESSAGE SUPPORT
/// <div style="margin-left: 3em">
///
/// \ref PAMI::Device::Generic::DECL_ADVANCE_ROUTINE "DECL_ADVANCE_ROUTINE(method, message, thread)"
///
///  Declare a static advance function stub (e.g. that may be use in setAdv()) which
///  calls an inlined function by the same name prepended with double-underscore.
///  In this way, the inlined advance function can be used internally for such things
///  as early advance, while the static function may be used for posting work.
///
/// \b Creates:
/// <div style="margin-left: 3em">
///
/// \ref PAMI::Device::Generic::method "pami_result_t method(pami_context_t, void *)"
///
/// </div>
/// \b Requires:
/// <div style="margin-left: 3em">
///
/// \ref PAMI::Device::Generic::__method "pami_result_t __method(thread *)"
///
/// </div>
/// \ref PAMI::Device::Generic::DECL_ADVANCE_ROUTINE2 "DECL_ADVANCE_ROUTINE2(method, message, thread)"
///
///   Declare a static advance function stub (e.g. that may be use in setAdv()) which
///   calls an inlined function by the same name prepended with double-underscore.
///   In this way, the inlined advance function can be used internally for such things
///   as early advance, while the static function may be used for posting work.
///
/// \b Creates:
/// <div style="margin-left: 3em">
///
/// \ref PAMI::Device::Generic::method "pami_result_t method(pami_context_t, void *)"
///
/// </div>
/// \b Requires:
/// <div style="margin-left: 3em">
///
/// \ref PAMI::Device::Generic::__method "pami_result_t __method(pami_context_t, thread *)"
///
///
/// </div>
/// </div>
/// \subsection use_gendev_suppt_dev DEVICE SUPPORT
/// <div style="margin-left: 3em">
///
/// \ref PAMI::Device::Generic::MultiSendQSubDevice "class MultiSendQSubDevice<thread, nthreads, usequeue>"
///
/// A send-queue with an array of thread objects. Inherits from the typedef
/// GenericDeviceMessageQueue which defines the type of queue needed by the
/// generic device to support a message object being on two queues at once.
///
/// \b Provides:
/// <div style="margin-left: 3em">
///
/// \ref PAMI::Device::Generic::MultiSendQSubDevice::getGenerics "Device *getGenerics(size_t client)"
///
/// \ref PAMI::Device::Generic::MultiSendQSubDevice::getQS "GenericDeviceMessageQueue *getQS()"
///
/// \ref PAMI::Device::Generic::MultiSendQSubDevice::__getThreads "void __getThreads(T_Thread **t, int *n)"
///
/// \ref PAMI::Device::Generic::MultiSendQSubDevice::__postNext "pami_context_t __postNext(GenericMessage *msg, bool devQueued)"
///
/// \ref PAMI::Device::Generic::MultiSendQSubDevice::__post "void __post(GenericMessage *msg)"
///
/// </div>
/// \b Requires:
/// <div style="margin-left: 3em">
///
/// \ref PAMI::Device::Generic::T_Message::setThreads "int msg->setThreads(thread **t)"
///
///
/// </div>
/// </div>
/// \ref PAMI::Device::Generic::CommonQueueSubDevice "Single Global Device with multiple Message types"
/// <div style="margin-left: 3em">
///
/// \ref PAMI::Device::Generic::CommonQueueSubDevice "class CommonQueueSubDevice"
/// \n
/// \ref PAMI::Device::Generic::SharedQueueSubDevice "class SharedQueueSubDevice<commondevice, thread, nthreads>"
///
/// A pair of classes that support a device model where there is a single
/// hardware device (and send queue) that is used by multiple different
/// message types. An example use of this is the BG/P Collective Network
/// where a single hardware device is used by messages implementing
/// broadcast, allreduce, allreduce with pre/post processing, and
/// allreduce supporting double-sums (on hardware that does not directly
/// support floating point operations).
///
/// \b Provides:
/// <div style="margin-left: 3em">
///
/// TBD...
///
/// </div>
/// </div>
/// <HR>

namespace PAMI {
namespace Device {
namespace Generic {

/// \brief A thread that may be associated with a Message.
///
/// A thread object that contains a pointer to the associated message object.
/// Inherits from GenericThread. Ctor initializes message pointer to NULL.
///
class GenericAdvanceThread : public GenericThread {

public:
        GenericAdvanceThread() :
        GenericThread(),
        _msg(NULL)
        {
        }

        /// \brief Set message object that thread works on
        ///
        /// \param[in] msg	Message object
        inline void setMsg(GenericMessage *msg) { _msg = msg; }

        /// \brief Set advance routine to be used
        ///
        /// Default setup of work function as typically needed for thread advance.
        /// Sets work function to 'advThr' and cookie (clientdata) to 'this'
        /// (i.e. this thread object).
        ///
        /// \param[in] advThr	Advance function
        inline void setAdv(pami_work_function advThr) { _func = advThr; _cookie = this; }

        /// \brief Get message object that thread works on
        ///
        /// \return	Message object
        inline GenericMessage *getMsg() { return _msg; }

protected:
        GenericMessage *_msg;	///< the message on which thread is working
}; /* class GenericAdvanceThread */

//////////////////////////////////////////////////////////////////////
///  \brief A Generic Device implmentation of a thread.
///
///  This class implements a useable, simple, thread object.
///  Other, more complex, thread classes are implemented in specific
///  sub-devices.  For example, see bgp/collective_network/CollectiveNetworkLib.h
///  and class BaseGenericCNThread.
//////////////////////////////////////////////////////////////////////
class SimpleAdvanceThread : public GenericAdvanceThread {
public:
        SimpleAdvanceThread() :
        GenericAdvanceThread(),
        _bytesLeft(0)
        {
        }
public:
        /// \brief byte count used by message advance routine as needed
        /// \ingroup use_gendev_suppt
        size_t _bytesLeft;
}; /* class SimpleAdvanceThread */

/// \brief Macro for declaring a routine as an advance routine for a thread
/// \ingroup use_gendev_suppt
///
/// Creates a static function named 'method' that may be used for a
/// thread's advance routine (thr->setAdv('method')). Assumes there is
/// also an inlined function named __'method' which contains the actual
/// advance code for the thread(s).
///
/// \param[in] method	Basename of method used to advance thread(s)
/// \param[in] message	Class of message
/// \param[in] thread	Class of thread
/// \return	PAMI_SUCCESS when complete, or PAMI_EAGAIN if more work to do
///
#define DECL_ADVANCE_ROUTINE(method,message,thread)			\
static pami_result_t method(pami_context_t context, void *t) {	\
        thread *thr = (thread *)t;				\
        message *msg = (message *)thr->getMsg();		\
        return msg->__##method(thr);				\
}
class T_Thread;	// happy doxygen

/// \fn static pami_result_t method(pami_context_t ctx, void *thr)
/// \brief Static function used for advancing work on thread/message
///
/// The static advance function stub, suitable for use in setAdv().
/// Actual advance code is in __method function of the Message.
///
/// \param[in] ctx	The context on which working is being done
/// \param[in] thr	The thread object being worked
/// \return	PAMI_SUCCESS when complete, or PAMI_EAGAIN if more work to do
extern pami_result_t method(pami_context_t ctx, void *thr);	// happy doxygen

/// \fn static pami_result_t __method(T_Thread *thr)
/// \brief Inline function used for advancing work on thread/message
///
/// The actual advance function. A message class that uses DECL_ADVANCE_ROUTINE()
/// to declare the advance routine(s) must implement one of these for each
/// advance routine.
///
/// \param[in] thr	The thread object being worked
/// \return	PAMI_SUCCESS when complete, or PAMI_EAGAIN if more work to do
extern pami_result_t __method(T_Thread *thr);	// happy doxygen

/// \brief Macro for declaring a routine as an advance routine for a thread
/// \ingroup use_gendev_suppt
///
/// Passes context to advance routine, along with thread object.
///
/// Creates a static function named 'method' that may be used for a
/// thread's advance routine (thr->setAdv('method')). Assumes there is
/// also an inlined function named __'method' which contains the actual
/// advance code for the thread(s).
///
/// \param[in] method	Basename of method used to advance thread(s)
/// \param[in] message	Class of message
/// \param[in] thread	Class of thread
/// \return	PAMI_SUCCESS when complete, or PAMI_EAGAIN if more work to do
///
#define DECL_ADVANCE_ROUTINE2(method,message,thread)		\
static pami_result_t method(pami_context_t context, void *t) {	\
        thread *thr = (thread *)t;				\
        message *msg = (message *)thr->getMsg();		\
        return msg->__##method(context, thr);			\
}

/// \fn pami_result_t __method(pami_context_t ctx, T_Thread *thr)
/// \brief Inline function used for advancing work on thread/message
///
/// The actual advance function. Has access to context if needed.
/// A message class that uses DECL_ADVANCE_ROUTINE2()
/// to declare the advance routine(s) must implement one of these for each
/// advance routine.
///
/// \param[in] ctx	The context on which work is being done
/// \param[in] thr	The thread object being worked
/// \return	PAMI_SUCCESS when complete, or PAMI_EAGAIN if more work to do
extern pami_result_t __method(pami_context_t ctx, T_Thread *thr);	// happy doxygen

/// \fn int T_Message::setThreads(T_Thread **t)
/// \brief inline function for setting up threads on a message
///
/// Sets up one or more threads for message, possibly advancing
/// threads. This means that threads might be complete before they
/// can be posted, and so typically this is checked and the postThread()
/// is skipped.
///
/// \param[out] t	Array of threads
/// \return	Number of threads setup in array
namespace T_Message { // This helps doxygen understand
extern int setThreads(T_Thread **t);	// happy doxygen
};

/// \brief Example sub-device for using multiple send queues
///
/// This is typically what 'local' point-to-point devices do, to enforce
/// ordering to a peer.
///
template <class T_Thread,int N_Threads,bool Use_Queue>
class MultiSendQSubDevice : public GenericDeviceMessageQueue {
public:
        MultiSendQSubDevice() :
        GenericDeviceMessageQueue() {
                // There must be at least one queue, since every message
                // requires a valid _QS from which to operate.
                // However, a device/message may not actually queue
                // anything to this _QS.
                for (int y = 0; y < N_Threads; ++y) {
                        new (&_threads[y]) T_Thread();
                }
        }

        /// \brief Initialization for the send-queue object
        /// \ingroup use_gendev_suppt_dev_msq_p
        ///
        /// \param[in] client		Id of current client
        /// \param[in] contextId	Id of current context
        /// \param[in] clt		Client
        /// \param[in] ctx		Context
        /// \param[in] mm		MemoryManager object (not used?)
        /// \param[in] devices		Array of Generic::Device objects for client
        /// \return	error code
        ///
        inline pami_result_t __init(size_t client, size_t contextId, pami_client_t clt, pami_context_t ctx, PAMI::Memory::MemoryManager *mm, PAMI::Device::Generic::Device *devices) {
                // assert(_generics[client] == NULL || _generics[client] == devices);
                _generics[client] = devices;
                return PAMI_SUCCESS;
        }

        /// \brief Accessor for arrays of generic devices
        /// \ingroup use_gendev_suppt_dev_msq_p
        ///
        /// \param[in] client		Id of current client
        /// \return	Array of generic devices for client
        ///
        inline PAMI::Device::Generic::Device *getGenerics(size_t client) {
                return _generics[client];
        }

        /// \brief Accessor for QS pointer to use with this object
        /// \ingroup use_gendev_suppt_dev_msq_p
        ///
        /// \return	Array of generic devices for client
        ///
        inline GenericDeviceMessageQueue *getQS() {
                return this;
        }

        /// \brief Acessor for thread objects for this send queue
        /// \ingroup use_gendev_suppt_dev_msq_p
        ///
        /// \param[out] t	Thread object array
        /// \param[out] n	Size of thread object array (number of elements)
        ///
        inline void __getThreads(T_Thread **t, int *n) {
                if (N_Threads > 0) {
                        *t = &_threads[0];
                        *n = N_Threads;
                } else {
                        *t = NULL;
                        *n = 0;
                }
        }

public:
        /// \brief Start next message
        /// \ingroup use_gendev_suppt_dev_msq_p
        ///
        /// Starts a message, by setting up threads and posting all objects
        /// to generic device slices. Places the first thread on a different
        /// context than the message.
        ///
        /// If message is completed during thread setup (early advance),
        /// and it is not yet queued, then does not post anything to
        /// the generic device.
        ///
        /// \param[in] msg		Message to be completed or queued
        /// \param[in] devQueued	Whether message is already on queue
        /// \return	context for completion callback, or NULL if not completed
        ///
        template <class T_Message>
        inline pami_context_t __postNext(PAMI::Device::Generic::GenericMessage *msg, bool devQueued) {
                PAMI::Device::Generic::Device *g;
                g = getGenerics(msg->getClientId());
                T_Thread *t;
                int n;
                msg->setStatus(PAMI::Device::Initialized);
                // setThreads() might complete some/all threads...
                n = static_cast<T_Message *>(msg)->setThreads(&t);
                size_t x = msg->getContextId();
                if (!devQueued && msg->getStatus() == PAMI::Device::Done) {
                        // PAMI_assert(g[x].getContext() != NULL);
                        return g[x].getContext();
                }
                size_t numctx = g[x].nContexts();
                g[x].postMsg(msg);
                while (n > 0) {
                        if (t->getStatus() != PAMI::Device::Complete) {
                                g[x].postThread(t);
                                if (++x >= numctx) x = 0;
                        }
                        ++t;
                        --n;
                }
                return NULL;
        }

        /// \brief Post message to device
        /// \ingroup use_gendev_suppt_dev_msq_p
        ///
        /// If queue is currently empty, then the message may be progressed
        /// and completed without queueing. Otherwise message is queued
        /// for later start.
        ///
        /// If this object is instanciated with Use_Queue 'false', then the
        /// message will always be progressed and never queued. This means that
        /// work will have been posted to the generic device, so the message
        /// should eventually complete.
        ///
        /// \param[in] msg	Message to be posted
        ///
        template <class T_Message>
        inline void __post(PAMI::Device::Generic::GenericMessage *msg) {
                // GenericDeviceMessageQueue *qs = (GenericDeviceMessageQueue *)msg->getQS();
                // the above would allow an implementation to vary where the queue is...

                // PAMI_assert(qs == this);
                bool first = (!Use_Queue || peek() == NULL);
                // If !Use_Queue, there must never be a message queued...
                // PAMI_assert(Use_Queue || peek() == NULL);
                if (first) {
                        pami_context_t ctx = __postNext<T_Message>(msg, false);
                        if (ctx) {
                                msg->executeCallback(ctx);
                                return;
                        }
                        // If this device does not use the queue, avoid
                        // enqueueing the unfinished message... assume that the
                        // __postNext() call setup everything on the generic
                        // device and we no longer care about it...
                        // Also avoid the peek() check above.
                }
                if (Use_Queue) enqueue(msg);
        }

protected:
        T_Thread _threads[N_Threads];	///< Threads for current message on device
        PAMI::Device::Generic::Device *_generics[PAMI_MAX_NUM_CLIENTS]; ///< generic device arrays
}; // class MultiSendQSubDevice

class NillSubDevice : public PAMI::Device::Generic::Device {
public:
	class Factory : public Interface::FactoryInterface<Factory,NillSubDevice,PAMI::Device::Generic::Device> {
	public:
		static inline NillSubDevice *generate_impl(size_t clientid, size_t num_ctx,
						Memory::MemoryManager & mm,
						PAMI::Device::Generic::Device *devices) {
                        (void)clientid;(void)num_ctx;(void)mm;
			return (NillSubDevice *)devices;
		}
		static inline pami_result_t init_impl(NillSubDevice *devs,
						size_t clientid, size_t contextid,
						pami_client_t clt, pami_context_t ctx,
						PAMI::Memory::MemoryManager *mm,
						PAMI::Device::Generic::Device *devices) {
                        (void)devs;(void)clientid;(void)contextid;(void)clt;
                        (void)ctx;(void)mm;(void)devices;
			return PAMI_SUCCESS;
		}
		static inline size_t advance_impl(NillSubDevice *devs,
						size_t clientid, size_t contextid) {
                        (void)devs;(void)clientid;(void)contextid;
			return 0;
		}
		static inline NillSubDevice &getDevice_impl(NillSubDevice *devs,
						size_t clientid, size_t contextid) {
			return (NillSubDevice &)
				PAMI::Device::Generic::Device::Factory::getDevice(devs,
								clientid, contextid);
		}
	}; // class NillSubDevice::Factory
}; // class NillSubDevice

///
/// Implements a shared-queue for use by multiple different Thr/Msg/Dev/Mdl sets
/// which all share the same hardware (system) resource. Such a family of sets
/// would each refer to a common instance of this class object when doing their
/// init. The Device class of each set would inherit from SharedQueueSubDevice.
///
/// Supports only one message active at a time.
///
class CommonQueueSubDevice : public GenericDeviceMessageQueue {
        #define ATOMIC_BUF_SIZE	16

public:

        /// \brief Default constructor for CommonQueueSubDevice
        CommonQueueSubDevice() :
        GenericDeviceMessageQueue(),
        _dispatch_id(0),
        _mm(NULL)
        {
        }

        virtual ~CommonQueueSubDevice() {}

        /// \note This is required to make "C" programs link successfully with virtual destructors
        inline void operator delete(void * p) { (void)p;PAMI_abortf("%s<%d>\n", __FILE__, __LINE__); }

        /// \brief returns a unique ID relative to this common sub-device
        ///
        /// \return integer ID unique to this CommonQueueSubDevice
        ///
        inline unsigned newDispID() {
                // caller must ensure number os valid for their hardware,
                // for example a network device might only have 8 bits in
                // the header for "dispatch ID".

                // might need to be atomic, in some situations?
                return ++_dispatch_id;
        }

        /// \brief init virtual function definition
        ///
        /// All classes that inherit from this must implement init(), and that
        /// must callback to __init().
        ///
        /// \param[in] mm		MemoryManager object
        /// \param[in] client		Client ID
        /// \param[in] contextId	Context ID
        /// \param[in] ctx		Context
        /// \return	Error code
        virtual pami_result_t init(PAMI::Memory::MemoryManager *mm, size_t client, size_t contextId, pami_context_t ctx) = 0;

        /// \brief CommonQueueSubDevice portion of init function
        ///
        /// All classes that inherit from this must implement init(), and that
        /// must callback to __init().
        ///
        /// \param[in] client		Client ID
        /// \param[in] contextId	Context ID
        /// \param[in] clt		Client
        /// \param[in] ctx		Context
        /// \param[in] mm		MemoryManager object
        /// \param[in] devices		Array of generic devices for client
        ///
        inline pami_result_t __init(size_t client, size_t contextId, pami_client_t clt, pami_context_t ctx, PAMI::Memory::MemoryManager *mm, const char *key, PAMI::Device::Generic::Device *devices) {
                (void)clt;(void)ctx;(void)key;
                if (client == 0) {
#if 0 // always an in-place (proc-scoped) counter...
			unsigned n = strlen(key);
			PAMI_assert_debugf(n + 5 < PAMI::Memory::MMKEYSIZE,
				"mm key \"%s\" overflow", key);
			strcpy((char *)&key[n], "-done");
#endif
                        _mm = mm;
                        //_doneThreads.init();
                        _doneThreads.fetch_and_clear();
                        _init = 1;
                }
                if (contextId == 0) {
                        _generics[client] = devices;
                }
                return PAMI_SUCCESS;
        }

        /// \brief Reset for threads prior to being re-used.
        ///
        inline void __resetThreads() {
                _doneThreads.fetch_and_clear();
        }

        /// \brief CommonQueueSubDevice portion of completion for a thread
        ///
        /// \param[in] t	Thread being completed
        /// \return	Total number of threads completed for message
        ///
        inline unsigned __completeThread(GenericAdvanceThread *t) {
                (void)t;
                // fetchIncr() returns value *before* increment,
                // and we need to return total number of threads completed,
                // so we return "+1".
                return _doneThreads.fetch_and_inc() + 1;
        }

        /// \brief Get SysDep object
        /// \return	SysDep object
        inline PAMI::Memory::MemoryManager *getSysdep() { return _mm; }

        /// \brief Get array of generic devices for client
        /// \param[in] client	Client ID (offset)
        /// \return	Array of generic devices
        inline PAMI::Device::Generic::Device *getGenerics(size_t client) {
                return _generics[client];
        }

        /// \brief Start new message and post to generic device
        /// \param[in] msg		Message to start
        /// \param[in] devQueued	Whether message is already on device send queue
        template <class T_Message, class T_Thread>
        inline pami_context_t __postNext(PAMI::Device::Generic::GenericMessage *msg, bool devQueued) {
                PAMI::Device::Generic::Device *g;
                g = getGenerics(msg->getClientId());
                T_Thread *t;
                int n;
                msg->setStatus(PAMI::Device::Initialized);
                // setThreads() might complete some/all threads...
                n = static_cast<T_Message *>(msg)->setThreads(&t);
                size_t x = msg->getContextId();
                if (!devQueued && msg->getStatus() == PAMI::Device::Done) {
                        // PAMI_assert(g[x].getContext() != NULL);
                        return g[x].getContext();
                }
                size_t numctx = g[x].nContexts();
                g[x].postMsg(msg);
                while (n > 0) {
                        if (t->getStatus() != PAMI::Device::Complete) {
                                g[x].postThread(t);
                                if (++x >= numctx) x = 0;
                        }
                        ++t;
                        --n;
                }
                return NULL;
        }

        /// \brief Internal posting of message to sub-device
        ///
        /// Since postNext() will try to advance the message, it may
        /// also complete it. This is tested and appropriate action taken.
        ///
        /// \param[in] msg	Message to start and/or enqueue
        ///
        template <class T_Message, class T_Thread>
        inline void __post(PAMI::Device::Generic::GenericMessage *msg) {
                // GenericDeviceMessageQueue *qs = (GenericDeviceMessageQueue *)msg->getQS();
                // PAMI_assert(qs == this);

                bool first = (peek() == NULL);
                if (first) {
                        pami_context_t ctx = __postNext<T_Message,T_Thread>(msg, false);
                        if (ctx) {
                                msg->executeCallback(ctx);
                                return;
                        }
                }
                enqueue(msg);
        }

private:
        int _init;				///< detect first call to init()
        GenericDeviceCounter _doneThreads;	///< counter used to track message complete
        unsigned _dispatch_id;			///< unique id for sub-devices/messages
        PAMI::Device::Generic::Device *_generics[PAMI_MAX_NUM_CLIENTS]; ///< save generic device array
        PAMI::Memory::MemoryManager *_mm;	///< saved SysDep
}; // class CommonQueueSubDevice

/// \brief class for a Model/Device/Message/Thread tuple that shares hardware with others
///
/// Each Model/Device/Message/Thread tuple will inherit from SharedQueueSubDevice and
/// reference the same instance of CommonQueueSubDevice in the constructor.
/// One instance of CommonQueueSubDevice represents the hardware, which is shared, that
/// can only accomodate one message (of any type) active at a time.
///
/// We would like to have the _threads[] storage kept in the CommonQueueSubDevice,
/// but each Model/Device/Message/Thread tuple could have a different sized Thread class
/// and so we must have the _threads[] array here, where we know the exact Thread type.
///
template <class T_CommonDevice, class T_Thread, int N_Threads>
class SharedQueueSubDevice {
public:
        /// \brief Constructor for SharedQueueSubDevice
        /// \param[in] common	Pointer to the common device object
        SharedQueueSubDevice(T_CommonDevice *common) :
        _common(common)
        {
        }

        /// \brief returns a unique ID relative to this common sub-device
        ///
        /// \return integer ID unique to the CommonQueueSubDevice for this sub-device
        ///
        inline unsigned newDispID() {
                return _common->newDispID();
        }

        /// \brief accessor for the common device for this sub-device
        /// \return	CommonQueueSubDevice
        inline T_CommonDevice *common() { return _common; }

        /// \brief Inform caller of where the threads array is
        ///
        /// In this case, threads are not in the CommonQueueSubDevice but
        /// instead in each SharedQueueSubDevice.
        ///
        /// \param[out] t	Pointer to threads array
        /// \param[out] n	Pointer to number of threads in array
        ///
        inline void getThreads(T_Thread **t, int *n) {
                *t = _threads;
                *n = N_Threads;
        }

        /// \brief Get pointer to generic device array for client
        /// \param[in] client	The client ID (offset)
        /// \return	Array of generic devices
        inline PAMI::Device::Generic::Device *getGenerics(size_t client) {
                return _common->getGenerics(client);
        }

        /// \brief Initialization of shared-queue device
        /// \param[in] client		Client ID
        /// \param[in] contextId	Context ID
        /// \param[in] clt		Client
        /// \param[in] ctx		Context
        /// \param[in] mm		MemoryManager
        /// \param[in] devices		Array of generic devices for client
        /// \return	Error code
        inline pami_result_t __init(size_t client, size_t contextId, pami_client_t clt, pami_context_t ctx, PAMI::Memory::MemoryManager *mm, PAMI::Device::Generic::Device *devices) {
                if (client == 0) {
                        // do this now so we don't have to every time we post
//			for (int x = 0; x < N_Threads; ++x)
//				//_threads[x].setPolled(true);
//			}
                }
                _common->__init(client, contextId, clt, ctx, mm, "/bgp-tree-dev", devices);
                return _common->init(mm, client, contextId, ctx);
        }

        /// \brief Get threads array to use
        /// \param[out] t	Threads array
        /// \param[out] n	Size of threads array
        inline void __getThreads(T_Thread **t, int *n) {
                *t = &_threads[0];
                *n = N_Threads;
        }

        /// \brief Internal posting of message to sub-device
        ///
        /// Since postNext() will try to advance the message, it may
        /// also complete it. This is tested and appropriate action taken.
        ///
        /// \param[in] msg	Message to start and/or enqueue
        ///
        template <class T_Message>
        inline void __post(PAMI::Device::Generic::GenericMessage *msg) {
// "template" keyword inserted to the next line, because of ISO C++03 14.2/4:
// When the name of a member template specialization appears after . or -> in a postfix-expression, 
// or after nested-name-specifier in a qualified-id, and the postfix-expression or qualified-id 
// explicitly depends on a template-parameter (14.6.2), the member template name 
// must be prefixed by the keyword template. Otherwise the name is assumed to name a non-template.
                 _common->template __post<T_Message,T_Thread>(msg);
        }

        /// \brief SharedQueueSubDevice portion of completion for a thread
        ///
        /// Must at least call CommonQueueSubDevice __completeThread().
        ///
        /// \param[in] thr	Thread being completed
        /// \return	Total number of threads completed for message
        ///
        inline unsigned __completeThread(T_Thread *thr) {
                return _common->__completeThread(thr);
        }

private:
        T_CommonDevice *_common;
        T_Thread _threads[N_Threads];
        int _nActiveThreads;
}; // class SharedQueueSubDevice

}; /* namespace Generic */
}; /* namespace Device */
}; /* namespace PAMI */

#endif // __components_devices_generic_SubDeviceSuppt_h__
