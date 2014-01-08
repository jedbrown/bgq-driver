/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */


/**
 * \file memory_impl.c
 *
 * \brief C File containing CNK Memory Implementation
 */

#include <spi/include/kernel/memory.h>


/**
 * \brief Declaration of the CNK Memory Region Cache.
 *
 * The memory cache has a fixed number of slots, one for each CNK
 * memory region (text, data, heap, shared memory, extra).
 */
CNKSPI_MemoryRegionCacheElement_t   _cnkspi_MemoryRegionCache[CNKSPI_MAX_NUM_CACHED_MEMORY_REGIONS];


/*!
 * \brief Last Accessed Cache Element Number
 */
uint64_t _cnkspi_MemoryRegionCacheLastAccessedElementNumber = 0;


/*!
 * \brief Number of Elements in the CNK Memory Region Cache
 */
uint64_t _cnkspi_MemoryRegionCacheNumElements = 0;


/*!
 * \brief Lock variable for the CNK Memory Region Cache
 *
 * This is just a variable that is zero when the CNK memory region cache is
 * unlocked, and non-zero when it is locked.  Fetch_and_Add() is used to 
 * lock it.  Whoever sees zero has the lock.  Zero it out to unlock it, 
 * when holding the lock.
 */
uint64_t _cnkspi_MemoryRegionCacheLock = 0;
