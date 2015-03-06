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
 * \file api/extension/c/bgq_l2atomic/BGQL2AtomicExtension.cc
 * \brief ???
 */
#include "api/extension/c/bgq_l2atomic/BGQL2AtomicExtension.h"

#include "Global.h"

PAMI::BGQL2AtomicExtension::BGQL2AtomicExtension ()
{
}

pami_result_t PAMI::BGQL2AtomicExtension::node_memalign(void **memptr, size_t alignment,
						size_t bytes, const char *key)
{
	return __global.l2atomicFactory.__nodescoped_mm.memalign(memptr,
							alignment, bytes, key);
}

void PAMI::BGQL2AtomicExtension::node_free(void *mem)
{
	__global.l2atomicFactory.__nodescoped_mm.free(mem);
}

pami_result_t PAMI::BGQL2AtomicExtension::proc_memalign(void **memptr, size_t alignment,
						size_t bytes, const char *key)
{
	return __global.l2atomicFactory.__procscoped_mm.memalign(memptr,
							alignment, bytes, key);
}

void PAMI::BGQL2AtomicExtension::proc_free(void *mem)
{
	__global.l2atomicFactory.__procscoped_mm.free(mem);
}
