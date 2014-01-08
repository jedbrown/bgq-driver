/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (c) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/*                                                                  */
/* US Government Users Restricted Rights - Use, duplication or      */
/* disclosure restricted by GSA ADP Schedule Contract with IBM      */
/* Corporation.                                                     */
/*                                                                  */
/* This software is available to you under either the GNU General   */
/* Public License (GPL) version 2 or the Eclipse Public License     */
/* (EPL) at your discretion.                                        */
/*                                                                  */

#include <spi/include/kernel/MU.h>
#include <spi/include/kernel/memory.h>

#include <mudm/include/mudm.h>
#include <mudm/include/mudm_inlines.h>
#include "mudm_macro.h"

#ifdef __FWEXT__
#include <firmware/include/fwext/fwext.h>
#include <firmware/include/Firmware.h>
#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <firmware/include/fwext/fwext_lib.h>

#include <firmware/include/fwext/fwext_nd.h>
#include <firmware/include/fwext/fwext_mu.h>

#endif

#include <hwi/include/bqc/nd_500_dcr.h>
#include <firmware/include/personality.h>


#include "common.h"

#include <mudm/include/mudm_utils.h>

#define SEC_IN_MICROSECONDS  1000000 
#define MIN_IN_MICROSECONDS 60000000

uint64_t microsec2cycles(void * personality,uint64_t microseconds){
Personality_t * p = (Personality_t *)personality;
return microseconds * p->Kernel_Config.FreqMHz;
}

uint64_t getUCI(void * personality){
 Personality_t * p = (Personality_t *)personality;
 //PRINT("MUDM: node UCI p->Kernel_Config.UCI = %llx \n",(LLUS)p->Kernel_Config.UCI);
 return p->Kernel_Config.UCI;
}

uint8_t getCNportOnIOnode(void * personality){
Personality_t * p = (Personality_t *)personality;
//uint8_t io_port = 0xFF;  //unknown
uint8_t io_port= MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_IO_PORT;//assumed default if not in personality
ENTER;
if (NULL==p)return io_port;
if (p->Network_Config.NetFlags2 & ND_CN_BRIDGE_PORT_6){
   io_port= MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_TORUS_PORT_6; 
   PRINT("io_port is MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_TORUS_PORT_6 \n");  
}
if (p->Network_Config.NetFlags2 & ND_CN_BRIDGE_PORT_7){
   io_port= MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_TORUS_PORT_7;
   PRINT("io_port is MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_TORUS_PORT_7 \n");      
}
if (p->Network_Config.NetFlags2 & ND_CN_BRIDGE_PORT_10){
   io_port= MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_IO_PORT;
   PRINT("io_port is MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_IO_PORT \n"); 
}
EXIT;
return io_port;
}



uint32_t get_my_torus_addr(void * personality){
   
    Personality_t * p = (Personality_t *)personality;
    uint32_t temp = p->Network_Config.Acoord;
    temp = (temp <<6) | p->Network_Config.Bcoord;
    temp = (temp <<6) | p->Network_Config.Ccoord;
    temp = (temp <<6) | p->Network_Config.Dcoord;
    temp = (temp <<6) | p->Network_Config.Ecoord;
    if (p->Kernel_Config.NodeConfig & PERS_ENABLE_IsIoNode){
      temp |= 0x80000000;
    }
    PRINT("torus-address=%08lx \n",(long unsigned int)temp);
    PRINT("from personality %d %d %d %d %d \n", p->Network_Config.Acoord, p->Network_Config.Bcoord, p->Network_Config.Ccoord, p->Network_Config.Dcoord, p->Network_Config.Ecoord);
    return temp;
}




uint32_t get_my_bridging_torus_addr(void * personality){  
    Personality_t * p = (Personality_t *)personality;
    uint32_t temp = p->Network_Config.cnBridge_A;
    temp = (temp <<6) | p->Network_Config.cnBridge_B;
    temp = (temp <<6) | p->Network_Config.cnBridge_C;
    temp = (temp <<6) | p->Network_Config.cnBridge_D;
    temp = (temp <<6) | p->Network_Config.cnBridge_E;
    return  temp;
}

uint32_t get_BlockID(void * personality){  
    Personality_t * p = (Personality_t *)personality;
    return p->Network_Config.BlockID;
}
