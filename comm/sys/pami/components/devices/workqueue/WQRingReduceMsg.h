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
 * \file components/devices/workqueue/WQRingReduceMsg.h
 * \brief ???
 */

#ifndef __components_devices_workqueue_WQRingReduceMsg_h__
#define __components_devices_workqueue_WQRingReduceMsg_h__

#include "PipeWorkQueue.h"
#include "components/devices/util/SubDeviceSuppt.h"
#include "components/devices/generic/AdvanceThread.h"
#include "components/devices/FactoryInterface.h"
#include <pami.h>
#include "components/devices/MulticombineModel.h"
#include "math/math_coremath.h"
#include "Global.h"

namespace PAMI {
namespace Device {

class WQRingReduceMdl;
class WQRingReduceMsg;
typedef PAMI::Device::Generic::SimpleAdvanceThread WQRingReduceThr;
typedef PAMI::Device::Generic::MultiSendQSubDevice<WQRingReduceThr,1,true> WQRingReduceQue;
typedef PAMI::Device::Generic::NillSubDevice WQRingReduceDev;

///
/// \brief A local barrier message that takes advantage of the
/// Load Linked and Store Conditional instructions
///
class WQRingReduceMsg : public PAMI::Device::Generic::GenericMessage {
private:
        enum roles {
                NO_ROLE = 0,
                NON_ROOT_ROLE = (1 << 0), // first role must be non-root(s)
                ROOT_ROLE = (1 << 1), // last role must be root
        };

public:
        WQRingReduceMsg(GenericDeviceMessageQueue *Generic_QS,
	        size_t client, size_t context,
                pami_multicombine_t *mcomb,
                PAMI::PipeWorkQueue *iwq,
                PAMI::PipeWorkQueue *swq,
                PAMI::PipeWorkQueue *rwq) :
        PAMI::Device::Generic::GenericMessage(Generic_QS, mcomb->cb_done,
					      client, context),
        _iwq(iwq),
        _swq(swq), // might be NULL
        _rwq(rwq),
        _count(mcomb->count),
        _shift(pami_dt_shift[mcomb->dtype])
        {
                if (_swq) {
                        // full combine...
                        _func = MATH_OP_FUNCS(mcomb->dtype, mcomb->optor, 2);
                } else {
                        // copy only
                        _func = NULL;
                }
        }

        // virtual function
        pami_context_t postNext(bool devQueued) {
		WQRingReduceQue *qs = (WQRingReduceQue *)getQS();
                return qs->__postNext<WQRingReduceMsg>(this, devQueued);
        }

        inline int setThreads(WQRingReduceThr **th) {
		WQRingReduceQue *qs = (WQRingReduceQue *)getQS();
                WQRingReduceThr *t;
                int n;
                qs->__getThreads(&t, &n);
                int nt = 0;
                // PAMI_assert(nt < n);
                _nThreads = 1; // must predict total number of threads
                t[nt].setMsg(this);
                t[nt].setAdv(advanceThread);
                t[nt].setStatus(PAMI::Device::Ready);
                t[nt]._bytesLeft = _count << _shift;
#ifdef USE_WAKEUP_VECTORS
                // not here - but somewhere/somehow...
                __setWakeup(thr);
#endif // USE_WAKEUP_VECTORS
                __advanceThread(&t[nt]);
                ++nt;
                *th = t;
                // PAMI_assert(nt > 0? && nt < n);
                return nt;
        }

protected:
        DECL_ADVANCE_ROUTINE(advanceThread,WQRingReduceMsg,WQRingReduceThr);
        inline pami_result_t __advanceThread(WQRingReduceThr *thr) {
                size_t min = thr->_bytesLeft;
                size_t wq = _rwq->bytesAvailableToProduce();
                if (wq < min) min = wq;
                wq = _iwq->bytesAvailableToConsume();
                if (wq < min) min = wq;
                // _swq != NULL iff _func != NULL...
                if (_swq) {
                        wq = _swq->bytesAvailableToConsume();
                        if (wq < min) min = wq;
                        if (min == 0) {
                                return PAMI_EAGAIN;
                        }
                        void *buf[2] = { _iwq->bufferToConsume(), _swq->bufferToConsume() };
                        _func(_rwq->bufferToProduce(), buf, 2, min >> _shift);
                        _swq->consumeBytes(min);
                } else {
                        if (min == 0) {
                                return PAMI_EAGAIN;
                        }
                        memcpy(_rwq->bufferToProduce(), _iwq->bufferToConsume(), min);
                }
                _iwq->consumeBytes(min);
                _rwq->produceBytes(min);
                thr->_bytesLeft -= min;
                if (thr->_bytesLeft == 0) {
                        // thread is Done, maybe not message
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
        /// \param[in] thr      The thread which wishes to be woken
        ///
        inline void __setWakeup(WQRingReduceThr *thr) {
#ifdef USE_WAKEUP_VECTORS
                void *v = thr->getWakeVec();
                _iwq->setConsumerWakeup(v);
                _rwq->setProducerWakeup(v);
                if (_swq) {
                        _swq->setConsumerWakeup(v);
                }
#endif // USE_WAKEUP_VECTORS
        }

        /// \brief clear a previously set wakeup
        ///
        /// \param[in] thr      The thread which no longer wishes to be woken
        ///
        inline void __clearWakeup(WQRingReduceThr *thr) {
#ifdef USE_WAKEUP_VECTORS
                _iwq->setConsumerWakeup(NULL);
                _rwq->setProducerWakeup(NULL);
                if (_swq) {
                        _swq->setConsumerWakeup(NULL);
                }
#endif // USE_WAKEUP_VECTORS
        }

        unsigned _nThreads;
        PAMI::PipeWorkQueue *_iwq;
        PAMI::PipeWorkQueue *_swq;
        PAMI::PipeWorkQueue *_rwq;
        size_t _count;
        int _shift;
        coremath _func;
}; //-- WQRingReduceMsg
//
class WQRingReduceMdl : public PAMI::Device::Interface::MulticombineModel<WQRingReduceMdl,WQRingReduceDev,sizeof(WQRingReduceMsg)> {
public:
        static const int NUM_ROLES = 2;
        static const int REPL_ROLE = 1;
        static const size_t sizeof_msg = sizeof(WQRingReduceMsg);

        WQRingReduceMdl(WQRingReduceDev &device, pami_result_t &status) :
        PAMI::Device::Interface::MulticombineModel<WQRingReduceMdl,WQRingReduceDev,sizeof(WQRingReduceMsg)>(device, status),
	_gd(&device)
        {
                PAMI::Memory::MemoryManager *mm = _gd->getMM();
                _me = __global.mapping.task();
                size_t t0 = __global.topology_local.index2Rank(0);
                size_t tz;
		char mmkey[PAMI::Memory::MMKEYSIZE];
		char *mms;
		mms = mmkey + sprintf(mmkey, "/WQRingReduceMdl-");
                __global.mapping.nodePeers(tz);
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

        inline void reset_impl() {}

	pami_result_t postMulticombineImmediate_impl(size_t                   client,
						     size_t                   context, 
						     pami_multicombine_t    * mcomb,
						     void                   * devinfo=NULL) 
	{
	  return PAMI_ERROR;
	}

	inline pami_result_t postMulticombine_impl(uint8_t (&state)[sizeof_msg], size_t client, size_t context, pami_multicombine_t *mcomb, void *devinfo=NULL);

private:
	PAMI::Device::Generic::Device *_gd;
	WQRingReduceQue _queue;
        size_t _me;
        PAMI::PipeWorkQueue _wq[PAMI_MAX_PROC_PER_NODE];
}; // class WQRingReduceMdl

 inline pami_result_t WQRingReduceMdl::postMulticombine_impl(uint8_t (&state)[sizeof_msg], size_t client, size_t context, pami_multicombine_t *mcomb, void *devinfo) {
        PAMI::Topology *data_topo = (PAMI::Topology *)mcomb->data_participants;
        PAMI::Topology *results_topo = (PAMI::Topology *)mcomb->results_participants;
        // data_participants will be all local nodes...
        // results_participants should be one only.
        // both MUST be local-only topologies. we don't verify.
        // we keep WQs for all local ranks so that we can adapt
        // to whatever those others are.
        //
        // PAMI_assert_debug(mcomb_info->results_participants->size() == 1);
        //
        // Simple "ring" reduce... .e.g:
        //
        //              (A) ---------> (B)
        //               ^              |
        //               |              |
        //               |              v
        //  "head" ===> (D) <--------- (C) <== "root"
        //
        // ("head" is always the next rank on the ring from "root")
        //
        //  copy     combine   combine   combine
        //   (D) ----> (A) ----> (B) ----> (C) ----> output
        //    ^         ^         ^         ^
        //    |         |         |         |
        //  input     input     input     input
        //
        size_t meix = data_topo->rank2Index(_me);
        size_t meix_1 = meix + 1;
        if (meix_1 >= data_topo->size()) {
                meix_1 -= data_topo->size();
        }
        size_t me_1 = data_topo->index2Rank(meix_1);
        WQRingReduceMsg *msg;
        // this had better match roles...
        if (results_topo->isRankMember(_me)) {
                // I am root - downstream from eveyone.
                // _input (op) _wq[meix_1] => _output
                // PAMI_assert(roles == ROOT_ROLE);
	  msg = new (&state) WQRingReduceMsg(_queue.getQS(), client, context, mcomb,
					     (PAMI::PipeWorkQueue *)mcomb->data, &_wq[meix_1], (PAMI::PipeWorkQueue *)mcomb->results);
        } else if (results_topo->isRankMember(me_1)) {
                // I am head of stream.
                // PAMI_assert(roles == NON_ROOT_ROLE);
#ifdef USE_FLAT_BUFFER
                _wq[meix].reset();
#endif /* USE_FLAT_BUFFER */
                msg = new (&state) WQRingReduceMsg(_queue.getQS(), client, context, mcomb,
                                        (PAMI::PipeWorkQueue *)mcomb->data, NULL, &_wq[meix]);
        } else {
                // I am upstream of root, but not head.
                // PAMI_assert(roles == NON_ROOT_ROLE);
#ifdef USE_FLAT_BUFFER
                _wq[meix].reset();
#endif /* USE_FLAT_BUFFER */
                msg = new (&state) WQRingReduceMsg(_queue.getQS(), client, context, mcomb,
                                        (PAMI::PipeWorkQueue *)mcomb->data, &_wq[meix_1], &_wq[meix]);
        }
        _queue.__post<WQRingReduceMsg>(msg);
        return PAMI_SUCCESS;
}

}; //-- Device
}; //-- PAMI

#endif // __components_devices_workqueue_wqringreducemsg_h__
