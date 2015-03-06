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
 * \file components/devices/bgq/mu2/msg/CollectiveDPutMulticast.h
 * \brief ???
 */
#ifndef __components_devices_bgq_mu2_msg_CollectiveDPutMulticast_h__
#define __components_devices_bgq_mu2_msg_CollectiveDPutMulticast_h__

#include "spi/include/mu/DescriptorBaseXX.h"
#include "util/trace.h"
#include "components/devices/bgq/mu2/InjChannel.h"
#include "components/devices/bgq/mu2/msg/MessageQueue.h"
#include "common/bgq/Mapping.h"
#include "components/memory/MemoryAllocator.h"

#include "Memory.h"

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      ///
      /// \brief Inject one or more collective descriptors into an
      /// inject fifo and wait for the collective to complete. This
      /// code currently only works with linear FIFOs. For the
      /// collective DMA when data is injected it must have buffers
      /// posted to receive the data on the destinations. The root of
      /// the broadcast injects data while the remaining nodes inject
      /// zeroes. The data is bit ored and the result is accumulated on the non
      /// root nodes.
      ///
      class CollectiveDPutMulticast : public MessageQueue::Element
      {
        public:
          ///Dummy default constructor
          CollectiveDPutMulticast (): _context(*(MU::Context *)NULL) {}

          ///
          /// \brief Inject descriptor(s) into a specific injection fifo
          ///
          /// \param[in] context the MU context for this message
          /// \param[in] fn  completion event fn
          /// \param[in] cookie callback cookie
          /// \param[in] pwq pipeworkqueue that has data to be consumed
          /// \param[in] length the totaly number of bytes to be transfered
          /// \param[in] zerobuf buffer of zeros
          /// \param[in] zsize size of zero buffer
          /// \param[in] counterAddress address of the counter address
          ///
          CollectiveDPutMulticast (MU::Context         & context,
                                   pami_context_t        pami_context,
                                   pami_event_function   fn,
                                   void                * cookie,
                                   PipeWorkQueue       * pwq,
                                   uint32_t              length,
                                   char                * zerobuf,
                                   unsigned              zsize,
                                   bool                  isroot,
                                   volatile uint64_t   * counterAddress):
              _context (context),
              _pami_context(pami_context),
              _injectedBytes (0),
              _length (length),
              _pwq (pwq),
              _fn (fn),
              _cookie (cookie),
              _doneInjecting (false),
              _zBuf (zerobuf),
              _zSize (zsize),
              _isRoot (isroot),
              _cc (length),
              _counterAddress (counterAddress)
          {
            //TRACE_FN_ENTER();
            //TRACE_FN_EXIT();
          };

          inline ~CollectiveDPutMulticast () {};

          void init()
          {
            char * payload = NULL;

            if (_isRoot)
              {
                payload = (char*)_pwq->bufferToConsume();
                PAMI_assert (payload != NULL);
              }
            else
              payload = _zBuf;

            // Determine the physical address of the (temporary) payload
            // buffer from the model state memory.
            Kernel_MemoryRegion_t memRegion;
            uint32_t rc;
            rc = Kernel_CreateMemoryRegion (&memRegion, payload, _length);
            PAMI_assert ( rc == 0 );
            uint64_t paddr = (uint64_t)memRegion.BasePa +
                             ((uint64_t)payload - (uint64_t)memRegion.BaseVa);

            _desc.setPayload (paddr, 0);
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
            //TRACE_FN_ENTER();

            if (_doneInjecting)
              return true;

            uint64_t bytes_available = 0;

            if (_isRoot)
              {
                bytes_available = _pwq->bytesAvailableToConsume() - _injectedBytes;

                if (bytes_available == 0)
                  return false;
              }
            else
              {
                bytes_available = _length - _injectedBytes;

                if (bytes_available > _zSize)
                  bytes_available = _zSize;
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
              return false;

            _injectedBytes  += bytes_available;
            _desc.setRecPutOffset(_injectedBytes);

            if (_isRoot)
              _desc.Pa_Payload  = _desc.Pa_Payload + bytes_available;

            _doneInjecting = (_injectedBytes == _length);

            //TRACE_FN_EXIT();
            return _doneInjecting;
          }

          bool advanceRecv ()
          {
            uint64_t cval = *_counterAddress;

            if (cval == _cc)
              return (cval == 0);

            Memory::sync();

            if (!_isRoot)
              _pwq->produceBytes(_cc - cval);

            _cc = cval;

            if (cval == 0)
              {
                if (_isRoot) //Call consume bytes only once.
                  _pwq->consumeBytes(_length);

                if (_fn)
                  _fn (_pami_context, _cookie, PAMI_SUCCESS);

                return true;
              }

            return false;
          }

          bool advance()
          {
            bool flag = advanceSend() && advanceRecv();

#if 0

            if (_doneInjecting)
              printf ("Done Injecting\n");

            if (*_counterAddress == 0)
              printf ("Done Receiving\n");
            else
              printf ("Waiting for bytes %lu\n", *_counterAddress);

#endif

            return flag;
          }

          inline PipeWorkQueue *getPwq() { return _pwq; }

          MUSPI_DescriptorBase     _desc; //The descriptor is setup externally and contains batids, sndbuffer base and msg length

        protected:

          MU::Context            & _context;
          pami_context_t           _pami_context;
          uint32_t                 _injectedBytes;
          uint32_t                 _length;        //Number of bytes to transfer
          PipeWorkQueue          * _pwq;
          pami_event_function      _fn;
          void                   * _cookie;
          bool                     _doneInjecting;
          char                   * _zBuf;
          unsigned                 _zSize;
          bool                     _isRoot;
          uint64_t                 _cc;
          volatile uint64_t      * _counterAddress;

      }; // class     PAMI::Device::MU::CollectiveDPutMulticast
    };   // namespace PAMI::Device::MU
  };     // namespace PAMI::Device
};       // namespace PAMI

#endif // __components_devices_bgq_mu2_msg_CollectiveDPutMulticast_h__                     
