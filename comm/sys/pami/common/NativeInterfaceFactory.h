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

#ifndef __NativeInterfaceFactoryCommon__
#define __NativeInterfaceFactoryCommon__

#include "util/trace.h"
#include "algorithms/interfaces/NativeInterface.h"
#include "algorithms/interfaces/NativeInterfaceFactory.h"

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#ifdef CCMI_TRACE_ALL
#define DO_TRACE_ENTEREXIT 1
#define DO_TRACE_DEBUG     1
#else
#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0
#endif

namespace PAMI {
  namespace NativeInterfaceCommon { // Common constants

    template <class T_Allocator, class T_NIAM, class T_NIAS, class T_Protocol, class T_Device>
      class NativeInterfaceFactory : public CCMI::Interfaces::NativeInterfaceFactory {
    protected:
      pami_client_t       _client;
      pami_context_t      _context;
      size_t              _client_id;	
      size_t              _context_id;
      T_Device          & _device;
      T_Allocator       & _allocator;

    public:
      
      NativeInterfaceFactory ( pami_client_t       client,
			       pami_context_t      context,
			       size_t              clientid,
			       size_t              contextid,
			       T_Device          & device,
			       T_Allocator       & allocator) : 
      CCMI::Interfaces::NativeInterfaceFactory(),
	_client (client),
	_context (context),
	_client_id  (clientid),
	_context_id (contextid),
	_device(device),
	_allocator(allocator)
	{	
	  COMPILE_TIME_ASSERT(sizeof(T_NIAM) <= T_Allocator::objsize);
	  COMPILE_TIME_ASSERT(sizeof(T_NIAS) <= T_Allocator::objsize);
		TRACE_FN_ENTER();
		TRACE_FORMAT("Allocator:  sizeof(T_NIAM) %zu, T_Allocator::objsize %zu\n",sizeof(T_NIAM),T_Allocator::objsize);
		TRACE_FORMAT("Allocator:  sizeof(T_NIAS) %zu, T_Allocator::objsize %zu\n",sizeof(T_NIAS),T_Allocator::objsize);
		TRACE_FN_EXIT();
	}
      
	virtual pami_result_t analyze(size_t context_id, pami_topology_t *topology, int phase, int* flag){return PAMI_SUCCESS;};

      /// \brief Construct a P2p Native Interface
      /// \details
      ///
      ///  A native interface is constructed.
      ///
      ///  Then a P2P protocol is constructed from the device and using the
      ///  same dispatch id and using the native interface's dispatch function
      ///  and the native interface as a cookie.
      ///
      ///  Finally, the P2P protocol is set into the native interface.
      ///
      virtual pami_result_t generate (int                                                        *   dispatch_id,
				      CCMI::Interfaces::NativeInterfaceFactory::NISelect             ni_select,
				      CCMI::Interfaces::NativeInterfaceFactory::NIType               ni_type,
				      size_t                                     nconnections,
				      CCMI::Interfaces::NativeInterface                          *&  ni)
      {
	//TRACE_FN_ENTER();
	pami_result_t result = PAMI_ERROR;
	
	// Construct the protocol(s) using the NI dispatch function and cookie
	COMPILE_TIME_ASSERT(sizeof(T_NIAS) <= T_Allocator::objsize);
	COMPILE_TIME_ASSERT(sizeof(T_NIAM) <= T_Allocator::objsize);
	ni = (CCMI::Interfaces::NativeInterface *) _allocator.allocateObject ();
	
	if (ni_type == CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED)
	  new ((void*)ni) T_NIAS(_client, _context, _context_id, _client_id);
	else
	  new ((void*)ni) T_NIAM(_client, _context, _context_id, _client_id);

	pami_endpoint_t            origin   = PAMI_ENDPOINT_INIT(_client_id, __global.mapping.task(), _context_id);
	
	pami_dispatch_p2p_function fn;
	size_t                     dispatch;
	T_Protocol                *protocol;       

	if( ni_select == CCMI::Interfaces::NativeInterfaceFactory::ALL || 
	    ni_select == CCMI::Interfaces::NativeInterfaceFactory::MULTICAST )
	  {
	    // Construct the mcast protocol using the NI dispatch function and cookie
	    if (ni_type == CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED)
	      fn = T_NIAS::dispatch_mcast;
	    else
	      fn = T_NIAM::dispatch_mcast;
	    dispatch = (*dispatch_id)--;

	    protocol = (T_Protocol*) T_Protocol::generate(dispatch,
							  fn,
							  (void*) ni,
							  _device,
							  origin,
							  _context,
							  (pami_dispatch_hint_t){0},
							  __global.heap_mm,
							  result);
        protocol->setPWQAllocator(ni->getSendPWQAllocator());
	    if (ni_type == CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED)
	      ((T_NIAS *)ni)->setMcastProtocol(dispatch, protocol);
	    else 
	      ((T_NIAM *)ni)->setMcastProtocol(dispatch, protocol);
	  }
	
	if( ni_select == CCMI::Interfaces::NativeInterfaceFactory::ALL || 
	    ni_select == CCMI::Interfaces::NativeInterfaceFactory::MANYTOMANY )
	  {
	    // Construct the m2m protocol using the NI dispatch function and cookie
	    dispatch = (*dispatch_id)--;
	    if (ni_type == CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED)
	      fn = T_NIAS::dispatch_m2m;
	    else
	      fn = T_NIAM::dispatch_m2m;
	    protocol  = (T_Protocol*) T_Protocol::generate(dispatch,
							   fn,
							   (void*) ni,
							   _device,
							   origin,
							   _context,
							   (pami_dispatch_hint_t){0},
							   __global.heap_mm,
							   result);
        protocol->setPWQAllocator(ni->getSendPWQAllocator());
	    if (ni_type == CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED)
	      ((T_NIAS *)ni)->setM2mProtocol(dispatch, protocol);
	    else 
	      ((T_NIAM *)ni)->setM2mProtocol(dispatch, protocol);
	  }
	
	if( ni_select == CCMI::Interfaces::NativeInterfaceFactory::ALL || 
	    ni_select == CCMI::Interfaces::NativeInterfaceFactory::P2P )
	  {
	    // Construct the p2p protocol using the NI dispatch function and cookie
	    dispatch = (*dispatch_id)--;
	    if (ni_type == CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED)
	      fn = T_NIAS::dispatch_send;
	    else
	      fn = T_NIAM::dispatch_send;

	    protocol  = (T_Protocol*) T_Protocol::generate(dispatch,
							   fn,
							   (void*) ni,
							   _device,
							   origin,
							   _context,
							   (pami_dispatch_hint_t){0},
							   __global.heap_mm,
							   result);
        protocol->setPWQAllocator(ni->getSendPWQAllocator());
	    if (ni_type == CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED)
	      ((T_NIAS *)ni)->setSendProtocol(dispatch, protocol);
	    else 
	      ((T_NIAM *)ni)->setSendProtocol(dispatch, protocol);

	    // Construct the p2p pwq protocol using the NI dispatch function and cookie
	    dispatch = (*dispatch_id)--;
	    if (ni_type == CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED)
	      fn = T_NIAS::dispatch_send_pwq;
	    else
	      fn = T_NIAM::dispatch_send_pwq;
	    protocol  = (T_Protocol*) T_Protocol::generate(dispatch,
							   fn,
							   (void*) ni,
							   _device,
							   origin,							   
							   _context,
							   (pami_dispatch_hint_t){0},
							   __global.heap_mm,
							   result);
	    //ni->setSendPWQProtocol(dispatch, protocol);
        protocol->setPWQAllocator(ni->getSendPWQAllocator());
	    if (ni_type == CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED)
	      ((T_NIAS *)ni)->setSendPWQProtocol(dispatch, protocol);
	    else 
	      ((T_NIAM *)ni)->setSendPWQProtocol(dispatch, protocol);
	  }
	
	// Return
	//TRACE_FN_EXIT();
	return result;
      }
    };
    
    
    template <class T_Allocator, class T_NIAM, class T_NIAS, class T_Protocol1, class T_Device1, class T_Protocol2, class T_Device2>
      class NativeInterfaceFactory2Device : public CCMI::Interfaces::NativeInterfaceFactory {
      
    protected:
      pami_client_t       _client;
      pami_context_t      _context;
      size_t              _client_id;	
      size_t              _context_id;
      T_Device1         & _device1;
      T_Device2         & _device2;
      T_Allocator       & _allocator;

    public:
      
      NativeInterfaceFactory2Device ( pami_client_t       client,
				      pami_context_t      context,
				      size_t              clientid,
				      size_t              contextid,
				      T_Device1         & device1,
				      T_Device2         & device2,
				      T_Allocator       & allocator ):
	CCMI::Interfaces::NativeInterfaceFactory(),
	_client (client),
	_context (context),
	_client_id  (clientid),
	_context_id (contextid),
	_device1 (device1),
	_device2 (device2),
	_allocator(allocator)
	{	
	  COMPILE_TIME_ASSERT(sizeof(T_NIAM) <= T_Allocator::objsize);
	  COMPILE_TIME_ASSERT(sizeof(T_NIAS) <= T_Allocator::objsize);
		TRACE_FN_ENTER();
		TRACE_FORMAT("Allocator:  sizeof(T_NIAM) %zu, T_Allocator::objsize %zu\n",sizeof(T_NIAM),T_Allocator::objsize);
		TRACE_FORMAT("Allocator:  sizeof(T_NIAS) %zu, T_Allocator::objsize %zu\n",sizeof(T_NIAS),T_Allocator::objsize);
		TRACE_FN_EXIT();
	}
      
	virtual pami_result_t analyze(size_t context_id, pami_topology_t *topo, int phase, int* flag){return PAMI_SUCCESS;};
      
      /// \brief Construct a P2p Native Interface
      /// \details
      ///
      ///  A native interface is constructed.
      ///
      ///  Then a P2P protocol is constructed from the device and using the
      ///  same dispatch id and using the native interface's dispatch function
      ///  and the native interface as a cookie.
      ///
      ///  Finally, the P2P protocol is set into the native interface.
      ///
      virtual pami_result_t generate (int                                *   dispatch_id,
				      CCMI::Interfaces::NativeInterfaceFactory::NISelect             ni_select,
				      CCMI::Interfaces::NativeInterfaceFactory::NIType               ni_type,
				      size_t                                                         nconnections,
				      CCMI::Interfaces::NativeInterface                          *&  ni)
      {
	//TRACE_FN_ENTER();
	pami_result_t result = PAMI_ERROR;
	

	// Get storage for the NI and construct it.
	COMPILE_TIME_ASSERT(sizeof(T_NIAS) <= T_Allocator::objsize);
	COMPILE_TIME_ASSERT(sizeof(T_NIAM) <= T_Allocator::objsize);
	ni = (CCMI::Interfaces::NativeInterface *) _allocator.allocateObject ();
	if (ni_type == CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED)
	  new ((void*)ni) T_NIAS(_client, _context, _context_id, _client_id);
	else
	  new ((void*)ni) T_NIAM(_client, _context, _context_id, _client_id);
	
	pami_endpoint_t origin = PAMI_ENDPOINT_INIT(_client_id, __global.mapping.task(), _context_id);
	
	// Construct the protocols using the NI dispatch function and cookie
	pami_dispatch_p2p_function fn;
	size_t                     dispatch;
	T_Protocol1               *protocol1;
	T_Protocol2               *protocol2;
	Protocol::Send::SendPWQ<Protocol::Send::Send>* composite;	

	if( ni_select == CCMI::Interfaces::NativeInterfaceFactory::ALL || 
	    ni_select == CCMI::Interfaces::NativeInterfaceFactory::MULTICAST )
	{
	  // Construct the mcast protocol using the NI dispatch function and cookie
	  if (ni_type == CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED)
	    fn = T_NIAS::dispatch_mcast;
	  else
	    fn = T_NIAM::dispatch_mcast;
	  dispatch = (*dispatch_id)--;
	  protocol1 = (T_Protocol1*) T_Protocol1::generate(dispatch,
							   fn,
							   (void*) ni,
							   _device1,
							   origin,
							   _context,
							   (pami_dispatch_hint_t){0},
							   __global.heap_mm,
							   result);
      protocol1->setPWQAllocator(ni->getSendPWQAllocator());
	  protocol2 = (T_Protocol2*) T_Protocol2::generate(dispatch,
							   fn,
							   (void*) ni,
							   _device2,
							   origin,
							   _context,
							   (pami_dispatch_hint_t){0},
							   __global.heap_mm,
							   result);
      protocol2->setPWQAllocator(ni->getSendPWQAllocator());
	  // Construct the composite from the two protocols
	  composite = (Protocol::Send::SendPWQ<Protocol::Send::Send>*)
	    Protocol::Send::Factory::generate(protocol1,
					      protocol2,
					      __global.heap_mm,
					      result);
	  // Set the composite protocol into the NI
	  //ni->setMcastProtocol(dispatch, composite);
      composite->setPWQAllocator(ni->getSendPWQAllocator());
	  if (ni_type == CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED)
	    ((T_NIAS *)ni)->setMcastProtocol(dispatch, composite);
	  else 
	    ((T_NIAM *)ni)->setMcastProtocol(dispatch, composite);
	}
	
	if( ni_select == CCMI::Interfaces::NativeInterfaceFactory::ALL || 
	    ni_select == CCMI::Interfaces::NativeInterfaceFactory::MANYTOMANY )
	{
	  // Construct the m2m protocol using the NI dispatch function and cookie
	  dispatch = (*dispatch_id)--;
	  if (ni_type == CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED)
	    fn = T_NIAS::dispatch_m2m;
	  else
	    fn = T_NIAM::dispatch_m2m;
	  protocol1 = (T_Protocol1*) T_Protocol1::generate(dispatch,
							   fn,
							   (void*) ni,
							   _device1,
							   origin,
							   _context,
							   (pami_dispatch_hint_t){0},
							   __global.heap_mm,
							   result);
      protocol1->setPWQAllocator(ni->getSendPWQAllocator());
	  protocol2 = (T_Protocol2*) T_Protocol2::generate(dispatch,
							   fn,
							   (void*) ni,
							   _device2,
							   origin,
							   _context,
							   (pami_dispatch_hint_t){0},
							   __global.heap_mm,
							   result);
      protocol2->setPWQAllocator(ni->getSendPWQAllocator());
	  // Construct the composite from the two protocols
	  composite = (Protocol::Send::SendPWQ<Protocol::Send::Send>*)
	    Protocol::Send::Factory::generate(protocol1,
					      protocol2,
					      __global.heap_mm,
					      result);
	  // Set the composite protocol into the NI
	  //ni->setM2mProtocol(dispatch, composite);
      composite->setPWQAllocator(ni->getSendPWQAllocator());
	  if (ni_type == CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED)
	    ((T_NIAS *)ni)->setM2mProtocol(dispatch, composite);
	  else 
	    ((T_NIAM *)ni)->setM2mProtocol(dispatch, composite);
	}
	
	if( ni_select == CCMI::Interfaces::NativeInterfaceFactory::ALL || 
	    ni_select == CCMI::Interfaces::NativeInterfaceFactory::P2P )
	{
	  // Construct the p2p protocol using the NI dispatch function and cookie
	  dispatch = (*dispatch_id)--;
	  if (ni_type == CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED)
	    fn = T_NIAS::dispatch_send;
	  else
	    fn = T_NIAM::dispatch_send;
	  protocol1 = (T_Protocol1*) T_Protocol1::generate(dispatch,
							   fn,
							   (void*) ni,
							   _device1,
							   origin,
							   _context,
							   (pami_dispatch_hint_t){0},
							   __global.heap_mm,
							   result);
      protocol1->setPWQAllocator(ni->getSendPWQAllocator());
	  protocol2 = (T_Protocol2*) T_Protocol2::generate(dispatch,
							   fn,
							   (void*) ni,
							   _device2,
							   origin,
							   _context,
							   (pami_dispatch_hint_t){0},
							   __global.heap_mm,
							   result);
      protocol2->setPWQAllocator(ni->getSendPWQAllocator());
	  // Construct the composite from the two protocols
	  composite = (Protocol::Send::SendPWQ<Protocol::Send::Send>*)
	    Protocol::Send::Factory::generate(protocol1,
					      protocol2,
					      __global.heap_mm,
					      result);
      composite->setPWQAllocator(ni->getSendPWQAllocator());
	  // Set the composite protocol into the NI
	  //ni->setSendProtocol(dispatch, composite);	  	
	  if (ni_type == CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED)
	    ((T_NIAS *)ni)->setSendProtocol(dispatch, composite);
	  else 
	    ((T_NIAM *)ni)->setSendProtocol(dispatch, composite);
	  
	  // Construct the p2p pwq protocol using the NI dispatch function and cookie
	  dispatch = (*dispatch_id)--;
	  if (ni_type == CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED)
	    fn = T_NIAS::dispatch_send_pwq;
	  else
	    fn = T_NIAM::dispatch_send_pwq;
	  protocol1 = (T_Protocol1*) T_Protocol1::generate(dispatch,
							   fn,
							   (void*) ni,
							   _device1,
							   origin,
							   _context,
							   (pami_dispatch_hint_t){0},
							   __global.heap_mm,
							   result);
      protocol1->setPWQAllocator(ni->getSendPWQAllocator());
	  protocol2 = (T_Protocol2*) T_Protocol2::generate(dispatch,
							   fn,
							   (void*) ni,
							   _device2,
							   origin,
							   _context,
							   (pami_dispatch_hint_t){0},
							   __global.heap_mm,
							   result);
      protocol2->setPWQAllocator(ni->getSendPWQAllocator());
	  // Construct the composite from the two protocols
	  composite = (Protocol::Send::SendPWQ<Protocol::Send::Send>*)
	    Protocol::Send::Factory::generate(protocol1,
					      protocol2,
					      __global.heap_mm,
					      result);
      composite->setPWQAllocator(ni->getSendPWQAllocator());
	  // Set the composite protocol into the NI
	  //ni->setSendPWQProtocol(dispatch, composite);
	  if (ni_type == CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED)
	    ((T_NIAS *)ni)->setSendPWQProtocol(dispatch, composite);
	  else 
	    ((T_NIAM *)ni)->setSendPWQProtocol(dispatch, composite);
	}      
	
	// Return
	//TRACE_FN_EXIT();
	return result;
      }      
    }; 
  }; //- Native Interface Common
}; //- PAMI
#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#endif
