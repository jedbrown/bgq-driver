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
 * \file sys/lapiunix/pami_sys.h
 * \brief "Default" platform specifc type definitions for IBM's PAMI message layer.
 */

#ifndef __default_pami_sys_h__
#define __default_pami_sys_h__

/* We need 64 bytes memory region for both 32- and 64-bit on PE implementation */
#ifndef __64BIT__
#define PAMI_CLIENT_MEMREGION_SIZE_STATIC (16*sizeof(uintptr_t))
#else
#define PAMI_CLIENT_MEMREGION_SIZE_STATIC (8*sizeof(uintptr_t))
#endif

#define PAMI_WORK_SIZE_STATIC (8*sizeof(uintptr_t))
#define PAMI_REQUEST_NQUADS 512

#endif /* __default_pami_sys_h__ */
