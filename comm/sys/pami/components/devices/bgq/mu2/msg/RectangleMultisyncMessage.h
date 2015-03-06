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
 * \file components/devices/bgq/mu2/msg/RectangleMultisyncMessage.h
 * \brief ???
 */
#ifndef __components_devices_bgq_mu2_msg_RectangleMultisyncMessage_h__
#define __components_devices_bgq_mu2_msg_RectangleMultisyncMessage_h__

#include "components/devices/bgq/mu2/Context.h"
#include "sys/pami.h"
#include "components/devices/bgq/mu2/MU_Util.h"

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      namespace Rectangle
      {
	static const size_t   NumClassRoutes  = 64;
	static const uint32_t UndefinedDest   = 0xffffffff;
	static const size_t   SENDING         = 0;
	static const size_t   WAITING         = 1;
	static const size_t   DONE            = 2;	
	static const uint64_t CounterDefault  = 0xffffffffffffffffUL;
	
	///
	/// \brief The state that stores the destiantions and counter
	///        for each connection. Aligned to 2 cache lines
	///
	struct MultisyncState {
	  uint32_t            _hwdest  [NumTorusDims][2];  //Destinations to send put messages (5 phases and +ve and -ve directions)
	  uint8_t             _sstatus [NumTorusDims][2];  //Send status = 0 (both sends ready to go), status 11 (both sends done) 
	  uint8_t             _dimsizes[NumTorusDims];     //Size-1 of each dimensions 
	  uint8_t             _iteration;                  //Iteration even or odd?
	};

	struct MultisyncParams {
	  MUSPI_DescriptorBase          _modeldesc  __attribute__((__aligned__(32)));  //Model desctiptor 
	  MU::Context                 & _mucontext;
	  pami_context_t                _context;
	  MultisyncState                _statevec   [NumClassRoutes];
	  volatile uint64_t             _countervec [NumClassRoutes*NumTorusDims*2];

	  MultisyncParams (MU::Context               & mucontext,
			   pami_context_t              context):
	  _mucontext(mucontext),
	  _context(context)
	  {
	    //Constructor
	  }
	};

	class MultisyncMessage {
	protected:
	  MultisyncParams             * _params;
	  pami_event_function           _cb_done;
	  void                        * _cookie;
	  size_t                        _classroute;
	  uint32_t                      _phaseStatus;      //Where are we in the current phase
	  uint32_t                      _phase;            //Dimension being processed (0...4)

	public:
	  pami_work_t                   _work;
	  
	  MultisyncMessage (MultisyncParams           * params,
			    pami_event_function         cb_done,
			    void                      * cookie,
			    size_t                      classroute):
	  _params(params),
	  _cb_done (cb_done),
	  _cookie (cookie),
	  _classroute(classroute),
	  _phaseStatus(DONE)
	    {
	      //Constructor
	    }

	  void init () {
	    _phase = 0;
	    _phaseStatus = SENDING;
	    _params->_statevec[_classroute]._iteration = (_params->_statevec[_classroute]._iteration + 1) & 0x1;
	  }

	  bool advance ();

	  bool injectDescriptors (uint32_t       dest0,
				  uint32_t       dest1,		
				  uint8_t      & status0,
				  uint8_t      & status1,
				  unsigned       fifo0,
				  unsigned       fifo1,
				  size_t         counter_offset,
				  unsigned       iteration) 
	  {
      register double fp0  FP_REGISTER(0);
      register double fp1  FP_REGISTER(1);
	    VECTOR_LOAD_NU (&_params->_modeldesc,  0, fp0);
	    VECTOR_LOAD_NU (&_params->_modeldesc, 32, fp1);		  	    
	    
	    uint64_t  map0     = _mu_fifomaps  [fifo0];
	    uint8_t   hints0   = _mu_hintsABCD [fifo0];
	    uint8_t   misc10   = _mu_pt2ptmisc1_deposit [fifo0];
	    uint64_t  map1     = _mu_fifomaps  [fifo1];
	    uint8_t   hints1   = _mu_hintsABCD [fifo1];
	    uint8_t   misc11   = _mu_pt2ptmisc1_deposit [fifo1];

	    if ((dest0 != UndefinedDest) && (status0 == 0)) {	  	    	 
	      size_t fifo = (fifo0 < _params->_mucontext.getNumInjFifos())? fifo0 : 0 ;
	      InjChannel & channel = _params->_mucontext.injectionGroup.channel[fifo];
	      bool flag = channel.hasFreeSpaceWithUpdate();

	      if (flag) {
		MUSPI_DescriptorBase * d = (MUSPI_DescriptorBase *) channel.getNextDescriptor (); 

		// Clone the message descriptors directly into the injection fifo.
		VECTOR_STORE_NU (d,  0, fp0);
		VECTOR_STORE_NU (d, 32, fp1);					
		//_params->_modeldesc.clone (d[0]);

		d[0].PacketHeader.messageUnitHeader.Packet_Types.Direct_Put.Counter_Offset = counter_offset;
		d[0].setDestination(dest0);
		d[0].setHintsABCD(hints0);
		d[0].setPt2PtMisc1(misc10);
		d[0].Torus_FIFO_Map = map0;
		channel.injFifoAdvanceDesc ();  
		status0 = 1;
	      }
	      else return false;
	    }
	      
	    if ((dest1 != UndefinedDest) && (status1 == 0)) {		
	      size_t fifo = (fifo1 < _params->_mucontext.getNumInjFifos())? fifo1 : 1 ;
	      InjChannel & channel = _params->_mucontext.injectionGroup.channel[fifo];
	      bool flag = channel.hasFreeSpaceWithUpdate();
	      
	      if (flag) {
		MUSPI_DescriptorBase * d = (MUSPI_DescriptorBase *) channel.getNextDescriptor (); 

		// Clone the message descriptors directly into the injection fifo.
		VECTOR_STORE_NU (d,  0, fp0);
		VECTOR_STORE_NU (d, 32, fp1);			
		//_params->_modeldesc.clone (d[0]);

		d[0].PacketHeader.messageUnitHeader.Packet_Types.Direct_Put.Counter_Offset = counter_offset;
		d[0].setDestination(dest1);
		d[0].setHintsABCD(hints1);
		d[0].setPt2PtMisc1(misc11);
		d[0].Torus_FIFO_Map = map1;
		channel.injFifoAdvanceDesc ();  
		status1 = 1;
	      }
	      else return false;
	    }
	      
	    return true;
	  }
	};   //MultisyncMessage
	
	inline bool MultisyncMessage::advance () 
	{
	  if (_phaseStatus == DONE)
	    return true;
	  
	  MultisyncState &barrier = _params->_statevec[_classroute];
	  unsigned iteration = barrier._iteration;
	  size_t status = _phaseStatus;
	  PAMI_assert (_classroute < NumClassRoutes);
	  PAMI_assert (iteration <= 1);
	  
	  size_t  counter_offset = (_classroute*NumTorusDims*2 + _phase*2 + iteration) * 8;
	  for (; _phase < NumTorusDims; ++_phase, counter_offset += 16) {
	    size_t dimsize = barrier._dimsizes[_phase];

	    //Nothing to do in this phase
	    if (unlikely (dimsize == 0) ) 
	      continue;
	    
	    size_t    fifo0 = _phase * 2;
	    size_t    fifo1 = _phase * 2 + 1;	      	      	    
	    if (likely(status == SENDING)) {		
	      //printf ("Injecting Descriptors to dest 0x%x, 0x%x and fifos %ld, %ld for iteration %d, class route %ld, dimsize %ld\n", 
	      //      barrier._hwdest[_phase][0], 
	      //      barrier._hwdest[_phase][1], 
	      //      fifo0,
	      //      fifo1,
	      //      iteration,
	      //      _classroute,
	      //      dimsize);
	      
	      bool done = injectDescriptors (barrier._hwdest[_phase][0], 
					     barrier._hwdest[_phase][1], 
					     barrier._sstatus[_phase][0], 
					     barrier._sstatus[_phase][1], 
					     fifo0,
					     fifo1,
					     counter_offset,
					     iteration);	      
	      
	      //Done injecting both descriptors
	      if (done) {
		status = WAITING;
		barrier._sstatus[_phase][0] = 0;
		barrier._sstatus[_phase][1] = 0;
		//printf ("Done injecting\n");
	      }
	      else break;
	    }
	    
	    if (status == WAITING) {		
	      volatile uint64_t *counter =(volatile uint64_t*)((char *)_params->_countervec + counter_offset);
	      size_t i = 100;
	      while ( (--i > 0) && (*counter != (CounterDefault - dimsize)) );
	      if (i <= 0)
		break;
	      
	      *counter = CounterDefault;
	      status = SENDING;  //Send the next phase
	    }
	  }
	  
	  _phaseStatus = status;
	  if (_phase  == NumTorusDims) {
	    _phaseStatus = DONE;
	    //	      printf ("barrier done\n");
	    if (_cb_done)
	      _cb_done (_params->_context, _cookie, PAMI_SUCCESS);
	    return true;
	  }	    
	  
	  return false;
	}       
      };  //--Rectangle
    };  //--MU
  };  //--Device
};  //--PAMI

#endif
