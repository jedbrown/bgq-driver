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
 * \file components/devices/ShmemCollInterface.h
 * \brief ???
 */
#ifndef __components_devices_ShmemCollInterface_h__
#define __components_devices_ShmemCollInterface_h__

#include <sys/uio.h>

#include "sys/pami.h"
#include "util/common.h"

namespace PAMI
{
  namespace Device
  {
    namespace Interface
	{

	 typedef int (*MatchFunction_t) (void* coll_desc, 			//my collective descriptor
									void* match_desc , 			//matched descriptor of master
									void  *match_func_param );  //device cookie

	}
  }
}

#endif
