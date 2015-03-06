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

#ifndef __algorithms_interfaces_NativeInterfaceFactory_h__
#define __algorithms_interfaces_NativeInterfaceFactory_h__

#include <pami.h>
#include "util/common.h"
#include "algorithms/interfaces/NativeInterface.h"

namespace CCMI {
  namespace Interfaces {
class Geometry;
    //class NativeInterface;

    class NativeInterfaceFactory {      
    public:

      typedef enum _nisel {
	MULTICAST      = 0,
	MULTICOMBINE      ,
	MANYTOMANY        ,
	P2P               ,
	ALL               ,
      } NISelect;

      typedef enum _nitype {
	ALLSIDED        = 0,
	ACTIVE_MESSAGE     ,
      } NIType; 
      
      NativeInterfaceFactory() {}      
      
      /* 
	 \brief generate a native interface to optimize operations given by NISelect flag input
	 \param [in] dispatch id. Id of the dispatch for AM M* implementations
	 \param [in] sflag.  Construct M* interfaces to support subset or all of the M* calls. 
	 \param[in] size of the connection id to NI (-1UL for unlimited connections)
	 \param [out] ni. Output native interface
	 \return PAMI_SUCCESS or PAMI_ERROR	         
      */
      virtual pami_result_t generate 
	( int               *  dispatch_id,
	  NISelect             sflag,
	  NIType               tflag,
	  size_t               nconnections,
	  NativeInterface   *& ni) = 0;

      /**
       * \brief Analyze the native interface factory on what operations / native interfaces it can build
       * \param[in] context_id
       * \param[in] topology  Communicator topology
       * \param[in] phase of the analyze
       * \param[out] flag 
       */
      virtual pami_result_t analyze(size_t context_id, pami_topology_t *topology, int phase, int *flag) = 0;
    };
  };
};


#endif
