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
#include "api/extension/c/collsel/CollselExtension.h"
#include "api/extension/c/collsel/Advisor.h"
#include "api/extension/c/collsel/AdvisorTable.h"

namespace PAMI{

pami_result_t CollselExtension::Collsel_init_fn(pami_client_t            client,
                                                advisor_configuration_t  configuration[],
                                                size_t                   num_configs,
                                                pami_context_t           contexts[],
                                                size_t                   num_contexts,
                                                advisor_t                *advisor)
{
  *advisor = (advisor_t) new Advisor(client,configuration,num_configs,
                                     contexts,num_contexts);
  return PAMI_SUCCESS;
}

pami_result_t CollselExtension::Collsel_destroy_fn(advisor_t *advisor)
{
  Advisor *adv = (Advisor *) *advisor;

  if(adv == NULL)
    return PAMI_INVAL;

  delete adv;
  *advisor = NULL;

  return PAMI_SUCCESS;
}

pami_result_t CollselExtension::Collsel_table_generate_fn(advisor_t         advisor,
                                                          char             *filename,
                                                          advisor_params_t *params,
                                                          int               mode)
{
  Advisor       *a  = (Advisor*) advisor;
  AdvisorTable  *at = new AdvisorTable(*a);
  return at->generate(filename,params, mode);
}


pami_result_t CollselExtension::Collsel_table_load_fn(advisor_t        advisor,
                                                      char            *filename,
                                                      advisor_table_t *advisor_table)
{
  pami_result_t res;
  Advisor      *a  = (Advisor*) advisor;
  AdvisorTable *at = new AdvisorTable(*a);
  // Load collective selection data from XML
  res = at->load(filename);
  if(res == PAMI_SUCCESS)
  {
    *advisor_table   = (advisor_table_t) at;
  }
  return res;
}


pami_result_t CollselExtension::Collsel_table_unload_fn(advisor_table_t *advisor_table)
{
  pami_result_t res;
  AdvisorTable *at = (AdvisorTable*) *advisor_table;
  // Free any allocated resources
  res = at->unload();
  if(res == PAMI_SUCCESS)
  {
    *advisor_table = NULL;
  }
  return res;
}

pami_result_t CollselExtension::Collsel_query_fn(advisor_table_t *advisor_table,
                                                 pami_geometry_t  geometry,
                                                 fast_query_t    *fast_query)
{
  pami_result_t res;
  AdvisorTable *at = (AdvisorTable*) *advisor_table;

  res = at->query(geometry, fast_query);

  return PAMI_SUCCESS;
}


pami_result_t CollselExtension::Collsel_advise_fn(fast_query_t        fast_query,
                                                  pami_xfer_type_t    xfer_type,
                                                  pami_xfer_t        *xfer,
                                                  advisor_algorithm_t algorithms_optimized[],
                                                  size_t              max_algorithms)
{
  // Todo:  Implement
  return PAMI_SUCCESS;
}


};//namespace PAMI
