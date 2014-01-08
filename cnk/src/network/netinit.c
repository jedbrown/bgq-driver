/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

// Includes
#include "Kernel.h"
#include "cnverbs.h"
#include <mudm/include/mudm.h>

extern int configureBlockGIBarrier();

struct mudm_cfg_info  mudmconfig;

int cnkmudm_allocate(struct mudm_memory_region* memregion, size_t length, void* callback_context)
{
   memregion->length      = length;
   memregion->base_vaddr  = cnk_malloc(length);
   memregion->base_paddr  = memregion->base_vaddr;
   
   if(memregion->base_vaddr == NULL)
      return ENOMEM;
   
   return 0;
}

int cnkmudm_free(struct mudm_memory_region* memregion, void* callback_context)
{
   cnk_free(memregion->base_vaddr);  // \note Current design of CNK does not reclaim storage
   return 0;
}

int network_init(void)
{
   int rc;

   configureBlockGIBarrier();
   
   // Generate an IPv4 address for the torus device.  The address is saved in the first interface section of the personality.
   // This has the side effect of overwriting an address that was put there by the control system but CNK does not 
   // support configuring an IP address through the database.
   Personality_t *pers = GetPersonality();
   memset(NodeState.torusIpAddress.octet, 0x00, sizeof(IP_Addr_t));
   struct TorusIPAddress generatedAddr;
   generatedAddr.octet0 = 10;
   generatedAddr.type = 2; // Special value for a compute node
   if ((pers->Network_Config.NetFlags2 & ND_CN_BRIDGE_PORT_6) != 0) {
      generatedAddr.port = 1;
   }
   else if ((pers->Network_Config.NetFlags2 & ND_CN_BRIDGE_PORT_7) !=0) {
      generatedAddr.port = 2;
   }
   else {
      generatedAddr.port = 0;
   }
   generatedAddr.aCoord = pers->Network_Config.Acoord;
   generatedAddr.bCoord = pers->Network_Config.Bcoord;
   generatedAddr.cCoord = pers->Network_Config.Ccoord;
   generatedAddr.dCoord = pers->Network_Config.Dcoord;
   generatedAddr.eCoord = pers->Network_Config.Ecoord;
   memcpy(NodeState.torusIpAddress.octet, &generatedAddr, sizeof(generatedAddr));
   TRACE( TRACE_Verbs, ("(I) network_init(): generated torus IPv4 address is %u.%u.%u.%u\n",
                        NodeState.torusIpAddress.octet[0], NodeState.torusIpAddress.octet[1],
                        NodeState.torusIpAddress.octet[2], NodeState.torusIpAddress.octet[3]) );
   // flightlog generated IPv4 address
#if 0
   printf("(I) network_init(): generated torus IPv4 address is %u.%u.%u.%u from %u %u %u %u %u\n",
          NodeState.torusIpAddress.octet[0], NodeState.torusIpAddress.octet[1],
          NodeState.torusIpAddress.octet[2], NodeState.torusIpAddress.octet[3],
          pers->Network_Config.Acoord, pers->Network_Config.Bcoord, pers->Network_Config.Ccoord, pers->Network_Config.Dcoord, pers->Network_Config.Ecoord);
#endif

   struct mudm_init_info mudminit;
   memset(&mudminit, 0, sizeof(mudminit));
   mudminit.callers_version = MUDM_VERSION;
   mudminit.req_inbound_connections = 5;
   mudminit.req_inj_fifos = 1;
   mudminit.req_rec_fifos = 1;
   mudminit.req_num_pollers = 1;
   mudminit.recv = cnverbs_recv;
   mudminit.recv_conn = cnverbs_recv_conn;
   mudminit.status = cnverbs_status;
   mudminit.allocate = cnkmudm_allocate;
   mudminit.free = cnkmudm_free;
   mudminit.personality = GetPersonality();
   mudminit.callback_context = NULL;
   mudminit.wakeupActive = CNK_ENABLE_MUDM_WAKEUP_POLL;
   NodeState.MUDM = NULL;
   
   rc = mudm_init(&mudminit, &mudmconfig, &NodeState.MUDM);
   if(rc != 0)
   {
      printf("(E) network_init(): mudm_init failed with rc=%d\n", rc);
      return rc;
   }
   addFlightRecorder(&mudmconfig.mudm_hi_wrap_flight_recorder);
   addFlightRecorder(&mudmconfig.mudm_lo_wrap_flight_recorder);
   
   rc = cnverbs_init();
   if(rc != 0)
   {
      printf("(E) network_init(): cnv_init failed with rc=%d\n", rc);
      return rc;
   }

   TRACE( TRACE_Verbs, ("(I) network_init(): networks are initialized\n") );
   return 0;
}

int network_poll(void)
{
   int rc = 0;
   // Receive data from the MU device when it is enabled.
   if(NodeState.Personality.Kernel_Config.NodeConfig & PERS_ENABLE_MU)
   {
      while (1)
      {
         mudm_start_poll(NodeState.MUDM, MUDM_POLLANDRETURN);
      }
   }
   // Park this thread if the MU device is not enabled.
   else
   {
      while(1);
   }

   return rc;
}

int network_term(void)
{
   // Shutdown the network.
   cnverbs_term();
   if(NodeState.MUDM)
       mudm_terminate(NodeState.MUDM);
   
   return 0;
}
