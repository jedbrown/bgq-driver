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
 * \file components/devices/misc/AtomicBarrierMsg.h
 * \brief Barrier that uses whatever Atomic Barrier object  is specified
 */

/// \page atomic_barrier_usage
///
/// Typical usage to instantiate a "generic" barrier based on an atomic object:
///
/// #include "components/atomic/bgp/LockBoxBarrier.h"
/// // Change this line to switch to different barrier implementations...
/// typedef PAMI::Barrier::BGP::LockBoxNodeProcBarrier MY_BARRIER;
///
/// #include "components/devices/generic/AtomicBarrierMsg.h"
/// typedef PAMI::Device::AtomicBarrierMdl<MY_BARRIER> MY_BARRIER_MODEL;
/// typedef PAMI::Device::AtomicBarrierMsg<MY_BARRIER> MY_BARRIER_MESSAGE;
///
/// pami_result_t status;
/// MY_BARRIER_MODEL _barrier(status);
/// PAMI_assert(status == PAMI_SUCCESS);
///
/// pami_multisync_t _msync;
/// MY_BARRIER_MESSAGE _msg;
/// _msync.request = &_msg;
/// _msync.cb_done = ...;
/// _msync.roles = ...;
/// _msync.participants = ...;
/// _barrier.postMultisync(&_msync);
///

#ifndef __components_devices_misc_AtomicBarrierMsg_h__
#define __components_devices_misc_AtomicBarrierMsg_h__

#include "Global.h"
#include "components/devices/util/SubDeviceSuppt.h"
#include "components/devices/generic/AdvanceThread.h"
#include "components/devices/MultisyncModel.h"
#include "components/devices/FactoryInterface.h"
#include "components/atomic/BarrierInterface.h"
#include <pami.h>

////////////////////////////////////////////////////////////////////////////////
/// \env{pami,PAMI_ATOMICBARRIER_LOOPS}
/// Number of attempts to complete barrier
/// in each pass.
///
/// \default 32
////////////////////////////////////////////////////////////////////////////////

namespace PAMI {
namespace Device {

template <class T_Barrier> class AtomicBarrierMsg;
template <class T_Barrier> class AtomicBarrierMdl;
typedef PAMI::Device::Generic::GenericAdvanceThread AtomicBarrierThr;
typedef PAMI::Device::Generic::MultiSendQSubDevice<AtomicBarrierThr,1,true> AtomicBarrierQue;
// This device is never instantiated
typedef PAMI::Device::Generic::NillSubDevice AtomicBarrierDev;

///
/// \brief A local barrier message that takes advantage of the
/// Load Linked and Store Conditional instructions
///
template <class T_Barrier>
class AtomicBarrierMsg : public PAMI::Device::Generic::GenericMessage {
public:

protected:
        friend class AtomicBarrierMdl<T_Barrier>;

        AtomicBarrierMsg(GenericDeviceMessageQueue *Generic_QS,
                T_Barrier *barrier,
		size_t            client,
		size_t            context,
                pami_multisync_t *msync,
		size_t loops) :
        PAMI::Device::Generic::GenericMessage(Generic_QS, msync->cb_done,
                                        client, context),
        _barrier(barrier),
	_loops(loops)
        {
                // PAMI_assert(role == DEFAULT_ROLE);
        }

protected:

        DECL_ADVANCE_ROUTINE(advanceThread,AtomicBarrierMsg<T_Barrier>,AtomicBarrierThr);
        inline pami_result_t __advanceThread(AtomicBarrierThr *thr) {
                for (size_t x = 0; x < _loops; ++x) {
                        if (_barrier->poll() == false) {
                                _barrier->end();
                                setStatus(PAMI::Device::Done);
                                thr->setStatus(PAMI::Device::Complete);
                                return PAMI_SUCCESS;
                        }
                }
                return PAMI_EAGAIN;
        }

public:
        // virtual function
        pami_context_t postNext(bool devQueued) {
		AtomicBarrierQue *qs = (AtomicBarrierQue *)getQS();
                return qs->__postNext<AtomicBarrierMsg>(this, devQueued);
        }

        inline int setThreads(AtomicBarrierThr **th) {
		AtomicBarrierQue *qs = (AtomicBarrierQue *)getQS();
                AtomicBarrierThr *t;
                int n;
                qs->__getThreads(&t, &n);
                t->setMsg(this);
                t->setAdv(advanceThread);
                t->setStatus(PAMI::Device::Ready);
                __advanceThread(t);
                *th = t;
                return 1;
        }

protected:
        T_Barrier *_barrier;
	size_t _loops;
}; //-- AtomicBarrierMsg

template <class T_Barrier>
class AtomicBarrierMdl : public PAMI::Device::Interface::MultisyncModel<AtomicBarrierMdl<T_Barrier>,
                                            AtomicBarrierDev,sizeof(AtomicBarrierMsg<T_Barrier>) > {
public:
        static const size_t sizeof_msg = sizeof(AtomicBarrierMsg<T_Barrier>);

        AtomicBarrierMdl(AtomicBarrierDev &device, pami_result_t &status) :
        PAMI::Device::Interface::MultisyncModel<AtomicBarrierMdl<T_Barrier>,
                        AtomicBarrierDev,sizeof(AtomicBarrierMsg<T_Barrier>) >(device, status),
	_gd(&device),
	_barrier (__global.topology_local.size(),
		(__global.topology_local.index2Rank(0) == __global.mapping.task())),
	_loops(32)
        {
		char mmkey[PAMI::Memory::MMKEYSIZE];
		sprintf(mmkey, "/pami-AtomicBarrierMdl-%zd-%zd",
					_gd->clientId(), _gd->contextId());

                // "default" barrier: all local processes...
                //size_t peers = __global.topology_local.size();
                //size_t peer0 = __global.topology_local.index2Rank(0);
                //size_t me = __global.mapping.task();
		// can't validate ctor, can't tell what memory 'this' points to...
		// caller needs to do that. (then caller does this, too?)
		if (!checkDataMm(_gd->getMM())) {
			status = PAMI_INVAL;
			return;
		}

		COMPILE_TIME_ASSERT(T_Barrier::indirect);
		_barrier.init(_gd->getMM(), mmkey);

		_queue.__init(_gd->clientId(), _gd->contextId(), NULL, _gd->getContext(),
						_gd->getMM(), _gd->getAllDevs());

		char *s = getenv("PAMI_ATOMICBARRIER_LOOPS");
		if (s) {
			_loops = strtoul(s, NULL, 0);
			if (!_loops) _loops = 1;
		}
        }

	static bool checkCtorMm(PAMI::Memory::MemoryManager *mm) {
		return true;//T_Barrier::checkCtorMm(mm);
	}

	static bool checkDataMm(PAMI::Memory::MemoryManager *mm) {
		return true;//T_Barrier::checkDataMm(mm);
	}


        inline pami_result_t postMultisyncImmediate_impl(size_t            client,
							 size_t            context,
							 pami_multisync_t *msync,
							 void *devinfo=NULL) 
	{
	  return PAMI_ERROR;
	}

        inline pami_result_t postMultisync_impl(uint8_t (&state)[sizeof_msg],
						size_t            client,
						size_t            context,
                                                pami_multisync_t *msync,
                                                void *devinfo=NULL);

private:
	PAMI::Device::Generic::Device *_gd;
        T_Barrier _barrier;
	AtomicBarrierQue _queue;
	size_t _loops;
}; // class AtomicBarrierMdl

}; //-- Device
}; //-- PAMI

template <class T_Barrier>
inline pami_result_t PAMI::Device::AtomicBarrierMdl<T_Barrier>::postMultisync_impl(uint8_t (&state)[sizeof_msg],
										   size_t            client,
										   size_t            context,  
                                                                                   pami_multisync_t *msync,
                                                                                   void *devinfo) {
        _barrier.begin();
        // See if we can complete the barrier immediately...
        for (size_t x = 0; x < _loops; ++x) {
                if (_barrier.poll() == false) {
                        _barrier.end();
                        if (msync->cb_done.function) {
                                pami_context_t ctx = _gd->getContext();
                                msync->cb_done.function(ctx, msync->cb_done.clientdata, PAMI_SUCCESS);
                        }
                        return PAMI_SUCCESS;
                }
        }
        // must "continue" current barrier, not start new one!
        AtomicBarrierMsg<T_Barrier> *msg;
        msg = new (&state) AtomicBarrierMsg<T_Barrier>(_queue.getQS(), &_barrier, client, context, msync, _loops);
        _queue.__post<AtomicBarrierMsg<T_Barrier> >(msg);
        return PAMI_SUCCESS;
}

#endif //  __components_devices_generic_atomicbarrier_h__
