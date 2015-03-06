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

#define MAX_THREADS 128

#include "../pami_util.h"
#include <pthread.h>
#include <time.h>

/* #define DEBUG */
/* #define USE_SHUFFLE */
static void *geom_bomb_test(void*);
int    fence_arrivals;

/*
 * Global data shared among the threads
 */
typedef struct geometry_data_t
{
  pami_geometry_t  geometry;
  pami_endpoint_t *eplist;
  unsigned         size;
  pthread_mutex_t  count_lock;
  pthread_cond_t   done;
  volatile int     count;
  volatile int     create_id;
  volatile int     destroy_id;
} geometry_data_t;

/*
 * Thread specific data
 * This includes a pointer to
 * the shared geometry data
 */
typedef struct thread_data_t
{
  pami_client_t    client;
  pami_context_t   context;
  int              thread_id;
  int              task_id;
  int              logical_rank;
  int              logical_size;
  int              num_tasks;
  int              contexts_per_task;
  unsigned         geometry_id;
  int              amMember;
  geometry_data_t *g;
} thread_data_t;

typedef struct cb_data_t
{
  geometry_data_t *gd;
  int              newval;
} cb_data_t;

/* Callbacks */
void cb_geometry_create_done (void *ctxt, void * clientdata, pami_result_t err)
{
  cb_data_t * cb = (cb_data_t*) clientdata;
  cb->gd->create_id = cb->newval;
}

void cb_geometry_destroy_done (void *ctxt, void * clientdata, pami_result_t err)
{
  cb_data_t * cb = (cb_data_t*) clientdata;
  cb->gd->destroy_id = cb->newval;
}

void fence_cb_done (void *ctxt, void * clientdata, pami_result_t err)
{
  int * arrived = (int *) clientdata;
  (*arrived)--;
}

/* Main thread will bootstrap the particpant threads*/
int main(int argc, char*argv[])
{
  setup_env();
  pthread_t threads[MAX_THREADS];
  pami_task_t          task_id;
  size_t               num_tasks;
  size_t               num_ep;
  pami_context_t      *context;
  pami_client_t        client;
  pami_geometry_t      world_geometry;
  size_t               num_algorithm[2];
  pami_algorithm_t    *always_works_algo = NULL;
  pami_metadata_t     *always_works_md   = NULL;
  pami_algorithm_t    *must_query_algo   = NULL;
  pami_metadata_t     *must_query_md     = NULL;
  pami_xfer_type_t     barrier_xfer      = PAMI_XFER_BARRIER;
  pami_xfer_t          barrier;
  volatile unsigned    poll_flag         = 0;
  int                  num_threads       = gNum_contexts;
  fence_arrivals                         = num_threads;

  assert(gNum_contexts > 0);
  context = (pami_context_t*)malloc(sizeof(pami_context_t) * gNum_contexts);
  thread_data_t *td = (thread_data_t*)malloc(sizeof(thread_data_t) * gNum_contexts);
  int rc = pami_init(&client,        /* Client             */
                     context,        /* Context            */
                     NULL,           /* Clientname=default */
                     &gNum_contexts, /* gNum_contexts       */
                     NULL,           /* null configuration */
                     0,              /* no configuration   */
                     &task_id,       /* task id            */
                     &num_tasks);    /* number of tasks    */
  if (rc == 1)
    return 1;

  num_ep = num_tasks *gNum_contexts;

  assert(task_id >= 0);
  assert(task_id < num_tasks);

  geometry_data_t *gd =(geometry_data_t*) malloc(sizeof(geometry_data_t));
  gd->count=0;
  pthread_mutex_init(&gd->count_lock, NULL);
  pthread_cond_init(&gd->done, NULL);

  int i=0;
  printf("Contexts: [ ");
  for(i=0;i<num_threads;i++)
    {
      td[i].client            = client;
      td[i].context           = context[i];
      td[i].thread_id         = i;
      td[i].task_id           = task_id;
      td[i].logical_rank      = task_id*num_threads+i;
      td[i].logical_size      = num_threads*num_tasks;
      td[i].num_tasks         = num_tasks;
      td[i].contexts_per_task = num_threads;
      td[i].geometry_id       = 0;
      td[i].g                 = gd;
      printf("(%d|%d)%p ", i, td[i].logical_rank, context[i]);
    }
  printf("]\n");

  if(task_id == 0) printf("%s:  Querying World Geometry\n", argv[0]);
  rc |= query_geometry_world(client,
                             context[0],
                             &world_geometry,
                             barrier_xfer,
                             num_algorithm,
                             &always_works_algo,
                             &always_works_md,
                             &must_query_algo,
                             &must_query_md);

  if (rc == 1)
    return 1;

  barrier.cb_done   = cb_done;
  barrier.cookie    = (void*) & poll_flag;
  barrier.algorithm = always_works_algo[0];

  assert(gNum_contexts >= num_threads);
  if(task_id == 0) printf("%s:  Tasks:%zu Threads/task:%d Contexts/task:%zu\n",
                          argv[0],num_tasks,num_threads, gNum_contexts);
  int t;
  for(t=0; t<num_threads; t++){
    rc = pthread_create(&threads[t], NULL, geom_bomb_test, (void*)(&td[t]));
    if (rc){
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }

  void* status;
  for(t=0; t<num_threads; t++) {
    rc = pthread_join(threads[t], &status);
    if (rc) {
      printf("ERROR; return code from pthread_join() is %d\n", rc);
      exit(-1);
    }
  }

  blocking_coll(context[0], &barrier, &poll_flag);

  free(always_works_algo);
  free(always_works_md);
  free(must_query_algo);
  free(must_query_md);
  rc |= pami_shutdown(&client, context, &gNum_contexts);
  return rc;
}

/* This creates a random permutation */
/* of the input array                */
void shuffle(int *array, size_t n, unsigned*seed)
{
  if (n > 1)
    {
      unsigned i;
      for (i = 0; i < n - 1; i++)
        {
          unsigned j = i + rand_r(seed) / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

/* This is a utility function for syncronizing the threads
 * but is not used for geometry syncronization among the
 * contexts
 */
static void simple_thread_barrier(thread_data_t   *td)
{
  geometry_data_t*g = td->g;
  pthread_mutex_lock(&g->count_lock);
  g->count++;
  if(g->count == td->contexts_per_task)
    {
      g->count = 0;
      pthread_cond_broadcast(&g->done);
    }
  else
    {
      PAMI_Context_advance (td->context, 1000);
      struct timespec to;
      clock_gettime(CLOCK_REALTIME, &to);
      to.tv_sec+=1;
      int rc = 0;
      while((rc=pthread_cond_wait(&g->done,&g->count_lock)) != 0);
    }
  pthread_mutex_unlock(&g->count_lock);
}

/*
 * Creates a geometry with a random endpoint
 */
static void create_random_geometry(thread_data_t   *td,
                                   unsigned         *seed)
{
  geometry_data_t *g = td->g;
  td->amMember        = 0;
  td->geometry_id++;
  if(td->geometry_id == 1024)
    td->geometry_id=1;
  cb_data_t cd = {g, td->geometry_id};
  
  /* Pick one thread to create the geometry */
  if(td->thread_id == 0)
    {
      int         *task_array    = (int*)malloc(td->num_tasks*sizeof(int));
      int         *context_array = (int*)malloc(td->contexts_per_task*sizeof(int));
      int          i             = 0;
      
      for(i=0; i<td->num_tasks; i++)
        task_array[i] = i;
      for(i=0; i<td->contexts_per_task; i++)
        context_array[i] = i;

      g->size  = td->logical_size;
#ifdef USE_SHUFFLE
      shuffle(task_array, td->num_tasks, seed);
      shuffle(context_array, td->contexts_per_task, seed);
      g->size  = (rand_r(seed)%(sz))+1;
#endif
      g->eplist=(pami_endpoint_t*)malloc(g->size*sizeof(pami_endpoint_t));

      int j,k;
      for(j=0,i=0;j<td->num_tasks && i<g->size; j++)
        for(k=0;k<td->contexts_per_task && i<g->size;k++)
          {
            PAMI_Endpoint_create(td->client,
                                 (pami_task_t)task_array[j],
                                 context_array[k],
                                 &g->eplist[i]);
            if(task_array[j] == td->task_id)
              td->amMember=1;
            i++;
          }
      if(td->amMember == 0)
        {
          free(task_array);
          free(context_array);
          free(g->eplist);
          return;
        }

#ifdef DEBUG
      fprintf(stderr, "%d:Geometry Size is %d,world_sz=%d (id=%d): [ ",
              pthread_self(),g->size, td->logical_size, td->geometry_id);
      for(i=0;i<g->size; i++)
        {
          pami_task_t t;
          size_t      o;
          PAMI_Endpoint_query(g->eplist[i],&t,&o);
          fprintf(stderr, "(%d,%ld) ",t,o);
        }
      fprintf(stderr, "]\n");
#endif
      int rc = PAMI_Geometry_create_endpointlist(td->client,
                                                 NULL,
                                                 0,
                                                 &g->geometry,
                                                 td->geometry_id,
                                                 g->eplist,
                                                 g->size,
                                                 td->context,
                                                 cb_geometry_create_done,
                                                 &cd);
      assert(rc == PAMI_SUCCESS);
      while(g->create_id != td->geometry_id)
        PAMI_Context_advance (td->context, 1);
      free(task_array);
      free(context_array);
#ifdef DEBUG
      fprintf(stderr, "t:%d c:%p tid:%d --> geometry created, entering barrier\n",
              pthread_self(),
              td->context,
              td->thread_id);
#endif
    }
  else
    {
#ifdef DEBUG
      fprintf(stderr, "t:%d c:%p tid:%d --> entering barrier, waiting for geometry create\n",
              pthread_self(),
              td->context,
              td->thread_id);
#endif
      while(g->create_id != td->geometry_id)
        PAMI_Context_advance (td->context, 1);
    }
#ifdef DEBUG
  fprintf(stderr, "t:%d c:%p tid:%d --> create_random_geometry: exit()\n",
          pthread_self(),
          td->context,
          td->thread_id);
#endif

}

/* Destroys a geometry with a random task list */
static void destroy_random_geometry(thread_data_t   *td)
{
  /* One thread destroys the geometry */
  geometry_data_t *g = td->g;
  cb_data_t cd = {g, td->geometry_id};
#ifdef DEBUG
  fprintf(stderr, "tid=%d, Geometry Destroy:  amMember=%d tid=%d\n",
          pthread_self(),
          td->amMember, td->thread_id);
#endif
  if(td->thread_id == 0 && td->amMember)
    {
      PAMI_Geometry_destroy(td->client,
                            &g->geometry,
                            td->context,
                            cb_geometry_destroy_done,
                            &cd);
      
      while(td->geometry_id != g->destroy_id)
        PAMI_Context_advance (td->context, 1);
      free(td->g->eplist);
    }
  else
    {
      while(td->geometry_id != g->destroy_id)
        PAMI_Context_advance (td->context, 1);
    }
}

/* Test case harness */
static void * geom_bomb_test(void* p)
{
  thread_data_t       *td        = (thread_data_t*)p;
  pami_context_t       myContext = (pami_context_t)td->context;
  pami_result_t        rc;
  unsigned             seed      = 4096;
  gContext                       = myContext;

  simple_thread_barrier(td);
  int i, niter=gNiterlat;
  for(i=0; i<niter; i++)
    {
      create_random_geometry(td,&seed);
      destroy_random_geometry(td);
      if(((i%100) == 0) && td->logical_rank==0)
        printf("GEOM BOMB!: Geometry Create Done %d/%d\n",i,niter);
    }
  if(td->logical_rank==0)
    printf("GEOM BOMB!: Geometry Create Done %d/%d\n",i,niter);

  rc = PAMI_Fence_all (myContext,
		       fence_cb_done,
		       &fence_arrivals);

  while (fence_arrivals != 0)
    rc = PAMI_Context_advance (myContext, 1);

  pthread_exit(NULL);
}
