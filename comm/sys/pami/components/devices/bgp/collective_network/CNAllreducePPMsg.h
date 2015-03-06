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
 * \file components/devices/bgp/collective_network/CNAllreducePPMsg.h
 * \brief Default collective network allreduce interface.
 */
#ifndef __components_devices_bgp_collective_network_CNAllreducePPMsg_h__
#define __components_devices_bgp_collective_network_CNAllreducePPMsg_h__

#include "util/common.h"
#include "components/devices/bgp/collective_network/CNDevice.h" // externs for env vars
#include "components/devices/bgp/collective_network/CNAllreduce.h"
#include "components/devices/bgp/collective_network/CollectiveNetworkLib.h"
#include "components/devices/bgp/collective_network/CNPacket.h"
#include "components/devices/generic/Device.h"
#include "components/devices/generic/Message.h"
#include "components/devices/generic/AdvanceThread.h"
#include "math/bgp/collective_network/pami_optibgmath.h"
#include "components/devices/FactoryInterface.h"

namespace PAMI {
namespace Device {
namespace BGP {

/**
 * \brief Collective Network Allreduce Send Message base class
 *
 * Collective Network Allreduce with source/dest as pipe WQ.
 */

class CNAllreducePPModel;
class CNAllreducePPMessage;
typedef PAMI::Device::BGP::BaseGenericCNThread CNAllreducePPThread;
class CNAllreducePPDevice : public PAMI::Device::Generic::SharedQueueSubDevice<CNDevice,CNAllreducePPThread,2> {
public:
        inline CNAllreducePPDevice(CNDevice *common) :
        PAMI::Device::Generic::SharedQueueSubDevice<CNDevice,CNAllreducePPThread,2>(common) {
        }

        class Factory : public Interface::FactoryInterface<Factory,CNAllreducePPDevice,Generic::Device> {
        public:
                static inline CNAllreducePPDevice *generate_impl(size_t client, size_t num_ctx, Memory::MemoryManager & mm, PAMI::Device::Generic::Device *devices);
                static inline pami_result_t init_impl(CNAllreducePPDevice *devs, size_t client, size_t contextId, pami_client_t clt, pami_context_t ctx, PAMI::Memory::MemoryManager *mm, PAMI::Device::Generic::Device *devices);
                static inline size_t advance_impl(CNAllreducePPDevice *devs, size_t client, size_t context);
                static inline CNAllreducePPDevice & getDevice_impl(CNAllreducePPDevice *devs, size_t client, size_t context);
        }; // class Factory
}; // class CNAllreducePPDevice

};	// BGP
};	// Device
};	// PAMI

extern PAMI::Device::BGP::CNAllreducePPDevice _g_cnallreducepp_dev;

namespace PAMI {
namespace Device {
namespace BGP {

inline CNAllreducePPDevice *CNAllreducePPDevice::Factory::generate_impl(size_t client, size_t num_ctx, Memory::MemoryManager &mm, PAMI::Device::Generic::Device *devices) {
        return &_g_cnallreducepp_dev;
}

inline pami_result_t CNAllreducePPDevice::Factory::init_impl(CNAllreducePPDevice *devs, size_t client, size_t contextId, pami_client_t clt, pami_context_t ctx, PAMI::Memory::MemoryManager *mm, PAMI::Device::Generic::Device *devices) {
        return _g_cnallreducepp_dev.__init(client, contextId, clt, ctx, mm, devices);
}

inline size_t CNAllreducePPDevice::Factory::advance_impl(CNAllreducePPDevice *devs, size_t client, size_t contextId) {
        return 0;
}

inline CNAllreducePPDevice & CNAllreducePPDevice::Factory::getDevice_impl(CNAllreducePPDevice *devs, size_t client, size_t contextId) {
        return _g_cnallreducepp_dev;
}

class CNAllreducePPMessage : public PAMI::Device::BGP::BaseGenericCNPPMessage {
        enum roles {
                NO_ROLE = 0,
                INJECTION_ROLE = (1 << 0), // first role must be "injector"
                RECEPTION_ROLE = (1 << 1), // last role must be "receptor"
        };
public:
        CNAllreducePPMessage(GenericDeviceMessageQueue *qs,
                        pami_multicombine_t *mcomb,
                        size_t bytes,
                        bool doStore,
                        unsigned dispatch_id,
                        PAMI::Device::BGP::CNAllreduceSetup tas) :
        BaseGenericCNPPMessage(qs, mcomb->client, mcomb->context,
                                (PAMI::PipeWorkQueue *)mcomb->data,
                                (PAMI::PipeWorkQueue *)mcomb->results,
                                bytes, doStore, mcomb->roles, mcomb->cb_done,
                                dispatch_id, tas),
        _roles(mcomb->roles)
        {
        }

        // virtual function
        pami_context_t postNext(bool devQueued) {
                return _g_cnallreducepp_dev.common()->__postNext<CNAllreducePPMessage,CNAllreducePPThread>(this, devQueued);
        }

        // _bytesLeft == bytes on network!
        inline int setThreads(CNAllreducePPThread **th) {
                CNAllreducePPThread *t;
                int n;
                _g_cnallreducepp_dev.__getThreads(&t, &n);
                int nt = 0;
                _g_cnallreducepp_dev.common()->__resetThreads();
                _nThreads = ((_roles & INJECTION_ROLE) != 0) + ((_roles & RECEPTION_ROLE) != 0);
                if (_roles & INJECTION_ROLE) {
                        t[nt].setMsg(this);
                        t[nt].setAdv(advanceInj);
                        t[nt].setStatus(PAMI::Device::Ready);
                        t[nt]._wq = _swq;
                        t[nt]._bytesLeft = _bytes << _allreduceSetup._logbytemult;
                        t[nt]._cycles = 1;
                        __advanceInj(&t[nt]);
                        ++nt;
                }
                if (_roles & RECEPTION_ROLE) {
                        t[nt].setMsg(this);
                        t[nt].setAdv(advanceRcp);
                        t[nt].setStatus(PAMI::Device::Ready);
                        t[nt]._wq = _rwq;
                        t[nt]._bytesLeft = _bytes << _allreduceSetup._logbytemult;
                        t[nt]._cycles = 3000;
                        __advanceRcp(&t[nt]);
                        ++nt;
                }
                // PAMI_assert(nt > 0? && nt < n);
                *th = t;
                return nt;
        }

protected:
        inline void __completeThread(CNAllreducePPThread *thr);

        DECL_ADVANCE_ROUTINE(advanceInj,CNAllreducePPMessage,CNAllreducePPThread);
        DECL_ADVANCE_ROUTINE(advanceRcp,CNAllreducePPMessage,CNAllreducePPThread);
        inline pami_result_t __advanceInj(CNAllreducePPThread *thr) {
                if (thr->_bytesLeft == 0) return PAMI_SUCCESS;
                unsigned hcount = BGPCN_FIFO_SIZE, dcount = BGPCN_QUADS_PER_FIFO;
                size_t avail = thr->_wq->bytesAvailableToConsume();
                char *buf = thr->_wq->bufferToConsume();
                bool aligned = (((unsigned)buf & 0x0f) == 0);
                size_t did = 0;
                if (__wait_send_fifo_to(thr, hcount, dcount, thr->_cycles)) {
                        return PAMI_EAGAIN;
                }
                __send_whole_packets(thr, hcount, dcount, avail, did, buf, aligned);
                __send_last_packet(thr, hcount, dcount, avail, did, buf, aligned);
                if (did) {
                        thr->_wq->consumeBytes(did);
                        if (thr->_bytesLeft == 0) {
                                thr->setStatus(PAMI::Device::Complete);
                                __completeThread(thr);
                                return PAMI_SUCCESS;
                        }
                }
                return PAMI_EAGAIN;
        }


        inline pami_result_t __advanceRcp(CNAllreducePPThread *thr) {
                if (thr->_bytesLeft == 0) return PAMI_SUCCESS;
                unsigned hcount = 0, dcount = 0;
                size_t avail = thr->_wq->bytesAvailableToProduce();
                char *buf = thr->_wq->bufferToProduce();
                bool aligned = (((unsigned)buf & 0x0f) == 0);
                size_t did = 0;
                if (__wait_recv_fifo_to(thr, hcount, dcount, thr->_cycles)) {
                        return PAMI_EAGAIN;
                }
                __recv_whole_packets(thr, hcount, dcount, avail, did, buf, aligned);
                __recv_last_packet(thr, hcount, dcount, avail, did, buf, aligned);
                if (did) {
                        thr->_wq->produceBytes(did);
                        if (thr->_bytesLeft == 0) {
                                thr->setStatus(PAMI::Device::Complete);
                                __completeThread(thr);
                                return PAMI_SUCCESS;
                        }
                }
                return PAMI_EAGAIN;
        }

        unsigned _roles;
        unsigned _nThreads;
}; // class CNAllreducePPMessage

class CNAllreducePPModel : public PAMI::Device::Interface::MulticombineModel<CNAllreducePPModel,CNAllreducePPDevice,sizeof(CNAllreducePPMessage)> {
public:
        static const int NUM_ROLES = 2;
        static const int REPL_ROLE = -1;
        static const size_t sizeof_msg = sizeof(CNAllreducePPMessage);

        CNAllreducePPModel(CNAllreducePPDevice &device, pami_result_t &status) :
        PAMI::Device::Interface::MulticombineModel<CNAllreducePPModel,CNAllreducePPDevice,sizeof(CNAllreducePPMessage)>(device,status)
        {
                // PAMI_assert(device == _g_cnallreducepp_dev);
                _dispatch_id = _g_cnallreducepp_dev.newDispID();
                _me = __global.mapping.task();
        }

        inline pami_result_t postMulticombine_impl(uint8_t (&state)[sizeof_msg], pami_multicombine_t *mcomb);

private:
        size_t _me;
        unsigned _dispatch_id;
}; // class CNAllreducePPModel

inline void CNAllreducePPMessage::__completeThread(CNAllreducePPThread *thr) {
        unsigned c = _g_cnallreducepp_dev.common()->__completeThread(thr);
        if (c >= _nThreads) {
                setStatus(PAMI::Device::Done);
        }
}

// Permit a NULL results_topo to mean "everyone" (i.e. "root == -1")
inline pami_result_t CNAllreducePPModel::postMulticombine_impl(uint8_t (&state)[sizeof_msg], pami_multicombine_t *mcomb) {
        PAMI::Device::BGP::CNAllreduceSetup &tas =
                PAMI::Device::BGP::CNAllreduceSetup::getCNAS(mcomb->dtype, mcomb->optor);
        // PAMI_assert(tas._pre != NULL);
        if (!tas._pre || !tas._post) {
PAMI_abort();
                return PAMI_ERROR;
        }
        PAMI::Topology *result_topo = (PAMI::Topology *)mcomb->results_participants;
        bool doStore = (!result_topo || result_topo->isRankMember(_me));
        size_t bytes = mcomb->count << pami_dt_shift[mcomb->dtype];

        // could try to complete allreduce before construction, but for now the code
        // is too dependent on having message and thread structures to get/keep context.
        // __post() will still try early advance... (after construction)
        CNAllreducePPMessage *msg;
        msg = new (&state) CNAllreducePPMessage(_g_cnallreducepp_dev.common(),
                        mcomb, bytes, doStore, _dispatch_id, tas);
        _g_cnallreducepp_dev.__post<CNAllreducePPMessage>(msg);
        return PAMI_SUCCESS;
}

};	// BGP
};	// Device
};	// PAMI

#endif // __component_devices_bgp_cnallreduceppmsg_h__
