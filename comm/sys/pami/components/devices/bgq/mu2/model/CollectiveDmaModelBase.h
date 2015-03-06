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
 * \file components/devices/bgq/mu2/model/CollectiveDmaModelBase.h
 * \brief ???
 */
#ifndef __components_devices_bgq_mu2_model_CollectiveDmaModelBase_h__
#define __components_devices_bgq_mu2_model_CollectiveDmaModelBase_h__

#include "spi/include/mu/DescriptorBaseXX.h"
#include "components/devices/MulticastModel.h"
#include "components/devices/MulticombineModel.h"
#include "components/memory/MemoryAllocator.h"
#include "util/trace.h"
#include "components/devices/bgq/mu2/Context.h"
#include "components/devices/bgq/mu2/msg/CollectiveDPutMulticast.h"
#include "components/devices/bgq/mu2/msg/CollectiveDPutMulticombine.h"
#include "sys/pami.h"
#include "math/a2qpx/Core_memcpy.h"

#include "Memory.h"

//#define MU_SHORT_BLOCKING_COLLECTIVE 0

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0


namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
#define TEMP_BUF_SIZE 512

      class CollectiveDmaModelBase
      {

        public:
          class CollState
          {
            public:
              bool                                _isInited;          /// Has this class been initialized?
              char                              * _tempBuf;           /// Temporary buffer for short collectives
              uint32_t                            _tempSize;          /// Size of temporary buffer
              volatile uint64_t                   _colCounter;        /// Counter used in the collective op
              uint64_t                            _tempPAddr;         /// Physical address of the temp buf
              int32_t                             _tBatID;            /// Bat id of the temporary buffer
              int32_t                             _pBatID;            /// payload bat id
              int32_t                             _cBatID;            /// counter bat id

              CollState ()
              {
                TRACE_FN_ENTER();
                _isInited = false;
                _tempSize = TEMP_BUF_SIZE;
                _tempBuf = (char *)malloc (_tempSize * sizeof(char));
                TRACE_FN_EXIT();
              }

              bool isInited () { return _isInited; }

              int tempBatID()    { return _tBatID; }
              int payloadBatID() { return _pBatID; }
              int counterBatID() { return _cBatID; }

              void init (MU::Context &mucontext, pami_result_t &status)
              {
                TRACE_FN_ENTER();
                if (_isInited) 
                  {
                  TRACE_FN_EXIT();
                  return;
                  }

                ///// Get the BAT IDS ///////////////
                //// Setup CounterVec in BAT
                _tBatID = mucontext.getShortCollectiveBatId();

                if (_tBatID == -1)
                  {
                    status = PAMI_ERROR;
                    TRACE_STRING("Error");
                    TRACE_FN_EXIT();
                    return;
                  }

                _pBatID = mucontext.getThroughputCollectiveBufferBatId ();

                if (_pBatID == -1)
                  {
                    status = PAMI_ERROR;
                    TRACE_STRING("Error");
                    TRACE_FN_EXIT();
                    return;
                  }

                _cBatID = mucontext.getThroughputCollectiveCounterBatId ();

                if (_cBatID == -1)
                  {
                    status = PAMI_ERROR;
                    TRACE_STRING("Error");
                    TRACE_FN_EXIT();
                    return;
                  }

                //printf ("Get Bat Ids %d %d %d\n", _tBatID, _pBatID, _cBatID);
                Kernel_MemoryRegion_t memRegion;
                int rc = 0;

                rc = Kernel_CreateMemoryRegion (&memRegion, (void *)_tempBuf, _tempSize);
                _tempPAddr = (uint64_t)memRegion.BasePa +
                             ((uint64_t)(void *)_tempBuf - (uint64_t)memRegion.BaseVa);
                mucontext.setShortCollectiveBatEntry (_tempPAddr);

                rc = Kernel_CreateMemoryRegion (&memRegion, (void *) & _collstate._colCounter, sizeof(uint64_t));
                PAMI_assert ( rc == 0 );
                uint64_t paddr = (uint64_t)memRegion.BasePa +
                                 ((uint64_t)(void *) & _collstate._colCounter - (uint64_t)memRegion.BaseVa);

                uint64_t atomic_address = MUSPI_GetAtomicAddress(paddr, MUHWI_ATOMIC_OPCODE_STORE_ADD);
                mucontext.setThroughputCollectiveCounterBatEntry (atomic_address);

                _isInited = true;
                TRACE_FN_EXIT();
                return;
              }
          };


          class CollectiveCompletionMsg
          {
            public:
              pami_event_function                        _cb_done;     /// Short message callback
              void                                     * _cookie;      /// Short message user client data
              PipeWorkQueue                            * _dpwq;        /// Short dstination PipeWorkQueue
              uint64_t                                   _bytes;       /// Short bytes
              volatile uint64_t                        * _counterAddress; /// Counter address

	      CollectiveCompletionMsg () {          
                TRACE_FN_ENTER();
                TRACE_FN_EXIT();
              }
          };

          CollectiveDmaModelBase ():
              _mucontext(*(MU::Context*)NULL),
              _context(NULL),
              _injChannel (*(InjChannel *)NULL),
              _gdev(*(Generic::Device*)NULL)
          {
          TRACE_FN_ENTER();
          TRACE_FN_EXIT();
          }

          CollectiveDmaModelBase (pami_context_t   pami_context,
                                  MU::Context    & context,
                                  pami_result_t  & status):
              _mucontext(context),
              _context(pami_context),
              _injChannel (context.injectionGroup.channel[0]),
              _gdev(*context.getProgressDevice())
          {
            TRACE_FN_ENTER();
            //The collective state must be initialized by task 0 context 0
            if (__global.mapping.t() == 0)
              _collstate.init(context, status);

            _scompmsg._counterAddress = &_collstate._colCounter;
            new (&_swork) PAMI::Device::Generic::GenericThread(short_advance, &_scompmsg);
            new (&_mwork) PAMI::Device::Generic::GenericThread(mid_advance, &_scompmsg);

            initDescBase();

	    _spinCycles = 0UL;
	    size_t num_nodes = 
	      __global.mapping.torusSize(0) * 
	      __global.mapping.torusSize(1) * 
	      __global.mapping.torusSize(2) * 
	      __global.mapping.torusSize(3) * 
	      __global.mapping.torusSize(4);
	    
	    if (num_nodes <= 512)
	      _spinCycles = 8 *  1600UL; //8  us
	    else if (num_nodes <= 8192)
	      _spinCycles = 10 * 1600UL; //10 us
	    else 
	      _spinCycles = 12 * 1600UL; //12 us

            TRACE_FN_EXIT();
          }

          void initDescBase()
          {
            TRACE_FN_ENTER();
            // Zero-out the descriptor models before initialization
            memset((void *)&_modeldesc, 0, sizeof(_modeldesc));

            // --------------------------------------------------------------------
            // Set the common base descriptor fields
            //
            // For the remote get packet, send it using the high priority torus
            // fifo map.  Everything else uses non-priority torus fifos, pinned
            // later based on destination.  This is necessary to avoid deadlock
            // when the remote get fifo fills.  Note that this is in conjunction
            // with using the high priority virtual channel (set elsewhere).
            // --------------------------------------------------------------------
            MUSPI_BaseDescriptorInfoFields_t base;
            memset((void *)&base, 0, sizeof(base));

            base.Pre_Fetch_Only  = MUHWI_DESCRIPTOR_PRE_FETCH_ONLY_NO;
            base.Message_Length  = 0;
            base.Payload_Address = _collstate._tempPAddr;
            _modeldesc.setBaseFields (&base);

            // --------------------------------------------------------------------
            // Set the common point-to-point descriptor fields
            //
            // For the remote get packet, send it on the high priority virtual
            // channel.  Everything else is on the deterministic virtual channel.
            // This is necessary to avoid deadlock when the remote get fifo fills.
            // Note that this is in conjunction with setting the high priority
            // torus fifo map (set elsewhere).
            // --------------------------------------------------------------------
            MUSPI_CollectiveDescriptorInfoFields_t coll;
            memset((void *)&coll, 0, sizeof(coll));

            //Setup for broadcast
            coll.Op_Code = MUHWI_COLLECTIVE_OP_CODE_OR;
            coll.Word_Length = 4; //preset to doubles and uint8 operations
            coll.Misc =
              MUHWI_PACKET_VIRTUAL_CHANNEL_USER_SUB_COMM |
              MUHWI_COLLECTIVE_TYPE_ALLREDUCE;
            coll.Class_Route = 0;

            _modeldesc.setCollectiveFields (&coll);
            _modeldesc.setDataPacketType (MUHWI_COLLECTIVE_DATA_PACKET_TYPE);
            _modeldesc.PacketHeader.NetworkHeader.collective.Byte8.Size = 16;
            _modeldesc.setTorusInjectionFIFOMap(MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CUSER);

            // --------------------------------------------------------------------
            // Set the direct put descriptor fields
            // --------------------------------------------------------------------
            MUSPI_DirectPutDescriptorInfoFields dput;
            memset((void *)&dput, 0, sizeof(dput));

            dput.Rec_Payload_Base_Address_Id = _collstate.tempBatID();
            dput.Rec_Payload_Offset          = 0;
            dput.Rec_Counter_Base_Address_Id = _collstate.counterBatID();
            dput.Rec_Counter_Offset          = 0;
            dput.Pacing                      = MUHWI_PACKET_DIRECT_PUT_IS_NOT_PACED;

            _modeldesc.setDirectPutFields (&dput);
            _modeldesc.setMessageUnitPacketType (MUHWI_PACKET_TYPE_PUT);

	    //Skip link checksumming the packet header
	    _modeldesc.setCollectiveSkip(8);	
	    
            TRACE_FN_EXIT();
          }

          void postShortCompletion (pami_event_function      cb_done,
                                    void                   * cookie,
                                    size_t                   bytes,
                                    PipeWorkQueue          * pwq)
          {
            TRACE_FN_ENTER();
            _scompmsg._cb_done = cb_done;
            _scompmsg._cookie  = cookie;
            _scompmsg._bytes   = bytes;
            _scompmsg._dpwq    = pwq;

            PAMI::Device::Generic::GenericThread *work = (PAMI::Device::Generic::GenericThread *) & _swork;
            _gdev.postThread(work);
            TRACE_FN_EXIT();
          }

          void postMidCompletion (pami_event_function      cb_done,
				  void                   * cookie,
				  size_t                   bytes,
				  PipeWorkQueue          * pwq)
          {
            TRACE_FN_ENTER();
            _scompmsg._cb_done = cb_done;
            _scompmsg._cookie  = cookie;
            _scompmsg._bytes   = bytes;
            _scompmsg._dpwq    = pwq;
	    
            PAMI::Device::Generic::GenericThread *work = (PAMI::Device::Generic::GenericThread *) & _mwork;
            _gdev.postThread(work);
            TRACE_FN_EXIT();
          }

          pami_result_t  postShortCollective (uint32_t        opcode,
                                              uint32_t        sizeoftype,
                                              uint32_t        bytes,
                                              char          * src,
                                              PipeWorkQueue * dpwq,
                                              pami_event_function   cb_done,
                                              void          * cookie,
                                              unsigned        classroute)
          {
            TRACE_FN_ENTER();
            TRACE_FORMAT("opcode %u, sizeoftype %u, bytes %u, src %p, dpwq %p, classroute %u", opcode, sizeoftype, bytes, src, dpwq, classroute);
            PAMI_assert (bytes <= _collstate._tempSize);
            PAMI_assert(bytes);
            _int64Cpy(_collstate._tempBuf, src, bytes);
            //memcpy(_collstate._tempBuf, src, bytes);

            bool flag = _injChannel.hasFreeSpaceWithUpdate();

            if (!flag)
              {
              TRACE_STRING("Error");
              TRACE_FN_EXIT();
              return PAMI_ERROR;
              }

            _collstate._colCounter = bytes;
            // Clone the message descriptors directly into the injection fifo.
            MUSPI_DescriptorBase *d = (MUSPI_DescriptorBase *) _injChannel.getNextDescriptor ();
            _modeldesc.clone (*d);
            d->setClassRoute (classroute);
            d->setPayload (_collstate._tempPAddr, bytes);
            d->setOpCode (opcode);
            d->setWordLength (sizeoftype);
            TRACE_HEXDATA(d, sizeof(*d));
            //d->PacketHeader.messageUnitHeader.Packet_Types.Direct_Put.Rec_Payload_Base_Address_Id = _pBatID;
            //_mucontext.setThroughputCollectiveBufferBatEntry(_collstate._tempPAddr);

            //fprintf(stderr, "Collective on class route %d\n", classroute);
            //MUSPI_DescriptorDumpHex ((char *)"Coll Descriptor", (MUHWI_Descriptor_t *)d);
            _injChannel.injFifoAdvanceDesc ();

#ifndef MU_SHORT_BLOCKING_COLLECTIVE
	    size_t end = GetTimeBase() + _spinCycles;
	    while ( (_collstate._colCounter != 0) && (GetTimeBase() < end) );

	    if (_collstate._colCounter == 0) {
	      Memory::sync();	      
	      if (dpwq)
		{
		  char *dst = dpwq->bufferToProduce();
		  _int64Cpy (dst, _collstate._tempBuf, bytes);
		  dpwq->produceBytes(bytes);
		}
	      
	      cb_done (_context, cookie, PAMI_SUCCESS);
	    }
	    else 
	      postShortCompletion(cb_done, cookie, bytes, dpwq);
#else

            while (_collstate._colCounter != 0);

            Memory::sync();

            if (dpwq)
              {
                char *dst = dpwq->bufferToProduce();
                _int64Cpy (dst, _collstate._tempBuf, bytes);
                dpwq->produceBytes(bytes);
              }

            cb_done (_context, cookie, PAMI_SUCCESS);
#endif

            TRACE_FN_EXIT();
            return PAMI_SUCCESS;
          }

          pami_result_t  postMidCollective (uint32_t              opcode,
					    uint32_t              sizeoftype,
					    uint32_t              bytes,
					    uint64_t              src_pa,
					    uint64_t              dst_pa,
					    PAMI::PipeWorkQueue * dpwq,
					    pami_event_function   cb_done,
					    void                * cookie,
					    unsigned              classroute)
          {
            TRACE_FN_ENTER();
            TRACE_FORMAT("postMidCollective: opcode %u, sizeoftype %u, bytes %u, src_pa %ld, dpwq %p, classroute %u\n", opcode, sizeoftype, bytes, src_pa, dpwq, classroute);
            PAMI_assert(bytes);	    
            bool flag = _injChannel.hasFreeSpaceWithUpdate();

            if (!flag)
            {
	      TRACE_STRING("Error");
              TRACE_FN_EXIT();
              return PAMI_ERROR;
	    }

	    _mucontext.setThroughputCollectiveBufferBatEntry(dst_pa);	    
	    _collstate._colCounter = bytes;
	    
            // Clone the message descriptors directly into the injection fifo.
            MUSPI_DescriptorBase *d = (MUSPI_DescriptorBase *) _injChannel.getNextDescriptor ();
            _modeldesc.clone (*d);
            d->setClassRoute (classroute);
            d->setPayload (src_pa, bytes);
            d->setOpCode (opcode);
            d->setWordLength (sizeoftype);
            d->PacketHeader.messageUnitHeader.Packet_Types.Direct_Put.Rec_Payload_Base_Address_Id = _collstate.payloadBatID();

            //MUSPI_DescriptorDumpHex((char*)"postMidCollective", d);
            _injChannel.injFifoAdvanceDesc ();
	    
#ifndef MU_SHORT_BLOCKING_COLLECTIVE
	    size_t end = GetTimeBase() + _spinCycles;
	    while ( (_collstate._colCounter != 0) && (GetTimeBase() < end) );

	    if (_collstate._colCounter == 0) {
	      Memory::sync();	    
	      if (dpwq)
		dpwq->produceBytes(bytes);
	      
	      cb_done (_context, cookie, PAMI_SUCCESS);
	    }
	    else
	      postMidCompletion(cb_done, cookie, bytes, dpwq);
#else
            while (_collstate._colCounter != 0);
            Memory::sync();
	    
            if (dpwq)
	      dpwq->produceBytes(bytes);
	    
            cb_done (_context, cookie, PAMI_SUCCESS);
#endif

            TRACE_FN_EXIT();
            return PAMI_SUCCESS;
          }


          pami_result_t postCollective (uint32_t                   bytes,
                                        PAMI::PipeWorkQueue      * src,
                                        PAMI::PipeWorkQueue      * dst,
                                        pami_event_function        cb_done,
                                        void                     * cookie,
                                        uint32_t                   op,
                                        uint32_t                   sizeoftype,
                                        unsigned                   classroute)
          {
            TRACE_FN_ENTER();
            //Pin the buffer to the bat id. On the root the src buffer
            //is used to receive the allreduce message
            char *dstbuf = NULL;
            dstbuf = dst->bufferToProduce();

            Kernel_MemoryRegion_t memRegion;
            uint rc = 0;
            rc = Kernel_CreateMemoryRegion (&memRegion, dstbuf, bytes);
            PAMI_assert ( rc == 0 );
            uint64_t paddr = (uint64_t)memRegion.BasePa +
                             ((uint64_t)dstbuf - (uint64_t)memRegion.BaseVa);
            _mucontext.setThroughputCollectiveBufferBatEntry(paddr);

            _collstate._colCounter = bytes;
            CollectiveDPutMulticombine *msg = new (&_mcomb_msg) CollectiveDPutMulticombine (_mucontext,
                cb_done,
                cookie,
                src,
                dst,
                bytes,
                op,
                sizeoftype,
                &_collstate._colCounter);
            _modeldesc.clone (msg->_desc);
            msg->_desc.PacketHeader.messageUnitHeader.Packet_Types.Direct_Put.Rec_Payload_Base_Address_Id = _collstate.payloadBatID();
            msg->_desc.setClassRoute (classroute);

            msg->init();
            bool flag = msg->advance();

            if (!flag)
	    {
	      PAMI::Device::Generic::GenericThread *work = new (&_work) PAMI::Device::Generic::GenericThread(advance, msg);
	      _mucontext.getProgressDevice()->postThread(work);
	    }

            TRACE_FN_EXIT();
            return PAMI_SUCCESS;
          }


          pami_result_t postBroadcast (uint32_t                   bytes,
                                       PAMI::PipeWorkQueue      * src,
                                       PAMI::PipeWorkQueue      * dst,
                                       pami_event_function        cb_done,
                                       void                     * cookie,
                                       char                     * zerobuf,
                                       uint32_t                   zbytes,
                                       bool                       isroot,
                                       unsigned                   classroute) __attribute__((noinline, weak));

          static pami_result_t short_advance (pami_context_t     context,
                                              void             * cookie)
          {
            TRACE_FN_ENTER();
            CollectiveCompletionMsg  *scmsg = (CollectiveCompletionMsg *) cookie;

            if (*scmsg->_counterAddress == 0)
              {
                Memory::sync();

                if (scmsg->_dpwq)
                  {
                    char *dst = scmsg->_dpwq->bufferToProduce();
                    _int64Cpy (dst, _collstate._tempBuf, scmsg->_bytes);
                    scmsg->_dpwq->produceBytes(scmsg->_bytes);
                  }

                if (scmsg->_cb_done)
                  scmsg->_cb_done (context, scmsg->_cookie, PAMI_SUCCESS);

                TRACE_FN_EXIT();
                return PAMI_SUCCESS;
              }

            TRACE_STRING("EAGAIN");
            TRACE_FN_EXIT();
            return PAMI_EAGAIN;
          }


          static pami_result_t mid_advance (pami_context_t     context,
					    void             * cookie)
          {
            TRACE_FN_ENTER();
            CollectiveCompletionMsg  *msg = (CollectiveCompletionMsg *) cookie;

            if (*msg->_counterAddress == 0)
              {
                Memory::sync();
		
                if (msg->_dpwq)
		  msg->_dpwq->produceBytes(msg->_bytes);
		
                if (msg->_cb_done)
                  msg->_cb_done (context, msg->_cookie, PAMI_SUCCESS);
		
                TRACE_FN_EXIT();
                return PAMI_SUCCESS;
              }

            TRACE_STRING("EAGAIN");
            TRACE_FN_EXIT();
            return PAMI_EAGAIN;
          }

          static pami_result_t advance (pami_context_t     context,
                                        void             * cookie)
          {
            TRACE_FN_ENTER();
            MessageQueue::Element *msg = (MessageQueue::Element *) cookie;
            bool done = msg->advance();

            if (done)
              {
              TRACE_FN_EXIT();
              return PAMI_SUCCESS;
              }
            TRACE_STRING("EAGAIN");
            TRACE_FN_EXIT();
            return PAMI_EAGAIN;
          }

          static CollState                            _collstate;

        protected:
          MU::Context                              & _mucontext;         /// Pointer to MU context
          pami_context_t                             _context;
          InjChannel                               & _injChannel;
          Generic::Device                          & _gdev;
	  size_t                                     _spinCycles;
          MUSPI_DescriptorBase                       _modeldesc;         /// Model descriptor
          CollectiveCompletionMsg                    _scompmsg;
          pami_work_t                                _swork;
          pami_work_t                                _mwork;
          pami_work_t                                _work;
          CollectiveDPutMulticast                    _mcast_msg;
          CollectiveDPutMulticombine                 _mcomb_msg;
      };

      pami_result_t CollectiveDmaModelBase::postBroadcast (uint32_t                   bytes,
                                                           PAMI::PipeWorkQueue      * src,
                                                           PAMI::PipeWorkQueue      * dst,
                                                           pami_event_function        cb_done,
                                                           void                     * cookie,
                                                           char                     * zerobuf,
                                                           uint32_t                   zbytes,
                                                           bool                       isroot,
                                                           unsigned                   classroute)
      {
        TRACE_FN_ENTER();
        //Pin the buffer to the bat id. On the root the src buffer
        //is used to receive the broadcast message
        char *dstbuf = NULL;

        if (isroot)
          dstbuf = src->bufferToConsume();
        else
          dstbuf = dst->bufferToProduce();

        Kernel_MemoryRegion_t memRegion;
        uint rc = 0;
        rc = Kernel_CreateMemoryRegion (&memRegion, dstbuf, bytes);
        PAMI_assert ( rc == 0 );
        uint64_t paddr = (uint64_t)memRegion.BasePa +
                         ((uint64_t)dstbuf - (uint64_t)memRegion.BaseVa);
        _mucontext.setThroughputCollectiveBufferBatEntry(paddr);

        _collstate._colCounter = bytes;
        CollectiveDPutMulticast *msg = new (&_mcast_msg) CollectiveDPutMulticast (_mucontext,
                                                                                  _context,
                                                                                  cb_done,
                                                                                  cookie,
                                                                                  (isroot) ? src : dst,
                                                                                  bytes,
                                                                                  zerobuf,
                                                                                  zbytes,
                                                                                  isroot,
                                                                                  &_collstate._colCounter);
        _modeldesc.clone (msg->_desc);
        msg->_desc.PacketHeader.messageUnitHeader.Packet_Types.Direct_Put.Rec_Payload_Base_Address_Id = _collstate.payloadBatID();
        msg->_desc.setClassRoute (classroute);

        msg->init();
        bool flag = msg->advance();

        if (!flag)
          {
            PAMI::Device::Generic::GenericThread *work = new (&_work) PAMI::Device::Generic::GenericThread(advance, msg);
            _mucontext.getProgressDevice()->postThread(work);
          }

        TRACE_FN_EXIT();
        return PAMI_SUCCESS;
      }

    };
  };
};
#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#endif
