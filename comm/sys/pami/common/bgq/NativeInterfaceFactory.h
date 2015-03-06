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

#ifndef __common_bgq_NativeInterfaceFactory__
#define __common_bgq_NativeInterfaceFactory__

#include  "util/trace.h"
#include  "algorithms/interfaces/NativeInterface.h"
#include  "algorithms/interfaces/NativeInterfaceFactory.h"

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

  template <class T_Allocator, class T_NI, class T_Device, CCMI::Interfaces::NativeInterfaceFactory::NISelect T_Sel, CCMI::Interfaces::NativeInterfaceFactory::NIType T_Type, size_t NCONN=(size_t)-1>
    class BGQNativeInterfaceFactory : public CCMI::Interfaces::NativeInterfaceFactory {
  protected:
    pami_client_t       _client;
    pami_context_t      _context;
    size_t              _client_id;	
    size_t              _context_id;
    T_Device          & _device;
    T_Allocator       & _allocator;
    
  public:
    
    BGQNativeInterfaceFactory ( pami_client_t       client,
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
        TRACE_FN_ENTER();
        COMPILE_TIME_ASSERT(sizeof(T_NI) <= T_Allocator::objsize);
        TRACE_FORMAT("Allocator:  sizeof(T_NI) %zu, T_Allocator::objsize %zu",sizeof(T_NI),T_Allocator::objsize);
        TRACE_FN_EXIT();
      }
    

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
				    size_t                                        nconnections, 
                                    CCMI::Interfaces::NativeInterface                          *&  ni)
    {
      TRACE_FN_ENTER();
      pami_result_t result = PAMI_ERROR;
      ni = NULL;           
      if (T_Sel != ni_select)
      {  
        TRACE_FN_EXIT();
        return result;
      }

      if (T_Type != ni_type)
      {  
        TRACE_FN_EXIT();
        return result;
      }
      
      if (NCONN > 0 && nconnections > NCONN)
      {
        TRACE_FN_EXIT();
        return result;
      }

      result = PAMI_SUCCESS;
      // Construct the protocol(s) using the NI dispatch function and cookie
      COMPILE_TIME_ASSERT(sizeof(T_NI) <= T_Allocator::objsize);
      ni = (CCMI::Interfaces::NativeInterface *) _allocator.allocateObject ();
      TRACE_FORMAT("<%p> ni %p", this,  ni);
      new ((void *)ni) T_NI (_device, _allocator, _client, _context, _context_id, _client_id, dispatch_id);
     
      TRACE_FN_EXIT();
      return result;
    }

    virtual pami_result_t  analyze(size_t context_id, pami_topology_t *topology, int phase, int* flag)
    {
      TRACE_FN_ENTER();
      *flag = 0; 
      TRACE_FORMAT("<%p> result %u/%u",this,PAMI_OTHER,*flag);
      TRACE_FN_EXIT();     
      return PAMI_SUCCESS;// query required (short metadata)
    }  
  };

  template <class T_Allocator, class T_NI, class T_Device1, class T_Device2, CCMI::Interfaces::NativeInterfaceFactory::NISelect T_Sel, CCMI::Interfaces::NativeInterfaceFactory::NIType T_Type, size_t NCONN=-1>
    class BGQNativeInterfaceFactory2Device : public BGQNativeInterfaceFactory <T_Allocator, T_NI, T_Device1, T_Sel, T_Type, NCONN> {
  protected:
    T_Device2         & _device2;
    
  public:    
    BGQNativeInterfaceFactory2Device ( pami_client_t       client,
				       pami_context_t      context,
				       size_t              clientid,
				       size_t              contextid,
				       T_Device1         & device1,
				       T_Device2         & device2,
				       T_Allocator       & allocator) : 
    BGQNativeInterfaceFactory<T_Allocator, T_NI, T_Device1, T_Sel, T_Type, NCONN> (client, context, clientid, contextid, device1, allocator),
    _device2(device2)
    {	
      COMPILE_TIME_ASSERT(sizeof(T_NI) <= T_Allocator::objsize);
      TRACE_FN_ENTER();
      TRACE_FORMAT("Allocator:  sizeof(T_NI) %zu, T_Allocator::objsize %zu",sizeof(T_NI),T_Allocator::objsize);
      TRACE_FN_EXIT();
    }
    
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
				    size_t                                                         nconnections, 
				    CCMI::Interfaces::NativeInterface                          *&  ni)
    {
      TRACE_FN_ENTER();
      pami_result_t result = PAMI_ERROR;
      ni = NULL;           
      if (T_Sel != ni_select)
      {  
	TRACE_FN_EXIT();
	return result;
      }
    
      if (T_Type != ni_type)
      {  
        TRACE_FN_EXIT();
        return result;
      }
    
      if (NCONN > 0 && nconnections > NCONN)
      {
        TRACE_FN_EXIT();
        return result;
      }
    
      result = PAMI_SUCCESS;
      // Construct the protocol(s) using the NI dispatch function and cookie
      COMPILE_TIME_ASSERT(sizeof(T_NI) <= T_Allocator::objsize);
      ni = (CCMI::Interfaces::NativeInterface *) this->_allocator.allocateObject ();
      TRACE_FORMAT("<%p> ni %p", this,  ni);
      new ((void *)ni) T_NI (this->_device, _device2, 
			     this->_allocator, this->_client, this->_context, 
			     this->_context_id, this->_client_id, 
			     dispatch_id);
      
      TRACE_FN_EXIT();
      return result;
    }
  
    virtual pami_result_t  analyze(size_t context_id, pami_topology_t *topology, int phase, int* flag)
    {
      TRACE_FN_ENTER();
      *flag = 0; 
      TRACE_FORMAT("<%p> result %u/%u",this,PAMI_OTHER,*flag);
      TRACE_FN_EXIT();     
      return PAMI_SUCCESS;// query required (short metadata)
    }  
  
  };

};


#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#endif      
