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
 * \file test/api/extensions/topology/topology.cc
 * \brief Simple topology interface tests
 */

#include "Global.h"


#define DBG_FPRINTF(x) //fprintf x

#define FPRINTF_TOPOLOGY(topo)    // disable fprintf's

#ifndef FPRINTF_TOPOLOGY // otherwise enable fprintfs
#define FPRINTF_TOPOLOGY(topo)                                                 \
{                                                                              \
size_t global_size = __global.topology_global.size();                          \
pami_topology_type_t type = topo.type();                                        \
fprintf(stderr,"type %d/%s, size %d\n",                                       \
        type,                                                                  \
        type==PAMI_EMPTY_TOPOLOGY?"PAMI_EMPTY_TOPOLOGY":                         \
        ((type==PAMI_SINGLE_TOPOLOGY)?"PAMI_SINGLE_TOPOLOGY":                    \
         ((type==PAMI_RANGE_TOPOLOGY)?"PAMI_RANGE_TOPOLOGY":                     \
          ((type==PAMI_LIST_TOPOLOGY)?"PAMI_LIST_TOPOLOGY":                      \
           ((type==PAMI_COORD_TOPOLOGY)?"PAMI_COORD_TOPOLOGY":"bogus")))),       \
        topo.size());                                                          \
for(unsigned j=0; j< topo.size(); ++j)                                         \
{                                                                              \
  fprintf(stderr,"index2Rank(%d)=%d\n",                                       \
                 j,topo.index2Rank(j));                                        \
}                                                                              \
for(unsigned j=0; j< global_size; ++j)                                         \
{                                                                              \
  fprintf(stderr,"Is rank %d a member? %s. rank2Index(%d)=%d\n",              \
                 j,topo.isRankMember(j)?"yes":"no",                            \
                 j,topo.rank2Index(j));                                        \
}                                                                              \
fprintf(stderr,"\n");                                                          \
}
#endif

// Topology, expected size (-1 means size should be > 0 but unknown)
// #define TEST_TOPOLOGY(topo,expected_size)
inline void TEST_TOPOLOGY(PAMI::Topology topo, size_t expected_size)
{
size_t global_size = __global.topology_global.size();
FPRINTF_TOPOLOGY(topo);
pami_topology_type_t type = topo.type();
if((expected_size==(size_t)-1) && (!topo.size()))
  fprintf(stderr,"FAIL: type %d/%s, expected size > 0, actual size %zu\n",
        type,
        type==PAMI_EMPTY_TOPOLOGY?"PAMI_EMPTY_TOPOLOGY":
        ((type==PAMI_SINGLE_TOPOLOGY)?"PAMI_SINGLE_TOPOLOGY":
         ((type==PAMI_RANGE_TOPOLOGY)?"PAMI_RANGE_TOPOLOGY":
          ((type==PAMI_LIST_TOPOLOGY)?"PAMI_LIST_TOPOLOGY":
           ((type==PAMI_COORD_TOPOLOGY)?"PAMI_COORD_TOPOLOGY":"bogus")))),
        topo.size());
if((expected_size!=(unsigned)-1) && (expected_size!=topo.size()))
  fprintf(stderr,"FAIL: type %d/%s, expected size %zu, actual size %zu\n",
        type,
        type==PAMI_EMPTY_TOPOLOGY?"PAMI_EMPTY_TOPOLOGY":
        ((type==PAMI_SINGLE_TOPOLOGY)?"PAMI_SINGLE_TOPOLOGY":
         ((type==PAMI_RANGE_TOPOLOGY)?"PAMI_RANGE_TOPOLOGY":
          ((type==PAMI_LIST_TOPOLOGY)?"PAMI_LIST_TOPOLOGY":
           ((type==PAMI_COORD_TOPOLOGY)?"PAMI_COORD_TOPOLOGY":"bogus")))),
        expected_size, topo.size());
if((type!=PAMI_EMPTY_TOPOLOGY) &&
   (type!=PAMI_SINGLE_TOPOLOGY) &&
   (type!=PAMI_RANGE_TOPOLOGY) &&
   (type!=PAMI_LIST_TOPOLOGY) &&
   (type!=PAMI_COORD_TOPOLOGY))
  fprintf(stderr,"FAIL: type %d/%s, size %zu\n",
        type,
        type==PAMI_EMPTY_TOPOLOGY?"PAMI_EMPTY_TOPOLOGY":
        ((type==PAMI_SINGLE_TOPOLOGY)?"PAMI_SINGLE_TOPOLOGY":
         ((type==PAMI_RANGE_TOPOLOGY)?"PAMI_RANGE_TOPOLOGY":
          ((type==PAMI_LIST_TOPOLOGY)?"PAMI_LIST_TOPOLOGY":
           ((type==PAMI_COORD_TOPOLOGY)?"PAMI_COORD_TOPOLOGY":"bogus")))),
        topo.size());
for(unsigned j=0; j< topo.size(); ++j)
{
  if(!topo.isRankMember(topo.index2Rank(j)))
  fprintf(stderr,"FAIL: index2Rank(%d)=%d is not a member\n",
                 j,topo.index2Rank(j));
}
size_t nmembers = 0;
for(size_t j=0; j< global_size; ++j)
{
  if(topo.isRankMember(j)) nmembers++;
  if((topo.rank2Index(j)!=(unsigned)-1) && (topo.rank2Index(j) >= topo.size()))
    fprintf(stderr,"FAIL: rank2Index(%zu)= %zu, index >= size %zu\n",
            j,topo.rank2Index(j),topo.size());
  if(!topo.isRankMember(j) && (topo.rank2Index(j) != (unsigned)-1))
    fprintf(stderr,"FAIL: Is %zu a member? %s. rank2Index(%zu)=%zu\n",
            j,topo.isRankMember(j)?"yes":"no",
            j,topo.rank2Index(j));
  if(topo.isRankMember(j) && (topo.rank2Index(j) == (unsigned)-1))
    fprintf(stderr,"FAIL: Is %zu a member? %s. rank2Index(%zu)=%zu\n",
            j,topo.isRankMember(j)?"yes":"no",
            j,topo.rank2Index(j));
  if(topo.isRankMember(j) &&
     (j != topo.index2Rank(topo.rank2Index(j))))
    fprintf(stderr,"FAIL: Is %zu a member? %s. rank2Index(%zu)=%zu index2Rank(%zu)=%d\n",
            j,topo.isRankMember(j)?"yes":"no",
            j,topo.rank2Index(j),
            topo.rank2Index(j),topo.index2Rank(topo.rank2Index(j)));
}
if(nmembers != topo.size())
    fprintf(stderr,"FAIL: nmembers %zu != size %zu\n",
            nmembers,topo.size());
}


int main(int argc, char ** argv)
{
  pami_client_t client;
  pami_context_t context;
  pami_result_t status = PAMI_ERROR;

  status = PAMI_Client_create("multicast test", &client, NULL, 0);
  if(status != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to initialize pami client. result = %d\n", status);
    return 1;
  }
  DBG_FPRINTF((stderr,"Client %p\n",client));

  status = PAMI_Context_createv(client, NULL, 0, &context, 1);
  if(status != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to create pami context. result = %d\n", status);
    return 1;
  }

  pami_configuration_t configuration;

  configuration.name = PAMI_CLIENT_TASK_ID;
  status = PAMI_Client_query(client, &configuration,1);
  if(status != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable query configuration (%d). result = %d\n", configuration.name, status);
    return 1;
  }
  size_t task_id = configuration.value.intval;
  DBG_FPRINTF((stderr, "My task id = %zu\n", task_id));

  configuration.name = PAMI_CLIENT_NUM_TASKS;
  status = PAMI_Client_query(client, &configuration,1);
  if(status != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable query configuration (%d). result = %d\n", configuration.name, status);
    return 1;
  }
  size_t num_tasks = configuration.value.intval;
  if(task_id == 0) fprintf(stderr, "Number of tasks = %zu\n", num_tasks);

// END standard setup
// ------------------------------------------------------------------------
if(task_id == 0)
{
  COMPILE_TIME_ASSERT(sizeof(pami_topology_t) >= sizeof(PAMI::Topology));

  size_t  gSize    = __global.topology_global.size();
  PAMI::Topology topology, subtopology, copy_topology;

  fprintf(stderr,"\n");fprintf(stderr,"global\n");
  TEST_TOPOLOGY(__global.topology_global,gSize);

  fprintf(stderr,"\n");fprintf(stderr,"global converted to list - DO NOT DO THIS TO GLOBAL IN REAL APPS\n");
  __global.topology_global.convertTopology(PAMI_LIST_TOPOLOGY);
  TEST_TOPOLOGY(__global.topology_global,gSize);

  pami_task_t *gRankList=NULL; __global.topology_global.rankList(&gRankList);//valid only after converting to list

  fprintf(stderr,"\n");fprintf(stderr,"local\n");
  TEST_TOPOLOGY(__global.topology_local,__global.topology_local.size());

  fprintf(stderr,"\n");fprintf(stderr,"local converted to list - DO NOT DO THIS TO GLOBAL IN REAL APPS\n");
  __global.topology_local.convertTopology(PAMI_LIST_TOPOLOGY);
  TEST_TOPOLOGY(__global.topology_local,__global.topology_local.size());

  fprintf(stderr,"\n");fprintf(stderr,"global subTopologyNthGlobal(0) - probably not supported on PAMI_LIST_TOPOLOGY?\n");
  __global.topology_global.subTopologyNthGlobal(&subtopology, 0); //0th rank on each locale
  TEST_TOPOLOGY(subtopology,(unsigned)-1);

  fprintf(stderr,"\n");fprintf(stderr,"global subTopologyNthGlobal(1) - probably not supported on PAMI_LIST_TOPOLOGY?\n");
  __global.topology_global.subTopologyNthGlobal(&subtopology, 1); //1st rank on each locale
  TEST_TOPOLOGY(subtopology,(unsigned)-1);

  fprintf(stderr,"\n");fprintf(stderr,"make a global from a list size(%zu)\n",gSize);
  new (&topology) PAMI::Topology(gRankList, (gSize));
  TEST_TOPOLOGY(topology,gSize);

  fprintf(stderr,"\n");fprintf(stderr,"copy the topology\n");
  copy_topology = topology;
  TEST_TOPOLOGY(copy_topology,topology.size());

  fprintf(stderr,"\n");fprintf(stderr,"make a global from a list: coverted to coord\n");
  topology.convertTopology(PAMI_COORD_TOPOLOGY);
  TEST_TOPOLOGY(topology,gSize);

  fprintf(stderr,"\n");fprintf(stderr,"make a global from a list: subTopologyNthGlobal(0)\n");
  topology.subTopologyNthGlobal(&subtopology, 0);
  TEST_TOPOLOGY(subtopology,(unsigned)-1);

  fprintf(stderr,"\n");fprintf(stderr,"make a global from a list: subTopologyNthGlobal(1)\n");
  topology.subTopologyNthGlobal(&subtopology, 1);
  TEST_TOPOLOGY(subtopology,(unsigned)-1);

  fprintf(stderr,"\n");fprintf(stderr,"make a global from a list: coord coverted back to list\n");
  topology.convertTopology(PAMI_LIST_TOPOLOGY);
  TEST_TOPOLOGY(topology,gSize);

  fprintf(stderr,"\n");fprintf(stderr,"make a global with last 1/2 list\n");
  new (&topology) PAMI::Topology(gRankList+(gSize/2), (gSize/2)); // everyone except 0
  TEST_TOPOLOGY(topology,(gSize/2));

  fprintf(stderr,"\n");fprintf(stderr,"make a global with last 1/2 list: coverted to coord\n");
  topology.convertTopology(PAMI_COORD_TOPOLOGY);
  TEST_TOPOLOGY(topology,(gSize/2));

  fprintf(stderr,"\n");fprintf(stderr,"make a global with last 1/2 list: subTopologyNthGlobal(0)\n");
  topology.subTopologyNthGlobal(&subtopology, 0);
  TEST_TOPOLOGY(subtopology,(unsigned)-1);

  fprintf(stderr,"\n");fprintf(stderr,"make a global with last 1/2 list: subTopologyNthGlobal(1)\n");
  topology.subTopologyNthGlobal(&subtopology, 1);
  TEST_TOPOLOGY(subtopology,(unsigned)-1);

  fprintf(stderr,"\n");fprintf(stderr,"make a global with last 1/2 list: coord coverted back to list\n");
  topology.convertTopology(PAMI_LIST_TOPOLOGY);
  TEST_TOPOLOGY(topology,gSize/2);

  fprintf(stderr,"\n");fprintf(stderr,"make a nth topology and manually make a ranklist topology from it\n");
  new (&topology) PAMI::Topology(gRankList, (gSize));
  topology.convertTopology(PAMI_COORD_TOPOLOGY);
  topology.subTopologyNthGlobal(&subtopology, 0);

  pami_task_t *ranklist = new pami_task_t[subtopology.size()];

  // loop all global ranks, if they're in the subtopology, put then in a ranklist.
  for(size_t i = 0, j = 0; i < gSize; ++i)
  {
     if(subtopology.isRankMember(i)) ranklist[j++] = i;
     PAMI_assert(j<=subtopology.size());
  }
  new (&topology) PAMI::Topology(ranklist, subtopology.size());
  // See if we built a good list topology from the Nth global subtopology
  TEST_TOPOLOGY(topology,subtopology.size());

  struct{
    pami_topology_t topology_t_array[11];
    PAMI::Topology topology_array[11];
  } s;

  memset(s.topology_array, -1, (sizeof(PAMI::Topology) * 11));
  for(size_t i = 0; i < 11; ++i)
    new (&s.topology_array[i]) PAMI::Topology(gRankList, (gSize));
  memset(s.topology_t_array, -1, (sizeof(pami_topology_t) * 11));
  for(size_t i = 0; i < 11; ++i)
    new (&s.topology_t_array[i]) PAMI::Topology(gRankList, (gSize));

  fprintf(stderr,"\n");fprintf(stderr,"An array of PAMI::Topology sizeof %zu * 11 = %zu\n",sizeof(PAMI::Topology),sizeof(PAMI::Topology)*11);
  for(size_t i = 0; i < 11; ++i)
    TEST_TOPOLOGY(s.topology_array[i],gSize);

  fprintf(stderr,"\n");fprintf(stderr,"An array of pami_topology_t sizeof %zu * 11 = %zu\n",sizeof(pami_topology_t),sizeof(pami_topology_t)*11);
  for(size_t i = 0; i < 11; ++i)
    TEST_TOPOLOGY((*(PAMI::Topology*)&s.topology_t_array[i]),gSize);

  for(size_t i = 0; i < 11; ++i)
    s.topology_array[i].convertTopology(PAMI_COORD_TOPOLOGY);

  for(size_t i = 0; i < 11; ++i)
    ((PAMI::Topology*)&s.topology_t_array[i])->convertTopology(PAMI_COORD_TOPOLOGY);

  fprintf(stderr,"\n");fprintf(stderr,"An array of PAMI::Topology sizeof %zu * 11 = %zu : converted to PAMI_COORD_TOPOLOGY \n",sizeof(PAMI::Topology),sizeof(PAMI::Topology)*11);
  for(size_t i = 0; i < 11; ++i)
    TEST_TOPOLOGY(s.topology_array[i],gSize);

  fprintf(stderr,"\n");fprintf(stderr,"An array of pami_topology_t sizeof %zu * 11 = %zu : converted to PAMI_COORD_TOPOLOGY \n",sizeof(pami_topology_t),sizeof(pami_topology_t)*11);
  for(size_t i = 0; i < 11; ++i)
    TEST_TOPOLOGY((*(PAMI::Topology*)&s.topology_t_array[i]),gSize);

  PAMI_assertf(sizeof(pami_topology_t) >= sizeof(PAMI::Topology),"sizeof(pami_topology_t) %zu >= %zu sizeof(PAMI::Topology)\n",sizeof(pami_topology_t),sizeof(PAMI::Topology));
  fprintf(stderr,"\n");fprintf(stderr,"DONE\n");
}

// ------------------------------------------------------------------------
  DBG_FPRINTF((stderr, "PAMI_Context_destroyv(&context, 1);\n"));
  status = PAMI_Context_destroyv(&context, 1);
  if(status != PAMI_SUCCESS)
  {
    fprintf(stderr, "Error. Unable to destroy pami context. result = %d\n", status);
    return 1;
  }

  DBG_FPRINTF((stderr, "PAMI_Client_destroy(&client);\n"));
  status = PAMI_Client_destroy(&client);
  if(status != PAMI_SUCCESS)
  {
    fprintf(stderr, "Error. Unable to finalize pami client. result = %d\n", status);
    return 1;
  }

  DBG_FPRINTF((stderr, "return 0;\n"));
  return 0;
}
