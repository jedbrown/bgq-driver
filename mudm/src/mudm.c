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

//! \file  mudm.c 
//! \brief Implementation of special programming interfaces for data transfers.
//! \remarks primarily intended for OFED or CN verbs implementers over torus

#include <spi/include/kernel/MU.h>
#include <spi/include/kernel/memory.h>

#include <mudm/include/mudm.h>
#include <mudm/include/mudm_bcast.h>
#include <mudm/include/mudm_inlines.h>
#include "mudm_macro.h"
#include <mudm/include/mudm_inlines.h>
#include <mudm/include/mudm_utils.h>



#include "common.h"
#include "mudm_pkt.h"
#include "rdma_object.h"

#include "common_inlines.h"
#include "mudm_connection.h"
#include "mudm_wu.h"

#include <hwi/include/bqc/nd_500_dcr.h>
#include <firmware/include/personality.h>
#include <spi/include/mu/Addressing_inlines.h>

#define RPRINT PRINT

int mudm_rdma_write(void* conn_context,
                    void* remoteRequestID,
                    uint64_t rdma_object,  
                    uint64_t bytes_xfer, 
                    struct mudm_sgl*  local_sgl, 
                    struct mudm_sgl* remote_sgl)
{  
  uint64_t           desc_count = -1; 
  struct mudm_connection * ccontext = (struct mudm_connection *)conn_context;
  struct my_context * mcontext = (struct my_context *)ccontext->maincontext;
  uint32_t totalbytes=0;
  
  uint64_t cur_local_pa  = 0;
  uint64_t cur_remote_pa = 0;
  uint64_t cur_local_memlength  = 0;
  uint64_t cur_remote_memlength  = 0;
  if (local_sgl == NULL) return -EINVAL;
  if (remote_sgl==NULL)  return -EINVAL;
  if (bytes_xfer==0) return -EINVAL;

  CHECK_CONN_MAGIC(ccontext);

  MUDM_LOG_RDMA_WRITE(ccontext->flight_recorder,remoteRequestID,ccontext, (void*)rdma_object, (LLUS)bytes_xfer);
  if ( unlikely(0==ccontext->injfifo_ctls->state) ){
      MUDM_REQUEST_CANCELED(ccontext -> flight_recorder,ECANCELED,0,conn_context,(uint64_t)remoteRequestID);
      MUDM_ERROR_HIT(ccontext -> flight_recorder,ECANCELED,__LINE__,0,(uint64_t)remoteRequestID);
      return -ECANCELED;
  }

  totalbytes=0;

  cur_local_pa  = local_sgl->physicalAddr;
  cur_remote_pa = remote_sgl->physicalAddr;
  cur_local_memlength  = local_sgl->memlength  ;
  cur_remote_memlength  = remote_sgl->memlength  ;


while( (cur_local_pa !=0 ) && (cur_remote_pa != 0)  )
  {
      //MPRINT(" local_sgl->physicalAddr=%llx local_sgl->memlength=%llx \n", (LLUS)local_sgl->physicalAddr,(LLUS)local_sgl->memlength);
      //MPRINT(" remote_sgl->physicalAddr=%llx remote_sgl->memlength=%llx\n", (LLUS)remote_sgl->physicalAddr,(LLUS)remote_sgl->memlength);
      
      //MPRINT(" cur_local_pa=%llx ,cur_local_memlength=%llx  \n", (LLUS)cur_remote_pa,(LLUS)cur_local_memlength);
      //MPRINT(" cur_remote_pa=%llx ,cur_remote_memlength=%llx  \n", (LLUS)cur_remote_pa,(LLUS)cur_remote_memlength);

      if (cur_local_memlength ==cur_remote_memlength  ){  
         totalbytes += cur_local_memlength  ;
         if (totalbytes > bytes_xfer) goto BADRDMA;   
 
         desc_count = InjFifoInjectDirectPut (ccontext->injfifo_ctls, &ccontext->mu_iDirectPutDescriptor, 
                                              cur_remote_memlength, cur_local_pa,  cur_remote_pa,
                                              rdma_object, 
                                              ccontext->flight_recorder);
         if (totalbytes >= bytes_xfer) break; 
         local_sgl++;
         remote_sgl++;  
        
         cur_local_pa  = local_sgl->physicalAddr;
         cur_local_memlength  = local_sgl->memlength  ;
                    
         cur_remote_pa  = remote_sgl->physicalAddr;
         cur_remote_memlength  = remote_sgl->memlength  ;       
      }
      else if (cur_local_memlength   < cur_remote_memlength  ){   
         totalbytes += cur_local_memlength  ; 
         if (totalbytes > bytes_xfer) goto BADRDMA;     
         desc_count = InjFifoInjectDirectPut (ccontext->injfifo_ctls, &ccontext->mu_iDirectPutDescriptor, 
                                              cur_local_memlength, cur_local_pa,  cur_remote_pa,
                                              rdma_object,  
                                              ccontext->flight_recorder); 
         if (totalbytes >= bytes_xfer) break; 
         cur_remote_pa += cur_local_memlength  ;
         cur_remote_memlength   -= cur_local_memlength  ;
         local_sgl++; 

         cur_local_pa  = local_sgl->physicalAddr;
         cur_local_memlength  = local_sgl->memlength  ;                  
      }
      else { /* (cur_local_memlength   > cur_remote_memlength  ) */  
         totalbytes += cur_remote_memlength;
         if (totalbytes > bytes_xfer) goto BADRDMA;   
  
         desc_count = InjFifoInjectDirectPut (ccontext->injfifo_ctls, &ccontext->mu_iDirectPutDescriptor, 
                                              cur_remote_memlength, cur_local_pa,  cur_remote_pa,
                                              rdma_object,  
                                              ccontext->flight_recorder); 
         if (totalbytes >= bytes_xfer) break; 
         cur_local_pa += cur_remote_memlength  ;
         cur_local_memlength   -= cur_remote_memlength  ;
         remote_sgl++;
                   
         cur_remote_pa  = remote_sgl->physicalAddr;
         cur_remote_memlength  = remote_sgl->memlength  ;
         
         
      }
      if ( unlikely(0==ccontext->injfifo_ctls->state) ){
          MUDM_REQUEST_CANCELED(ccontext -> flight_recorder,ECANCELED,0,conn_context,(uint64_t)remoteRequestID);
          MUDM_ERROR_HIT(ccontext -> flight_recorder,ECANCELED,__LINE__,0,(uint64_t)remoteRequestID);
          return -ECANCELED;
      }

  }/* endwhile */
  if ( likely(totalbytes == bytes_xfer) ){
    EXIT;
    return 0;
  }
  else {
BADRDMA:
    MPRINT("EINVAL--mismatch on RDMA transfer sizes for source to dest %s:%d \n", __FUNCTION__, __LINE__); 
    desc_count = InjFifoInjectDirectPut (ccontext->injfifo_ctls, &ccontext->mu_iDirectPutDescriptor, 
                                              sizeof(void *), 
                                              mcontext->mudm_context_phys_addr + offsetof(struct my_context,rdma_EINVAL_source), 
                                              rdma_object+offsetof(struct mu_element,error ),
                                              rdma_object,  
                                              ccontext->flight_recorder);
    MUDM_ERROR_HIT(ccontext -> flight_recorder,EINVAL,__LINE__,totalbytes,bytes_xfer); 
    return -EINVAL;
    
  }
}/*mudm_rdma_write */
EXPORT_SYMBOL(mudm_rdma_write);


int mudm_send_pkt(void* conn_context ,
                     void* requestID, 
                     uint16_t type,
                     void * payload_paddr, /* physical address of payload */
                     uint16_t payload_length)
{
    
    uint64_t           desc_count = -1; 
    
    struct mudm_connection * ccontext = (struct mudm_connection *)conn_context; 
    struct pkt_descriptor * spo = NULL;
ENTER; 
    if ( unlikely(payload_length > TORUS_MAX_PAYLOAD_SIZE ) )return -EINVAL; /*exceeded memfifo packet size */ 
    CHECK_CONN_MAGIC(ccontext);
    desc_count = conn_memfifo(ccontext,type,(uint64_t)payload_paddr,(uint64_t)payload_length);

    do{
      if ( MUSPI_CheckDescComplete(ccontext->injfifo_ctls->injfifo, desc_count) == 1){
         return 0;
      }
      if ( unlikely(0==ccontext->injfifo_ctls->state) ){//link inactive?
        MUDM_REQUEST_CANCELED(ccontext -> flight_recorder,ECANCELED,type,conn_context,requestID);
        return -ECANCELED;
      }
      if ( likely(requestID != NULL) )spo = alloc_pkt_message(ccontext->smallpa_obj_ctls);
      if ( likely(spo != NULL) ){
        set_pkt_req_id(spo, (uint64_t)requestID);
        PRINT("<<Packet requestID=%p \n",  requestID);
        release_pkt_to_poll_ccontext(spo, desc_count,ccontext);
        return -EINPROGRESS;
      }    
    }while(1);

    return 0;
}
EXPORT_SYMBOL(mudm_send_pkt);


int mudm_send_packet(void* conn_context ,
                     void* requestID, 
                     uint16_t type, 
                     char * payload,
                     void * payload_paddr, /* physical address of payload */
                     uint16_t payload_length){
if (payload_paddr != NULL) 
  return mudm_send_pkt(conn_context , requestID, type, payload_paddr, payload_length);
else
  return -EINVAL;
}
EXPORT_SYMBOL(mudm_send_packet);


int mudm_send_message_remote(void* conn_context,
                        uint16_t type, 
                        struct ionet_send_remote_payload * payload)
{
    
    uint64_t           desc_count = -1; 
    struct pkt_descriptor * pktd = NULL;  /* for packet handling */
    uint64_t physaddr;
    char * msg2send;
    uint64_t payload_length;
    struct ionet_send_remote_payload * mudm_payload;
    struct mudm_rdma_object * rdma_obj_item;
    struct mudm_connection * ccontext = (struct mudm_connection *)conn_context; 
    //struct my_context * mcontext = (struct my_context *)ccontext->maincontext;

ENTER;
    PRINT("payload->sgl_num=%llu \n",(LLUS)payload->sgl_num);
    if (unlikely(payload->sgl_num == 0) ) return -EFAULT;
    if ( unlikely(payload->sgl_num > MUDM_MAX_REMOTE_SGE) ){
       MUDM_ERROR_HIT(ccontext -> flight_recorder,E2BIG,__LINE__,payload->sgl_num,MUDM_MAX_REMOTE_SGE);
       return -E2BIG;
    }
    if ( unlikely(0==ccontext->injfifo_ctls->state) ){
        MUDM_REQUEST_CANCELED(ccontext -> flight_recorder,ECANCELED,type,conn_context,payload->RequestID);
        return -ECANCELED;
    }
    pktd = alloc_pkt_message(ccontext->packetcontrols);
    if ( unlikely(pktd == NULL) ){
     PRINT("pktd==NULL %s line %d \n",__FUNCTION__,__LINE__);
     return -EBUSY;
    }

    physaddr = pktd->message_pa;  
    DPRINT("physaddr=%llx \n", (LLUS)physaddr);
    msg2send = (char *)pktd->pkt_msg;

    payload_length = sizeof(struct ionet_send_remote_payload)
                     - (MUDM_MAX_REMOTE_SGE - payload->sgl_num) * sizeof(struct mudm_sgl);
    
    mudm_payload = (struct ionet_send_remote_payload *)msg2send;


    memcpy(mudm_payload,payload,payload_length);
    PRINT("type=%x \n",type);
    switch(type) /* outbound request */
    {
      
      case MUDM_RDMA_WRITE:
      {
        
        rdma_obj_item = allocate_rdma_object(ccontext->rdma_obj_ctls);
        if (NULL==rdma_obj_item){
          release_pkt_to_free(pktd, 0);
          MPRINT("rdma_obj_item is NULL \n");
          MHERE;
          return -EBUSY;
        }
        mudm_payload->rdma_object = rdma_obj_item->rdma_object_pa;
        rdma_obj_item->mue->mu_counter = (uint64_t)(mudm_payload->data_length ); 
        rdma_obj_item->mue->error = 0;
        rdma_obj_item->local_request_id = (uint64_t)mudm_payload->RequestID;
        rdma_obj_item->remote_rdma_obj_pa=0; /* Unknown here */
        rdma_obj_item->local_connect=conn_context;  

        release_rdma_obj_to_poll(rdma_obj_item, COUNTER);/* only care about counter on origination */
        break;
      }

      case MUDM_RDMA_READ:
      case MUDM_PKT_DATA_REMOTE: /* different from RDMA_READ in that rkey is ignored ..... */
      case MUDM_RDMA_READ_IMM:
      {

        rdma_obj_item = allocate_rdma_object(ccontext->rdma_obj_ctls);
        if (NULL==rdma_obj_item){
          release_pkt_to_free(pktd, 0);
          MPRINT("rdma_obj_item is NULL \n");
          MHERE;
          return -EBUSY;
        }
        mudm_payload->rdma_object = rdma_obj_item->rdma_object_pa;
        /* add on size of pointer for filling in of local request ID from remote side and then doing RDMA write */
        rdma_obj_item->mue->mu_counter = sizeof(void *); /* write 8 bytes to here to complete request */
        rdma_obj_item->mue->error = 0;
        
        rdma_obj_item->local_request_id = (uint64_t)mudm_payload->RequestID;
        rdma_obj_item->remote_rdma_obj_pa=0; /* Unknown here  */
        rdma_obj_item->local_connect=conn_context;  

        release_rdma_obj_to_poll(rdma_obj_item, COUNTER);/* only care about counter on origination */
        break;
      }
      default:
      {

        release_pkt_to_free(pktd, 0);
        MUDM_REQUEST_CANCELED(ccontext -> flight_recorder,EINVAL,type,conn_context,mudm_payload->RequestID);
        MUDM_ERROR_HIT(ccontext -> flight_recorder,EINVAL,__LINE__,type,mudm_payload->RequestID);
        return -EINVAL;
      }
    }

    DPRINT("Past case statements in FILE %s FUNCTION %s LINE %d DATE %s\n",__FILE__,__FUNCTION__,__LINE__, __DATE__);   

    desc_count = conn_memfifo(ccontext,type,physaddr,(uint64_t)payload_length);

    if ( unlikely(0==ccontext->injfifo_ctls->state) ){//link state is inactive
        MUDM_REQUEST_CANCELED(ccontext -> flight_recorder,ECANCELED,type,conn_context,mudm_payload->RequestID);
        MUDM_ERROR_HIT(ccontext -> flight_recorder,ECANCELED,__LINE__,type,mudm_payload->RequestID);
        release_pkt_to_free(pktd, 0);
        //cancel RDMA object already in poll
        rdma_obj_item->local_request_id = 0;
        my_ppc_msync();//just to be overly cautious
        rdma_obj_item->mue->mu_counter = 0;
        my_ppc_msync();//just to be overly cautious
        return -ECANCELED;
    }
    
    PRINT("desc_count=%llu @ line=%d \n",(long long unsigned int)desc_count,__LINE__);

    
    if ( MUSPI_CheckDescComplete(ccontext->injfifo_ctls->injfifo, desc_count) == 1){
      release_pkt_to_free(pktd, desc_count);
    }
    else {
      release_pkt_to_poll_ccontext(pktd, desc_count,ccontext);
    }
    EXIT;
    return -EINPROGRESS;
}
EXPORT_SYMBOL(mudm_send_message_remote);


int mudm_rdma_read(void* conn_context ,
                                   void* requestID, 
                                   void* remote_requestID, 
                                   uint64_t rdma_object, /* from header with type=RDMA_READ */
                                   uint64_t bytes_xfer,                                  
                                   struct mudm_sgl*  local_sgl, 
                                   struct mudm_sgl* remote_sgl){

  
  MUHWI_Descriptor_t *muspi_iDirectPutDescriptor=NULL;
  uint32_t  putdesc_count=0;
  uint32_t totalbytes=0;
  uint64_t desc_count = (uint64_t)-1;
  struct mudm_rdma_object * rdma_obj_item=NULL;
  struct pkt_descriptor * pktd = NULL;  /* for directput handling */

  struct mudm_connection * ccontext = (struct mudm_connection *)conn_context;
  //struct my_context * mcontext = (struct my_context *)ccontext->maincontext;

  uint64_t cur_local_pa  = 0;
  uint64_t cur_remote_pa = 0;
  uint64_t cur_local_memlength  = 0;
  uint64_t cur_remote_memlength  = 0;
  
  CHECK_CONN_MAGIC(ccontext);
  
  DPRINT("FUNCTION %s local_sgl=%p remote_sgl=%p bytes_xfer=%llu \n",__FUNCTION__,local_sgl,remote_sgl,(LLUS)bytes_xfer);
  
  if ( unlikely(0==ccontext->injfifo_ctls->state) ){
        MUDM_REQUEST_CANCELED(ccontext -> flight_recorder,ECANCELED,0,conn_context,requestID);
        MUDM_ERROR_HIT(ccontext -> flight_recorder,ECANCELED,__LINE__,0,requestID);
        return -ECANCELED;
  }
  pktd = alloc_pkt_message(ccontext->packetcontrols);
  
  if ( unlikely (pktd == NULL) ){
     PRINT("pktd==NULL %s line %d \n",__FUNCTION__,__LINE__);
     EXIT;
     return -EBUSY;
  }
  rdma_obj_item = allocate_rdma_object(ccontext->rdma_obj_ctls);
  if (NULL==rdma_obj_item){
      release_pkt_to_free(pktd,0);
      PRINT("rdma_obj_item is NULL \n");
      EXIT;
      return -EBUSY;
  }
  MUDM_LOG_RDMA_READ(ccontext->flight_recorder,remote_requestID,requestID, (void*)rdma_object, (LLUS)bytes_xfer);

  rdma_obj_item -> mue->mu_counter = bytes_xfer;
  rdma_obj_item -> mue->error = 0;
  rdma_obj_item -> local_request_id = (uint64_t)requestID; //NOTE: need local request ID for when RDMA completes for long send message (receive request
  rdma_obj_item->remote_rdma_obj_pa=rdma_object;
  rdma_obj_item -> local_connect = conn_context; /* needed for notifying remote end of connection */
  


  cur_local_pa  = local_sgl->physicalAddr;
  cur_remote_pa = remote_sgl->physicalAddr;
  cur_local_memlength  = local_sgl->memlength  ;
  cur_remote_memlength  = remote_sgl->memlength  ;

while( (cur_local_pa !=0 ) && (cur_remote_pa != 0)  )
  {   
      if (putdesc_count == 8){
          // use memfifo to emulate remote get operation
          desc_count = dput_memfifo(ccontext,MUDM_DPUT,pktd->message_pa,(uint64_t)(sizeof(MUHWI_Descriptor_t) )*putdesc_count,
                                    &ccontext->rget_memfifo_sent );
          if ( unlikely(0==ccontext->injfifo_ctls->state) ){//IO link is inactive
              MUDM_REQUEST_CANCELED(ccontext -> flight_recorder,ECANCELED,0,conn_context,requestID);
              MUDM_ERROR_HIT(ccontext -> flight_recorder,ECANCELED,__LINE__,0,requestID);
              release_pkt_to_free(pktd,0);
              free_rdma_obj(rdma_obj_item);
              return -ECANCELED;
          }
          while( MUSPI_CheckDescComplete(ccontext->injfifo_ctls->injfifo, desc_count) == 0);  //plan to reuse pktd memory region
          putdesc_count = 0;  // restart on sending bundle
      }

      muspi_iDirectPutDescriptor = (MUHWI_Descriptor_t * )pktd->pkt_msg;
      muspi_iDirectPutDescriptor += putdesc_count;  //use correct space in packet area
      memcpy(muspi_iDirectPutDescriptor,&ccontext->mu_iDirectPutDescriptor4rget,sizeof(MUHWI_Descriptor_t) );
     
      set_counter_directput(muspi_iDirectPutDescriptor,rdma_obj_item->rdma_object_pa);

      if (cur_local_memlength  == cur_remote_memlength  ){  
         totalbytes += cur_local_memlength  ;   
         if (totalbytes > bytes_xfer){ 
            cur_local_memlength = bytes_xfer - (totalbytes - cur_local_memlength) ;
            //MHERE;
         }  
         set_dma_directput(muspi_iDirectPutDescriptor, cur_local_memlength  ,
                                  cur_remote_pa, cur_local_pa );/*uint64_t  source_physaddr, uint64_t  dest_physaddr */
         putdesc_count++;
         if (totalbytes >= bytes_xfer) break; //leave while loop
         local_sgl++;
         remote_sgl++;  

         cur_local_pa  = local_sgl->physicalAddr;
         cur_local_memlength  = local_sgl->memlength  ;
            
         cur_remote_pa  = remote_sgl->physicalAddr;
         cur_remote_memlength  = remote_sgl->memlength  ;         
      }
      else if (cur_local_memlength   < cur_remote_memlength  ){   
         totalbytes += cur_local_memlength  ; 
         if (totalbytes > bytes_xfer){ 
           cur_local_memlength = bytes_xfer - (totalbytes - cur_local_memlength) ;
           //MHERE;
         }   
         set_dma_directput(muspi_iDirectPutDescriptor, cur_local_memlength  ,
                                   cur_remote_pa, cur_local_pa );/*uint64_t  source_physaddr, uint64_t  dest_physaddr */
         putdesc_count++;
         if (totalbytes >= bytes_xfer) break;
         cur_remote_pa += cur_local_memlength  ;
         cur_remote_memlength   -= cur_local_memlength  ;
         local_sgl++; 

         cur_local_pa  = local_sgl->physicalAddr;
         cur_local_memlength  = local_sgl->memlength  ;         
      }
      else { /* (cur_local_memlength   > cur_remote_memlength  ) */ 
         totalbytes += cur_remote_memlength  ; 
         if (totalbytes > bytes_xfer){ 
           cur_remote_memlength  = bytes_xfer - (totalbytes - cur_remote_memlength);
           //MHERE;
         } 
         set_dma_directput(muspi_iDirectPutDescriptor, cur_remote_memlength  ,
                                   cur_remote_pa, cur_local_pa );/*uint64_t  source_physaddr, uint64_t  dest_physaddr */
         putdesc_count++;
         if (totalbytes >= bytes_xfer) break;
         cur_local_pa += cur_remote_memlength  ;
         cur_local_memlength   -= cur_remote_memlength  ;
         remote_sgl++;
           
         cur_remote_pa  = remote_sgl->physicalAddr;
         cur_remote_memlength  = remote_sgl->memlength  ;         
      }        
  }/* endwhile */

  if  ( likely(totalbytes == bytes_xfer) ){
     if ( likely(putdesc_count > 0) ){
       // use memfifo to emulate remote get operation
       desc_count = dput_memfifo(ccontext,MUDM_DPUT,pktd->message_pa,(uint64_t)(sizeof(MUHWI_Descriptor_t) * putdesc_count ),&ccontext->rget_memfifo_sent ); 
       if ( unlikely(0==ccontext->injfifo_ctls->state) ){
              MUDM_REQUEST_CANCELED(ccontext -> flight_recorder,ECANCELED,0,conn_context,requestID);
              MUDM_ERROR_HIT(ccontext -> flight_recorder,ECANCELED,__LINE__,0,requestID);
              release_pkt_to_free(pktd,0);
              free_rdma_obj(rdma_obj_item);
              return -ECANCELED;
       }
     }
     
     // 12/15/2010 counter rarely hits zero here--archived that check in archive_inline.h
     release_rdma_obj_to_poll(rdma_obj_item, COUNTER);
     //release of rdma_obj long enough to have packet taken by MU 
     if ( MUSPI_CheckDescComplete(ccontext->injfifo_ctls->injfifo, desc_count) == 1){
            RPRINT("mudm_rdma_read free pktd=%p \n",pktd);
            release_pkt_to_free(pktd, desc_count);
     }
     else{
            RPRINT("mudm_rdma_read release to poll pktd=%p \n",pktd);
            release_pkt_to_poll_ccontext(pktd, desc_count,ccontext);
     }
     return -EINPROGRESS;  
  }
  else if (desc_count==-1){//no directputs were sent to the remote to execute
     MPRINT("mudm_rdma_read total != xfer but no remote directputs done totalbytes=%llu bytes_xfer=%llu\n",(LLUS)totalbytes,(LLUS)bytes_xfer);
      
     if (rdma_obj_item ->remote_rdma_obj_pa){ //tell other side about the error 
         struct my_context * mcontext = (struct my_context *)ccontext->maincontext;
         desc_count = direct_put(ccontext ,
                               mcontext->mudm_context_phys_addr + offsetof(struct my_context,rdma_EINVAL_source), 
                               rdma_obj_item ->remote_rdma_obj_pa + offsetof(struct mu_element,error ),
                               sizeof(mcontext->rdma_EINVAL_source),
                               rdma_obj_item ->remote_rdma_obj_pa );
     }  
     release_pkt_to_free(pktd, 0);
     free_rdma_obj(rdma_obj_item);
     MUDM_REQUEST_CANCELED(ccontext -> flight_recorder,EINVAL,0,conn_context,requestID);
     MUDM_ERROR_HIT(ccontext -> flight_recorder,EINVAL,__LINE__,0,requestID);
     MEXIT;
     return -EINVAL;
  }
  else if  (totalbytes > bytes_xfer ) { 
     MPRINT("mudm_rdma_read totalbytes > bytes_xfer totalbytes=%llu bytes_xfer=%llu\n",(LLUS)totalbytes,(LLUS)bytes_xfer);
     
     if (putdesc_count > 0){
       // use memfifo to emulate remote get operation
       MHERE;
       desc_count = dput_memfifo(ccontext,MUDM_DPUT,pktd->message_pa,(uint64_t)(sizeof(MUHWI_Descriptor_t) * putdesc_count ),&ccontext->rget_memfifo_sent ); 
     }
     //! \todo re-evaluate while looping--in io-link reset, ensure counter is zeroed
     while(rdma_obj_item ->mue-> mu_counter){//wait on counter to hit zero unless link goes inactive
       if ( unlikely(0==ccontext->injfifo_ctls->state) ){//IO link inactive?
              MUDM_REQUEST_CANCELED(ccontext -> flight_recorder,ECANCELED,0,conn_context,requestID);
              MUDM_ERROR_HIT(ccontext -> flight_recorder,ECANCELED,__LINE__,0,requestID);
              release_pkt_to_free(pktd,0);
              free_rdma_obj(rdma_obj_item);
              return -ECANCELED;
       }
     } 
     release_pkt_to_free(pktd, desc_count);
     
     if (rdma_obj_item ->remote_rdma_obj_pa){ //tell other side about the error 
         struct my_context * mcontext = (struct my_context *)ccontext->maincontext;
         desc_count = direct_put(ccontext ,
                               mcontext->mudm_context_phys_addr + offsetof(struct my_context,rdma_EINVAL_source), 
                               rdma_obj_item ->remote_rdma_obj_pa + offsetof(struct mu_element,error ),
                               sizeof(mcontext->rdma_EINVAL_source),
                               rdma_obj_item ->remote_rdma_obj_pa );

     }
     free_rdma_obj(rdma_obj_item);
     MEXIT;
     MUDM_ERROR_HIT(ccontext -> flight_recorder,EINVAL,__LINE__,0,requestID);
     return -EINVAL;
  }
  else { // (totalbytes < bytes_xfer 
     uint64_t nonzero_counter = bytes_xfer - totalbytes;
     MPRINT("mudm_rdma_read totalbytes < bytes_xfer totalbytes=%llu bytes_xfer=%llu\n",(LLUS)totalbytes,(LLUS)bytes_xfer);
     //! \todo (TODO)  free the RDMA for now at the end but find a better solution
         
     rdma_obj_item -> local_request_id = 0;
     //! \todo consider sending a remote direct put that sets the error after any issued remote directputs complete
     //! but no need to send a remote direct put if no remote directputs were issued....
     //! \todo TODO THIS NEEDS TO CHANGE SINCE the bits set the counter to refresh in cache only if zeroed!!!!!!!!!!
     //! \todo maybe orphan the rdam_obj_item?  
     while(rdma_obj_item->mue ->mu_counter != nonzero_counter); /* hold here until the pending RDMA operations complete */
     if (rdma_obj_item->remote_rdma_obj_pa){ //tell other side about the error 
       struct my_context * mcontext = (struct my_context *)ccontext->maincontext;
       desc_count = direct_put(ccontext ,
                               mcontext->mudm_context_phys_addr + offsetof(struct my_context,rdma_EINVAL_source), 
                               rdma_obj_item ->remote_rdma_obj_pa + offsetof(struct mu_element,error ),
                               sizeof(mcontext->rdma_EINVAL_source),
                               rdma_obj_item ->remote_rdma_obj_pa );
       /* no waiting for RDMA write notice to be completed */
       release_pkt_to_free(pktd, desc_count);
       free_rdma_obj(rdma_obj_item);  
     }
     MEXIT;
     MUDM_ERROR_HIT(ccontext -> flight_recorder,EINVAL,__LINE__,0,requestID);
     return -EINVAL;
  }
  
  return 0;
} /* end mudm_rdma_read */
EXPORT_SYMBOL(mudm_rdma_read);


static uint64_t memfifoRecvCount=0;

uint64_t InjFifoInject_rget_emulation (MUDM_InjFifo_t   * injfifo_ctls, 
					 void              * desc, 
                                         BG_FlightRecorderRegistry_t *logregistry) 
{   
    MUHWI_Descriptor_t  * descPtr;
    void * descPtrTemp=NULL;
    uint64_t desc_count;

    lock(&injfifo_ctls->inj_fifo_lock);
    MUDM_GET_NEXT_DESC(desc_count,injfifo_ctls,&descPtrTemp);
    descPtr = (MUHWI_Descriptor_t  * )descPtrTemp;
    memcpy (descPtr, desc, BGQ_MU_DESCRIPTOR_SIZE_IN_BYTES);
    //DUMPHEXSTR("inj descriptor",descPtr,BGQ_MU_DESCRIPTOR_SIZE_IN_BYTES);
    LOG_DESCR_HEADER(logregistry,descPtr);
    LOG_INJECT_DESCR(logregistry,&descPtr->PacketHeader);
    my_mbar();//no need for mbar once special TLB bit U1=1
    MUSPI_InjFifoAdvanceDesc (injfifo_ctls->injfifo);
    unlock(&injfifo_ctls->inj_fifo_lock);
    //$$$
    PRINT("injected %p desc_count=%llu \n",descPtr,(LLUS)desc_count);
    return desc_count;
}

int multi_directput(struct my_context * mcontext, MUHWI_PacketHeader_t * hdr, uint32_t bytes, MUDM_InjFifo_t * injfifo_ctls) 
{
  MUHWI_Descriptor_t * pt2Descriptor = NULL;
  int i=0;
  uint64_t           desc_count = -1; 
  int num_descriptors  = (hdr->NetworkHeader.pt2pt.Byte8.Size) >> 1; //Size=# of 32 byte chunks
  PRINT("num_descriptors=%d \n",num_descriptors);
  pt2Descriptor = (MUHWI_Descriptor_t *)(hdr + 1);
  for (i=0;i<num_descriptors;i++){
    desc_count = InjFifoInject_rget_emulation (injfifo_ctls,pt2Descriptor, &mcontext->mudm_hi_wrap_flight_recorder);
    pt2Descriptor++;//advance to next descriptor for next iteration
  }
  return 0;
};

int ok2RAS_pacing(struct my_context * mcontext){
  uint64_t now_timestamp=GetTimeBase2(); 
  uint64_t cycles_per_sec = microsec2cycles(mcontext->personality,1000000);  
  if ( (now_timestamp - mcontext->RAS_pacing_timestamp) >= (5 * cycles_per_sec) ){
      mcontext->RAS_pacing_timestamp = now_timestamp;   
      return 1;  
  }
  else return 0;
}

void reject_packet_ras(int return_code,void * hdr, uint32_t message_id){
     uint64_t * bytes8 = (uint64_t *)hdr;
     int i = 0;
     MUDM_RASBEGIN(5);
     MUDM_RASPUSH(-return_code);
     for(i=0;i<4;i++) MUDM_RASPUSH(bytes8[i]);
     MUDM_RASFINAL(message_id);    
}

void took2long_ras(void * hdr, uint32_t message_id){
     uint64_t * bytes8 = (uint64_t *)hdr;
     int i = 0;
     MUDM_RASBEGIN(4);
     for(i=0;i<4;i++) MUDM_RASPUSH(bytes8[i]);
     MUDM_RASFINAL(message_id);    
}

int reject_packet(struct my_context * mcontext, MUHWI_PacketHeader_t * hdr, uint32_t bytes, int return_code)
{
    __attribute((aligned(64))) MUHWI_Descriptor_t  md;
    MUHWI_Destination_t use_Destination;
    uint64_t           desc_count = -1; 

    SoftwareBytes_t *sw_hdr = (SoftwareBytes_t *)(&hdr->messageUnitHeader.Packet_Types.Memory_FIFO.Put_Offset_LSB);
    SoftwareBytes_t sb;
    uint8_t direction;
    uint8_t  io_port = 0;
    int IO_link_choice = 0;  

    sb.ionet_hdr.torus_source_node = sw_hdr->ionet_hdr.torus_dest_node;
    sb.ionet_hdr.torus_dest_node  = sw_hdr-> ionet_hdr.torus_source_node;
    sb.ionet_hdr.source_qpn  = sw_hdr-> ionet_hdr.dest_qpn ;
    sb.ionet_hdr.dest_qpn    = sw_hdr-> ionet_hdr.source_qpn;
    sb.ionet_hdr.payload_length = sw_hdr-> ionet_hdr.type; //stuff type received in otherwise unused length field
    sb.ionet_hdr.sequence_number = sw_hdr-> ionet_hdr.sequence_number;
    sb.ionet_hdr.type = MUDM_PKT_ERROR;
    switch (return_code){
      case -ENOTCONN:
      {
       sb.ionet_hdr.type = MUDM_PKT_ENOTCONN;
       if (ok2RAS_pacing(mcontext) ) reject_packet_ras(return_code,(void *)hdr,MUDMRAS_PKT_RCV_ERR);     
       break;
      }
      case -EPROTO:
      case -EILSEQ:
      {
        sb.ionet_hdr.type = MUDM_PKT_EILSEQ;
        reject_packet_ras(return_code,(void *)hdr,MUDMRAS_PKT_RCV_ERR_STOP); 
        break;
      }
      default: break;      
    }

    direction =  set_direction_request(sb.ionet_hdr.torus_source_node ,sb.ionet_hdr.torus_dest_node);
    if (CN2IONODE==direction) {/* need bridging CN which has IO link */
       use_Destination =mcontext->mybridgingCNnode;
    }
    else{
      use_Destination.Destination.Destination= sb.ionet_hdr.torus_dest_node;
      io_port = 0;
      return -ENOSYS; //not supported currently for other cases...
    }

    InitPt2PtMemoryFIFODescriptor(&md,
                                   &sb, //copy the originator Software Bytes
                                   0, //no payload physical addr
                                   0, //no payload length
                                   use_Destination,
                                   direction, 
                                   io_port,
                                   mcontext->sys_rec_fifo_id[IO_link_choice]
                                             );
   desc_count = InjFifoInject (&mcontext->injfifo_ctls[IO_link_choice], &md, &mcontext->mudm_hi_wrap_flight_recorder);

   return 0;
}

int mudm_recv_packet (struct my_context * mcontext, MUHWI_PacketHeader_t * hdr, uint32_t bytes) 
{
 
  SoftwareBytes_t *sw_hdr = (SoftwareBytes_t *)(&hdr->messageUnitHeader.Packet_Types.Memory_FIFO.Put_Offset_LSB);
  char * message = (char *)sw_hdr;
  //$$$   MDUMPHEXSTR("inbound memfifo ", sw_hdr+2, 16);
  memfifoRecvCount++;

  if (0==hdr->NetworkHeader.pt2pt.Byte2.Interrupt)  //oh no, not set--need to fix
  {
   MDUMPHEXSTR("No interrupt bit in this mudm_recv_packet ",hdr, 32);
   MHERE;
  }

  RCV_PACKET_DESCR(&mcontext->mudm_hi_wrap_flight_recorder, hdr);
  switch(sw_hdr->ionet_hdr.type)
  {   
    
     case MUDM_DPUT:
           //! \todo TODO Need to know receive q this came in on...
           if (hdr->messageUnitHeader.Packet_Types.Memory_FIFO.Rec_FIFO_Id == mcontext->sys_rec_fifo_id[0])
             multi_directput(mcontext,hdr,bytes,&mcontext->injfifo_ctls[0]);
           else if (NUM_IO_LINKS > 1){
             multi_directput(mcontext,hdr,bytes,&mcontext->injfifo_ctls[1]);
           }
     break;
      
     case MUDM_PKT_DATA_IMM:             
     case MUDM_PKT_DATA_REMOTE: 
     {    
        int rc=(mcontext-> recv)( message,mcontext->callback_context);
        if (rc < 0){
            DB_RECV_CALLBACK(&mcontext->mudm_hi_wrap_flight_recorder, rc, -rc,sw_hdr->ionet_hdr.type,__LINE__);
            if (bytes > 32){
              DB_RAW_DATA(&mcontext->mudm_hi_wrap_flight_recorder, hdr+32);
            }
            reject_packet(mcontext,hdr,bytes,rc);
        }
        break;
     }
     case MUDM_CONN_REQUEST : 
     { 
        connect_request(mcontext,hdr,bytes);       
        break; 
     } 
     case MUDM_CONN_REPLY :
     {
        connect_reply(mcontext,hdr,bytes);
        break;
     } 

     case MUDM_DISCONNECTED:
     {
       disconnect_request(mcontext,hdr,bytes);       
       break;
     }
     case MUDM_CNK2CNK_FIFO:
     {
       
       break;
     }

     case MUDM_PKT_EILSEQ:
     case MUDM_PKT_ERROR:
     case MUDM_PKT_ENOTCONN:
       if (ok2RAS_pacing(mcontext) ) reject_packet_ras(sw_hdr->ionet_hdr.type,(void *)hdr,MUDMRAS_PKT_REJECTED);
     break;

     case MUDM_IP_IMMED :
     case MUDM_IP_REMOTE:
     default: /*call default */
     {
       int rc= (mcontext-> recv)( message,mcontext->callback_context);
       if (rc<0){
            DB_RECV_CALLBACK(&mcontext->mudm_hi_wrap_flight_recorder, rc, -rc,sw_hdr->ionet_hdr.type,__LINE__);
            if (bytes > 32){
              DB_RAW_DATA(&mcontext->mudm_hi_wrap_flight_recorder, message+32);
            }
        }
     }//end default case
  }
  return 0;
}
EXPORT_SYMBOL(mudm_recv_packet);


static uint64_t wrap_count = 0;
MUHWI_PacketHeader_t * getNextPacketWrap (
						     MUSPI_RecFifo_t  *rec_fifo,
						     uint32_t         *pbytes) 
{
  
  void                   * va_head;         /* Snapshot of the fifo's head */  
  MUHWI_PacketHeader_t   * hdr;
  MUSPI_Fifo_t  *fifo_ptr = &rec_fifo->_fifo;
  int end_bytes=0, start_bytes=0;

  va_head = fifo_ptr->va_head;  
  /*MUSPI_assert (va_head <= fifo_ptr->va_tail);*/
  
  hdr = (MUHWI_PacketHeader_t *) va_head;
  *pbytes   = (hdr->NetworkHeader.pt2pt.Byte8.Size + 1) << 5;
  
  if ((uint64_t)va_head + *pbytes < (uint64_t)(fifo_ptr->va_end)) {
    fifo_ptr->va_head = (void*)((uint64_t)va_head + *pbytes);
    //$$$
    PRINT("WRAPPED CHECKED, SHORT \n");
    return hdr;
  }
  //$$$ 
  wrap_count++;
  PRINT("WRAPPED wrap_count = %llu \n", (LLUS)wrap_count);
  end_bytes = (uint64_t)(fifo_ptr->va_end) - (uint64_t)va_head;  
  start_bytes = *pbytes - end_bytes;

  DPRINT("va_head=%p fifo_ptr->va_start=%p start_bytes=%d end_bytes=%d \n",va_head,fifo_ptr->va_start,start_bytes,end_bytes);
  DUMPHEX(va_head,end_bytes);
  DUMPHEX(fifo_ptr->va_start, start_bytes);

  memcpy (rec_fifo->_wrapBuf, va_head, end_bytes);
  memcpy (rec_fifo->_wrapBuf + end_bytes, fifo_ptr->va_start, start_bytes);
  
  //DUMPHEX(rec_fifo->_wrapBuf, end_bytes+start_bytes);
  //DPRINT("print overrun past end of buffer\n");
  //DUMPHEX(va_head, end_bytes+start_bytes);
  
  fifo_ptr->va_head = (void*)((uint64_t)(fifo_ptr->va_start) + start_bytes);
  DPRINT("reset fifo_ptr->va_head=%p fifo_ptr->va_start=%p start_bytes=%d \n", fifo_ptr->va_head, fifo_ptr->va_start, start_bytes);
  return (MUHWI_PacketHeader_t *) rec_fifo->_wrapBuf;
}


uint64_t mudm_fifo_poll(struct my_context * mcontext, MUSPI_RecFifo_t       * rfifo)
{
  uint64_t packets_done=0;
  uint32_t wrap = 0;
  uint32_t cur_bytes = 0;
  uint32_t total_bytes = 0;
  uint32_t cumulative_bytes = 0;
  MUHWI_PacketHeader_t *hdr = 0;
  
  
  while ((total_bytes = MUSPI_getAvailableBytes (rfifo, &wrap)) != 0) 
  {
    PRINT(" %s total_bytes=%d wrap=%d \n",__FUNCTION__,total_bytes,wrap);
    if (wrap == 0) 
    {
      /* No fifo wrap.  Process each packet. */
      cumulative_bytes = 0;
      while (cumulative_bytes < total_bytes ) 
      {
	hdr = MUSPI_getNextPacketOptimized (rfifo, &cur_bytes);
        
        if (hdr->NetworkHeader.pt2pt.Data_Packet_Type==0x55)
          mudm_recv_packet(mcontext, hdr, cur_bytes); 
        else
          mudm_recv_bcast(mcontext, hdr, cur_bytes);  

	cumulative_bytes += cur_bytes;
        packets_done++;
        //PRINT("cumulative_bytes=%d cur_bytes=%d \n",cumulative_bytes, cur_bytes);
	/* Touch head for next packet. */
      }
    }
    else 
    {
      /* Packets wrap around to the top of the fifo.  Handle the one packet
       * that wraps.
       */
      hdr = getNextPacketWrap (rfifo, &cur_bytes);

      if (hdr->NetworkHeader.pt2pt.Data_Packet_Type==0x55)
          mudm_recv_packet(mcontext, hdr, cur_bytes); 
      else
          mudm_recv_bcast(mcontext, hdr, cur_bytes); 
      packets_done++; 
    }

    /* Store the updated fifo head. */
    MUSPI_syncRecFifoHwHead (rfifo);
   
  }
  return packets_done;
};
EXPORT_SYMBOL(mudm_fifo_poll);


int poll_iolink(void* mudm_context, uint32_t io_link){
  struct my_context * mcontext=(struct my_context *)mudm_context;
  uint64_t outstanding_pkt_reqid=0;
  uint64_t received_pkt=0;
  uint64_t counters_pending=0;

  if (mcontext->injfifo_ctls[io_link].state){
    if (mcontext->packetcontrols[io_link]-> pkt_reqid_list){
       outstanding_pkt_reqid += poll_pkt_message(mcontext,mcontext->packetcontrols[io_link]);
    }
  }
  if (mcontext->injfifo_ctls[io_link].state){
    if (mcontext->smallpa_obj_ctls[io_link]-> pkt_reqid_list){
       outstanding_pkt_reqid += poll_pkt_message(mcontext,mcontext->smallpa_obj_ctls[io_link]);
    }
  }
  if (mcontext->injfifo_ctls[io_link].state){
    
    received_pkt = mudm_fifo_poll(mudm_context, MUSPI_IdToRecFifo (0, &mcontext->MU_subgroup_mem[io_link]->rfifo_subgroup));   
  }
  if (mcontext->injfifo_ctls[io_link].state){
    counters_pending = rdma_poll_rdma_counter(mcontext,mcontext->rdma_obj_ctls[io_link]);
  }

  if (counters_pending) return 0;
  else if (outstanding_pkt_reqid) return 1;
  else if (mcontext->conn_pendlist[io_link] ){
#if 1
     struct mudm_connection * ccontext= mcontext->conn_pendlist[io_link];
     if (ccontext->my_timestamp){
         uint64_t now_timestamp=GetTimeBase2(); 
         if ( (now_timestamp - ccontext->my_timestamp)> (2*96000000000ull) ){//NUM * 2 minute of cycles
             // \todo: TODO actions of RAS, quit?
             //if (ok2RAS_pacing(mcontext) ) took2long_ras((void *)&ccontext->mu_iMemoryFifoDescriptor.PacketHeader,MUDMRAS_CN_TOOK2LONG);      
             //MPRINT("CN_TOOK2LONG now timestamp=%llu conn timestamp=%llu \n",(LLUS)now_timestamp,(LLUS)ccontext->my_timestamp);
             MUDM_CN_TOOK2LONG(&mcontext->mudm_hi_wrap_flight_recorder,&ccontext->mu_iMemoryFifoDescriptor.PacketHeader);
             ccontext->my_timestamp = 0; //take actions only once
         }
     } 
#endif   
     return 2;
  }
  //else return 1;

  return 0;

};

int mudm_poll_iolink(void* mudm_context, uint32_t io_link){
   poll_iolink(mudm_context, io_link);
   return 0;
};
EXPORT_SYMBOL(mudm_poll_iolink);


int mudm_poll_iolink_wakeup(void* mudm_context, uint32_t io_link){
  struct my_context * mcontext=(struct my_context *)mudm_context;
  struct MU_subgroup_memory *musm = mcontext-> MU_subgroup_mem[io_link];
  int last_poll_value;
  //MENTER;
  /** \param[in]  mask  Indicates which interrupts to clear.
 *                   - Bits  0 through 31 clear injection fifo threshold crossing
 *                   - Bits 32 through 47 clear reception fifo threshold crossing
 *                   - Bits 48 through 63 clear reception fifo packet arrival
 */

  //MPRINT("musm->clearInterrupts_mask=%llx \n",(LLUS)musm->clearInterrupts_mask);
  //MDUMPHEXSTR("&musm->rfifo_subgroup",&musm->rfifo_subgroup,sizeof(musm->rfifo_subgroup) );
  //MPRINT("mcontext->memblock[io_link].pa_start=%llx mcontext->memblock[io_link].mask=%llx \n",(LLUS)mcontext->memblock[io_link].pa_start,(LLUS)mcontext->memblock[io_link].mask );
  
  //MPRINT("musm->WU_ArmMU_subgroup_thread_mask=%llx \n",(LLUS)musm->WU_ArmMU_subgroup_thread_mask);
  while (mcontext->state){
    wakeup_clear_IPI();//clear any IPI issued wakeup
    MUSPI_ClearInterrupts (&musm->rfifo_subgroup, musm->clearInterrupts_mask );
    //MUSPI_RecFifoSubGroup_t * sg_ptr= &musm->rfifo_subgroup;
    //sg_ptr->groupStatus->clearInterruptStatus = (uint64_t)(-1);
    WU_ArmWithAddress(mcontext->memblock[io_link].pa_start,mcontext->memblock[io_link].mask);
    WU_ArmMU_system(musm->WU_ArmMU_subgroup_thread_mask);

    last_poll_value = poll_iolink(mudm_context, io_link);
    if  (mcontext->system_bcast.RDMA_bcast_control.state)
              rdma_bcast_poll_reduce(mcontext);

    if (last_poll_value){
      // MPRINT("last_poll_value=%llu ",(LLUS)last_poll_value); MHERE;  
      continue;   
    }

    wakeup_wait();
    
  }
  HERE;
  WU_DisarmMU(musm->WU_ArmMU_subgroup_thread_mask);
  
  EXIT;
  return 0;
};

int mudm_start_poll(void* mudm_context,uint32_t options)
{
  int i;
  struct my_context * mcontext=(struct my_context *)mudm_context;
  mcontext->poll_active = 1;

  //MENTER;
  if (mcontext->wakeupActive==0){
	  for(i=0;i<NUM_IO_LINKS;i++)  /*BEAM*//*loop doesn't iterate*/
	  {  

	      mudm_poll_iolink(mudm_context, i);
	  }
	  if  (mcontext->system_bcast.RDMA_bcast_control.state)
	      rdma_bcast_poll_reduce(mcontext);
  }
  else {
          for(i=0;i<NUM_IO_LINKS;i++)  /*BEAM*//*loop doesn't iterate*/
	  {  

	      mudm_poll_iolink_wakeup(mudm_context, i);
	  }
  }
  mcontext->poll_active = 0;
  return 0;
}
EXPORT_SYMBOL(mudm_start_poll);
