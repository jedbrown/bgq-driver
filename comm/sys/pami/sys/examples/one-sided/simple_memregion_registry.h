/* ------------------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                                      */
/* Blue Gene/Q 5765-PER 5765-PRP                                             */
/* © Copyright IBM Corp. 2012 All Rights Reserved                            */
/* US Government Users Restricted Rights - Use, duplication or disclosure    */
/*   restricted by GSA ADP Schedule Contract with IBM Corp.                  */
/*                                                                           */
/* This software is available to you under the Eclipse Public License (EPL). */
/* ------------------------------------------------------------------------- */

#ifndef __simple_memregion_registry_h__
#define __simple_memregion_registry_h__

#include <string.h>
#include <stdio.h>

#include <pami.h>

#define SIMPLE_MEMREGION_REGISTRY_MAX_CONTEXTS 16
#define SIMPLE_MEMREGION_REGISTRY_DISPATCH_ID  10



/**
 * \brief Memory region registry data for a single pami memory region.
 */
typedef struct simple_memregion_registry_data_t
{
  pami_memregion_t mr;     /**< Opaque memory region */
  size_t           size;   /**< Number of bytes in the memory region */
  void *           vaddr;  /**< Virtual address on the origin task */

} simple_memregion_registry_data_t;

/**
 * \brief Memory region data from all tasks that share a hash id.
 */
typedef struct simple_memregion_registry_entry_t
{
  size_t                                     hash;   /**< Shared hash id */
  struct simple_memregion_registry_data_t  * data;   /**< Malloc'd storage; one element per task */
  unsigned                                 * active; /**< Malloc'd storage; one element per task; 0 if not yet initialized */

  struct simple_memregion_registry_entry_t * next;   /**< Pointer to next set of memory region data */

} simple_memregion_registry_entry_t;

/**
 * \brief Memory region registry container.
 */
typedef struct simple_memregion_registry_t
{
  size_t              offset;  /**< communication context offset used to exchange memory regions */
  pami_context_t      context; /**< context to use for the exchange */
  pami_client_t       client;  /**< client associated with the contexts */

  /**
   * Each context has its own list of memory regions; this is because memory
   * regions are context-based.
   */
  struct simple_memregion_registry_entry_t * head[SIMPLE_MEMREGION_REGISTRY_MAX_CONTEXTS];

} simple_memregion_registry_t;


/**
 * \brief Initialize a 'world' memory region registry
 *
 * \param [in] context Communication context to manage the registry
 */
simple_memregion_registry_t * simple_memregion_registry_init (pami_client_t client,
                                                              pami_context_t context,
                                                              size_t offset);


/**
 * \brief Add a memory region to the 'world' registry
 *
 * \note This example memory region registry assumes all endpoints will
 *       participate. For more flexibility a pami geometry could be
 *       specified instead of a pami client.
 *
 * \param [in] client The PAMI client
 * \param [in] context Communication context used to send the new memory region
 * \param [in] mr      Memory region to add to the registry
 * \param [in] mr_ep   The endpoint associated with the memory region
 * \param [in] mr_hash A unique hash id to differentiate memory region registry entries
 */
void simple_memregion_registry_insert (simple_memregion_registry_t * registry,
                                       pami_memregion_t            * mr,
                                       pami_endpoint_t               mr_ep,
                                       void                        * local_data_addr,
                                       size_t                        local_data_bytes,
                                       size_t                        mr_hash);


void simple_memregion_registry_add (simple_memregion_registry_t * registry,
                                    pami_context_t              * context_array,
                                    size_t                        mr_context_offset,
                                    size_t                        mr_hash,
                                    void                        * local_data_addr,
                                    size_t                        local_data_bytes,
                                    pami_memregion_t            * local_memregion);

/**
 * \brief Query the memory region registry for a memory region associated with an endpoint and hash id
 *
 * \param [in] registry The memory region registry
 * \param [in] endpoint The endpoint to query for the memory region
 * \param [in] hash     The hash id of the memory region to query
 *
 * \retval non-NULL The memory region was returned
 * \retval NULL     The memory region has not yet been added to the registry
 *                  by the remote endpoint; the registry context must be
 *                  advanced before the registry query is attempted again
 */
simple_memregion_registry_data_t *
simple_memregion_registry_query (simple_memregion_registry_t * registry,
                                 pami_endpoint_t               endpoint,
                                 size_t                        hash);



#endif /* __simple_memregion_registry_h__ */
