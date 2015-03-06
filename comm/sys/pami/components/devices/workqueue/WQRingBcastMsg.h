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
 * \file components/devices/workqueue/WQRingBcastMsg.h
 * \brief ???
 */

#ifndef __components_devices_workqueue_WQRingBcastMsg_h__
#define __components_devices_workqueue_WQRingBcastMsg_h__

#include "components/devices/generic/AdvanceThread.h"
#include "components/devices/util/SubDeviceSuppt.h"
#include <pami.h>
#include "Global.h"
#include "PipeWorkQueue.h"
#include "Topology.h"
#include "components/devices/MulticastModel.h"
#include "components/devices/FactoryInterface.h"

namespace PAMI {
namespace Device {

class WQRingBcastMdl;
class WQRingBcastMsg;
typedef PAMI::Device::Generic::SimpleAdvanceThread WQRingBcastThr;
typedef PAMI::Device::Generic::MultiSendQSubDevice<WQRingBcastThr,1,true> WQRingBcastQue;
typedef PAMI::Device::Generic::NillSubDevice WQRingBcastDev;

///
/// \brief A local bcast message that takes advantage of the
/// shared-memory workqueues
///
class WQRingBcastMsg : public PAMI::Device::Generic::GenericMessage {
private:
        enum roles {
                NO_ROLE = 0,
                ROOT_ROLE = (1 << 0), // first role must be root
                NON_ROOT_ROLE = (1 << 1), // last role must be non-root(s)
        };
public:
        WQRingBcastMsg(GenericDeviceMessageQueue *Generic_QS,
		       size_t            context,
		       size_t            client, 
		       pami_multicast_t *mcast,
		       PAMI::PipeWorkQueue *iwq,
		       PAMI::PipeWorkQueue *swq,
		       PAMI::PipeWorkQueue *rwq) :
        PAMI::Device::Generic::GenericMessage(Generic_QS, mcast->cb_done,
                                client, context),
        _iwq(iwq),
        _swq(swq), // might be NULL
        _rwq(rwq), // might be NULL (but not both)
        _bytes(mcast->bytes)
        {
        }

        // virtual function
        pami_context_t postNext(bool devQueued) {
		WQRingBcastQue *qs = (WQRingBcastQue *)getQS();
                return qs->__postNext<WQRingBcastMsg>(this, devQueued);
        }

        inline int setThreads(WQRingBcastThr **th) {
		WQRingBcastQue *qs = (WQRingBcastQue *)getQS();
                WQRingBcastThr *t;
                int n;
                qs->__getThreads(&t, &n);
                int nt = 0;
                // PAMI_assert(nt < n);
                _nThreads = 1; // must predict total number of threads
                t[nt].setMsg(this);
                t[nt].setAdv(advanceThread);
                t[nt].setStatus(PAMI::Device::Ready);
                t[nt]._bytesLeft = _bytes;
#ifdef USE_WAKEUP_VECTORS
                // not here - but somewhere/somehow...
                __setWakeup(thr);
#endif // USE_WAKEUP_VECTORS
                __advanceThread(&t[nt]);
                ++nt;
                // PAMI_assert(nt > 0? && nt < n);
                *th = t;
                return nt;
        }

protected:
        DECL_ADVANCE_ROUTINE(advanceThread,WQRingBcastMsg,WQRingBcastThr);
        inline pami_result_t __advanceThread(WQRingBcastThr *thr) {
                size_t min = thr->_bytesLeft;
                size_t wq = _iwq->bytesAvailableToConsume();
                if (wq < min) min = wq;
                if (_swq) {
                        wq = _swq->bytesAvailableToProduce();
                        if (wq < min) min = wq;
                }
                if (_rwq) {
                        wq = _rwq->bytesAvailableToProduce();
                        if (wq < min) min = wq;
                }
                if (min == 0) {
                        return PAMI_EAGAIN;
                }
                if (_rwq) {
                        memcpy(_rwq->bufferToProduce(), _iwq->bufferToConsume(), min);
                        _rwq->produceBytes(min);
                }
                if (_swq) {
                        memcpy(_swq->bufferToProduce(), _iwq->bufferToConsume(), min);
                        _swq->produceBytes(min);
                }
                _iwq->consumeBytes(min);
                thr->_bytesLeft -= min;
                if (thr->_bytesLeft == 0) {
                        thr->setStatus(PAMI::Device::Complete);
#ifdef USE_WAKEUP_VECTORS
                        __clearWakeup(thr);
#endif // USE_WAKEUP_VECTORS
                        setStatus(PAMI::Device::Done);
                        return PAMI_SUCCESS;
                }
                return PAMI_EAGAIN;
        }

        /// \brief arrange to be woken up when inputs/outputs become "ready"
        ///
        /// This is very simple - we wake up if any input/output becomes
        /// ready and it might be a false wakeup if any other input/output
        /// is not (yet) ready.
        ///
        /// \param[in] thr	The thread which wishes to be woken
        ///
        inline void __setWakeup(WQRingBcastThr *thr) {
#ifdef USE_WAKEUP_VECTORS
                void *v = thr->getWakeVec();
                _iwq->setConsumerWakeup(v);
                if (_rwq) {
                        _rwq->setProducerWakeup(v);
                }
                if (_swq) {
                        _swq->setProducerWakeup(v);
                }
#endif // USE_WAKEUP_VECTORS
        }

        /// \brief clear a previously set wakeup
        ///
        /// \param[in] thr	The thread which no longer wishes to be woken
        ///
        inline void __clearWakeup(WQRingBcastThr *thr) {
#ifdef USE_WAKEUP_VECTORS
                _iwq->setConsumerWakeup(NULL);
                if (_rwq) {
                        _rwq->setProducerWakeup(NULL);
                }
                if (_swq) {
                        _swq->setProducerWakeup(NULL);
                }
#endif // USE_WAKEUP_VECTORS
        }

        unsigned _nThreads;
        PAMI::PipeWorkQueue *_iwq;
        PAMI::PipeWorkQueue *_swq;
        PAMI::PipeWorkQueue *_rwq;
        size_t _bytes;
}; //-- WQRingBcastMsg

class WQRingBcastMdl : public PAMI::Device::Interface::MulticastModel<WQRingBcastMdl,WQRingBcastDev,sizeof(WQRingBcastMsg)> {
public:
        static const int NUM_ROLES = 2;
        static const int REPL_ROLE = 1;
        static const size_t sizeof_msg = sizeof(WQRingBcastMsg);

        WQRingBcastMdl(WQRingBcastDev &device, pami_result_t &status) :
        PAMI::Device::Interface::MulticastModel<WQRingBcastMdl,WQRingBcastDev,sizeof(WQRingBcastMsg)>(device, status),
	_gd(&device)
        {
                PAMI::Memory::MemoryManager *mm = _gd->getMM();
                _me = __global.mapping.task();
                size_t t0 = __global.topology_local.index2Rank(0);
                size_t tz;
                __global.mapping.nodePeers(tz);
		char mmkey[PAMI::Memory::MMKEYSIZE];
		char *mms;
		mms = mmkey + sprintf(mmkey, "/WQRingBcastMdl-");
                for (size_t x = 0; x < tz; ++x) {
			sprintf(mms, "%zd", x);
#ifdef USE_FLAT_BUFFER
                        _wq[x].configure_no_reset(mm, mmkey, USE_FLAT_BUFFER, 0);
#else /* ! USE_FLAT_BUFFER */
                        _wq[x].configure_no_reset(mm, mmkey, 8192);
#endif /* ! USE_FLAT_BUFFER */
                        _wq[x].barrier_reset(tz, _me == t0);
                }
		_queue.__init(_gd->clientId(), _gd->contextId(), NULL, _gd->getContext(), _gd->getMM(), _gd->getAllDevs());
        }

        inline pami_result_t postMulticastImmediate_impl(size_t            client,
							 size_t            context, 
							 pami_multicast_t *mcast,
							 void             *devinfo=NULL)
	{
	  return PAMI_ERROR;
	}

        inline pami_result_t postMulticast_impl(uint8_t (&state)[sizeof_msg],
						size_t            client,
						size_t            context, 
                                                pami_multicast_t *mcast,
                                                void *devinfo);

private:
	PAMI::Device::Generic::Device *_gd;
	WQRingBcastQue _queue;
        size_t _me;
        PAMI::PipeWorkQueue _wq[PAMI_MAX_PROC_PER_NODE];
}; // class WQRingBcastMdl

inline pami_result_t WQRingBcastMdl::postMulticast_impl(uint8_t (&state)[sizeof_msg],
							size_t            client,
							size_t            context,   
                                                        pami_multicast_t *mcast,
                                                        void * devinfo) {
        PAMI::Topology *dst_topo = (PAMI::Topology *)mcast->dst_participants;
        PAMI::Topology *src_topo = (PAMI::Topology *)mcast->src_participants;
        size_t root = src_topo->index2Rank(0); // assert size(0 == 1...
        bool iamroot = (root == _me);
        bool iamlast = false;
        // root may not be member of dst_topo!
        size_t meix_1;
        size_t me_1;
        size_t meix = dst_topo->rank2Index(_me);
        if (meix == (size_t)-1) {
                PAMI_assertf(iamroot, "WQRingBcastMdl::postMulticast called by non-participant\n");
                // meix = 0; // not used
                meix_1 = 0; // output to _wq[0]
        } else {
                // input from _wq[meix], output to _wq[meix+1]...
                // this only gets funky if the root is also a member of dst_topo...
                meix_1 = meix + 1;
                if (meix_1 >= dst_topo->size()) {
                        meix_1 -= dst_topo->size();
                }
                me_1 = dst_topo->index2Rank(meix_1);
                // if my downstream is "root", or if root is not in dst_topo and
                if (dst_topo->isRankMember(root)) {
                        iamlast = (me_1 == root);
                } else {
                        iamlast = (meix_1 == 0);
                }
        }
        WQRingBcastMsg *msg;
        if (iamroot) {      // I am root
                // I am root - at head of stream
                // PAMI_assert(roles == ROOT_ROLE);
                // _input ===> _wq[meix]
#ifdef USE_FLAT_BUFFER
                _wq[meix_1].reset();
#endif /* USE_FLAT_BUFFER */
                msg = new (&state) WQRingBcastMsg(_queue.getQS(), client, context, mcast,
                                        (PAMI::PipeWorkQueue *)mcast->src, &_wq[meix_1], NULL);
        } else if (iamlast) {
                // I am tail of stream - no one is downstream from me.
                // PAMI_assert(roles == NON_ROOT_ROLE);
                // _wq[meix_1] ===> results
	  msg = new (&state) WQRingBcastMsg(_queue.getQS(), client, context, mcast,
                                        &_wq[meix], NULL, (PAMI::PipeWorkQueue *)mcast->dst);
        } else {
                // PAMI_assert(roles == NON_ROOT_ROLE);
                // _wq[meix_1] =+=> results
                //              +=> _wq[meix]
#ifdef USE_FLAT_BUFFER
                _wq[meix_1].reset();
#endif /* USE_FLAT_BUFFER */
                msg = new (&state) WQRingBcastMsg(_queue.getQS(), client, context, mcast,
                                        &_wq[meix], &_wq[meix_1], (PAMI::PipeWorkQueue *)mcast->dst);
        }
        _queue.__post<WQRingBcastMsg>(msg);
        return PAMI_SUCCESS;
}

}; //-- Device
}; //-- PAMI

#endif // __components_devices_workqueue_wqringbcastmsg_h__
