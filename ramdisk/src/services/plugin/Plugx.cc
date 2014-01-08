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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

/*!
 * \file 
 * \brief
 */

#include "Plugx.h"
#include <ramdisk/include/services/UserMessages.h>
#include <unistd.h>
#include <stdio.h>
#include <firmware/include/personality.h>
#include <hwi/include/common/uci.h>
#include <ostream>
#include <iostream>
#include <fstream>
#include <iomanip>


//! Header describing a I/O service message.

void printMessageHeader(const bgcios::UserMessage& m)
{
  printf("Msg: service=%d version=%d type=%d rank=%d sequenceId=%d returnCode=%d errorCode=%d length=%d jobId=%lld \n",
             m.header.service, m.header.version, m.header.type, m.header.rank, m.header.sequenceId, m.header.returnCode, 
             m.header.errorCode, m.header.length, 
             (long long unsigned int)m.header.jobId);
}
using namespace std;
// This is used to dump the personality 
// We dump in a human readable form here since it is for debug.
//ostream & operator<<(ostream &os, const Personality_t& p)
ostream & operator<<(ostream &os, const Personality_t& p)
{
  os.fill('0');
  os << hex;
  os << "BGQ Personality dump --------" << endl;
  os << " CRC . ........... 0x" << setw(4) << p.CRC << endl;
  os << " Version ......... 0x" << setw(2) << (uint16_t) p.Version << endl;
  os << " Size (words) .... 0x" << setw(2) << (uint16_t) p.PersonalitySizeWords << endl;
  os << " ----------------------------" << endl;
  os << " Kernel Config" << endl;
  char loc[32];
  bg_uci_toString(p.Kernel_Config.UCI, loc);
  os << "  UCI ... ........ 0x" << setw(16) << p.Kernel_Config.UCI << "  (location: " << loc << ")" << endl;
  uint64_t nc = p.Kernel_Config.NodeConfig;
  os << "  Node Config .... 0x" << setw(16) << nc << endl;
  if (nc & PERS_ENABLE_MMU) os << "  \tMMU Enabled" << endl;
  if (nc & PERS_ENABLE_IsIoNode) os << "  \tIsIoNode Enabled" << endl;
  if (nc & PERS_ENABLE_TakeCPU) os << "  \tTakeCPU Enabled" << endl;
  if (nc & PERS_ENABLE_MU) os << "  \tMU Enabled" << endl;
  if (nc & PERS_ENABLE_ND) os << "  \tND Enabled" << endl;
  if (nc & PERS_ENABLE_Timestamps) os << "  \tTimestamps Enabled" << endl;
  if (nc & PERS_ENABLE_BeDRAM) os << "  \tBeDRAM Enabled" << endl;
  if (nc & PERS_ENABLE_ClockStop) os << "  \tClockStop Enabled" << endl;
  if (nc & PERS_ENABLE_DrArbiter) os << "  \tDrArbiter Enabled" << endl;
  if (nc & PERS_ENABLE_DevBus) os << "  \tDevBus Enabled" << endl;
  if (nc & PERS_ENABLE_L1P) os << "  \tL1P Enabled" << endl;
  if (nc & PERS_ENABLE_L2) os << "  \tL2 Enabled" << endl;
  if (nc & PERS_ENABLE_MSGC) os << "  \tMSGC Enabled" << endl;
  if (nc & PERS_ENABLE_TestInt) os << "  \tTestInt Enabled" << endl;
  if (nc & PERS_ENABLE_NodeRepro) os << "  \tNodeRepro Enabled" << endl;
  if (nc & PERS_ENABLE_PartitionRepro) os << "  \tPartitionRepro Enabled" << endl;
  if (nc & PERS_ENABLE_DD1_Workarounds) os << "  \tDD1_Workarounds Enabled" << endl;
  if (nc & PERS_ENABLE_A2_Errata) os << "  \tA2_Errata Enabled" << endl;
  if (nc & PERS_ENABLE_A2_IU_LLB) os << "  \tA2_IU_LLB Enabled" << endl;
  if (nc & PERS_ENABLE_A2_XU_LLB) os << "  \tA2_XU_LLB Enabled" << endl;
  if (nc & PERS_ENABLE_DDRCalibration) os << "  \tDDRCalibration Enabled" << endl;
  if (nc & PERS_ENABLE_DDRFastInit) os << "  \tDDRFastInit Enabled" << endl;
  if (nc & PERS_ENABLE_AppPreload) os << "  \tAppPreload Enabled" << endl;
  if (nc & PERS_ENABLE_IOServices) os << "  \tIOServices Enabled" << endl;
  if (nc & PERS_ENABLE_SpecCapDDR) os << "  \t_SpecCapDDR Enabled" << endl;
  if (nc & PERS_ENABLE_L2Only) os << "  \tL2Only Enabled" << endl;
  if (nc & PERS_ENABLE_FPGA) os << "  \tFPGA Enabled" << endl;
  if (nc & PERS_ENABLE_DDRINIT) os << "  \tDDRINIT Enabled" << endl;
  if (nc & PERS_ENABLE_Mailbox) os << "  \tMailbox Enabled" << endl;
  if (nc & PERS_ENABLE_Simulation) os << "  \tSimulation Enabled" << endl;
  if (nc & PERS_ENABLE_Mambo) os << "  \tMambo Enabled" << endl;
  if (nc & PERS_ENABLE_JTagConsole) os << "  \tJTagConsole Enabled" << endl;
  if (nc & PERS_ENABLE_JTagLoader) os << "  \tJTagLoader Enabled" << endl;
  if (nc & PERS_ENABLE_FPU) os << "  \tFPU Enabled" << endl;
  if (nc & PERS_ENABLE_L2Counters) os << "  \tL2Counters Enabled" << endl;
  if (nc & PERS_ENABLE_Wakeup) os << "  \tWakeup Enabled" << endl;
  if (nc & PERS_ENABLE_BIC) os << "  \tBIC Enabled" << endl;
  if (nc & PERS_ENABLE_DDR) os << "  \tDDR Enabled" << endl;
  if (nc & PERS_ENABLE_GlobalInts) os << "  \tGlobalInts Enabled" << endl;
  if (nc & PERS_ENABLE_SerDes) os << "  \tSerDes Enabled" << endl;
  if (nc & PERS_ENABLE_UPC) os << "  \tUPC Enabled" << endl;
  if (nc & PERS_ENABLE_EnvMon) os << "  \tEnvMon Enabled" << endl;
  if (nc & PERS_ENABLE_PCIe) os << "  \tPCIe Enabled" << endl;
  if (nc & PERS_ENABLE_DiagnosticsMode) os << "  \tDiagnosticsMode Enabled" << endl;
  os << "  Trace Config ... 0x" << setw(16) << p.Kernel_Config.TraceConfig << endl;
  os << "  RASPolicy ...... 0x" << setw(8) << p.Kernel_Config.RASPolicy << endl;
  os << "  Freq (MHz) ..... 0x" << setw(8) << p.Kernel_Config.FreqMHz << endl;
  os << "  Clock Stop ..... 0x" << setw(16) << p.Kernel_Config.ClockStop << endl;
  os << " ----------------------------" << endl;
  os << " DDR Config" << endl;
  os << "  DDR Flags ...... 0x" << setw(8) << p.DDR_Config.DDRFlags << endl;
  os << "  DDR Size MB .... 0x" << setw(8) << p.DDR_Config.DDRSizeMB << endl;
  os << " ----------------------------" << endl;
  os << " Network Config" << endl;
  os << "  Block ID ....... 0x" << setw(8) << p.Network_Config.BlockID << endl;
  uint64_t nf = p.Network_Config.NetFlags;
  os << "  NetFlags ....... 0x" << setw(16) << nf << endl;
  if (nf & ND_ENABLE_ION_LINK) os << "  \tND ION LINK Enabled" << endl;  
//  if (ND_GET_LOOPBACK(0,nf)) os << "  \tND DIM A LoopBack " << dec << ND_GET_LOOPBACK(0,nf) << endl;  
//  if (ND_GET_LOOPBACK(1,nf)) os << "  \tND DIM B LoopBack " << dec << ND_GET_LOOPBACK(1,nf) << endl;  
//  if (ND_GET_LOOPBACK(2,nf)) os << "  \tND DIM C LoopBack " << dec << ND_GET_LOOPBACK(2,nf) << endl;  
//  if (ND_GET_LOOPBACK(3,nf)) os << "  \tND DIM D LoopBack " << dec << ND_GET_LOOPBACK(3,nf) << endl;  
//  if (ND_GET_LOOPBACK(4,nf)) os << "  \tND DIM E LoopBack " << dec << ND_GET_LOOPBACK(4,nf) << endl;  
//  if (ND_GET_LOOPBACK(5,nf)) os << "  \tND DIM IO LoopBack " << dec << ND_GET_LOOPBACK(5,nf) << endl;  
  if (nf & ND_ENABLE_TORUS_DIM_A) os << "  \tTorus DIM A Enabled" << endl;  
  if (nf & ND_ENABLE_TORUS_DIM_B) os << "  \tTorus DIM B Enabled" << endl;  
  if (nf & ND_ENABLE_TORUS_DIM_C) os << "  \tTorus DIM C Enabled" << endl;  
  if (nf & ND_ENABLE_TORUS_DIM_D) os << "  \tTorus DIM D Enabled" << endl;  
  if (nf & ND_ENABLE_TORUS_DIM_E) os << "  \tTorus DIM E Enabled" << endl;  
  if (nf & ND_ENABLE_TORUS_RAN_HINTS_A) os << "  \tRAN HINTS A Enabled" << endl;  
  if (nf & ND_ENABLE_TORUS_RAN_HINTS_B) os << "  \tRAN HINTS B Enabled" << endl;  
  if (nf & ND_ENABLE_TORUS_RAN_HINTS_C) os << "  \tRAN HINTS C Enabled" << endl;  
  if (nf & ND_ENABLE_TORUS_RAN_HINTS_D) os << "  \tRAN HINTS D Enabled" << endl;  
  if (nf & ND_ENABLE_TORUS_RAN_HINTS_E) os << "  \tRAN HINTS E Enabled" << endl;  
  if (ND_SE_ARB_RE_INJ_RATIO(nf)) os << "  \tSE ARB RE INJ Ration " << dec << ND_SE_ARB_RE_INJ_RATIO(nf)<< endl;  
//  if (ND_GET_DET_ORDER_MASK(nf,0)) os << "  \tDET ORDER MASK 0 " << dec << ND_GET_DET_ORDER_MASK(nf,0) << endl;  
//  if (ND_GET_DET_ORDER_MASK(nf,1)) os << "  \tDET ORDER MASK 1 " << dec << ND_GET_DET_ORDER_MASK(nf,1) << endl;  
//  if (ND_GET_DET_ORDER_MASK(nf,2)) os << "  \tDET ORDER MASK 2 " << dec << ND_GET_DET_ORDER_MASK(nf,2) << endl;  
//  if (ND_GET_DET_ORDER_MASK(nf,3)) os << "  \tDET ORDER MASK 3 " << dec << ND_GET_DET_ORDER_MASK(nf,3) << endl;  
  uint64_t nf2 = p.Network_Config.NetFlags2;
  os << "  NetFlags2....... 0x" << setw(16) << nf2 << endl;
  if (nf2 & ND_ENABLE_LINK_A_MINUS) os << "  \tA MINUS LINK Enabled" << endl;
  if (nf2 & ND_ENABLE_LINK_A_PLUS) os << "  \tA PLUS LINK Enabled" << endl;
  if (nf2 & ND_ENABLE_LINK_B_MINUS) os << "  \tB MINUS LINK Enabled" << endl;
  if (nf2 & ND_ENABLE_LINK_B_PLUS) os << "  \tB PLUS LINK Enabled" << endl;
  if (nf2 & ND_ENABLE_LINK_C_MINUS) os << "  \tC MINUS LINK Enabled" << endl;
  if (nf2 & ND_ENABLE_LINK_C_PLUS) os << "  \tC PLUS LINK Enabled" << endl;
  if (nf2 & ND_ENABLE_LINK_D_MINUS) os << "  \tD MINUS LINK Enabled" << endl;
  if (nf2 & ND_ENABLE_LINK_D_PLUS) os << "  \tD PLUS LINK Enabled" << endl;
  if (nf2 & ND_ENABLE_LINK_E_MINUS) os << "  \tE MINUS LINK Enabled" << endl;
  if (nf2 & ND_ENABLE_LINK_E_PLUS) os << "  \tE PLUS LINK Enabled" << endl;
  if (nf2 & SD_ENABLE_T0) os << "  \tSerdes T0 LINK Enabled" << endl;
  if (nf2 & SD_ENABLE_T1) os << "  \tSerdes T1 LINK Enabled" << endl;
  if (nf2 & SD_ENABLE_T2) os << "  \tSerdes T2 LINK Enabled" << endl;
  if (nf2 & SD_ENABLE_T3) os << "  \tSerdes T3 LINK Enabled" << endl;
  if (nf2 & SD_ENABLE_T4) os << "  \tSerdes T4 LINK Enabled" << endl;
  if (nf2 & SD_ENABLE_T5) os << "  \tSerdes T5 LINK Enabled" << endl;
  if (nf2 & SD_ENABLE_T6) os << "  \tSerdes T6 LINK Enabled" << endl;
  if (nf2 & SD_ENABLE_T7) os << "  \tSerdes T7 LINK Enabled" << endl;
  if (nf2 & SD_ENABLE_T8) os << "  \tSerdes T8 LINK Enabled" << endl;
  if (nf2 & SD_ENABLE_T9) os << "  \tSerdes T9 LINK Enabled" << endl;
  if (nf2 & SD_ENABLE_TIO) os << "  \tSerdes TIO LINK Enabled" << endl;
  if (nf2 & SD_ENABLE_HALFSPEED) os << "  \tSerdes HALFSPEED Enabled" << endl;
  if (nf2 & TI_USE_PORT6_FOR_IO) os << "  \tTestint use Port 6  for IO" << endl;
  if (nf2 & TI_USE_PORT7_FOR_IO) os << "  \tTestint use Port 7  for IO" << endl;
  if (nf2 & TI_TORUS_DIM_A_REVERSED) os << "  \tTestint DIM A Reversed" << endl;
  if (nf2 & TI_TORUS_DIM_B_REVERSED) os << "  \tTestint DIM B Reversed" << endl;
  if (nf2 & TI_TORUS_DIM_C_REVERSED) os << "  \tTestint DIM C Reversed" << endl;
  if (nf2 & TI_TORUS_DIM_D_REVERSED) os << "  \tTestint DIM D Reversed" << endl;
  if (nf2 & TI_TORUS_DIM_E_REVERSED) os << "  \tTestint DIM E Reversed" << endl;
  if (nf2 & ND_CN_BRIDGE_PORT_6) os << "  \tCN Bridge Port 6" << endl;
  if (nf2 & ND_CN_BRIDGE_PORT_7) os << "  \tCN Bridge Port 7" << endl;
  if (nf2 & ND_CN_BRIDGE_PORT_10) os << "  \tCN Bridge Port 10" << endl;
  if (nf2 & SD_RESET) os << "  \tSerdes Reset" << endl;
  os << "  A Nodes ........ 0x" << setw(4) << (uint16_t)p.Network_Config.Anodes << endl;
  os << "  B Nodes ........ 0x" << setw(4) << (uint16_t)p.Network_Config.Bnodes << endl;
  os << "  C Nodes ........ 0x" << setw(4) << (uint16_t)p.Network_Config.Cnodes << endl;
  os << "  D Nodes ........ 0x" << setw(4) << (uint16_t)p.Network_Config.Dnodes << endl;
  os << "  E Nodes ........ 0x" << setw(4) << (uint16_t)p.Network_Config.Enodes << endl;
  os << "  A Coord ........ 0x" << setw(4) << (uint16_t)p.Network_Config.Acoord << endl;
  os << "  B Coord ........ 0x" << setw(4) << (uint16_t)p.Network_Config.Bcoord << endl;
  os << "  C Coord ........ 0x" << setw(4) << (uint16_t)p.Network_Config.Ccoord << endl;
  os << "  D Coord ........ 0x" << setw(4) << (uint16_t)p.Network_Config.Dcoord << endl;
  os << "  E Coord ........ 0x" << setw(4) << (uint16_t)p.Network_Config.Ecoord << endl;
  os << "  Classroute - GInterrupt Inputs .. 0x" << setw(4) << p.Network_Config.PrimordialClassRoute.GlobIntUpPortInputs << endl;
  os << "  Classroute - GInterrupt Outputs . 0x" << setw(4) << p.Network_Config.PrimordialClassRoute.GlobIntUpPortOutputs << endl;
  os << "  Classroute - Collective Inputs .. 0x" << setw(4) << p.Network_Config.PrimordialClassRoute.CollectiveTypeAndUpPortInputs << endl;
  os << "  Classroute - Collective Outputs . 0x" << setw(4) << p.Network_Config.PrimordialClassRoute.CollectiveUpPortOutputs << endl;

  os << " ----------------------------" << endl;
  os << " Ethernet Config" << endl;
  os << "  Sec Key ........... ";
  os << "0x";
  for (unsigned i = 0; i < PERSONALITY_LEN_SECKEY; ++i)
    os << std::setfill('0') << std::setw(2) << std::hex << static_cast<unsigned>(p.Ethernet_Config.SecurityKey[i]);
  os << endl;

  os << dec;
  os.fill(' ');
  return os;
}
 


namespace bgcios {
namespace sysio {

void Plugx::message(const bgcios::UserMessage& receivedMsg, struct MessageHeader& replyMsg)
{
      //printMessageHeader(receivedMsg);
      if (receivedMsg.header.version != PLUGIN_VERSION)
      {
    	  setErrno(replyMsg, (uint32_t)receivedMsg.header.version, PLUGIN_ERROR_INVALID_VERSION);
    	  return;
      }
      if (receivedMsg.header.type == PLUGIN_OP_TYPE_MSG_ECHO ){
       // uint32_t setReplyHeader(const bgcios::UserMessage& receivedMsg, struct MessageHeader& replyMsg, uint16_t typeForReply,uint32_t length4Reply=32)
        setReplyHeader(receivedMsg, replyMsg, PLUGIN_OP_TYPE_MSG_ECHO_REPLY);

        long unsigned int dataLength = receivedMsg.header.length - sizeof(MessageHeader);
        char *data = (char *)receivedMsg.MessageData;
        UserMessage *uMsg;
        uMsg = (UserMessage *)(&replyMsg);
        char *returnData = ((char *)uMsg) + sizeof(MessageHeader);
        //echo data back to cnk application
        memcpy(returnData,data,dataLength);
        setReplyLength(replyMsg, (uint32_t)(sizeof(MessageHeader) + dataLength) );
        setErrno(replyMsg, 0, 0);
      }
      else if (receivedMsg.header.type == PLUGIN_OP_TYPE_MSG_GETHOSTNAME ){
        // uint32_t setReplyHeader(const bgcios::UserMessage& receivedMsg, struct MessageHeader& replyMsg, uint16_t typeForReply,uint32_t length4Reply=32)
        setReplyHeader(receivedMsg, replyMsg, PLUGIN_OP_TYPE_MSG_GETHOSTNAME_REPLY);
        UserMessage *uMsg = (UserMessage *)(&replyMsg);
        //getHostName
        int rc = gethostname(uMsg->MessageData,UserMessageDataSize);
        //printf("rc=%d MessageData=%s\n",rc,uMsg->MessageData);
        if (rc>=0){
          setReplyLength(replyMsg, (uint32_t)(sizeof(MessageHeader) + strlen(uMsg->MessageData) ) );
          setErrno(replyMsg, 0, 0);
        }
        else {
          setReplyLength(replyMsg, (uint32_t)sizeof(MessageHeader) );
          setErrno(replyMsg, (uint32_t)rc, (uint32_t)errno);

        }
      }
      else
      {
    	  setErrno(replyMsg, (uint32_t)receivedMsg.header.type, PLUGIN_ERROR_INVALID_OP1);
      }
      //printMessageHeader(receivedMsg);
      return;
}

void Plugx::messageFdRDMA(const bgcios::UserMessageFdRDMA& receivedMsg, struct MessageHeader& replyMsg, void * RDMAbuffer, uint64_t size)
{
    //printf("fd0=%d fd1=%d \n", receivedMsg.ionode_fd[0],receivedMsg.ionode_fd[1]);
    setReplyHeader(receivedMsg, replyMsg,receivedMsg.header.type);
    setErrno(replyMsg, 0, 0);
    if (receivedMsg.header.version != PLUGIN_VERSION)
    {
  	  setErrno(replyMsg, (uint32_t)receivedMsg.header.version, PLUGIN_ERROR_INVALID_VERSION2);
          //turn off doing any RDMA operations after returning to caller
          setRDMArequestOnMsg(receivedMsg, RDMAdone);
  	  return;
    }
    if (receivedMsg.header.type == PLUGIN_OP_TYPE_WRITE_FILE)
    {   
       setReplyHeader(receivedMsg, replyMsg, PLUGIN_OP_TYPE_WRITE_FILE_REPLY);
       //From the IO node, doing a read of the CNK memory when returning to caller
       //printf("target file=%s \n",receivedMsg.MessageData);
       setRDMArequestOnMsg(receivedMsg, RDMAread);
    }
    if (receivedMsg.header.type == PLUGIN_OP_TYPE_READ_FILE)
    {   
       setReplyHeader(receivedMsg, replyMsg, PLUGIN_OP_TYPE_READ_FILE_REPLY);

       uint64_t totalXfer = 0;
       for (int i=0; i<receivedMsg.numberOfRdmaRegions;i++){
          totalXfer += receivedMsg.uRDMA[i].cnk_bytes;
       }
       bgcios::UserMessageRDMAxferBytes * reply = (bgcios::UserMessageRDMAxferBytes *)&replyMsg;
       reply->bytesXfer=0;
       setReplyLength(replyMsg, sizeof(bgcios::UserMessageRDMAxferBytes ) );

       if (totalXfer > size)totalXfer=size;
       ssize_t rc= ::read(receivedMsg.ionode_fd[1],RDMAbuffer,totalXfer);

       //printf("rc=%d fd=%d \n", (int)rc,receivedMsg.ionode_fd[1]);
       if (rc <= 0){//an error in reading the file
          setErrno(replyMsg, (uint32_t)errno, PLUGIN_ERROR_READ_FILE2_ERRNO);
          setRDMArequestOnMsg(receivedMsg, RDMAdone); //No RDMA to be done, feed back error
          return;
       }
       else {
         reply->bytesXfer=(uint64_t)rc;     
       }
       //From the IO node, doing a read of the CNK memory when returning to caller
       
       setRDMArequestOnMsg(receivedMsg, RDMAwrite);
       
    }
    return;
}


void Plugx::finishFdRDMA(const bgcios::UserMessageFdRDMA& receivedMsg, struct MessageHeader& replyMsg, void * RDMAbuffer, uint64_t size)
{
    //after the calling code finished doing any RDMA operations, finish any post RDMA processing

    errno=(int)size; //get rid of compiler message that size is unused
    if (receivedMsg.header.type == PLUGIN_OP_TYPE_WRITE_FILE)
    {    
       bgcios::UserMessageRDMAxferBytes * reply = (bgcios::UserMessageRDMAxferBytes *)&replyMsg;
       reply->bytesXfer=0;
       setReplyLength(replyMsg, sizeof(bgcios::UserMessageRDMAxferBytes ) );

       if ( getRDMAerrno() ){//was there an RDMA error?
          setErrno(replyMsg,getRDMAerrno(),PLUGIN_ERROR_READ_RDMA_ERROR);
          return;
       }
       errno = 0;
       ssize_t rc = ::write(receivedMsg.ionode_fd[0], RDMAbuffer, size);          
       if (rc < 0){
          setErrno(replyMsg, (uint32_t)errno, PLUGIN_ERROR_WRITE_FILE1_ERRNO);
          return;
       }
       else{                
          reply->bytesXfer=(uint64_t)rc;
       }
       
       //char buff[1024];
       //getcwd(buff,1024);
       //printf("target file=%s cwd=%s\n",receivedMsg.MessageData,buff);
       //pfile.open() will require fully qualified path name since cwd is simulated in CNK
       const Personality_t * p = (const Personality_t *)RDMAbuffer;
       fstream pfile;
       pfile.open(receivedMsg.MessageData,fstream::in | fstream::out | fstream::app);
       if (pfile.is_open()){}
       else printf("not open errno=%d\n",errno);
       pfile<<*p;
       pfile.close();

       setErrno(replyMsg, 0, 0);
       
    }
    if (receivedMsg.header.type == PLUGIN_OP_TYPE_READ_FILE){
       setErrno(replyMsg, 0, 0);
      
    }
    return;
}

}} // End namespaces

extern "C" {


bgcios::sysio::Plugin* create()
{
  return new bgcios::sysio::Plugx();
}


void destroy(bgcios::sysio::Plugin* plugin)
{
  delete((bgcios::sysio::Plugx*)plugin);
}

}/*end extern C */
