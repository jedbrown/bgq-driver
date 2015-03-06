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


#ifndef __components_devices_bgq_mu2_msg_InjectDPutBase_h__
#define __components_devices_bgq_mu2_msg_InjectDPutBase_h__

#include "spi/include/mu/DescriptorBaseXX.h"
#include "util/trace.h"
#include "components/devices/bgq/mu2/InjChannel.h"
#include "components/devices/bgq/mu2/msg/MessageQueue.h"
#include "common/bgq/Mapping.h"
#include "components/memory/MemoryAllocator.h"
#include "components/devices/bgq/mu2/MU_Util.h"

#undef DO_TRACE_ENTEREXIT 
#undef DO_TRACE_DEBUG     
#ifdef CCMI_TRACE_ALL
  #define DO_TRACE_ENTEREXIT 1
  #define DO_TRACE_DEBUG     1
#else
  #define DO_TRACE_ENTEREXIT 0
  #define DO_TRACE_DEBUG     0
#endif

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {      
      ///
      /// \brief Inject one or more dput descriptors into an inject
      /// fifo      
      ///
      class InjectDPutBase {
      protected:
	bool                     _doneCompletion;
	bool                     _processInjection;
	unsigned                 _consumedBytes;
	PAMI::PipeWorkQueue   *  _pwq;
	
      public:
	InjectDPutBase (): 
	  _doneCompletion (false),
	  _processInjection(false),
	  _consumedBytes(0),
	  _pwq(NULL) {}

	bool done () { return _doneCompletion; }

	bool isActive() { 
	  if (_processInjection) {
	    size_t bytes_available = _pwq->getBytesProduced();
	    if (bytes_available == _consumedBytes)
	      return false;	    	    	  
	  }
	  return true;
	}

	PipeWorkQueue *getPwq() { return _pwq; }
	unsigned       getConsumedBytes() { return _consumedBytes; }

	virtual bool advance() { PAMI_abort(); return false; }	
      };  //-- InjectDPutBase
    };  //-- MU
  };  //-- Device
};  //-- PAMI

#endif
