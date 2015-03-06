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
 * \file components/devices/bgq/mu2/model/RectangleMultisyncModel.h
 * \brief ???
 */
#ifndef __components_devices_bgq_mu2_model_RectangleMultisyncModel_h__
#define __components_devices_bgq_mu2_model_RectangleMultisyncModel_h__

#include "components/devices/bgq/mu2/Context.h"
#include "sys/pami.h"
#include "components/devices/MultisyncModel.h"
#include "components/devices/bgq/mu2/MU_Util.h"
#include "components/devices/bgq/mu2/msg/RectangleMultisyncMessage.h"

#define MU_RECTANGLE_BLOCKING_BARRIER   0

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      namespace Rectangle
      {
	class MultisyncModel : public Interface::MultisyncModel<MultisyncModel, MU::Context, sizeof(MultisyncMessage)>
	{
	public:
	  class CounterState {
	  public:
	    bool                     _initialized;
	    uint16_t                 _pbatid;
	    uint16_t                 _cbatid;	 
	    MultisyncParams        * _params;
	    uint64_t                 _payload;
	    uint64_t                 _crbitmap;

	    ///
	    /// \brief CounterState constructor
	    ////
	    CounterState() : 
	      _initialized(false),
	      _pbatid((unsigned short)-1),
	      _cbatid((unsigned short)-1),
	      _params(NULL),
	      _payload(0),  //Payload of the put
	      _crbitmap (0xFFFFFFFFFFFFFFFFUL)  //Reserve the first two bits
	      {}

	    pami_result_t  initialize(pami_context_t    context,
				      MU::Context     & mucontext) 
	    {
	      if (_initialized)
		return PAMI_SUCCESS;

	      //Test global allreduce to allocate ids
	      //_counterstate._crbitmap&=~(0x1<<(__global.mapping.task()+1)); 

	      //Allocate params to a 32b aligned location
	      __global.heap_mm->memalign ((void**)&_params, 64, sizeof(MultisyncParams));
	      memset(_params, 0, sizeof(MultisyncParams));
	      new (_params) MultisyncParams (mucontext, context);
	      
	      //Set all bits to 1 enabling counters to have all bits 1
	      memset ((void *)_params->_countervec, 0xff, NumClassRoutes * NumTorusDims * 2 * sizeof(uint64_t));
	      
	      //Allocate bat to map counters
	      ///// Get the BAT IDS ///////////////
	      //// Setup CounterVec in BAT 
	      int32_t rcBAT = 0;
	      rcBAT = mucontext.allocateBatIds (1, &_pbatid);  
	      PAMI_assert (rcBAT != -1);
	      if (rcBAT == -1)
		return PAMI_ERROR;
	      
	      rcBAT = mucontext.allocateBatIds (1, &_cbatid);  
	      PAMI_assert (rcBAT != -1);
	      if (rcBAT == -1)
		return PAMI_ERROR;
	      
	      int rc = 0;
	      Kernel_MemoryRegion_t memRegion;
	      rc = Kernel_CreateMemoryRegion (&memRegion, (void *)_params->_countervec, sizeof(uint64_t) * NumTorusDims * NumClassRoutes * 2);
	      if (rc != 0)
		return PAMI_ERROR;

	      uint64_t paddr = (uint64_t)memRegion.BasePa +
		((uint64_t)(void *)_params->_countervec - 
		 (uint64_t)memRegion.BaseVa);	      
	      uint64_t atomic_address = 
		MUSPI_GetAtomicAddress(paddr, MUHWI_ATOMIC_OPCODE_STORE_ADD);
	      mucontext.setBatEntry (_cbatid, atomic_address);	    
	      
	      rc = Kernel_CreateMemoryRegion (&memRegion, (void *)&_payload, 
					      sizeof(size_t));
	      if (rc != 0)
		return PAMI_ERROR;

	      paddr = (uint64_t)memRegion.BasePa + 
		((uint64_t)(void *)&_payload - (uint64_t)memRegion.BaseVa); 

	      mucontext.setBatEntry (_pbatid, paddr);  	

	      buildP2pDputModelDescriptor(_params->_modeldesc,_pbatid,_cbatid);
	      _params->_modeldesc.setDeposit (MUHWI_PACKET_DEPOSIT);   	      
              _params->_modeldesc.setPayload (paddr, 1);

	      _initialized = true;
	      return PAMI_SUCCESS;
	    }
	  };


	public:
	  static const size_t   sizeof_msg      = sizeof(MultisyncMessage);
	  
	  MultisyncModel (pami_client_t     client,
			  pami_context_t    context,
			  MU::Context     & mucontext,
			  pami_result_t   & status):
	  Interface::MultisyncModel<MultisyncModel, MU::Context, sizeof(MultisyncMessage)> (mucontext, status),
	  _params(NULL)
	    {
	      COMPILE_TIME_ASSERT (NumClassRoutes <= sizeof(unsigned long) * 8);
	      
	      status = _counterstate.initialize(context, mucontext);	      
	      _params = _counterstate._params;
              pami_coord_t dummy;
              __global.personality.jobRectangle(_jobLL, dummy);
	    } 
	  
	  uint64_t getAllocationVector () { return _counterstate._crbitmap; }
	  
	  ///
	  /// \brief Once a globally unique id has been allocated the
	  ///        MultisyncState must be configured
	  ///
	  pami_result_t  configureClassRoute (size_t       id,
					      Topology   * topology) __attribute__((noinline, weak));
	  
	  /// \see PAMI::Device::Interface::MultisyncModel::postMultisync
	  pami_result_t postMultisyncImmediate_impl(size_t            client,
						    size_t            context, 
						    pami_multisync_t *msync,
						    void             *devinfo = NULL) 
	  {
	    return PAMI_ERROR;
	  }
	  

	  /// \see PAMI::Device::Interface::MultisyncModel::postMultisync
	  pami_result_t postMultisync_impl(uint8_t (&state)[sizeof_msg],
					   size_t             client,
					   size_t             context, 
					   pami_multisync_t * msync,
					   void             * devinfo = NULL) 
	  {
	    size_t classroute = 0;
	    if (devinfo) 
	      classroute = (size_t)devinfo - 1;
	    
	    //printf ("Starting collective on class route %ld, devinfo %ld\n", classroute, (size_t)devinfo);
	    if ((_counterstate._crbitmap & (0x1 << classroute)) != 0) {
	      printf ("Call configure class route\n");
	      configureClassRoute (classroute, (Topology*)msync->participants);
	    }
	    
	    MultisyncMessage *msg = new (state) MultisyncMessage (_params,
								  msync->cb_done.function, 
								  msync->cb_done.clientdata, 
								  classroute);
	    msg->init();
#if MU_RECTANGLE_BLOCKING_BARRIER 
	    while (!msg->advance());
#else
	    for (size_t i = 0; i < 64; ++i)
	      if (msg->advance()) 
		return PAMI_SUCCESS;
	    
	    new (&msg->_work) PAMI::Device::Generic::GenericThread(advance, msg);
	    PAMI::Device::Generic::GenericThread *work = (PAMI::Device::Generic::GenericThread *)&msg->_work;
	    _params->_mucontext.getProgressDevice()->postThread(work);
#endif
	    return PAMI_SUCCESS;
	  }
	  
	  static pami_result_t advance (pami_context_t     context,
					void             * cookie)
	  {
	    MultisyncMessage *msg = (MultisyncMessage *) cookie;	  	  
	    for (size_t i = 0; i < 16; ++i)
	      if (msg->advance()) 
		return PAMI_SUCCESS;
	    
	    return PAMI_EAGAIN;
	  }
	  
	protected:
	  MultisyncParams          * _params;
          pami_coord_t             _jobLL;

	  static CounterState        _counterstate;
	};
	
	///
	/// \brief Once a globally unique id has been allocated the
	///        MultisyncState must be configured
	///
	pami_result_t  MultisyncModel::configureClassRoute (size_t       id,
							    Topology   * topology) 
	{
    if ( (_counterstate._crbitmap & (0x1 << id)) == 0 )
      return PAMI_ERROR;

	  PAMI_assert (_params->_statevec != NULL);
	  _counterstate._crbitmap &= ~(0x1 << id);  
	  memset(&_params->_statevec[id], 0, sizeof(MultisyncState));	  
	  
	  //Build list of destinations to send despost bit packets to
	  pami_coord_t *ll, *ur, self;
	  unsigned char *istorus;
	  int rc  = 0;
	  Topology rect = *topology;
	  if (rect.type() != PAMI_COORD_TOPOLOGY)
	    rc = rect.convertTopology (PAMI_COORD_TOPOLOGY);
	  PAMI_assert (rect.type() == PAMI_COORD_TOPOLOGY);
	  rc = rect.rectSeg(&ll, &ur, &istorus);
	  PAMI_assert (rc == 0);
	  
	  __global.mapping.task2network(__global.mapping.task(), &self, PAMI_N_TORUS_NETWORK);

	  MUHWI_Destination_t me = *_params->_mucontext.getMuDestinationSelf();
	  size_t  i;
	  uint32_t myaddr = me.Destination.Destination;	

	  //Process dimensions A B C D E
	  for (i = 0; i < NumTorusDims; ++i ){ 	   
	    //We only support contiguous rectangles. We don't support wrapped rectangles
	    PAMI_assert (ur->u.n_torus.coords[i] >= ll->u.n_torus.coords[i]);

	    uint32_t *destinations = _params->_statevec[id]._hwdest[i];
	    destinations[0] = UndefinedDest;
	    destinations[1] = UndefinedDest;
	    _params->_statevec[id]._dimsizes[i] = ur->u.n_torus.coords[i] - ll->u.n_torus.coords[i];
	    //The length of this dimension is 1
	    if (ur->u.n_torus.coords[i] == ll->u.n_torus.coords[i])
	      continue;
	  
	    uint32_t dstidx = (NumTorusDims - i - 1) * 6;
	    uint32_t dest = (myaddr & ~(0x3f << dstidx));	  

	    if (!istorus[i]) //Mesh
	    {
	      //positive direction
	      if (ur->u.n_torus.coords[i] != self.u.n_torus.coords[i]) 
		destinations[1] = dest | ((ur->u.n_torus.coords[i] + _jobLL.u.n_torus.coords[i]) << dstidx);
	      
	      //negative direction
	      if (ll->u.n_torus.coords[i] != self.u.n_torus.coords[i]) 
		destinations[0] = dest | ((ll->u.n_torus.coords[i] + _jobLL.u.n_torus.coords[i]) << dstidx);
	    }                   
	    else //Send along +ve dir
	    {
	      if (self.u.n_torus.coords[i] == ll->u.n_torus.coords[i])
		destinations[1] = dest | ((ur->u.n_torus.coords[i] + _jobLL.u.n_torus.coords[i]) << dstidx);
	      else {
		PAMI_assert (self.u.n_torus.coords[i] >= 1);
		destinations[1] = dest | ((self.u.n_torus.coords[i]-1 + _jobLL.u.n_torus.coords[i]) << dstidx);
	      }
	    }
	  }	  

	  return PAMI_SUCCESS;
	}
      
      };  //--MultisyncModel
    };  //--Rectangle
  };  //--MU
};  //--PAMI
#endif
