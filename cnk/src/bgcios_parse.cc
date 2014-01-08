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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include <ramdisk/include/services/MessageHeader.h>
#include <ramdisk/include/services/SysioMessages.h>
#include <ramdisk/include/services/JobctlMessages.h>
#include <ramdisk/include/services/StdioMessages.h>


using namespace bgcios;
#define NUMPROCESSORS 68
uint64_t fopStart[NUMPROCESSORS];
uint64_t fopSeqID[NUMPROCESSORS];
uint64_t lastSysioSeqID;


#define LLUS long long unsigned int
void Flight_SysioMsgDecoder(size_t bufsize, char* buffer, const BG_FlightRecorderLog_t* log, void* ptr)
{   
    bgcios::MessageHeader * mh = (bgcios::MessageHeader *)log->data;

    size_t length = snprintf(buffer, bufsize, "svc=%ld type=%ld rank=%d seq=%ld msglen=%ld jobid=%lld",(long int)mh->service,(long int)mh->type,(int)mh->rank,(long int)mh->sequenceId,(long int)mh->length,(long long int)mh->jobId);    
    buffer += length;
    bufsize -= length;

    if (mh->errorCode){     
      switch (mh->errorCode){
       case ENOENT:
         length = snprintf(buffer,bufsize,">ENOENT(%ld)",(long int)mh->errorCode); break;
       case EINTR:
         length = snprintf(buffer,bufsize,">EINTR(%ld)",(long int)mh->errorCode); break;
       case ECONNREFUSED:
         length = snprintf(buffer,bufsize,">ECONNREFUSED(%ld)",(long int)mh->errorCode); break;
       case EHOSTUNREACH:
         length = snprintf(buffer,bufsize,">EHOSTUNREACH(%ld)",(long int)mh->errorCode); break;
       case EINVAL:
         length = snprintf(buffer,bufsize,">EINVAL(%ld)",(long int)mh->errorCode); break;
       case ESPIPE:
         length = snprintf(buffer,bufsize,">ESPIPE(%ld)",(long int)mh->errorCode); break;
       case EEXIST:
         length = snprintf(buffer,bufsize,">EEXIST(%ld)",(long int)mh->errorCode); break;
       case ENOTCONN:
         length = snprintf(buffer,bufsize,">ENOTCONN(%ld)",(long int)mh->errorCode); break;
       case ESTALE:
         length = snprintf(buffer,bufsize,">ESTALE(%ld)",(long int)mh->errorCode); break;
       case EBUSY:
         length = snprintf(buffer,bufsize,">EBUSY(%ld)",(long int)mh->errorCode); break;
       case EAGAIN:
         length = snprintf(buffer,bufsize,">EAGAIN(%ld)",(long int)mh->errorCode); break;
       case ENOSPC:
         length = snprintf(buffer,bufsize,">ENOSPC(%ld)",(long int)mh->errorCode); break;
       default:
         length = snprintf(buffer,bufsize,">EC(%ld)",(long int)mh->errorCode);
      }
      buffer += length;
      bufsize -= length;
    }

    if (mh->returnCode){
      length = snprintf(buffer,bufsize," rc=%d",(int)mh->returnCode);
      buffer += length;
      bufsize -= length;
    }

    char * text = NULL;
    if (SysioUserServiceFdRDMA == mh->service) text=(char *)"svc=SysioUserServiceFdRDMA";
    else if (SysioUserService == mh->service) text=(char *)"svc=SysioUserService";
    else
    switch(mh->type){
        case jobctl::ErrorAck: text=(char *)"jobctl::ErrorAck";break;

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
        case sysio::Recv: text=(char *)"sysio::Recv";break;
        case sysio::RecvAck: text=(char *)"sysio::RecvAck";break;
        case sysio::Send: text=(char *)"sysio::Send";break;
        case sysio::SendAck: text=(char *)"sysio::SendAck";break;

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
        case sysio::Unlink: text=(char *)"sysio::Unlink";break;
        case sysio::UnlinkAck: text=(char *)"sysio::UnlinkAck";break;
        case sysio::Link: text=(char *)"sysio::Link";break;
        case sysio::LinkAck: text=(char *)"sysio::LinkAck";break;
        case sysio::Symlink: text=(char *)"sysio::Symlink";break;
        case sysio::SymlinkAck: text=(char *)"sysio::SymlinkAck";break;

#define SYSIO(type) case sysio::type: text=(char *)"sysio::"#type;break; 
         SYSIO(FsetXattr);
         SYSIO(FsetXattrAck);
         SYSIO(FgetXattr);
         SYSIO(FgetXattrAck);
         SYSIO(FremoveXattr);
         SYSIO(FremoveXattrAck);
         SYSIO(FlistXattr);
         SYSIO(FlistXattrAck);

         SYSIO(LsetXattr);
         SYSIO(LsetXattrAck);
         SYSIO(LgetXattr);
         SYSIO(LgetXattrAck);
         SYSIO(LremoveXattr);
         SYSIO(LremoveXattrAck);
         SYSIO(LlistXattr);
         SYSIO(LlistXattrAck);

         SYSIO(PsetXattr);
         SYSIO(PsetXattrAck);
         SYSIO(PgetXattr);
         SYSIO(PgetXattrAck);
         SYSIO(PremoveXattr);
         SYSIO(PremoveXattrAck);
         SYSIO(PlistXattr);
         SYSIO(PlistXattrAck);


        default:   break;
    }
    if (text){  
       length = snprintf(buffer,bufsize," %s",text);
       buffer += length;
       bufsize -= length;      
    }

    if ( (log->id==FL_SYSMSGSND) && (log->hwthread < NUMPROCESSORS) ){
       fopStart[log->hwthread]=log->timestamp;
       fopSeqID[log->hwthread]=mh->sequenceId;
    }
    else if ( (log->id==FL_SYSMSGRCV) && (log->hwthread < NUMPROCESSORS )){
       if (mh->sequenceId==fopSeqID[log->hwthread]){
          uint64_t timeDiff = log->timestamp - fopStart[log->hwthread];
          length=snprintf(buffer,bufsize, " CYC=%lld ",(LLUS)timeDiff);
          buffer += length;
          bufsize -= length;
       }
    }
    
}


