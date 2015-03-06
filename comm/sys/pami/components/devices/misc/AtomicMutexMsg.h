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
 * \file components/devices/misc/AtomicMutexMsg.h
 * \brief Mutex that uses whatever Atomic Mutex object  is specified
 */

/// \page atomic_mutex_usage
///
/// Typical usage to instantiate a "generic" mutex based on an atomic object:
///
/// #include "components/atomic/bgp/LockBoxMutex.h"
/// // Change this line to switch to different mutex implementations...
/// typedef PAMI::Mutex::BGP::LockBoxNodeProcMutex MY_MUTEX;
///
/// #include "components/devices/generic/AtomicMutexMsg.h"
/// typedef PAMI::Device::AtomicMutexMdl<MY_MUTEX> MY_MUTEX_MODEL;
/// typedef PAMI::Device::AtomicMutexMsg<MY_MUTEX> MY_MUTEX_MESSAGE;
///
/// pami_result_t status;
/// MY_MUTEX_MODEL _mutex(status);
/// PAMI_assert(status == PAMI_SUCCESS);
///
/// pami_multisync_t _msync;
/// MY_MUTEX_MESSAGE _msg;
/// _msync.request = &_msg;
/// _msync.cb_done = ...;
/// _msync.roles = ...;
/// _msync.participants = ...;
/// _mutex.postMultisync(&_msync);
///

#ifndef __components_devices_misc_AtomicMutexMsg_h__
#define __components_devices_misc_AtomicMutexMsg_h__

#include "Global.h"
#include "components/devices/util/SubDeviceSuppt.h"
#include "components/devices/generic/AdvanceThread.h"
#include "components/devices/MultisyncModel.h"
#include "components/devices/FactoryInterface.h"
#include "components/atomic/MutexInterface.h"
#include <pami.h>

namespace PAMI {
namespace Device {

template <class T_Mutex> class AtomicMutexMsg;
template <class T_Mutex> class AtomicMutexMdl;
template <class T_Mutex> class SharedAtomicMutexMdl;
typedef PAMI::Device::Generic::GenericAdvanceThread AtomicMutexThr;
typedef PAMI::Device::Generic::MultiSendQSubDevice<AtomicMutexThr,1,true> AtomicMutexQue;
// This device is never instantiated
typedef PAMI::Device::Generic::NillSubDevice AtomicMutexDev;

///
/// \brief A local mutex message that takes advantage of the
/// Load Linked and Store Conditional instructions
///
template <class T_Mutex>
class AtomicMutexMsg : public PAMI::Device::Generic::GenericMessage {
public:

protected:
	friend class AtomicMutexMdl<T_Mutex>;
	friend class SharedAtomicMutexMdl<T_Mutex>;

	AtomicMutexMsg(GenericDeviceMessageQueue *Generic_QS,
		T_Mutex *mutex,
		size_t            client,
		size_t            context,
		pami_multisync_t *msync) :
	PAMI::Device::Generic::GenericMessage(Generic_QS, msync->cb_done,
					      client, context),
	_role(msync->roles),
	_mutex(mutex)
	{
		// PAMI_assert(role == DEFAULT_ROLE);
	}

protected:

	DECL_ADVANCE_ROUTINE(advanceAcquireThread,AtomicMutexMsg<T_Mutex>,PAMI::Device::Generic::GenericAdvanceThread);
	inline pami_result_t __advanceAcquireThread(PAMI::Device::Generic::GenericAdvanceThread *thr) {
		if (_mutex->tryAcquire()) {
			setStatus(PAMI::Device::Done);
			thr->setStatus(PAMI::Device::Complete);
			return PAMI_SUCCESS;
		}
		return PAMI_EAGAIN;
	}

	DECL_ADVANCE_ROUTINE(advanceReleaseThread,AtomicMutexMsg<T_Mutex>,PAMI::Device::Generic::GenericAdvanceThread);
	// probably never used in this implementation (unlock never blocks or fails)
	inline pami_result_t __advanceReleaseThread(PAMI::Device::Generic::GenericAdvanceThread *thr) {
		_mutex->release();
		setStatus(PAMI::Device::Done);
		thr->setStatus(PAMI::Device::Complete);
		return PAMI_SUCCESS;
	}

public:
	// virtual function
	pami_context_t postNext(bool devQueued) {
		AtomicMutexQue *qs = (AtomicMutexQue *)getQS();
		return qs->__postNext<AtomicMutexMsg>(this, devQueued);
	}

	inline int setThreads(AtomicMutexThr **th) {
		AtomicMutexThr *t = &_thr;
		t->setMsg(this);
		if (_role == AtomicMutexMdl<T_Mutex>::LOCK_ROLE) {
			t->setAdv(advanceAcquireThread);
		} else if (_role == AtomicMutexMdl<T_Mutex>::UNLOCK_ROLE) {
			t->setAdv(advanceReleaseThread);
		} else {
			PAMI_abortf("Internal error: invalid role in AtomicMutexMsg");
		}
		t->setStatus(PAMI::Device::Ready);
		// "early advance" already done in model post...
		*th = t;
		return 1;
	}

protected:
	unsigned _role;
	T_Mutex *_mutex;
	PAMI::Device::Generic::GenericAdvanceThread _thr;
}; //-- AtomicMutexMsg

template <class T_Mutex>
class AtomicMutexMdl : public PAMI::Device::Interface::MultisyncModel<AtomicMutexMdl<T_Mutex>,
					    AtomicMutexDev,sizeof(AtomicMutexMsg<T_Mutex>) > {
public:
	static const size_t sizeof_msg = sizeof(AtomicMutexMsg<T_Mutex>);
	static const unsigned LOCK_ROLE = (1 << 0);
	static const unsigned UNLOCK_ROLE = (1 << 1);

	AtomicMutexMdl(AtomicMutexDev &device, pami_result_t &status) :
	  PAMI::Device::Interface::MultisyncModel<AtomicMutexMdl<T_Mutex>,
			AtomicMutexDev,sizeof(AtomicMutexMsg<T_Mutex>) >(device, status),
	_gd(&device)
	{
		if (!checkDataMm(_gd->getMM())) {
			status = PAMI_INVAL;
			return;
		}
		_mutex.init(_gd->getMM(), NULL); // problem...
		_queue.__init(_gd->clientId(), _gd->contextId(), NULL, _gd->getContext(), _gd->getMM(), _gd->getAllDevs());
	}

	static bool checkCtorMm(PAMI::Memory::MemoryManager *mm) {
		return T_Mutex::checkCtorMm(mm) &&
			((mm->attrs() & PAMI::Memory::PAMI_MM_NODESCOPE) == 0);
	}

	static bool checkDataMm(PAMI::Memory::MemoryManager *mm) {
		return T_Mutex::checkDataMm(mm);
	}

        inline pami_result_t postMultisyncImmediate_impl(size_t            client,
							 size_t            context,
							 pami_multisync_t *msync,
							 void *devinfo=NULL) 
	{
	  return PAMI_ERROR;
	}

	inline pami_result_t postMultisync_impl(uint8_t (&state)[sizeof_msg],
						size_t          client,
						size_t          context, 
					       pami_multisync_t *msync);

private:
	PAMI::Device::Generic::Device *_gd;
	T_Mutex _mutex;
	AtomicMutexQue _queue;
}; // class AtomicMutexMdl

template <class T_Mutex>
class SharedAtomicMutexMdl : public PAMI::Device::Interface::MultisyncModel<SharedAtomicMutexMdl<T_Mutex>,
					    AtomicMutexDev,sizeof(AtomicMutexMsg<T_Mutex>) > {
public:
	static const size_t sizeof_msg = sizeof(AtomicMutexMsg<T_Mutex>);
	static const unsigned LOCK_ROLE = AtomicMutexMdl<T_Mutex>::LOCK_ROLE;
	static const unsigned UNLOCK_ROLE = AtomicMutexMdl<T_Mutex>::UNLOCK_ROLE;

	SharedAtomicMutexMdl(AtomicMutexDev &device, T_Mutex *mtx, pami_result_t &status) :
	  PAMI::Device::Interface::MultisyncModel<SharedAtomicMutexMdl<T_Mutex>,
			AtomicMutexDev,sizeof(AtomicMutexMsg<T_Mutex>) >(device, status),
	_gd(&device),
	_mutex(mtx)
	{
		// mutex was initialized by caller, probably shared between many models.
		_queue.__init(_gd->clientId(), _gd->contextId(), NULL, _gd->getContext(), _gd->getMM(), _gd->getAllDevs());
	}

	static bool checkCtorMm(PAMI::Memory::MemoryManager *mm) {
		return ((mm->attrs() & PAMI::Memory::PAMI_MM_NODESCOPE) == 0);
	}

	static bool checkDataMm(PAMI::Memory::MemoryManager *mm) {
		return T_Mutex::checkDataMm(mm);
	}

	inline pami_result_t postMultisync_impl(uint8_t (&state)[sizeof_msg],
						size_t          client,
						size_t          context, 
						pami_multisync_t *msync, void *devinfo);

private:
	PAMI::Device::Generic::Device *_gd;
	T_Mutex *_mutex;
	AtomicMutexQue _queue;
}; // class SharedAtomicMutexMdl

}; //-- Device
}; //-- PAMI

template <class T_Mutex>
inline pami_result_t PAMI::Device::AtomicMutexMdl<T_Mutex>::postMultisync_impl(uint8_t (&state)[sizeof_msg],
									       size_t          client,
									       size_t          context, 
										 pami_multisync_t *msync) {
	if (msync->roles == LOCK_ROLE) {
		if (_mutex.tryAcquire()) {
			if (msync->cb_done.function) {
				pami_context_t ctx = _gd->getContext();
				msync->cb_done.function(ctx, msync->cb_done.clientdata, PAMI_SUCCESS);
			}
			return PAMI_SUCCESS;
		}
		// must "continue" current mutex, not start new one!
		AtomicMutexMsg<T_Mutex> *msg;
		msg = new (&state) AtomicMutexMsg<T_Mutex>(_queue.getQS(), &_mutex, client, context, msync);
		_queue.__post<AtomicMutexMsg<T_Mutex> >(msg);
		return PAMI_SUCCESS;
	}
	if (msync->roles == UNLOCK_ROLE) {
		// in this case, unlock never blocks so don't need 'state'
		_mutex.release();
		return PAMI_SUCCESS;
	}
	return PAMI_ERROR;
}

template <class T_Mutex>
inline pami_result_t PAMI::Device::SharedAtomicMutexMdl<T_Mutex>::postMultisync_impl(uint8_t (&state)[sizeof_msg],
										     size_t          client,
										     size_t          context, 
										 pami_multisync_t *msync, void *devinfo) {
	if (msync->roles == LOCK_ROLE) {
		if (_mutex->tryAcquire()) {
			if (msync->cb_done.function) {
				pami_context_t ctx = _gd->getContext();
				msync->cb_done.function(ctx, msync->cb_done.clientdata, PAMI_SUCCESS);
			}
			return PAMI_SUCCESS;
		}
		// must "continue" current mutex, not start new one!
		AtomicMutexMsg<T_Mutex> *msg;
		msg = new (&state) AtomicMutexMsg<T_Mutex>(_queue.getQS(), _mutex, client, context, msync);
		_queue.__post<AtomicMutexMsg<T_Mutex> >(msg);
		return PAMI_SUCCESS;
	}
	if (msync->roles == UNLOCK_ROLE) {
		// in this case, unlock never blocks so don't need 'state'
		_mutex->release();
		return PAMI_SUCCESS;
	}
	return PAMI_ERROR;
}

#endif // __components_devices_misc_AtomicMutexMsg_h__
