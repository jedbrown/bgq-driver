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
 * \file api/extensions/collsel/pami_tune.c
 * \brief Run collectives benchmark utilizing the PAMI collsel extension
 */

#include <pami.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

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
   size_t            *procs_per_node;     /** List of processes per node */
   size_t             num_procs_per_node; /** Size of ppn list */
   size_t            *geometry_sizes;     /** List of geometry sizes */
   size_t             num_geometry_sizes; /** Size of geometry list */
   size_t            *message_sizes;      /** List of message sizes */
   size_t             num_message_sizes;  /** Message sizes list size */
   int                iter;
   int                verify;
   int                verbose;
   int                checkpoint;
} advisor_params_t;

/* Initialize collective selection extension */
typedef pami_result_t (*pami_extension_collsel_init) (pami_client_t,
                                                      advisor_configuration_t [],
                                                      size_t,
                                                      pami_context_t [],
                                                      size_t,
                                                      advisor_t *);
/* Run the collectives benchmark and save the results in XML format */
typedef pami_result_t (*pami_extension_collsel_table_generate) (advisor_t, char *,
                                                                advisor_params_t *,
                                                                int);
/* Finalize the collective selection extension */
typedef pami_result_t (*pami_extension_collsel_destroy) (advisor_t *);

const char *         xfer_array_str[PAMI_XFER_COUNT];
int task_id;
int num_tasks;


static int print_usage()
{
  if(!task_id)
    fputs("Usage: pami_tune [options]\n\
Options:\n\
  -c            Comma separated list of collectives to benchmark\n\
                Valid options are: \n\
                   allgather, allgatherv, allgatherv_int, allreduce, alltoall,\n\
                   alltoallv, alltoallv_int, ambroadcast, amgather, amreduce,\n\
                   amscatter, barrier, broadcast, gather, gatherv, gatherv_int,\n\
                   reduce, reduce_scatter, scan, scatter, scatterv, scatterv_int\n\
                   (Default: all collectives)\n\n\
  -m            Comma separated list of message sizes to benchmark\n\
                (Default: 1 to 2^k, where k <= 20)\n\n\
  -g            Comma separated list of geometry sizes to benchmark\n\
                (Default: 2 to 2^k, where k <= world geometry size)\n\n\
  -i            Number of benchmark iterations per algorithm\n\
                (Default: 100)\n\n\
  -f <file>     Input file containing benchmark parameters\n\
                You can override a parameter with a command line argument\n\n\
  -o <file>     Output XML file containing benchmark results\n\
                (Default: pami_tune_results.xml)\n\n\
  -d            Diagnostics mode. Verify correctness of collective algorithms\n\
                (Default: Disabled)\n\n\
  -v            Verbose mode\n\
                (Default: Disabled)\n\n\
  -x            Checkpoint mode. Enable pami_tune checkpointing\n\
                (Default: Disabled)\n\n\
  -h            Print this help message\n\n", stdout);

  return 0;
}

static void init_advisor_params(advisor_params_t *params)
{
  params->num_collectives = 0;
  params->num_procs_per_node = 0;
  params->num_geometry_sizes = 0;
  params->num_message_sizes = 0;
  params->collectives = NULL;
  params->procs_per_node = NULL;
  params->geometry_sizes = NULL;
  params->message_sizes = NULL;
  params->iter = 100;
  /* Set the following to -1, so that we can
     check if the user has set them or not */
  params->verify = -1;
  params->verbose = -1;
  params->checkpoint = -1;
}

static void free_advisor_params(advisor_params_t *params)
{
  if(params->collectives) free(params->collectives);
  if(params->procs_per_node) free(params->procs_per_node);
  if(params->geometry_sizes) free(params->geometry_sizes);
  if(params->message_sizes) free(params->message_sizes);
}

static void init_xfer_tables()
{
  xfer_array_str[PAMI_XFER_BROADCAST]     ="broadcast";
  xfer_array_str[PAMI_XFER_ALLREDUCE]     ="allreduce";
  xfer_array_str[PAMI_XFER_REDUCE]        ="reduce";
  xfer_array_str[PAMI_XFER_ALLGATHER]     ="allgather";
  xfer_array_str[PAMI_XFER_ALLGATHERV]    ="allgatherv";
  xfer_array_str[PAMI_XFER_ALLGATHERV_INT]="allgatherv_int";
  xfer_array_str[PAMI_XFER_SCATTER]       ="scatter";
  xfer_array_str[PAMI_XFER_SCATTERV]      ="scatterv";
  xfer_array_str[PAMI_XFER_SCATTERV_INT]  ="scatterv_int";
  xfer_array_str[PAMI_XFER_GATHER]        ="gather";
  xfer_array_str[PAMI_XFER_GATHERV]       ="gatherv";
  xfer_array_str[PAMI_XFER_GATHERV_INT]   ="gatherv_int";
  xfer_array_str[PAMI_XFER_BARRIER]       ="barrier";
  xfer_array_str[PAMI_XFER_ALLTOALL]      ="alltoall";
  xfer_array_str[PAMI_XFER_ALLTOALLV]     ="alltoallv";
  xfer_array_str[PAMI_XFER_ALLTOALLV_INT] ="alltoallv_int";
  xfer_array_str[PAMI_XFER_SCAN]          ="scan";
  xfer_array_str[PAMI_XFER_REDUCE_SCATTER]="reduce_scatter";
  xfer_array_str[PAMI_XFER_AMBROADCAST]   ="ambroadcast";
  xfer_array_str[PAMI_XFER_AMSCATTER]     ="amscatter";
  xfer_array_str[PAMI_XFER_AMGATHER]      ="amgather";
  xfer_array_str[PAMI_XFER_AMREDUCE]      ="amreduce";
}



static int process_collectives(char *coll_arg, advisor_params_t *params)
{
  int i, ret = 0, arg_len = strlen(coll_arg);
  char *collectives = (char *) malloc(arg_len + 1);
  char *coll;
  /* if already set via config file, free it */
  if(params->collectives)
  {
    free(params->collectives);
    params->num_collectives = 0;
  }
  /* Allocating some extra space should be fine */
  params->collectives = (pami_xfer_type_t *)malloc(sizeof(pami_xfer_type_t)*PAMI_XFER_COUNT);

  strcpy(collectives, coll_arg);
  coll = strtok(collectives,",");
  while (coll != NULL)
  {
    for(i=0; i<PAMI_XFER_COUNT; i++)
    {
      if(strcmp(coll, xfer_array_str[i]) == 0)
      {
        params->collectives[params->num_collectives++] = i;
        break;
      }
    }
    /* arg did not match any collective */
    if(i == PAMI_XFER_COUNT)
    {
      free(params->collectives);
      params->collectives = NULL;
      if(!task_id)
      {
        fprintf(stderr, "Invalid collective: %s\n", coll);
      }
      ret = 1;
      break;
    }
    coll = strtok(NULL,",");
  }
  free(collectives);
  return ret;
}


static int process_msg_sizes(char *msg_sizes_arg, advisor_params_t *params)
{
  int ret = 0, arg_len = strlen(msg_sizes_arg);
  char *msg_sizes = (char *) malloc(arg_len + 1);
  char *msg_sz;
  size_t tmp;
  /* if already set via config file, free it */
  if(params->message_sizes)
  {
    free(params->message_sizes);
    params->num_message_sizes = 0;
  }
  /* Allocating some extra space should be fine */
  params->message_sizes = (size_t *)malloc(sizeof(size_t) * 50);

  strcpy(msg_sizes, msg_sizes_arg);
  msg_sz = strtok(msg_sizes,",");
  while (msg_sz != NULL)
  {
    tmp = strtol(msg_sz, NULL, 10);
    if(tmp == 0)
    {
      free(params->message_sizes);
      params->message_sizes = NULL;
      if(!task_id)
      {
        fprintf(stderr, "Invalid message size: %s\n", msg_sz);
      }
      ret = 1;
      break;
    }

    params->message_sizes[params->num_message_sizes++] = tmp;
    msg_sz = strtok(NULL,",");
  }
  free(msg_sizes);
  return ret;
}

static int process_geo_sizes(char *geo_sizes_arg, advisor_params_t *params)
{
  int ret = 0, arg_len = strlen(geo_sizes_arg);
  char *geo_sizes = (char *) malloc(arg_len + 1);
  char *geo_sz;
  size_t tmp;
  /* if already set via config file, free it */
  if(params->geometry_sizes)
  {
    free(params->geometry_sizes);
    params->num_geometry_sizes = 0;
  }
  /* Allocating some extra space should be fine */
  params->geometry_sizes = (size_t *)malloc(sizeof(size_t) * 50);

  strcpy(geo_sizes, geo_sizes_arg);
  geo_sz = strtok(geo_sizes,",");
  while (geo_sz != NULL)
  {
    tmp = strtol(geo_sz, NULL, 10);
    if(tmp == 0 || tmp > num_tasks)
    {
      free(params->geometry_sizes);
      params->geometry_sizes = NULL;
      if(!task_id)
      {
        fprintf(stderr, "Invalid geometry size: %s\n", geo_sz);
      }
      ret = 1;
      break;
    }

    params->geometry_sizes[params->num_geometry_sizes++] = tmp;
    geo_sz = strtok(NULL,",");
  }
  free(geo_sizes);
  return ret;
}

static int process_output_file(char *filename, char **out_file)
{
  char *newname;
  int i, filename_len, ret = 0;

  filename_len = strlen(filename);

  /* Check if file already exists */
  if(access(filename, F_OK) == 0)
  {
    fprintf(stderr, "File %s already exists, renaming existing file\n", filename);
    newname = (char *) malloc(filename_len + 4);
    for (i = 0; i < 100; ++i)
    {
      sprintf(newname,"%s.%d", filename, i);
      if(!(access(newname, F_OK) == 0))
      {
        ret = rename(filename, newname);
        break;
      }
    }
    free(newname);
    if(i == 50 || ret != 0)
    {
      fprintf(stderr, "Error renaming file\n");
      return 1;
    }
  }
  /* if file name is already set via config file, free it */
  if(*out_file) free(*out_file);
  *out_file = (char *) malloc(filename_len + 1);
  strcpy(*out_file, filename);

  return ret;
}

static char* ltrim(char *line)
{
  while(*line && isspace(*line))
    ++line;

  return line;
}

static char* rtrim(char *line)
{
  char *end = line + strlen(line);
  while(end > line && isspace(*--end))
    *end = '\0';

  return line;
}

static int checkvalue(char *name, char *value, const char *filename, int *ival)
{
  int ret = 0;
  char *tmp;
  *ival = (int)strtol(value, &tmp, 10);
  if(*ival != 1)
  {
    if((*ival == 0 && value == tmp)|| *ival != 0)
    {
      if(!task_id)
        fprintf(stderr, "Invalid value for %s parameter: %s in file: %s\n", name, value, filename);
      ret = 1;
    }
  }
  return ret;
}

static int process_ini_file(const char *filename, advisor_params_t *params, char **out_file)
{
  char *line, *start, *name, *value;
  int ret = 0;
  FILE *file = fopen(filename, "r");
  if(!file)
  {
    fprintf(stderr, "Error. Can't open file %s\n", filename);
    return 1;
  }
  line = (char *) malloc(1000);

  while (fgets(line, 1000, file) != NULL)
  {
    start = ltrim(rtrim(line));
    /* Skip comments and sections */
    if(*start == ';' || *start == '[' || *start == '#')
      continue;
    name = strtok(line, "=");
    if(name == NULL) continue;
    value = strtok(NULL, "=");
    if(value == NULL) continue;
    name  = rtrim(name);
    value = ltrim(value);
    /* Do not override command line values if they are set */
    if(strcmp(name, "collectives") == 0)
    {
      if(!params->collectives)
        ret = process_collectives(value, params);
    }
    else if(strcmp(name, "message_sizes") == 0)
    {
      if(!params->message_sizes)
        ret = process_msg_sizes(value, params);
    }
    else if(strcmp(name, "geometry_sizes") == 0)
    {
      if(!params->geometry_sizes)
        ret = process_geo_sizes(value, params);
    }
    else if(strcmp(name, "output_file") == 0)
    {
      if(!*out_file && !task_id) /* Only task 0 creates o/p file */
        ret = process_output_file(value, out_file);
    }
    else if(strcmp(name, "iterations") == 0)
    {
      if(params->iter == 100)
      {
        params->iter = atoi(value);
        if(params->iter <= 0)
        {
          if(!task_id)
            fprintf(stderr, "Invalid iteration count: %s in file: %s\n", value, filename);
          ret = 1;
        }
      }
    }
    else if(strcmp(name, "verbose") == 0)
    {
      if(params->verbose == -1)
        ret = checkvalue(name, value, filename, &params->verbose);
    }
    else if(strcmp(name, "diagnostics") == 0)
    {
      if(params->verify == -1)
        ret = checkvalue(name, value, filename, &params->verify);
    }
    else if(strcmp(name, "checkpoint") == 0)
    {
      if(params->checkpoint == -1)
        ret = checkvalue(name, value, filename, &params->checkpoint);
    }
    else
    {
      fprintf(stderr, "Invalid parameter: %s in file: %s\n", name, filename);
      ret = 1;
    }
    if(ret) break;
  }
  free(line);
  fclose(file);

  return ret;
}


static int process_arg(int argc, char *argv[], advisor_params_t *params, char ** out_file)
{
   int i,c,ret = 0;

   init_xfer_tables();
   params->verify = 0;

   size_t *ppn;
   ppn = (size_t*)realloc(params->procs_per_node, 1*sizeof(size_t));
   if (ppn == NULL) printf("allocating for procs_per_node failed\n");

   ppn[0] = 1;
   params->num_procs_per_node = 1;
   params->procs_per_node = ppn;

   opterr = 0;
   while ((c = getopt (argc, argv, "c:m:g:f:o:i:v::d::x::h::")) != -1)
   {
     switch (c)
     {
       case 'c':
         ret = process_collectives(optarg, params);
         break;
       case 'm':
         ret = process_msg_sizes(optarg, params);
         break;
       case 'g':
         ret = process_geo_sizes(optarg, params);
         break;
       case 'f':
         ret = process_ini_file(optarg, params, out_file);
         break;
       case 'o':
         if(!task_id) /* Only task 0 creates o/p file */
           ret = process_output_file(optarg, out_file);
         break;
       case 'i':
         params->iter = atoi(optarg);
         if(params->iter <= 0)
         {
           if(!task_id)
             fprintf(stderr, "Invalid iteration count %s\n", optarg);
           ret = 1;
         }
         break;
       case 'd':
         params->verify = 1;
         break;
       case 'v':
         params->verbose = 1;
         break;
       case 'x':
         params->checkpoint = 1;
         break;
       case 'h':
         ret = 1;
         break;
       case '?':
         if(!task_id)
         {
           if (optopt == 'c' || optopt == 'm' || optopt == 'g' ||
               optopt == 'f' || optopt == 'o' || optopt == 'i')
           {
             fprintf (stderr, "Option -%c requires an argument.\n", optopt);
           }
           else if (isprint (optopt))
           {
             fprintf (stderr, "Unknown option `-%c'.\n", optopt);
           }
           else
           {
             fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
           }
         }
         ret = 1;
         break;
       default:
         abort();
         break;
     }
     if(ret) return ret;
   }
   if(!task_id)
   {
     if (optind < argc)
     {
       printf ("Non-option arguments: ");
       while (optind < argc)
         printf ("%s ", argv[optind++]);
       printf ("\n");
     }
   }

   /* If user did not specify any collectives, benchmark all */
   if(params->num_collectives == 0)
   {
     params->collectives = (pami_xfer_type_t *)malloc(sizeof(pami_xfer_type_t)*PAMI_XFER_COUNT);
     for(i = 0; i < PAMI_XFER_COUNT; i++)
     {
       params->collectives[params->num_collectives++] = i;
     }
   }
   /* If user did not set any of the following parameters, disable them */
   if(params->verbose == -1) params->verbose = 0;
   if(params->verify == -1) params->verify = 0;
   if(params->checkpoint == -1) params->checkpoint = 0;
   return 0;
}

static void print_params(advisor_params_t *params, char *output_file)
{
  size_t i;
  printf("  Benchmark Parameters\n");
  printf("  --------------------\n");

  printf("  Collectives:\n   ");
  for(i=0; i<params->num_collectives; i++){
    printf(" %s", xfer_array_str[params->collectives[i]]);
  }
  printf("\n  Geometry sizes:\n   ");
  for(i=0; i<params->num_geometry_sizes; i++){
    printf(" %zu", params->geometry_sizes[i]);
  }
  printf("\n  Message sizes:\n   ");
  for(i=0; i<params->num_message_sizes; i++){
    printf(" %zu", params->message_sizes[i]);
  }
  printf("\n  Iterations       : %d\n", params->iter);
  printf("  Output file      : %s\n", output_file);
  printf("  Checkpoint mode  : %d\n", params->checkpoint);
  printf("  Diagnostics mode : %d\n", params->verify);
  printf("  Verbose mode     : %d\n", params->verbose);
}

int main(int argc, char ** argv)
{
  int ret = 0;
  char *output_file = NULL;
  pami_client_t client;
  pami_context_t context;
  pami_configuration_t config;
  pami_result_t status = PAMI_ERROR;

  advisor_params_t params;
  init_advisor_params(&params);

  status = PAMI_Client_create("PAMITUNE", &client, NULL, 0);
  if(status != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to initialize pami client. result = %d\n", status);
    return 1;
  }

  config.name = PAMI_CLIENT_TASK_ID;
  status = PAMI_Client_query(client, &config, 1);
  if (status != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to query configuration.name=(%d): result = %d\n",
             config.name, status);
    ret = 1;
    goto destroy_client;
  }
  task_id = config.value.intval;

  config.name = PAMI_CLIENT_NUM_TASKS;
  status = PAMI_Client_query(client, &config, 1);
  if (status != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to query configuration.name=(%d): result = %d\n",
             config.name, status);
    ret = 1;
    goto destroy_client;
  }
  num_tasks = config.value.intval;

  int result = process_arg(argc, argv, &params, &output_file);
  if(result)
  {
    print_usage();
    ret = 1;
    goto destroy_client;
  }
  /* If user did not set output filename use default */
  if(output_file == NULL && !task_id)
  {
    if(process_output_file("pami_tune_results.xml", &output_file))
    {
      ret = 1;
      goto destroy_client;
    }
  }
  if(params.verbose && !task_id)
    print_params(&params, output_file);

  status = PAMI_Context_createv(client, NULL, 0, &context, 1);
  if(status != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to create pami context. result = %d\n", status);
    ret = 1;
    goto destroy_client;
  }

  pami_extension_t extension;
  PAMI_Extension_open (client, "EXT_collsel", &extension);
  if(status != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to load the PAMI \"EXT_collsel\" extension. result = %d\n", status);
    ret = 1;
    goto destroy_context;
  }

  advisor_t advisor;
  advisor_configuration_t configuration[1];
  pami_extension_collsel_init pamix_collsel_init =
    (pami_extension_collsel_init) PAMI_Extension_symbol (extension, "Collsel_init_fn");
  pamix_collsel_init (client, configuration, 1, &context, 1, &advisor);

  pami_extension_collsel_table_generate pamix_collsel_table_generate =
    (pami_extension_collsel_table_generate) PAMI_Extension_symbol (extension, "Collsel_table_generate_fn");
  status = pamix_collsel_table_generate (advisor, output_file, &params, 1);

  pami_extension_collsel_destroy pamix_collsel_destroy =
    (pami_extension_collsel_destroy) PAMI_Extension_symbol (extension, "Collsel_destroy_fn");
  status = pamix_collsel_destroy (&advisor);


  status = PAMI_Extension_close (extension);
  if(status != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. The \"EXT_collsel\" extension could not be closed. result = %d\n", status);
    ret = 1;
    goto destroy_context;
  }

destroy_context:
  status = PAMI_Context_destroyv(&context, 1);
  if(status != PAMI_SUCCESS)
  {
    fprintf(stderr, "Error. Unable to destroy pami context. result = %d\n", status);
    return 1;
  }

destroy_client:
  status = PAMI_Client_destroy(&client);
  if(status != PAMI_SUCCESS)
  {
    fprintf(stderr, "Error. Unable to destroy pami client. result = %d\n", status);
    return 1;
  }

  free_advisor_params(&params);
  if(output_file)
    free(output_file);

  return ret;
}

