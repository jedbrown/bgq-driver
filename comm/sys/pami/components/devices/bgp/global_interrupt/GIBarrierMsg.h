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
#ifndef __components_devices_bgp_global_interrupt_GIBarrierMsg_h__
#define __components_devices_bgp_global_interrupt_GIBarrierMsg_h__

#include "util/common.h"
#include "components/devices/MultisyncModel.h"
#include "components/devices/FactoryInterface.h"
#include "components/devices/util/SubDeviceSuppt.h"
#include "components/devices/generic/AdvanceThread.h"
#include "spi/bgp_SPI.h"

////////////////////////////////////////////////////////////////////////
///  \file components/devices/bgp/global_interrupt/GIBarrierMsg.h
///  \brief Global Interrupt Device
///
///  The GI classes implement a BaseDevice and a Message object
///  to post into the queueing system.  The GI device is currently
///  used to implement barriers, so the GI device posts a message
///  and uses a global interrupt sysdep to signal the GI wire
///  This is used to implement
///  -
///  - Barriers
///
///  Definitions:
///  - giMessage:  A global interrupt message
///  - Device:     The global interrupt queue system
///
///  Namespace:  PAMI, the messaging namespace.
///
////////////////////////////////////////////////////////////////////////

namespace PAMI {
namespace Device {
namespace BGP {

class giModel;
class giMessage;
typedef PAMI::Device::Generic::GenericAdvanceThread giThread;
class giDevice : public PAMI::Device::Generic::MultiSendQSubDevice<giThread,1,true> {
public:
        class Factory : public Interface::FactoryInterface<Factory,giDevice,Generic::Device> {
        public:
                static inline giDevice *generate_impl(size_t client, size_t num_ctx, Memory::MemoryManager &mm, PAMI::Device::Generic::Device *devices);
                static inline pami_result_t init_impl(giDevice *devs, size_t client, size_t contextId, pami_client_t clt, pami_context_t ctx, PAMI::Memory::MemoryManager *mm, PAMI::Device::Generic::Device *devices);
                static inline size_t advance_impl(giDevice *devs, size_t client, size_t contextId);
                static inline giDevice & getDevice_impl(giDevice *devs, size_t client, size_t contextId);
        }; // class Factory
}; // class giDevice

}; // namespace BGP
}; // namespace Device
}; // namespace PAMI

extern PAMI::Device::BGP::giDevice _g_gibarrier_dev;

namespace PAMI {
namespace Device {
namespace BGP {

inline giDevice *giDevice::Factory::generate_impl(size_t client, size_t num_ctx, Memory::MemoryManager &mm, PAMI::Device::Generic::Device *devices) {
        return &_g_gibarrier_dev;
}

inline pami_result_t giDevice::Factory::init_impl(giDevice *devs, size_t client, size_t contextId, pami_client_t clt, pami_context_t ctx, PAMI::Memory::MemoryManager *mm, PAMI::Device::Generic::Device *devices) {
        return _g_gibarrier_dev.__init(client, contextId, clt, ctx, mm, devices);
}

inline size_t giDevice::Factory::advance_impl(giDevice *devs, size_t client, size_t contextId) {
        return 0;
}

inline giDevice & giDevice::Factory::getDevice_impl(giDevice *devs, size_t client, size_t contextId) {
        return _g_gibarrier_dev;
}

//////////////////////////////////////////////////////////////////////
///  \brief Global interrupt message class
///  This message is posted to a GI device
//////////////////////////////////////////////////////////////////////
class giMessage : public PAMI::Device::Generic::GenericMessage {
public:

protected:
        friend class giModel;

        //////////////////////////////////////////////////////////////////
        /// \brief  GI Message constructor
        /// \param cb: A "done" callback structure to be executed
        //////////////////////////////////////////////////////////////////
        giMessage(GenericDeviceMessageQueue *GI_QS, pami_multisync_t *msync) :
        PAMI::Device::Generic::GenericMessage(GI_QS, msync->cb_done, 0, 0)
        {
          // Since the above args to the GenericMessage constructor are bogus, we shouldn't use this.
          PAMI_abort();
        }

protected:
        static const int GI_CHANNEL = 0;

        DECL_ADVANCE_ROUTINE(advanceThread,giMessage,giThread);
        inline pami_result_t __advanceThread(giThread *thr) {
                PAMI::Device::MessageStatus stat = getStatus();

                unsigned loop = 32;
                while (stat != PAMI::Device::Done && loop--) {
                        switch(stat) {
                        case PAMI::Device::Initialized:
                                GlobInt_InitBarrier(GI_CHANNEL);
                                stat = PAMI::Device::Active;
                                // FALLTHROUGH
                        case PAMI::Device::Active:
                                if (GlobInt_QueryDone(GI_CHANNEL) == 0) {
                                        break;
                                }
                                stat = PAMI::Device::Done;
                                // FALLTHROUGH
                        case PAMI::Device::Done:
                                thr->setStatus(PAMI::Device::Complete);
                                break;
                        default:
                                PAMI_abortf("Unexpected message status of %d (loop %d)\n", stat, loop);
                        }
                }
                setStatus(stat);
                return stat == PAMI::Device::Done ? PAMI_SUCCESS : PAMI_EAGAIN;
        }

public:
        // virtual function
        pami_context_t postNext(bool devQueued) {
                return _g_gibarrier_dev.__postNext<giMessage>(this, devQueued);
        }

        inline int setThreads(giThread **th) {
                // This is only called if we are the top of the queue.
                // We get our threads object(s) from our device.
                giThread *t;
                int n;
                _g_gibarrier_dev.__getThreads(&t, &n);
                t->setMsg(this);
                t->setAdv(advanceThread);
                t->setStatus(PAMI::Device::Ready);
                __advanceThread(t);
                *th = t;
                return 1;
        }

protected:
}; // class giMessage

class giModel : public PAMI::Device::Interface::MultisyncModel<giModel,giDevice,sizeof(giMessage)> {
public:
        static const size_t sizeof_msg = sizeof(giMessage);

        giModel(giDevice &device, pami_result_t &status) :
        PAMI::Device::Interface::MultisyncModel<giModel,giDevice,sizeof(giMessage)>(device,status)
        {
                // PAMI_assert(device == _g_gibarrier_dev);
                // if we need sysdep, use _g_gibarrier_dev.getSysdep()...
        }

        inline pami_result_t postMultisync_impl(uint8_t (&state)[sizeof_msg], pami_multisync_t *msync, void *devinfo=NULL);

private:
}; // class giModel

}; // namespace BGP
}; // namespace Device
}; // namespace PAMI

inline pami_result_t PAMI::Device::BGP::giModel::postMultisync_impl(uint8_t (&state)[sizeof_msg], pami_multisync_t *msync, void *devinfo) {
        // PAMI_assert(participants == ctor topology)
        giMessage *msg;

        msg = new (&state) giMessage(_g_gibarrier_dev.getQS(), msync);
        _g_gibarrier_dev.__post<giMessage>(msg);
        return PAMI_SUCCESS;
}

#endif // __components_devices_bgp_gibarriermsg_h__
