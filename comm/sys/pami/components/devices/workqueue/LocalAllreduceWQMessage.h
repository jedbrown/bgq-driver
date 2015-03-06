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
 * \file components/devices/workqueue/LocalAllreduceWQMessage.h
 * \brief ???
 */

#ifndef __components_devices_workqueue_LocalAllreduceWQMessage_h__
#define __components_devices_workqueue_LocalAllreduceWQMessage_h__

#include "components/devices/MulticombineModel.h"
#include "components/devices/FactoryInterface.h"
#include "components/devices/workqueue/SharedWorkQueue.h"
#include "components/devices/workqueue/MemoryWorkQueue.h"
#include "math/math_coremath.h"
#include "Global.h"
#include "components/devices/util/SubDeviceSuppt.h"
#include "components/devices/generic/AdvanceThread.h"
#undef TRACE_ERR
#define TRACE_ERR(x) //fprintf x

namespace PAMI {
namespace Device {

class LocalAllreduceWQModel;
class LocalAllreduceWQMessage;
typedef PAMI::Device::Generic::GenericAdvanceThread LocalAllreduceWQThread;
typedef PAMI::Device::Generic::MultiSendQSubDevice<LocalAllreduceWQThread,1,true> LocalAllreduceWQPendQ;
typedef PAMI::Device::Generic::NillSubDevice LocalAllreduceWQDevice;

class LocalAllreduceWQMessage : public PAMI::Device::Generic::GenericMessage {
private:
        enum roles {
                NO_ROLE = 0,
                NON_ROOT_ROLE = (1 << 0), // first role must be non-root(s)
                ROOT_ROLE = (1 << 1), // last role must be root
        };
public:

          ///
          /// \brief Local (intranode) allreduce collective message.
          ///
          /// \param[in] device       Shared Memory device
          /// \param[in] cb           Callback to invoke when the broadcast is complete
          /// \param[in] sharedmemory Location of the shared memory
          /// \param[in] peer         This core's peer id
          /// \param[in] peers        Number of ranks active on this node
          /// \param[in] sbuffer      Location of the source reduce buffer
          /// \param[in] rbuffer      Location of the result reduce buffer
          /// \param[in] count        Number of elements to reduce
          /// \param[in] func         Math function to invoke to perform the reduction
          /// \param[in] dtshift      Shift in byts of the elements for the reduction
          ///
          inline LocalAllreduceWQMessage (GenericDeviceMessageQueue      * device,
					  size_t               client,
					  size_t               context,					  
                                          pami_multicombine_t *mcomb,
                                          PAMI::Device::WorkQueue::SharedWorkQueue & workqueue,
                                          unsigned          peer,
                                          unsigned          npeers,
                                          coremath          func,
                                          int               dtshift) :
            PAMI::Device::Generic::GenericMessage (device, mcomb->cb_done,
						   client, context),
            _peer (peer),
            _func (func),
            _dtshift (dtshift),
            _source ((PAMI::PipeWorkQueue *)mcomb->data),
            _result ((PAMI::PipeWorkQueue *)mcomb->results),
            _shared (workqueue)
          {
            TRACE_ERR((stderr,  "%s enter\n", __PRETTY_FUNCTION__));
          }

protected:
        DECL_ADVANCE_ROUTINE(advanceThread,LocalAllreduceWQMessage,LocalAllreduceWQThread);
        inline pami_result_t __advanceThread(LocalAllreduceWQThread *thr) {
                if (_peer == 0) {
                        _shared.Q2Q (*_source, (coremath1) PAMI::Device::WorkQueue::SharedWorkQueue::shmemcpy, 0);
                } else {
                        _shared.reduceInPlace (*_source, _func, _dtshift);
                }

                _shared.Q2Qr (*_result, (coremath1) PAMI::Device::WorkQueue::SharedWorkQueue::shmemcpy, 0);

                // If all bytes have been copied from the shared queue into the
                // local result buffer then the peer is done.
                // NOTE! This assumes the result WQ is a "flat buffer" and thus
                // actually has a "hard stop".
                if (_result->bytesAvailableToProduce() == 0) {
                        setStatus(PAMI::Device::Done);
                        thr->setStatus(PAMI::Device::Complete);
                }

                return getStatus() == PAMI::Device::Done ? PAMI_SUCCESS : PAMI_EAGAIN;
        }

public:
        // virtual function
        pami_context_t postNext(bool devQueued) {
		LocalAllreduceWQPendQ *qs = (LocalAllreduceWQPendQ *)getQS();
                return qs->__postNext<LocalAllreduceWQMessage>(this, devQueued);
        }

        inline int setThreads(LocalAllreduceWQThread **th) {
		LocalAllreduceWQPendQ *qs = (LocalAllreduceWQPendQ *)getQS();
                LocalAllreduceWQThread *t;
                int n;
                qs->__getThreads(&t, &n);
                t->setMsg(this);
                t->setAdv(advanceThread);
                t->setStatus(PAMI::Device::Ready);
                __advanceThread(t);
                *th = t;
                return 1;
        }

private:

          unsigned          _peer;
          coremath          _func;
          int               _dtshift;
          PAMI::PipeWorkQueue   *_source;
          PAMI::PipeWorkQueue   *_result;
          PAMI::Device::WorkQueue::SharedWorkQueue & _shared;
}; // class LocalAllreduceWQMessage

class LocalAllreduceWQModel : public PAMI::Device::Interface::MulticombineModel<LocalAllreduceWQModel,LocalAllreduceWQDevice,sizeof(LocalAllreduceWQMessage)> {
public:
        static const int NUM_ROLES = 2;
        static const int REPL_ROLE = 1;
        static const size_t sizeof_msg = sizeof(LocalAllreduceWQMessage);

        LocalAllreduceWQModel(LocalAllreduceWQDevice &device, pami_result_t &status) :
        PAMI::Device::Interface::MulticombineModel<LocalAllreduceWQModel,LocalAllreduceWQDevice,sizeof(LocalAllreduceWQMessage)>(device, status),
	_gd(&device),
        _peer(__global.topology_local.rank2Index(__global.mapping.task())),
        _npeers(__global.topology_local.size())
        {
		TRACE_ERR((stderr, "%s enter\n", __PRETTY_FUNCTION__));
		char key[PAMI::Memory::MMKEYSIZE];
		sprintf(key, "/LocalAllreduceWQModel-%zd-%zd",
					_gd->clientId(), _gd->contextId());
		new (&_shared) PAMI::Device::WorkQueue::SharedWorkQueue(_gd->getMM(), key);
                if (!_shared.available()) {
                        status = PAMI_ERROR;
                        return;
                }
                _shared.barrier_reset(_npeers, (_peer == 0));
                _shared.setProducers(_npeers, _peer);
                _shared.setConsumers(_npeers, _peer);
                // since we hard-code topology_local, we know _peer==0 exists...
		_queue.__init(_gd->clientId(), _gd->contextId(), NULL, _gd->getContext(), _gd->getMM(), _gd->getAllDevs());
        }

        inline void reset_impl() {
                if (_peer == 0) {
                        _shared.reset();
                }
        }
	
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
        PAMI::Device::WorkQueue::SharedWorkQueue _shared;
        unsigned _peer;
        unsigned _npeers;
	LocalAllreduceWQPendQ _queue;
}; // class LocalAllreduceWQModel

 inline pami_result_t LocalAllreduceWQModel::postMulticombine_impl(uint8_t (&state)[sizeof_msg], size_t client, size_t context, pami_multicombine_t *mcomb, void *devinfo) {
        // assert((data_topo .U. results_topo).size() == _npeers);
        int dtshift = pami_dt_shift[mcomb->dtype];
        coremath func = MATH_OP_FUNCS(mcomb->dtype, mcomb->optor, 2);

        LocalAllreduceWQMessage *msg =
                new (&state) LocalAllreduceWQMessage(_queue.getQS(),
						     client, context, 
						     mcomb, _shared, _peer, _npeers,
						     func, dtshift);
        _queue.__post<LocalAllreduceWQMessage>(msg);
        return PAMI_SUCCESS;
}

}; // namespace Device
}; // namespace PAMI

#endif
