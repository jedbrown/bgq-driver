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
 * \file test/internals/bgq/personality.cc
 * \brief Simple multicast tests.
 */


#include "Global.h"

#include <hwi/include/bqc/nd_500_dcr.h>

#include <unistd.h>

int main(int argc, char ** argv)
{
  char* var;
  unsigned dcr_num = ND_500_DCR_base + ND_500_DCR__CTRL_COORDS_offset;

  unsigned long long dcr = DCRReadUser(dcr_num);

  fprintf(stderr,"main(): address of TORUS NODE_COORDINATES DCR(%#X) = %p\n", dcr_num, &dcr);
  fprintf(stderr,"main(): contents of TORUS NODE_COORDINATES DCR = %#16.16llx\n", dcr);

  fprintf(stderr,"NODE_COORD A = %#llx\n", ND_500_DCR__CTRL_COORDS__NODE_COORD_A_get(dcr));
  fprintf(stderr,"NODE_COORD B = %#llx\n", ND_500_DCR__CTRL_COORDS__NODE_COORD_B_get(dcr));
  fprintf(stderr,"NODE_COORD C = %#llx\n", ND_500_DCR__CTRL_COORDS__NODE_COORD_C_get(dcr));
  fprintf(stderr,"NODE_COORD D = %#llx\n", ND_500_DCR__CTRL_COORDS__NODE_COORD_D_get(dcr));
  fprintf(stderr,"NODE_COORD E = %#llx\n", ND_500_DCR__CTRL_COORDS__NODE_COORD_E_get(dcr));

  fprintf(stderr,"MAX_COORD A = %#llx\n", ND_500_DCR__CTRL_COORDS__MAX_COORD_A_get(dcr));
  fprintf(stderr,"MAX_COORD B = %#llx\n", ND_500_DCR__CTRL_COORDS__MAX_COORD_B_get(dcr));
  fprintf(stderr,"MAX_COORD C = %#llx\n", ND_500_DCR__CTRL_COORDS__MAX_COORD_C_get(dcr));
  fprintf(stderr,"MAX_COORD D = %#llx\n", ND_500_DCR__CTRL_COORDS__MAX_COORD_D_get(dcr));
  fprintf(stderr,"MAX_COORD E = %#llx\n", ND_500_DCR__CTRL_COORDS__MAX_COORD_E_get(dcr));


  var = getenv("BG_PROCESSESPERNODE");
  fprintf(stderr,"BG_PROCESSESPERNODE %s\n",var? var: "NULL");

  var = getenv("BG_SHAREDMEMSIZE");
  fprintf(stderr,"BG_SHAREDMEMSIZE %s\n",var? var: "NULL");

  var = getenv("BG_MEMSIZE");
  fprintf(stderr,"BG_MEMSIZE %s\n",var? var: "NULL");

  fprintf(stderr,"BGQ Personality A coord %zu\n",__global.personality.aCoord());
  fprintf(stderr,"BGQ Personality B coord %zu\n",__global.personality.bCoord());
  fprintf(stderr,"BGQ Personality C coord %zu\n",__global.personality.cCoord());
  fprintf(stderr,"BGQ Personality D coord %zu\n",__global.personality.dCoord());
  fprintf(stderr,"BGQ Personality E coord %zu\n",__global.personality.eCoord());

  fprintf(stderr,"BGQ Personality A size %zu\n",__global.personality.aSize());
  fprintf(stderr,"BGQ Personality B size %zu\n",__global.personality.bSize());
  fprintf(stderr,"BGQ Personality C size %zu\n",__global.personality.cSize());
  fprintf(stderr,"BGQ Personality D size %zu\n",__global.personality.dSize());
  fprintf(stderr,"BGQ Personality E size %zu\n",__global.personality.eSize());

  size_t task_id = __global.mapping.task();
  fprintf(stderr,"HELLO from task %zu\n",task_id);

  char clientname[] = "PAMI";
  pami_client_t client;
  fprintf(stderr, "PAMI_Client_create()\n");
  PAMI_Client_create (clientname, &client, NULL, 0);
  pami_context_t context;
  fprintf(stderr, "PAMI_Context_createv()\n");
  { size_t _n = 1; PAMI_Context_createv (client, NULL, 0, &context, _n); }

  pami_configuration_t configuration;
  pami_result_t result;

  configuration.name = PAMI_CLIENT_TASK_ID;
  result = PAMI_Client_query(client, &configuration,1);
  size_t my_rank = configuration.value.intval;
  fprintf(stderr, "PAMI_CLIENT_TASK_ID %zu\n",my_rank);

  configuration.name = PAMI_CLIENT_NUM_TASKS;
  result = PAMI_Client_query(client, &configuration, 1);
  size_t num_tasks = configuration.value.intval;
  fprintf(stderr, "PAMI_CLIENT_NUM_TASKS %zu\n",num_tasks);

  configuration.name = PAMI_CLIENT_WTICK;
  result = PAMI_Client_query(client, &configuration,1);
  double tick = configuration.value.doubleval;
  fprintf(stderr, "PAMI_CLIENT_WTICK %f\n",tick);

  fprintf(stderr,"GOODBYE from task %zu/%zu\n",task_id,my_rank);

  return 0;
}
