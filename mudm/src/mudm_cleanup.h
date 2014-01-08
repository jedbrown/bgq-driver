/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (c) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/*                                                                  */
/* US Government Users Restricted Rights - Use, duplication or      */
/* disclosure restricted by GSA ADP Schedule Contract with IBM      */
/* Corporation.                                                     */
/*                                                                  */
/* This software is available to you under either the GNU General   */
/* Public License (GPL) version 2 or the Eclipse Public License     */
/* (EPL) at your discretion.                                        */
/*                                                                  */

//! \file  mudm_cleanup.h 
//! \brief Cleanup allocated structures
//! \remarks Intended to assist in termination when mudm_terminate is called


#ifndef	_MUDM_CLEANUP_H_ /* Prevent multiple inclusion */
#define	_MUDM_CLEANUP_H_
#include <mudm/include/mudm.h>

#define MAX_CLEANUP_ENTRIES 25
//! \todo TODO: consider moving cleanup table as static into mudm_cleanup.c instead of in my_context (main control block)

struct my_context;

typedef uint64_t (*cleanup_fn_t)(struct mudm_memory_region * memregion,struct my_context * mcontext, void * args);

uint64_t free_mem(struct mudm_memory_region * memregion,
                  struct my_context * mcontext, 
                  void * args);


struct cleanup_entry {
  struct mudm_memory_region * memregion;
  uint64_t status;
  void * argslist;
  cleanup_fn_t cleanup_fn;
};


int alloc_object(size_t length_mem,
                 struct mudm_memory_region * mregion,
                 struct my_context * mcontext, 
                 void * args, 
                 cleanup_fn_t fn
                 );

int add_object(struct mudm_memory_region * mregion,struct my_context * mcontext, void * args, cleanup_fn_t fn);


#endif //_MUDM_CLEANUP_H_
