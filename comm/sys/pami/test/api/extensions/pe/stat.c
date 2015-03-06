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
 * \file test/api/extensions/pe/stat.c
 * \brief Simple test of context statistics in the PAMI PE extension
 */

#include <stdio.h>
#include <string.h>
#include <pami.h>
#include <pami_ext_pe.h>

#define DBG_FPRINTF(x) /*fprintf x */
#define RC(statement) \
{ \
    pami_result_t rc = statement; \
    if (rc != PAMI_SUCCESS) { \
        fprintf (stderr, #statement " rc = %d, line %d\n", \
                rc, __LINE__); \
        exit(1); \
    } \
}

int main(int argc, char ** argv)
{
  pami_client_t     client;
  pami_context_t    context;
  pami_result_t     status = PAMI_ERROR;
  
  /* create PAMI client */
  RC( PAMI_Client_create("TEST", &client, NULL, 0) );
  DBG_FPRINTF((stderr,"Client created successfully at 0x%p\n",client));

  /* create PAMI context */
  RC( PAMI_Context_createv(client, NULL, 0, &context, 1) );
  DBG_FPRINTF((stderr,"Context created successfully at 0x%p\n",context));

  /* ------------------------------------------------------------------------ */

  pami_extension_t      extension;
  const char            ext_name[] = "EXT_pe_extension";
  pami_configuration_t  stat_config;
  pami_statistics_t*    stat_p;
  pami_counter_t*       stat_counters;

  /* open PAMI extension */
  RC( PAMI_Extension_open (client, ext_name, &extension) );
  DBG_FPRINTF((stderr,"Open %s successfully.\n", ext_name));

  /* query for PAMI context statistics */
  /* this query is in PE extension */
  bzero(&stat_config, sizeof(pami_configuration_t));
  stat_config.name = PAMI_CONTEXT_STATISTICS;
  RC( PAMI_Context_query(context, &stat_config, 1) );

  /* display all the statistics counters */
  stat_p = (pami_statistics_t*)stat_config.value.chararray;
  stat_counters = stat_p->counters;
  int i;
  for (i = 0; i < stat_p->count; i ++) {
      printf("#%2d STAT\t%s:%lu\n", i+1, stat_counters[i].name,
             stat_counters[i].value);
  } 

  /* close PAMI extension */
  RC( PAMI_Extension_close (extension) );
  DBG_FPRINTF((stderr,"Close %s successfully.\n", ext_name));

  /* ------------------------------------------------------------------------ */
  /* destroy PAMI context */
  RC( PAMI_Context_destroyv(&context, 1) );
  DBG_FPRINTF((stderr, "PAMI context destroyed successfully\n"));

  /* destroy PAMI client */
  RC( PAMI_Client_destroy(&client) );
  DBG_FPRINTF((stderr, "PAMI client destroyed successfully\n"));

  return 0;
}
