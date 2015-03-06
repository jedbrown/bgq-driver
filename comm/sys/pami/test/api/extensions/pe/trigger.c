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
 * \file test/api/extensions/pe/trigger.c
 * \brief Simple test of trigger in the PAMI PE extension
 */

#include <stdio.h>
#include <string.h>
#include <pami.h>
#include <pami_ext_pe.h>

#define DBG_FPRINTF(x) fprintf x 
#define RC(statement) \
{ \
    pami_result_t rc = statement; \
    if (rc != PAMI_SUCCESS) { \
        fprintf (stderr, #statement " rc = %d, line %d\n", \
                rc, __LINE__); \
        exit(1); \
    } \
}

volatile int trigger_bool;

/* a simple trigger function */
int my_trigger(pami_context_t pc, int argc, char *argv[])
{
  printf("Entered my_trigger with %d args: context at 0x%p\n", argc, pc);
  int i;
  for (i = 0; i < argc; i ++) {
      printf("\tArg #%d:\t%s\n", i+1, argv[i]);
  }
  trigger_bool = 0;

  return 0; /* successfully finished */
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
  const char            trigger_name[] = "pami_trigger";
  pami_trigger_t        trigger;
  pami_configuration_t  config;

  /* open PAMI extension */
  RC( PAMI_Extension_open (client, ext_name, &extension) );
  DBG_FPRINTF((stderr,"Open %s successfully.\n", ext_name));

  /* initialize trigger structure */
  trigger.trigger_name = (char*)trigger_name;
  trigger.trigger_func = my_trigger;

  /* add a trigger to PAMI context */
  /* this configuration is in PE extension */
  bzero(&config, sizeof(pami_configuration_t));
  config.name = PAMI_CONTEXT_TRIGGER;
  config.value.chararray = (char*)&trigger;
  RC( PAMI_Context_update(context, &config, 1) );
  DBG_FPRINTF((stderr,"PAMI trigger (%s) was added\n", trigger_name));

  /* wait for the trigger to be invoked 
   * trigger can be invoked using
   * "pnsd_trigger -j job_key trigger_name args" command */ 
  trigger_bool = 1;
  while (trigger_bool)
    PAMI_Context_advance(context, 1);

  /* remote this trigger from PAMI context 
   * only need to set trigger_func in trigger struct to NULL */
  trigger.trigger_func = NULL; 
  RC( PAMI_Context_update(context, &config, 1) );
  DBG_FPRINTF((stderr,"PAMI trigger (%s) was removed\n", trigger_name));

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
