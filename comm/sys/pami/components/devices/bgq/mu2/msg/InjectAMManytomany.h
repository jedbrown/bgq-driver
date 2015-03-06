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
/// \file components/devices/bgq/mu2/msg/InjectAMManytomany.h
/// \brief ???
///

#ifndef __components_devices_bgq_mu2_msg_InjectAMManytomany_h__
#define __components_devices_bgq_mu2_msg_InjectAMManytomany_h__

#include "components/devices/bgq/mu2/InjChannel.h"
#include "components/devices/bgq/mu2/msg/MessageQueue.h"
#include "components/devices/bgq/mu2/MemoryFifoPacketHeader.h"
#include "components/devices/bgq/mu2/MU_Util.h"

#include "util/trace.h"
#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0

#define PRIME_A         1346567UL
#define PRIME_B        17032781UL


namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      static const uint64_t AnyFifoMapVec [4] = {AnyFifoMap, MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
						 MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
						 MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0};
      struct M2mHdr {
	uint32_t  connid;
	uint32_t  srcidx;
      };

      template <typename T_Int, bool T_Single>
      class InjectAMManytomany
      {
      public:
	///
	/// \brief Inject descriptor(s) into a specific injection fifo to N destinations
	///
	/// \param[in] injfifo Specific injection fifo for the descriptor(s)
	///
	InjectAMManytomany<T_Int, T_Single> () :
	_mucontext (*(MU::Context*)NULL),
	  _next (0),
	  _model(*(MUSPI_DescriptorBase*)NULL),
	  _fn (NULL),
	  _cookie (NULL)
	  {
	    TRACE_FN_ENTER();	      
	    TRACE_FN_EXIT();
	  };
	
	InjectAMManytomany<T_Int, T_Single> (MU::Context               & mucontext,
					     Topology                  * topology,
					     M2MPipeWorkQueueT<T_Int, T_Single>      * pwq,
					     pami_event_function         fn,
					     void                      * cookie,
					     uint32_t                    connid,
					     MUSPI_DescriptorBase      & descbase, 
					     uint32_t                    srcidx,
					     pami_context_t              ctxt) :
	_mucontext (mucontext),
	  _doneCompletion(false),
	  _doneSending(false),
	  _topology (topology),
	  _nranks (topology->size()),
	  _next (0),
	  _pwq(pwq),
	  //_metasize(msgcount),
	  _model(descbase),
	  _fn (fn),
	  _cookie (cookie),
	  _ctxt(ctxt)
          {
            TRACE_FN_ENTER();

	    _src_base = pwq->bufferToConsume(0);
	    size_t bytes = pwq->bytesAvailableToConsume(0);

	    //convert virtual to physical
	    int rc = 0;
	    Kernel_MemoryRegion_t memRegion;
	    rc = Kernel_CreateMemoryRegion (&memRegion, (void *) _src_base, bytes);
	    PAMI_assertf ( rc == 0,"_src_base %p, bytes %zu, rc %d",_src_base, bytes, rc );
	    _paddr_base = (uint64_t)memRegion.BasePa +
	      ((uint64_t)(void *)_src_base - (uint64_t)memRegion.BaseVa);
	    
	    _amhdr.srcidx = srcidx;
	    _amhdr.connid = connid;

	    // Our arrays assume no more than 2*NumTorusDims fifos, may as well check now in the ctor.
	    PAMI_assertf(_mucontext.getNumInjFifos() <= 2*NumTorusDims,"number of context fifos %zu",_mucontext.getNumInjFifos());
	    for (size_t fifo = 0; fifo < 2*NumTorusDims; fifo++) 
	      _sequence[fifo] = UNDEFINED_SEQ_NO;
	    
            TRACE_FN_EXIT();
          };

	~InjectAMManytomany<T_Int, T_Single> () {};

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
	  bool advance ()
	  {
            TRACE_FN_ENTER();
	    
	    size_t numInjFifos = _mucontext.getNumInjFifos();
	    if (!_doneSending) {
	      MUSPI_DescriptorBase desc __attribute__((__aligned__(32)));
	      _model.clone (desc);
	      MemoryFifoPacketHeader *hdr = (MemoryFifoPacketHeader*) &desc.PacketHeader;
	      hdr->setMetaData(&_amhdr, sizeof(M2mHdr));

	      bool is_small  = _pwq->bytesAvailableToConsume(0) <= 2048;
	      bool is_medium = _pwq->bytesAvailableToConsume(0) <= 8192;

	      //We assumePRIME_B0  > max ranks (BG/Q is expected to have 6.8M tasks)
	      size_t random_seed, curidx;
	      if (is_small) {
		random_seed = PRIME_B * (_amhdr.srcidx+1) + PRIME_A;  
		curidx = ((random_seed % _nranks) + _next) % _nranks;
	      }
	      else {
		random_seed = PRIME_B*(_amhdr.srcidx+1) + PRIME_A; 
		curidx = (random_seed + _next * PRIME_A) % _nranks;
	      }
	      
	      size_t bytes_available = _pwq->bytesAvailableToConsume(0);
	      	      
	      size_t num_msgs_perfifo = 1;
	      if(is_small)
		num_msgs_perfifo = 16;
	      else if(is_medium)
		num_msgs_perfifo = 4;

	      size_t fifo = 0;	    
	      size_t rfifo8 =  *(size_t*)((char*)&desc + 40);
        register double fp0  FP_REGISTER(0);
        register double fp1  FP_REGISTER(1);
	      VECTOR_LOAD_NU (&desc,  0, fp0);
	      VECTOR_LOAD_NU (&desc, 32, fp1);	

	      while (_next  < _nranks) {
		InjChannel &channel = _mucontext.injectionGroup.channel[fifo];
		size_t ndesc = channel.getFreeDescriptorCountWithUpdate ();			
		MUSPI_DescriptorBase *d = (MUSPI_DescriptorBase *) channel.getNextDescriptor ();
		if (ndesc == 0)
		  break;
		if (ndesc > num_msgs_perfifo) 	//only permit 4 destinations per fifo at a time
		  ndesc = num_msgs_perfifo;		
		if ((_next + ndesc) > _nranks)
		  ndesc = _nranks - _next;

		register size_t i = 0;
		register int64_t paddr_base = (int64_t)_paddr_base - (int64_t)_src_base;
		size_t *rfifo8_addr = (size_t*)((char *)d + 40 /*8b aligned start of rfifo id*/);
		while (i < ndesc) {
		  //Wait for data to arrive in pipeworkqueue
		  if (!T_Single) {
		    bytes_available = _pwq->bytesAvailableToConsume(curidx);
		    if (bytes_available == 0) {		      
		      --ndesc;
		      ++_next;
		      
		      if (is_small) {
			++curidx;
			if (curidx >= _nranks)
			  curidx = 0;
		      }
		      else
			curidx = (random_seed + _next * PRIME_A) % _nranks;

		      continue;
		    }
		  }

		  //We know that we inject all data at one shot
		  char *src = _pwq->getBufferBase(curidx);
		  uint64_t paddr = (uint64_t)(paddr_base + (int64_t)src);
		  
		  //printf("%ld: Injecting descriptor to %d with offset %ld bytes %ld\n", __global.mapping.task(),
		  //     _ranks[curidx],
		  //     offset, bytes_available);
		  
		  uint32_t              dest;
		  uint16_t              rfifo;
		  uint64_t              map;		
		  // Clone the message descriptors directly into the injection fifo.
		  _mucontext.pinFifo (_topology->index2Rank(curidx),
				      0,
				      dest,
				      rfifo,
				      map);
		  size_t loc = (map & (0x30UL)) >> 4; //at the 6th and 7th bits the local fifos are specified
		  map = AnyFifoMapVec[loc];		
		  
		  size_t rfifo8_cur = rfifo8 | (((size_t)rfifo)<<22);
		  VECTOR_STORE_NU (d+i,  0, fp0);
		  VECTOR_STORE_NU (d+i, 32, fp1);	
		  //desc.clone (d+i);
		  d[i].setDestination (dest);
		  //d[i].setRecFIFOId (rfifo);
		  *rfifo8_addr = rfifo8_cur;
		  rfifo8_addr += 8; //Move to next desc
		  d[i].setPayload(paddr, bytes_available);
		  d[i].setTorusInjectionFIFOMap (map);		
		  //MUSPI_DescriptorDumpHex ((char *)"M2M Desc", &d);		  
		  ++i;
		  ++_next;
		  
		  if (is_small) {
		    ++curidx;
		    if (curidx >= _nranks)
		      curidx = 0;
		  }
		  else
		    curidx = (random_seed + _next * PRIME_A) % _nranks;		  
		  
		}
		
		// Advance the injection fifo tail pointer. This will be
		// moved outside the loop when an "advance multiple" is
		// available.
		_sequence[fifo] = channel.injFifoAdvanceDescMultiple(ndesc);
		//_next += ndesc;

		//if (curidx >= _nranks)
		//curidx = 0;
	
		fifo ++;
		if (fifo >= numInjFifos)
		  fifo = 0;
	      }

	      _doneSending = (_next == _nranks);	    
	    }	    

	    if (_doneSending) {
	      size_t fifo   = 0;
	      uint32_t done = 1;

	      for (fifo = 0; fifo < numInjFifos; fifo++) {
		if (_sequence[fifo] != UNDEFINED_SEQ_NO) {
		  InjChannel &channel = _mucontext.injectionGroup.channel[fifo];
		  unsigned rc = channel.checkDescComplete(_sequence[fifo]);	      
		  _sequence[fifo] |= seqno_table[rc];
		  done &=  rc;		  
		}
	      }

	      _doneCompletion = done;
	    }


            if (_doneCompletion && _fn != NULL)
	    {
	      _fn (_ctxt, _cookie, PAMI_SUCCESS);
	    }
	    
            TRACE_FN_EXIT();
            return _doneCompletion;
          }

	  pami_work_t  *workobj  () { return &_work; }

        protected:
	  MU::Context         & _mucontext;
	  bool                  _doneCompletion;
	  bool                  _doneSending;
	  Topology            * _topology;
	  size_t                _nranks;
          size_t                _next;
	  M2MPipeWorkQueueT<T_Int, T_Single>    * _pwq;
	  //unsigned            _metasize;
	  uint64_t              _paddr_base;
	  char                * _src_base;
	  M2mHdr                _amhdr;	  
	  MUSPI_DescriptorBase   &_model;
          pami_event_function   _fn;
          void                * _cookie;
	  pami_work_t           _work;
	  uint64_t              _sequence[NumTorusDims * 2];
	  pami_context_t        _ctxt;
      }; // class     PAMI::Device::MU::InjectAMManytomany
      
    };   // namespace PAMI::Device::MU
  };     // namespace PAMI::Device
};       // namespace PAMI

#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG

#endif // __components_devices_bgq_mu2_msg_InjectAMManytomany_h__
