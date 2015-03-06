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
 * \file components/devices/mpi/mpidevice.h
 * \brief ???
 */

#ifndef __components_devices_mpi_mpidevice_h__
#define __components_devices_mpi_mpidevice_h__

#include "components/devices/BaseDevice.h"
#include "components/devices/PacketInterface.h"
#include "components/devices/mpi/mpimessage.h"
#include "components/devices/generic/Device.h"
#include "components/devices/FactoryInterface.h"
#include <map>
#include <list>
#include "util/ccmi_debug.h"

#include "components/devices/MulticastModel.h"
#include "components/devices/MulticombineModel.h"
#include "components/devices/MultisyncModel.h"
#include "components/devices/ManytomanyModel.h"

#undef TRACE_DEVICE
  #define TRACE_DEVICE(x) //fprintf x

namespace PAMI
{
  namespace Device
  {
#define DISPATCH_SET_SIZE 256
    typedef struct mpi_dispatch_info_t
    {
      PAMI::Device::Interface::RecvFunction_t  recv_func;
      void                      *recv_func_parm;
    }mpi_dispatch_info_t;

    typedef struct mpi_mcast_dispatch_info_t
    {
      pami_dispatch_multicast_function recv_func;
      void                         *async_arg;
    }mpi_mcast_dispatch_info_t;

    typedef struct mpi_m2m_dispatch_info_t
    {
      pami_dispatch_manytomany_function  recv_func;
      void                          *async_arg;
    }mpi_m2m_dispatch_info_t;

    class MPIDevice : public Interface::BaseDevice<MPIDevice>,
                      public Interface::PacketDevice<MPIDevice>
    {
    public:
      static const size_t packet_payload_size = 224;
      inline MPIDevice (size_t ncontext = 1) :
      Interface::BaseDevice<MPIDevice> (),
      Interface::PacketDevice<MPIDevice>(),
      _dispatch_id(0),
      _curMcastTag(MULTISYNC_TAG),
      _ncontexts (ncontext)
      {
        // \todo These MPI functions should probably move out of the constructor if we keep this class
        MPI_Comm_dup(MPI_COMM_WORLD,&_communicator);
        MPI_Comm_size(_communicator, (int*)&_peers);
        TRACE_DEVICE((stderr,"<%p>MPIDevice()\n",this));
        
        // Initialize the deterministic packet connection array.
        unsigned i, num_endpoints = ncontext * _peers;
        pami_result_t mmrc;
	      mmrc = __global.heap_mm->memalign((void **) & _connection, 16, sizeof(void *) * num_endpoints);
        PAMI_assertf(mmrc == PAMI_SUCCESS, "memalign failed for packet connection array, rc=%d\n", mmrc);

        for (i = 0; i < num_endpoints; i++) _connection[i] = NULL;
        
      };

        class Factory : public Interface::FactoryInterface<Factory,MPIDevice,Generic::Device> {
        public:

static inline MPIDevice *generate_impl(size_t clientid, size_t num_ctx, Memory::MemoryManager & mm, PAMI::Device::Generic::Device *devices) {
        size_t x;
        MPIDevice *devs;
        pami_result_t rc;
        rc = __global.heap_mm->memalign((void **)&devs, 16, sizeof(*devs) * num_ctx);
        PAMI_assertf(rc == PAMI_SUCCESS, "alloc failed for MPIDevice[%zu], errno=%d\n", num_ctx, errno);
        for (x = 0; x < num_ctx; ++x) {
                new (&devs[x]) MPIDevice(num_ctx);
        }
        return devs;
}

static inline pami_result_t init_impl(MPIDevice *devs, size_t client, size_t contextId, pami_client_t clt, pami_context_t ctx, PAMI::Memory::MemoryManager *mm, PAMI::Device::Generic::Device *devices) {
        return getDevice_impl(devs, client, contextId).init_impl(mm, client, 0, ctx, contextId);
}

static inline size_t advance_impl(MPIDevice *devs, size_t client, size_t contextId) {
        return getDevice_impl(devs, client, contextId).advance_impl();
}

static inline MPIDevice & getDevice_impl(MPIDevice *devs, size_t client, size_t contextId) {
        return devs[contextId];
}
        }; // class Factory


      // Implement BaseDevice Routines

      inline ~MPIDevice () {};


      int registerRecvFunction (size_t                     dispatch,
                                PAMI::Device::Interface::RecvFunction_t  recv_func,
                                void                      *recv_func_parm)
        {
          unsigned i;
          TRACE_DEVICE((stderr,"<%p>MPIDevice::registerRecvFunction dispatch %zu/%zu\n",this,dispatch,dispatch * DISPATCH_SET_SIZE));
          for (i=0; i<DISPATCH_SET_SIZE; i++)
          {
            unsigned id = dispatch * DISPATCH_SET_SIZE + i;
            if (_dispatch_table[id].recv_func == NULL)
            {
              _dispatch_table[id].recv_func=recv_func;
              _dispatch_table[id].recv_func_parm=recv_func_parm;
              _dispatch_lookup[id] = _dispatch_table[id];

              return id;
            }
          }
          return -1;
        }

      int initMcast()
      {
        return _mcast_dispatch_id++;
      }

      int initM2M()
      {
        return _m2m_dispatch_id++;
      }

      void registerMcastRecvFunction (int                           dispatch_id,
                                      pami_dispatch_multicast_function recv_func,
                                      void                         *async_arg)
      {
        _mcast_dispatch_table[dispatch_id].recv_func=recv_func;
        _mcast_dispatch_table[dispatch_id].async_arg=async_arg;
        _mcast_dispatch_lookup[dispatch_id]=_mcast_dispatch_table[dispatch_id];
        TRACE_DEVICE((stderr,"<%p>MPIDevice::registerMcastRecvFunction %d\n",this,_dispatch_id));
      }

      void registerM2MRecvFunction (int                           dispatch_id,
                                    pami_dispatch_manytomany_function  recv_func,
                                    void                         *async_arg)
      {
        _m2m_dispatch_table[dispatch_id].recv_func=recv_func;
        _m2m_dispatch_table[dispatch_id].async_arg=async_arg;
        _m2m_dispatch_lookup[dispatch_id]=_m2m_dispatch_table[dispatch_id];
        TRACE_DEVICE((stderr,"<%p>MPIDevice::registerM2MRecvFunction %d\n",this,_dispatch_id));
      }

      inline pami_result_t init_impl (Memory::MemoryManager *mm, size_t clientid, size_t num_ctx, pami_context_t context, size_t offset)
      {
        _context = context;
        _contextid = offset;

        return PAMI_SUCCESS;
      };

      inline pami_context_t getContext_impl ()
      {
        return _context;
      };

      inline size_t getContextOffset_impl ()
      {
        return _contextid;
      };

      inline size_t getContextCount_impl ()
      {
        PAMI_assertf(_ncontexts > 0, "invalid number of contexts .. %zu\n", _ncontexts);
        return _ncontexts;
      };

      inline bool isInit_impl ()
      {
        PAMI_abort();
        return false;
      };
      inline bool isReliableNetwork ()
      {
        return true;
      };

      inline int advance_impl ()
      {
        static int dbg = 1;
        int flag = 0;
        MPI_Status sts;
        int events=0;
	pami_result_t res;

        if(dbg) {
          TRACE_DEVICE((stderr,"<%p>MPIDevice::advance_impl\n",this));
          dbg = 0;
        }
#ifdef EMULATE_NONDETERMINISTIC_DEVICE
        // Check the P2P *pending* send queue
        while (!_pendingQ.empty())
        {
          MPIMessage * msg = _pendingQ.front();
          _pendingQ.pop_front();
          MPI_Isend (&msg->_p2p_msg,
                     sizeof(msg->_p2p_msg),
                     MPI_CHAR,
                     msg->_target_task,
                     0,
                     _communicator,
                     &msg->_request);
          enqueue(msg);
        }
#endif
        // Check the P2P send queue
        std::list<MPIMessage*>::iterator it_p2p;
        for(it_p2p=_sendQ.begin();it_p2p != _sendQ.end(); it_p2p++)
        {
          flag            = 0;
          MPI_Testall(1,&((*it_p2p)->_request),&flag,MPI_STATUSES_IGNORE);
          if(flag)
          {
            events++;
            TRACE_DEVICE((stderr,"<%p>MPIDevice::advance_impl() p2p\n",this)); dbg = 1;
            pami_event_function  done_fn = (*it_p2p)->_done_fn;
            void               *cookie  = (*it_p2p)->_cookie;
            pami_client_t       client = (*it_p2p)->_client;
            size_t       context = (*it_p2p)->_context;
            _sendQ.remove((*it_p2p));
            if((*it_p2p)->_freeme)
              __global.heap_mm->free(*it_p2p);

            if(done_fn)
              done_fn(NULL,//PAMI_Client_getcontext(client,context),  \todo fix this
                      cookie,PAMI_SUCCESS);
            break;
          }
        }
        // Check the Multicast send queue
        // Check the M2M send Queue
        std::list<MPIM2MMessage*>::iterator it;
        for(it=_m2msendQ.begin();it != _m2msendQ.end(); it++)
        {
          int numStatuses = (*it)->_num;
          flag            = 0;
          MPI_Testall(numStatuses,(*it)->_reqs,&flag,MPI_STATUSES_IGNORE);
          if(flag)
          {
            events++;
            TRACE_DEVICE((stderr,"<%p>MPIDevice::advance_impl m2m\n",this)); dbg = 1;
            if((*it)->_done_fn )
              ((*it)->_done_fn)(NULL, (*it)->_cookie, PAMI_SUCCESS);

            __global.heap_mm->free ((*it)->_reqs);
            __global.heap_mm->free ((*it)->_bufs);
            _m2msendQ.remove((*it));
            __global.heap_mm->free (*it);
            break;
          }
        }

        // Check the msync send Queue
        std::map<int, MPIMsyncMessage*>::iterator it_msync;
        for(it_msync=_msyncsendQ.begin();it_msync != _msyncsendQ.end(); it_msync++)
        {


          MPIMsyncMessage *m = it_msync->second;
          if(m->_sendStarted==false)
              {
                int mpi_rc = MPI_Isend (&m->_p2p_msg,
                                        sizeof(m->_p2p_msg),
                                        MPI_CHAR,
                                        m->_dests[m->_phase],
                                        MULTISYNC_TAG,
                                        _communicator,
                                        &m->_reqs[m->_phase]);
                PAMI_assert(mpi_rc == MPI_SUCCESS);
                m->_sendStarted = true;
              }
          if(m->_sendDone == false)
              {
                MPI_Test(&m->_reqs[m->_phase],&flag,MPI_STATUS_IGNORE);
                if(flag)
                    {
                      m->_sendDone=true;
                    }
              }
          if(m->_sendDone == true && m->_recvDone == true)
              {
                m->_sendStarted = false;
                m->_sendDone    = false;
                m->_recvDone    = false;
                m->_phase++;
                if(m->_phase == m->_numphases)
                    {
                      m->_cb_done.function(NULL,m->_cb_done.clientdata, PAMI_SUCCESS);
                      _msyncsendQ.erase(it_msync);
                      break;
                    }
              }
        }





        flag = 0;
        int rc = MPI_Iprobe (MPI_ANY_SOURCE, MPI_ANY_TAG, _communicator, &flag, &sts);
        PAMI_assert(rc == MPI_SUCCESS);
        if(flag)
        {
          events++;
          TRACE_DEVICE((stderr,"<%p>MPIDevice::advance_impl MPI_Iprobe %d\n",this,sts.MPI_TAG)); dbg = 1;
          //p2p messages
          switch(sts.MPI_TAG)
          {
          case P2P_PACKET_TAG: // p2p packet
            {
              int nbytes = 0;
              MPI_Get_count(&sts, MPI_BYTE, &nbytes);
              MPIMessage *msg;
	      res = __global.heap_mm->memalign((void **)&msg, 0, sizeof(*msg));
	      PAMI_assertf(res == PAMI_SUCCESS, "alloc of msg failed");
              int rc = MPI_Recv(&msg->_p2p_msg,nbytes,MPI_BYTE,sts.
                                MPI_SOURCE,sts.MPI_TAG,
                                _communicator,&sts);
              PAMI_assert(rc == MPI_SUCCESS);
              size_t dispatch_id      = msg->_p2p_msg._dispatch_id;
              TRACE_DEVICE((stderr,"<%p>MPIDevice::advance_impl MPI_Recv nbytes %d, dispatch_id %zu\n",
                             this, nbytes,dispatch_id));
              _currentBuf = msg->_p2p_msg._payload;
              mpi_dispatch_info_t mdi = _dispatch_lookup[dispatch_id];
              if(mdi.recv_func)
                mdi.recv_func(msg->_p2p_msg._metadata,
                              msg->_p2p_msg._payload,
                              msg->_p2p_msg._payloadsize0+msg->_p2p_msg._payloadsize1,
                              mdi.recv_func_parm,
                              NULL);
              __global.heap_mm->free(msg);
            }
            break;
          case P2P_PACKET_DATA_TAG: // p2p packet + data
            {
              int nbytes = 0;
              MPI_Get_count(&sts, MPI_BYTE, &nbytes);
              MPIMessage *msg;
	      res = __global.heap_mm->memalign((void **)&msg, 0, sizeof(*msg) + nbytes);
	      PAMI_assertf(res == PAMI_SUCCESS, "alloc of msg failed");
              int rc = MPI_Recv(&msg->_p2p_msg,nbytes,MPI_BYTE,sts.
                                MPI_SOURCE,sts.MPI_TAG,
                                _communicator,&sts);
              PAMI_assert(rc == MPI_SUCCESS);
              size_t dispatch_id      = msg->_p2p_msg._dispatch_id;
              TRACE_DEVICE((stderr,"<%p>MPIDevice::advance_impl MPI_Recv nbytes %d, dispatch_id %zu\n",
                             this, nbytes,dispatch_id));
              _currentBuf = (char*)msg->_p2p_msg._metadata+msg->_p2p_msg._metadatasize;
              mpi_dispatch_info_t mdi = _dispatch_lookup[dispatch_id];
              if(mdi.recv_func)
                mdi.recv_func(msg->_p2p_msg._metadata,
                              (char*)msg->_p2p_msg._metadata+msg->_p2p_msg._metadatasize,
                              msg->_p2p_msg._payloadsize0+msg->_p2p_msg._payloadsize1,
                              mdi.recv_func_parm,
                              NULL);
              __global.heap_mm->free(msg);
            }
            break;
         case MULTISYNC_TAG:
             {
               unsigned conn_id;
               int      nbytes;
               MPI_Get_count(&sts, MPI_BYTE, &nbytes);
               int rc            = MPI_Recv(&conn_id,
                                            sizeof(conn_id),
                                            MPI_BYTE,
                                            sts.MPI_SOURCE,
                                            sts.MPI_TAG,
                                            _communicator,
                                            &sts);
               PAMI_assert (rc == MPI_SUCCESS);
               MPIMsyncMessage *m = _msyncsendQ[conn_id];
               if (m)
                 m->_recvDone = true;
               else
                 _msyncsendQ[conn_id] = (MPIMsyncMessage*)0x1;
             }
             break;
         default:
           PAMI_abort();
          }
        }
        return events;
      };

      // Implement MessageDevice Routines
      static const size_t metadata_size = 128;
      static const size_t payload_size  = 224;

      // Implement Packet Device Routines
      inline int    read_impl(void * dst, size_t bytes, void * cookie)
      {
        memcpy(dst, _currentBuf, bytes);
        return -1;
      }

          /// \see PAMI::Device::Interface::PacketDevice::Deterministic::clearConnection()
          inline void clearConnection_impl (size_t task, size_t offset)
          {
            size_t index = _peers + offset * _ncontexts;
      
            PAMI_assert_debugf(_connection[index] != NULL, "Error. _connection[%zu] was not previously set.\n", index);

            _connection[index] = NULL;
          }
              
          /// \see PAMI::Device::Interface::PacketDevice::Deterministic::getConnection()
          inline void * getConnection_impl (size_t task, size_t offset)
          {
            size_t index = _peers + offset * _ncontexts;
      
            PAMI_assert_debugf(_connection[index] != NULL, "Error. _connection[%zu] was not previously set.\n", index);

            return _connection[index];
          }

          /// \see PAMI::Device::Interface::PacketDevice::Deterministic::setConnection()
          inline void setConnection_impl (void * value, size_t task, size_t offset)
          {
            size_t index = _peers + offset * _ncontexts;
      
            PAMI_assert_debugf(_connection[index] != NULL, "Error. _connection[%zu] was previously set.\n", index);

            _connection[index] = value;
          }



      inline size_t peers_impl ()
      {
        return _peers;
      }

      inline size_t task2peer_impl (size_t task)
      {
        PAMI_assert(task < _peers);
        return task;
      }

      inline bool isPeer_impl (size_t task)
      {
#if 0
        PAMI::Interface::Mapping::nodeaddr_t node;
        size_t peer;

        __global.mapping.task2node(task,node);
        pami_result_t result = __global.mapping.node2peer(node,peer);

        return result == PAMI_SUCCESS;
#else
        return true;
#endif
      }


      inline void enqueue(MPIMessage* msg)
      {
        TRACE_DEVICE((stderr,
                      "<%p>MPIDevice::enqueue message size 0 %zu, size 1 %zu, msize %zu\n",
                      this,
                      (size_t)msg->_p2p_msg._payloadsize0,
                      (size_t)msg->_p2p_msg._payloadsize1,
                      (size_t)msg->_p2p_msg._metadatasize));
        _sendQ.push_front(msg);
      }

      inline void enqueue(MPIMcastMessage* msg)
      {
        TRACE_DEVICE((stderr,"<%p>MPIDevice::enqueue mcast message size %zu\n",this, (size_t)msg->_bytes));
        _mcastsendQ.push_front(msg);
      }

      inline void enqueue(MPIMcastRecvMessage *msg)
      {
        TRACE_DEVICE((stderr,
                      "<%p>MPIDevice::enqueue mcast recv message \n",
                      this));
  //,
  //(size_t)msg->_pwidth,
  //(size_t)msg->_bytes));
        _mcastrecvQ.push_front(msg);
      }




      inline void enqueue(MPIM2MRecvMessage<size_t> *msg)
      {
        TRACE_DEVICE((stderr,
                      "<%p>MPIDevice::enqueue m2m recv message size %zu\n",
                      this,
                      (size_t)msg->_sizes[0]));
        _m2mrecvQ.push_front(msg);
      }

      inline void enqueue(MPIM2MMessage *msg)
      {
        TRACE_DEVICE((stderr,
                      "<%p>MPIDevice::enqueue m2m message total size %zu\n",
                      this,
                      (size_t)msg->_totalsize));
        _m2msendQ.push_front(msg);
      }

      inline void enqueue(MPIMsyncMessage *msg)
      {
        if(_msyncsendQ[msg->_p2p_msg._connection_id] == (MPIMsyncMessage*)0x1)
          msg->_recvDone = true;
        _msyncsendQ[msg->_p2p_msg._connection_id] = msg;
      }

      inline void addToNonDeterministicQueue(MPIMessage* msg, unsigned long long random)
      {
        size_t index, insert = random % _pendingQ.size();
        std::list<MPIMessage*>::iterator it;
        for (index = 0; index < insert; index++) it++;
        _pendingQ.insert(it,msg);
      }

      MPI_Comm                                  _communicator;
      char                                     *_currentBuf;
      size_t                                    _peers;
      size_t                                    _dispatch_id;
      size_t                                    _mcast_dispatch_id;
      size_t                                    _m2m_dispatch_id;
      std::map<int, mpi_dispatch_info_t>        _dispatch_lookup;
      std::map<int, mpi_mcast_dispatch_info_t>  _mcast_dispatch_lookup;
      std::map<int, mpi_m2m_dispatch_info_t>    _m2m_dispatch_lookup;
      std::list<MPIMessage*>                    _sendQ;
      std::list<MPIMcastMessage*>               _mcastsendQ;
      std::list<MPIM2MMessage*>                 _m2msendQ;
      std::list<MPIMcastRecvMessage*>           _mcastrecvQ;
      std::list<MPIM2MRecvMessage<size_t> *>    _m2mrecvQ;
      std::list<MPIMessage*>                    _pendingQ;
      std::map<int,MPIMsyncMessage*>            _msyncsendQ;
      mpi_dispatch_info_t                       _dispatch_table[256*DISPATCH_SET_SIZE];
      mpi_mcast_dispatch_info_t                 _mcast_dispatch_table[256];
      mpi_m2m_dispatch_info_t                   _m2m_dispatch_table[256];
      int                                       _curMcastTag;
      pami_context_t                             _context;
      size_t                                    _contextid;
      size_t                                    _ncontexts;
      
      // -------------------------------------------------------------
      // Deterministic packet interface connection array
      // -------------------------------------------------------------
        
      void ** _connection;
    };
#undef DISPATCH_SET_SIZE
  };
};
#endif // __components_devices_mpi_mpipacketdevice_h__
