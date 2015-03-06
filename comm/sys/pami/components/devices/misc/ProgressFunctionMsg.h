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
 * \file components/devices/misc/ProgressFunctionMsg.h
 * \brief Add a general function to the progress engine loop/queue
 */

#ifndef __components_devices_misc_ProgressFunctionMsg_h__
#define __components_devices_misc_ProgressFunctionMsg_h__

#include "components/devices/generic/Device.h"
#include "components/devices/generic/AdvanceThread.h"
#include <pami.h>
#include "components/devices/FactoryInterface.h"

typedef struct {
        size_t client;
        size_t context;
        void *request;
        pami_work_function func;
        void *clientdata;
        pami_callback_t cb_done;
} PAMI_ProgressFunc_t;

namespace PAMI {
namespace Device {

class ProgressFunctionMdl;

class ProgressFunctionDev {
public:
        class Factory : public Interface::FactoryInterface<Factory,ProgressFunctionDev,Generic::Device>  {
        public:
                static inline ProgressFunctionDev *generate_impl(size_t client, size_t num_ctx, Memory::MemoryManager & mm, PAMI::Device::Generic::Device *devices);
                static inline pami_result_t init_impl(ProgressFunctionDev *devs, size_t client, size_t contextId, pami_client_t clt, pami_context_t ctx, PAMI::Memory::MemoryManager *mm, PAMI::Device::Generic::Device *devices);
                static inline size_t advance_impl(ProgressFunctionDev *devs, size_t client, size_t context);
                static inline ProgressFunctionDev & getDevice_impl(ProgressFunctionDev *devs, size_t client, size_t context);
        }; // class ProgressFunctionDev::Factory
        inline ProgressFunctionDev() {}

        inline pami_result_t init(size_t client, size_t ctx, PAMI::Device::Generic::Device *devices) {
                _generics[client] = devices;
                return PAMI_SUCCESS;
        }

        inline PAMI::Device::Generic::Device *getGeneric(size_t client, size_t contextId) {
                return &_generics[client][contextId];
        }

        inline pami_context_t getContext(size_t client, size_t contextId) {
                return getGeneric(client, contextId)->getContext();
        }
protected:
        PAMI::Device::Generic::Device *_generics[PAMI_MAX_NUM_CLIENTS];
}; // class ProgressFunctionDev

}; //-- Device
}; //-- PAMI

extern PAMI::Device::ProgressFunctionDev _g_progfunc_dev;

namespace PAMI {
namespace Device {

inline ProgressFunctionDev *ProgressFunctionDev::Factory::generate_impl(size_t client, size_t num_ctx, Memory::MemoryManager & mm, PAMI::Device::Generic::Device *devices) {
        return &_g_progfunc_dev;
}

inline pami_result_t ProgressFunctionDev::Factory::init_impl(ProgressFunctionDev *devs, size_t client, size_t contextId, pami_client_t clt, pami_context_t ctx, PAMI::Memory::MemoryManager *mm, PAMI::Device::Generic::Device *devices) {
        return _g_progfunc_dev.init(client, contextId, devices);
}

inline size_t ProgressFunctionDev::Factory::advance_impl(ProgressFunctionDev *devs, size_t client, size_t context) { return 0; }

inline ProgressFunctionDev & ProgressFunctionDev::Factory::getDevice_impl(ProgressFunctionDev *devs, size_t client, size_t context) { return _g_progfunc_dev; }


/// If this ever expands into multiple types, need to make this a subclass
class ProgressFunctionMdl {

public:
        static const size_t sizeof_msg = sizeof(PAMI::Device::Generic::GenericThread);

        /// This model is typically never constructed, but rather just
        /// generateMessage called directly.
        ProgressFunctionMdl() {
        }
        /// In case someone constructs it the "standard" way, don't complain.
        ProgressFunctionMdl(pami_result_t &status) {
                status = PAMI_SUCCESS;
        }

        inline bool postWork(PAMI_ProgressFunc_t *pf);

private:
}; // class ProgressFunctionMdl

}; //-- Device
}; //-- PAMI

inline bool PAMI::Device::ProgressFunctionMdl::postWork(PAMI_ProgressFunc_t *pf) {
        PAMI::Device::Generic::GenericThread *thr = (PAMI::Device::Generic::GenericThread *)pf->request;
        PAMI::Device::Generic::Device *gd;
#if 0
#warning can we really advance this progress function here?
        // need a better way to get pami_context_t...
        // problem is that this "message" has not even been constructed yet,
        // let alone posted to a generic device queue, so we have no other
        // way to derive the pami_context_t (unless it is passed-in).
        pami_context_t ctx = _g_progfunc_dev.getContext(pf->client, pf->context);
        int rc = pf->func(ctx, pf->clientdata);
        if (rc != PAMI_EAGAIN) {
                if (pf->cb_done.function) {
                        pf->cb_done.function(ctx, pf->cb_done.clientdata, rc);
                }
                return true;
        }
#endif
        new (thr) PAMI::Device::Generic::GenericThread(pf->func, pf->clientdata);
        gd = _g_progfunc_dev.getGeneric(pf->client, pf->context);
        thr->setStatus(PAMI::Device::Ready);
        gd->postThread(thr);
        return true;
}

#endif /* __components_devices_generic_progressfunction_h__ */
