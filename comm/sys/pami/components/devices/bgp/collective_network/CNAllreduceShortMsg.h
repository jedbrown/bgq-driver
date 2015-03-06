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
 * \file components/devices/bgp/collective_network/CNAllreduceShortMsg.h
 * \brief Default collective network allreduce interface.
 */
#ifndef __components_devices_bgp_collective_network_CNAllreduceShortMsg_h__
#define __components_devices_bgp_collective_network_CNAllreduceShortMsg_h__

#include "util/common.h"
#include "components/devices/bgp/collective_network/CNDevice.h" // externs for env vars
#include "components/devices/bgp/collective_network/CNAllreduce.h"
#include "components/devices/bgp/collective_network/CollectiveNetworkLib.h"
#include "components/devices/bgp/collective_network/CNPacket.h"
#include "components/devices/generic/Device.h"
#include "components/devices/generic/Message.h"
#include "components/devices/generic/AdvanceThread.h"

namespace PAMI {
namespace Device {
namespace BGP {

/**
 * \brief Collective Network Allreduce Short-protocol Message base class
 *
 * Collective Network Allreduce with source/dest as pipe WQ.
 *
 * Note, normally these messages will never be posted on the device.  However,
 * we can't ensure they won't need to be queued swo we can't optimize that away.
 * But we optimize for the expected case and pay a bigger penalty if we have to post.
 *
 * We do know that each message/process will have only one role, so we can optimize that.
 * We can also optimize for one (no) thread.
 */

class CNAllreduceShortModel;
class CNAllreduceShortMessage;
typedef PAMI::Device::BGP::BaseGenericCNThread CNAllreduceShortThread;
typedef PAMI::Device::Generic::SharedQueueSubDevice<CNAllreduceShortModel,CNDevice,CNAllreduceShortMessage,CNAllreduceShortThread,2> CNAllreduceShortDevice;

};	// BGP
};	// Device
};	// PAMI

extern PAMI::Device::BGP::CNAllreduceShortDevice _g_cnallreduceshort_dev;

namespace PAMI {
namespace Device {
namespace BGP {

class CNAllreduceShortMessage : public PAMI::Device::BGP::BaseGenericCNMessage {
        enum roles {
                NO_ROLE = 0,
                INJECTION_ROLE = (1 << 0), // first role must be "injector"
                RECEPTION_ROLE = (1 << 1), // last role must be "receptor"
                LOCAL_ROLE = (1 << 2), // local-only work
        };
public
        CNAllreduceShortMessage(GenericDeviceMessageQueue &qs,
                        pami_multicombine_t *mcomb,
                        size_t bytes,
                        bool doStore,
                        unsigned dispatch_id,
                        PAMI::Device::BGP::CNAllreduceSetup &tas) :
        BaseGenericCNMessage(qs, mcomb->client, mcomb->context,
                                (PAMI::PipeWorkQueue *)mcomb->data,
                                (PAMI::PipeWorkQueue *)mcomb->results,
                                bytes, doStore, mcomb->roles, mcomb->cb_done,
                                dispatch_id, tas._hhfunc, tas._opsize),
        _roles(roles)
        {
        }

        STD_POSTNEXT(CNAllreduceShortDevice,CNAllreduceShortThread,&_g_cnallreduceshort_dev)

        inline pami_result_t advanceThread(pami_context_t context, void *t);
protected:
        //friend class CNAllreduceShortDevice;
        friend class PAMI::Device::Generic::SharedQueueSubDevice<CNAllreduceShortModel,CNDevice,CNAllreduceShortMessage,CNAllreduceShortThread,2>;

        ADVANCE_ROUTINE(advanceInj,CNAllreduceShortMessage,CNAllreduceShortThread);
        ADVANCE_ROUTINE(advanceRcp,CNAllreduceShortMessage,CNAllreduceShortThread);
        inline int __setThreads(CNAllreduceShortThread *t, int n) {
                int nt = 0;
                _g_cnallreduceshort_dev.common()->__resetThreads();
                _nThreads = ((_roles & INJECTION_ROLE) != 0) + ((_roles & RECEPTION_ROLE) != 0);
                if (_roles & INJECTION_ROLE) {
                        t[nt].setMsg(this);
                        t[nt].setAdv(advanceInj);
                        t[nt].setStatus(PAMI::Device::Ready);
                        t[nt]._wq = _swq;
                        t[nt]._bytesLeft = _bytes;
                        t[nt]._cycles = 1;
                        __advanceInj(&t[nt]);
                        ++nt;
                }
                if (_roles & RECEPTION_ROLE) {
                        t[nt].setMsg(this);
                        t[nt].setAdv(advanceRcp);
                        t[nt].setStatus(PAMI::Device::Ready);
                        t[nt]._wq = _rwq;
                        t[nt]._bytesLeft = _bytes;
                        t[nt]._cycles = 3000;
                        __advanceRcp(&t[nt]);
                        ++nt;
                }
                // PAMI_assert(nt > 0? && nt < n);
                return nt;
        }

        inline void __completeThread(CNAllreduceShortThread *thr);

        ///
        /// Optimize for injecting entire message right now - or else do nothing.
        ///
        ///
        ///
        inline pami_result_t __advanceInj(CNAllreduceShortThread *thr) {
                unsigned hcount = TREE_FIFO_SIZE, dcount = TREE_QUADS_PER_FIFO;
                size_t avail = thr->_wq->bytesAvailableToConsume();
                char *buf = thr->_wq->bufferToConsume();
                bool aligned = (((unsigned)buf & 0x0f) == 0);
                size_t did = 0;
                if (avail < TREE_PKT_SIZE && avail < thr->_bytesLeft) {
                        return PAMI_EAGAIN;
                }
                // is this possible??
                if (avail > thr->_bytesLeft) avail = thr->_bytesLeft;
                if (__wait_send_fifo_to(thr, hcount, dcount, thr->_cycles)) {
                        return PAMI_EAGAIN;
                }
                __send_whole_packets(thr, hcount, dcount, avail, did, buf, aligned);
                __send_last_packet(thr, hcount, dcount, avail, did, buf, aligned);
                thr->_wq->consumeBytes(did);
                if (thr->_bytesLeft == 0) {
                        thr->setStatus(PAMI::Device::Complete);
                        __completeThread(thr);
                        return PAMI_SUCCESS;
                }
                return PAMI_EAGAIN;
        }


        inline pami_result_t __advanceRcp(CNAllreduceShortThread *thr) {
                if (thr->_bytesLeft == 0) return PAMI_SUCCESS;
                unsigned hcount = 0, dcount = 0;
                unsigned toCopy = thr->_bytesLeft >= TREE_PKT_SIZE ? TREE_PKT_SIZE : thr->_bytesLeft;
                size_t avail = thr->_wq->bytesAvailableToProduce();
                char *buf = thr->_wq->bufferToProduce();
                bool aligned = (((unsigned)buf & 0x0f) == 0);
                size_t did = 0;
                if (avail < toCopy) {
                        return PAMI_EAGAIN;
                }
                if (__wait_recv_fifo_to(thr, hcount, dcount, thr->_cycles)) {
                        return PAMI_EAGAIN;
                }
                __recv_whole_packets(thr, hcount, dcount, avail, did, buf, aligned);
                __recv_last_packet(thr, hcount, dcount, avail, did, buf, aligned);
                thr->_wq->produceBytes(did);
                if (thr->_bytesLeft == 0) {
                        thr->setStatus(PAMI::Device::Complete);
                        __completeThread(thr);
                        return PAMI_SUCCESS;
                }
                return PAMI_EAGAIN;
        }

        static inline bool __inject_msg(PAMI::Device::WorkQueue::SharedWorkQueue *swqs,
                        PAMI::Device::WorkQueue::SharedWorkQueue *rwq,
                        char **bufs, char *sbuf,
                        char *results, size_t bytes, size_t count,
                        coremath _func, unsigned peer, unsigned npeers, unsigned &ndone
                        unsigned dispatch_id, PAMI::Device::BGP::CNAllreduceSetup &tas,
                        bool doStore) {
                // need to do a "while (__tsc() - t0 < cycles)..."
                ndone = (1 << peer);
                unsigned done = (1 << npeers) - 1;
                unsigned i, curr;
                // avg 700 cycles (also depends on arrival times)
                while (ndone < done) {
                        for (i = 0; i < npeers; ++i) {
                                curr = (1 << i);
                                if (!(ndone & curr) &&
                                    swqs[i].bytesAvailableToConsume() >= bytes) {
                                        ndone |= curr;
                                }
                        }
                }
                // ~ 200-900 cycles (over range of _count)
                func((void *)sbuf, (void **)bufs, npeers, count);

                // ~170 cycles...
                for (i = 0; i < npeers; ++i) {
                        if (i != peer) {
                                swqs[i]->consumeBytes(MAX_SHORT_MSG);
                        }
                }
                // Now, send "_sbuffer" on CN...
                while (bytes > 0) {
                        rc = TreeAllreduceMessage::advance((unsigned)-1, ctx);
                        if (rc == PAMI::Device::Tree::Done) {
                                break;
                        }
                }

                // wait for, and "recv", our data from Collective Network Recv core...
                // This effectively provides a barrier, so we do it
                // even if we don't want the data.  In that case it's
                // only byte counts and no data is moved.
                while (rwq->bytesAvailableToConsume(peer) < bytes);
                if (doStore) {
                        memcpy(results, rwq->bufferToConsume(peer), bytes);
                }
                rwq->consumeBytes(MAX_SHORT_MSG, peer);
                return true;
        }

        static inline bool __inject_msg_pre(PAMI::Device::WorkQueue::SharedWorkQueue *swqs,
                        PAMI::Device::WorkQueue::SharedWorkQueue *rwq,
                        char **bufs, char *sbuf,
                        char *results, size_t bytes, size_t count,
                        coremath _func, unsigned peer, unsigned npeers, unsigned &ndone
                        unsigned dispatch_id, PAMI::Device::BGP::CNAllreduceSetup &tas,
                        bool doStore) {
                // need to do a "while (__tsc() - t0 < cycles)..."
                ndone = (1 << peer);
                unsigned done = (1 << npeers) - 1;
                unsigned i, curr;
                // avg 700 cycles (also depends on arrival times)
                while (ndone < done) {
                        for (i = 0; i < npeers; ++i) {
                                curr = (1 << i);
                                if (!(ndone & curr) &&
                                    swqs[i].bytesAvailableToConsume() >= bytes) {
                                        ndone |= curr;
                                }
                        }
                }
                // ~ 200-900 cycles (over range of _count)
                func((void *)sbuf, (void **)bufs, npeers, count);

                // ~170 cycles...
                for (i = 0; i < npeers; ++i) {
                        if (i != peer) {
                                swqs[i].consumeBytes(MAX_SHORT_MSG);
                        }
                }
                // Now, send "_mybuf" on CN...
                while (bytes > 0) {
                        rc = TreeAllreducePreMessage::advance((unsigned)-1, ctx);
                        if (rc == PAMI::Device::Tree::Done) {
                                break;
                        }
                }

                // wait for, and "recv", our data from Collective Network Recv core...
                // This effectively provides a barrier, so we do it
                // even if we don't want the data.  In that case it's
                // only byte counts and no data is moved.
                while (rwq->bytesAvailableToConsume(peer) < bytes);
                if (doStore) {
                            memcpy(results, rwq->bufferToConsume(peer), bytes);
                }
                rwq->consumeBytes(MAX_SHORT_MSG, peer);
                return true;
        }

        static inline bool __recept_msg(PAMI::Device::WorkQueue::SharedWorkQueue *swq,
                        PAMI::Device::WorkQueue::SharedWorkQueue *rwq,
                        char *data, char *results, size_t bytes, unsigned peer,
                        unsigned dispatch_id, PAMI::Device::BGP::CNAllreduceSetup &tas,
                        bool &primed, bool doStore, bool doBcast) {
                if (!primed) {
                        memcpy(swq->bufferToProduce(), data, bytes);
                        swq->produceBytes(MAX_SHORT_MSG);
                        primed = true;
                }
                do {
                        rc = TreeAllreduceRecvMessage::advance((unsigned)-1, ctx);
                } while (rc != PAMI::Device::Tree::Done);

                // This effectively provides a barrier, so we do it
                // even if we don't want the data.  In that case it's
                // only byte counts and no data is moved.
                rwq->produceBytes(MAX_SHORT_MSG);
                if (doStore) {
                        memcpy(results, rwq->bufferToConsume(), bytes);
                }
                rwq->consumeBytes(MAX_SHORT_MSG, peer);
                return true;
        }

        static inline bool __recept_msg_post(PAMI::Device::WorkQueue::SharedWorkQueue *swq,
                        PAMI::Device::WorkQueue::SharedWorkQueue *rwq,
                        char *data, char *results, size_t bytes, unsigned peer,
                        unsigned dispatch_id, PAMI::Device::BGP::CNAllreduceSetup &tas,
                        bool &primed, bool doStore, bool doBcast) {
                if (!primed) {
                        memcpy(swq->bufferToProduce(), data, bytes);
                        swq->produceBytes(MAX_SHORT_MSG);
                        primed = true;
                }
                do {
                        rc = TreeAllreduceRecvPostMessage::advance((unsigned)-1, ctx);
                } while (rc != PAMI::Device::Tree::Done);
                // This effectively provides a barrier, so we do it
                // even if we don't want the data.  In that case it's
                // only byte counts and no data is moved.
                rwq->produceBytes(MAX_SHORT_MSG);
                if (doStore) {
                        memcpy(results, rwq->bufferToConsume, bytes);
                }
                rwq->consumeBytes(MAX_SHORT_MSG, peer);
                return true;
        }

        /// This function does not return until done
        static inline bool __local_msg(PAMI::Device::WorkQueue::SharedWorkQueue *swq,
                        PAMI::Device::WorkQueue::SharedWorkQueue *rwq,
                        char *data, char *results, size_t bytes, unsigned peer,
                        bool &primed, bool doStore) {
                if (!primed) {
                        memcpy(swq->bufferToProduce(), data, bytes);
                        swq->produceBytes(MAX_SHORT_MSG);
                        primed = true;
                }
                // wait for, and "recv", our data from Collective Network Recv core...
                // This effectively provides a barrier, so we do it
                // even if we don't want the data.  In that case it's
                // only byte counts and no data is moved.
                while (rwq->bytesAvailableToConsume(peer) < bytes);
                if (doBcast) {
                            memcpy(results, rwq->bufferToConsume(peer), bytes);
                }
                rwq->consumeBytes(MAX_SHORT_MSG, _peer);
                return true;
        }

        unsigned _roles;
}; // class CNAllreduceShortMessage

class CNAllreduceShortModel : public Impl::MulticombineModelImpl {
public:
        static const int NUM_ROLES = 3;
        static const int REPL_ROLE = 2;
        static const size_t sizeof_msg = sizeof(CNAllreduceShortMessage);

        static const int MAX_SHORT_MSG = TREE_PKT_SIZE;

        CNAllreduceShortModel(PAMI::SysDep *sysdep, pami_result_t &status) :
        Impl::MulticombineModelImpl(status),
        _swq(sysdep, NUM_CORES, MAX_SHORT_MSG)
        {
                _dispatch_id = _g_cnallreduceshort_dev.newDispID();
                _me = __global.mapping().rank();
                _npeers = __global.topology_local->size();
                _me_ix = __global.topology_local->rank2Index(_me);
                // at least one must do this
                PAMI::Device::BGP::CNAllreduceSetup::initCNAS();
                unsigned i;
                _swq.reset();
                for (i = 0; i < NUM_CORES; ++i) {
                        _bufs[i] = _swqs[i].bufferToConsume();
                }
        }

        inline bool postMulticombine_impl(CNAllreduceShortMessage *msg);

private:
        size_t _me;
        unsigned _npeers;
        unsigned _me_ix;
        unsigned _dispatch_id;
        PAMI::Device::WorkQueue::SharedWorkQueue _swqs[NUM_CORES];
        PAMI::Device::WorkQueue::SharedWorkQueue _rwq;
        char *_bufs[NUM_CORES];
        char _sbuffer[MAX_SHORT_MSG] __attribute__((__aligned__(16)));
        static inline void compile_time_assert () {
                COMPILE_TIME_ASSERT(sizeof(pami_request_t) >= sizeof(CNAllreduceShortMessage));
        }
}; // class CNAllreduceShortModel

inline void CNAllreduceShortMessage::__completeThread(CNAllreduceShortThread *thr) {
        unsigned c = _g_cnallreduceshort_dev.common()->__completeThread(thr);
        if (c >= _nThreads) {
                setStatus(PAMI::Device::Done);
        }
}

pami_result_t CNAllreduceShortMessage::advanceThread(pami_context_t context, void *t) {
        return __advanceThread((CNAllreduceShortThread *)t);
}

///
/// Permit a NULL results_topo to mean "everyone" (i.e. "root == -1")
///
/// If this routine is not externally protected from being called from
/// multiple threads with the intent of operating different allreduces,
/// then this code will have to determine 'first' differently (thread-safe).
///
inline bool CNAllreduceShortModel::postMulticombine_impl(CNAllreduceShortMessage *msg) {
        size_t bytes = _getCount() << pami_dt_shift[_getDt()];
        if (bytes > MAX_SHORT_MSG) {
                return false;
        }
        PAMI::Device::BGP::CNAllreduceSetup &tas = PAMI::Device::BGP::CNAllreduceSetup::getCNAS(_getDt(), _getOp());
        PAMI::Topology *results_topo = (PAMI::Topology *)_getResultsRanks();
        bool doStore = (!results_topo || results_topo->isRankMember(_me));
        bool doBcast = (!results_topo || results_topo->size() > 1);

        bool first = (_g_cnallreduceshort_dev.common()->getCurrent() == NULL);
        bool done = false;
        bool primed = false;
        if (first && _getData()->bytesAvailableToConsume() >= bytes &&
                        _getResults()->bytesAvailableToProduce() >= bytes) {
                PAMI:PipeWorkQueue *data = (PAMI:PipeWorkQueue *)_getData();
                PAMI:PipeWorkQueue *results = (PAMI:PipeWorkQueue *)_getResults();
                if (_getRoles() & CNAllreduceShortMessage::INJECTION_ROLE) {
                        _bufs[_me_ix] = data->bufferToConsume();
                        coremath func = MATH_OP_FUNCS(_getDt(), _getOp(), _npeers);
                        unsigned ndone = 0;
                        if (tas._pre) {
                                done = CNAllreduceShortMessage::__inject_msg_pre(_swqs, &_rwq, _bufs, _sbuf,
                                        results->bufferToProduce(),
                                        bytes, _getcount(), func, _me_ix, _npeers, ndone _dispatch_id, tas, doStore);
                        } else {
                                done = CNAllreduceShortMessage::__inject_msg(_swqs, &_rwq, _bufs, _sbuf,
                                        results->bufferToProduce(),
                                        bytes, _getcount(), func, _me_ix, _npeers, ndone _dispatch_id, tas, doStore);
                        }
                } else if (_getRoles() & CNAllreduceShortMessage::RECEPTION_ROLE) {
                        if (tas._pre) {
                                done = CNAllreduceShortMessage::__recept_msg_post(&_swq[_me_ix], &_rwq,
                                        data->bufferToConsume(), results->bufferToProduce(),
                                        _dispatch_id, tas, _me_ix, doStore, doBcast);
                        } else {
                                done = CNAllreduceShortMessage::__recept_msg(&_swq[_me_ix], &_rwq,
                                        data->bufferToConsume(), results->bufferToProduce(),
                                        _dispatch_id, tas, _me_ix, doStore, doBcast);
                        }
                } else /* if (roles & CNAllreduceShortMessage::LOCAL_ROLE) */ {
                        done = CNAllreduceShortMessage::__local_msg(&_swq[_me_ix], &_rwq,
                                        data->bufferToConsume(), results->bufferToProduce(),
                                        bytes, _me_ix, primed, doStore);

                }
                if (done) {
                        if (_getCallback().function) {
                                _getCallback().function(_getCallback().clientdata, NULL);
                        }
                        return true;
                }
                // we should never get here... as it works now...
                // otherwise, we need to take the context of the (partial) advance
                // above and pass it to the message constructor.
                PAMI_abort();
        }

        // could try to complete allreduce before construction, but for now the code
        // is too dependent on having message and thread structures to get/keep context.
        // __post() will still try early advance... (after construction)
        // details TBD...
        new (msg) CNAllreduceShortMessage(*_g_cnallreduceshort_dev.common(),
                        mcomb,
                        bytes, doStore, _dispatch_id, tas
                        // context from possible pre-ctor advance???
                        );
        _g_cnallreduceshort_dev.__post(msg);
        return true;
}


};	// BGP
};	// Device
};	// PAMI

#endif
#error CNAllreduceShortMsg.h is not ready

/**
 * \brief Collective Network Allreduce Send Message for Short allreduces
 *
 * Based on a TreeAllreduceMessage.
 */
class TreeAllreduceShortSendMessage :
                public TreeAllreduceMessage {
public:

        /**
         * \brief Constructor for Tree Allreduce Send Message for Short allreduces
         *
         * \param[in] TreeQS	Tree Device
         * \param[in] swqs	Array of WQ used by local nodes to "send" us data
         * \param[in] rwq	WQ to use for receiving results
         * \param[in] root	Root node or -1 for allreduce
         * \param[in] peer	Our local node ID
         * \param[in] npeers	Number of local nodes
         * \param[in] func	Math function to use (4-way)
         * \param[in] bytes	Bytes to send from source
         * \param[in] consistency Consistency model for the transfer
         * \param[in] classroute Tree classroute value
         * \param[in] cb	Completion callback
         * \param[in] sbuffer	Source/Send buffer
         * \param[in] rbuffer	Dest/Recv buffer
         * \param[in] dt	Datatype
         * \param[in] op	Operation
         * \param[in] count	Number of elements
         * \param[in] dispatch_id Id for packet header
         * \param[in] tas	Tree Allreduce info
         */
        inline TreeAllreduceShortSendMessage(
                PAMI::Device::Tree::Device    &TreeQS,
                PAMI::Device::WorkQueue::SharedWorkQueue **swqs,
                PAMI::Device::WorkQueue::SharedWorkQueue *rwq,
                unsigned           root,
                unsigned           peer,
                unsigned           npeers,
                coremath           func,
                unsigned           bytes,
                PAMI_Consistency   consistency,
                unsigned           classroute,
                pami_callback_t    cb,
                char             * sbuffer,
                char             * rbuffer,
                pami_dt		dt,
                pami_op		op,
                unsigned           count,
                unsigned           dispatch_id,
                TreeAllreduceSetup tas) :
        TreeAllreduceMessage(TreeQS, root,
                swqs[peer]->bufferToProduce(),
                bytes, dt, op, classroute,
                dispatch_id, cb, tas),
        _swqs (swqs),
        _rwq (rwq),
        _ibuffer (sbuffer),
        _obuffer (rbuffer),
        _peer (peer),
        _npeers (npeers),
        _bytesIn (bytes),
        _count (count),
        _func (func),
        _doBcast(root == (unsigned)-1) {
                // ~300 cycles. This code depends on
                // always consuming/producing the full WQ size.
                // (i.e. bufferToConsume() must be constant)
                unsigned i;
                for (i = 0; i < _npeers; ++i) {
                        if (i == _peer) _bufs[i] = _ibuffer;
                        else _bufs[i] = _swqs[i]->bufferToConsume();
                }
                //  internal_restart ();
        };

        int reset(unsigned collid=0) { return internal_restart(collid); }

        int advance(unsigned cycles, PAMI::Device::Tree::TreeMsgContext ctx);
private:

        inline int internal_restart(unsigned collid) {
                TreeAllreduceMessage::reset(collid);
                _ndone = 0;
                return 0;
        }

        PAMI::Device::WorkQueue::SharedWorkQueue **_swqs;
        PAMI::Device::WorkQueue::SharedWorkQueue *_rwq;
        char *_ibuffer;
        char *_obuffer;
        unsigned _peer;
        unsigned _npeers;
        unsigned _bytesIn;
        unsigned _count;
        coremath _func;
        bool _doBcast;
        unsigned _ndone;
        char *_bufs[NUM_CORES];
}; /* TreeAllreduceShortSendMessage */

/**
 * \brief Tree Allreduce Send Message for Short allreduces with pre-processing
 */
class TreeAllreduceShortSendPreMessage :
                public TreeAllreducePreMessage {
public:

        /**
         * \brief Constructor for Tree Allreduce Send Message for Short allreduces with pre-processing
         *
         * \param[in] TreeQS	Tree Device
         * \param[in] swqs	Array of WQ used by local nodes to "send" us data
         * \param[in] rwq	WQ to use for receiving results
         * \param[in] root	Root node or -1 for allreduce
         * \param[in] peer	Our local node ID
         * \param[in] npeers	Number of local nodes
         * \param[in] func	Math function to use (4-way)
         * \param[in] bytes	Bytes to send from source
         * \param[in] consistency Consistency model for the transfer
         * \param[in] classroute Tree classroute value
         * \param[in] cb	Completion callback
         * \param[in] sbuffer	Source/Send buffer
         * \param[in] rbuffer	Dest/Recv buffer
         * \param[in] dt	Datatype
         * \param[in] op	Operation
         * \param[in] count	Number of elements
         * \param[in] dispatch_id Id for packet header
         * \param[in] tas	Tree Allreduce info
         */
        inline TreeAllreduceShortSendPreMessage(
                PAMI::Device::Tree::Device    &TreeQS,
                PAMI::Device::WorkQueue::SharedWorkQueue **swqs,
                PAMI::Device::WorkQueue::SharedWorkQueue *rwq,
                unsigned           root,
                unsigned           peer,
                unsigned           npeers,
                coremath           func,
                unsigned           bytes,
                PAMI_Consistency   consistency,
                unsigned           classroute,
                pami_callback_t    cb,
                char             * sbuffer,
                char             * rbuffer,
                pami_dt		dt,
                pami_op		op,
                unsigned           count,
                unsigned           dispatch_id,
                TreeAllreduceSetup tas) :
        TreeAllreducePreMessage(TreeQS, root,
                swqs[peer]->bufferToProduce(),
                bytes, dt, op, classroute,
                dispatch_id, cb, tas),
        _swqs (swqs),
        _rwq (rwq),
        _ibuffer (sbuffer),
        _obuffer (rbuffer),
        _peer (peer),
        _npeers (npeers),
        _bytesIn (bytes),
        _count (count),
        _func (func),
        _doBcast(root == (unsigned)-1) {
                // ~300 cycles. This code depends on
                // always consuming/producing the full WQ size.
                // (i.e. bufferToConsume() must be constant)
                unsigned i;
                for (i = 0; i < _npeers; ++i) {
                        if (i == _peer) _bufs[i] = _ibuffer;
                        else _bufs[i] = _swqs[i]->bufferToConsume();
                }
                //  internal_restart();
        };

        int reset(unsigned collid=0) { return internal_restart(collid); }

        int advance(unsigned cycles, PAMI::Device::Tree::TreeMsgContext ctx);
private:

        inline int internal_restart(unsigned collid) {
                TreeAllreducePreMessage::reset(collid);
                _ndone = 0;
                return 0;
        }

        PAMI::Device::WorkQueue::SharedWorkQueue **_swqs;
        PAMI::Device::WorkQueue::SharedWorkQueue *_rwq;
        char *_ibuffer;
        char *_obuffer;
        unsigned _peer;
        unsigned _npeers;
        unsigned _bytesIn;
        unsigned _count;
        coremath _func;
        bool _doBcast;
        unsigned _ndone;
        char *_bufs[NUM_CORES];
}; /* TreeAllreduceShortSendPreMessage */

/**
 * \brief Tree Allreduce Recv Message for Short allreduces
 */
class TreeAllreduceShortRecvMessage :
                public TreeAllreduceRecvMessage {
public:

        /**
         * \brief Constructor for Tree Allreduce Recv Message for Short allreduces
         *
         * \param[in] TreeQS	Tree Device
         * \param[in] swq	WQ used to send data
         * \param[in] rwq	WQ to use for receiving results
         * \param[in] root	Root node or -1 for allreduce
         * \param[in] peer	Our local node ID
         * \param[in] npeers	Number of local nodes
         * \param[in] bytes	Bytes to send from source
         * \param[in] pwidth	Pipeline width
         * \param[in] consistency Consistency model for the transfer
         * \param[in] cb	Completion callback
         * \param[in] sbuffer	Source/Send buffer
         * \param[in] rbuffer	Dest/Recv buffer
         * \param[in] dt	Datatype
         * \param[in] op	Operation
         * \param[in] recvcode	Store/NoStore
         * \param[in] tas	Tree Allreduce info
         */
        inline TreeAllreduceShortRecvMessage(
                PAMI::Device::Tree::Device    &TreeQS,
                PAMI::Device::WorkQueue::SharedWorkQueue *swq,
                PAMI::Device::WorkQueue::SharedWorkQueue *rwq,
                unsigned           root,
                unsigned           peer,
                unsigned           npeers,
                unsigned           bytes,
                unsigned           pwidth,
                PAMI_Consistency   consistency,
                pami_callback_t    cb,
                char             * sbuffer,
                char             * rbuffer,
                pami_dt		dt,
                pami_op		op,
                PAMI_Opcode_t      recvcode,
                TreeAllreduceSetup tas) :
        TreeAllreduceRecvMessage(TreeQS, cb, rwq->bufferToProduce(),
                bytes, pwidth, dt, op, recvcode, tas),
        _rwq (rwq),
        _swq (swq),
        _ibuffer (sbuffer),
        _obuffer (rbuffer),
        _peer (peer),
        _npeers (npeers),
        _bytesIn (bytes),
        _doBcast(root == (unsigned)-1),
        _primed(0) {
                //  internal_restart();
        };


        int reset(unsigned collid=0) { return internal_restart(collid); }

        int advance(unsigned cycles, PAMI::Device::Tree::TreeMsgContext ctx);
private:

        inline int internal_restart(unsigned collid)
        {
                _primed = 0;
                TreeAllreduceRecvMessage::reset(collid);
                return 0;
        }

        PAMI::Device::WorkQueue::SharedWorkQueue *_rwq;
        PAMI::Device::WorkQueue::SharedWorkQueue *_swq;
        char *_ibuffer;
        char *_obuffer;
        unsigned _peer;
        unsigned _npeers;
        unsigned _bytesIn;
        bool _doBcast;
        unsigned _primed;
}; /* TreeAllreduceShortRecvMessage */

/**
 * \brief Tree Allreduce Recv Message for Short allreduces with post-processing
 */
class TreeAllreduceShortRecvPostMessage :
                public TreeAllreduceRecvPostMessage {
public:

        /**
         * \brief Constructor for Tree Allreduce Recv Message for Short allreduces with post-processing
         *
         * \param[in] TreeQS	Tree Device
         * \param[in] swq	WQ used to send data
         * \param[in] rwq	WQ to use for receiving results
         * \param[in] root	Root node or -1 for allreduce
         * \param[in] peer	Our local node ID
         * \param[in] npeers	Number of local nodes
         * \param[in] bytes	Bytes to send from source
         * \param[in] pwidth	Pipeline width
         * \param[in] consistency Consistency model for the transfer
         * \param[in] cb	Completion callback
         * \param[in] sbuffer	Source/Send buffer
         * \param[in] rbuffer	Dest/Recv buffer
         * \param[in] dt	Datatype
         * \param[in] op	Operation
         * \param[in] recvcode	Store/NoStore
         * \param[in] tas	Tree Allreduce info
         */
        inline TreeAllreduceShortRecvPostMessage(
                PAMI::Device::Tree::Device    &TreeQS,
                PAMI::Device::WorkQueue::SharedWorkQueue *swq,
                PAMI::Device::WorkQueue::SharedWorkQueue *rwq,
                unsigned           root,
                unsigned           peer,
                unsigned           npeers,
                unsigned           bytes,
                unsigned           pwidth,
                PAMI_Consistency   consistency,
                pami_callback_t    cb,
                char             * sbuffer,
                char             * rbuffer,
                pami_dt		dt,
                pami_op		op,
                PAMI_Opcode_t      recvcode,
                TreeAllreduceSetup tas) :
        TreeAllreduceRecvPostMessage(TreeQS, cb, rwq->bufferToProduce(),
                bytes, pwidth, dt, op, recvcode, tas),
        _rwq (rwq),
        _swq (swq),
        _ibuffer (sbuffer),
        _obuffer (rbuffer),
        _peer (peer),
        _npeers (npeers),
        _bytesIn (bytes),
        _doBcast(root == (unsigned)-1),
        _primed(0) {
                //  internal_restart();
        };


        int reset(unsigned collid=0) { return internal_restart(collid); }

        int advance(unsigned cycles, PAMI::Device::Tree::TreeMsgContext ctx);
private:

        inline int internal_restart(unsigned collid) {
                _primed = 0;
                TreeAllreduceRecvPostMessage::reset(collid);
                return 0;
        }

        PAMI::Device::WorkQueue::SharedWorkQueue *_rwq;
        PAMI::Device::WorkQueue::SharedWorkQueue *_swq;
        char *_ibuffer;
        char *_obuffer;
        unsigned _peer;
        unsigned _npeers;
        unsigned _bytesIn;
        bool _doBcast;
        unsigned _primed;
}; /* TreeAllreduceShortRecvPostMessage */

/**
 * \brief Tree Allreduce Local Message for Short allreduces
 *
 * Does not actually send/recv on CN, only handles "local nodes" processing.
 */
class TreeAllreduceShortLocalMessage :
                public PAMI::Device::Tree::TreeMessage {
public:

        /**
         * \brief Constructor for Tree Allreduce Local Message for Short allreduces
         *
         * \param[in] TreeQS	Tree Device
         * \param[in] swq	WQ used to send data
         * \param[in] rwq	WQ to use for receiving results
         * \param[in] root	Root node or -1 for allreduce
         * \param[in] peer	Our local node ID
         * \param[in] npeers	Number of local nodes
         * \param[in] bytes	Bytes to send from source
         * \param[in] cb	Completion callback
         * \param[in] consistency Consistency model for the transfer
         * \param[in] sbuffer	Source/Send buffer
         * \param[in] rbuffer	Dest/Recv buffer
         */
        inline TreeAllreduceShortLocalMessage(
                PAMI::Device::Tree::Device    &TreeQS,
                PAMI::Device::WorkQueue::SharedWorkQueue *swq,
                PAMI::Device::WorkQueue::SharedWorkQueue *rwq,
                unsigned           root,
                unsigned           peer,
                unsigned           npeers,
                unsigned           bytes,
                pami_callback_t    cb,
                PAMI_Consistency   consistency,
                char             * sbuffer,
                char             * rbuffer) :
        TreeMessage(TreeQS, cb, bytes),
        _rwq (rwq),	// Tree Recv side WQ
        _swq (swq), // Tree Send side WQ
        _ibuffer (sbuffer),
        _obuffer (rbuffer),
        _peer(peer),
        _bytesIn (bytes),
        _doBcast(root == (unsigned)-1),
        _primed(0) {
                //  internal_restart();
        };

        // This is actually neither PostRecv nor PreSend...
        pami_result_t start() { ((PAMI::Device::Tree::Device &)_QS).postPostRecv(*this); return PAMI_SUCCESS; }

        int reset(unsigned collid=0) { return internal_restart(collid); }

        int advance(unsigned cycles, PAMI::Device::Tree::TreeMsgContext ctx);
private:

        inline int internal_restart(unsigned collid) {
                TreeMessage::reset(collid);
                _primed = 0;
                return 0;
        }

        PAMI::Device::WorkQueue::SharedWorkQueue *_rwq;
        PAMI::Device::WorkQueue::SharedWorkQueue *_swq;
        char *_ibuffer;
        char *_obuffer;
        unsigned _peer;
        unsigned _bytesIn;
        bool _doBcast;
        unsigned _primed;
}; /* TreeAllreduceShortLocalMessage */
