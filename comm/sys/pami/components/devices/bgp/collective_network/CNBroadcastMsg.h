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
 * \file components/devices/bgp/collective_network/CNBroadcastMsg.h
 * \brief Default collective network broadcast interface.
 */
#ifndef __components_devices_bgp_collective_network_CNBroadcastMsg_h__
#define __components_devices_bgp_collective_network_CNBroadcastMsg_h__

#include "util/common.h"
#include "components/devices/bgp/collective_network/CNDevice.h" // externs for env vars
#include "components/devices/bgp/collective_network/CollectiveNetworkLib.h"
#include "components/devices/bgp/collective_network/CNPacket.h"
#include "components/devices/generic/Device.h"
#include "components/devices/generic/Message.h"
#include "components/devices/generic/AdvanceThread.h"
#include "components/devices/MulticastModel.h"
#include "components/devices/FactoryInterface.h"

/**
 * \page cn_bcast Collective Network Broadcast
 *
 * Broadcast is implemented on the collective network using
 * an allreduce operation with an OR operator and all non-root
 * nodes injecting zero. Additionally, the root node discards
 * all received packets. Note, all nodes must inject and receive
 * in order to complete the operation.
 */

namespace PAMI {
namespace Device {
namespace BGP {

class CNBroadcastModel;
class CNBroadcastMessage;
typedef PAMI::Device::BGP::BaseGenericCNThread CNBroadcastThread;
class CNBroadcastDevice : public PAMI::Device::Generic::SharedQueueSubDevice<CNDevice,CNBroadcastThread,2> {
public:
        inline CNBroadcastDevice(CNDevice *common) :
        PAMI::Device::Generic::SharedQueueSubDevice<CNDevice,CNBroadcastThread,2>(common) {
        }

        class Factory : public Interface::FactoryInterface<Factory,CNBroadcastDevice,Generic::Device> {
        public:
                static inline CNBroadcastDevice *generate_impl(size_t client, size_t num_ctx, Memory::MemoryManager & mm, PAMI::Device::Generic::Device *devices);
                static inline pami_result_t init_impl(CNBroadcastDevice *devs, size_t client, size_t contextId, pami_client_t clt, pami_context_t ctx, PAMI::Memory::MemoryManager *mm, PAMI::Device::Generic::Device *devices);
                static inline size_t advance_impl(CNBroadcastDevice *devs, size_t client, size_t context);
                static inline CNBroadcastDevice & getDevice_impl(CNBroadcastDevice *devs, size_t client, size_t context);
        }; // class Factory
}; // class CNBroadcastDevice

};	// BGP
};	// Device
};	// PAMI

extern PAMI::Device::BGP::CNBroadcastDevice _g_cnbroadcast_dev;

namespace PAMI {
namespace Device {
namespace BGP {

inline CNBroadcastDevice *CNBroadcastDevice::Factory::generate_impl(size_t client, size_t num_ctx, Memory::MemoryManager &mm, PAMI::Device::Generic::Device *devices) {
        return &_g_cnbroadcast_dev;
}

inline pami_result_t CNBroadcastDevice::Factory::init_impl(CNBroadcastDevice *devs, size_t client, size_t contextId, pami_client_t clt, pami_context_t ctx, PAMI::Memory::MemoryManager *mm, PAMI::Device::Generic::Device *devices) {
        return _g_cnbroadcast_dev.__init(client, contextId, clt, ctx, mm, devices);
}

inline size_t CNBroadcastDevice::Factory::advance_impl(CNBroadcastDevice *devs, size_t client, size_t contextId) {
        return 0;
}

inline CNBroadcastDevice & CNBroadcastDevice::Factory::getDevice_impl(CNBroadcastDevice *devs, size_t client, size_t contextId) {
        return _g_cnbroadcast_dev;
}

/**
 * \brief Collective Network Broadcast Send
 *
 */

class CNBroadcastMessage : public PAMI::Device::BGP::BaseGenericCNMessage {
        enum roles {
                NO_ROLE = 0,
                INJECTION_ROLE = (1 << 0), // first role must be "injector"
                RECEPTION_ROLE = (1 << 1), // last role must be "receptor"
        };
public:
        CNBroadcastMessage(GenericDeviceMessageQueue *qs,
                        pami_multicast_t *mcast,
                        size_t bytes,
                        bool doStore,
                        bool doData,
                        unsigned dispatch_id) :
        BaseGenericCNMessage(qs, mcast->client, mcast->context,
                (PAMI::PipeWorkQueue *)mcast->src, (PAMI::PipeWorkQueue *)mcast->dst,
                bytes, doStore, mcast->roles, mcast->cb_done,
                dispatch_id, PAMI::Device::BGP::COMBINE_OP_OR, BGPCN_PKT_SIZE),
        _doData(doData),
        _roles(mcast->roles)
        {
        }

        // virtual function
        pami_context_t postNext(bool devQueued) {
                return _g_cnbroadcast_dev.common()->__postNext<CNBroadcastMessage,CNBroadcastThread>(this, devQueued);
        }

        inline int setThreads(CNBroadcastThread **th) {
                CNBroadcastThread *t;
                int n;
                _g_cnbroadcast_dev.__getThreads(&t, &n);
                int nt = 0;
                _g_cnbroadcast_dev.common()->__resetThreads();
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
        inline void __completeThread(CNBroadcastThread *thr);

        DECL_ADVANCE_ROUTINE(advanceInj,CNBroadcastMessage,CNBroadcastThread);
        DECL_ADVANCE_ROUTINE(advanceRcp,CNBroadcastMessage,CNBroadcastThread);
        inline pami_result_t __advanceInj(CNBroadcastThread *thr) {
                if (thr->_bytesLeft == 0) return PAMI_SUCCESS;
                unsigned hcount = BGPCN_FIFO_SIZE, dcount = BGPCN_QUADS_PER_FIFO;
                // thr->_wq is not valid unless _doData...
                if (__wait_send_fifo_to(thr, hcount, dcount, thr->_cycles)) {
                        return PAMI_EAGAIN;
                }
                if (_doData) {
                        size_t avail = thr->_wq->bytesAvailableToConsume();
                        char *buf = thr->_wq->bufferToConsume();
                        bool aligned = (((unsigned)buf & 0x0f) == 0);
                        size_t did = 0;
                        if (avail < BGPCN_PKT_SIZE && avail < thr->_bytesLeft) {
                                return PAMI_EAGAIN;
                        }
                        // is this possible??
                        if (avail > thr->_bytesLeft) avail = thr->_bytesLeft;
                        __send_whole_packets(thr, hcount, dcount, avail, did, buf, aligned);
                        __send_last_packet(thr, hcount, dcount, avail, did, buf, aligned);
                        thr->_wq->consumeBytes(did);
                } else {
                        __send_null_packets(thr, hcount, dcount);
                }
                if (thr->_bytesLeft == 0) {
                        thr->setStatus(PAMI::Device::Complete);
                        __completeThread(thr);
                        return PAMI_SUCCESS;
                }
                return PAMI_EAGAIN;
        }

        inline pami_result_t __advanceRcp(CNBroadcastThread *thr) {
                if (thr->_bytesLeft == 0) return PAMI_SUCCESS;
                unsigned hcount = 0, dcount = 0;
                size_t did = 0;
                if (__wait_recv_fifo_to(thr, hcount, dcount, thr->_cycles)) {
                        return PAMI_EAGAIN;
                }
                // thr->_wq is not valid unless _doStore...
                if (_doStore) {
                        unsigned toCopy = thr->_bytesLeft >= BGPCN_PKT_SIZE ? BGPCN_PKT_SIZE : thr->_bytesLeft;
                        size_t avail = thr->_wq->bytesAvailableToProduce();
                        char *buf = thr->_wq->bufferToProduce();
                        bool aligned = (((unsigned)buf & 0x0f) == 0);
                        if (avail < toCopy) {
                                return PAMI_EAGAIN;
                        }
                        __recv_whole_packets(thr, hcount, dcount, avail, did, buf, aligned);
                        __recv_last_packet(thr, hcount, dcount, avail, did, buf, aligned);
                        if (did) {
                                thr->_wq->produceBytes(did);
                        }
                } else {
                        __recv_null_packets(thr, hcount, dcount, did);
                }
                if (thr->_bytesLeft == 0) {
                        thr->setStatus(PAMI::Device::Complete);
                        __completeThread(thr);
                        return PAMI_SUCCESS;
                }
                return PAMI_EAGAIN;
        }

        bool _doData;
        unsigned _roles;
        unsigned _nThreads;
}; // class CNBroadcastMessage

class CNBroadcastModel : public PAMI::Device::Interface::MulticastModel<CNBroadcastModel,CNBroadcastDevice,sizeof(CNBroadcastMessage)> {
public:
        static const int NUM_ROLES = 2;
        static const int REPL_ROLE = -1;
        static const size_t sizeof_msg = sizeof(CNBroadcastMessage);

        CNBroadcastModel(CNBroadcastDevice &device, pami_result_t &status) :
        PAMI::Device::Interface::MulticastModel<CNBroadcastModel,CNBroadcastDevice,sizeof(CNBroadcastMessage)>(device,status)
        {
                // PAMI_assert(device == _g_cnbroadcast_dev);
                _dispatch_id = _g_cnbroadcast_dev.newDispID();
                _me = __global.mapping.task();
        }

        inline pami_result_t postMulticast_impl(uint8_t (&state)[sizeof_msg], pami_multicast_t *mcast, void *devinfo=NULL);

private:
        size_t _me;
        unsigned _dispatch_id;
}; // class CNBroadcastModel

inline void CNBroadcastMessage::__completeThread(CNBroadcastThread *thr) {
        unsigned c = _g_cnbroadcast_dev.common()->__completeThread(thr);
        if (c >= _nThreads) {
                setStatus(PAMI::Device::Done);
        }
}

inline pami_result_t CNBroadcastModel::postMulticast_impl(uint8_t (&state)[sizeof_msg], pami_multicast_t *mcast, void *devinfo) {
        PAMI::Topology *src_topo = (PAMI::Topology *)mcast->src_participants;
        PAMI::Topology *dst_topo = (PAMI::Topology *)mcast->dst_participants;
        bool doData = (!src_topo || src_topo->isRankMember(_me));
        bool doStore = (!dst_topo || dst_topo->isRankMember(_me));

        // could try to complete broadcast before construction, but for now the code
        // is too dependent on having message and thread structures to get/keep context.
        // __post() will still try early advance... (after construction)
        CNBroadcastMessage *msg;
        msg = new (&state) CNBroadcastMessage(_g_cnbroadcast_dev.common(),
                        mcast, mcast->bytes, doStore, doData, _dispatch_id);
        _g_cnbroadcast_dev.__post<CNBroadcastMessage>(msg);
        return PAMI_SUCCESS;
}

};	// BGP
};	// Device
};	// PAMI

#endif // __components_devices_bgp_cnbroadcastmsg_h__
