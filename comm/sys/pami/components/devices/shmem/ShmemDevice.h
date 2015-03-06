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
 * \file components/devices/shmem/ShmemDevice.h
 * \brief ???
 */

#ifndef __components_devices_shmem_ShmemDevice_h__
#define __components_devices_shmem_ShmemDevice_h__

#include <sys/uio.h>

#include <pami.h>

#include "components/memory/MemoryManager.h"
#include "Arch.h"
#include "Memory.h"
#include "Memregion.h"

#include "components/atomic/native/NativeMutex.h"
#include "components/atomic/CounterInterface.h"
#include "components/devices/BaseDevice.h"
#include "components/devices/FactoryInterface.h"
#include "components/devices/PacketInterface.h"
#include "components/devices/generic/Device.h"
#include "components/devices/shmem/ShmemDispatch.h"
#include "components/devices/shmem/ShmemMessage.h"
#include "components/devices/shmem/ShmemPacket.h"
#include "components/devices/shmem/ShmemWork.h"
#include "components/devices/shmem/shaddr/SystemShaddr.h"
#include "components/devices/shmem/shaddr/NoShaddr.h"
#include "components/devices/shmem/wakeup/WakeupNoop.h"
#include "components/memory/MemoryAllocator.h"
#include "util/queue/Queue.h"
#include "util/queue/CircularQueue.h"

#include "components/devices/shmem/ShmemCollDesc.h"
#include "components/devices/shmem/msgs/BaseMessage.h"

#include "common/default/PipeWorkQueue.h"

#include "components/devices/ShmemCollInterface.h"
#include "components/devices/MulticastModel.h"
#include "components/devices/MultisyncModel.h"
#include "components/devices/MulticombineModel.h"

#include "components/atomic/indirect/IndirectCounter.h"

//#define TRAP_ADVANCE_DEADLOCK
#define ADVANCE_DEADLOCK_MAX_LOOP 10000

//#define EMULATE_NONDETERMINISTIC_SHMEM_DEVICE
#define EMULATE_NONDETERMINISTIC_SHMEM_DEVICE_FREQUENCY 4

//#define EMULATE_UNRELIABLE_SHMEM_DEVICE
#define EMULATE_UNRELIABLE_SHMEM_DEVICE_FREQUENCY 10

#undef TRACE_ERR
#define TRACE_ERR(x) //fprintf x

#define MATCH_DISPATCH_SIZE	256

namespace PAMI
{
  namespace Device
  {

    typedef struct match_dispatch_t
    {
      Interface::MatchFunction_t   function;
      void                      * clientdata;
    } match_dispatch_t;

    template < class T_Fifo,
    class T_Atomic = Counter::Indirect<Counter::Native>,
    class T_Shaddr = Shmem::NoShaddr,
    unsigned T_FifoCount = 64,
    unsigned T_SetCount = 512 >
    class ShmemDevice : public Interface::BaseDevice< ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount> >,
        public Interface::PacketDevice<ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount> >
    {
      public:

//#ifdef __pami_target_bgq__   // Todo:  find platform agnostic way to enable BGQ::IndirectL2
//        typedef Shmem::CollectiveFifo<Counter::Indirect<Counter::Native> >  CollectiveFifo;
//#else
//        typedef Shmem::CollectiveFifo<PAMI::Counter::BGQ::IndirectL2>  CollectiveFifo;
//#endif

        typedef Shmem::CollectiveFifo<T_Atomic> CollectiveFifo;

        typedef Shmem::BaseMessage<ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount> > BaseMessage;


        // Inner factory class
        class Factory : public Interface::FactoryInterface<Factory, ShmemDevice, PAMI::Device::Generic::Device>
        {
          public:

            static inline size_t getInstanceMemoryRequired_impl (size_t clientid,
                                                                 Memory::MemoryManager & mm)
            {
              size_t npeers = 0;
              __global.mapping.nodePeers (npeers);
              return T_Fifo::fifo_memory_size * npeers + CollectiveFifo::fifo_memory_size;
            };

            static inline size_t getInstanceMaximum_impl (size_t clientid, Memory::MemoryManager & mm)
            {
              size_t npeers = 0;
              __global.mapping.nodePeers (npeers);
              size_t instance_maximum = T_FifoCount / npeers;

              return instance_maximum;
            };

            static inline ShmemDevice * generate_impl (size_t clientid,
                                                       size_t ncontexts,
                                                       Memory::MemoryManager & mm,
                                                       PAMI::Device::Generic::Device * progress)
            {
              TRACE_ERR((stderr, ">> ShmemDevice::Factory::generate_impl() ncontexts = %zu\n", ncontexts));

              // Allocate an array of shared memory devices, one for each
              // context in this task (from heap, not from shared memory)
              ShmemDevice * devices;
              pami_result_t mmrc;
              mmrc = __global.heap_mm->memalign((void **) & devices, 16, sizeof(*devices) * ncontexts);
              PAMI_assertf(mmrc == PAMI_SUCCESS, "memalign failed for shared memory devices, rc=%d\n", mmrc);

              // Instantiate the shared memory devices
              unsigned i;

              for (i = 0; i < ncontexts; i++)
                {
                  new (&devices[i]) ShmemDevice (clientid, ncontexts, mm, i, progress);
                }

              TRACE_ERR((stderr, "<< ShmemDevice::Factory::generate_impl()\n"));
              return devices;
            };

            static inline pami_result_t init_impl (ShmemDevice     * devices,
                                                   size_t            clientid,
                                                   size_t            contextid,
                                                   pami_client_t     client,
                                                   pami_context_t    context,
                                                   Memory::MemoryManager *mm,
                                                   PAMI::Device::Generic::Device * progress)
            {
              return getDevice_impl(devices, clientid, contextid).init (client, context);
            };

            static inline size_t advance_impl (ShmemDevice * devices,
                                               size_t        clientid,
                                               size_t        contextid)
            {
              return devices[contextid].advance ();
            };

            static inline ShmemDevice & getDevice_impl (ShmemDevice * devices,
                                                        size_t        clientid,
                                                        size_t        contextid)
            {
              return devices[contextid];
            };
        };

        inline ShmemDevice (size_t clientid, size_t ncontexts,
                            Memory::MemoryManager & mm, size_t contextid,
                            PAMI::Device::Generic::Device * progress) :
            Interface::BaseDevice< ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount> > (),
            Interface::PacketDevice< ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount> > (),
            _clientid (clientid),
            _contextid (contextid),
            _ncontexts (ncontexts),
            _nfifos (0),
#ifdef EMULATE_NONDETERMINISTIC_SHMEM_DEVICE
            __ndQ (),
            __ndpkt (),
#endif
            _progress (progress),
            _local_progress_device (&(Generic::Device::Factory::getDevice (progress, 0, contextid))),
            _dispatch (),
#ifdef SHM_USE_COLLECTIVE_FIFO
            shaddr (this),
            _desc_fifo ()
#else
            shaddr (this)
#endif
        {
          TRACE_ERR((stderr, "ShmemDevice() constructor\n"));

#ifdef SHM_USE_COLLECTIVE_FIFO
          // Create a unique string, useful for memory manager alloaction, etc.
          snprintf(_unique_str, 15, "%2.2zu-%2.2zu", _clientid, _contextid);
#endif
          // Get the peer information for this task
          _peer = 0;
          PAMI::Interface::Mapping::nodeaddr_t address;
          __global.mapping.nodeAddr (address);
          __global.mapping.node2peer (address, _peer);
          __global.mapping.nodePeers (_npeers);
          _task = __global.mapping.task();

          // Initialize all fifos on the node
          size_t i, j;
          _nfifos = _npeers * _ncontexts;
          char fifokey[PAMI::Memory::MMKEYSIZE];

          for (i = 0; i < _npeers; i++)
            {
              for (j = 0; j < _ncontexts; j++)
                {
                  size_t fnum = (i * _ncontexts) + j;
                  snprintf (fifokey, PAMI::Memory::MMKEYSIZE - 1, "/device-shmem-client-%zu-fifo-%zu", _clientid, fnum);
                  bool result = false;
                  result = _fifo[fnum].initialize (&mm, fifokey, _npeers, i);
                  PAMI_assert (result == true);
                }
            }

          // Initialize the reception fifo assigned to this context
          _rfifo.initialize (_fifo[fnum(_peer, _contextid)]);

          // Initialize the local send queue for each fifo on the node
          TRACE_ERR((stderr, "ShmemDevice() constructor: _nfifos = %zu, _npeers = %zu, _ncontexts = %zu\n", _nfifos, _npeers, _ncontexts));

          for (i = 0; i < _nfifos; i++)
            {
              PAMI_assert (_local_progress_device != NULL);
              _sendQ[i].init(_local_progress_device);
            }

          // Initialize the collective descriptor fifo
#ifdef SHM_USE_COLLECTIVE_FIFO
          _desc_fifo.init (mm, _unique_str);
#endif

          // Initialize the deterministic packet connection array.
          for (i = 0; i < _nfifos; i++) _connection[i] = NULL;
        };

        inline ~ShmemDevice () {};

        inline size_t getLocalRank ();

        // ------------------------------------------

        /// \see PAMI::Device::Interface::BaseDevice::isInit()
        bool isInit_impl ();

        /// \see PAMI::Device::Interface::BaseDevice::peers()
        inline size_t peers_impl ();

        /// \see PAMI::Device::Interface::BaseDevice::task2peer()
        inline size_t task2peer_impl (size_t task);

        /// \see PAMI::Device::Interface::BaseDevice::isPeer()
        inline bool isPeer_impl (size_t task);

        inline pami_context_t getContext_impl ();

        inline size_t getContextId_impl ();
        inline size_t getContextOffset_impl ();
        inline size_t getContextCount_impl ();

        // ------------------------------------------

        /// \see PAMI::Device::Interface::PacketDevice::read()
        inline int read_impl (void * buf, size_t length, void * cookie);

        /// \see PAMI::Device::Interface::PacketDevice::Deterministic::clearConnection()
        inline void clearConnection_impl (size_t task, size_t offset);

        /// \see PAMI::Device::Interface::PacketDevice::Deterministic::getConnection()
        inline void * getConnection_impl (size_t task, size_t offset);

        /// \see PAMI::Device::Interface::PacketDevice::Deterministic::setConnection()
        inline void setConnection_impl (void * value, size_t task, size_t offset);

        static const bool reliable      = true;
        static const bool deterministic = true;

        static const size_t metadata_size = T_Fifo::packet_header_size - sizeof(uint16_t);
        static const size_t payload_size  = T_Fifo::packet_payload_size;

        // ------------------------------------------

        static const bool shaddr_va_supported    = T_Shaddr::shaddr_va_supported;
        static const bool shaddr_mr_supported    = T_Shaddr::shaddr_mr_supported;

        static const bool shaddr_read_supported  = T_Shaddr::shaddr_read_supported;
        static const bool shaddr_write_supported = T_Shaddr::shaddr_write_supported;

        // ------------------------------------------

        static const size_t completion_work_size = sizeof(Shmem::RecPacketWork<T_Fifo>);

        ///
        /// \brief Register the receive function to dispatch when a packet arrives.
        ///
        /// \param [in]  set            Dispatch set identifier
        /// \param [in]  recv_func      Receive function to dispatch
        /// \param [in]  recv_func_parm Receive function client data
        /// \param [out] id             Dispatch id for this registration
        ///
        /// \return registration result
        ///
        pami_result_t registerRecvFunction (size_t                      set,
                                            Interface::RecvFunction_t   recv_func,
                                            void                      * recv_func_parm,
                                            uint16_t                  & id);

        ///
        /// \brief Register the system receive function to dispatch when a packet arrives.
        ///
        /// System dispatch ids are allocated by the device.
        ///
        /// \param [in]  recv_func      Receive function to dispatch
        /// \param [in]  recv_func_parm Receive function client data
        /// \param [out] id             Dispatch id for this registration
        ///
        /// \return registration result
        ///
        pami_result_t registerSystemRecvFunction (Interface::RecvFunction_t   recv_func,
                                                  void                      * recv_func_parm,
                                                  uint16_t                  & id);

        void dispatch (uint16_t id, void * metadata, void * payload, size_t bytes);

        pami_result_t post (size_t ififo, Shmem::SendQueue::Message * msg);

        template <unsigned T_StateBytes>
        pami_result_t postCompletion (uint8_t               (&state)[T_StateBytes],
                                      pami_event_function   local_fn,
                                      void                * cookie,
                                      size_t                fnum,
                                      size_t                sequence);

        ///
        /// \brief Check if the send queue to an injection fifo is empty
        ///
        ///
        inline bool isSendQueueEmpty (size_t fnum);

        inline Shmem::SendQueue *getQS (size_t fnum);

        inline pami_result_t init (pami_client_t client, pami_context_t context);

        inline size_t advance ();

        inline size_t fnum (size_t peer, size_t offset);

        inline bool activePackets (size_t fnum);

        inline char * getUniqueString ();

        inline PAMI::Device::Generic::Device * getLocalProgressDevice ();

        inline PAMI::Device::Generic::Device * getProgressDevice (size_t id);

        pami_result_t getShmemWorldDesc(typename CollectiveFifo::Descriptor **my_desc);
        pami_result_t registerMatchDispatch (Interface::MatchFunction_t   match_func, void * recv_func_parm, uint16_t &id);

        T_Fifo _fifo[T_FifoCount];  //< Injection fifo array for all node contexts
        T_Fifo  _rfifo;             //< Fifo to use as the reception fifo
        size_t * _last_inj_sequence_id;

        pami_client_t       _client;
        pami_context_t      _context;
        size_t             _clientid;
        size_t             _contextid;
        size_t             _ncontexts;
        size_t             _nfifos;

#ifdef EMULATE_NONDETERMINISTIC_SHMEM_DEVICE
        Queue                                            __ndQ;
        MemoryAllocator < sizeof(UnexpectedPacket), 16 > __ndpkt;
#endif

        Shmem::SendQueue    _sendQ[T_FifoCount];
        PAMI::Device::Generic::Device * _progress;
        PAMI::Device::Generic::Device * _local_progress_device;

        size_t            _npeers;
        size_t            _task;
        size_t            _peer;

        Shmem::Dispatch<Shmem::Packet<typename T_Fifo::Packet>, T_SetCount, 8 >  _dispatch;
        Shmem::Shaddr::System<T_Shaddr>                           shaddr;

        // -------------------------------------------------------------
        // Collectives
        // -------------------------------------------------------------
#ifdef SHM_USE_COLLECTIVE_FIFO
        CollectiveFifo  _desc_fifo;
        PAMI::Queue     _collectiveQ;
        inline void post_obj(BaseMessage *obj)
        {_collectiveQ.enqueue(obj);}
        PAMI::Queue::Iterator _Iter;
        match_dispatch_t  _match_dispatch[MATCH_DISPATCH_SIZE];
        char _unique_str[16];
#endif
        // -------------------------------------------------------------
        // Deterministic packet interface connection array
        // -------------------------------------------------------------

        void * _connection[T_FifoCount];
    };

    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    inline size_t ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::getLocalRank()
    {
      return _peer;
    }

    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    inline pami_context_t ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::getContext_impl()
    {
      return _context;
    }

    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    inline size_t ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::getContextId_impl()
    {
      return _contextid;
    }
    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    inline size_t ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::getContextOffset_impl()
    {
      return getContextId_impl();
    }

    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    inline size_t ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::getContextCount_impl()
    {
      return _ncontexts;
    }

    ///
    /// \brief Check if the send queue to a local rank is empty
    ///
    /// \see fnum
    /// \param[in] fnum  Local injection fifo number
    ///
    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    inline bool ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::isSendQueueEmpty (size_t fnum)
    {
      return (_sendQ[fnum].size() == 0);
    }

    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    inline Shmem::SendQueue * ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::getQS (size_t fnum)
    {
      return &_sendQ[fnum];
    }

    /// \see PAMI::Device::Interface::PacketDevice::read()
    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    int ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::read_impl (void * dst, size_t length, void * cookie)
    {
      memcpy (dst, cookie, length);
      return 0;
    }

    /// \see PAMI::Device::Interface::PacketDevice::Deterministic::clearConnection()
    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    void ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::clearConnection_impl (size_t task, size_t offset)
    {
      size_t index = task2peer_impl (task) + offset * _npeers;
      TRACE_ERR((stderr, "ShmemDevice::clearConnection_impl(%zu, %zu), _connection[%zu] = %p -> %p\n", task, offset, index, _connection[index], (void *) NULL));

      PAMI_assert_debugf(_connection[index] != NULL, "Error. _connection[%zu] was not previously set.\n", index);

      _connection[index] = NULL;
    }

    /// \see PAMI::Device::Interface::PacketDevice::Deterministic::getConnection()
    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    void * ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::getConnection_impl (size_t task, size_t offset)
    {
      size_t index = task2peer_impl (task) + offset * _npeers;
      TRACE_ERR((stderr, "ShmemDevice::getConnection_impl(%zu, %zu), _connection[%zu] = %p\n", task, offset, index, _connection[index]));

      PAMI_assert_debugf(_connection[index] != NULL, "Error. _connection[%zu] was not previously set.\n", index);

      return _connection[index];
    }

    /// \see PAMI::Device::Interface::PacketDevice::Deterministic::setConnection()
    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    void ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::setConnection_impl (void * value, size_t task, size_t offset)
    {
      size_t index = task2peer_impl (task) + offset * _npeers;
      TRACE_ERR((stderr, "ShmemDevice::setConnection_impl(%zu, %zu), _connection[%zu] = %p -> %p\n", task, offset, index, _connection[index], value));

      PAMI_assert_debugf(_connection[index] == NULL, "Error. _connection[%zu] was previously set.\n", index);

      _connection[index] = value;
    }

    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    size_t ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::advance ()
    {
      size_t events = 0;

      while (_rfifo.consumePacket(_dispatch))
        {
          events++;
        }

      /*pami_result_t res = PAMI_EAGAIN;
        if (_adv_obj != NULL)
        {
        res = _adv_obj->__advance(_context, (void*)_adv_obj);
        if (res == PAMI_SUCCESS) _adv_obj = NULL;
        }*/
#ifdef SHM_USE_COLLECTIVE_FIFO
      pami_result_t res;
      BaseMessage *msg;
      _collectiveQ.iter_begin(&_Iter);

      for (; _collectiveQ.iter_check(&_Iter); _collectiveQ.iter_end(&_Iter))
        {
          events++;
          msg = (BaseMessage *)_collectiveQ.iter_current(&_Iter);
          res = msg->__advance(_context, (void*)msg);

          if (res != PAMI_EAGAIN)
            {
              _collectiveQ.iter_remove(&_Iter);
              continue;
            }
        }


      /* Releasing done descriptors for comm world communicators */
      if (!_desc_fifo.is_empty())
        {
          //printf("Calling shmem colldevice advance\n");
          _desc_fifo.release_done_descriptors();
          events++;
        }
#endif
#ifdef TRAP_ADVANCE_DEADLOCK
      static size_t iteration = 0;
      TRACE_ERR((stderr, "(%zu) ShmemDevice::advance() iteration %zu \n", __global.mapping.task(), iteration));
      PAMI_assert (iteration++ < ADVANCE_DEADLOCK_MAX_LOOP);

      if (events) iteration = 0;

#endif
      return events;
    }

    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    inline size_t ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::fnum (size_t peer, size_t offset)
    {
      TRACE_ERR((stderr, "(%zu) ShmemDevice::fnum(%zu,%zu) -> %zu \n", __global.mapping.task(), peer, offset, _ncontexts * peer + offset));
      return _ncontexts * peer + offset;
    }

    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    inline bool ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::activePackets (size_t fnum)
    {
      return (_fifo[fnum].lastPacketConsumed() < _fifo[fnum].lastPacketProduced());
    }

    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    inline PAMI::Device::Generic::Device * ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::getLocalProgressDevice ()
    {
      return _local_progress_device;
    }

    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    inline PAMI::Device::Generic::Device * ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::getProgressDevice (size_t id)
    {
      return _local_progress_device;
    }

  };
};
#undef TRACE_ERR

// Include the non-inline method definitions
#include "components/devices/shmem/ShmemDevice_impl.h"

#endif // __components_devices_shmem_ShmemDevice_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
