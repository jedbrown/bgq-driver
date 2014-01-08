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

#include "mudm_trace.h"
#include "mudm_macro.h"
#include "mudm_ras.h"
#include "common.h"

#ifdef __LINUX__
#define K_ATOMIC 
#else
#include <cnk/include/klibtypes.h>
#endif

#define ENABLE_MUDM_FLIGHTLOG_DEBUG 1

char * opcodes[] = {"and","or","xor","invalid","unsigned add","unsigned min","unsigned max", "invalid","signed add","signed min",
                    "signed max", "invalid","floating point add","floating point min","floating point max","invalid"};


uint64_t FlightLock_hiwrap K_ATOMIC;//data traffic at high wrap rate
uint64_t FlightLock_nowrap K_ATOMIC;//non-wrapping for static processing
uint64_t FlightLock_debug K_ATOMIC; //debug buffer for special processing, NOT REGISTERED 

BG_FlightRecorderLog_t FlightLog_nowrap[FlightLogSize_nowrap];
BG_FlightRecorderLog_t FlightLog_hiwrap[FlightLogSize_hiwrap];
BG_FlightRecorderLog_t FlightLog_debug[FlightLogSize_debug];

BG_FlightRecorderRegistry_t mudm_debug_flight_recorder __attribute__ ((aligned (64)));


const char FORMAT4byHex16[]="MUDM raw: %016llx %016llx %016llx %016llx";

BG_FlightRecorderFormatter_t MUDM_FLIGHTLOG_FMT[] =
{
    { "FL_INVALD", FLIGHTRECORDER_PRINTF, "Invalid flight recorder entry", NULL },
//using short form for function pointer assignment
#define STANDARD(name) { #name, FLIGHTRECORDER_PRINTF, FORMAT4byHex16, MUDM_formatFunction,NULL },
#define FLIGHTPRINT(name,format) { #name, FLIGHTRECORDER_PRINTF, format, MUDM_formatFunction,NULL },
#define FLIGHTFUNCT(name,function) { #name, FLIGHTRECORDER_FUNC, FORMAT4byHex16, function,NULL },
#include "flightlog.h"
#undef STANDARD
#undef FLIGHTPRINT
#undef FLIGHTFUNCT
};

void complete_init_mudm_flightlog_fmt(void * anchor){
  int i;
  for (i=0; i<FL_NUMENTRIES;i++){
    MUDM_FLIGHTLOG_FMT[i].anchorPtr=anchor;
#if 0
    MUDM_FLIGHTLOG_FMT[i].type = FLIGHTRECORDER_FUNC;
#endif 
  }
}

const char MUDM_HIWRAP[]="hiwrap";
void fillin_FlightLogegistyEntry_hiwrap(BG_FlightRecorderRegistry_t * bgfreg){
    bgfreg->flightlock = &FlightLock_hiwrap;
    bgfreg->flightlog  = (void *)FlightLog_hiwrap;
    bgfreg->flightsize = FlightLogSize_hiwrap;
    bgfreg->flightformatter = MUDM_FLIGHTLOG_FMT;
    bgfreg->num_ids=FL_NUMENTRIES;
    bgfreg->lastStateSet = 0;
    bgfreg->lastState = 0;
    bgfreg->lastStateTotal=0;
    bgfreg->registryName=MUDM_HIWRAP;
    //MPRINT("HIWRAP bgfreg->flightlog=%p bgfreg->flightsize=%llu \n",bgfreg->flightlog,(LLUS)bgfreg->flightsize);
};

const char MUDM_NOWRAP[]="lowrap";
void fillin_FlightLogegistyEntry_nowrap(BG_FlightRecorderRegistry_t * bgfreg){
    bgfreg->flightlock = &FlightLock_nowrap;
    bgfreg->flightlog  = (void *)FlightLog_nowrap;
    bgfreg->flightsize = FlightLogSize_nowrap;
    bgfreg->flightformatter = MUDM_FLIGHTLOG_FMT;
    bgfreg->num_ids=FL_NUMENTRIES;
    bgfreg->lastStateSet = 0;
    bgfreg->lastState = 0;
    bgfreg->lastStateTotal=0;
    bgfreg->registryName=MUDM_NOWRAP;
};

const char MUDM_FLDEBUG[]="DEBUG";
void fillin_FlightLogegistyEntry_debug(BG_FlightRecorderRegistry_t * bgfreg){
    bgfreg->flightlock = &FlightLock_debug;
    bgfreg->flightlog  = (void *)FlightLog_debug;
    bgfreg->flightsize =  FlightLogSize_debug;
    bgfreg->flightformatter = MUDM_FLIGHTLOG_FMT;
    bgfreg->num_ids=FL_NUMENTRIES;
    bgfreg->lastStateSet = 0;
    bgfreg->lastState = 0;
    bgfreg->lastStateTotal=0;
    bgfreg->registryName=MUDM_FLDEBUG;
};


int parse_node2string(char * target,int target_length,uint32_t nodeAddr){
  int length;
  MUHWI_Destination_t na;
  if (nodeAddr == (uint32_t)(-1) ){ return (length = snprintf(target, target_length, 
       "(unknown)" ) );}
  na.Destination.Destination = nodeAddr;
  length = snprintf(target, target_length, 
       "(%d %d %d %d %d)",
        na.Destination.A_Destination,na.Destination.B_Destination,na.Destination.C_Destination,na.Destination.D_Destination,na.Destination.E_Destination);
  return length;
};


#define LUS long unsigned int 

void MUDM_parse_connect_state(size_t bufferSize, char* buffer, const BG_FlightRecorderLog_t* logentry, void* extraDataPtr){
  int length;
  struct mudm_connection cm;
  char remote_node[32];
  char bridge_node[32];
       
  memcpy((void*)&cm.state,logentry->data,32);

  length = snprintf(buffer, bufferSize, "MUDM Connect State=%d my_index=%d remote_rec_FIFO_Id=%x local_rec_FIFO_Id=%x remote_BlockID=%d ",
                    cm.state, cm.my_index, cm.remote_rec_FIFO_Id, cm.local_rec_FIFO_Id, cm.remote_BlockID);
  bufferSize -= length;
  buffer += length;
  
   parse_node2string(remote_node,sizeof(remote_node),cm.destination.Destination.Destination);
   parse_node2string(bridge_node,sizeof(bridge_node),cm.remote_bridgingCNnode.Destination.Destination);

   length=snprintf(buffer, bufferSize," destination=%s  remote_bridgingCNnode=%s",remote_node,bridge_node);
   bufferSize -= length;
   buffer += length;

   switch(cm.io_port){
       break;
       case MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_TORUS_PORT_6:
       length = snprintf(buffer, bufferSize," io_port=Port6 ");
       break;
       case  MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_TORUS_PORT_7:
       length = snprintf(buffer, bufferSize," io_port=Port7 ");
       break;
       case MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_IO_PORT:
       length = snprintf(buffer, bufferSize," io_port=Port10");  
       break;     
       default: length=0;break;
     }
     bufferSize -= length;
     buffer += length;


//  uint16_t dest_conn_index;
//  uint8_t  io_port;                        /* if IO node, use an IO port to CN */ 
// uint8_t  reverse_io_port;                /* if IO node, use an IO port to CN */ 
//  uint8_t  direction;                      /* IONODE2CN, CN2ION, CN2CN, ION2ION */
//  uint8_t  reverse_direction;              /* IONODE2CN, CN2ION, CN2CN, ION2ION */
//  uint16_t lowbit_timestamp;               /* for key matching on disconnect    */

#if ENABLE_MUDM_FLIGHTLOG_DEBUG
     length = snprintf(buffer, bufferSize, " raw: %016llx %016llx %016llx %016llx", (LLUS)logentry->data[0], (LLUS)logentry->data[1],(LLUS)logentry->data[2], (LLUS)logentry->data[3]);
     bufferSize -= length;
     buffer += length;
#endif     
     return;
}

void MUDM_parseInjNetHdr(size_t bufferSize, char* buffer, const BG_FlightRecorderLog_t* logentry, void* extraDataPtr)
{
    const MUHWI_Descriptor_t* desc = (const MUHWI_Descriptor_t*)logentry->data;
    
    snprintf(buffer, bufferSize, "MUDM inject network header from PhysicalAddr=0x%llx for %ld bytes.  FIFO map=0x%llx  %s",
             (unsigned long long)desc->Pa_Payload,
             (unsigned long)desc->Message_Length,
             (unsigned long long)desc->Torus_FIFO_Map,
             (desc->Half_Word1.Interrupt)?"Interrupt_Packet":"Non-Interrupt_Packet");
}

void MUDM_parseC0BCASTRDMA(size_t bufferSize, char* buffer, const BG_FlightRecorderLog_t* logentry, void* extraDataPtr)
{
//    FLIGHTPRINT(C0_BCASTRDMA, "MUDM bcast origin_bcast_node=%llx, class_route=%llu,local_status_address=%llx, local_status_value=%llu")
    char nodeString[32];
    parse_node2string(nodeString, sizeof(nodeString), logentry->data[0]);
    
    snprintf(buffer, bufferSize, "MUDM bcast from node %s class_route=%llu, local_status_address=0x%llx, local_status_value=%llu",
             nodeString,
             (unsigned long long)logentry->data[1],
             (unsigned long long)logentry->data[2],
             (unsigned long long)logentry->data[3]
        );
}

void MUDM_parsePK(size_t bufferSize, char* buffer, const BG_FlightRecorderLog_t* logentry, void* extraDataPtr){
     const char payl[]="userLength=";
     const char dkey[]="disc-key=";
     const char seqn[]=    "sequence#=";
     const char dput_sqn[]="rget_seq#=";
     const char cidx[]="context-index=";
     const char type_error[]="typeRejected=";
          
     const char * start = seqn;
     
     const char * b4type = payl;

     const char ion[]="IO";
     const char cn[] ="CN";
     const char * sourcenodetype =cn;
     const char * destnodetype=cn;
     char MU_dest_node[32];
     MUHWI_PacketHeader_t * hdr = (MUHWI_PacketHeader_t *)logentry->data;
     /* Set pointer to the network header portion of the packet header.  */
     MUHWI_Pt2PtNetworkHeader_t *pkth = &(hdr->NetworkHeader.pt2pt);

     SoftwareBytes_t *sw_hdr = (SoftwareBytes_t *)(&hdr->messageUnitHeader.Packet_Types.Memory_FIFO.Put_Offset_LSB);
     size_t length = 0;
     char * typeString;
     parse_node2string(MU_dest_node,sizeof(MU_dest_node),pkth->Destination.Destination.Destination);

     length = snprintf(buffer, bufferSize, "MUDM inject MU packet header: ");
     bufferSize -= length;
     buffer += length;
     
     if (pkth->Byte8.Packet_Type==0)
     {
       char user_source_node[32];
       char user_dest_node[32];
       
       parse_node2string(user_source_node,sizeof(user_source_node),sw_hdr->ionet_hdr.torus_source_node);
       parse_node2string(user_dest_node,sizeof(user_dest_node),sw_hdr->ionet_hdr.torus_dest_node);
       
       if (sw_hdr->ionet_hdr.torus_dest_node & 0x80000000) destnodetype = ion;
       if (sw_hdr->ionet_hdr.torus_source_node & 0x80000000) sourcenodetype = ion;
       switch(sw_hdr->ionet_hdr.type)
       {   

       case MUDM_DPUT:           typeString = "MUDM_DPUT           "; start=dput_sqn; break;
       case MUDM_PKT_DATA_IMM:   typeString = "MUDM_PKT_DATA_IMM   "; break;           
       case MUDM_PKT_DATA_REMOTE:typeString = "MUDM_PKT_DATA_REMOTE"; break;
       case MUDM_IP_IMMED :      typeString = "MUDM_IP_IMMED       "; break;
       case MUDM_IP_REMOTE:      typeString = "MUDM_IP_REMOTE      "; break;
       case MUDM_CONN_REQUEST :  typeString = "MUDM_CONN_REQUEST   "; start=NULL; break;
       case MUDM_CONN_REPLY :    typeString = "MUDM_CONN_REPLY     "; start=NULL; break;
       case MUDM_DISCONNECTED:   typeString = "MUDM_DISCONNECTED   "; start=cidx; b4type=dkey; break;
       case MUDM_CNK2CNK_FIFO:   typeString = "MUDM_CNK2CNK_FIFO   "; break;
       case MUDM_RDMA_READ:      typeString = "MUDM_RDMA_READ      "; break;
       case MUDM_RDMA_WRITE:     typeString = "MUDM_RDMA_WRITE     "; break;
       case MUDM_PKT_ERROR:      typeString = "MUDM_PKT_ERROR      "; b4type=type_error; break;
       case MUDM_PKT_ENOTCONN:   typeString = "MUDM_PKT_ENOTCONN   "; b4type=type_error; break;
       case MUDM_PKT_EILSEQ :    typeString = "MUDM_PKT_EILSEQ     "; b4type=type_error; break;
       default:                  typeString = "!!UNKNOWN"; break;
       }

       if (start){
           length = snprintf(buffer, bufferSize, " %s %s%s==>%s%s %s%d %s%d",typeString,sourcenodetype,user_source_node,destnodetype,user_dest_node,start,sw_hdr->ionet_hdr.sequence_number, b4type,sw_hdr->ionet_hdr.payload_length);
       }
       else {
         length = snprintf(buffer, bufferSize, " %s %s%s==>%s%s %s0x%x",typeString,sourcenodetype,user_source_node,destnodetype,user_dest_node, b4type,sw_hdr->ionet_hdr.payload_length);
       }

       bufferSize -= length;
       buffer += length;

       length=snprintf(buffer, bufferSize," Rec_FIFO_Id=%d", hdr->messageUnitHeader.Packet_Types.Memory_FIFO.Rec_FIFO_Id);
       bufferSize -= length;
       buffer += length;

       if(pkth->Byte2.Interrupt)
           length=snprintf(buffer, bufferSize," Interrupt_Packet");
       else
           length=snprintf(buffer, bufferSize," Non-Interrupt_Packet");
       bufferSize -= length;
       buffer += length;
     }
     else if (pkth->Byte8.Packet_Type==1)
     {
       uint64_t temp_targ_phy =  hdr->messageUnitHeader.Packet_Types.Memory_FIFO.Put_Offset_MSB;
       temp_targ_phy = (temp_targ_phy<<32) + hdr->messageUnitHeader.Packet_Types.Memory_FIFO.Put_Offset_LSB;
       length = snprintf(buffer, bufferSize, " >>RDMA DIRECTPUT counter=%llx",(LLUS)(logentry->data[3]&0xFFFFFFFF8) );//36-bits
       bufferSize -= length;
       buffer += length;
       //! \todo index the atomic op string
       if (temp_targ_phy & 1000000000){
         length = snprintf(buffer, bufferSize, " dest@=%llx dest-atomic-op=%llx",(LLUS)(temp_targ_phy&0xFFFFFFFF8) ,(LLUS)(temp_targ_phy & 0x7) );
         bufferSize -= length;
         buffer += length;
       }
       else{
         length = snprintf(buffer, bufferSize, " dest@=%llx ",(LLUS)temp_targ_phy);
         bufferSize -= length;
         buffer += length;
       }
     }
     else {
       length = snprintf(buffer, bufferSize, " !!Unexpected Packet Type");
       bufferSize -= length;
       buffer += length;

     }
     switch(pkth->Byte2.Return_From_IO_Node){
       break;
       case 1:
       length = snprintf(buffer, bufferSize," Port6 ");
       break;
       case 2:
       length = snprintf(buffer, bufferSize," Port7 ");
       break;
       case 3:
       length = snprintf(buffer, bufferSize," Port10");  
       break;     
       default: length=0;break;
     }
       bufferSize -= length;
       buffer += length;


       length = snprintf(buffer, bufferSize," VC=%d  MUPKT=%s ",pkth->Byte3.Virtual_channel,MU_dest_node);
       bufferSize -= length;
       buffer += length;

       switch(sw_hdr->ionet_hdr.type)
       {   

       case MUDM_PKT_DATA_IMM:             
       case MUDM_PKT_DATA_REMOTE:       
       case MUDM_CONN_REPLY :    
       case MUDM_DISCONNECTED:   
       case MUDM_RDMA_READ:      
       case MUDM_RDMA_WRITE:  
         length = snprintf(buffer, bufferSize," sQP=%d dQP=%d",sw_hdr->ionet_hdr.source_qpn,sw_hdr->ionet_hdr.dest_qpn);   
         bufferSize -= length;
         buffer += length;   
         break;
       case MUDM_CONN_REQUEST :
         length = snprintf(buffer, bufferSize," sQP=%d",sw_hdr->ionet_hdr.source_qpn);   
         bufferSize -= length;
         buffer += length;   
         break;  
       default:  break;
       }

#if ENABLE_MUDM_FLIGHTLOG_DEBUG
       length = snprintf(buffer, bufferSize, " raw: %016llx %016llx %016llx %016llx", (LLUS)logentry->data[0], (LLUS)logentry->data[1],(LLUS)logentry->data[2], (LLUS)logentry->data[3]);
       bufferSize -= length;
       buffer += length;  
#endif
};

void MUDM_parsePKbcast(size_t bufferSize, char* buffer, const BG_FlightRecorderLog_t* logentry, void* extraDataPtr){
     
     const char payl[]="userLength=";
     const char seqn[]=    "sequence#=";
          
     const char * start = seqn;
     
     const char * b4type = payl;
     
     char MU_dest_node[32];
     MUHWI_PacketHeader_t * hdr = (MUHWI_PacketHeader_t *)logentry->data;
     /* Set pointer to the network header portion of the packet header.  */
     MUHWI_Pt2PtNetworkHeader_t *pkth = &(hdr->NetworkHeader.pt2pt);
     MUHWI_CollectiveNetworkHeader_t *chdr = &(hdr->NetworkHeader.collective);
     int word_length = 1<<(chdr->Byte1.Word_Length + 2);

     SoftwareBytes_t *sw_hdr = (SoftwareBytes_t *)(&hdr->messageUnitHeader.Packet_Types.Memory_FIFO.Put_Offset_LSB);
     size_t length = 0;
     char * typeString;
     parse_node2string(MU_dest_node,sizeof(MU_dest_node),pkth->Destination.Destination.Destination);

     length = snprintf(buffer, bufferSize, "MUDM bcast packet: ");
     bufferSize -= length;
     buffer += length;
     
     switch(sw_hdr->ionet_hdr.type)
     {   

       case MUDM_RDY_RDMA_BCAST: typeString = "MUDM_RDY_RDMA_BCAST "; break;
       case MUDM_CLR_RDMA_BCAST: typeString = "MUDM_CLR_RDMA_BCAST "; break;    
       case MUDM_REDUCE_ONE:     typeString = "MUDM_REDUCE_ONE     "; break;   
       case MUDM_REDUCE_ALL:     typeString = "MUDM_REDUCE_ALL     "; break;   
       case MUDM_REDUCE_ALL_INT: typeString = "MUDM_REDUCE_ALL_INT "; break;   
       case MUDM_REDUCE_BCAST_ORIGIN: typeString = "MUDM_REDUCE_BCAST_ORIGIN "; break;          
       case MUDM_REDUCE_BCAST_FINISH: typeString = "MUDM_REDUCE_BCAST_FINISH "; break;   

       default:                  typeString = "!!UNKNOWN"; start=NULL; break;
      }
      

      if (start){
         length = snprintf(buffer, bufferSize, " %s  %s%d %s%d", typeString,start,sw_hdr->ionet_hdr.sequence_number, b4type,sw_hdr->ionet_hdr.payload_length);
      }
      else {
          length = snprintf(buffer, bufferSize, " %s  %s0x%x", typeString, b4type,sw_hdr->ionet_hdr.payload_length);
      }

       bufferSize -= length;
       buffer += length;

       
       if (chdr->Data_Packet_Type ==MUHWI_COLLECTIVE_DATA_PACKET_TYPE){
         length=snprintf(buffer, bufferSize," Class_Route=%d", chdr->Byte2.Class_Route);
         bufferSize -= length;
         buffer += length;
       }

       length=snprintf(buffer, bufferSize," Rec_FIFO_Id=%d", hdr->messageUnitHeader.Packet_Types.Memory_FIFO.Rec_FIFO_Id);
       bufferSize -= length;
       buffer += length;
   

     switch(pkth->Byte2.Return_From_IO_Node){
       break;
       case 1:
       length = snprintf(buffer, bufferSize," Port6 ");
       break;
       case 2:
       length = snprintf(buffer, bufferSize," Port7 ");
       break;
       case 3:
       length = snprintf(buffer, bufferSize," Port10");  
       break;     
       default: length=0;break;
     }
     bufferSize -= length;
     buffer += length;

     length = snprintf(buffer, bufferSize," VC=%d MUPKT=%s ",pkth->Byte3.Virtual_channel,MU_dest_node);
     bufferSize -= length;
     buffer += length;
    
     if(chdr->Byte2.Interrupt)
         length=snprintf(buffer, bufferSize," Interrupt_Packet");
     else
         length=snprintf(buffer, bufferSize," Non-Interrupt_Packet");

     bufferSize -= length;
     buffer += length;

     switch(chdr->Byte3.Collective_Type){
       case 2: length=snprintf(buffer, bufferSize," AllReduce opCode=%0x(%s) wordLength=%d VC=%d ",chdr->Byte1.OpCode,opcodes[chdr->Byte1.OpCode],word_length,pkth->Byte3.Virtual_channel);
       break;
       case 3: length=snprintf(buffer, bufferSize," Reduce opCode=%0x(%s) wordLength=%d VC=%d MUPKT=>%s ",chdr->Byte1.OpCode,opcodes[chdr->Byte1.OpCode],word_length,pkth->Byte3.Virtual_channel,MU_dest_node);
       break;     
       default: length=snprintf(buffer, bufferSize," VC=%d ",pkth->Byte3.Virtual_channel);
       break;
     }

#if ENABLE_MUDM_FLIGHTLOG_DEBUG
      length = snprintf(buffer, bufferSize, " raw: %016llx %016llx %016llx %016llx", (LLUS)logentry->data[0], (LLUS)logentry->data[1],(LLUS)logentry->data[2], (LLUS)logentry->data[3]);
     bufferSize -= length;
     buffer += length;
#endif
};

void MUDM_parseDirectPut(size_t bufferSize, char* buffer, const BG_FlightRecorderLog_t* logentry, void* extraDataPtr){
     uint64_t temp_targ_phy = 0;
     char MU_dest_node[32];
     MUHWI_PacketHeader_t * hdr = (MUHWI_PacketHeader_t *)logentry->data;
     /* Set pointer to the network header portion of the packet header.  */
     MUHWI_Pt2PtNetworkHeader_t *pkth = &(hdr->NetworkHeader.pt2pt);
     MUHWI_CollectiveNetworkHeader_t *chdr = &(hdr->NetworkHeader.collective);
     MUHWI_MessageUnitHeader_t * mhdr =   &(hdr->messageUnitHeader);

     size_t length = 0;

     parse_node2string(MU_dest_node,sizeof(MU_dest_node),pkth->Destination.Destination.Destination);

     length = snprintf(buffer, bufferSize, "MUDM directput packet: ");
     bufferSize -= length;
     buffer += length;
     
     if (chdr->Data_Packet_Type ==MUHWI_COLLECTIVE_DATA_PACKET_TYPE){
       length=snprintf(buffer, bufferSize," Class_Route=%d", chdr->Byte2.Class_Route);
       bufferSize -= length;
       buffer += length;
     }
     switch(pkth->Byte2.Return_From_IO_Node){
       break;
       case 1:
       length = snprintf(buffer, bufferSize," Port6 ");
       break;
       case 2:
       length = snprintf(buffer, bufferSize," Port7 ");
       break;
       case 3:
       length = snprintf(buffer, bufferSize," Port10");  
       break;     
       default: length=0;break;
     }
     bufferSize -= length;
     buffer += length;

       length = snprintf(buffer, bufferSize," VC=%d MUPKT=>%s ",pkth->Byte3.Virtual_channel,MU_dest_node);
       bufferSize -= length;
       buffer += length;
  

       length=snprintf(buffer, bufferSize," >>dest_batId=%d",mhdr->Packet_Types.Direct_Put.Rec_Payload_Base_Address_Id);
       bufferSize -= length;
       buffer += length;
       length=snprintf(buffer, bufferSize," Counter_batId=%d",mhdr->Packet_Types.Direct_Put.Rec_Counter_Base_Address_Id);
       bufferSize -= length;
       buffer += length;       
       
       temp_targ_phy =  hdr->messageUnitHeader.Packet_Types.Memory_FIFO.Put_Offset_MSB;
       temp_targ_phy = (temp_targ_phy<<32) + hdr->messageUnitHeader.Packet_Types.Memory_FIFO.Put_Offset_LSB;
       length = snprintf(buffer, bufferSize, " dest@=%llx",(LLUS)(temp_targ_phy&0xFFFFFFFF8)  );
       if (temp_targ_phy & 0x1000000000){
         length = snprintf(buffer, bufferSize, " dest@=%llx dest-atomic-op=%0llx",(LLUS)(temp_targ_phy&0xFFFFFFFF8),(LLUS)(temp_targ_phy & 0x7) );
         bufferSize -= length;
         buffer += length;
       }
       else{
         length = snprintf(buffer, bufferSize, " dest@=%llx",(LLUS)(temp_targ_phy&0xFFFFFFFFF)  );
         bufferSize -= length;
         buffer += length;       
       }

       length = snprintf(buffer, bufferSize, " >>counter@=%llx",(LLUS)(logentry->data[3]&0xFFFFFFFF8) );//36-bits
       bufferSize -= length;
       buffer += length;  

       length = snprintf(buffer, bufferSize, " counterType=%llx",(LLUS)(logentry->data[3]&0x07) );
       bufferSize -= length;
       buffer += length;   
       
#if ENABLE_MUDM_FLIGHTLOG_DEBUG
        length = snprintf(buffer, bufferSize, " raw: %016llx %016llx %016llx %016llx", (LLUS)logentry->data[0], (LLUS)logentry->data[1],(LLUS)logentry->data[2], (LLUS)logentry->data[3]);
       bufferSize -= length;
       buffer += length;
#endif
};

void MUDM_formatFunction(size_t bufferSize, char* buffer, const BG_FlightRecorderLog_t* logentry, void* extraDataPtr){
     // temporary 
     size_t length = 0;
     length = snprintf(buffer, bufferSize, MUDM_FLIGHTLOG_FMT[logentry->id].formatString, logentry->data[0], logentry->data[1],logentry->data[2], logentry->data[3]);
     bufferSize -= length;
     buffer += length;
};

int dump_flight_entry(BG_FlightRecorderRegistry_t* logregistry, uint64_t entry_num){
   char scratch[1024];
   char * buffer=scratch;
   size_t bufferSize=sizeof(scratch); 
   size_t length = 0;
   const BG_FlightRecorderLog_t* logentry = &logregistry->flightlog[entry_num];
   BG_FlightRecorderFormatter_t* fmt = &logregistry->flightformatter[logentry->id];
   length = snprintf(buffer, bufferSize, "TB=%016lx %s:%-2d ", (LUS)logentry->timestamp, fmt->id_str, logentry->hwthread);   
   bufferSize -= length;
   buffer     += length;
   length = snprintf(buffer, bufferSize, "%6ld(%s)",(LUS)entry_num, logregistry->registryName); 
   bufferSize -= length;
   buffer     += length;
   //MHERE;
   switch(fmt->type)
            {
                case FLIGHTRECORDER_PRINTF:
                    length = snprintf(buffer, bufferSize, fmt->formatString, logentry->data[0], logentry->data[1], logentry->data[2], logentry->data[3]);
                    bufferSize -= length;
                    buffer += length;
                    break;
                case FLIGHTRECORDER_FUNC:
                    fmt->formatFunction(bufferSize, buffer, logentry, fmt->anchorPtr);
                    length = strlen(buffer);
                    bufferSize -= length;
                    buffer     += length;
                    break;
                default:
                    MPRINT("fmt->type for flight recorder is invalid \n");
                    return -EINVAL;
                    break;
            }
    MPRINT("%s\n",scratch);
   return 0;
}


void dump_flightlog_leadup(BG_FlightRecorderRegistry_t* logregistry, uint64_t entry_num,uint64_t num2dump){
   uint64_t i; 
   uint64_t num_dumped=0;
   uint64_t logsize = logregistry->flightsize;
   MPRINT(" \n \n");
   if (entry_num >= (num2dump -1)){
    for (i=(entry_num - num2dump); i<=entry_num;i++){
     dump_flight_entry(logregistry, i);
     num_dumped++;
    }
   }
   else{
    for (i=logsize-(num2dump-(entry_num + 1) ); i<logsize;i++){
     dump_flight_entry(logregistry, i);
     num_dumped++;
    }
    for (i=0; i<=entry_num;i++){
     dump_flight_entry(logregistry, i);
     num_dumped++;
    }
   }
return;
}

void MUDM_personality_info(size_t bufferSize, char* buffer, const BG_FlightRecorderLog_t* logentry, void* extraDataPtr){

}

void MUDM_receiveFifoInfo(size_t bufferSize, char* buffer, const BG_FlightRecorderLog_t* logentry, void* extraDataPtr){
   MUHWI_Fifo_t *  hwfifo = (MUHWI_Fifo_t *)logentry->data[0];    /**< Virtual address pointer to MU SRAM for this fifo */ 
   void * va_start    =  (void *)logentry->data[1];  
  
   //uint64_t groupID  =  logentry->data[3];
   //MUDM_DB_RECF_INFO(&mcontext->mudm_no_wrap_flight_recorder,0, SYS_GROUPID, init_subgroup_id[i],init_sys_rec_fifo_id[i]);   
   uint64_t length = snprintf(buffer, bufferSize, "hwfifo=%p  va_start=%p subgroupID=%llu rec_fifo_id=%llu ",hwfifo,va_start,(LLUS)logentry->data[2],(LLUS)logentry->data[3]);
   bufferSize -= length;
   buffer += length;

   if (hwfifo)
   {
     length = snprintf(buffer, bufferSize, " hwfifo: pa_start=%llx, size=%llx, pa_head=%llx pa_tail=%llx",(LLUS)hwfifo->pa_start,(LLUS)(hwfifo->size_1+1),(LLUS)hwfifo->pa_head,(LLUS)hwfifo->pa_tail);
     bufferSize -= length;
     buffer += length;
   }
 
     return;
}

void MUDM_injectFifoInfo(size_t bufferSize, char* buffer, const BG_FlightRecorderLog_t* logentry, void* extraDataPtr){
   MUHWI_Fifo_t *  hwfifo = (MUHWI_Fifo_t *)logentry->data[0];    /**< Virtual address pointer to MU SRAM for this fifo */ 
   MUHWI_InjFifo_t * hw_injfifo= (MUHWI_InjFifo_t *)logentry->data[1];
   void * va_start    =  (void *)logentry->data[2];  


   uint64_t length = snprintf(buffer, bufferSize, "hwfifo=%p hw_injfifo=%p va_start=%p init_subgroup_id[i]=%llu ",hwfifo,hw_injfifo,va_start,(LLUS)logentry->data[3]);
   bufferSize -= length;
   buffer += length;

   if (hwfifo)
   {
     length = snprintf(buffer, bufferSize, " hwfifo: pa_start=%llx, size=0x%llx, pa_head=0x%llx pa_tail=0x%llx",(LLUS)hwfifo->pa_start,(LLUS)(hwfifo->size_1+1),(LLUS)hwfifo->pa_head,(LLUS)hwfifo->pa_tail);
     bufferSize -= length;
     buffer += length;
   }
   
   if (hw_injfifo)
   {
     length = snprintf(buffer, bufferSize, " hw_injfifo: descCount=%llu freeSpace=%llu ",(LLUS)hw_injfifo->descCount,(LLUS)hw_injfifo->freeSpace);
     bufferSize -= length;
     buffer += length;
   }  
 
     return;
}



uint64_t log_injfifo_info (MUDM_InjFifo_t   * injfifo_ctls, BG_FlightRecorderRegistry_t * logreg, uint64_t postRAS){
 MUSPI_InjFifo_t * ififo = injfifo_ctls->injfifo;
 MUHWI_InjFifo_t * hw_ififo = ififo->hw_injfifo;//additional injfifo in SRAM, descCount and freeSpace
 MUSPI_Fifo_t    * fifo = &ififo->_fifo;//Fifo shadow structure
 //MUHWI_Fifo_t      *  hwfifo = fifo->hwfifo;   /**< Virtual address pointer to MU SRAM for this fifo */
 uint64_t paHead = fifo->hwfifo->pa_head;
 uint64_t descCount = hw_ififo->descCount;
 uint64_t freeSpace = hw_ififo->freeSpace;
 void * vaHead = fifo->va_start + (paHead-fifo->pa_start);
 MUHWI_Descriptor_t  * descPtr = (MUHWI_Descriptor_t  *)vaHead;
 MUHWI_Destination_t na = descPtr->PacketHeader.NetworkHeader.pt2pt.Destination;
 uint64_t paTail = fifo->hwfifo->pa_tail;
 uint64_t numBytes = fifo->size_1 + 1;
 uint64_t numDescriptors = numBytes/64;
 uint64_t  numDescriptorsUndone = (paTail-paHead)/64;
 if (paTail < paHead){//tail wrapped around
   numDescriptorsUndone = numDescriptors - (paHead - paTail)/64;
 }

if (paHead!=paTail)
{
  //MDUMPHEXSTR("injFifo hw headptr->descriptor",vaHead,64);
  DB_INJECT_FIFO_POINTERS(logreg,fifo->va_start,fifo->va_end,vaHead,fifo->va_tail);
  DB_INJECT_FIFO_NUMBERS(logreg,descCount,freeSpace,numDescriptors,numDescriptorsUndone);
  DB_DESCR_HEADER_STUCK(logreg,vaHead);
  DB_INJECT_DESCR_STUCK(logreg,vaHead + 32);

  if (postRAS){  

    uint64_t * bytes8 = (uint64_t *)vaHead;
    int i = 0;
    MUDM_RASBEGIN(8);
      for(i=0;i<8;i++) MUDM_RASPUSH(bytes8[i]);
    MUDM_RASFINAL(MUDMRAS_STUCK_INJ_DESCRIPTOR);
      
    //MPRINT("compute node destination = %d %d %d %d %d \n",  na.Destination.A_Destination,na.Destination.B_Destination,na.Destination.C_Destination,na.Destination.D_Destination,na.Destination.E_Destination);
   MUDM_RASBEGIN(10);
    MUDM_RASPUSH((uint64_t)fifo->va_start)  MUDM_RASPUSH((uint64_t)fifo->va_end) MUDM_RASPUSH((uint64_t)vaHead) MUDM_RASPUSH((uint64_t)fifo->va_tail) MUDM_RASPUSH((uint64_t)numDescriptorsUndone) 
    MUDM_RASPUSH(na.Destination.A_Destination)
    MUDM_RASPUSH(na.Destination.B_Destination)
    MUDM_RASPUSH(na.Destination.C_Destination)
    MUDM_RASPUSH(na.Destination.D_Destination)
    MUDM_RASPUSH(na.Destination.E_Destination)
   MUDM_RASFINAL(postRAS);  
  }
}

return numDescriptorsUndone;
};
