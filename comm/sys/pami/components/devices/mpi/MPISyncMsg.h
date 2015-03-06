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
 * \file components/devices/mpi/MPISyncMsg.h
 * \brief ???
 */

#ifndef __components_devices_mpi_MPISyncMsg_h__
#define __components_devices_mpi_MPISyncMsg_h__

#include "components/devices/generic/AdvanceThread.h"
#include "components/devices/util/SubDeviceSuppt.h"
#include <pami.h>
#include "Global.h"
#include "PipeWorkQueue.h"
#include "Topology.h"
#include "components/devices/MultisyncModel.h"

#undef TRACE_DEVICE
#ifndef TRACE_DEVICE
  #define TRACE_DEVICE(x) //fprintf x
#endif

namespace PAMI
{
  namespace Device
  {

    class MPISyncMdl;

    class MPISyncDev : public PAMI::Device::Generic::MultiSendQSubDevice<PAMI::Device::Generic::SimpleAdvanceThread,1,true>
    {
    public:
      MPI_Comm _msync_communicator;
      MPISyncDev() :
      PAMI::Device::Generic::MultiSendQSubDevice<PAMI::Device::Generic::SimpleAdvanceThread,1,true>(),
      _msync_communicator(MPI_COMM_NULL)
      {
      }

        class Factory : public Interface::FactoryInterface<Factory,MPISyncDev,Generic::Device> {
        public:
                static inline MPISyncDev *generate_impl(size_t client, size_t num_ctx, Memory::MemoryManager & mm, PAMI::Device::Generic::Device *devices);
                static inline pami_result_t init_impl(MPISyncDev *devs, size_t client, size_t contextId, pami_client_t clt, pami_context_t ctx, PAMI::Memory::MemoryManager *mm, PAMI::Device::Generic::Device *devices);
                static inline size_t advance_impl(MPISyncDev *devs, size_t client, size_t context);
                static MPISyncDev &getDevice_impl(MPISyncDev *devs, size_t client, size_t context);
        }; // class Factory

    };
  }; //-- Device
}; //-- PAMI

static PAMI::Device::MPISyncDev _g_mpisync_dev;

namespace PAMI
{
  namespace Device
  {

inline MPISyncDev *MPISyncDev::Factory::generate_impl(size_t client, size_t num_ctx, Memory::MemoryManager &mm, PAMI::Device::Generic::Device *devices) {
        return &_g_mpisync_dev;
}

inline pami_result_t MPISyncDev::Factory::init_impl(MPISyncDev *devs, size_t client, size_t contextId, pami_client_t clt, pami_context_t ctx, PAMI::Memory::MemoryManager *mm, PAMI::Device::Generic::Device *devices) {
        MPI_Comm_dup(MPI_COMM_WORLD,&_g_mpisync_dev._msync_communicator);
        return _g_mpisync_dev.__init(client, contextId, clt, ctx, mm, devices);
}

inline size_t MPISyncDev::Factory::advance_impl(MPISyncDev *devs, size_t client, size_t contextId) {
        return 0;
}

inline MPISyncDev & MPISyncDev::Factory::getDevice_impl(MPISyncDev *devs, size_t client, size_t contextId) {
        return _g_mpisync_dev;
}

///
/// \brief
///
    class MPISyncMsg : public PAMI::Device::Generic::GenericMessage
    {
    private:
      enum roles
      {
        NO_ROLE = 0,
        ROOT_ROLE = (1 << 0), // first role must be root
        NON_ROOT_ROLE = (1 << 1), // last role must be non-root(s)
      };
    public:
      MPISyncMsg(GenericDeviceMessageQueue *Generic_QS,
                 pami_multisync_t *msync) :
      PAMI::Device::Generic::GenericMessage(Generic_QS, msync->cb_done,
                                           0,0), // hardcoded client/context ids
      _participants((PAMI::Topology *)msync->participants),
      _tag(msync->connection_id),
      _idx(0),
      _req(MPI_REQUEST_NULL),
      _pendingStatus(PAMI::Device::Initialized),
      _root(_participants->index2Rank(0))
      {
        TRACE_DEVICE((stderr,"<%p>MPISyncMsg client %zu, context %zu\n",this,
                      msync->client, msync->context));
        if(_participants->size() == 1)
        {
          // We have to use a local pending status because the sub device is too smart for us and will
          // reset the _status to initialized after __setThreads
          _pendingStatus = PAMI::Device::Done; //setStatus(PAMI::Device::Done);
          return;
        }
        if(_root == __global.mapping.task())
        {
          _idx++;
        }
        else
        {
          int rc = MPI_Send(NULL, 0, MPI_BYTE,
                            _root,_tag,_g_mpisync_dev._msync_communicator);
          TRACE_DEVICE((stderr,"<%p>MPISyncMsg::ctor send rc = %d, dst %zu, tag %d \n",this,
                        rc, _root, _tag));
        }
        int rc = MPI_Irecv(NULL,0, MPI_BYTE,
                           _participants->index2Rank(_idx),_tag,
                           _g_mpisync_dev._msync_communicator, &_req);
        TRACE_DEVICE((stderr,"<%p>MPISyncMsg::ctor irecv rc = %d, dst %zu, tag %d \n",this,
                      rc, _participants->index2Rank(_idx), _tag));
      }

      // virtual function
      pami_context_t postNext(bool devQueued) {
        return _g_mpisync_dev.__postNext<MPISyncMsg>(this, devQueued);
      }

      inline int setThreads(PAMI::Device::Generic::SimpleAdvanceThread **th)
      {
        PAMI::Device::Generic::SimpleAdvanceThread *t;
        int n;
        _g_mpisync_dev.__getThreads(&t, &n);
        int nt = 0;
        // PAMI_assert(nt < n);
        t[nt].setMsg(this);
        t[nt].setAdv(advanceThread);
        t[nt].setStatus(PAMI::Device::Ready);
        ++nt;
        // PAMI_assert(nt > 0? && nt < n);
        _nThreads = nt;
        TRACE_DEVICE((stderr,"<%p>MPISyncMsg::__setThreads(%d) _nThreads %d\n",this,
                      n,nt));
        *th = t;
        return nt;
      }

protected:
      DECL_ADVANCE_ROUTINE(advanceThread,MPISyncMsg,PAMI::Device::Generic::SimpleAdvanceThread);
      inline pami_result_t __advanceThread(PAMI::Device::Generic::SimpleAdvanceThread *thr)
      {
        if(getStatus() == PAMI::Device::Done)
        {
          fprintf(stderr, "Warning: message/thread advanced after Done\n");
          return PAMI_SUCCESS;
        }
        if(_pendingStatus == PAMI::Device::Done)
        {
          // This happens when there is no data to send/receive and ctor set a "pending status" to done,
          //  so on the first advance, setDone and return.
          thr->setStatus(PAMI::Device::Complete);
          setStatus(PAMI::Device::Done);
          TRACE_DEVICE((stderr,"<%p>MPISyncMsg::__advanceThread() done - no participants\n",this));
          return PAMI_SUCCESS;
        }
        int flag = 0;
        MPI_Status status;
        //static unsigned count = 5; if(count) count--;
        //if(count)TRACE_DEVICE((stderr,"<%p>MPISyncMsg::__advanceThread() idx %zu/%zu, currBytes %zu, bytesLeft %zu, tag %d %s\n",this,
        //              _idx, _dst->size(), _currBytes, thr->_bytesLeft, _tag,_req == MPI_REQUEST_NULL?"MPI_REQUEST_NULL":""));
        if(_req != MPI_REQUEST_NULL)
        {
          MPI_Test(&_req, &flag, &status);
          if(flag)
          {
            TRACE_DEVICE((stderr,"<%p>MPISyncMsg::__advanceThread test done, dst %zu, tag %d \n",this,
                          _participants->index2Rank(_idx), _tag));
            _req = MPI_REQUEST_NULL; // redundant?
          }
          else
          {
            return PAMI_EAGAIN;
          }
          // current message was completed...
        }
        if(_root == __global.mapping.task())
        {
          if(++_idx < _participants->size())
          {
            int rc = MPI_Irecv(NULL,0, MPI_BYTE,
                               _participants->index2Rank(_idx),_tag,
                               _g_mpisync_dev._msync_communicator, &_req);
            TRACE_DEVICE((stderr,"<%p>MPISyncMsg::__advanceThread irecv rc = %d, dst %zu, tag %d \n",this,
                          rc, _participants->index2Rank(_idx), _tag));
            return PAMI_EAGAIN;
          }
          else
          {
            for(size_t idx=1; idx < _participants->size(); ++idx)
            {
              int rc = MPI_Send(NULL, 0, MPI_BYTE,
                                _participants->index2Rank(idx),_tag,
                                _g_mpisync_dev._msync_communicator);
              TRACE_DEVICE((stderr,"<%p>MPISyncMsg::__advanceThread() sending rc = %d, idx %zu, dst %zu, tag %d\n",this,
                            rc,idx, _participants->index2Rank(idx), _tag));
            }
          }
        }
        thr->setStatus(PAMI::Device::Complete);
        setStatus(PAMI::Device::Done);
        return PAMI_SUCCESS;
      }

      unsigned _nThreads;
      PAMI::Topology *_participants;
      int            _tag;
      size_t         _idx;
      MPI_Request    _req;
      MessageStatus  _pendingStatus;
      size_t         _root; // first rank in the sync - arbitrary 'root'
    }; //-- MPISyncMsg

    class MPISyncMdl : public PAMI::Device::Interface::MultisyncModel<MPISyncMdl,MPISyncDev,sizeof(MPISyncMsg)>
    {
    public:
      static const int NUM_ROLES = 2;
      static const int REPL_ROLE = 1;
      static const size_t sizeof_msg = sizeof(MPISyncMsg);

      MPISyncMdl(MPISyncDev dev, pami_result_t &status) :
      PAMI::Device::Interface::MultisyncModel<MPISyncMdl,MPISyncDev,sizeof(MPISyncMsg)>(dev, status)
      {
        TRACE_DEVICE((stderr,"<%p>MPISyncMdl()\n",this));
        //PAMI::SysDep *sd = _g_mpisync_dev.getSysdep();
      }

      inline pami_result_t postMultisync_impl(uint8_t         (&state)[sizeof_msg],
						size_t clientid, size_t contextid,
                                              pami_multisync_t *msync,
                                              void *devinfo = NULL);

    private:
    }; // class MPISyncMdl

    inline pami_result_t MPISyncMdl::postMultisync_impl(uint8_t         (&state)[sizeof_msg],
						size_t clientid, size_t contextid,
                                                        pami_multisync_t *msync,
                                                        void             *devinfo)
    {
      TRACE_DEVICE((stderr,"<%p>MPISyncMdl::postMulticast() connection_id %d, request %p\n",this,
                    msync->connection_id, &state));
      MPISyncMsg *msg =
      new (&state) MPISyncMsg(_g_mpisync_dev.getQS(), msync);
      _g_mpisync_dev.__post<MPISyncMsg>(msg);
      return PAMI_SUCCESS;
    }

  }; //-- Device
}; //-- PAMI
#undef TRACE_DEVICE
#endif // __components_devices_workqueue_mpisyncmsg_h__
