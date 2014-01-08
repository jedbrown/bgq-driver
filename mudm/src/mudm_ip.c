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


//! \file  mudm_ip.c
//! \brief Internet protocol methods


#include <spi/include/kernel/MU.h>
#include <spi/include/kernel/memory.h>

#include <mudm/include/mudm.h>
#include "mudm_macro.h"
#include <mudm/include/mudm_inlines.h>
#include <mudm/include/mudm_utils.h>



#include "common.h"
#include "mudm_pkt.h"
#include "rdma_object.h"
#include "common_inlines.h"
//need mudm_connection for SetUpDestination
#include "mudm_connection.h" 


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
#include <spi/include/mu/Addressing_inlines.h>

//! \todo TODO: Need to add options to deal with io links on xmit
int mudm_pkt_imm(void* mudm_context,
                      void* requestID,       
                      struct ionet_pkt_header * header,           
                      uint16_t sgl_num,  
                      uint16_t data_length,
                      struct   mudm_sgl_virt*  sgl)
{
    

    struct my_context * mcontext = (struct my_context *)mudm_context; 
    struct pkt_descriptor * pktd = NULL;  /* for packet handling */
    char * msg2send=NULL;
    uint16_t payload_length = 0;
    int i;
    int rc;

    if (mcontext->magic != MUDM_MAGIC){ 
      MPRINT("BAD MCONTEXT in FUNCTION %s \n",__FUNCTION__);
      return -EFAULT;
    }
 
    pktd = alloc_pkt_message(mcontext->packetcontrols[0]);
    if (pktd == NULL){
     PRINT("pktd==NULL %s line %d \n",__FUNCTION__,__LINE__);
     return -EBUSY;
    }
    
    msg2send=(char *)pktd->pkt_msg;
   

    for (i=0;i<sgl_num;i++){
	memcpy(msg2send,sgl[i].virtualAddr,sgl[i].memlength);
        payload_length += sgl[i].memlength;
        msg2send += sgl[i].memlength;/*advance pointer */
    }/* endwhile */    
    
    if (payload_length != data_length) {
      release_pkt_to_free(pktd, 0);
      MPRINT("EFAULT payload_length=%llu NOT EQUAL data_length=%llu \n",(LLUS)payload_length,(LLUS)data_length);
      return -EFAULT;
    }
    //rc =  mudm_pkt_imm_pa(mudm_context ,requestID,header,(void *)pktd->pkt_msg, data_length);
    return rc=-EINVAL;
    release_pkt_to_free(pktd, 0); /* release when done, transmission is done */
    /* \todo: requestID handling */
return 0;
}


