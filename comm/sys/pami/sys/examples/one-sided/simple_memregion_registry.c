
/* ------------------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                                      */
/* Blue Gene/Q 5765-PER 5765-PRP                                             */
/* © Copyright IBM Corp. 2012 All Rights Reserved                            */
/* US Government Users Restricted Rights - Use, duplication or disclosure    */
/*   restricted by GSA ADP Schedule Contract with IBM Corp.                  */
/*                                                                           */
/* This software is available to you under the Eclipse Public License (EPL). */
/* ------------------------------------------------------------------------- */

#include "simple_memregion_registry.h"





typedef struct simple_memregion_registry_exchange_header
{
  size_t          hash;
  pami_endpoint_t endpoint;
} simple_memregion_registry_exchange_header_t;

void simple_memregion_registry_decrement (pami_context_t   context,
                                          void           * cookie,
                                          pami_result_t result)
{
  unsigned * value_ptr = (unsigned *) cookie;
  (*value_ptr)--;
}

void simple_memregion_registry_recv_cb (pami_context_t    context,
                                        void            * cookie,
                                        const void      * header_addr,
                                        size_t            header_size,
                                        const void      * pipe_addr,
                                        size_t            data_size,
                                        pami_endpoint_t   origin,
                                        pami_recv_t     * recv)
{
  simple_memregion_registry_t * registry =
    (simple_memregion_registry_t *) cookie;

  simple_memregion_registry_exchange_header_t * header =
    (simple_memregion_registry_exchange_header_t *) header_addr;

  fprintf (stdout, "(%03d) simple_memregion_registry_recv_cb(), cookie = %p, header->endpoint = 0x%08x, header->hash = %zu\n", __LINE__, cookie, header->endpoint, header->hash);

  pami_task_t task;
  size_t      offset;
  PAMI_Endpoint_query (header->endpoint, & task, & offset);

  struct simple_memregion_registry_entry_t * entry = registry->head[offset];
  while (entry != NULL)
  {
    if (entry->hash == header->hash)
    {
      if (entry->active[task] == 0)
      {
        fprintf (stdout, "(%03d) Found existing memregion registry\n", __LINE__);
        memcpy ((void *) & entry->data[task], (void *) pipe_addr, sizeof(simple_memregion_registry_data_t));
        entry->active[task] = 1;
        return;
      }
      else
      {
        fprintf (stdout, "(%03d) ERROR. Duplicate memregion entry\n", __LINE__);
        exit (1);
        return;
      }
    }
    entry = entry->next;
  }


  /*
   * 'Unexpected' memregion registry entry
   */
  fprintf (stdout, "(%03d) 'unexpected' memregion registry\n", __LINE__);

  size_t num_tasks = size (registry->client);

  entry = (struct simple_memregion_registry_entry_t *) malloc (sizeof(struct simple_memregion_registry_entry_t));

  entry->hash   = header->hash;
  entry->active = (unsigned *) malloc (sizeof(unsigned) * num_tasks);
  entry->data   = (simple_memregion_registry_data_t *) malloc (sizeof(simple_memregion_registry_data_t) * num_tasks);

  unsigned i;
  for (i=0; i<num_tasks; i++)
  {
    entry->active[i] = 0;
  }

  memcpy ((void *) & entry->data[task], (void *) pipe_addr, sizeof(simple_memregion_registry_data_t));
  entry->active[task] = 1;

  /*
   * Insert the new memregion registry entry
   */
  entry->next = registry->head[offset];
  registry->head[offset] = entry;

  return;
}








/**
 * \brief Initialize a 'world' memory region registry
 *
 * \param [in] context Communication context to manage the registry
 */
simple_memregion_registry_t * simple_memregion_registry_init (pami_client_t  client,
                                                              pami_context_t context,
                                                              size_t         offset)
{
  simple_memregion_registry_t * registry =
    (simple_memregion_registry_t *) malloc (sizeof(simple_memregion_registry_t));

  registry->offset  = offset;
  registry->context = context;
  registry->client  = client;

  unsigned i;
  for (i=0; i<SIMPLE_MEMREGION_REGISTRY_MAX_CONTEXTS; i++)
  {
    registry->head[i] = NULL;  
  }


  /*
   * Register the memory region registry exchange dispatch.
   */
  pami_dispatch_hint_t mr_hint = {0};
  pami_dispatch_callback_function mr_dispatch;
  mr_dispatch.p2p = simple_memregion_registry_recv_cb;
  PAMI_Dispatch_set (context, SIMPLE_MEMREGION_REGISTRY_DISPATCH_ID, mr_dispatch,
                     (void *) registry, mr_hint);

  return registry;
}


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
                                       size_t                        mr_hash)
{

  simple_memregion_registry_exchange_header_t header;
  header.hash     = mr_hash;
  header.endpoint = mr_ep;
 
  simple_memregion_registry_data_t data;
  memcpy ((void *)&data.mr, (void *)mr, sizeof(pami_memregion_t));
  //data.mr    = *mr;
  data.size  = local_data_bytes;
  data.vaddr = local_data_addr;
   
  /*
   * Exchange the memory regions
   */
  volatile unsigned active = 0;

  pami_send_t parameters = {0};
  parameters.send.dispatch = SIMPLE_MEMREGION_REGISTRY_DISPATCH_ID;
  parameters.send.header.iov_base = (void *) & header;
  parameters.send.header.iov_len = sizeof(simple_memregion_registry_exchange_header_t);
  parameters.send.data.iov_base = (void *) &data;
  parameters.send.data.iov_len = sizeof(simple_memregion_registry_data_t);
  parameters.events.cookie = (void *) & active;
  parameters.events.local_fn = simple_memregion_registry_decrement;

  
  size_t i, num_tasks = size (registry->client);
  for (i = 0; i < num_tasks; i++)
    {
      PAMI_Endpoint_create (registry->client, i, registry->offset, & parameters.send.dest);
      active++;
      PAMI_Send (registry->context, & parameters);

      /*
       * Advance until the local memory region data has been sent to the task.
       */
      while (active > 0)
        PAMI_Context_advance (registry->context, 1);
    }
}


void simple_memregion_registry_add (simple_memregion_registry_t * registry,
                                    pami_context_t              * context_array,
                                    size_t                        mr_context_offset,
                                    size_t                        mr_hash,
                                    void                        * local_data_addr,
                                    size_t                        local_data_bytes,
                                    pami_memregion_t            * local_memregion)
{
  pami_endpoint_t local_endpoint;

  size_t actual_memregion_bytes = 0;

  PAMI_Memregion_create (context_array[mr_context_offset],
                         local_data_addr,
                         local_data_bytes,
                         & actual_memregion_bytes,
                         local_memregion);

  PAMI_Endpoint_create (registry->client, task(registry->client), mr_context_offset, & local_endpoint);

  simple_memregion_registry_insert (registry,
                                    local_memregion,
                                    local_endpoint,
                                    local_data_addr,
                                    local_data_bytes,
                                    mr_hash);

  return;
}


/**
 * \brief Query the memory region registry for a memory region associated with an endpoint and hash id
 *
 * \param [in] endpoint The endpoint to query for the memory region
 * \param [in] hash     The hash id of the memory region to query
 * \param [out] memregion The memory region
 *
 * \retval PAMI_SUCCESS The memory region was returned
 * \retval PAMI_EAGAIN  The memory region has not yet been added to the registry
 *                      by the remote endpoint; the registry context must be
 *                      advanced before the registry query is attempted again
 */
simple_memregion_registry_data_t *
simple_memregion_registry_query (simple_memregion_registry_t * registry,
                                 pami_endpoint_t               endpoint,
                                 size_t                        hash)
{
  pami_task_t task;
  size_t      offset;
  PAMI_Endpoint_query (endpoint, & task, & offset);

  struct simple_memregion_registry_entry_t * entry = registry->head[offset];
  while (entry != NULL)
  {
    if (entry->hash == hash)
    {
      if (entry->active[task] > 0)
      {
        return & entry->data[task];
      }
      else
      {
        return NULL;
      }
    }
    entry = entry->next;
  }

  return NULL;
}



