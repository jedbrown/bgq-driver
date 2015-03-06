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

#ifndef __MU_NULL_MULTI_COMBINE_H__
#define __MU_NULL_MULTI_COMBINE_H__

#include "components/devices/MulticombineModel.h"
#include "components/devices/bgq/mu2/Context.h"
#include "sys/pami.h"

///
/// \brief Null Models to optimize space for native ineterfaces that do not call multicombine
///
namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      class NullMulticombineModel: public Interface::MulticombineModel < NullMulticombineModel, MU::Context, 1 >
      {
      public:
	static const size_t   sizeof_msg      =  1;
	  
	NullMulticombineModel (pami_client_t     client,
			       pami_context_t    context,
			       MU::Context     & mucontext,
			       pami_result_t   & status):
	Interface::MulticombineModel <NullMulticombineModel, MU::Context, sizeof_msg> (mucontext, status)
	{
	}

	pami_result_t postMulticombineImmediate_impl(size_t                   client,
						     size_t                   context,
						     pami_multicombine_t    * mcombine,
						     void                   * devinfo = NULL)
	  
	{
	  printf ("PAMI Error : In NULL Multicombine\n");
	  return PAMI_ERROR;
	}
	
      
	pami_result_t postMulticombine_impl(uint8_t (&state)[sizeof_msg],
					    size_t                client,
					    size_t                context, 
					    pami_multicombine_t * mcomb,
					    void                * devinfo = NULL) 
	{
	printf ("PAMI Error : In NULL Multicombine\n");
	return PAMI_ERROR;	
	}
      };
    };
  };
};
#endif
