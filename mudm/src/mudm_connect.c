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

//Using MUDM_PRINT define here will print traces within this modulealloc_status = alloc_object(pc->num_pkts * packetsize,&pc->pkt_list_region,mcontext,NULL, free_mem);
//#define MUDM_PRINT 1

#include <spi/include/kernel/MU.h>
#include <spi/include/kernel/memory.h>

#include <mudm/include/mudm.h>
#include <mudm/include/mudm_inlines.h>
#include <mudm/include/mudm_utils.h>



#include "common.h"
#include "mudm_pkt.h"
#include "common_inlines.h"
#include "mudm_connection.h"
//#include "mudm_ras.h"


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

void log_mudm_bad_recv_conn_callback(BG_FlightRecorderRegistry_t* logregistry,uint64_t status, uint64_t linenum, struct mudm_connection * ccontext){
  uint64_t temp1= (status<<32) + linenum;
  uint64_t state = ccontext->state;
  uint64_t index = ccontext->my_index;
  uint64_t low16bit_timestamp = ccontext->lowbit_timestamp;
  uint64_t remote_index = ccontext->dest_conn_index;
  uint64_t temp2 = (state<<48) + (index<<32)+(remote_index<<16) +  low16bit_timestamp;
  MUDM_BAD_RECV_CONN_CALLBACK(logregistry,temp1,temp2,ccontext,ccontext->remote_timestamp);
};

/////////////////////////////////////////////////////////////////////////////////////////////
int conn_list_init( struct my_context * mcontext, uint32_t max_connections ){
  /*  connection context preparation */
  
  struct mudm_connection * mct; /* for working with connection context blocks */
  int alloc_status; /* status from allocate callback */
  int i;

  mcontext-> conn_freelist   = NULL; /* connection context free list */
  
  for (i=0; i < NUM_IO_LINKS; i++){  /*BEAM*//*loop doesn't iterate*/
    mcontext-> conn_pendlist[i]   = NULL;  /* connection started and waiting for acceptance */
    mcontext-> conn_activelist[i] = NULL;  /* connection active*/
    mcontext-> conn_abortlist[i] = NULL;  
  }
  
  alloc_status = alloc_object(max_connections * (sizeof(struct mudm_connection)),&mcontext->conn_list_mregion,mcontext,NULL, free_mem);
  mcontext->max_conn = max_connections;
  mcontext-> conn_freelist = mcontext-> conn_list_mregion.base_vaddr;
  if (alloc_status) return alloc_status;

  mct = mcontext-> conn_freelist;
  for (i=0; i<max_connections;i++)
  { 
    mct->my_physaddr = (uint64_t)mcontext->conn_list_mregion.base_paddr + i * sizeof(struct mudm_connection);
    mct->my_index = i;
    mct->maincontext=mcontext;
    mct->state=CONN_FREE;
    mct->nextconn = mct + 1;
    mct->prevconn = NULL;
    ++mct;
  } /* endfor i */
  --mct;/*back up one block to the last block*/
  mct->nextconn = NULL;
  mcontext->conn_list_lock.atom = 0; /* initialize connection spin lock */
  return 0;
};

void put_on_pending_list(struct my_context * mcontext, struct mudm_connection * ccontext,int LINK_INDEX){
   struct mudm_connection ** pendlist = &mcontext->conn_pendlist[LINK_INDEX];
   ccontext->conn_pendlist = pendlist;
    MSPIN_LOCK(mcontext->conn_list_lock);
      ccontext -> state = CONN_PENDING;
      if (*pendlist != NULL) {
        (*pendlist) -> prevconn = ccontext; 
      }
      ccontext -> nextconn = *pendlist;
      ccontext -> prevconn = NULL; 
      *pendlist = ccontext;
    MSPIN_UNLOCK(mcontext->conn_list_lock);
}

struct mudm_connection *conn_list_getfree(struct my_context * mcontext){

    struct mudm_connection *ccontext = NULL;
    
    //take a ccontext from the free list
    MSPIN_LOCK(mcontext->conn_list_lock);
      ccontext = mcontext-> conn_freelist;
      if (ccontext) mcontext-> conn_freelist = ccontext -> nextconn;  
    MSPIN_UNLOCK(mcontext->conn_list_lock);
    
    if (ccontext==NULL){
       return NULL;
    }
    //ccontext is floating, not on a list
    ccontext->my_timestamp = MUDM_CONN_GETFREE(&mcontext->mudm_hi_wrap_flight_recorder, &ccontext->state);
    ccontext->lowbit_timestamp = ccontext->my_timestamp;

    ccontext -> flight_recorder = &mcontext->mudm_hi_wrap_flight_recorder;
    ccontext -> nextconn = NULL; 
    ccontext -> prevconn = NULL; 
    ccontext -> maincontext = mcontext;
    ccontext -> magic = CONN_MAGIC;
    ccontext -> memfifo_sent = 0;
    ccontext -> rget_memfifo_sent=0;

    ccontext->injfifo_ctls = &mcontext->injfifo_ctls[0];//default

   return ccontext;
}

void connection_activate( struct my_context * mcontext, struct mudm_connection * ccontext ){
    struct mudm_connection ** activelist = ccontext->conn_activelist;
    struct mudm_connection ** pendlist   = ccontext->conn_pendlist;

    //put the ccontext on the active list
    MSPIN_LOCK(mcontext->conn_list_lock);
    /* remove from pending list and put on active list */

    /* remove from double-linked list */
    if (ccontext->nextconn != NULL) (ccontext->nextconn)->prevconn = ccontext->prevconn;
    if (ccontext->prevconn != NULL)  (ccontext->prevconn)->nextconn = ccontext->nextconn;
    if ( (*pendlist) == ccontext){ /* head pointer points to this conn context */
      *pendlist = ccontext->nextconn;
    }

    if ( (*activelist) != NULL) {
       (*activelist)->prevconn = ccontext; 
    }
    ccontext->prevconn = NULL;
    ccontext -> nextconn = *activelist;
    ccontext -> state = CONN_ACTIVE;
    *activelist = ccontext;
    MSPIN_UNLOCK(mcontext->conn_list_lock);
}

int connection_free( struct my_context * mcontext, struct mudm_connection * ccontext ){

   struct mudm_connection ** activelist = NULL;
   struct mudm_connection ** pendlist = NULL;
   struct mudm_connection ** abortlist = NULL;

   if (ccontext==NULL) return -EFAULT;
   CHECK_CONN_MAGIC(ccontext);

 
   //! \todo TODO  ??? Need an in-use count on the mudm_connection for in-flight processing in another thread
   //! and an intermediate state of CONN_FREE_PENDING so additional processing is inhibited......
   //! Consider an intemediate queue for CONN_FREE_PENDING so that the connection block is not reused off the free list
   //! OR could assume orderly processing  and just check state ????? 
   ccontext->state=CONN_FREE;  //do this right away!

   MSPIN_LOCK(mcontext->conn_list_lock);
    /* remove from pending list and put on active list */
    PRINT("Entering %s FUNCTION %s LINE %d DATE %s\n",__FILE__,__FUNCTION__,__LINE__, __DATE__);
    /* remove from double-linked list (active list or pending list) */
    if (ccontext->nextconn != NULL) (ccontext->nextconn)->prevconn = ccontext->prevconn;
    if ( likely(ccontext->prevconn != NULL) ) {
        (ccontext->prevconn)->nextconn = ccontext->nextconn;
    }
    else {
      activelist = ccontext->conn_activelist;
      pendlist   = ccontext->conn_pendlist;
      abortlist   = ccontext->conn_abortlist;
      if ( (*activelist !=NULL) && (*activelist == ccontext)){ /* head pointer points to this conn context */
        (*activelist) = ccontext->nextconn;
      }

      else if ((*pendlist != NULL) && (*pendlist == ccontext) ){ /* head pointer points to this conn context */
        *pendlist = ccontext->nextconn;
      }
      else if ((*abortlist != NULL) && (*abortlist == ccontext) ){ /* head pointer points to this conn context */
        *pendlist = ccontext->nextconn;
      }
      else{
         //MHERE;
         //PRINT("ERROR--CALLED connection_free w/o a valid active or pending list\n");
         //return -ENOTCONN;
      } 
    }
    ccontext -> prevconn = NULL;
    ccontext -> nextconn = mcontext-> conn_freelist;
    mcontext-> conn_freelist = ccontext;
    //DUMPHEXSTR("freed connection context \n",ccontext, sizeof(struct mudm_connection) ); 
    //@@@
    ccontext -> magic = CONN_MAGIC; //valid object check needs to work, check state after magic check
    MSPIN_UNLOCK(mcontext->conn_list_lock);
    MUDM_FREED_CONNECTION(&mcontext->mudm_hi_wrap_flight_recorder, &ccontext->state);
    EXIT;
    return 0;
}

int mudm_disconnect(void * context){

  struct mudm_connection * ccontext = (struct mudm_connection *)context; 
  struct mudm_disconnect_hdr* disc_hdr  = ( struct mudm_disconnect_hdr*)(&ccontext->mu_iMemoryFifoDescriptor.PacketHeader.messageUnitHeader.Packet_Types.Memory_FIFO.Put_Offset_LSB);
  
  struct my_context * mcontext;
  
  uint64_t desc_count = -1;
  HERE; 
  CHECK_CONN_MAGIC(ccontext);
  //@@@

  if (ccontext->state==CONN_FREE){
     MPRINT("ENOTCONN for ccontext=%p \n",ccontext);
     HERE;
     return -ENOTCONN;
  }
  else if (ccontext->state==CONN_DISCONNECTING)
  {
    MPRINT("ECONNRESET for ccontext=%p state is CONN_DISCONNECTING \n",ccontext);
    HERE;
    return -ECONNRESET;
  }

  Update_memfifo_payload(  &ccontext->mu_iMemoryFifoDescriptor, 0,0);
  disc_hdr->dest_context_index = ccontext -> dest_conn_index;  //will be used to find the ccontext on the remote
  disc_hdr->type = MUDM_DISCONNECTED;
  disc_hdr-> lowbytes_timestamp = ccontext -> remote_timestamp;
  //MPRINT("timestamp=%llx 16bit=%x \n", (LLUS) ccontext -> remote_timestamp, disc_hdr-> lowbytes_timestamp);
  ccontext->state=CONN_DISCONNECTING;
  mcontext = (struct my_context *)ccontext->maincontext;

  // \TODO: is the connection clean of transactions?  Need to check and address situation
  desc_count = InjFifoInject (ccontext->injfifo_ctls, &ccontext->mu_iMemoryFifoDescriptor,&mcontext->mudm_hi_wrap_flight_recorder);
  if ( unlikely( (uint64_t)(-1) == desc_count) ){
   return -ECANCELED;
  }

  //no need to wait or check for completion--no packet payload
  connection_free(mcontext,ccontext);

  #ifdef __LINUX__ 
  return -ENOSYS;
  #else
  return 0;
  #endif
};
EXPORT_SYMBOL(mudm_disconnect);


int disconnect_indicate(struct my_context * mcontext, struct mudm_connection *ccontext, int free_option){
  int callback_STATUS = 0;
  // flight record status indication......
  // flip source and destination torus address 
  // flip source and destination QPs
  // point at re-constructed memfifo ionet_header and issue recv up 
  struct ionet_header* ionet_hdr  = (struct ionet_header*)(&ccontext->mu_iMemoryFifoDescriptor.PacketHeader.messageUnitHeader.Packet_Types.Memory_FIFO.Put_Offset_LSB);
  uint64_t source_node = ionet_hdr-> torus_source_node;
  uint16_t source_qpnum = ionet_hdr-> source_qpn;
  ionet_hdr-> source_qpn = ionet_hdr-> dest_qpn;
  ionet_hdr-> dest_qpn =  source_qpnum;
  ionet_hdr-> torus_source_node = ionet_hdr-> torus_dest_node;
  ionet_hdr-> torus_dest_node = source_node;
  ionet_hdr-> type = MUDM_DISCONNECTED;
  ionet_hdr-> payload_length = 0;
  ionet_hdr-> sequence_number = ccontext->my_index;
  MUDM_DISC_INDICATION(&mcontext->mudm_hi_wrap_flight_recorder,&ccontext->mu_iMemoryFifoDescriptor.PacketHeader);
  callback_STATUS = (mcontext-> recv_conn)((void *) ccontext, ionet_hdr, mcontext->callback_context);
  //MHERE;
  if (callback_STATUS){
     log_mudm_bad_recv_conn_callback(&mcontext->mudm_hi_wrap_flight_recorder,callback_STATUS, __LINE__, ccontext);       
      MPRINT("callback_STATUS=%d @line=%d \n",callback_STATUS,__LINE__);
  }
  if (free_option) connection_free(mcontext,ccontext);
  return callback_STATUS;
};

int parse_dest2string(char * target,int target_length,MUHWI_Destination_t na){
  int length;
  if (na.Destination.Destination == (uint32_t)(-1) ){ return (length = snprintf(target, target_length, 
       "(unknown)" ) );}
  length = snprintf(target, target_length, 
       "(%d %d %d %d %d)",
        na.Destination.A_Destination,na.Destination.B_Destination,na.Destination.C_Destination,na.Destination.D_Destination,na.Destination.E_Destination);
  return length;
};

int32_t flush_iolink_connection_list(struct my_context * mcontext,struct mudm_connection ** conn_activelist){
  
  struct mudm_connection * ccontext = *conn_activelist;
  struct mudm_connection * first_ccontext = ccontext;
  struct mudm_connection * last_ccontext = ccontext;
  if (NULL==ccontext) return 0;
  int num_connections_freed = 0;
  while (NULL != ccontext){
     last_ccontext = ccontext;
     ccontext->state=CONN_FREE;
     disconnect_indicate(mcontext, ccontext, 0); //0=dontfree
     ccontext->conn_activelist=NULL;
     ccontext = ccontext->nextconn;
     num_connections_freed++;
  }  
  MPRINT("%s freed connections=%d \n",__FUNCTION__, num_connections_freed);
  //return the whole list to the free list
  MSPIN_LOCK(mcontext->conn_list_lock);
    last_ccontext->nextconn = mcontext-> conn_freelist;
     mcontext-> conn_freelist = first_ccontext;
    *conn_activelist = NULL;
  MSPIN_UNLOCK(mcontext->conn_list_lock);
  return 0;
};


int disconnect_request(struct my_context * mcontext, MUHWI_PacketHeader_t * hdr, uint32_t bytes){
      int callback_STATUS = 0;
      struct mudm_disconnect_hdr * disc_hdr = (struct mudm_disconnect_hdr *)(&hdr->messageUnitHeader.Packet_Types.Memory_FIFO.Put_Offset_LSB);
      char * message = (char *)disc_hdr;

      struct mudm_connection * ccontext = (struct mudm_connection *)(mcontext-> conn_list_mregion.base_vaddr) 
                                                                     + disc_hdr->dest_context_index;
      //MPRINT("ccontext=%p \n",ccontext);
      DUMPHEXSTR("MUHWI_PacketHeader_t hdr plus message",hdr,bytes);
      CHECK_CONN_MAGIC(ccontext);
      //! \todo TODO check whether timestamp matches what is in the ccontext.  Other checks are possible as well
      PRINT("Entering %s FUNCTION %s LINE %d DATE %s\n",__FILE__,__FUNCTION__,__LINE__, __DATE__); 
         
      // 65536 different possible timestamps, so rather rare for a random match on an error
      if (disc_hdr-> lowbytes_timestamp != ccontext->lowbit_timestamp){
        MPRINT("Mismatch 16-bit timestamp keys disc_hdr-> lowbytes_timestamp=%x local=%x \n",disc_hdr-> lowbytes_timestamp, ccontext->lowbit_timestamp);
        
        return -EINVAL;
      }

      if ( (ccontext->state==CONN_FREE)
       || (ccontext->state==CONN_DISCONNECTING) ) {
        uint32_t error=ENOTCONN;
        if (ccontext->state==CONN_DISCONNECTING) error=ECONNRESET;

        MPRINT("ENOTCONN/ECONNRESET for ccontext=%p state is CONN_FREE",ccontext); MHERE;
        uint64_t data0,data1,data2,data3;
        memcpy(&data0,&disc_hdr->torus_source_node,8);        
        memcpy(&data1,&disc_hdr->lowbytes_timestamp,8);
        data2 = (uint64_t)ccontext;            //grab the pointer to the local ccontext
        data3 =  error;
        data3 |= (( uint64_t) ccontext->remote_BlockID)<<32;
        MUDM_CONN_ERROR(ccontext -> flight_recorder,data0,data1,data2,data3);
        //MUDM_RASBEGIN(4); MUDM_RASPUSH(data0)  MUDM_RASPUSH(data1) MUDM_RASPUSH(data3) MUDM_RASPUSH(data3)  MUDM_RASFINAL(MUDMRAS_ENOCONN);
        return -error;
      }

      ccontext->state=CONN_DISCONNECTING;
      connection_free(mcontext,ccontext); 
      //MHERE;
      callback_STATUS = (mcontext-> recv_conn)((void *) ccontext,(struct ionet_header*)message, mcontext->callback_context);
      if (callback_STATUS){     
          log_mudm_bad_recv_conn_callback(&mcontext->mudm_hi_wrap_flight_recorder,callback_STATUS, __LINE__, ccontext);       
          MPRINT("callback_STATUS=%d @line=%d \n",callback_STATUS,__LINE__);
      }
      return callback_STATUS;
}
 

int set_direction_request(uint32_t src_torus,uint32_t dest_torus)
{
  if ( src_torus == MUDM_NODE_LOOPBACK) {
    PRINT("Configured NODE_LOOPBACK \n");
    return NODE_LOOPBACK;
  }
  if (src_torus == dest_torus){
    PRINT("NODE_LOOPBACK \n");
    return NODE_LOOPBACK;
  }
  else if (dest_torus==MUDM_CN2IO_NODE){
     PRINT("CN2IONODE\n");
     return CN2IONODE;
  }
  
  else if (src_torus & 0x80000000)/*IONODE*/
  { 
     if (dest_torus & 0x80000000 )return ION2ION;
     else return IONODE2CN;
  }
  else //src_torus is CN
  {
     if (dest_torus & 0x80000000 )return CN2IONODE;
     else return CN2CN;
  }
  
}

int  mudm_connect(void* mudm_context,
                 void** conn_context, 
                 uint32_t dest_node, 
                 struct ionet_connect * conn_request,
                 char * private_data,
                 uint16_t private_data_length)
{
    struct my_context * mcontext = (struct my_context *)mudm_context;

    char *msg2send = NULL; 
    char * payload=(char *)conn_request + sizeof(struct ionet_header);
    uint32_t payload_length = sizeof(struct ionet_connect)-sizeof(struct ionet_header);
    uint64_t           desc_count = -1; 
    struct pkt_descriptor * pktd = NULL;  /* for packet handling */

    SoftwareBytes_t  SoftwareBytes;
    struct mudm_connect_payload * mudm_payload; 
    struct mudm_connection * ccontext;  /* convenience variable */ 
    MUHWI_Destination_t use_Destination;
    
    if (mcontext->magic != MUDM_MAGIC) return -EINVAL;
    //print_ionet_connect(conn_request);

    //check private_data_length
    if (private_data_length>256) return -EINVAL;
    conn_request->private_data_length = private_data_length;
    PRINT("private_data_length=%d \n", private_data_length);

    ccontext = conn_list_getfree(mcontext);
    if (ccontext == NULL){
         MPRINT("ccontext==NULL, out or resources \n");
         return -EXFULL;
    }
    DUMPHEXSTR("ionet_connect conn_request",conn_request,sizeof(struct ionet_connect));

    *conn_context = ccontext;

    ccontext->direction=set_direction_request(mcontext->myTorusAddress.Destination.Destination,dest_node);

    //default until reason to override (default on index 0)
    ccontext->local_rec_FIFO_Id  = mcontext->sys_rec_fifo_id[0];
    ccontext->remote_rec_FIFO_Id = mcontext->sys_rec_fifo_id[0];
    ccontext->injfifo_ctls = &mcontext->injfifo_ctls[0];//default
    ccontext->packetcontrols = mcontext->packetcontrols[0];
    ccontext->smallpa_obj_ctls = mcontext->smallpa_obj_ctls[0];
    ccontext->rdma_obj_ctls    = mcontext->rdma_obj_ctls[0];
    ccontext-> conn_activelist = &mcontext-> conn_activelist[0];
    ccontext-> conn_abortlist = &mcontext-> conn_abortlist[0];

    //! \note BGQ assumes connections are made from compute nodes to an IO node for IO services
    //! and so a connection from the IO node to the compute node is not supported
    //! \note  Testing between IO nodes over the IO link--not ports 6 or 7 (D +/-)--has limited support
    //! \note  True connection support from IO node to compute nodes would require determining which IO link to use
    //! \note  Connections between IO nodes over the IO torus should be supported by another separate pair of 
    //! MU injection fifo and MU receive fifo distinct from the pair per each IO link
    if (ccontext->direction==IONODE2CN){
       ccontext->reverse_direction = CN2IONODE;
       if (mcontext-> my_using_IO_port != MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_IO_PORT){
          if (NUM_IO_LINKS>1){
             // Not expected that an IO node will do the connect request to the compute node, but use this for testing maybe
             ccontext->local_rec_FIFO_Id = mcontext->sys_rec_fifo_id[1];
             ccontext->injfifo_ctls = &mcontext->injfifo_ctls[1]; //mcontext->IdToInjFifo[1];//use the 2nd link
             ccontext->packetcontrols = mcontext->packetcontrols[1];
             ccontext->smallpa_obj_ctls = mcontext->smallpa_obj_ctls[1];
             ccontext->rdma_obj_ctls    = mcontext->rdma_obj_ctls[1];
             ccontext-> conn_activelist = &mcontext-> conn_activelist[1];
             ccontext-> conn_abortlist = &mcontext-> conn_abortlist[1];
          }
       } 
    }
    else if (ccontext->direction==CN2IONODE){
       ccontext->reverse_direction = IONODE2CN;
       
       if (mcontext-> my_using_IO_port != MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_IO_PORT){
          ccontext->remote_rec_FIFO_Id = get_init_sys_rec_fifo_id(1);
       }
    }
    else ccontext->reverse_direction = ccontext->direction; /* same type of node to node either direction */

    pktd = alloc_pkt_message(ccontext->packetcontrols);
    if (pktd == NULL){
     MPRINT("\n pktd==NULL %s line %d \n",__FUNCTION__,__LINE__);
     connection_free((struct my_context *)ccontext->maincontext,ccontext);
     return -EBUSY;
    }
    msg2send = (char *)pktd->pkt_msg;
    //PRINT("msg2send pointer=%p \n",msg2send);
    memcpy(msg2send,payload,payload_length);
        // handle private_data
    if (private_data){
      memcpy(msg2send+ payload_length,private_data,private_data_length);
    }

    //adjust for private_data in setting the mudm_payload location
    DUMPHEXSTR("connect payload + private data",msg2send,payload_length+private_data_length);

    PRINT("direction=%d \n",ccontext->direction);
    
    ccontext->lowbit_timestamp = ccontext->my_timestamp;

    mudm_payload = (struct mudm_connect_payload *)( msg2send + 256);    
    mudm_payload-> source_conn_index = ccontext->my_index; 
    mudm_payload-> version = MUDM_VERSION;
    mudm_payload-> dest_conn_index = -1;  // use an invalid value

    ccontext -> io_port = mcontext->my_using_IO_port;
    mudm_payload-> using_IO_port = mcontext->my_using_IO_port;

    //! \todo Need an algorithm for timestamp computation
    mudm_payload-> timestamp = ccontext->my_timestamp; 

    mudm_payload->rec_FIFO_Id = ccontext->local_rec_FIFO_Id;

    mudm_payload-> myBridgeTorusAddress = mcontext->mybridgingCNnode;
    mudm_payload-> sourceUCI = mcontext->myUCI; 
    mudm_payload-> source_BlockID =  mcontext-> myBlockID;                                

    MUDM_CONN_INJ_PAYLOAD(&mcontext->mudm_hi_wrap_flight_recorder,mudm_payload);
    
    memcpy(&SoftwareBytes.ionet_hdr,conn_request,sizeof(SoftwareBytes_t) );

    SoftwareBytes.ionet_hdr.sequence_number = ccontext->my_index;
    SoftwareBytes.ionet_hdr.torus_source_node = mcontext->myTorusAddress.Destination.Destination;
    SoftwareBytes.ionet_hdr.torus_dest_node = dest_node;
    ccontext->destination.Destination.Destination=dest_node;

    // adjust header length for private data
    SoftwareBytes.ionet_hdr.payload_length = payload_length + private_data_length;
    SoftwareBytes.ionet_hdr.type = MUDM_CONN_REQUEST;


    if (CN2IONODE==ccontext->direction) {/* need bridging CN which has IO link */
       use_Destination =mcontext->mybridgingCNnode;
       PRINT("CN2IONODE dest node is BCN\n");
    }
    else if (NODE_LOOPBACK==ccontext->direction){/* deal with loopback */
      SoftwareBytes.ionet_hdr.torus_dest_node=mcontext->myTorusAddress.Destination.Destination; //destination is same as source
      use_Destination=mcontext->myTorusAddress; //destination is same as source
      PRINT("NODE_LOOPBACK dest node\n");
    }
    else /* Use actual Destination value */
    {
      use_Destination = ccontext->destination;
    }
    
    DUMPHEXSTR("SoftwareBytes",&SoftwareBytes,sizeof(SoftwareBytes)); 
    InitPt2PtMemoryFIFODescriptor(  &ccontext->mu_iMemoryFifoDescriptor,
                                               &SoftwareBytes,
                                               pktd->message_pa,/* packet address */
                                               256 + sizeof(struct mudm_connect_payload),  
                                               use_Destination,
                                               ccontext->direction, 
                                               ccontext -> io_port,
                                               ccontext->remote_rec_FIFO_Id
                                             );


   initPt2PtDirectPutDescriptor( &ccontext->mu_iDirectPutDescriptor,0,0,use_Destination,ccontext->direction,
                                 ccontext-> io_port,0,0);
 
    
    if (CN2IONODE==ccontext->reverse_direction) {/* need bridging CN which has IO link */
       ccontext->reverse_destination=ccontext->remote_bridgingCNnode;  
       //! \todo This needs to be updated on the connect reply 
    }
    else { /* Use actual Destination value */
           ccontext->reverse_destination=mcontext->myTorusAddress;
    }

    ccontext -> reverse_io_port = ccontext -> io_port; 
    if (ccontext -> reverse_io_port == 0xff) ccontext -> reverse_io_port = MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_IO_PORT;

    initPt2PtDirectPutDescriptor( &ccontext->mu_iDirectPutDescriptor4rget,0,0,ccontext->reverse_destination,
                                  ccontext->reverse_direction,ccontext-> reverse_io_port,0,0);

    put_on_pending_list(mcontext, ccontext,0);  //put on pending list
    desc_count = InjFifoInject (ccontext->injfifo_ctls, &ccontext->mu_iMemoryFifoDescriptor,&mcontext->mudm_hi_wrap_flight_recorder);
    if ( unlikely( (uint64_t)(-1) == desc_count) ){  
       release_pkt_to_free(pktd, desc_count);
       // need to free the connection
       connection_free(mcontext,ccontext);
       return -ECANCELED;
    }
    if (MUSPI_CheckDescComplete(ccontext->injfifo_ctls->injfifo, desc_count) ){
      release_pkt_to_free(pktd, desc_count);
    } 
    else {//handle asynchronously 
       pktd->flightinfo4delay = (uint64_t *)&ccontext->mu_iMemoryFifoDescriptor.PacketHeader;
       release_pkt_to_poll_ccontext(pktd, desc_count,ccontext);
       wakeup();
    }
    
return 0;
}
EXPORT_SYMBOL(mudm_connect);


int mudm_conn_reply(void* conn_context,
                struct ionet_connect * conn_reply,
                char * private_data,
                uint16_t private_data_length)
{
    struct my_context * mcontext = NULL;
    char * payload=(char *)conn_reply + sizeof(struct ionet_header);
    uint32_t payload_length = sizeof(struct ionet_connect)-sizeof(struct ionet_header);
    uint32_t status = conn_reply->status;
    uint32_t return_status = 0;
    

    uint64_t           desc_count = -1; 
    struct pkt_descriptor * pktd = NULL;  /* for packet handling */

    SoftwareBytes_t  SoftwareBytes;
    struct mudm_connect_payload * mudm_payload; 

    struct mudm_connection * ccontext = (struct mudm_connection *)conn_context; 

    MUHWI_Destination_t use_Destination = ccontext->destination;  //default using destination
    //uint64_t msg_size_in_bytes = payload_length +private_data_length; 
    char * msg2send = NULL;

    if (ccontext -> memfifo_sent){
          RESET_SEQNUM_TO_ZERO(ccontext -> flight_recorder,ccontext);
          MPRINT("SOFT ERROR: NONZERO ccontext -> memfifo_sent=%llu\n",(LLUS)ccontext -> memfifo_sent);
          ccontext -> memfifo_sent = 0;
    }
    //check private_data_length
    if (private_data_length>256) return -EINVAL;
    conn_reply->private_data_length = private_data_length;
    PRINT("private_data_length=%d \n", private_data_length);
    
    DUMPHEXSTR("ionet_connect conn_reply",conn_reply,sizeof(struct ionet_connect));

    CHECK_CONN_MAGIC(ccontext);
    mcontext = (struct my_context *)ccontext->maincontext;
    if (mcontext->magic != MUDM_MAGIC) return -EFAULT;

    pktd = alloc_pkt_message(ccontext->packetcontrols);
    if (pktd == NULL){
     MPRINT("pktd==NULL %s line %d \n",__FUNCTION__,__LINE__);
     return -EBUSY;  
    }
    msg2send = (char *)pktd->pkt_msg;
    memcpy(msg2send,payload,payload_length);
    // handle private_data
    if (private_data){
      memcpy(msg2send+ payload_length,private_data,private_data_length);
    }

    //Need to update the memfifo packet header so that disconnect indicate will work if connection died during accept processing
    
    memcpy(&SoftwareBytes.ionet_hdr,conn_reply,sizeof(SoftwareBytes_t) );
    SoftwareBytes.ionet_hdr.sequence_number = ccontext->my_index;
    SoftwareBytes.ionet_hdr.torus_source_node = mcontext->myTorusAddress.Destination.Destination;
    SoftwareBytes.ionet_hdr.torus_dest_node = ccontext->destination.Destination.Destination;
    // adjust header length for private data
    SoftwareBytes.ionet_hdr.payload_length = payload_length + private_data_length;
    SoftwareBytes.ionet_hdr.type = MUDM_CONN_REPLY;
    //MDUMPHEXSTR("SoftwareBytes",&SoftwareBytes,sizeof(SoftwareBytes));
    
    if (CN2IONODE==ccontext->direction) {/* need bridging CN which has IO link */
       use_Destination = mcontext->mybridgingCNnode;
    }

    InitPt2PtMemoryFIFODescriptor(  &ccontext->mu_iMemoryFifoDescriptor,
                                               &SoftwareBytes,
                                               pktd->message_pa,/* packet address */
                                               256 + sizeof(struct mudm_connect_payload), 
                                               use_Destination,
                                               ccontext->direction, 
                                               ccontext -> io_port,
                                               ccontext->remote_rec_FIFO_Id
                                             );

    //MPRINT("state=%llx \n",(LLUS)ccontext->state);
    if (ccontext->state==CONN_ABORTED){
       release_pkt_to_free(pktd, desc_count);
       return_status = -ECANCELED;
       goto CONN_RESP_ERR;
    }

    //adjust for private_data in setting the mudm_payload location
    DUMPHEXSTR("msg2send before payload stuff",msg2send,payload_length+private_data_length);

    mudm_payload = (struct mudm_connect_payload *)( msg2send + 256);   
    mudm_payload-> source_conn_index = ccontext->my_index; 
    mudm_payload-> version = MUDM_VERSION; 
    mudm_payload-> dest_conn_index = ccontext->dest_conn_index;
   
    mudm_payload-> using_IO_port = ccontext -> io_port; //set on inbound connection request from mudm_payload
    mudm_payload-> timestamp = ccontext->my_timestamp;  

    mudm_payload->rec_FIFO_Id = ccontext->local_rec_FIFO_Id;
    
    mudm_payload-> myBridgeTorusAddress = mcontext->mybridgingCNnode;
    mudm_payload-> sourceUCI = mcontext->myUCI;
    mudm_payload-> source_BlockID =  mcontext-> myBlockID;

    MUDM_CONN_INJ_PAYLOAD(&mcontext->mudm_hi_wrap_flight_recorder,mudm_payload);


   initPt2PtDirectPutDescriptor( &ccontext->mu_iDirectPutDescriptor,0,0,ccontext->destination,ccontext->direction,
                                 ccontext-> io_port,0,0);                                            

    if (CN2IONODE==ccontext->reverse_direction) {/* need bridging CN which has IO link */
       ccontext->reverse_destination=ccontext->remote_bridgingCNnode; 
    }
    else { /* Use actual Destination value */
      PRINT("NOT CN2IONODE and NOT IONODE2CN  \n");
      ccontext->reverse_destination = mcontext->myTorusAddress;
    }
  
    ccontext -> reverse_io_port = ccontext -> io_port; 
    HERE;
    PRINT("ccontext->reverse_io_port=%x \n",ccontext -> reverse_io_port);
    initPt2PtDirectPutDescriptor( &ccontext->mu_iDirectPutDescriptor4rget,0,0,ccontext->reverse_destination,ccontext->reverse_direction,
                                 ccontext-> reverse_io_port,0,0);


   

    //MPRINT("ccontext=%p, ccontext->packetcontrols->IdToInjFifo=%p ccontext->injfifo_ctls->injfifo=%p\n",ccontext,ccontext->packetcontrols->IdToInjFifo, ccontext->injfifo_ctls->injfifo);
      //MPRINT("ccontext=%p, ccontext->smallpa_obj_ctls->IdToInjFifo=%p ccontext->injfifo_ctls->injfifo=%p\n",ccontext,ccontext->smallpa_obj_ctls->IdToInjFifo, ccontext->injfifo_ctls->injfifo);

    if (status){
       uint64_t data0,data1,data2,data3;      
        memcpy(&data0,&conn_reply->ionet_hdr.torus_source_node,8);
        //data0 = *(uint64_t *)&ic->ionet_hdr.torus_source_node; //will get source and dest nodes with this cheat
        memcpy(&data1,&conn_reply->source_port,8);
        //data1 = *(uint64_t *)&ic->source_port; //will get source and dest ports with this cheat
        data2 = (uint64_t)ccontext;            //grab the pointer to the local ccontext
        data3 =  -status;
        data3 |= (( uint64_t) ccontext->remote_BlockID)<<32;  
        ccontext->state=CONN_DISCONNECTING;
        MUDM_CONN_ERROR(ccontext -> flight_recorder,data0,data1,data2,data3);
    }
    else if (ccontext->state==CONN_PENDING){
      /* activate the connection and put it on the active list */
      connection_activate(mcontext, ccontext );
    }
    else {
      MPRINT("ccontext state = %llx \n", (LLUS)ccontext->state);  MHERE;
      connection_activate(mcontext, ccontext );
    }
    if (ccontext -> memfifo_sent){
          RESET_SEQNUM_TO_ZERO(ccontext -> flight_recorder,ccontext);
          MPRINT("SOFT ERROR: NONZERO ccontext -> memfifo_sent=%llu\n",(LLUS)ccontext -> memfifo_sent);
          ccontext -> memfifo_sent = 0;
    }
    desc_count = InjFifoInject (ccontext->injfifo_ctls, &ccontext->mu_iMemoryFifoDescriptor,&mcontext->mudm_hi_wrap_flight_recorder);
    if ( unlikely( (uint64_t)(-1) == desc_count) ){  
       release_pkt_to_free(pktd, desc_count);
       return_status = -ECANCELED;
       goto CONN_RESP_ERR;
    }
    if (ccontext -> memfifo_sent){
          RESET_SEQNUM_TO_ZERO(ccontext -> flight_recorder,ccontext);
          MPRINT("SOFT ERROR: NONZERO ccontext -> memfifo_sent=%llu\n",(LLUS)ccontext -> memfifo_sent);
          ccontext -> memfifo_sent = 0;
    }
    SoftwareBytes.ionet_hdr.sequence_number = 0; //reset this...

    if (MUSPI_CheckDescComplete(ccontext->injfifo_ctls->injfifo, desc_count) ){
      release_pkt_to_free(pktd, desc_count);
    } 
    else {//handle asynchronously 
       pktd->flightinfo4delay = (uint64_t *)&ccontext->mu_iMemoryFifoDescriptor.PacketHeader;
       release_pkt_to_poll_ccontext(pktd, desc_count,ccontext);
       wakeup();
    }
    if (status != 0){ /* release the connection cb */
      goto CONN_RESP_ERR;
    }
    //if (ccontext->destination.Destination.Destination==0)dump_ccontext_info(ccontext);//for testing of dump_context_info
 return 0;

 CONN_RESP_ERR:
    
  if (status==0){ 
   uint64_t data0,data1,data2,data3;

   memcpy(&data0,&conn_reply->ionet_hdr.torus_source_node,8);
   //data0 = *(uint64_t *)&ic->ionet_hdr.torus_source_node; //will get source and dest nodes with this cheat
   memcpy(&data1,&conn_reply->source_port,8);
   //data1 = *(uint64_t *)&ic->source_port; //will get source and dest ports with this cheat
   data2 = (uint64_t)ccontext;            //grab the pointer to the local ccontext
   data3 =  -return_status;
   data3 |= (( uint64_t) ccontext->remote_BlockID)<<32;
   ccontext->state=CONN_ORPHANED;
   MUDM_CONN_ERROR(ccontext -> flight_recorder,data0,data1,data2,data3);
   //disconnect_indicate(mcontext, ccontext, 1); //1=free
   connection_free(mcontext, ccontext);
   return -ENOTCONN;
  }
  else {//connection was rejected by MUDM user, so just free the connection
    connection_free(mcontext, ccontext);
  }
  return 0;  
}
EXPORT_SYMBOL(mudm_conn_reply);


int connect_request(struct my_context * mcontext, MUHWI_PacketHeader_t * hdr, uint32_t bytes){
       int callback_STATUS = 0;
       SoftwareBytes_t *sw_hdr = (SoftwareBytes_t *)(&hdr->messageUnitHeader.Packet_Types.Memory_FIFO.Put_Offset_LSB);
       char * message = (char *)sw_hdr;
       struct ionet_connect * ion_conn = (struct ionet_connect *)(message);
       struct ionet_header * ionet_hdr = (struct ionet_header *)(message);
       struct mudm_connect_payload * mudm_payload = (struct mudm_connect_payload *)( (char *)hdr + 32 + 256);
       struct mudm_connection * ccontext;  /* convenience variable */ 
       int LINK_INDEX = 0;  //Assume first link unless indicated otherwise

       PRINT("Entering %s FUNCTION %s LINE %d DATE %s\n",__FILE__,__FUNCTION__,__LINE__, __DATE__); 
       DUMPHEXSTR("MUHWI_PacketHeader_t hdr",hdr,bytes);
       
       ccontext = conn_list_getfree(mcontext);
       if (ccontext == NULL){
         MPRINT("ccontext==NULL, out of resources (inbound connect request)\n");          
         MUDM_CONN_LIST(&mcontext->mudm_hi_wrap_flight_recorder,ccontext,mcontext-> conn_freelist,mcontext-> conn_pendlist,mcontext->conn_activelist);
         return -EXFULL;
         //! \todo TODO Tell other side to die w/ reason -EXFULL
         ////// build a special disconnect to MUDM on other node and then pop status via special connect reply to MUDM user???
       }

        MUDM_CONN_RCV_PAYLOAD(&mcontext->mudm_hi_wrap_flight_recorder,mudm_payload);

        ccontext->remote_rec_FIFO_Id = mudm_payload->rec_FIFO_Id;
        
        // deterimine direction consideration
        ccontext->direction=set_direction_request(mcontext->myTorusAddress.Destination.Destination,
                            ionet_hdr->torus_source_node);

        //! \note BGQ assumes connections are made from compute nodes to an IO node for IO services
        //! and so a connection from the IO node to the compute node is not supported
        //! \note  Testing between IO nodes over the IO link--not ports 6 or 7 (D +/-)--has limited support
        //! \note  True connection support from IO node to compute nodes would require determining which IO link to use
        //! \note  Connections between IO nodes over the IO torus should be supported by another separate pair of 
        //! MU injection fifo and MU receive fifo distinct from the pair per each IO link
        if (ccontext->direction==IONODE2CN){
           ccontext->reverse_direction = CN2IONODE;

           if (mudm_payload-> using_IO_port != MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_IO_PORT){
              if (NUM_IO_LINKS>1) {
                LINK_INDEX = 1;
              }
           } 
        }
        else if (ccontext->direction==CN2IONODE){
           ccontext->reverse_direction = IONODE2CN;
        }
        else ccontext->reverse_direction = ccontext->direction; /* same type of node to node either direction */

        ccontext->injfifo_ctls = &mcontext->injfifo_ctls[LINK_INDEX];
        ccontext->local_rec_FIFO_Id = mcontext->sys_rec_fifo_id[LINK_INDEX];
        ccontext->packetcontrols = mcontext->packetcontrols[LINK_INDEX];
        ccontext->smallpa_obj_ctls = mcontext->smallpa_obj_ctls[LINK_INDEX];
        ccontext->rdma_obj_ctls    = mcontext->rdma_obj_ctls[LINK_INDEX];
        ccontext-> conn_activelist = &mcontext-> conn_activelist[LINK_INDEX];
        ccontext-> conn_abortlist = &mcontext-> conn_abortlist[LINK_INDEX];

        mcontext -> remote_BlockID[LINK_INDEX] = mudm_payload->source_BlockID;
        if ( unlikely(0==mcontext ->link4reset[LINK_INDEX]) ){
           if (mudm_payload-> using_IO_port == MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_IO_PORT){ 
             mcontext ->link4reset[LINK_INDEX]=10;
           }
           else if (mudm_payload-> using_IO_port == MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_TORUS_PORT_6){
              mcontext ->link4reset[LINK_INDEX]=6;
           }
            else if (mudm_payload-> using_IO_port == MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_TORUS_PORT_7){
              mcontext ->link4reset[LINK_INDEX]=7;
           }
           else{
              MPRINT("unexpected value for mudm_payload-> using_IO_port = %x \n",mudm_payload-> using_IO_port);
           }

        }
        if (mudm_payload-> version != MUDM_VERSION){
          MPRINT("MISMATCH in MUDM Version between Nodes \n");
        }
        ccontext -> destination.Destination.Destination = ionet_hdr->torus_source_node; //destination is source node address of remote
        ccontext -> dest_conn_index = mudm_payload-> source_conn_index;          
        ccontext -> remote_bridgingCNnode = mudm_payload-> myBridgeTorusAddress;       
        ccontext -> reverse_io_port = mudm_payload-> using_IO_port;
        ccontext -> io_port = mudm_payload-> using_IO_port;
        ccontext -> remote_BlockID = mudm_payload->source_BlockID;
        ccontext -> remote_timestamp = mudm_payload->timestamp;

        memcpy(&ccontext->mu_iMemoryFifoDescriptor.PacketHeader,hdr,32);//added for CN_TOOK2LONG hang detection on receiving node (IO node)
        put_on_pending_list(mcontext, ccontext,LINK_INDEX);
        
         //MPRINT("ccontext=%p, ccontext->packetcontrols->IdToInjFifo=%p ccontext->injfifo_ctls->injfifo=%p\n",ccontext,ccontext->packetcontrols->IdToInjFifo, ccontext->injfifo_ctls->injfifo);
        //MPRINT("ccontext=%p, ccontext->smallpa_obj_ctls->IdToInjFifo=%p ccontext->injfifo_ctls->injfifo=%p\n",ccontext,ccontext->smallpa_obj_ctls->IdToInjFifo, ccontext->injfifo_ctls->injfifo);
        if (ccontext -> memfifo_sent){
          RESET_SEQNUM_TO_ZERO(ccontext -> flight_recorder,ccontext);
          MPRINT("SOFT ERROR: NONZERO ccontext -> memfifo_sent=%llu\n",(LLUS)ccontext -> memfifo_sent);
          ccontext -> memfifo_sent = 0;
        }
        callback_STATUS = (mcontext-> recv_conn)((void *) ccontext,(struct ionet_header*)ion_conn, mcontext->callback_context);
        if (ccontext -> memfifo_sent){
          RESET_SEQNUM_TO_ZERO(ccontext -> flight_recorder,ccontext);
          MPRINT("SOFT ERROR: NONZERO ccontext -> memfifo_sent=%llu\n",(LLUS)ccontext -> memfifo_sent);
          ccontext -> memfifo_sent = 0;
        }
        if (callback_STATUS){   
          log_mudm_bad_recv_conn_callback(&mcontext->mudm_hi_wrap_flight_recorder,callback_STATUS, __LINE__, ccontext);       
          MPRINT("callback_STATUS=%d @line=%d \n",callback_STATUS,__LINE__);
        }
    return callback_STATUS;
}

int connect_reply(struct my_context * mcontext, MUHWI_PacketHeader_t * hdr, uint32_t bytes){
      int callback_STATUS = 0;

      SoftwareBytes_t *sw_hdr = (SoftwareBytes_t *)(&hdr->messageUnitHeader.Packet_Types.Memory_FIFO.Put_Offset_LSB);

      struct ionet_connect * ic = (struct ionet_connect *)sw_hdr;
      uint32_t status = ic->status;
      SoftwareBytes_t * sbt;
      struct mudm_connect_payload * mudm_payload = (struct mudm_connect_payload *)( (char *)hdr + 32 + 256);
      struct mudm_connection * ccontext = NULL;  /* convenience variable */ 

      //PRINT("Entering %s FUNCTION %s LINE %d DATE %s\n",__FILE__,__FUNCTION__,__LINE__, __DATE__); 
      MUDM_CONN_RCV_PAYLOAD(&mcontext->mudm_hi_wrap_flight_recorder,mudm_payload);     
      
      if (mudm_payload-> dest_conn_index < mcontext->max_connections){
        ccontext = (struct mudm_connection *)(mcontext-> conn_list_mregion.base_vaddr) + mudm_payload-> dest_conn_index;
      }
      else {
        MHERE;
        MPRINT("error in mudm_payload-> dest_conn_index=%x \n", mudm_payload-> dest_conn_index);
        return -1;
      }
      
      CHECK_CONN_MAGIC(ccontext);
      /* \TODO: need to do a sanity check on ccontext?   verify mcontext == ccontext -> maincontext */
      if (mcontext!= ccontext->maincontext) {
        MPRINT("\nmcontext!= ccontext->maincontext \n\n");
      }

      sbt = addressSoftwareBytes( &ccontext->mu_iMemoryFifoDescriptor );
      sbt->ionet_hdr.dest_qpn = sw_hdr->ionet_hdr.source_qpn;
      sbt->ionet_hdr.torus_dest_node = sw_hdr->ionet_hdr.torus_source_node;

      if ((ccontext -> io_port != mudm_payload-> using_IO_port) 
          && (IONODE2CN==ccontext->reverse_direction)
         ){//
         ccontext -> reverse_io_port = mudm_payload-> using_IO_port;
         ccontext -> io_port = mudm_payload-> using_IO_port;
         //! \todo Need to update remote get direct put io port
         initPt2PtDirectPutDescriptor( &ccontext->mu_iDirectPutDescriptor4rget,0,0,ccontext->reverse_destination,
                                       ccontext->reverse_direction, ccontext-> reverse_io_port,0,0);
      }

      if (mudm_payload-> version != MUDM_VERSION){
          MPRINT("MISMATCH in MUDM Version between Nodes \n");
      }

      ccontext -> dest_conn_index = mudm_payload-> source_conn_index;          
      ccontext -> remote_bridgingCNnode = mudm_payload-> myBridgeTorusAddress;
      ccontext -> remote_timestamp = mudm_payload->timestamp;
      ccontext -> remote_BlockID = mudm_payload->source_BlockID;
      // Would be unusual to tell originator to use a different receive FIFO, but handle it 
      if (ccontext->remote_rec_FIFO_Id != mudm_payload->rec_FIFO_Id){
         ccontext->remote_rec_FIFO_Id = mudm_payload->rec_FIFO_Id;
         ccontext->mu_iMemoryFifoDescriptor.PacketHeader.messageUnitHeader.Packet_Types.Memory_FIFO.Rec_FIFO_Id 
              = mudm_payload->rec_FIFO_Id; 
      }

      /* activate the connection and put it on the active list */
      connection_activate(mcontext, ccontext );
     
      if (status != 0) {
        uint64_t data0,data1,data2,data3;
        memcpy(&data0,&ic->ionet_hdr.torus_source_node,8);
        //data0 = *(uint64_t *)&ic->ionet_hdr.torus_source_node; //will get source and dest nodes with this cheat
        memcpy(&data1,&ic->source_port,8);
        //data1 = *(uint64_t *)&ic->source_port; //will get source and dest ports with this cheat
        data2 = (uint64_t)ccontext;            //grab the pointer to the local ccontext
        data3 =  -status;
        data3 |= (( uint64_t) ccontext->remote_BlockID)<<32;
        ccontext->state=CONN_DISCONNECTING;
        MUDM_CONN_ERROR(ccontext -> flight_recorder,data0,data1,data2,data3);

      }
      MUDM_CONN_START(&mcontext->mudm_hi_wrap_flight_recorder, &ccontext->state);
      callback_STATUS = (mcontext-> recv_conn)((void *) ccontext,(struct ionet_header*)ic, mcontext->callback_context);
      if (callback_STATUS){
          log_mudm_bad_recv_conn_callback(&mcontext->mudm_hi_wrap_flight_recorder,callback_STATUS, __LINE__, ccontext);       
      }
      if (status != 0) {
        connection_free((struct my_context *)ccontext->maincontext,ccontext);
      }
      return callback_STATUS;
}

void dump_ccontext_info(struct mudm_connection * ccontext){
  MENTER;
  LOG_MF_TEMPLATE(ccontext -> flight_recorder,&ccontext->mu_iMemoryFifoDescriptor.PacketHeader.NetworkHeader.pt2pt); 
  LOG_DP_TEMPLATE(ccontext -> flight_recorder,&ccontext->mu_iDirectPutDescriptor.PacketHeader.NetworkHeader.pt2pt);
  LOG_RGET_DP_TEMPLATE(ccontext -> flight_recorder,&ccontext->mu_iDirectPutDescriptor4rget.PacketHeader.NetworkHeader.pt2pt);
  MUDM_STATUS_CONNECTION(ccontext -> flight_recorder,&ccontext->state);
  MEXIT;
  //MDUMPHEXSTR("connection context \n",ccontext, sizeof(struct mudm_connection) ); 
  return;
}
