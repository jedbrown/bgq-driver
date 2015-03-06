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
 * \file components/devices/bgp/collective_network/CNAllreduceMsg.h
 * \brief Default collective network allreduce interface.
 */
#ifndef __components_devices_bgp_collective_network_CNAllreduceMsg_h__
#define __components_devices_bgp_collective_network_CNAllreduceMsg_h__

#include "util/common.h"
#include "components/devices/bgp/collective_network/CNDevice.h" // externs for env vars
#include "components/devices/bgp/collective_network/CNAllreduce.h"
#include "components/devices/bgp/collective_network/CollectiveNetworkLib.h"
#include "components/devices/bgp/collective_network/CNPacket.h"
#include "components/devices/generic/Device.h"
#include "components/devices/generic/Message.h"
#include "components/devices/generic/AdvanceThread.h"
#include "components/devices/MulticombineModel.h"
#include "components/devices/FactoryInterface.h"

/// \page pami_multicombine_examples
///
/// #include "components/devices/bgp/collective_network/CNAllreduceMsg.h"
/// typedef PAMI::Device::BGP::CNAllreduceModel   MY_ALLREDUCE_MODEL;
/// typedef PAMI::Device::BGP::CNAllreduceMessage MY_ALLREDUCE_MEGSSAGE;
///
/// pami_result_t status;
/// MY_ALLREDUCE_MODEL _allreduce(status);
/// PAMI_assert(status == PAMI_SUCCESS);
///
/// pami_multicombine_t _mcomb;
/// MY_ALLREDUCE_MEGSSAGE _msg;
/// _mcomb.request = &_msg;
/// _mcomb.cb_done = ...;
/// _mcomb.roles = ...;
/// _mcomb.data = ...;
/// ...
/// _allreduce.postMulticombine(&_mcomb);
///

namespace PAMI {
namespace Device {
namespace BGP {

/**
 * \brief Tree Allreduce Send Message base class
 *
 * Tree Allreduce with source/dest as pipeworkqueue.
 */

class CNAllreduceModel;
class CNAllreduceMessage;
typedef PAMI::Device::BGP::BaseGenericCNThread CNAllreduceThread;
class CNAllreduceDevice : public PAMI::Device::Generic::SharedQueueSubDevice<CNDevice,CNAllreduceThread,2> {
public:
        inline CNAllreduceDevice(CNDevice *common) :
        PAMI::Device::Generic::SharedQueueSubDevice<CNDevice,CNAllreduceThread,2>(common) {
        }
        class Factory : public Interface::FactoryInterface<Factory,CNAllreduceDevice,Generic::Device> {
        public:
                static inline CNAllreduceDevice *generate_impl(size_t client, size_t num_ctx, Memory::MemoryManager & mm, PAMI::Device::Generic::Device *devices);
                static inline pami_result_t init_impl(CNAllreduceDevice *devs, size_t client, size_t contextId, pami_client_t clt, pami_context_t ctx, PAMI::Memory::MemoryManager *mm, PAMI::Device::Generic::Device *devices);
                static inline size_t advance_impl(CNAllreduceDevice *devs, size_t client, size_t context);
                static CNAllreduceDevice &getDevice_impl(CNAllreduceDevice *devs, size_t client, size_t context);
        }; // class Factory
}; // class CNAllreduceDevice

};	// BGP
};	// Device
};	// PAMI

extern PAMI::Device::BGP::CNAllreduceDevice _g_cnallreduce_dev;

namespace PAMI {
namespace Device {
namespace BGP {

inline CNAllreduceDevice *CNAllreduceDevice::Factory::generate_impl(size_t client, size_t num_ctx, Memory::MemoryManager &mm, PAMI::Device::Generic::Device *devices) {
        return &_g_cnallreduce_dev;
}

inline pami_result_t CNAllreduceDevice::Factory::init_impl(CNAllreduceDevice *devs, size_t client, size_t contextId, pami_client_t clt, pami_context_t ctx, PAMI::Memory::MemoryManager *mm, PAMI::Device::Generic::Device *devices) {
        return _g_cnallreduce_dev.__init(client, contextId, clt, ctx, mm, devices);
}

inline size_t CNAllreduceDevice::Factory::advance_impl(CNAllreduceDevice *devs, size_t client, size_t contextId) {
        return 0;
}

inline CNAllreduceDevice & CNAllreduceDevice::Factory::getDevice_impl(CNAllreduceDevice *devs, size_t client, size_t contextId) {
        return _g_cnallreduce_dev;
}

class CNAllreduceMessage : public PAMI::Device::BGP::BaseGenericCNMessage {
        enum roles {
                NO_ROLE = 0,
                INJECTION_ROLE = (1 << 0), // first role must be "injector"
                RECEPTION_ROLE = (1 << 1), // last role must be "receptor"
        };
public:
        CNAllreduceMessage(GenericDeviceMessageQueue *qs,
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
        _roles(mcomb->roles)
        {
        }

        // virtual function
        pami_context_t postNext(bool devQueued) {
                return _g_cnallreduce_dev.common()->__postNext<CNAllreduceMessage,CNAllreduceThread>(this, devQueued);
        }

        inline int setThreads(CNAllreduceThread **th) {
                CNAllreduceThread *t;
                int n;
                _g_cnallreduce_dev.__getThreads(&t, &n);
                int nt = 0;
                _g_cnallreduce_dev.common()->__resetThreads();
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
                        // maybe not inject reception here?
                        __advanceRcp(&t[nt]);
                        ++nt;
                }
                // PAMI_assert(nt > 0? && nt < n);
                *th = t;
                return nt;
        }

protected:
        inline void __completeThread(CNAllreduceThread *thr);

        DECL_ADVANCE_ROUTINE(advanceInj,CNAllreduceMessage,CNAllreduceThread);
        DECL_ADVANCE_ROUTINE(advanceRcp,CNAllreduceMessage,CNAllreduceThread);
        inline pami_result_t __advanceInj(CNAllreduceThread *thr) {
                if (thr->_bytesLeft == 0) return PAMI_SUCCESS;
                unsigned hcount = BGPCN_FIFO_SIZE, dcount = BGPCN_QUADS_PER_FIFO;
                size_t avail = thr->_wq->bytesAvailableToConsume();
                char *buf = thr->_wq->bufferToConsume();
                bool aligned = (((unsigned)buf & 0x0f) == 0);
                size_t did = 0;
                if (avail < BGPCN_PKT_SIZE && avail < thr->_bytesLeft) {
                        return PAMI_EAGAIN;
                }
                if (__wait_send_fifo_to(thr, hcount, dcount, thr->_cycles)) {
                        return PAMI_EAGAIN;
                }
                __send_whole_packets(thr, hcount, dcount, avail, did, buf, aligned);
                __send_last_packet(thr, hcount, dcount, avail, did, buf, aligned);
                if (did) {
                        thr->_wq->consumeBytes(did);
                }
                if (thr->_bytesLeft == 0) {
                        thr->setStatus(PAMI::Device::Complete);
                        __completeThread(thr);
                        return PAMI_SUCCESS;
                }
                return PAMI_EAGAIN;
        }


        inline pami_result_t __advanceRcp(CNAllreduceThread *thr) {
                if (thr->_bytesLeft == 0) return PAMI_SUCCESS;
                unsigned hcount = 0, dcount = 0;
                unsigned toCopy = thr->_bytesLeft >= BGPCN_PKT_SIZE ? BGPCN_PKT_SIZE : thr->_bytesLeft;
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
                if (did) {
                        thr->_wq->produceBytes(did);
                }
                if (thr->_bytesLeft == 0) {
                        thr->setStatus(PAMI::Device::Complete);
                        __completeThread(thr);
                        return PAMI_SUCCESS;
                }
                return PAMI_EAGAIN;
        }

        unsigned _roles;
        unsigned _nThreads;
}; // class CNAllreduceMessage

class CNAllreduceModel : public PAMI::Device::Interface::MulticombineModel<CNAllreduceModel,CNAllreduceDevice,sizeof(CNAllreduceMessage)> {
public:
        static const int NUM_ROLES = 2;
        static const int REPL_ROLE = -1;
        static const size_t sizeof_msg = sizeof(CNAllreduceMessage);

        CNAllreduceModel(CNAllreduceDevice &device,pami_result_t &status) :
        PAMI::Device::Interface::MulticombineModel<CNAllreduceModel,CNAllreduceDevice,sizeof(CNAllreduceMessage)>(device,status)
        {
                // PAMI_assert(device == _g_cnallreduce_dev);
                _dispatch_id = _g_cnallreduce_dev.newDispID();
                _me = __global.mapping.task();
                // at least one must do this
                PAMI::Device::BGP::CNAllreduceSetup::initCNAS();
        }

        inline pami_result_t postMulticombine_impl(uint8_t (&state)[sizeof_msg], pami_multicombine_t *mcomb, void *devinfo=NULL);

private:
        size_t _me;
        unsigned _dispatch_id;
}; // class CNAllreduceModel

inline void CNAllreduceMessage::__completeThread(CNAllreduceThread *thr) {
        unsigned c = _g_cnallreduce_dev.common()->__completeThread(thr);
        if (c >= _nThreads) {
                setStatus(PAMI::Device::Done);
        }
}

// Permit a NULL results_topo to mean "everyone" (i.e. "root == -1")
inline pami_result_t CNAllreduceModel::postMulticombine_impl(uint8_t (&state)[sizeof_msg], pami_multicombine_t *mcomb, void *devinfo) {
        PAMI::Device::BGP::CNAllreduceSetup &tas = PAMI::Device::BGP::CNAllreduceSetup::getCNAS(mcomb->dtype, mcomb->optor);
        // PAMI_assert(tas._pre == NULL);
        if (tas._pre) {
                return PAMI_ERROR;
        }
        PAMI::Topology *result_topo = (PAMI::Topology *)mcomb->results_participants;
        bool doStore = (!result_topo || result_topo->isRankMember(_me));
        size_t bytes = mcomb->count << pami_dt_shift[mcomb->dtype];

        // could try to complete allreduce before construction, but for now the code
        // is too dependent on having message and thread structures to get/keep context.
        // __post() will still try early advance... (after construction)
        CNAllreduceMessage *msg;
        msg = new (&state) CNAllreduceMessage(_g_cnallreduce_dev.common(),
                        mcomb, bytes, doStore, _dispatch_id, tas);
        _g_cnallreduce_dev.__post<CNAllreduceMessage>(msg);
        return PAMI_SUCCESS;
}


};	// BGP
};	// Device
};	// PAMI

#endif // __components_devices_bgp_cnallreducemsg_h__
