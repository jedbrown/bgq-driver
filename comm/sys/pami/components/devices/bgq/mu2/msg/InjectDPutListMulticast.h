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

#ifndef __components_devices_bgq_mu2_msg_InjectDPutListMulticast_h__
#define __components_devices_bgq_mu2_msg_InjectDPutListMulticast_h__

#include "components/devices/bgq/mu2/msg/InjectDPutBase.h"

#define NUM_FIFOS   10

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {      
      ///
      /// \brief Inject one or more descriptors into an inject fifo
      ///
      ///
      class InjectDPutListMulticast : public InjectDPutBase
      {
      public:	
	/// \brief Constructor
	/// \param[in] context the MU context for this message
	InjectDPutListMulticast (MU::Context &context):
	  InjectDPutBase(),
	  _context (context),
	  _nextdst (0),
	  _ndestinations(0),
	  //_processInjection (false),
	  _localMulticast(1),
	  _length(0),	
	  _pami_context(NULL),
	  _fn (NULL),
	  _cookie (NULL),
		_topology(NULL)
	    {
	      //Default constructor
	    }

	///
	/// \brief Inject descriptor(s) into a specific injection fifo
	///
	/// \param[in] fn  completion event fn
	/// \param[in] cookie callback cookie
	/// \param[in] me  my coordinates as an MUHWI_Destination_t 
	/// \param[in] ref  the ref of the axial topology
	/// \param[in] istorus istorus bits
	/// \param[in] ll  lower left of the axial
	/// \param[in] ur  upper right of the axial
	/// \param[in] pwq pipeworkqueue that has data to be consumed
	/// \param[in] length the totaly number of bytes to be transfered
	/// \param[in] localMultcast : should this message do local sends
	///
	void initialize (pami_context_t     context,
			 pami_event_function   fn,
			 void                * cookie,	    
			Topology             * topology,
			 PipeWorkQueue       * pwq,
			 uint64_t              length,
			 uint32_t              localMulticast) 
	{
	  TRACE_FN_ENTER();	 
	  _nextdst          = 0;
	  _ndestinations    = topology->size();
	  this->_processInjection = true;
	  this->_doneCompletion   = false;
	  _length           = length;	
	  this->_consumedBytes = 0;
	  this->_pwq           = pwq;
	  _pami_context     = context;
	  _fn               = fn;
	  _cookie           = cookie;
	  _topology         = topology;

	  if (_ndestinations > 1) 
	    for (unsigned fnum = 0; fnum < NUM_FIFOS; fnum++)
	      _lastCompletionSeqNo[fnum] = UNDEFINED_SEQ_NO;
	  else { //_ndestinations == 1
	    pami_task_t            rank = _topology->index2Rank(0);
	    bool islocal = false;
	    islocal = __global.mapping.isPeer(__global.mapping.task(), rank);
	    CCMI_assert(!islocal || localMulticast);
	    
	    MUHWI_Destination_t   dest;
	    uint64_t              map;
	    size_t                tcoord;
	    uint32_t              fnum;
	    fnum = _context.pinFifo (rank,
				     0,
				     dest,
				     tcoord,
				     map);
	    _fnum = fnum;
	    MUHWI_MessageUnitHeader_t *muh_model = &(_desc.PacketHeader.messageUnitHeader);
	    uint pid = muh_model->Packet_Types.Direct_Put.Rec_Payload_Base_Address_Id;
	    _cid=muh_model->Packet_Types.Direct_Put.Rec_Counter_Base_Address_Id;
	    //translate depending on destination rank
	    muh_model->Packet_Types.Direct_Put.Rec_Payload_Base_Address_Id = _context.pinBatId(tcoord, pid); 
	    //Model does not update cid with every message
	    _cid = _context.pinBatId(tcoord, _cid);  
	    
	    _desc.setDestination(dest);
	    _desc.Torus_FIFO_Map = map;    
	  }
	  
	  // Determine the physical address of the (temporary) payload
	  // buffer from the model state memory.
	  char *payload = (char*)pwq->bufferToConsume();
	  Kernel_MemoryRegion_t  memRegion;
	  uint32_t rc;
	  rc = Kernel_CreateMemoryRegion (&memRegion, payload, length);
	  PAMI_assert ( rc == 0 );
	  uint64_t paddr = (uint64_t)memRegion.BasePa +
	    ((uint64_t)payload - (uint64_t)memRegion.BaseVa);
	  
	  _desc.setPayload (paddr, 0); 
	  _desc.setRecPutOffset(0);
	  _localMulticast = localMulticast;
	  TRACE_FN_EXIT();
	};
	  
	inline ~InjectDPutListMulticast () {};

	inline bool advance_single() {
	  TRACE_FN_ENTER();
	  uint64_t sequence = 0;
	  uint64_t bytes_available = 0;

	  if (likely(this->_processInjection)) {
	    //Set fields in first iteration
	    bytes_available = this->_pwq->getBytesProduced();
	    if (bytes_available == this->_consumedBytes)
	      return false;	    	    
	    uint64_t newbytes    = bytes_available - this->_consumedBytes; 
	    register double fp0  FP_REGISTER(0);
	    register double fp1  FP_REGISTER(1);
	    VECTOR_LOAD_NU (&_desc,  0, fp0);
	    VECTOR_LOAD_NU (&_desc, 32, fp1);		  	    
		  
	    InjChannel & channel = _context.injectionGroup.channel[_fnum];
	    bool flag = channel.hasFreeSpaceWithUpdate ();	    
	    if (likely(flag)) {
	      MUSPI_DescriptorBase * d = (MUSPI_DescriptorBase *) channel.getNextDescriptor ();
	      MUHWI_MessageUnitHeader_t *muh_model = &(d->PacketHeader.messageUnitHeader);

	      VECTOR_STORE_NU (d,  0, fp0);
	      VECTOR_STORE_NU (d, 32, fp1);	
	      d->Message_Length = newbytes;
	      muh_model->Packet_Types.Direct_Put.Rec_Counter_Base_Address_Id=_cid;
	      sequence = channel.injFifoAdvanceDesc ();	      
	      _lastCompletionSeqNo[0] = sequence;
	    }
	    //No descriptor slots available, so come back and try later
	    else return false;	  
	    
	    if (bytes_available < _length) {
	      reset(bytes_available);	 
	      return false;
	    }
	  }
	  
	  this->_processInjection = false;
	  InjChannel & channel = _context.injectionGroup.channel[_fnum];
	  unsigned rc = channel.checkDescComplete(_lastCompletionSeqNo[0]); 
	  this->_doneCompletion = rc;	 
	  
	  if (rc && _fn)
	    _fn (_pami_context, _cookie, PAMI_SUCCESS);
	  
	  TRACE_FN_EXIT();
	  return this->_doneCompletion;
	}
	
	virtual bool advance ()
	{
	  //TRACE_FN_ENTER();
	  //printf("InjectDPutMulticast:  advance:  nextdst=%u ndst=%u\n",_nextdst, _ndestinations);

	  //Fast path
	  if (likely(_ndestinations == 1))
	    return advance_single();

	  uint64_t sequence = 0;
	  uint64_t bytes_available = 0;
	  unsigned done = 1;

	  if (likely(this->_processInjection)) {
	    //The is computed when the first descriptor for this round is injected
	    if (likely(_nextdst == 0)) {  //When nextdst == 0
	      bytes_available = this->_pwq->getBytesProduced(); //bytesAvailableToConsume();
	      if (bytes_available == this->_consumedBytes)
		return false;
	      
	      uint64_t newbytes    = bytes_available - this->_consumedBytes;
	      _desc.Message_Length = newbytes;
	    }
	    else 
	      bytes_available = this->_consumedBytes + _desc.Message_Length;  

	    if (_nextdst < _ndestinations) 
	    {
        register double fp0  FP_REGISTER(0);
        register double fp1  FP_REGISTER(1);
	      VECTOR_LOAD_NU (&_desc,  0, fp0);
	      VECTOR_LOAD_NU (&_desc, 32, fp1);		  	    

	      MUHWI_MessageUnitHeader_t *muh_model = &(_desc.PacketHeader.messageUnitHeader);
	      uint pid = muh_model->Packet_Types.Direct_Put.Rec_Payload_Base_Address_Id;
	      uint cid = muh_model->Packet_Types.Direct_Put.Rec_Counter_Base_Address_Id;	      
	      
	      do { 
		pami_task_t            rank = _topology->index2Rank(_nextdst);
		bool islocal = __global.mapping.isPeer(__global.mapping.task(), rank);
		if (!islocal || _localMulticast) {
		  MUHWI_Destination_t   dest;
		  uint64_t              map;
		  size_t                tcoord;
		  uint32_t              fnum;
		  fnum = _context.pinFifo (rank,
					   0,
					   dest,
					   tcoord,
					   map);
		  
		  InjChannel & channel = _context.injectionGroup.channel[fnum];
		  bool flag = channel.hasFreeSpaceWithUpdate ();
		  
		  if (likely(flag)) {
		    // Clone the message descriptors directly into the injection fifo.
		    MUSPI_DescriptorBase * d = (MUSPI_DescriptorBase *) channel.getNextDescriptor ();
		    //_desc.clone (*d);
		    VECTOR_STORE_NU (d,  0, fp0);
		    VECTOR_STORE_NU (d, 32, fp1);	
		    d->setDestination(dest);
		    d->Torus_FIFO_Map = map;
		    
		    MUHWI_MessageUnitHeader_t *muh_dput  = &(d->PacketHeader.messageUnitHeader);		    
		    muh_dput->Packet_Types.Direct_Put.Rec_Payload_Base_Address_Id = _context.pinBatId(tcoord, pid); 
		    muh_dput->Packet_Types.Direct_Put.Rec_Counter_Base_Address_Id = _context.pinBatId(tcoord, cid);
		    
		    sequence = channel.injFifoAdvanceDesc ();	      
		    _lastCompletionSeqNo[fnum] = sequence;
		  }
		  //No descriptor slots available, so come back and try later
		  else return false;
		}
		_nextdst++;
	      } while(_nextdst < _ndestinations);
	    }

	    if (bytes_available < _length) {
	      reset(bytes_available);	 
	      return false;
	    }
	  } //We have completed processing all injections
	  
	  this->_processInjection = false;
	  //printf ("Completed injection\n");

	  for (unsigned fnum = 0; fnum < NUM_FIFOS; fnum++) 	    
	    if (_lastCompletionSeqNo[fnum] != UNDEFINED_SEQ_NO) {
	      InjChannel & channel = _context.injectionGroup.channel[fnum];
	      unsigned rc = channel.checkDescComplete(_lastCompletionSeqNo[fnum]);
	      _lastCompletionSeqNo[fnum] |= seqno_table[rc];
	      done &=  rc;
	    }
	  this->_doneCompletion = done;	 

	  if (done && _fn) {
	    //printf ("Done completion \n");
	    _fn (_pami_context, _cookie, PAMI_SUCCESS);
	  }
	  
	  //TRACE_FN_EXIT();
	  return this->_doneCompletion;
	}

	///
	/// \brief Reset the internal state of the message
	///
	/// \note Only used for message reuse.
	///
	inline void reset (uint64_t bytes_available) { 
	  _nextdst = 0; 
	  uint64_t newbytes         = bytes_available - this->_consumedBytes; 
	  this->_consumedBytes      = bytes_available; 
	  _desc.setRecPutOffset(this->_consumedBytes);
	  _desc.Pa_Payload          = _desc.Pa_Payload + newbytes;	    
	}
	
	inline uint32_t ndestinations() { return _ndestinations; }

	static const size_t SC_MAXRANKS  = 128; // arbitrarily based on CCMI::Executor::ScheduleCache::SC_MAXRANKS

	//The descriptor is setup externally and contains batids, sndbuffer base and msg length
	MUSPI_DescriptorBase     _desc __attribute__((__aligned__(32))); 

      protected:	
	MU::Context            & _context;
	uint32_t                 _nextdst;
	uint32_t                 _ndestinations;
	//bool                     _processInjection;
	bool                     _localMulticast;
	uint64_t                 _length;        //Number of bytes to transfer
	pami_context_t           _pami_context;
	pami_event_function      _fn;
	void                   * _cookie;
	Topology               * _topology;
	uint32_t                 _fnum;
	uint16_t                 _cid;
	uint8_t                  _fifos[NUM_FIFOS];
	uint64_t                 _lastCompletionSeqNo[NUM_FIFOS];
      } __attribute__((__aligned__(32))); //class PAMI::Device::MU::InjectDPutListMulticast     

    };   // namespace PAMI::Device::MU                          
  };     // namespace PAMI::Device           
};       // namespace PAMI                                   

#endif // __components_devices_bgq_mu2_msg_InjectDPutListMulticast_h__                    
