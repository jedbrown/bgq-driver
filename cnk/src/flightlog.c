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
#include "Kernel.h"
#include "network/cnverbs.h"
#include "firmware/include/mailbox.h"
#include <fcntl.h>
#define MAX_FLIGHTRECORDERS 8

uint64_t flightlogregistrycount = 0;
BG_FlightRecorderRegistry_t flightlogregistry[MAX_FLIGHTRECORDERS];

uint64_t FlightLock K_ATOMIC;
BG_FlightRecorderLog_t FlightLog[FlightLogSize];
uint64_t FlightLock_high K_ATOMIC;
BG_FlightRecorderLog_t FlightLog_high[FlightLogSize_high];

BG_FlightRecorderFormatter_t FLIGHTLOG_FMT[] =
{
    { "FL_INVALD", FLIGHTRECORDER_PRINTF, "Invalid flight recorder entry", NULL },
#define FLIGHTPRINT(name, format) { #name, FLIGHTRECORDER_PRINTF, format, NULL, NULL},
#define FLIGHTFUNCT(name, function) { #name, FLIGHTRECORDER_FUNC, NULL, function, NULL},
#include "flightlog.h"
};

static void FLIGHTBUILDREGISTRY(BG_FlightRecorderRegistry_t* reg, 
                                BG_FlightRecorderFormatter_t* formatter, 
                                const char* name,
                                uint64_t* loglock, BG_FlightRecorderLog_t* logdata, uint64_t logsize)
{
    reg->flightlock      = loglock;
    reg->flightlog       = logdata;
    reg->flightsize      = logsize;
    reg->flightformatter = formatter;
    reg->num_ids         = FL_NUMENTRIES;
    reg->registryName    = name;
    reg->lastStateSet    =0;
    reg->lastState       =0;
    reg->lastStateTotal  =0;
}

int addFlightRecorder(BG_FlightRecorderRegistry_t* reg)
{
    if(flightlogregistrycount >= MAX_FLIGHTRECORDERS)
        return -1;
    memcpy(&flightlogregistry[flightlogregistrycount++], reg, sizeof(BG_FlightRecorderRegistry_t));
    return 0;
}

int registerFlightRecorder()
{
    if(flightlogregistrycount >= MAX_FLIGHTRECORDERS)
        return -1;
    FLIGHTBUILDREGISTRY(&flightlogregistry[flightlogregistrycount++], FLIGHTLOG_FMT, "CNK events", FLIGHTLOG);
    FLIGHTBUILDREGISTRY(&flightlogregistry[flightlogregistrycount++], FLIGHTLOG_FMT, "CNK major events", FLIGHTLOG_high);
    return 0;
}

int getFlightRecorder(size_t bufferSize, char* buffer)
{
    uint64_t x;
    uint64_t more = 0;
    uint64_t numlogs = flightlogregistrycount;
    BG_FlightRecorderRegistry_t flightlog[MAX_FLIGHTRECORDERS];
    memcpy(flightlog, flightlogregistry, sizeof(flightlog));
    if(numlogs == 0)
        return 0;
    
    for(x=0; x<numlogs; x++)
    {
        flightlog[x].lastStateSet   = 0;
        flightlog[x].lastState      = 0;
        flightlog[x].lastStateTotal = 0;
    }

    Kernel_DecodeFlightLogs(numlogs, flightlog, bufferSize, buffer, &more);
    return 0;
}

Lock_Atomic_t FlightLogBufferLock;
char FlightLogBuffer[16384-sizeof(MailBoxHeader_t)-256];   // size of mailbox minus mailbox header

int dumpFlightRecorder()
{
    uint64_t x;
    uint64_t more     = 0;
    
    if(flightlogregistrycount == 0)
        return 0;

    Kernel_Lock(&FlightLogBufferLock);
    ppc_msync();
    
    for(x=0; x<flightlogregistrycount; x++)
    {
        flightlogregistry[x].lastStateSet   = 0;
        flightlogregistry[x].lastState      = 0;
        flightlogregistry[x].lastStateTotal = 0;
    }
    
    do
    {
        Kernel_DecodeFlightLogs(flightlogregistrycount, flightlogregistry, sizeof(FlightLogBuffer), FlightLogBuffer, &more);
        if(NodeState.TraceConfig & TRACE_FlightLog)
        {
            NodeState.FW_Interface.putn(FlightLogBuffer, strlen(FlightLogBuffer)+1);
        }
    }
    while(more);
    Kernel_Unlock(&FlightLogBufferLock);    
    
    return 0;
}


void Flight_SyscallDecoder(size_t bufsize, char* buffer, const BG_FlightRecorderLog_t* log, void* ptr)
{
    uint64_t syscallnum = 0;
    const char* syscall_name = NULL;
    if(0) { }
#define SYSCALL(name, spec) else if(syscallnum++ == log->data[0]) { syscall_name = #name; }
#include "syscalls/stdsyscalls.h"
    else
    {
        syscallnum = SYSCALL_START;
        if(0) {}
#define SYSCALL(name) else if(syscallnum++ == log->data[0]) { syscall_name = #name; }    
#include "cnk/include/SPI_syscalls.h"
    }
    if(syscall_name == NULL) { syscall_name = "UNDEFINED"; }
    
    snprintf(buffer, bufsize, "Syscall %-4ld at  IP=0x%016lx    LR=0x%016lx    SP=0x%016lx (%s)", log->data[0], log->data[1], log->data[2], log->data[3], syscall_name);
}

void Flight_SyscallReturnDecoder(size_t bufsize, char* buffer, const BG_FlightRecorderLog_t* log, void* ptr)
{
    int length;
    uint64_t flags = log->data[1];
    length = snprintf(buffer, bufsize, "Syscall Return GPR3=0x%016lx%s", log->data[0], (flags?" with pendingFlags=":""));
    buffer  += length;
    bufsize -= length;
    
#define PENDINGFLAG(name) if((length > 0) && (flags & name)) { length = snprintf(buffer, bufsize, "%s ", #name); flags ^= name; bufsize -= length; buffer += length; }
    PENDINGFLAG(KTHR_PENDING_SNAPSHOT);
    PENDINGFLAG(KTHR_PENDING_MIGRATE);
    PENDINGFLAG(KTHR_PENDING_YIELD);
    PENDINGFLAG(KTHR_PENDING_SIGNAL);
    PENDINGFLAG(KTHR_PENDING_CLONE);
    PENDINGFLAG(KTHR_PENDING_SIGRETURN);
    PENDINGFLAG(KTHR_PENDING_NVGPR);
    if((flags) && (length > 0))
    {
        length = snprintf(buffer, bufsize, "Unreg(0x%lx) ", flags);
    }
}


void Flight_SchedDispatchDecoder(size_t bufsize, char* buffer, const BG_FlightRecorderLog_t* log, void* ptr)
{
    uint64_t arg1 = log->data[0];
    uint64_t arg2 = log->data[1];
    uint64_t arg3 = log->data[2];
    uint64_t arg4 = log->data[3];
    snprintf(buffer, bufsize, "Sched Disp  HWT=%ld TID=%ld SLOT=%ld IP=0x%016lx THRDS=%ld RUNNABLE=%ld ORDER=%010lx PENDING=0x%02lx PhysPID=%ld", 
             arg3 & 0xFF,                               // ProcessorID of the hardware thread 
             arg1 & 0xFFFF,                             // Thread ID of thread being dispatched
             (arg1>>16) & 0xFF,                         // Scheduler slot index being dispatched
             arg2,                                      // Instruction pointer of the location to begin execution
             (arg3>>SPRG_SPIinfo_NumThds)&0xFF,         // Number of software threads currently active on this hardware thread
             (arg3>>SPRG_SPIinfo_Runnable)&0xFF,        // Number of runnable software threads
             (arg4>>24),                                // Dispatch ordering data
             arg4&0xFF,                                 // Pending condition (located in lower byte)
             (arg1>>32) & 0xFFFF                        // physical pid 
             );
}   

void Flight_SchedBlockRemoteDecoder(size_t bufsize, char* buffer, const BG_FlightRecorderLog_t* log, void* ptr)
{
    uint64_t arg1 = log->data[0];
    uint64_t arg2 = log->data[1];
    uint64_t arg3 = log->data[2];
    uint64_t arg4 = log->data[3];
    snprintf(buffer, bufsize, "Sched Block-Remote  HWT=%ld TID from=%ld to=%ld IP=0x%016lx THRDS=%ld RUNNABLE=%ld REASON=%s%s%s%s%s%s%s%s%s%s", 
             arg3 & 0xFF,                               // ProcessorID of the hardware thread
             arg1/1000,                                 // The TID of the requesting software thread
             arg1%1000,                                 // The TID of the target software thread
             arg2,                                      // Instruction pointer of the requesting software thread
             (arg3>>SPRG_SPIinfo_NumThds)&0xFF,         // Number of software threads currently active on this hardware thread
             (arg3>>SPRG_SPIinfo_Runnable)&0xFF,        // Number of runnable software threads
             (arg4 & SCHED_STATE_FREE    ? "FREE "  : ""),// Requestor thread is setting the FREE block code in the target thread 
             (arg4 & SCHED_STATE_SLEEP   ? "SLEEP " : ""),// Requestor thread is setting the SLEEP block code in the target thread
             (arg4 & SCHED_STATE_FUTEX   ? "FUTEX " : ""),// Requestor thread is setting the FUTEX block code in the target thread
             (arg4 & SCHED_STATE_FLOCK   ? "FLOCK " : ""),// Requestor thread is setting the FLOCK block code in the target thread
             (arg4 & SCHED_STATE_RESET   ? "RESET " : ""),// Requestor thread is setting the RESET block code in the target thread
             (arg4 & SCHED_STATE_HOLD    ? "HOLD "  : ""),// Requestor thread is setting the HOLD block code in the target thread
             (arg4 & SCHED_STATE_SUSPEND ? "SUSP "  : ""),// Requestor thread is setting the SUSPEND block code in the target thread 
             (arg4 & SCHED_STATE_POOF    ? "POOF  " : ""),// Requestor thread is setting the POOF block code in the target thread
             (arg4 & SCHED_STATE_RESERVED ? "RESERVED" : ""),// Setting the RESERVED block code 
             (arg4 & SCHED_STATE_APPEXIT ? "APPEXIT " : "") // Setting the APP EXIT block code 
             );
}

void Flight_SchedBlockDecoder(size_t bufsize, char* buffer, const BG_FlightRecorderLog_t* log, void* ptr)
{
    uint64_t arg1 = log->data[0];
    uint64_t arg2 = log->data[1];
    uint64_t arg3 = log->data[2];
    uint64_t arg4 = log->data[3];
    snprintf(buffer, bufsize, "Sched Block  HWT=%ld TID=%ld IP=0x%016lx THRDS=%ld RUNNABLE=%ld REASON=%s%s%s%s%s%s%s%s%s%s", 
             arg3 & 0xFF,                               // ProcessorID of the hardware thread
             arg1,                                      // The TID being blocked
             arg2,                                      // Instruction pointer of the thread being blocked
             (arg3>>SPRG_SPIinfo_NumThds)&0xFF,         // Number of software threads currently active on this hardware thread
             (arg3>>SPRG_SPIinfo_Runnable)&0xFF,        // Number of runnable software threads
             (arg4 & SCHED_STATE_FREE    ? "FREE  " : ""),// Setting the FREE block code  
             (arg4 & SCHED_STATE_SLEEP   ? "SLEEP " : ""),// Setting the SLEEP block code
             (arg4 & SCHED_STATE_FUTEX   ? "FUTEX " : ""),// Setting the FUTEX block code
             (arg4 & SCHED_STATE_FLOCK   ? "FLOCK " : ""),// Setting the FLOCK block code
             (arg4 & SCHED_STATE_RESET   ? "RESET " : ""),// Setting the RESET block code
             (arg4 & SCHED_STATE_HOLD    ? "HOLD  " : ""),// Setting the HOLD block code 
             (arg4 & SCHED_STATE_SUSPEND ? "SUSP  " : ""),// Setting the SUSPEND block code 
             (arg4 & SCHED_STATE_POOF    ? "POOF  " : ""),// Setting the POOF block code 
             (arg4 & SCHED_STATE_RESERVED ? "RESERVED" : ""),// Setting the RESERVED block code 
             (arg4 & SCHED_STATE_APPEXIT ? "APPEXIT " : "") // Setting the APP EXIT block code 
             );
}

void Flight_SchedUnBlockRemoteDecoder(size_t bufsize, char* buffer, const BG_FlightRecorderLog_t* log, void* ptr)
{
    uint64_t arg1 = log->data[0];
    uint64_t arg2 = log->data[1];
    uint64_t arg3 = log->data[2];
    uint64_t arg4 = log->data[3];
    snprintf(buffer, bufsize, "Sched Unblock-Remote  HWT=%ld TID from=%ld to=%ld IP=0x%016lx THRDS=%ld RUNNABLE=%ld REASON=%s%s%s%s%s%s%s%s%s%s", 
             arg3 & 0xFF,                               // ProcessorID of the hardware thread
             arg1/1000,                                 // The TID of the requesting software thread
             arg1%1000,                                 // The TID of the target software thread
             arg2,                                      // Instruction pointer of the requesting software thread
             (arg3>>SPRG_SPIinfo_NumThds)&0xFF,         // Number of software threads currently active on this hardware thread
             (arg3>>SPRG_SPIinfo_Runnable)&0xFF,        // Number of runnable software threads
             (arg4 & SCHED_STATE_FREE    ? "FREE "  : ""),// Requestor thread is resetting the FREE block code in the target thread 
             (arg4 & SCHED_STATE_SLEEP   ? "SLEEP " : ""),// Requestor thread is resetting the SLEEP block code in the target thread
             (arg4 & SCHED_STATE_FUTEX   ? "FUTEX " : ""),// Requestor thread is resetting the FUTEX block code in the target thread
             (arg4 & SCHED_STATE_FLOCK   ? "FLOCK " : ""),// Requestor thread is resetting the FLOCK block code in the target thread
             (arg4 & SCHED_STATE_RESET   ? "RESET " : ""),// Requestor thread is resetting the RESET block code in the target thread
             (arg4 & SCHED_STATE_HOLD    ? "HOLD "  : ""),// Requestor thread is resetting the HOLD block code in the target thread
             (arg4 & SCHED_STATE_SUSPEND ? "SUSP "  : ""),// Requestor thread is resetting the SUSPEND block code in the target thread 
             (arg4 & SCHED_STATE_POOF    ? "POOF  " : ""), // Requestor thread is resetting the POOF block code in the target thread
             (arg4 & SCHED_STATE_RESERVED ? "RESERVED" : ""),// Requestor thread is resetting the RESERVED block code 
             (arg4 & SCHED_STATE_APPEXIT ? "APPEXIT " : "") // Requestor thread is resetting the APP EXIT block code 
             );
}

void Flight_SchedUnBlockDecoder(size_t bufsize, char* buffer, const BG_FlightRecorderLog_t* log, void* ptr)
{
    uint64_t arg1 = log->data[0];
    uint64_t arg2 = log->data[1];
    uint64_t arg3 = log->data[2];
    uint64_t arg4 = log->data[3];
    snprintf(buffer, bufsize, "Sched Unblock  HWT=%ld TID=%ld IP=0x%016lx THRDS=%ld RUNNABLE=%ld REASON=%s%s%s%s%s%s%s%s%s%s", 
             arg3 & 0xFF,                               // ProcessorID of the hardware thread
             arg1,                                      // The TID being blocked
             arg2,                                      // Instruction pointer of the thread being blocked
             (arg3>>SPRG_SPIinfo_NumThds)&0xFF,         // Number of software threads currently active on this hardware thread
             (arg3>>SPRG_SPIinfo_Runnable)&0xFF,        // Number of runnable software threads
             (arg4 & SCHED_STATE_FREE    ? "FREE "  : ""),// Resetting the FREE block code  
             (arg4 & SCHED_STATE_SLEEP   ? "SLEEP " : ""),// Resetting the SLEEP block code
             (arg4 & SCHED_STATE_FUTEX   ? "FUTEX " : ""),// Resetting the FUTEX block code
             (arg4 & SCHED_STATE_FLOCK   ? "FLOCK " : ""),// Resetting the FLOCK block code
             (arg4 & SCHED_STATE_RESET   ? "RESET " : ""),// Resetting the RESET block code
             (arg4 & SCHED_STATE_HOLD    ? "HOLD "  : ""),// Resetting the HOLD block code 
             (arg4 & SCHED_STATE_SUSPEND ? "SUSP "  : ""),// Resetting the SUSPEND block code 
             (arg4 & SCHED_STATE_POOF    ? "POOF  " : ""),// Resetting the POOF block code 
             (arg4 & SCHED_STATE_RESERVED ? "RESERVED" : ""),// Resetting the RESERVED block code 
             (arg4 & SCHED_STATE_APPEXIT ? "APPEXIT " : "") // Resetting the APP EXIT block code 
             );
}

void Flight_SchedPriorityChange(size_t bufsize, char* buffer, const BG_FlightRecorderLog_t* log, void* ptr)
{
    uint64_t arg1 = log->data[0];
    uint64_t arg2 = log->data[1];
    uint64_t arg3 = log->data[2];
    uint64_t arg4 = log->data[3];
    snprintf(buffer, bufsize, "Sched Priority TID=%ld Original Priority=%ld New Priority=%ld THRDS=%ld RUNNABLE=%ld", 
             arg1,                                      // Thread ID target of the priority change 
             arg2,                                      // Original thread priority
             arg3,                                      // New thread priority
             (arg4>>SPRG_SPIinfo_NumThds)&0xFF,         // Number of software threads currently active on this hardware thread
             (arg4>>SPRG_SPIinfo_Runnable)&0xFF         // Number of runnable software threads
             );
}   


void Flight_CNVerbAddCompletionDecoder(size_t bufsize, char* buffer, const BG_FlightRecorderLog_t* log, void* ptr)
{
    char *status = NULL;
    switch (log->data[2]) {
        case CNV_WC_SUCCESS:          status = "WC_SUCCESS"; break;
        case CNV_WC_FATAL_ERR:        status = "WC_FATAL_ERR"; break;
        case CNV_WC_RESP_TIMEOUT_ERR: status = "WC_RESP_TIMEOUT_ERR"; break;
        case CNV_WC_GENERAL_ERR:      status = "WC_GENERAL_ERR"; break;
        default:                      status = "unknown"; break;
    }
    char *state = NULL;
    switch (log->data[3]) {
        case CNVERBS_WC_INIT:            state = "WC_INIT";            break;
        case CNVERBS_WC_POSTED_SEND:     state = "WC_POSTED_SEND";     break;
        case CNVERBS_WC_POSTED_RECV:     state = "WC_POSTED_RECV";     break;
        case CNVERBS_WC_RECV_INPROGRESS: state = "WC_RECV_INPROGRESS"; break;
        case CNVERBS_WC_READY:           state = "WC_READY";           break;
        default:                         state = "unknown";            break;
    }
    snprintf(buffer, bufsize, "Added work completion with status %s (%ld) from previous state %s (%ld) to cq %ld which now has %ld completions",
             status, log->data[2], state, log->data[3], log->data[0], log->data[1]+1);
}


void Flight_ToolMsgDecoder(size_t bufsize, char* buffer, const BG_FlightRecorderLog_t* log, void* ptr)
{
    uint64_t arg1 = log->data[0] & 0xFFFF; // lower part of first parm is message type
    uint64_t arg2 = log->data[1] & 0xFFFFFFFF; // lower part of second parm is the rank
    uint64_t arg3 = log->data[2];
    uint64_t arg4 = (log->data[0]) >> 32; // upper part of first parm is sequence number
    uint64_t arg5 = (log->data[1]) >> 32; // upper part of second parm is message length  
    // Should use the constants for the Message Types, however its buried in a namespace of a C++ header file.
    char *msgtext = "Msg";
    switch(arg1)
    {
    case 5001: // Attach
        msgtext = "Attach";
        break;
    case 5003: // Detach
        msgtext = "Detach";
        break;
    case 5005: // Query
        msgtext = "Query";
        break;
    case 5007: // Update
        msgtext = "Update";
        break;
    case 5010: // SetupJobAck
        msgtext = "SetupJobAck";
        break;
    case 5012: // NotifyAck  
        msgtext = "NotifyAck";
        break;
    case 5013: // Control
        msgtext = "Control";
        break;
    }
    snprintf(buffer, bufsize, "Tool %s %ld Toolid %ld Rank %ld Seq# %ld Lngth %ld", msgtext, arg1, arg3, arg2, arg4, arg5); 
}

void Flight_ToolMsgAckDecoder(size_t bufsize, char* buffer, const BG_FlightRecorderLog_t* log, void* ptr)
{
    uint64_t arg1 = log->data[0] & 0xFFFF; // lower part of first parm is message type
    uint64_t arg2 = log->data[1] & 0xFFFFFFFF; // lower part of second parm is the rank
    uint64_t arg3 = log->data[2];
    uint64_t arg4 = (log->data[0]) >> 32; // upper part of first parm is sequence number
    uint64_t arg5 = (log->data[1]) >> 32; // upper part of second parm is message length  

    char *msgtext = "MsgAck";
    switch(arg1)
    {
    case 5002: // AttachAck
        msgtext = "AttachAck";
        break;
    case 5004: // DetachAck
        msgtext = "DetachAck";
        break;
    case 5006: // QueryAck
        msgtext = "QueryAck";
        break;
    case 5008: // UpdateAck
        msgtext = "UpdateAck";
        break;
    case 5014: // ControlAck
        msgtext = "ControlAck";
        break;
    }
    snprintf(buffer, bufsize, "Tool %s(%ld) Toolid %ld Rank %ld Seq# %ld Lngth %ld",msgtext, arg1, arg3, arg2, arg4, arg5); 
}
void Flight_ToolCmdDecoder(size_t bufsize, char* buffer, const BG_FlightRecorderLog_t* log, void* ptr)
{
    uint64_t arg1 = log->data[0];
    uint64_t arg2 = log->data[1];
    uint64_t arg3 = log->data[2];
    uint64_t arg4 = log->data[3];
    // Should use the constants for the Message Types, however its buried in a namespace of a C++ header file.
    char* cmdtext = "Cmd";
    switch (arg1)
    {
    case 217: // Send signal
        cmdtext = "SendSignal";
        break;
    case 219: // Continue process
        cmdtext = "ContinueProcess";
        break;
    case 221: // Step thread
        cmdtext = "StepThread";
        break;
    case 249: // Release control
        cmdtext = "ReleaseControl";
        break;
    case 251: // Set Continuation signal
        cmdtext = "SetContinuationSignal";
        break;
    case 209: // Hold thread
        cmdtext = "Holdthread";
        break;
    case 211: // Release thread
        cmdtext = "ReleaseThread";
        break;
    case 223: // Set breakpoint
        cmdtext = "SetBreakpoint";
        break;
    case 225: // Reset breakpoint
        cmdtext = "ResetBreakpoint";
        break;
    case 109: // Get memory
        cmdtext = "GetMemory";
        break;
    }
    snprintf(buffer, bufsize, "Tool %s(%ld) TID %ld Toolid %ld Rank %ld", cmdtext, arg1, arg2, arg4, arg3); 
}

void Flight_ToolNotifyDecoder(size_t bufsize, char* buffer, const BG_FlightRecorderLog_t* log, void* ptr)
{
    uint64_t arg1 = log->data[0] & 0xFF;
    uint64_t arg2 = log->data[1]; 
    uint64_t arg3 = log->data[2];
    uint64_t arg4 = log->data[3];
    uint64_t arg5 = (log->data[0]) >> 32;
    uint64_t arg6,arg7; 
    // Enumeration values are buried in a C++ namespace in ToolctlMessage.h. 
    switch (arg1)
    {
    case 0: // signal
        arg2 &= 0xFFFF;
        arg6 = (log->data[1]) >> 32;
        arg3 &= 0xFFFFFFFF;
        arg7 = (log->data[2]) >> 32;
        snprintf(buffer, bufsize, "Tool Notify Signal %ld Rank %ld TID %ld Toolid %ld seq# %ld specActive %ld", arg2, arg6, arg3, arg4, arg5, arg7); 
        break;
    case 1: // termination
        snprintf(buffer, bufsize, "Tool Notify EXIT Status %08lx Rank %ld Toolid %ld seq# %ld", arg2, arg3, arg4, arg5); 
        break;
    case 2: // control
        arg2 &= 0xFFFF;
        arg6 = (log->data[1]) >> 32;
        switch (arg2)
        {
        case 0: // Conflict
            snprintf(buffer, bufsize, "Tool Notify Conflict Rank %ld Toolid1 %ld Toolid2 %ld seq# %ld", arg6, arg3, arg4, arg5); 
            break;
        case 1: // Available
            snprintf(buffer, bufsize, "Tool Notify Available Rank %ld Toolid1 %ld Toolid2 %ld seq# %ld", arg6, arg3, arg4, arg5); 
            break;
        default:
            snprintf(buffer, bufsize, "Tool Notify Control UNKNOWN REASON %ld Rank %ld %ld %ld seq# %ld",arg2, arg6, arg3,arg4, arg5); 
            break;
        }
        break;
    default:
        snprintf(buffer, bufsize, "Tool Notify UNKNOWN TYPE %ld %ld %ld %ld",log->data[0],log->data[1],log->data[2],log->data[3]); 
    }
}

void Flight_MailboxDecoder(size_t bufsize, char* buffer, const BG_FlightRecorderLog_t* log, void* ptr)
{
    uint64_t arg1 = log->data[0];
    uint64_t arg2 = log->data[1];
    char* commandname = NULL;
    char tmpname[64];
    
#define name(n) case n: commandname = #n + 13; break;    
    switch (arg1)
    {
        name(JMB_CMD2CORE_NONE);
        name(JMB_CMD2CORE_WRITE);
        name(JMB_CMD2CORE_FILL);
        name(JMB_CMD2CORE_READ);
        name(JMB_CMD2CORE_LAUNCH);
        name(JMB_CMD2CORE_STDIN);
        name(JMB_CMD2CORE_BARRIER_ACK);
        name(JMB_CMD2CORE_CONFIGURE_DOMAINS);
        name(JMB_CMD2CORE_ELF_SCTN_CRC);
        name(JMB_CMD2CORE_CONTROL_SYSTEM_REQUEST);
        default: 
            snprintf(tmpname, sizeof(tmpname), "Invalid Command %ld", arg1); 
            commandname = tmpname;
            break;
    }
    snprintf(buffer, bufsize, "Node received mailbox command '%s' with a length of %ld bytes", commandname, arg2);
}

void Flight_HaltDecoder(size_t bufsize, char* buffer, const BG_FlightRecorderLog_t* log, void* ptr)
{
    uint64_t arg1 = log->data[0];
    switch(arg1)
    {
        case 1: snprintf(buffer, bufsize, "Kernel_Halt().  Recursive halt detected"); break;
        case 2: snprintf(buffer, bufsize, "Kernel_Halt().  Halting pluggable file systems"); break;
        case 3: snprintf(buffer, bufsize, "Kernel_Halt().  Halting job control and tool control"); break;
        case 4: snprintf(buffer, bufsize, "Kernel_Halt().  Halting network interfaces"); break;
        case 5: snprintf(buffer, bufsize, "Kernel_Halt().  Halting hardware threads"); break;
        case 6: snprintf(buffer, bufsize, "Kernel_Halt().  Notifying control system of successful halt"); break;
        case 7: snprintf(buffer, bufsize, "Kernel_Halt().  Halt completed"); break;
    }
}

void Flight_IPIDecoder(size_t bufsize, char* buffer, const BG_FlightRecorderLog_t* log, void* ptr)
{
    char namebuffer[64];
    const char* formatstr;
    const char* function_name = namebuffer;
    snprintf(namebuffer, sizeof(namebuffer), "unreg(0x%016lx)", log->data[1]);
#define IPIHANDLER(name) if( ((uint64_t)((void*)name)) == log->data[1]) function_name = #name;
#include "core2core.h"
    switch(log->id)
    {
        case FL_DELIVRIPI:
            formatstr = "Send IPI to hwthread %ld to execute '%s' with parm1=%lx, parm2=%lx";
            break;
        case FL_RECEIVIPI:
            formatstr = "Received IPI from hwthread %ld to execute '%s' with parm1=%lx.  Interrupted IAR was 0x%016lx";
            break;
        case FL_IPIORPHAN:
            formatstr = "Forcing delivery of orphaned IPI from hwt %ld to execute '%s' with parm1=%lx. Interrupted IAR was 0x%016lx";
            break;
        case FL_FLUSHTCMD:
            formatstr = "Tool deadlock avoidance. IPI from hwthread %ld to execute '%s' with parm1=%lx.  Interrupted IAR was 0x%016lx";
            break;
        case FL_ADLOCKIPI:
            formatstr = "Avoid Ack deadlock. Process IPI from hwthread %ld to execute '%s' with parm1=%lx.  Interrupted IAR was 0x%016lx";
            break;
        default:
            formatstr = "Unknown IPI flow!!! %ld %s %ld %ld";
    }
    snprintf(buffer, bufsize, formatstr, log->data[0], function_name, log->data[2], log->data[3]);
}


void Flight_SpeculationRestart(size_t bufsize, char* buffer, const BG_FlightRecorderLog_t* log, void* ptr)
{
    uint64_t arg1 = log->data[0];
    uint64_t arg2 = log->data[1];
    uint64_t arg3 = log->data[2];
    uint64_t arg4 = log->data[3];

    snprintf(buffer, bufsize, "Speculation Restart SpecID=%ld  Rolling back to IAR=0x%016lx with StackPointer=0x%016lx and GPR3=0x%016lx", 
             arg1, arg2, arg3, arg4);
}

void Flight_FlockDecoder(size_t bufsize, char* buffer, const BG_FlightRecorderLog_t* log, void* ptr)
{
    uint64_t arg1 = log->data[0];
    uint64_t arg2 = log->data[1];
    uint64_t remote_fd  = log->data[2] >> 32;
    uint64_t ltype      = log->data[2] & 0xffffffffull;
    uint64_t cmd2       = (log->data[3] >> 48);
    uint64_t cmd        = (log->data[3] >> 32) & 0xffff;
    uint64_t whence     = log->data[3] & 0xffffffffull;
    
    const char* ltype_str;
    const char* whence_str;
    const char* cmd_str;
    switch(ltype)
    {
        case F_RDLCK:
            ltype_str = "RDLCK";
            break;
        case F_WRLCK:
            ltype_str = "WRLCK";
            break;
        case F_UNLCK:
            ltype_str = "UNLCK";
            break;
        default:
            ltype_str = "?????";
    }
    switch(whence)
    {
        case SEEK_SET: whence_str = "SEEK_SET"; break;
        case SEEK_CUR: whence_str = "SEEK_CUR"; break;
        case SEEK_END: whence_str = "SEEK_END"; break;
        default:       whence_str = "????????"; break;
    }
    switch(cmd)
    {
        case F_GETLK:              
            cmd_str = "GETLOCK"; 
            break;
        case F_SETLK:  cmd_str = "SETLOCK"; break;
        case F_SETLKW: cmd_str = "SETLOCK_WAIT"; break;
        default:       cmd_str = "????????"; break;
    }
    if(cmd != cmd2)
        cmd_str = "SETLOCK_WAIT->SETLOCK";
    
    snprintf(buffer, bufsize, "Advisory file %s  (cmd=%ld  cmd2=%ld).  type=%s  start=%ld len=%ld remoteFD=%ld  whence=%s", cmd_str, cmd,cmd2, ltype_str, arg1, arg2, remote_fd, whence_str);
}
