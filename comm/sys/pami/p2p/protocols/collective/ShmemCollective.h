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
 * \file p2p/protocols/collective/ShmemCollective.h
 * \brief ???
 */
#ifndef __p2p_protocols_collective_ShmemCollective_h__
#define __p2p_protocols_collective_ShmemCollective_h__

#include "components/memory/MemoryAllocator.h"
#include "util/common.h"

#ifndef TRACE_ERR
#define TRACE_ERR(x) // fprintf x
#endif

namespace PAMI
{
  namespace Protocol
  {
    namespace ShmemCollective
    {

	template <class T_Model, class T_Device, class T_Descriptor>
	class ShmemCollective
	{

	protected:

	typedef uint8_t collective_t[T_Model::packet_model_state_bytes];

	public:

	inline ShmemCollective (size_t                     dispatch,
							pami_dispatch_callback_function dispatch_fn,
							void                     * cookie,
							T_Device                 & device,
							pami_result_t             & status) :
					    	_collective_model (device),
							_context (device.getContext()),
							_dispatch_fn (dispatch_fn),
							_cookie (cookie),
							_device (device)
		{

				// ----------------------------------------------------------------
				// Compile-time assertions
				// ----------------------------------------------------------------

				TRACE_ERR((stderr, "ShmemCollective() register 'immediate' model\n"));
				status = _collective_model.init (dispatch, dispatch_collective_sync, this);
				TRACE_ERR((stderr, "ShmemCollective() 'immediate' model status = %d\n", status));
		}

		inline pami_result_t collective_impl (pami_multicast_t *mcastinfo)
		{
			_collective_model.postDescriptor(mcastinfo);

		}


		protected:

		inline void freeSendState (send_t * object)
		{
				_allocator.returnObject ((void *) object);
		}

		MemoryAllocator < sizeof(collective_t), 16 > _allocator;

		T_Model                    _collective_model;

		pami_context_t              _context;
		pami_dispatch_callback_function _dispatch_fn;
		void                     * _cookie;
		T_Device                 & _device;


		static int dispatch_collective_matched(T_Descriptor* descriptor)
		{

			  collective_t * coll = (collective_t *) _allocator.allocateObject ();
			  _collective_model.postCollective(coll, descriptor);
				return 0;
		};



	};


    }
  }
}
