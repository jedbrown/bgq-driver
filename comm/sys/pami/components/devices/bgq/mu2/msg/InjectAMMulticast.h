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
/// \file components/devices/bgq/mu2/msg/InjectAMMulticast.h
/// \brief ???
///

#ifndef __components_devices_bgq_mu2_msg_InjectAMMulticast_h__
#define __components_devices_bgq_mu2_msg_InjectAMMulticast_h__

#include "components/devices/bgq/mu2/InjChannel.h"
#include "components/devices/bgq/mu2/msg/MessageQueue.h"

#include "util/trace.h"
#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      class InjectAMMulticast : public MessageQueue::Element
      {
      public:

	///
	/// \brief Inject descriptor(s) into a specific injection fifo to N destinations
	///
	/// \param[in] injfifo Specific injection fifo for the descriptor(s)
	///
	InjectAMMulticast () :
	  _context (*(Context*)NULL),
	  _channel (*(InjChannel*)NULL),
	  _next (0),
	  _fn (NULL),
	  _cookie (NULL)
	    {
	      TRACE_FN_ENTER();	      
	      TRACE_FN_EXIT();
          };

          InjectAMMulticast (MU::Context         & context,
			     InjChannel          & channel,
           Topology            * topology,
			     PipeWorkQueue       * pwq,
			     unsigned              bytes,
			     unsigned              msgcount,
			     pami_event_function   fn,
			     void                * cookie) :
	  _context (context),
	  _channel (channel),
	  _topology (topology),
	  _nranks (topology->size()),
	  _next (0),
	  _pwq(pwq),
	  _bytes(bytes),
	  _metasize(msgcount),
	  _fn (fn),
	  _cookie (cookie)
          {
            TRACE_FN_ENTER();
            TRACE_FORMAT("topology  %p, nranks %zu,bytes %u",topology, _nranks, bytes);
            TRACE_FN_EXIT();
          };

          ~InjectAMMulticast () {};

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
          virtual bool advance ()
          {
            TRACE_FN_ENTER();

	    //Wait for data to arrive in pipeworkqueue
	    size_t bytes_available = 0;
	    if (_bytes > 0) {
	      bytes_available = _pwq->bytesAvailableToConsume();
	      if (bytes_available != _bytes)
		return false;
	      
	      char *src = _pwq->bufferToConsume();
	      memcpy(packetBuf()+_metasize*sizeof(pami_quad_t),src,_bytes); 
	    }

            size_t ndesc = _channel.getFreeDescriptorCountWithUpdate ();
	    TRACE_FORMAT("InjectAMMulticast:  advance:  ndesc=%zu\n",ndesc);

            // Clone the message descriptors directly into the injection fifo.
            MUSPI_DescriptorBase * d = (MUSPI_DescriptorBase *) _channel.getNextDescriptor ();

            size_t i;
            uint64_t sequence = 0;

            for (i = 0; i < ndesc && (_next + i) < _nranks; i++)
              {
		MUHWI_Destination_t   dest;
		uint16_t              rfifo;
		uint64_t              map;
    pami_task_t           rank = _topology->index2Rank(_next + i);
		_context.pinFifo (rank,
				  0,
				  dest,
				  rfifo,
				  map);

                _desc.clone (d[i]);
		d[i].setDestination (dest);
		d[i].setRecFIFOId (rfifo);
		d[i].setTorusInjectionFIFOMap (map);
                TRACE_FORMAT("inject descriptor (%p) from message (%p)", &d[_next+i], this);		
              }
            _next += i;
	  
	    // Advance the injection fifo tail pointer. This will be
	    // moved outside the loop when an "advance multiple" is
	    // available.
	    sequence = _channel.injFifoAdvanceDescMultiple (i);
            bool done = (_next == _nranks);

            if (done && likely(_fn != NULL))
            {
	      _channel.addCompletionEvent (_state, _fn, _cookie, sequence);
	    }
	    
            TRACE_FN_EXIT();
            return done;
          }

          ///
          /// \brief Reset the internal state of the message
          ///
          /// \note Only used for message reuse.
          ///
          void reset () { _next = 0; };

          MUSPI_DescriptorBase   _desc;
	  pami_work_t  *workobj  () { return &_work; }
	  char         *packetBuf() { return (char *)_pktbuf; }

        protected:
	  MU::Context         & _context;
          InjChannel          & _channel;
          Topology            * _topology;
          size_t                _nranks;
          size_t                _next;
	  PipeWorkQueue       * _pwq;
	  unsigned              _bytes;
	  unsigned              _metasize;
          pami_event_function   _fn;
          void                * _cookie;
          uint8_t               _state[InjChannel::completion_event_state_bytes];
	  pami_work_t           _work;
	  uint8_t               _pktbuf [MU::Context::packet_payload_size];

      }; // class     PAMI::Device::MU::InjectAMMulticast
      
    };   // namespace PAMI::Device::MU
  };     // namespace PAMI::Device
};       // namespace PAMI

#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG

#endif // __components_devices_bgq_mu2_msg_InjectAMMulticast_h__
