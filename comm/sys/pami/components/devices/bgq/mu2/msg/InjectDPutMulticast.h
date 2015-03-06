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
 * \file components/devices/bgq/mu2/msg/InjectDPutMulticast.h
 * \brief ???
 */
#ifndef __components_devices_bgq_mu2_msg_InjectDPutMulticast_h__
#define __components_devices_bgq_mu2_msg_InjectDPutMulticast_h__

#include "components/devices/bgq/mu2/msg/InjectDPutBase.h"

#define LOCAL_DIM  5 //the index of the local dimension
#define MAX_CHANNELS  (2 * NumTorusDims)

#define MESH            PAMI::Interface::Mapping::Mesh
#define TORUS_POSITIVE  PAMI::Interface::Mapping::TorusPositive
#define TORUS_NEGATIVE  PAMI::Interface::Mapping::TorusNegative

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
      class InjectDPutMulticast : public InjectDPutBase
      {
      public:

	/// \brief Constructor
	/// \param[in] context the MU context for this message
	InjectDPutMulticast (MU::Context &context):
	_context (context),
	  _nextdst (0),
	  _ndestinations(0),
	  //_processInjection (false),
	  _localMulticast(0),
	  _startfnum(0),
	  _endfnum(0),  
	  _length(0),	
	  _pami_context(NULL),
	  _fn (NULL),
	  _cookie (NULL),
	  _localStart(0),
	  //_myLocal(0),
	  _localEnd(0),
	  _localIdx(0)
	    {
	      //Default constructor

              pami_coord_t dummy;
              __global.personality.jobRectangle(_jobLL, dummy);
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
			 MUHWI_Destination_t * me, 
			 pami_coord_t        * ref, 
			 unsigned char       * isTorus, 
			 pami_coord_t        * ll, 
			 pami_coord_t        * ur,
			 PipeWorkQueue       * pwq,
			 uint64_t              length,
			 uint32_t              localMulticast) 
	{
	  TRACE_FN_ENTER();	 
	  _nextdst          = 0;
	  _ndestinations    = 0;
	  this->_processInjection = true;
	  this->_doneCompletion   = false;
	  _localMulticast   = localMulticast;
	  _length           = length;	
	  this->_consumedBytes = 0;
	  this->_pwq        = pwq;
	  _pami_context     = context;
	  _fn               = fn;
	  _cookie           = cookie;
	  _startfnum        = 0;
	  _endfnum          = 0;  
	  _localStart       = 0;
	  _myLocal          = 0;
	  _localEnd         = 0;
	  _localIdx         = 0;
	      
	  setupDestinations (me, ref, isTorus, ll, ur);
	  if (localMulticast)
	    setupLocalDestinations (me, ref, isTorus, ll, ur);
	  
	  for (unsigned fnum = 0; fnum < 2*NumTorusDims; fnum++)
	    _lastCompletionSeqNo[fnum] = UNDEFINED_SEQ_NO;
	  
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
	  TRACE_FN_EXIT();
	};
	  
	inline ~InjectDPutMulticast () {};
	
	void setupLocalDestinations (MUHWI_Destination_t  * me, 
				     pami_coord_t         * ref, 
				     unsigned char        * isTorus, 
				     pami_coord_t         * ll, 
				     pami_coord_t         * ur)  
	{
	  //Assuming dimension 5 is the local dimension
	  _localStart = ll->u.n_torus.coords[LOCAL_DIM];
	  _localEnd   = ur->u.n_torus.coords[LOCAL_DIM] + 1;
	  _myLocal    = ref->u.n_torus.coords[LOCAL_DIM];
	  _localIdx   = _localStart;

	  _startfnum = 0;
	  _endfnum = MAX_CHANNELS - 1;
	  //printf ("Setting up local destinations %d %d %d\n", _localStart, _localEnd, _myLocal);
	}

	void setupDestinations(MUHWI_Destination_t    * me, 
			       pami_coord_t           * ref, 
			       unsigned char          * isTorus, 
			       pami_coord_t           * ll, 
			       pami_coord_t           * ur);
	
	void advanceLocal (unsigned bytes_available) __attribute__((noinline, weak));

	virtual bool advance ()
	{
	  //TRACE_FN_ENTER();	  
	  //TRACE_FORMAT("InjectDPutMulticast:  advance:  ndesc=%zu\n",ndesc);
	  uint64_t sequence = 0;
	  uint64_t bytes_available = 0;
	  unsigned done = 1;

	  if (likely(this->_processInjection)) {
	    //The is computed when the first descriptor for this round is injected
	    if (likely(_nextdst == 0 && _localIdx == _localStart)) {  //When nextdst == 0, localIdx == localStart
	      bytes_available = this->_pwq->getBytesProduced(); //_pwq->bytesAvailableToConsume();
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
	      
	      do { 
		size_t                fnum    = _fifos[_nextdst];
		uint64_t              map     = _mu_fifomaps[fnum];
		uint8_t               hints   = _mu_hintsABCD[fnum];
		uint8_t               hints_e = _mu_pt2ptmisc1_deposit[fnum];
		uint32_t              dest    = _destinations[_nextdst];
		
		InjChannel & channel = _context.injectionGroup.channel[fnum];
		bool flag = channel.hasFreeSpaceWithUpdate ();
		
		if (likely(flag)) {
		  // Clone the message descriptors directly into the injection fifo.
		  MUSPI_DescriptorBase * d = (MUSPI_DescriptorBase *) channel.getNextDescriptor ();	    
		  //_desc.clone (*d);
		  VECTOR_STORE_NU (d,  0, fp0);
		  VECTOR_STORE_NU (d, 32, fp1);	
		  d->setDestination(dest);
		  d->setHintsABCD(hints);
		  d->setPt2PtMisc1(hints_e);
		  d->Torus_FIFO_Map = map;
		  
		  sequence = channel.injFifoAdvanceDesc ();	      
		  _lastCompletionSeqNo[fnum] = sequence;
		}
		//No descriptor slots available, so come back and try later
		else return false;
		
		_nextdst++;
	      } while(_nextdst < _ndestinations);
	    }

	    if (unlikely(_localMulticast)) {
	      advanceLocal (bytes_available);	  
	      if (_localIdx != _localEnd)
		return false;
	    }
	  
	    if (bytes_available < _length) {
	      reset(bytes_available);	 
	      return false;
	    }
	  } //We have completed processing all injections
	  
	  this->_processInjection = false;
	  for (unsigned fnum = _startfnum; fnum <= _endfnum; fnum++) 	    
	    if (_lastCompletionSeqNo[fnum] != UNDEFINED_SEQ_NO) {
	      InjChannel & channel = _context.injectionGroup.channel[fnum];
	      unsigned rc = channel.checkDescComplete(_lastCompletionSeqNo[fnum]);	      
	      _lastCompletionSeqNo[fnum] |= seqno_table[rc];
	      done &=  rc;
	    }
	  this->_doneCompletion = done;
	  
	  if (done && _fn)
	    _fn (_pami_context, _cookie, PAMI_SUCCESS);
	  
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

	  _localIdx = _localStart; 
	}
	
	inline uint32_t ndestinations() { return _ndestinations; }

	//The descriptor is setup externally and contains batids, sndbuffer base and msg length
	MUSPI_DescriptorBase     _desc __attribute__((__aligned__(32))); 

      protected:	
	MU::Context            & _context;
	uint32_t                 _nextdst;
	uint32_t                 _ndestinations;
	//bool                     _processInjection;
	bool                     _localMulticast;
	uint32_t                 _startfnum;
	uint32_t                 _endfnum;
	uint64_t                 _length;        //Number of bytes to transfer
	pami_context_t           _pami_context;
	pami_event_function      _fn;
	void                   * _cookie;
	uint8_t                  _fifos[NumTorusDims*2];
	uint32_t                 _destinations[NumTorusDims*2];
	uint8_t                  _localStart;
	uint8_t                  _myLocal;
	uint8_t                  _localEnd;
	uint8_t                  _localIdx;
	uint64_t                 _lastCompletionSeqNo[NumTorusDims*2];
        pami_coord_t             _jobLL;
      } __attribute__((__aligned__(32))); // class     PAMI::Device::MU::InjectDPutMulticast     

      void InjectDPutMulticast::advanceLocal (unsigned bytes_available)
      {
	//advance the local descriptors
	unsigned                fnum    = 0;
	for ( ; _localIdx < _localEnd; _localIdx ++) {
	  if (_localIdx != _myLocal) { //skip the root
	    //uint64_t              map     =  (fnum & 0x1) ? MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0 : 
	    //MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1;
	    
	    uint64_t              map     = MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0 | MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1;
	    uint8_t               hints   = 0;
	    uint8_t               hints_e = MUHWI_PACKET_HINT_E_NONE | 
	      MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE | 
	      MUHWI_PACKET_USE_DETERMINISTIC_ROUTING | 
	      MUHWI_PACKET_DO_NOT_DEPOSIT;
	    MUHWI_Destination_t   dest    = *_context.getMuDestinationSelf();
	    
	    InjChannel & channel = _context.injectionGroup.channel[fnum];
	    bool flag = channel.hasFreeSpaceWithUpdate ();
	    
	    if (flag) {
	      // Clone the message descriptors directly into the injection fifo.
	      MUSPI_DescriptorBase * d = (MUSPI_DescriptorBase *) channel.getNextDescriptor ();
	      
	      _desc.clone (*d);
	      d->setDestination(dest);
	      d->setHintsABCD(hints);
	      d->setPt2PtMisc1(hints_e);
	      d->Torus_FIFO_Map = map;
	      
	      MUHWI_MessageUnitHeader_t *muh_model = &(_desc.PacketHeader.messageUnitHeader);
	      MUHWI_MessageUnitHeader_t *muh_dput  = &(d->PacketHeader.messageUnitHeader);
	      
	      uint pid = muh_model->Packet_Types.Direct_Put.Rec_Payload_Base_Address_Id;
	      uint cid = muh_model->Packet_Types.Direct_Put.Rec_Counter_Base_Address_Id;
	      muh_dput->Packet_Types.Direct_Put.Rec_Payload_Base_Address_Id = _context.pinBatId(_localIdx, pid); 
	      muh_dput->Packet_Types.Direct_Put.Rec_Counter_Base_Address_Id = _context.pinBatId(_localIdx, cid);
	      
	      uint64_t sequence = channel.injFifoAdvanceDesc ();		
	      _lastCompletionSeqNo [fnum] = sequence;		  

	      fnum ++;
	      if (fnum == MAX_CHANNELS)
		fnum = 0;
	    }
	    else 
	      return;
	  }
	}
      }

      inline void InjectDPutMulticast::setupDestinations(MUHWI_Destination_t    * me, 
							 pami_coord_t           * ref, 
							 unsigned char          * isTorus, 
							 pami_coord_t           * ll, 
							 pami_coord_t           * ur)
      {
	uint32_t i;
	uint32_t ndest  = 0; 
	uint32_t myaddr = me->Destination.Destination;
	
	//Process dimensions A B C D E
	for (i = 0; i < NumTorusDims; i++ ){ 
	  //The length of this dimension is 1
	  if (ur->u.n_torus.coords[i] == ll->u.n_torus.coords[i])
	    continue;
	  
	  int   nidx = 2*i;
	  int   pidx = 2*i+1;
	  int   dstidx = (NumTorusDims - i - 1) * 6;
	  uint32_t dest = (myaddr & ~(0x3f << dstidx));
	  
	  if (isTorus[i] == MESH) //Mesh
	    {
	      //positive direction
	      if (ur->u.n_torus.coords[i] != ref->u.n_torus.coords[i]) {
		//printf ("HERE %d %ld \n\n", i, ur->u.n_torus.coords[i]);
		_destinations[ndest] = dest | ((ur->u.n_torus.coords[i] + _jobLL.u.n_torus.coords[i]) << dstidx);
		_fifos[ndest] = pidx;
		ndest ++;
	      }		
	      
	      //negative direction
	      if (ll->u.n_torus.coords[i] != ref->u.n_torus.coords[i]) {
		_destinations[ndest] = dest | ((ll->u.n_torus.coords[i] + _jobLL.u.n_torus.coords[i]) << dstidx);
		_fifos[ndest] = nidx;
		ndest ++;
	      }
	    }                   
	  else if (isTorus[i] == TORUS_POSITIVE)
	    {
	      if (ref->u.n_torus.coords[i] == ll->u.n_torus.coords[i])
		_destinations[ndest] = dest | (ur->u.n_torus.coords[i] << dstidx);
	      else
		_destinations[ndest] = dest | ((ref->u.n_torus.coords[i]-1 + _jobLL.u.n_torus.coords[i]) << dstidx);
	      _fifos[ndest] = pidx;
	      ndest ++;
	    }
	  else if (isTorus[i] == TORUS_NEGATIVE)
	    {
	      if (ref->u.n_torus.coords[i] == ur->u.n_torus.coords[i]) 
		_destinations[ndest] = dest | (ll->u.n_torus.coords[i] << dstidx);
	      else
		_destinations[ndest] = dest | ((ref->u.n_torus.coords[i]+1 + _jobLL.u.n_torus.coords[i]) << dstidx);
	      _fifos[ndest] = nidx;
	      ndest ++;
	    }
	  //else PAMI_abort();
	}
      
	_ndestinations = ndest;
	
	if (ndest > 0) {
	  _startfnum = _fifos[0];
	  _endfnum = _fifos[0];
	  for (i = 1; i < ndest; i++) {
	    if (_fifos[i] < _startfnum)
	      _startfnum = _fifos[i];
	    if (_fifos[i] > _endfnum)
	      _endfnum = _fifos[i];
	  }
	}
	
#if 0
	MUHWI_Destination mudest;
	mudest.Destination.Destination = _destinations[ndest];
	for (i = 0; i < ndest; i++)
	  printf ("(%d,%d,%d,%d,%d) Sending to 0x%x (%d,%d,%d,%d,%d) direction %d\n", 
		  me->Destination.A_Destination,
		  me->Destination.B_Destination,
		  me->Destination.C_Destination,
		  me->Destination.D_Destination,
		  me->Destination.E_Destination,
		  _destinations[i].Destination.Destination,
		    _destinations[i].Destination.A_Destination,
		  _destinations[i].Destination.B_Destination,
		  _destinations[i].Destination.C_Destination,
		  _destinations[i].Destination.D_Destination,
		  _destinations[i].Destination.E_Destination,
		  (_fifos[i] & 0x1)
		  );
#endif
      }  
      
      

    };   // namespace PAMI::Device::MU                          
  };     // namespace PAMI::Device           
};       // namespace PAMI                                   

#endif // __components_devices_bgq_mu2_msg_InjectDPutMulticast_h__                     
