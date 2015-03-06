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
 * \file test/api/p2p/udp_simple_send.cc
 * \brief ???
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <pami.h>

#define ITERATIONS 100
//#define ITERATIONS 1


#define BUFSIZE 255


#define WARMUP

#undef TRACE_ERR
#ifndef TRACE_ERR
#define TRACE_ERR(x)  fprintf x
#endif

volatile unsigned _send_active;
volatile unsigned _recv_active;
volatile unsigned _recv_iteration;
char              _recv_buffer[BUFSIZE] __attribute__ ((__aligned__(16)));

size_t         _dispatch[100];
unsigned       _dispatch_count;

//char buffer[BUFSIZE];

size_t _my_rank;

//typedef struct
//{
//  size_t sndlen;
//} header_t;

void printHexLine2( char * data, size_t num, size_t pad )
{
  size_t i;
  std::cout << "<x>: " << (void *)data << ": ";
  for ( i=0; i<num; i++ )
  {
    printf("%02x ", (uint8_t)data[i]);
  }
  for ( i=0; i<pad; i++ )
  {
    printf("   ");
  }
  printf(" ");
  for ( i=0; i<num; i++ )
  {
    if ( isgraph((unsigned)data[i]))
    {
      printf ("%c", data[i] );
    } else {
      printf(".");
    }
  }
  printf("\n");
}
/* --------------------------------------------------------------- */

static void rcvdecrement (pami_context_t   context,
                       void          * cookie,
                       pami_result_t    result)
{
  unsigned * value = (unsigned *) cookie;
  TRACE_ERR((stderr, "(%zu) ***** in rcvdecrement() cookie = %p, %d => %d\n", _my_rank, cookie, *value, *value-1));
  std::cout << _recv_buffer << std::endl;
  --*value;
}

static void snddecrement (pami_context_t   context,
                       void          * cookie,
                       pami_result_t    result)
{
  unsigned * value = (unsigned *) cookie;
  TRACE_ERR((stderr, "(%zu) ***** in snddecrement() cookie = %p, %d => %d\n", _my_rank, cookie, *value, *value-1));
  --*value;
}

/* --------------------------------------------------------------- */
static void test_dispatch (
    pami_context_t        context,      /**< IN: PAMI context */
    void               * cookie,       /**< IN: dispatch cookie */
    const void         * header_addr,  /**< IN: header address */
    size_t               header_size,  /**< IN: header size */
    const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
    size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
    pami_endpoint_t origin,
pami_recv_t         * recv)        /**< OUT: receive message structure */
{
   std::cout << "****** in test_dispatch() " << std::endl;
  if (pipe_addr != NULL)
  {
    memcpy (_recv_buffer, pipe_addr, pipe_size);
    std::cout << "Msg = " << _recv_buffer << std::endl;
    unsigned * value = (unsigned *) cookie;
    TRACE_ERR((stderr, "(%zu) ****** in test_dispatch() short recv:  cookie = %p, decrement: %d => %d\n", _my_rank, cookie, *value, *value-1));
    --*value;
  }
  else
  {
    //header_t * header = (header_t *) header_addr;
    TRACE_ERR((stderr, "(%zu) ****** in test_dispatch() async recv:  cookie = %p, pipe_size = %zu\n", _my_rank, cookie, pipe_size));

    recv->local_fn = rcvdecrement;
    recv->cookie   = cookie;
    recv->type     = PAMI_TYPE_BYTE;
    recv->addr     = _recv_buffer;
    recv->offset   = 0;

  }

  _recv_iteration++;
}

void send_once (pami_context_t context, pami_send_t * parameters)
{
 std::cout << __FILE__ << ":" << __LINE__ << std::endl;
  //pami_result_t result =
    PAMI_Send (context, parameters);
  TRACE_ERR((stderr, "(%zu) send_once() Before advance\n", _my_rank));
  while (_send_active) PAMI_Context_advance (context, 100);
  _send_active = 1;
  TRACE_ERR((stderr, "(%zu) send_once()  After advance\n", _my_rank));
}

void recv_once (pami_context_t context)
{
 std::cout << __FILE__ << __LINE__ << std::endl;
  TRACE_ERR((stderr, "(%zu) recv_once() Before advance\n", _my_rank));
  while (_recv_active) PAMI_Context_advance (context, 100);
  //print received buffer
  fprintf (stdout, "\n Received Message = %s\n",_recv_buffer);
  printHexLine2( _recv_buffer, 16, 0 );
  fflush (stdout);
  _recv_active = 1;
  TRACE_ERR((stderr, "(%zu) recv_once()  After advance\n", _my_rank));
}

unsigned long long test (pami_context_t context, size_t dispatch, size_t hdrsize, size_t sndlen, size_t myrank, pami_endpoint_t origin, pami_endpoint_t target)
{
  std::cout << __FILE__ << __LINE__ << std::endl;
  _recv_active = 1;
  _recv_iteration = 0;
  _send_active = 1;

  char metadata[BUFSIZE];
  char buffer[255];
  size_t sndlen1;
  unsigned i;
  pami_send_t parameters;

  if (myrank == 0)
  {
    fprintf (stdout, "\n Enter the message to send: ");
    fflush (stdout);
    fgets(buffer,sizeof(buffer),stdin);
    //scanf("%s",buffer);
    sndlen1 =strlen(buffer);
    printHexLine2( buffer, sndlen1, 0 );

    TRACE_ERR((stderr, "(%zu) Do test ... sndlen = %zu\n", myrank, sndlen1));

    //header_t header;
    //header.sndlen = sndlen1;


    parameters.send.dispatch        = dispatch;
    parameters.send.header.iov_base = metadata;
    parameters.send.header.iov_len  = hdrsize;
    parameters.send.data.iov_base   = buffer;
    parameters.send.data.iov_len    = sndlen1;
    parameters.events.cookie        = (void *) &_send_active;
    parameters.events.local_fn      = snddecrement;
    parameters.events.remote_fn     = NULL;

    //barrier ();



    parameters.send.dest = target;
    for (i = 0; i < ITERATIONS; i++)
    {
      TRACE_ERR((stderr, "(%zu) Starting Iteration %d of size %d\n", myrank, i, ITERATIONS));
      send_once (context, &parameters);
    }
  }
  else if (myrank == 1)
  {
    for (i = 0; i < ITERATIONS; i++)
    {
      TRACE_ERR((stderr, "(%zu) Starting Iteration %d of size %d\n", myrank, i, ITERATIONS));
      recv_once (context);
    }
  }

  return 0;
}

int main (int argc, char ** argv)
{
 std::cout << __FILE__ << __LINE__ << std::endl;
  TRACE_ERR((stderr, "Start test ...\n"));

  size_t hdrsize =0;


  char clientname[] = "PAMI";
  pami_client_t client;
  TRACE_ERR((stderr, "... before PAMI_Client_create()\n"));
  PAMI_Client_create (clientname, &client, NULL, 0);
  TRACE_ERR((stderr, "...  after PAMI_Client_create()\n"));
  pami_context_t context;
  TRACE_ERR((stderr, "... before PAMI_Context_createv()\n"));
  { size_t _n = 1; PAMI_Context_createv (client, NULL, 0, &context, _n); }
  TRACE_ERR((stderr, "...  after PAMI_Context_createv()\n"));

  //TRACE_ERR((stderr, "... before barrier_init()\n"));
  //barrier_init (client, context, 0);
  //TRACE_ERR((stderr, "...  after barrier_init()\n"));


  /* Register the protocols to test */
  unsigned i = 0;
  _dispatch_count = 0;

  _dispatch[_dispatch_count] = _dispatch_count + 1;

  pami_dispatch_callback_function fn;
  fn.p2p = test_dispatch;
  pami_dispatch_hint_t options={};
  TRACE_ERR((stderr, "Before PAMI_Dispatch_set() .. &_recv_active = %p, recv_active = %d\n", &_recv_active, _recv_active));
  pami_result_t result = PAMI_Dispatch_set (context,
                                          _dispatch[_dispatch_count++],
                                          fn,
                                          (void *)&_recv_active,
                                          options);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable register pami dispatch. result = %d\n", result);
    return 1;
  }

  pami_configuration_t configuration;

  configuration.name = PAMI_CLIENT_TASK_ID;
  result = PAMI_Client_query(client, &configuration,1);
  _my_rank = configuration.value.intval;
  std::cout << "Rank = " << _my_rank << std::endl;

  configuration.name = PAMI_CLIENT_NUM_TASKS;
  result = PAMI_Client_query(client, &configuration,1);
  size_t num_tasks = configuration.value.intval;
  std::cout << "Size = " << num_tasks << std::endl;

  configuration.name = PAMI_CLIENT_WTICK;
  result = PAMI_Client_query(client, &configuration,1);
  //double tick = configuration.value.doubleval;

   size_t val = argc;

  pami_endpoint_t origin, target;
  PAMI_Endpoint_create (client, 0, 0, &origin);
  PAMI_Endpoint_create (client, 1, 0, &target);


   if (argc==1)
       val=1;
   else
       val =atoi(argv[1]) ;

        fprintf (stdout, "** The test will run %zu times ***\n", val);
    fflush(stdout);

  for(i=0;i<val;i++){

//	  /* Display some test header information */
//	  if (_my_rank == 0)
//	  {

                   test (context, _dispatch[0], hdrsize, 0, _my_rank, origin, target);

//	  }
  }
        fprintf (stdout, "** Test completed. **\n");

  PAMI_Client_destroy(&client);

  return 0;
}
#undef TRACE_ERR
