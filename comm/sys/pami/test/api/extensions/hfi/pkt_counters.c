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
 * \file test/api/extensions/hfi/pkt_counters.c
 * \brief Simple test of HFI packet counters in the PAMI HFI extension
 */

#include <stdio.h>
#include <pami.h>
#include <pami_ext_hfi.h>

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
  const char            ext_name[] = "EXT_hfi_extension";
  const char            sym_name[] = "hfi_pkt_counters";
  hfi_pkt_counters_fn   hfi_counters = NULL;
  hfi_pkt_counter_t     pkt_counter;

  /* open PAMI extension */
  RC( PAMI_Extension_open (client, ext_name, &extension) );
  DBG_FPRINTF((stderr,"Open %s successfully.\n", ext_name));

  /* load PAMI extension function */
  hfi_counters = (hfi_pkt_counters_fn) 
      PAMI_Extension_symbol (extension, sym_name);
  if (hfi_counters == (void *)NULL)
  {
    fprintf (stderr, "Error. Failed to load %s function in %s\n",
             sym_name, ext_name); 
    return 1;
  } 
  DBG_FPRINTF((stderr,"Loaded function %s in %s successfully.\n", 
              sym_name, ext_name));

  /* invoke PAMI extension function */
  RC( hfi_counters(context, &pkt_counter) );
  DBG_FPRINTF((stderr,"Function %s invoked successfully.\n", 
              sym_name));
  printf( "Pkt sent =         %lu\n"
          "Pkt sent dropped = %lu\n"
          "Ind pkt sent =     %lu\n"
          "Pkt recv =         %lu\n"
          "Pkt recv dropped = %lu\n"
          "Ind pkt recv =     %lu\n"
          "Imm pkt sent =     %lu\n",
          pkt_counter.total_packets_sent,
          pkt_counter.packets_send_drop,
          pkt_counter.indicate_packet_sent,
          pkt_counter.total_packets_recv,
          pkt_counter.packets_recv_drop,
          pkt_counter.indicate_packet_recv,
          pkt_counter.immediate_packet_sent);

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
