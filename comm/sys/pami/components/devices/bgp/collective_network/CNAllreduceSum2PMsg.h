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
 * \file components/devices/bgp/collective_network/CNAllreduceSum2PMsg.h
 * \brief Default collective network allreduce interface
 * for DOUBLE-SUM 2-Pass algorithm
 */
#ifndef __components_devices_bgp_collective_network_CNAllreduceSum2PMsg_h__
#define __components_devices_bgp_collective_network_CNAllreduceSum2PMsg_h__

#include "util/common.h"
#include "components/devices/workqueue/SharedWorkQueue.h"
#include "components/devices/bgp/collective_network/CNDevice.h" // externs for env vars
#include "components/devices/bgp/collective_network/CNAllreduce.h"
#include "components/devices/bgp/collective_network/CollectiveNetworkLib.h"
#include "components/devices/bgp/collective_network/CNPacket.h"
#include "components/devices/generic/Device.h"
#include "components/devices/generic/Message.h"
#include "components/devices/generic/AdvanceThread.h"
#include "math/math_coremath.h"
#include "math/bgp/collective_network/DblUtils.h"
#include "components/devices/FactoryInterface.h"

/**
 * \page twopass_dblsum The 2-Pass DOUBLE-SUM algorithm
 *
 * The 2-Pass DOUBLE-SUM algorithm is as follows. Each double
 * is split into a 16-bit exponent and a 96-bit mantissa. Exponents
 * must be processed before the corresponding mantissa can be sent.
 * Exponents are processed by the tree using a MAX operator. This
 * results in the largest exponent of a set of doubles being received.
 * This MAX exponent must be fed-back to the sending side and used
 * to adjust the mantissa so that all (integer) mantissas are aligned
 * decimally. Then the mantissa is processed by the tree using an ADD
 * operator. The received mantissa must be combined with the MAX exponent
 * to re-form a double which is the result.
 *
 * For efficiency sake, exponents and mantissas are buffered into
 * tree packets. 126 exponents are placed in a packet. This will
 * correspond to 6 packets of 21 mantissas each. Exponent packets are
 * sent up to some threshold intended to approximate the time at which
 * the first exponent (result) packet will be received. At this point,
 * the received exponents may be used to start sending the corresponding
 * mantissas. Note that all nodes must have a common understanding of
 * when to switch between exponent processing and mantissa processing,
 * otherwise the tree hardware will be proccessing disimilar packet
 * types resulting in garbage in the received packets.
 *
 * Because both the mantissa send and mantissa receive must use the
 * received exponents, there must be coordination between the sender
 * and receiver and sharing of this data. In the basic message type,
 * this is done with a static buffer. This will only work in SMP mode,
 * or at least a scheme where the sender and receiver are in the same
 * process image (have access to the same memory). An alternative
 * scheme uses the WorkQueue object, possibly backed by shared memory,
 * to create a single-producer dual-consumer FIFO for the exponent
 * packets. Receiving an exponent packets results in the packet being
 * produced into the WorkQueue FIFO. At this point, both the mantissa
 * sender and mantissa receiver have access to the data. The data will
 * not be overwritten until both have consumed it. Note, it is still
 * necessary for the exponent and mantissa senders to follow a strict
 * pattern, as the tree cannot process exponent and mantissa packets
 * simultaniously. It is expected that the static buffer scheme will
 * be obsoleted.
 *
 * This algorithm can be adapted to other operators.
 */

/** \brief The number of exponent packets to send before switching
 * to mantissa packets
 *
 * Ideally, this number would be computed based on tree depth
 * (partition size). If the exponent packets have not started
 * to arrive in the receive FIFO by the time we switch to mantissas,
 * we will wait for them to arrive and waste time. So the larger
 * the tree the more exponent packets we should send before switching.
 * This timing also affects the point at which 1-Pass becomes slower
 * than 2-pass.
 *
 * This value is used as the default for the _expcycle variable,
 * which is the actual limit/switch point. The current basic 2-pass
 * message class that uses a static buffer as the feedback mechanism
 * actually sizes the feedback buffer with this constant and in this
 * case _expcycle must be <= EXPCOUNT. In other message types _expcycle
 * may be set to any value.
 */

namespace PAMI {
namespace Device {
namespace BGP {

class CNAllreduce2PModel;
class CNAllreduce2PMessage;
typedef PAMI::Device::BGP::BaseGenericCNThread CNAllreduce2PThread;
class CNAllreduce2PDevice : public PAMI::Device::Generic::SharedQueueSubDevice<CNDevice,CNAllreduce2PThread,2> {
public:
        inline CNAllreduce2PDevice(CNDevice *common) :
        PAMI::Device::Generic::SharedQueueSubDevice<CNDevice,CNAllreduce2PThread,2>(common) {
        }

        class Factory : public Interface::FactoryInterface<Factory,CNAllreduce2PDevice,Generic::Device> {
        public:
                static inline CNAllreduce2PDevice *generate_impl(size_t client, size_t num_ctx, Memory::MemoryManager & mm, PAMI::Device::Generic::Device *devices);
                static inline pami_result_t init_impl(CNAllreduce2PDevice *devs, size_t client, size_t contextId, pami_client_t clt, pami_context_t ctx, PAMI::Memory::MemoryManager *mm, PAMI::Device::Generic::Device *devices);
                static inline size_t advance_impl(CNAllreduce2PDevice *devs, size_t client, size_t context);
                static inline CNAllreduce2PDevice & getDevice_impl(CNAllreduce2PDevice *devs, size_t client, size_t context);
        }; // class Factory
}; // class CNAllreduce2PDevice

};	// BGP
};	// Device
};	// PAMI

extern PAMI::Device::BGP::CNAllreduce2PDevice _g_cnallreduce2p_dev;

namespace PAMI {
namespace Device {
namespace BGP {

inline CNAllreduce2PDevice *CNAllreduce2PDevice::Factory::generate_impl(size_t client, size_t num_ctx, Memory::MemoryManager &mm, PAMI::Device::Generic::Device *devices) {
        return &_g_cnallreduce2p_dev;
}

inline pami_result_t CNAllreduce2PDevice::Factory::init_impl(CNAllreduce2PDevice *devs, size_t client, size_t contextId, pami_client_t clt, pami_context_t ctx, PAMI::Memory::MemoryManager *mm, PAMI::Device::Generic::Device *devices) {
        return _g_cnallreduce2p_dev.__init(client, contextId, clt, ctx, mm, devices);
}

inline size_t CNAllreduce2PDevice::Factory::advance_impl(CNAllreduce2PDevice *devs, size_t client, size_t contextId) {
        return 0;
}

inline CNAllreduce2PDevice & CNAllreduce2PDevice::Factory::getDevice_impl(CNAllreduce2PDevice *devs, size_t client, size_t contextId) {
        return _g_cnallreduce2p_dev;
}

/**
 * \brief collective Network Allreduce DOUBLE-SUM 2-Pass Send
 *
 * A static buffer is used for exponent feedback.
 *
 */

class CNAllreduce2PMessage : public PAMI::Device::BGP::BaseGenericCN2PMessage {
        enum roles {
                NO_ROLE = 0,
                INJECTION_ROLE = (1 << 0), // first role must be "injector"
                RECEPTION_ROLE = (1 << 1), // last role must be "receptor"
        };
public:
        CNAllreduce2PMessage(GenericDeviceMessageQueue *qs,
                pami_multicombine_t *mcomb,
                PAMI::Device::WorkQueue::WorkQueue &ewq,
                PAMI::Device::WorkQueue::WorkQueue &mwq,
                PAMI::Device::WorkQueue::WorkQueue &xwq,
                size_t bytes,
                bool doStore,
                unsigned dispatch_id_e,
                unsigned dispatch_id_m) :
        BaseGenericCN2PMessage(qs, mcomb->client, mcomb->context,
                                ewq, mwq, xwq,
                                (PAMI::PipeWorkQueue *)mcomb->data,
                                (PAMI::PipeWorkQueue *)mcomb->results,
                                bytes, doStore, mcomb->roles, mcomb->cb_done,
                                dispatch_id_e, dispatch_id_m),
        _roles(mcomb->roles),
        _offx(0)
        {
        }

        // virtual function
        pami_context_t postNext(bool devQueued) {
                return _g_cnallreduce2p_dev.common()->__postNext<CNAllreduce2PMessage,CNAllreduce2PThread>(this, devQueued);
        }

        inline int setThreads(CNAllreduce2PThread **th) {
                CNAllreduce2PThread *t;
                int n;
                _g_cnallreduce2p_dev.__getThreads(&t, &n);
                int nt = 0;
                _g_cnallreduce2p_dev.common()->__resetThreads();
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
                *th = t;
                return nt;
        }

protected:
        DECL_ADVANCE_ROUTINE(advanceInj,CNAllreduce2PMessage,CNAllreduce2PThread);
        DECL_ADVANCE_ROUTINE(advanceRcp,CNAllreduce2PMessage,CNAllreduce2PThread);
        inline pami_result_t __advanceInj(CNAllreduce2PThread *thr) {
                pami_result_t rc = PAMI_EAGAIN;
                unsigned hcount = BGPCN_FIFO_SIZE, dcount = BGPCN_QUADS_PER_FIFO;
                size_t avail = thr->_wq->bytesAvailableToConsume();
                char *buf = thr->_wq->bufferToConsume();
                unsigned expRemain = _expcount - _expsent;
                unsigned manRemain = _expsent - _mansent;
                size_t didx = 0;
                size_t did = 0;
                if (__wait_send_fifo_to(thr, hcount, dcount, thr->_cycles)) {
                        return rc;
                }
                char *bufx = buf + _offx;
                __send_expo_packets(thr, hcount, dcount, expRemain, avail, didx, bufx);
                _offx += didx;
                // mantissas dont use original data buffer, partial mantissas in _mwq.
                __send_mant_packets(thr, hcount, dcount, manRemain, did);
                if (did) {
                        thr->_bytesLeft -= did;
                        thr->_wq->consumeBytes(did);
                        _offx -= did;
                }
                if (thr->_bytesLeft == 0) {
                        rc = PAMI_SUCCESS;
                        thr->setStatus(PAMI::Device::Complete);
                        __completeThread(thr);
                }
                return rc;
        }

        inline pami_result_t __advanceRcp(CNAllreduce2PThread *thr) {
                pami_result_t rc = PAMI_EAGAIN;
                register unsigned hcount = 0, dcount = 0;
                if (__wait_recv_fifo_to(thr, hcount, dcount, thr->_cycles)) {
                        return rc;
                }
                _BGP_TreeHwHdr hdr;
                size_t avail = thr->_wq->bytesAvailableToProduce();
                char *buf = thr->_wq->bufferToProduce();
                size_t total = 0;
                size_t left = thr->_bytesLeft;
                size_t dstBytes = (left < MANT_PER_PKT * sizeof(double) ?
                                left : MANT_PER_PKT * sizeof(double));
                // we have to check avail before reading header, otherwise we
                // might get committed to reading payload without any place to
                // store it. This means we must also have space available in order
                // to process an exponent packet - even though it doesnt need it.
                while (left > 0 && avail >= dstBytes &&
                                hcount > 0 && dcount > 0) {
                        CollectiveRawReceiveHeader(VIRTUAL_CHANNEL, &hdr);
                        --hcount;
                        if (hdr.CtvHdr.Tag == _expo_disp_id) {
                                __recv_expo_packet(thr);
                                // recv expo can never complete message
                        } else if (hdr.CtvHdr.Tag == _mant_disp_id) {
                                size_t did = 0;
                                __recv_mant_packet(thr, avail, did, buf + total, dstBytes);
                                if (did) {
                                        left -= did;
                                        total += did;
                                        avail -= did;
                                }
                        } else {
fprintf(stderr, "bad packet header 0x%08x\n", hdr.CtvHdr.word);
CollectiveRawReceivePacketNoHdrNoStore(VIRTUAL_CHANNEL);
//				PAMI_abort();
                        }
                        dcount -= BGPCN_QUADS_PER_PKT;
                }
                if (total > 0) {
                        thr->_bytesLeft -= total;
                        thr->_wq->produceBytes(total);
                        if (thr->_bytesLeft == 0) {
                                rc = PAMI_SUCCESS;
                                thr->setStatus(PAMI::Device::Complete);
                                __completeThread(thr);
                        }
                }
                return rc;
        }

        inline void __completeThread(CNAllreduce2PThread *thr);

private:
        unsigned _roles;
        unsigned _offx;
        unsigned _nThreads;
}; // class CNAllreduce2PMessage

class CNAllreduce2PModel : public PAMI::Device::Interface::MulticombineModel<CNAllreduce2PModel,CNAllreduce2PDevice,sizeof(CNAllreduce2PMessage)> {
public:
        static const int NUM_ROLES = 2;
        static const int REPL_ROLE = -1;
        static const size_t sizeof_msg = sizeof(CNAllreduce2PMessage);

        CNAllreduce2PModel(CNAllreduce2PDevice &device, pami_result_t &status) :
        PAMI::Device::Interface::MulticombineModel<CNAllreduce2PModel,CNAllreduce2PDevice,sizeof(CNAllreduce2PMessage)>(device,status),
        _dispatch_id_e(_g_cnallreduce2p_dev.newDispID()),
        _dispatch_id_m(_g_cnallreduce2p_dev.newDispID())
        {
                // PAMI_assert(device == _g_cnallreduce2p_dev);
		char key[PAMI::Memory::MMKEYSIZE];
		size_t keylen = sprintf(key, "/CNAllreduce2PModel-");
		key[keylen+1] = '\0';

		// we depend on doing consumeBytes(bytesAvailableToConsume()) in order
		// to "jump" to next "boundary" so we maintain alignment for each cycle.
		// this requires the WQ behavior based on workunits and worksize that
		// creates artificial "boundaries" at those points.
		key[keylen] = 'e';
		new (&_ewq) PAMI::Device::WorkQueue::SharedWorkQueue(
				_g_cnallreduce2p_dev.common()->getSysdep(),
				key, EXPO_WQ_SIZE, BGPCN_PKT_SIZE);

		key[keylen] = 'm';
		new (&_mwq) PAMI::Device::WorkQueue::SharedWorkQueue(
				_g_cnallreduce2p_dev.common()->getSysdep(),
				key, EXPO_WQ_SIZE, MANT_WQ_FACT * BGPCN_PKT_SIZE);

		key[keylen] = 'x';
		new (&_xwq) PAMI::Device::WorkQueue::SharedWorkQueue(
				_g_cnallreduce2p_dev.common()->getSysdep(),
				key, EXPO_WQ_SIZE, BGPCN_PKT_SIZE);

                _me = __global.mapping.task();
                _ewq.setConsumers(1, 0);
                _ewq.setProducers(1, 0);
                _mwq.setConsumers(1, 0);
                _mwq.setProducers(1, 0);
                _xwq.setConsumers(2, 0);
                _xwq.setProducers(1, 0);
                reset_impl();
        }

        inline void reset_impl() {
                _ewq.reset();
                _mwq.reset();
                _xwq.reset();
        }

        inline pami_result_t postMulticombine_impl(uint8_t (&state)[sizeof_msg], pami_multicombine_t *mcomb);

private:
        PAMI::Device::WorkQueue::SharedWorkQueue _ewq;
        PAMI::Device::WorkQueue::SharedWorkQueue _mwq;
        PAMI::Device::WorkQueue::SharedWorkQueue _xwq;
        unsigned _dispatch_id_e;
        unsigned _dispatch_id_m;
        size_t _me;

        static inline void compile_time_assert () {
                COMPILE_TIME_ASSERT(EXPO_WQ_SIZE >= EXPCOUNT);
                COMPILE_TIME_ASSERT((EXPO_WQ_SIZE & (EXPO_WQ_SIZE - 1)) == 0);
                COMPILE_TIME_ASSERT(MANT_WQ_FACT * EXPO_WQ_SIZE >= EXPO_MANT_FACTOR * EXPCOUNT);
                COMPILE_TIME_ASSERT((MANT_WQ_FACT & (MANT_WQ_FACT - 1)) == 0);
        }
}; // class CNAllreduce2PModel

inline void CNAllreduce2PMessage::__completeThread(CNAllreduce2PThread *thr) {
        unsigned c = _g_cnallreduce2p_dev.common()->__completeThread(thr);
        if (c >= _nThreads) {
                setStatus(PAMI::Device::Done);
        }
}

inline pami_result_t CNAllreduce2PModel::postMulticombine_impl(uint8_t (&state)[sizeof_msg], pami_multicombine_t *mcomb) {
        // we don't need CNAllreduceSetup since we know this is DOUBLE-SUM
        PAMI::Topology *results_topo = (PAMI::Topology *)mcomb->results_participants;
        bool doStore = (!results_topo || results_topo->isRankMember(_me));
        size_t bytes = mcomb->count << pami_dt_shift[mcomb->dtype];
        // could try to complete allreduce before construction, but for now the code
        // is too dependent on having message and thread structures to get/keep context.
        // __post() will still try early advance... (after construction)
        CNAllreduce2PMessage *msg;
        msg = new (&state) CNAllreduce2PMessage(_g_cnallreduce2p_dev.common(),
                        mcomb, _ewq, _mwq, _xwq,
                        bytes, doStore, _dispatch_id_e, _dispatch_id_m);
        _g_cnallreduce2p_dev.__post<CNAllreduce2PMessage>(msg);
        return PAMI_SUCCESS;
}

};	// BGP
};	// Device
};	// PAMI

#endif // __components_devices_bgp_cnallreduce2psummsg_h__
