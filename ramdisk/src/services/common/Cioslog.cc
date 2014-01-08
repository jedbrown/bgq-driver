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
/* (C) Copyright IBM Corp.  2011, 2012                              */
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

//! \file  Cioslog.cc
//! \brief Declaration 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#include <ramdisk/include/services/MessageHeader.h>
#include <ramdisk/include/services/SysioMessages.h>
#include <ramdisk/include/services/JobctlMessages.h>
#include <ramdisk/include/services/StdioMessages.h>
#include <ramdisk/include/services/IosctlMessages.h>

#include <ramdisk/include/services/common/Cioslog.h>
#include <stdio.h>
#include <hwi/include/bqc/A2_inlines.h>

#include <infiniband/verbs.h>

using namespace bgcios;

static unsigned int FlightLogSize=16;

static BG_FlightRecorderLog_t dummy[16];

static BG_FlightRecorderLog_t * FlightLog = dummy;

void setFlightLogSize(unsigned int size){
  if (size==0) size=16;
  FlightLogSize=size;
  FlightLog = new BG_FlightRecorderLog_t[size];
}

const char * CIOS_FLIGHTLOG_FMT[] =
{
    "FL_INVALD", 
//using short form for function pointer assignment
#define STANDARD(name)  #name ,
#include <ramdisk/include/services/common/flightlog.h>
#undef STANDARD
  "FL_INLAST"
};
static volatile uint32_t total_entries = 0;
static uint32_t fl_index=0;
static uint32_t wrapped=0;
static int log_fd= 2;  //stdout default

#define LLUS long long unsigned int
void printMsg(uint32_t ID, bgcios::MessageHeader *mh){
   printf("%s: type=%u rank=%u jobid=%llu sequenceId=%d length=%d timestamp=%llu\n", CIOS_FLIGHTLOG_FMT[ID],(int)mh->type,(int)mh->rank,(LLUS)mh->jobId, (int)mh->sequenceId,(int)mh->length,(LLUS)GetTimeBase());
}
#undef LLUS 



uint32_t logMsg(uint32_t ID,bgcios::MessageHeader *mh){
   uint32_t index = fl_index++;
   BG_FlightRecorderLog_t * entry = &FlightLog[index];
   entry->entry_num = total_entries++;
   entry->id = ID;
   entry->timeStamp = GetTimeBase();
   uint64_t * msg = (uint64_t *)mh;
   entry->data[0] = msg[0];
   entry->data[1] = msg[1];
   entry->data[2] = msg[2];
   entry->data[3] = msg[3];
   entry->ci.other = 0;  //not using other...
   if (fl_index >= FlightLogSize){
     fl_index=0;
     wrapped=1;
   }
   return index;
}

uint32_t logMsgWC(uint32_t ID,bgcios::MessageHeader *mh,struct ibv_wc *wc){
   uint32_t index = fl_index++;
   BG_FlightRecorderLog_t * entry = &FlightLog[index];
   entry->entry_num = total_entries++;
   entry->id = ID;
   entry->timeStamp = GetTimeBase();
   uint64_t * msg = (uint64_t *)mh;
   entry->data[0] = msg[0];
   entry->data[1] = msg[1];
   entry->data[2] = msg[2];
   entry->data[3] = msg[3];
   entry->ci.BGV_recv[0] = //remote QP number unknown...
   entry->ci.BGV_recv[1] = wc->qp_num; //local QP number
   if (fl_index >= FlightLogSize){
     fl_index=0;
     wrapped=1;
   }
   return index;
}

uint32_t logMsgQpNum(uint32_t ID,bgcios::MessageHeader *mh,uint32_t qp_num){
   uint32_t index = fl_index++;
   BG_FlightRecorderLog_t * entry = &FlightLog[index];
   entry->entry_num = total_entries++;
   entry->id = ID;
   entry->timeStamp = GetTimeBase();
   uint64_t * msg = (uint64_t *)mh;
   entry->data[0] = msg[0];
   entry->data[1] = msg[1];
   entry->data[2] = msg[2];
   entry->data[3] = msg[3];
   entry->ci.BGV_recv[0] = 0; //remote QP number unknown...
   entry->ci.BGV_recv[1] = qp_num; //local QP number
   if (fl_index >= FlightLogSize){
     fl_index=0;
     wrapped=1;
   }
   return index;
}

size_t Flight_CIOS_MsgDecoder(size_t bufsize, char* buffer, const BG_FlightRecorderLog_t* log, bool doendl=true);

size_t Flight_CIOS_MsgDecoder(size_t bufsize, char* buffer, const BG_FlightRecorderLog_t* log, bool doendl)
{   


    bgcios::MessageHeader * mh = (bgcios::MessageHeader *)log->data;
    char * buffer_start = buffer;

    size_t length = (size_t)snprintf(buffer, bufsize, "entry=%8x timestamp=%16llx %s: ",log->entry_num,(long long unsigned int)log->timeStamp,CIOS_FLIGHTLOG_FMT[log->id]);
    buffer += length;
    bufsize -= length;

    length = (size_t)snprintf(buffer, bufsize, "svc=%ld type=%ld rank=%d seq=%ld msglen=%ld jobid=%lld",(long int)mh->service,(long int)mh->type,(int)mh->rank,(long int)mh->sequenceId,(long int)mh->length,(long long int)mh->jobId);    
    buffer += length;
    bufsize -= length;

    if (mh->errorCode){     
      switch (mh->errorCode){
       case ENOENT:
         length = (size_t)snprintf(buffer,bufsize,">ENOENT(%ld)",(long int)mh->errorCode); break;
       case EINTR:
         length = (size_t)snprintf(buffer,bufsize,">EINTR(%ld)",(long int)mh->errorCode); break;
       case ECONNREFUSED:
         length = (size_t)snprintf(buffer,bufsize,">ECONNREFUSED(%ld)",(long int)mh->errorCode); break;
       case EHOSTUNREACH:
         length = (size_t)snprintf(buffer,bufsize,">EHOSTUNREACH(%ld)",(long int)mh->errorCode); break;
       case EINVAL:
         length = (size_t)snprintf(buffer,bufsize,">EINVAL(%ld)",(long int)mh->errorCode); break;
       case ESPIPE:
         length = (size_t)snprintf(buffer,bufsize,">ESPIPE(%ld)",(long int)mh->errorCode); break;
       case EEXIST:
         length = (size_t)snprintf(buffer,bufsize,">EEXIST(%ld)",(long int)mh->errorCode); break;
       case ENOTCONN:
         length = (size_t)snprintf(buffer,bufsize,">ENOTCONN(%ld)",(long int)mh->errorCode); break;
       default:
         length = (size_t)snprintf(buffer,bufsize,">EC(%ld)",(long int)mh->errorCode);
      }
      buffer += length;
      bufsize -= length;
    }

    if (mh->returnCode){
      length = (size_t)snprintf(buffer,bufsize," rc=%ld",(long int)mh->returnCode);
      buffer += length;
      bufsize -= length;
    }

    char * text = NULL;
    switch(mh->type){
        case iosctl::ErrorAck: text=(char *)"iosctl::ErrorAck";break;
        case jobctl::ErrorAck: text=(char *)"jobctl::ErrorAck";break;

        case iosctl::Ready: text=(char *)"iosctl::Ready";break;
        case iosctl::StartNodeServices: text=(char *)"iosctl::StartNodeServices";break;
        case iosctl::Interrupt: text=(char *)"iosctl::Interrupt";break;

        case jobctl::LoadJob: text=(char *)"jobctl::LoadJob"; break;
        case jobctl::LoadJobAck: text=(char *)"jobctl::LoadJobAck"; break;
        case jobctl::StartJob: text=(char *)"jobctl::StartJob"; break;
        case jobctl::StartJobAck: text=(char *)"jobctl::StartJobAck"; break;
        case jobctl::ExitJob: text=(char *)"jobctl::ExitJob"; break;
        case jobctl::ExitJobAck: text=(char *)"jobctl::ExitJobAck"; break;
        case jobctl::ExitProcess: text=(char *)"jobctl::ExitProcess"; break;
        case jobctl::ExitProcessAck: text=(char *)"jobctl::ExitProcessAck"; break;

        case jobctl::SignalJob: text=(char *)"jobctl::SignalJob";break;
        case jobctl::SignalJobAck: text=(char *)"jobctl::SignalJobAck";break;
        case jobctl::CleanupJob: text=(char *)"jobctl::CleanupJob";break;
        case jobctl::CleanupJobAck: text=(char *)"jobctl::CleanupJobAck";break;
        case jobctl::Authenticate: text=(char *)"jobctl::Authenticate";break;
        case jobctl::AuthenticateAck: text=(char *)"jobctl::AuthenticateAck";break;

        case jobctl::Reconnect: text=(char *)"jobctl::Reconnect";break;
        case jobctl::ReconnectAck: text=(char *)"jobctl::ReconnectAck";break;

        case stdio::ErrorAck: text=(char *)"ErrorAck";break;
        case stdio::WriteStdout:text=(char *)"stdio::WriteStdout";break;
        case stdio::WriteStdoutAck: text=(char *)"stdio::WriteStdoutAck";break;
        case stdio::WriteStderr: text=(char *)"stdio::WriteStderr";break;
        case stdio::WriteStderrAck: text=(char *)"stdio::WriteStderrAck";break;
        case stdio::CloseStdio: text=(char *)"stdio::CloseStdio";break;
        case stdio::CloseStdioAck: text=(char *)"stdio::CloseStdioAck";break;


        case jobctl::DiscoverNode: text=(char *)"jobctl::DiscoverNode"; break;
        case jobctl::DiscoverNodeAck: text=(char *)"jobctl::DiscoverNodeAck";break;
        case jobctl::SetupJob: text=(char *)"jobctl::SetupJob"; break;
        case jobctl::SetupJobAck: text=(char *)"jobctl::SetupJobAck";break;
 
        case jobctl::Completed: text=(char *)"jobctl::Completed"; break;

        case sysio::WriteKernelInternal: text=(char *)"sysio::WriteKernelInternal";break;
        case sysio::OpenKernelInternal: text=(char *)"sysio::OpenKernelInternal";break;
        case sysio::CloseKernelInternal: text=(char *)"sysio::CloseKernelInternal";break;

        case sysio::SetupJob: text=(char *)"sysio::SetupJob";break;
        case sysio::SetupJobAck: text=(char *)"sysio::SetupJobAck";break;
        case sysio::CleanupJob: text=(char *)"sysio::CleanupJob";break;
        case sysio::CleanupJobAck: text=(char *)"sysio::CleanupJobAck";break;
        case sysio::Open: text=(char *)"sysio::Open";break;
        case sysio::OpenAck: text=(char *)"sysio::OpenAck";break;
        case sysio::Close: text=(char *)"sysio::Close";break;
        case sysio::CloseAck: text=(char *)"sysio::CloseAck";break;
        case sysio::Socket: text=(char *)"sysio::Socket";break;
        case sysio::SocketAck: text=(char *)"sysio::SocketAck";break;

        case sysio::Write: text=(char *)"sysio::Write";break;
        case sysio::WriteAck: text=(char *)"sysio::WriteAck";break;

        case sysio::Pread64: text=(char *)"sysio::Pread64";break;
        case sysio::Pread64Ack: text=(char *)"sysio::Pread64Ack";break;

        case sysio::Read: text=(char *)"sysio::Read";break;
        case sysio::ReadAck: text=(char *)"sysio::ReadAck";break;
        case sysio::Lseek64: text=(char *)"sysio::Lseek64";break;
        case sysio::Lseek64Ack: text=(char *)"sysio::Lseek64Ack";break;
        case sysio::Stat64: text=(char *)"sysio::Stat64";break;
        case sysio::Stat64Ack: text=(char *)"sysio::Stat64Ack";break;
        case sysio::Fstat64: text=(char *)"sysio::Fstat64";break;
        case sysio::Fstat64Ack: text=(char *)"sysio::Fstat64Ack";break;
        case sysio::Poll: text=(char *)"sysio::Poll";break;
        case sysio::PollAck: text=(char *)"sysio::PollAck";break;
        case sysio::Readlink: text=(char *)"sysio::Readlink";break;
        case sysio::ReadlinkAck: text=(char *)"sysio::ReadlinkAck";break;
        case sysio::Fcntl: text=(char *)"sysio::Fcntl";break;
        case sysio::FcntlAck: text=(char *)"sysio::FcntlAck";break;


        default:   break;
    }
    if (text){  
       length = (size_t)snprintf(buffer,bufsize," %s",text);
       buffer += length;
       bufsize -= length;      
    }
   
    if (log->id ==BGV_SEND_MSG){
        length = (size_t)snprintf(buffer, bufsize, " lQP=%d ",log->ci.BGV_recv[1] );
        buffer += length;
        bufsize -= length;    
   }
   else if (log->id ==BGV_RECV_MSG){
        length = (size_t)snprintf(buffer, bufsize, " lQP=%d ",log->ci.BGV_recv[1] );
        buffer += length;
        bufsize -= length;    
   }

   if (doendl){
      length = (size_t)snprintf(buffer,bufsize," \n");
       buffer += length;
       bufsize -= length;
   }
  
     return (size_t)strlen(buffer_start);
}


void printLogMsg(const char * pathname){
    const size_t BUFSIZE = 1024;
    char buffer[BUFSIZE];
    size_t numChars = 0;
    log_fd = open(pathname,O_TRUNC | O_CREAT | O_RDWR, S_IRWXU|S_IRWXG|S_IRWXO );
    if (log_fd == -1)log_fd = 2;
    //printf("fl_index=%d log_fd=%d\n",fl_index,log_fd);
    ssize_t rc=0;
    if (wrapped) {
       for (uint32_t i=fl_index;i<FlightLogSize;i++){
          //printMsg(i, (bgcios::MessageHeader *)FlightLog[i].data);
          numChars = Flight_CIOS_MsgDecoder(BUFSIZE, buffer, &FlightLog[i]);
          rc=write(log_fd,buffer,numChars);
          if (rc==0){
             printf("rc=%d errno=%d \n",(int)rc,errno);
             printf("numChars=%d %s",(int)numChars,buffer);
          }
       }
    }
    for (uint32_t i=0;i<fl_index;i++){
          //printMsg(FlightLog[i].id, (bgcios::MessageHeader *)FlightLog[i].data);
          numChars = Flight_CIOS_MsgDecoder(BUFSIZE, buffer, &FlightLog[i]);
          rc=write(log_fd,buffer,numChars);
          if (rc==0){
             printf("rc=%d errno=%d\n",(int)rc,errno);
             printf("numChars=%d %s",(int)numChars,buffer);
          }
    }
    if (log_fd != 2) close(log_fd);
}

void printLogEntry(uint32_t entry){
    const size_t BUFSIZE = 1024;
    char buffer[BUFSIZE];
    size_t numChars = 0;
    numChars = Flight_CIOS_MsgDecoder(BUFSIZE, buffer, &FlightLog[entry]);
    write(2,buffer,numChars);
}


size_t snprintfLogEntry(size_t bufsize, char* buffer,uint32_t entry ){
    size_t numChars = Flight_CIOS_MsgDecoder(bufsize, buffer, &FlightLog[entry],false);
    return numChars;
}

void printprevLogEntries(uint32_t entry,uint32_t quantity){
   if (quantity>FlightLogSize)quantity=FlightLogSize;
   if (entry>FlightLogSize) return;
   if (quantity==0) return;
   uint32_t spot = entry;
   if (entry >= quantity){
       spot=(entry+1)-quantity; 
   }
   else{
       spot = FlightLogSize - (quantity - (entry + 1));
   }
   for (uint32_t i = 0; i<quantity; i++){
      printLogEntry(spot);
      spot++;
      if (spot==FlightLogSize)spot=0;
   }
}

void printlastLogEntries(uint32_t quantity){
   uint32_t index = fl_index;
   if (index==0)index = FlightLogSize - 1;
   else index--;
   printprevLogEntries(index, quantity);
}
