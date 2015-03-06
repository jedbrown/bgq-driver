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
 * \file components/devices/mpi/MPIBcastMsg.h
 * \brief ???
 */

#ifndef __components_devices_mpi_MPIBcastMsg_h__
#define __components_devices_mpi_MPIBcastMsg_h__

#include "components/devices/generic/AdvanceThread.h"
#include "components/devices/util/SubDeviceSuppt.h"
#include <pami.h>
#include "Global.h"
#include "PipeWorkQueue.h"
#include "Topology.h"
#include "components/devices/MulticastModel.h"

#ifndef DO_DEBUG
 #define DO_DEBUG(x) //x
#endif

#undef TRACE_DEVICE
#ifndef TRACE_DEVICE
#define TRACE_DEVICE(x) //fprintf x
#endif

namespace PAMI
{
  namespace Device
  {

    class MPIBcastMdl;

    class MPIBcastDev : public PAMI::Device::Generic::MultiSendQSubDevice<PAMI::Device::Generic::SimpleAdvanceThread,1,true>
    {
    public:
      MPI_Comm _mcast_communicator;
      MPIBcastDev() :
      PAMI::Device::Generic::MultiSendQSubDevice<PAMI::Device::Generic::SimpleAdvanceThread,1,true>(),
      _mcast_communicator(MPI_COMM_NULL)
      {
      };

        class Factory : public Interface::FactoryInterface<Factory,MPIBcastDev,Generic::Device> {
        public:
                static inline MPIBcastDev *generate_impl(size_t client, size_t num_ctx, Memory::MemoryManager & mm, PAMI::Device::Generic::Device *devices);
                static inline pami_result_t init_impl(MPIBcastDev *devs, size_t client, size_t contextId, pami_client_t clt, pami_context_t ctx, PAMI::Memory::MemoryManager *mm, PAMI::Device::Generic::Device *devices);
                static inline size_t advance_impl(MPIBcastDev *devs, size_t client, size_t context);
                static MPIBcastDev &getDevice_impl(MPIBcastDev *devs, size_t client, size_t context);
        }; // class Factory

    };
  }; //-- Device
}; //-- PAMI

static PAMI::Device::MPIBcastDev _g_mpibcast_dev;

namespace PAMI
{
  namespace Device
  {

inline MPIBcastDev *MPIBcastDev::Factory::generate_impl(size_t client, size_t num_ctx, Memory::MemoryManager &mm, PAMI::Device::Generic::Device *devices) {
        return &_g_mpibcast_dev;
}

inline pami_result_t MPIBcastDev::Factory::init_impl(MPIBcastDev *devs, size_t client, size_t contextId, pami_client_t clt, pami_context_t ctx, PAMI::Memory::MemoryManager *mm, PAMI::Device::Generic::Device *devices) {
        MPI_Comm_dup(MPI_COMM_WORLD,&_g_mpibcast_dev._mcast_communicator);
        return _g_mpibcast_dev.__init(client, contextId, clt, ctx, mm, devices);
}

inline size_t MPIBcastDev::Factory::advance_impl(MPIBcastDev *devs, size_t client, size_t contextId) {
        return 0;
}

inline MPIBcastDev & MPIBcastDev::Factory::getDevice_impl(MPIBcastDev *devs, size_t client, size_t contextId) {
        return _g_mpibcast_dev;
}

///
/// \brief
///
    class MPIBcastMsg : public PAMI::Device::Generic::GenericMessage
    {
    private:
      enum roles
      {
        NO_ROLE = 0,
        ROOT_ROLE = (1 << 0), // first role must be root
        NON_ROOT_ROLE = (1 << 1), // last role must be non-root(s)
      };
    public:
      MPIBcastMsg(GenericDeviceMessageQueue *Generic_QS,
                  pami_multicast_t *mcast) :
      PAMI::Device::Generic::GenericMessage(Generic_QS, mcast->cb_done,
                                           0,0), // hardcoded clientid,contextid
      _dst((PAMI::Topology *)mcast->dst_participants),
      _iwq((PAMI::PipeWorkQueue *)mcast->src),
      _rwq((PAMI::PipeWorkQueue *)mcast->dst),
      _bytes(mcast->bytes),
      _tag(mcast->connection_id),
      _idx(0),
      _currBytes(0),
      _currBuf(NULL),
      _req(MPI_REQUEST_NULL),
      _pendingStatus(PAMI::Device::Initialized)
      {
        PAMI::Topology *src_topo = (PAMI::Topology *)mcast->src_participants;
        //PAMI_assert(src_topo != NULL);
        if(src_topo && (src_topo->type()!=PAMI_EMPTY_TOPOLOGY))
        {
          _root = src_topo->index2Rank(0);

          //I've had some bad topo's, so try to detect it here...
          PAMI_assert(src_topo->size() == 1); //technically only one root...
          PAMI_assert((0 == src_topo->rank2Index(_root)) && src_topo->isRankMember(_root));
        }
        else // we must be a dst_participant and we don't particularly care who is the root - just not me.
          _root = MPI_ANY_SOURCE;

        TRACE_DEVICE((stderr,"<%p>MPIBcastMsg client %#zX, context %zu, root %zu, dst size %zu, iwq %p, rwq %p, bytes %zu/%zu/%zu\n",this,
                      mcast->client, mcast->context, _root, _dst->size(),  _iwq, _rwq, _bytes,
                      _iwq?_iwq->bytesAvailableToConsume():-1,
                      _rwq?_rwq->bytesAvailableToProduce():-1));
        bool iamroot = (_root == __global.mapping.task());
        if(iamroot)
        {
          TRACE_DEVICE((stderr,"<%p>MPIBcastMsg i am root\n",this));
          //_rwq = NULL;// \todo Why?  Can't a source (root) also be a destination?

          // no actual data to send, indicate we're done with a pending status (for advance)
          if((_iwq == NULL) || (_bytes == 0))
          {
            TRACE_DEVICE((stderr,"<%p>MPIBcastMsg root has no data\n",this));
            // We have to use a local pending status because the sub device is too smart for us and will
            // reset the _status to initialized after __setThreads
            _pendingStatus = PAMI::Device::Done; //setStatus(PAMI::Device::Done);
          }
          DO_DEBUG(unsigned j = 0;)
          DO_DEBUG(for(unsigned j=0; j<_dst->size(); ++j) fprintf(stderr,"<%p>MPIBcastMsg _dst[%u]=%zu, size %zu\n",this,j,(size_t)_dst->index2Rank(j),_dst->size()));
        }
        else // I must be a dst_participant
        {
          TRACE_DEVICE((stderr,"<%p>MPIBcastMsg i am dst\n",this));
          //PAMI_assert(_dst->isRankMember(__global.mapping.task()));
          // no actual data to send, indicate we're done with a pending status (for advance)
          if((_rwq == NULL) && (_bytes == 0))
          {
            TRACE_DEVICE((stderr,"<%p>MPIBcastMsg dst expects no data\n",this));
            // We have to use a local pending status because the sub device is too smart for us and will
            // reset the _status to initialized after __setThreads
            _pendingStatus = PAMI::Device::Done; //setStatus(PAMI::Device::Done);
          }
          _iwq = NULL; // ignore the src pwq
        }
        //PAMI_assertf(_rwq || _iwq, "MPIBcastMsg has neither input or output data\n");
      }

      // virtual function
      pami_context_t postNext(bool devQueued) {
        TRACE_DEVICE((stderr,"<%p>MPIBcastMsg::postNext(%u)\n",this,(unsigned) devQueued));
        return _g_mpibcast_dev.__postNext<MPIBcastMsg>(this, devQueued);
      }

      inline int setThreads(PAMI::Device::Generic::SimpleAdvanceThread **th)
      {
        PAMI::Device::Generic::SimpleAdvanceThread *t;
        int n;
        _g_mpibcast_dev.__getThreads(&t, &n);
        int nt = 0;
        _nThreads = 1;  // must predict total number of threads now,
        // so early advance(s) work
        t[nt].setMsg(this);
        t[nt].setAdv(advanceThread);
        t[nt].setStatus(PAMI::Device::Ready);
        t[nt]._bytesLeft = _bytes;
        __advanceThread(&t[nt]);
        ++nt;
        // PAMI_assert(nt > 0? && nt < n);
        TRACE_DEVICE((stderr,"<%p>MPIBcastMsg::__setThreads(%d) _nThreads %d, bytes left %zu\n",this,
                      n,nt,t[nt]._bytesLeft));
        *th = t;
        return nt;
      }

    protected:
      DECL_ADVANCE_ROUTINE(advanceThread,MPIBcastMsg,PAMI::Device::Generic::SimpleAdvanceThread);
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
          TRACE_DEVICE((stderr,"<%p>MPIBcastMsg::__advanceThread() done - no data\n",this));
          return PAMI_SUCCESS;
        }
        int flag = 0;
        MPI_Status status;
        DO_DEBUG(static unsigned count = 5);
        DO_DEBUG(if(count) count--);
        DO_DEBUG(if(count)TRACE_DEVICE((stderr,"<%p>MPIBcastMsg::__advanceThread() idx %zu/%zu, currBytes %zu, bytesLeft %zu, tag %d %s\n",this,
                                        _idx, _dst->size(), _currBytes, thr->_bytesLeft, _tag,_req == MPI_REQUEST_NULL?"MPI_REQUEST_NULL":"")));
        if(_req != MPI_REQUEST_NULL)
        {
          MPI_Test(&_req, &flag, &status);
          DO_DEBUG(if(count)TRACE_DEVICE((stderr,"<%p>MPIBcastMsg::__advanceThread() MPI_Test(_req, flag=%d, status) %s;\n",this,flag,!flag?"EAGAIN":" ")));
          if(flag)
          {
            _req = MPI_REQUEST_NULL; // redundant?
          }
          else
          {
            return PAMI_EAGAIN;
          }
          // current message was completed...
        }
        if(_iwq) // root - keep send until all ranks have all bytes...
        {
          if(_idx >= _dst->size())
          {
            thr->_bytesLeft -= _currBytes;
            _iwq->consumeBytes(_currBytes);
            if(thr->_bytesLeft == 0)
            {
              thr->setStatus(PAMI::Device::Complete);
              setStatus(PAMI::Device::Done);
              return PAMI_SUCCESS;
            }
            _currBytes = 0;
          }
          if(_currBytes == 0)
          {
            _currBytes = _iwq->bytesAvailableToConsume();
            if(_currBytes > 1024) _currBytes = 1024; // ??
            _currBuf = _iwq->bufferToConsume();
            _idx = 0;
          }
          if(_currBytes == 0)
          {
            return PAMI_EAGAIN;
          }
          TRACE_DEVICE((stderr,"<%p>MPIBcastMsg::__advanceThread() sending, idx %zu, currBytes %zu, bytesLeft %zu, dst %zu, tag %d %s\n",this,
                        _idx, _currBytes, thr->_bytesLeft, _dst->index2Rank(_idx), _tag,_req == MPI_REQUEST_NULL?"MPI_REQUEST_NULL":""));
          int rc = 0;
          if(_dst->index2Rank(_idx) == __global.mapping.task()) // This src task is also a dst? do a local copy
          {
            memcpy(_rwq->bufferToProduce(), _currBuf, _currBytes);
            _rwq->produceBytes(_currBytes);
          }
          else
          {
            rc = MPI_Isend(_currBuf, _currBytes, MPI_BYTE,
                           _dst->index2Rank(_idx), _tag,
                           _g_mpibcast_dev._mcast_communicator, &_req);
          }
          TRACE_DEVICE((stderr,"<%p>MPIBcastMsg::__advanceThread() sending rc = %d, idx %zu, currBytes %zu, bytesLeft %zu, dst %zu, tag %d %s\n",this,
                        rc,_idx, _currBytes, thr->_bytesLeft, _dst->index2Rank(_idx), _tag,_req == MPI_REQUEST_NULL?"MPI_REQUEST_NULL":""));
          DO_DEBUG(count = 5);
          // error checking?
          ++_idx;
        }
        else // receive data until done...
        {
          PAMI_assert(_rwq);
          if(flag)
          {
            int scount;
            MPI_Get_count(&status, MPI_BYTE, &scount);
            if((size_t)scount < _currBytes)
            {
              // make sure we only count what was recv'ed
              _currBytes = scount;
            }
            else
            {
              // how does MPI_Status.count work?
              PAMI_assertf((size_t)scount <= _currBytes,
                          "MPIBcastMsg recv overrun (got %d, kept %zu)\n",
                          scount, _currBytes);
            }
          }

          // if we are here, we completed a chunk of recv data...
          thr->_bytesLeft -= _currBytes;
          _rwq->produceBytes(_currBytes);
          if(thr->_bytesLeft == 0)
          {
            thr->setStatus(PAMI::Device::Complete);
            setStatus(PAMI::Device::Done);
            return PAMI_SUCCESS;
          }
          _currBytes = _rwq->bytesAvailableToProduce();
          _currBuf = _rwq->bufferToProduce();
          TRACE_DEVICE((stderr,"<%p>MPIBcastMsg::__advanceThread() recving idx %zu, currBytes %zu, bytesLeft %zu, src %zu, tag %d %s\n",this,
                        _idx, _currBytes, thr->_bytesLeft, _root, _tag,_req == MPI_REQUEST_NULL?"MPI_REQUEST_NULL":""));
          if(_currBytes == 0)
          {
            return PAMI_EAGAIN;
          }
          DO_DEBUG(count = 5);
          int rc = MPI_Irecv(_currBuf, _currBytes, MPI_BYTE,
                             _root, _tag,
                             _g_mpibcast_dev._mcast_communicator, &_req);
          TRACE_DEVICE((stderr,"<%p>MPIBcastMsg::__advanceThread() recving rc = %d, idx %zu, currBytes %zu, bytesLeft %zu, src %zu, tag %d %s\n",this,
                        rc, _idx, _currBytes, thr->_bytesLeft, _root, _tag, _req == MPI_REQUEST_NULL?"MPI_REQUEST_NULL":""));
          // error checking?
        }

        return PAMI_EAGAIN;
      }

      unsigned _nThreads;
      PAMI::Topology *_dst;
      PAMI::PipeWorkQueue *_iwq;
      PAMI::PipeWorkQueue *_rwq;
      size_t _root;
      size_t _bytes;
      int _tag;
      size_t _idx;
      size_t _currBytes;
      char *_currBuf;
      MPI_Request _req;
      MessageStatus _pendingStatus;
    }; //-- MPIBcastMsg

    class MPIBcastMdl : public PAMI::Device::Interface::MulticastModel<MPIBcastMdl,MPIBcastDev,sizeof(MPIBcastMsg)>
    {
    public:
      static const int NUM_ROLES = 2;
      static const int REPL_ROLE = 1;
      static const size_t sizeof_msg = sizeof(MPIBcastMsg);

      MPIBcastMdl(MPIBcastDev &dev, pami_result_t &status) :
        PAMI::Device::Interface::MulticastModel<MPIBcastMdl,MPIBcastDev, sizeof(MPIBcastMsg)>(dev, status)
      {
        TRACE_DEVICE((stderr,"<%p>MPIBcastMdl()\n",this));
      }

      inline pami_result_t postMulticast_impl(uint8_t (&state)[sizeof_msg],
						size_t clientid, size_t contextid,
                                              pami_multicast_t *mcast,
                                              void             *devinfo);

    private:
    }; // class MPIBcastMdl

    inline pami_result_t MPIBcastMdl::postMulticast_impl(uint8_t (&state)[sizeof_msg],
							size_t clientid, size_t contextid,
                                                         pami_multicast_t *mcast,
                                                         void *devinfo)
    {
      TRACE_DEVICE((stderr,"<%p>MPIBcastMdl::postMulticast() dispatch %zu, connection_id %d, msgcount %d, bytes %zu, request %p\n",this,
                    mcast->dispatch, mcast->connection_id, mcast->msgcount, mcast->bytes, &state));
      MPIBcastMsg *msg =
      new (&state) MPIBcastMsg(_g_mpibcast_dev.getQS(), mcast);
      _g_mpibcast_dev.__post<MPIBcastMsg>(msg);
      return PAMI_SUCCESS;
    }

  }; //-- Device
}; //-- PAMI
#undef TRACE_DEVICE
#endif // __components_devices_workqueue_MPIBcastMsg_h__
