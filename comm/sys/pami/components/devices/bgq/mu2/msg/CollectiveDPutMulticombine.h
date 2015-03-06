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
///
/// \file components/devices/bgq/mu2/msg/CollectiveDPutMulticombine.h
/// \brief ???
///
#ifndef __components_devices_bgq_mu2_msg_CollectiveDPutMulticombine_h__
#define __components_devices_bgq_mu2_msg_CollectiveDPutMulticombine_h__

#include "spi/include/mu/DescriptorBaseXX.h"
#include "util/trace.h"
#include "components/devices/bgq/mu2/InjChannel.h"
#include "components/devices/bgq/mu2/msg/MessageQueue.h"
#include "common/bgq/Mapping.h"
#include "components/memory/MemoryAllocator.h"

#include "Memory.h"

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
      ///
      /// \brief Inject one or more collective descriptors into an
      /// injection fifo and wait for the collective to complete. This
      /// code currently only works with linear PipeWorkQueues.
      ///
      class CollectiveDPutMulticombine : public MessageQueue::Element
      {
        public:

          ///Default dummy constructor
          CollectiveDPutMulticombine (): _context(*(MU::Context*)NULL) {}

          ///
          /// \brief Inject descriptor(s) into a specific injection fifo
          ///
          /// \param[in] context the MU context for this message
          /// \param[in] fn  completion event fn
          /// \param[in] cookie callback cookie
          /// \param[in] spwq pipeworkqueue that has data to be consumed
          /// \param[in] dpwq pipeworkqueue where data has to be produced
          /// \param[in] length the totaly number of bytes to be transfered
          /// \param[in] op operation identifier
          /// \param[in] sizeoftype the size of the datatype
          /// \param[in] counterAddress address of the counter
          ///
          CollectiveDPutMulticombine (MU::Context         & context,
                                      pami_event_function   fn,
                                      void                * cookie,
                                      PipeWorkQueue       * spwq,
                                      PipeWorkQueue       * dpwq,
                                      uint32_t              length,
                                      uint32_t              op,
                                      uint32_t              sizeoftype,
                                      volatile uint64_t   * counterAddress):
              _context (context),
              _injectedBytes (0),
              _length (length),
              _spwq (spwq),
              _dpwq (dpwq),
              _fn (fn),
              _cookie (cookie),
              //	  _isRoot (isroot),
              _op (op),
              _sizeoftype(sizeoftype),
              _doneInjecting (false),
              _cc (length),
              _counterAddress (counterAddress)
          {
            TRACE_FN_ENTER();
            TRACE_FN_EXIT();
          };

          inline ~CollectiveDPutMulticombine () {};

          void init()
          {
            char * payload = NULL;
            //Find the src buf
            payload = (char*)_spwq->bufferToConsume();
            PAMI_assert (payload != NULL);

            // Determine the physical address of the (temporary) payload
            // buffer from the model state memory.
            Kernel_MemoryRegion_t memRegion;
            uint32_t rc;
            rc = Kernel_CreateMemoryRegion (&memRegion, payload, _length);
            PAMI_assert ( rc == 0 );
            uint64_t paddr = (uint64_t)memRegion.BasePa +
                             ((uint64_t)payload - (uint64_t)memRegion.BaseVa);

            _desc.setPayload (paddr, 0);
            _desc.setOpCode (_op);
            _desc.setWordLength (_sizeoftype);
          }

          ///
          /// \brief Inject descriptor message virtual advance implementation
          ///
          /// The inject descriptor message is complete only after all
          /// descriptors have been injected into the injection fifo.
          ///
          /// The number of descriptors to inject is specified as a template
          /// parameter for the class.
          ///
          /// \see MU::MessageQueue::Element::advance
          ///
          /// \retval true  Message complete
          /// \retval false Message incomplete and must remain on the queue
          ///
          bool advanceSend ()
          {
            TRACE_FN_ENTER();

            if (_doneInjecting)
              {
              TRACE_FN_EXIT();
              return true;
              }

            uint64_t bytes_available = 0;
            bytes_available = _spwq->bytesAvailableToConsume() - _injectedBytes;

            if (bytes_available == 0)
              {
              TRACE_FN_EXIT();
              return false;
              }

            _desc.Message_Length = bytes_available;
            //The is computed when the first descriptor for this round is injected
            size_t                fnum    = 0;
            InjChannel & channel = _context.injectionGroup.channel[fnum];
            size_t ndesc = channel.getFreeDescriptorCountWithUpdate ();

            if (ndesc > 0)
              {
                // Clone the message descriptors directly into the injection fifo.
                MUSPI_DescriptorBase * d = (MUSPI_DescriptorBase *) channel.getNextDescriptor ();
                //MUSPI_DescriptorDumpHex ((char *)"Coll Descriptor", (MUHWI_Descriptor_t *)&_desc);
                //fflush(stdout);
                //fflush(stderr);

                _desc.clone (*d);
                channel.injFifoAdvanceDesc ();
              }
            //No descriptor slots available, so come back and try later
            else
              {
              TRACE_FN_EXIT();
              return false;
              }

            _injectedBytes  += bytes_available;
            _desc.setRecPutOffset(_injectedBytes);
            _desc.Pa_Payload  = _desc.Pa_Payload + bytes_available;

            _doneInjecting = (_injectedBytes == _length);

            TRACE_FN_EXIT();
            return _doneInjecting;
          }

          bool advanceRecv ()
          {
            TRACE_FN_ENTER();
            uint64_t cval = *_counterAddress;

            if (cval == _cc)
              {
              TRACE_FN_EXIT();
              return (cval == 0);
              }

            Memory::sync();
            _dpwq->produceBytes(_cc - cval);
            _cc = cval;

            if (cval == 0)
              {
                _spwq->consumeBytes(_length);

                if (_fn)
                  _fn (NULL, _cookie, PAMI_SUCCESS);

                TRACE_FN_EXIT();
                return true;
              }

            TRACE_FN_EXIT();
            return false;
          }

          bool advance()
          {
            bool flag = advanceSend() && advanceRecv();

#if DO_TRACE_DEBUG
            TRACE_FN_ENTER();

            if (_doneInjecting)
              TRACE_STRING("Done Injecting");

            if (*_counterAddress == 0)
            {
              TRACE_STRING("Done Receiving");
            }
            else
              TRACE_FORMAT("Waiting for bytes, counterAdress = %lu", *_counterAddress);

            TRACE_FN_EXIT();
#endif

            return flag;
          }

          MUSPI_DescriptorBase     _desc; //The descriptor is setup externally and contains batids, sndbuffer base and msg length

        protected:

          MU::Context            & _context;
          uint32_t                 _injectedBytes;
          uint32_t                 _length;        //Number of bytes to transfer
          PipeWorkQueue          * _spwq;
          PipeWorkQueue          * _dpwq;
          pami_event_function      _fn;
          void                   * _cookie;
          uint32_t                 _op;
          uint32_t                 _sizeoftype;
          bool                     _doneInjecting;
          //bool                     _isRoot;
          uint64_t                 _cc;
          volatile uint64_t      * _counterAddress;

      }; // class     PAMI::Device::MU::CollectiveDPutMulticombine
    };   // namespace PAMI::Device::MU
  };     // namespace PAMI::Device
};       // namespace PAMI

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#endif // __components_devices_bgq_mu2_msg_CollectiveDPutMulticombine_h__                     
