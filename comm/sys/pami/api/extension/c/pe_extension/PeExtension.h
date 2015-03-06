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
 * \file api/extension/c/pe_extension/PeExtension.h
 * \brief ???
 */
#ifndef __api_extension_c_pe_extension_PeExtension_h__
#define __api_extension_c_pe_extension_PeExtension_h__

#include <pami.h>
#include "pami_ext_pe.h"

#define PAMI_MAX_CLIENT_NUM   128 /**< max number of clients allowed */
#define PAMI_MAX_CONTEXT_NUM  128 /**< max number of contexts allowed in a client */

typedef struct {
    unsigned mask; // mask of a trace type
    char     *str; // string of a trace type
} pe_extension_itrace_mask_t;

namespace PAMI
{
  // internal structure for pami_active_context_t  
  typedef struct
  {
    size_t          context_num;
    pami_context_t  contexts[PAMI_MAX_CONTEXT_NUM];
  } PamiActiveContexts;

  // internal structure/class for pami_active_client_t
  class PamiActiveClients 
  {
    size_t          client_num;
    pami_client_t   clients[PAMI_MAX_CLIENT_NUM];

    public:
    PamiActiveClients() {
      client_num = 0;
      for (size_t i = 0; i < PAMI_MAX_CLIENT_NUM; i ++)
        clients[i] = NULL;
    }
    inline pami_result_t AddClient(pami_client_t client) {
      if (client_num < PAMI_MAX_CLIENT_NUM && client != NULL) {  
        clients[client_num] = client;
        client_num ++;
        return PAMI_SUCCESS;
      } else {
        return PAMI_ERROR;
      }
    }
    inline pami_result_t RemoveClient(pami_client_t client) {
      bool found = false; 

      for (size_t i = 0; i < client_num; i ++) {
        if (!found) {
          if (clients[i] == client)
            found = true;
        } else {
          clients[i-1] = clients[i];
          if (i == client_num - 1)
            clients[i] = NULL;  
        } 
      }

      if (found) {
        client_num --;
        return PAMI_SUCCESS;
      } else
        return PAMI_ERROR;  
    }
  };

  class PeExtension
  {
    public:
        
      /**
      * \brief PAMI Global query that can be invoked before any clients being
      * created
      *
      * \param[in]  configs       Array of queries
      * \param[out] num_configs   Number of queries
      *
      * \retval PAMI_SUCCESS  The queries successfully handled.
      * \retval PAMI_INVAL    The queries are not recognized.
      */
      static pami_result_t global_query(pami_configuration_t  configs[], 
                                        size_t                num_configs);  

      static unsigned trace_mask;
      static void itrace_read_masks(pe_extension_itrace_mask_t* masks, unsigned masks_cnt, char* mask_env);
      static void itrace(unsigned type, char* fmt, ...);
  };
};

#endif // __api_extension_c_pe_extension_PeExtension_h__
