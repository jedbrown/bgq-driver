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
 * \file test/api/extensions/torus/extension_torus.c
 * \brief Simple test of the PAMI "test" extension
 */

#include <stdio.h>
#include <pami.h>


typedef struct pami_extension_torus_information
{
  size_t   dims;
  size_t * coord;
  size_t * size;
  size_t * torus;
} pami_extension_torus_information_t;

typedef const pami_extension_torus_information_t * (*pami_extension_torus_information_fn) ();
typedef pami_result_t (*pami_extension_torus_task2torus_fn) (pami_task_t, size_t[]);
typedef pami_result_t (*pami_extension_torus_torus2task_fn) (size_t[], pami_task_t *);

#define DBG_FPRINTF(x) /*fprintf x */

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
  DBG_FPRINTF((stderr,"Client %p\n",client));

  status = PAMI_Context_createv(client, NULL, 0, &context, 1);
  if(status != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to create pami context. result = %d\n", status);
    return 1;
  }

  /* ------------------------------------------------------------------------ */

  pami_extension_t extension;
  status = PAMI_Extension_open (client, "EXT_torus_network", &extension);
  if(status != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. The \"EXT_torus_network\" extension is not implemented. result = %d\n", status);
    return 1;
  }

  pami_extension_torus_information_fn pamix_torus_info =
    (pami_extension_torus_information_fn) PAMI_Extension_symbol (extension, "information");
  if (pamix_torus_info == (void *)NULL)
  {
    fprintf (stderr, "Error. The \"EXT_torus_network\" extension function \"information\" is not implemented. result = %d\n", status);
    return 1;
  }

  const pami_extension_torus_information_t * info = pamix_torus_info ();

  fprintf (stdout, "Torus Dimensions:  %zu\n", info->dims);

  char str[1024];
  size_t i, nchars;

  for (nchars=i=0; i<(info->dims-1); i++)
    nchars += snprintf (&str[nchars],1023-nchars, "%zu,", info->coord[i]);
  nchars += snprintf (&str[nchars],1023-nchars, "%zu", info->coord[info->dims-1]);
  fprintf (stdout, "Torus Coordinates: [%s]\n", str);

  for (nchars=i=0; i<(info->dims-1); i++)
    nchars += snprintf (&str[nchars],1023-nchars, "%zu,", info->size[i]);
  nchars += snprintf (&str[nchars],1023-nchars, "%zu", info->size[info->dims-1]);
  fprintf (stdout, "Torus Size:        [%s]\n", str);

  for (nchars=i=0; i<(info->dims-1); i++)
    nchars += snprintf (&str[nchars],1023-nchars, "%zu,", info->torus[i]);
  nchars += snprintf (&str[nchars],1023-nchars, "%zu", info->torus[info->dims-1]);
  fprintf (stdout, "Torus Wrap:        [%s]\n", str);



  pami_extension_torus_task2torus_fn pamix_torus_task2torus =
    (pami_extension_torus_task2torus_fn) PAMI_Extension_symbol (extension, "task2torus");
  if (pamix_torus_task2torus == (void *)NULL)
  {
    fprintf (stderr, "Error. The \"EXT_torus_network\" extension function \"task2torus\" is not implemented. result = %d\n", status);
    return 1;
  }

  pami_task_t task = 1;
  size_t * coord = (size_t *) malloc (sizeof(size_t) * info->dims);
  status = pamix_torus_task2torus (task, coord);
  if (status != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error.  Unable to query the torus coordinates of task 1\n");
    return 1;
  }

  for (nchars=i=0; i<(info->dims-1); i++)
    nchars += snprintf (&str[nchars],1023-nchars, "%zu,", coord[i]);
  nchars += snprintf (&str[nchars],1023-nchars, "%zu", coord[i]);
  fprintf (stdout, "Task 1 Torus Coordinates:     [%s]\n", str);


  pami_extension_torus_torus2task_fn pamix_torus_torus2task =
    (pami_extension_torus_torus2task_fn) PAMI_Extension_symbol (extension, "torus2task");
  if (pamix_torus_torus2task == (void *)NULL)
  {
    fprintf (stderr, "Error. The \"EXT_torus_network\" extension function \"torus2task\" is not implemented. result = %d\n", status);
    return 1;
  }

  /*coord[0] = 0; */
  /*coord[1] = 0; */
  /*coord[2] = 0; */
  /*coord[3] = 1; */
  status = pamix_torus_torus2task (coord, &task);
  if (status != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error.  Unable to query the task for coordinates [%zu,%zu,%zu,%zu]\n",coord[0],coord[1],coord[2],coord[3]);
    return 1;
  }
  for (nchars=i=0; i<(info->dims-1); i++)
    nchars += snprintf (&str[nchars],1023-nchars, "%zu,", coord[i]);
  nchars += snprintf (&str[nchars],1023-nchars, "%zu", coord[i]);
  fprintf (stdout, "Task at Torus Coordinates [%s]: %d\n", str, task);;






  status = PAMI_Extension_close (extension);
  if(status != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. The \"EXT_torus_network\" extension could not be closed. result = %d\n", status);
    return 1;
  }


  /* ------------------------------------------------------------------------ */
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
