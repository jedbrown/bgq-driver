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
 * \file test/api/extensions/collsel/table_generate.c
 * \brief Table generate api test case
 */

#include <stdio.h>
#include <pami.h>

typedef void* advisor_t;
typedef void* advisor_table_t;
typedef enum {
  dummy,
} advisor_attribute_name_t;

typedef union
{
  size_t         intval;
  double         doubleval;
  const char *   chararray;
  const size_t * intarray;
} advisor_attribute_value_t;

typedef struct
{
  advisor_attribute_name_t  name;
  advisor_attribute_value_t value;
} advisor_configuration_t;

typedef struct {
   pami_xfer_type_t  *collectives;        /** List of collectives to benchmark */
   size_t             num_collectives;    /** Size of collectives list */
   size_t            *geometry_sizes;     /** List of geometry sizes */
   size_t             num_geometry_sizes; /** Size of geometry list */
   size_t            *message_sizes;      /** List of message sizes */
   size_t             num_message_sizes;  /** Message sizes list size */
   int                iter;
   int                verify;
   int                verbose;
} advisor_params_t;

typedef pami_result_t (*pami_extension_collsel_init) (pami_client_t, advisor_configuration_t [], size_t, \
					            pami_context_t [], size_t, advisor_t *);
typedef pami_result_t (*pami_extension_collsel_table_generate) (advisor_t, char *, advisor_params_t *, int);
typedef pami_result_t (*pami_extension_collsel_destroy) (advisor_t *);


int main(int argc, char ** argv)
{
  pami_client_t client;
  pami_context_t context;
  pami_result_t status = PAMI_ERROR;

  status = PAMI_Client_create("TEST", &client, NULL, 0);
  if(status != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to initialize pami client. result = %d\n", status);
    return 1;
  }

  status = PAMI_Context_createv(client, NULL, 0, &context, 1);
  if(status != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to create pami context. result = %d\n", status);
    return 1;
  }

  /* ------------------------------------------------------------------------ */

  pami_extension_t extension;
  status = PAMI_Extension_open (client, "EXT_collsel", &extension);
  if(status != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. The \"EXT_collsel\" extension is not implemented. result = %d\n", status);
    return 1;
  }

  //printf("before table init\n");
  pami_extension_collsel_init pamix_collsel_init = 
    (pami_extension_collsel_init) PAMI_Extension_symbol (extension, "Collsel_init_fn");
  if (pamix_collsel_init == (void *)NULL)
  {
    fprintf (stderr, "Error. The \"EXT_collsel\" extension function \"Collsel_init_fn\" is not implemented. result = %d\n", status);
    return 1;
  }
  advisor_t advisor;
  advisor_configuration_t configuration[1];
  status = pamix_collsel_init (client, configuration, 1, &context, 1, &advisor);

  pami_extension_collsel_table_generate pamix_collsel_table_generate = 
    (pami_extension_collsel_table_generate) PAMI_Extension_symbol (extension, "Collsel_table_generate_fn");
  if (pamix_collsel_table_generate == (void *)NULL)
  {
    fprintf (stderr, "Error. The \"EXT_collsel\" extension function \"Collsel_table_generate_fn\" is not implemented. result = %d\n", status);
    return 1;
  }
  advisor_params_t params;
  int i = 0;
  //do a bcast on 16 np 1000 bytes
  params.collectives = (pami_xfer_type_t *)malloc(sizeof(pami_xfer_type_t) * 32);
  params.collectives[i++] = PAMI_XFER_BROADCAST;
  params.collectives[i++] = PAMI_XFER_ALLREDUCE;
  params.collectives[i++] = PAMI_XFER_REDUCE;
  params.collectives[i++] = PAMI_XFER_ALLGATHER;
  params.collectives[i++] = PAMI_XFER_ALLGATHERV;
  params.collectives[i++] = PAMI_XFER_ALLGATHERV_INT;
  params.collectives[i++] = PAMI_XFER_SCATTER;
  params.collectives[i++] = PAMI_XFER_SCATTERV;
  params.collectives[i++] = PAMI_XFER_SCATTERV_INT;
  params.collectives[i++] = PAMI_XFER_GATHER;
  params.collectives[i++] = PAMI_XFER_GATHERV;
  params.collectives[i++] = PAMI_XFER_GATHERV_INT;
  params.collectives[i++] = PAMI_XFER_BARRIER;
  params.collectives[i++] = PAMI_XFER_ALLTOALL;
  params.collectives[i++] = PAMI_XFER_ALLTOALLV;
  params.collectives[i++] = PAMI_XFER_ALLTOALLV_INT;
  params.collectives[i++] = PAMI_XFER_SCAN;
  params.collectives[i++] = PAMI_XFER_REDUCE_SCATTER;
  params.num_collectives = i;

  params.geometry_sizes = (size_t *)malloc(sizeof(size_t));
  params.geometry_sizes[0] =512;
  params.num_geometry_sizes = 1;
  params.message_sizes = (size_t *)malloc(sizeof(size_t));
  //params.message_sizes[0] = 2000;
  params.message_sizes[0] = 10000;
  params.num_message_sizes = 1;
  params.iter = 2;
  params.verify = 0;
  params.verbose = 0;
  //params.verify = 1;
  //params.verbose = 1;
  status = pamix_collsel_table_generate (advisor, NULL, &params, 1);

  pami_extension_collsel_destroy pamix_collsel_destroy =
    (pami_extension_collsel_destroy) PAMI_Extension_symbol (extension, "Collsel_destroy_fn");
  if (pamix_collsel_destroy == (void *)NULL)
  {
    fprintf (stderr, "Error. The \"EXT_collsel\" extension function \"Collsel_destroy_fn\" is not implemented. result = %d\n", status);
    return 1;
  }
  status = pamix_collsel_destroy (advisor);
  //printf("after collsel destroy\n");

  free(params.collectives);
  free(params.geometry_sizes);
  free(params.message_sizes);

  status = PAMI_Extension_close (extension);
  if(status != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. The \"EXT_collsel\" extension could not be closed. result = %d\n", status);
    return 1;
  }
  //printf("after extension close\n");


  /* ------------------------------------------------------------------------ */
  status = PAMI_Context_destroyv(&context, 1);
  if(status != PAMI_SUCCESS)
  {
    fprintf(stderr, "Error. Unable to destroy pami context. result = %d\n", status);
    return 1;
  }
  //printf("after context destroy\n");

  status = PAMI_Client_destroy(&client);
  if(status != PAMI_SUCCESS)
  {
    fprintf(stderr, "Error. Unable to finalize pami client. result = %d\n", status);
    return 1;
  }
  //printf("after client destroy\n");

  return 0;
}
