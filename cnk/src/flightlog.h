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

#ifndef FLIGHTLOG_H_
#define FLIGHTLOG_H_

#include <hwi/include/common/compiler_support.h>
__BEGIN_DECLS

extern uint64_t               FlightLock;
#define FlightLogSize         8192  // 48 bytes per entry => 786432 bytes
extern BG_FlightRecorderLog_t FlightLog[FlightLogSize];
#define FLIGHTLOG             &FlightLock, FlightLog, FlightLogSize

extern uint64_t               FlightLock_high;
#define FlightLogSize_high    1024 // 48 bytes per entry => 49152 bytes
extern BG_FlightRecorderLog_t FlightLog_high[FlightLogSize_high];
#define FLIGHTLOG_high        &FlightLock_high, FlightLog_high, FlightLogSize_high


#define FLIGHTDECODER(name) extern void name(size_t bufsize, char* buffer, const BG_FlightRecorderLog_t* logentry, void* ptr)
enum
{
    FL_INVALID=0,
#define FLIGHTPRINT(name, format)   name,
#define FLIGHTFUNCT(name, function) name,
#include "flightlog.h"
    FL_NUMENTRIES
};

extern int addFlightRecorder(BG_FlightRecorderRegistry_t* reg);
extern int registerFlightRecorder();
extern int dumpFlightRecorder();
extern int getFlightRecorder(size_t bufferSize, char* buffer);

/*********************************/
/* Decoder definitions
**********************************/
FLIGHTDECODER(Flight_SyscallDecoder);
FLIGHTDECODER(Flight_SchedDispatchDecoder);
FLIGHTDECODER(Flight_SchedBlockRemoteDecoder);
FLIGHTDECODER(Flight_SchedBlockDecoder);
FLIGHTDECODER(Flight_SchedUnBlockRemoteDecoder);
FLIGHTDECODER(Flight_SchedUnBlockDecoder);
FLIGHTDECODER(Flight_CNVerbAddCompletionDecoder);
FLIGHTDECODER(Flight_ToolMsgDecoder);
FLIGHTDECODER(Flight_ToolMsgAckDecoder);
FLIGHTDECODER(Flight_ToolCmdDecoder);
FLIGHTDECODER(Flight_ToolNotifyDecoder);
FLIGHTDECODER(Flight_MailboxDecoder);
FLIGHTDECODER(Flight_HaltDecoder);
FLIGHTDECODER(Flight_SchedPriorityChange);
FLIGHTDECODER(Flight_IPIDecoder);
FLIGHTDECODER(Flight_SyscallReturnDecoder);
FLIGHTDECODER(Flight_SysioMsgDecoder);
FLIGHTDECODER(Flight_SpeculationRestart);


/**********************************/

__END_DECLS
#endif
#ifndef FLIGHTPRINT
#define FLIGHTPRINT(a,b)
#endif
#ifndef FLIGHTFUNCT
#define FLIGHTFUNCT(a,b)
#endif

/*
  Flight recorder log IDs and Format strings
  IDs must be unique.
  Please keep IDs the same length, as the ID names are included in the output.  You can add underscores as a suffix if you'd like to pad the names.
  
  Format strings should take 4 64-bit integer arguments.  
  
  These IDs can be referenced in your code via:      Kernel_WriteFlightLog(FLIGHTLOG, FL_SYSCALLAT, sc_num, context->ip, context->lr, context->gpr[1]);
  
  \note BG_TRACE=0x4000000000 dumps the flight recorder log on process exit.
 */


FLIGHTFUNCT(FL_SYSCALLAT, Flight_SyscallDecoder)
FLIGHTPRINT(FL_SYSCALLEN, "Syscall Entry  GPR3=0x%016lx  GPR4=0x%016lx  GPR5=0x%016lx  GPR6=0x%016lx")
//FLIGHTPRINT(FL_SYSCALLRT, "Syscall Return GPR3=0x%016lx")
FLIGHTFUNCT(FL_SYSCALLRT, Flight_SyscallReturnDecoder)
FLIGHTPRINT(FL_MEMCPY___, "Large memory copy LR=0x%016lx Dst=0x%016lx Src=0x%016lx Length=%ld bytes")
FLIGHTPRINT(FL_MEMSET___, "Large memory set  LR=0x%016lx Dst=0x%016lx Value=0x%lx Length=%ld bytes")
FLIGHTPRINT(FL_MALLOC___, "Dynamic memory allocation LR=0x%016lx for %ld bytes")
FLIGHTFUNCT(FL_SPCRESTRT, Flight_SpeculationRestart)
FLIGHTPRINT(FL_L2CENTINT, "L2 central speculation interrupt.  SpecID=%ld  Interrupt occurred at IAR=0x%016lx  LR=0x%016lx")
FLIGHTPRINT(FL_MAPTLB___, "Map TLB entry at virtualAddress=0x%016lx to physicalAddress=0x%016lx with size=%ld  flag/pid=%016lx")
FLIGHTPRINT(FL_UNMAPTLB_, "Unmapping TLB entry with virtualAddress=0x%016lx and size=%ld and pid=%ld")
FLIGHTPRINT(FL_DEBUGEXCP, "Debug exception.  IP=0x%016lx DBSR::ESR=0x%016lx  IntCode=0x%016lx  DEAR=0x%016lx")
FLIGHTPRINT(FL_SCHDENTER, "Sched Entry     TID=%3ld IP=0x%016lx SPRG7L=0x%08lx LR=0x%016lx")
FLIGHTFUNCT(FL_SCHEDBLKL, Flight_SchedBlockDecoder)
FLIGHTFUNCT(FL_SCHEDBLKR, Flight_SchedBlockRemoteDecoder)
FLIGHTFUNCT(FL_SCHEDUNBL, Flight_SchedUnBlockDecoder)
FLIGHTFUNCT(FL_SCHEDUNBR, Flight_SchedUnBlockRemoteDecoder)
FLIGHTFUNCT(FL_SCHEDDISP, Flight_SchedDispatchDecoder)
FLIGHTPRINT(FL_TLBADDSEG, "Static TLB Mapper.  Add segment.  Type=%ld   Vaddr=0x%016lx for %ld bytes  ProcessMask=%016lx")
FLIGHTPRINT(FL_VMMInited, "CNK virtual memory subsystem initialized.  TextStart=0x%016lx  KernelSize=%ld bytes  TextSize=%ld bytes  AtomicSize=%ld bytes")
FLIGHTPRINT(FL_TLBMAPRST, "Static TLB Mapper.  Reset.  Number of processes=%ld   Number of appagents=%ld")
FLIGHTPRINT(FL_TLBMAPCMP, "Static TLB Mapper.  Complete.  Wasted=%ld  TotalHeapSize=%ld  BestWaste=%ld  BestMode=%ld")
FLIGHTPRINT(FL_TLBMAPPER, "Static TLB Mapper.  Running partitioned mapper")
FLIGHTPRINT(FL_TLBSEGOVL, "Static TLB Mapper.  Attempted to add a overlapping segments.  Segment %ld  address=%ld  low=%ld  high=%ld")
FLIGHTPRINT(FL_L2PRTYWIN, "TM Speculation.  Thread won priority conflict and enemy has been invalidated, resuming thread.  MySpecID=%lx  EnemySpecID=%lx  MyPriority=%016lx EnemyPriority=%016lx")
FLIGHTPRINT(FL_L2NOSTRCT, "TM Speculation.  Conflict state structure not defined.  SpecState=0x%p")
FLIGHTPRINT(FL_L2NOCNFLT, "TM Speculation.  No single conflict detected  MyCR=%016lx")
FLIGHTPRINT(FL_L2COMMIT_, "TM Speculation.  Speculation interrupt occurred but we are not speculative.  specid=%lx")
FLIGHTPRINT(FL_PWRMGMTEV, "Proactive power throttling event.  Delaying %ld cycles.  Thread idle until timebase=0x%016lx  # of HWThreads participating=%ld")
FLIGHTPRINT(FL_KERNCRASH, "CNK is abnormally terminating with status %ld.  Link Register=0x%016lx  SRR0=0x%016lx  CSRR0=0x%016lx")
FLIGHTPRINT(FL_MALLCANON, "Memory allocation for %ld bytes (rounded up to %ld bytes).  Memory now available %ld bytes.   Returning address 0x%p.")
FLIGHTPRINT(FL_MALLCADDR, "Memory allocation by address 0x%p for %ld bytes.  Memory now available %ld bytes.  Returning address 0x%p.")
FLIGHTPRINT(FL_MALLCFREE, "Memory deallocation for address 0x%p for %ld bytes.  Memory now available %ld bytes.")
FLIGHTPRINT(FL_MALLCINIT, "Memory allocator initialization.  Starting at address %p for %ld bytes.")
FLIGHTPRINT(FL_MALLCBRK_, "Memory break pointer moved by %ld bytes to address 0x%p.")
FLIGHTPRINT(FL_MALLCBRKI, "Memory break pointer initialized to address 0x%p.")
FLIGHTPRINT(FL_PWRMGMTTH, "Proactive power throttling set.  DDR throttling at %ld.  Processor throttling for %ld pclk every %ld pclk")
FLIGHTPRINT(FL_PWRMGMTSD, "Proactive power throttling disabled.  ")
FLIGHTPRINT(FL_SIGHANDLE, "Signal handler launched.  TID %ld, signal %ld, hndlr 0x%lx, ucontext 0x%lx")
FLIGHTPRINT(FL_SIGRETURN, "Signal handler returned.  TID %ld, ucontext 0x%lx  Next IP=0x%lx")
FLIGHTPRINT(FL_TLBMAPFAI, "Static TLB Mapper.  Failed to find a map.  Requested application arena for all processes is 0x%08lx bytes.  Process 0 arena would've been 0x%08lx bytes (excludes shared)")
FLIGHTFUNCT(FL_CNVADDWCE, Flight_CNVerbAddCompletionDecoder)
FLIGHTPRINT(FL_CNVRMVWCE, "Removed work completion 0x%lx for hw thread %lu from completion queue %ld which now has %ld completions")
FLIGHTPRINT(FL_CNVDISCON, "Disconnect request was ignored")
FLIGHTPRINT(FL_CNVCONREQ, "Connect request was ignored")
FLIGHTPRINT(FL_CNVBADQPN, "Local queue pair %lu (handle %lu) in message is invalid")
FLIGHTPRINT(FL_CNVQPNUSE, "Local queue pair %lu (handle %lu) is not in use")
FLIGHTPRINT(FL_CNVCONREJ, "Connect request from queue pair %lu (handle %lu) was rejected with status %lu")
FLIGHTPRINT(FL_CNVCONACC, "Connect completed for queue pair %u (handle %u) to remote queue pair %u")
FLIGHTPRINT(FL_CNVBADPSN, "Sequence number %lu in packet does not match expected value %lu for queue pair %lu (handle %lu)")
FLIGHTPRINT(FL_CNVBADSQP, "Source QP %lu in packet does not match expected value %lu for source QP %lu (handle %lu)")
FLIGHTPRINT(FL_CNVNORECV, "A receive is not posted for queue pair %lu (handle %lu) ")
FLIGHTPRINT(FL_CNVBADIMM, "Bad immmediate packet has starting  data 0x%lx 0x%lx 0x%lx 0x%lx" )
FLIGHTPRINT(FL_CNVBADRMT, "Bad remote packet starting at payload.remote_vaddr  %016lx %016lx %016lx %016lx ")

FLIGHTPRINT(FL_CNVRDMARD, "mudm_rdma_read() failed with rc %d for queue pair %u (handle %u)")
FLIGHTPRINT(FL_CNVBADMR_, "Memory region with rkey %lu was not found")
FLIGHTPRINT(FL_CNVMRSIZE, "Memory region with rkey %lu and length %lu is too small for request of %lu bytes")
FLIGHTPRINT(FL_CNVBADWCE, "Work completion %lu with state %lu does not have outstanding operation")
FLIGHTPRINT(FL_CNVNULLCQ, "Work completion %lu has a null cq pointer, qp %lu, state %lu")
FLIGHTPRINT(FL_PWREVNTA2, "Reactive power threshold exceeded for processor domain.  current=%ld mA.  Starting processor throttling every %ld usec for %ld usec.  IP=%016lx")
FLIGHTPRINT(FL_PWREVNTMC, "Reactive power threshold exceeded for memory controller domain.  current=%ld mA.  Starting DDR throttling at %ld.  IP=%016lx")
FLIGHTPRINT(FL_APPBCASTO, "Collective load broadcast elfimage object data. Addr %016lx, Len %ld, Load status:%016lx")
FLIGHTPRINT(FL_APPBCASTS, "Collective load broadcast section data. Addr %016lx, Len %ld, Load status:%016lx")
FLIGHTPRINT(FL_CNVPSTSND, "Posted send to queue pair %lu using work completion 0x%lx from completion queue %ld, return code %ld")
FLIGHTPRINT(FL_KERNEXITD, "CNK is exitting with status %ld.  Link Register=0x%016lx  SRR0=0x%016lx  CSRR0=0x%016lx")
FLIGHTPRINT(FL_CNVREGMRE, "Registering a memory region for address 0x%16lx with length %lu in protection domain %lu failed with error %ld")
FLIGHTPRINT(FL_CNVDRGMRE, "Deregistering a memory region for address 0x%16lx with length %lu and local key %lu failed with error %ld")
FLIGHTPRINT(FL_CNVPSTRCV, "Posted recv to queue pair %lu using work completion 0x%lx from completion queue %ld.  wr_id=%lx")
FLIGHTPRINT(FL_CNVSELWCE, "Selected work completion 0x%lx from completion queue %ld for hw thread %lu from recv callback, first address is %lx")
FLIGHTPRINT(FL_CNVPOLLCQ, "Returned %ld work completions for hw thread %ld to caller for completion queue %lu, %ld work completions are still available")
FLIGHTPRINT(FL_JOBSETUP_, "Job %lu completed setup with %lu ranks on this node and %lu ranks participating in job for blockid %lu")
FLIGHTPRINT(FL_JOBLOADED, "Job %lu is loaded with returnode=%ld and errorCode=%ld")
FLIGHTPRINT(FL_JOBSTARTD, "Job %lu is started at timestamp 0x%016lx")
FLIGHTPRINT(FL_JOBEXITED, "Job %lu exited with status 0x%016lx")
FLIGHTPRINT(FL_JOBPROCEX, "Job %lu rank %lu exited with status 0x%08lx, agent %lu")
FLIGHTPRINT(FL_JOBSIGNAL, "Job %lu signaled with signal %ld")
FLIGHTPRINT(FL_JOBCLEAND, "Job %lu completed cleanup")
FLIGHTFUNCT(FL_TOOLRCMSG, Flight_ToolMsgDecoder)
FLIGHTFUNCT(FL_TOOLMSGAK, Flight_ToolMsgAckDecoder)
FLIGHTFUNCT(FL_TOOLCMD__, Flight_ToolCmdDecoder)
FLIGHTFUNCT(FL_TOOLNTIFY, Flight_ToolNotifyDecoder)
FLIGHTPRINT(FL_TOOLDEFER, "Tool SignalEncounterred Deferring Notify signum=%d Tid=%d NotifyCtlMask=%016lx")
FLIGHTPRINT(FL_TOOLTTBFD, "Tool Trap table address found. Address=%016lx Depth=%ld Procmask=%016lx")
FLIGHTPRINT(FL_TOOLTTBNF, "Tool Trap table address not found. Returning next available entry. Address=%016lx Depth=%ld")
FLIGHTPRINT(FL_TOOLTTBFU, "Tool Trap table full. Address=%016lx Depth=%ld")
FLIGHTPRINT(FL_TOOLCMDCP, "Tool Command Complete. CMD=%01d TID=%lu RC=%ld")
FLIGHTPRINT(FL_TOOLCMDCO, "Tool Continue Process NotifyCtlMask=%016lx notifiedHwThread=%lu notifiedTID %lu targetTID=%lu")
FLIGHTPRINT(FL_TOOLCMDST, "Tool Step Thread NotifyCtlMask=%016lx notifiedHwThread=%lu notifiedTID %lu targetTID=%lu")
FLIGHTFUNCT(FL_MBOXINPUT, Flight_MailboxDecoder)
FLIGHTPRINT(FL_MBOXINERR, "Mailbox input interrupt occurred but mailbox poll returned error %ld")
FLIGHTFUNCT(FL_KERNHALTD, Flight_HaltDecoder)
FLIGHTPRINT(FL_KERNREADY, "Kernel has successfully initialized")
FLIGHTPRINT(FL_CNVDISCNE, "Disconnecting queue pair %lu (handle %lu) failed with error %ld")
FLIGHTPRINT(FL_CNVDESQPE, "Destroying queue pair %lu (handle %lu) failed with error %ld")
FLIGHTPRINT(FL_CNVDESCQE, "Destroying completion queue %lu failed with error %ld")
FLIGHTPRINT(FL_CNVDEAPDE, "Deallocating protection domain %lu failed with error %ld")
FLIGHTPRINT(FL_TOOLABRKM, "Add breakpoint via SetMemory. Address=%016lx Instr=%016lx result=%ld")
FLIGHTPRINT(FL_TOOLABRKB, "Add breakpoint via SetBreakpoint. Address=%016lx Instr=%016lx result=%ld")
FLIGHTPRINT(FL_TOOLRBRKM, "Remove breakpoint via SetMemory. Address=%016lx Instr=%016lx result=%ld")
FLIGHTPRINT(FL_TOOLRBRKB, "Remove breakpoint via RemoveBreakpoint. Address=%016lx Instr=%016lx result=&ld")
FLIGHTPRINT(FL_VERMISMAT, "For message service %lu CNK protocol version %lu does not match CIOS protocol version %lu")
FLIGHTPRINT(FL_MEMMOVEUP, "Moving memory from address 0x%016lx to 0x%016lx for %ld bytes.")
FLIGHTPRINT(FL_SHMDEFSPC, "ShmFS redefine memory from address 0x%016lx for %ld bytes --> address 0x%016lx for %ld bytes")
FLIGHTPRINT(FL_SHMCMPSPB, "ShmFS Compacting memory (begin)")
FLIGHTPRINT(FL_SHMCMPSPF, "ShmFS Compacting memory (finish) %ld bytes moved, %ld bytes free.")
FLIGHTPRINT(FL_TOOLSSWAT, "Start SetWatch Address1=%016lx refcnt=%ld Address2=%016lx refcnt=%ld")
FLIGHTPRINT(FL_TOOLESWAT, "End SetWatch Address1=%016lx refcnt=%ld Address2=%016lx refcnt=%ld")
FLIGHTPRINT(FL_TOOLSRWAT, "Start ResetWatch Address1=%016lx refcnt=%ld Address2=%016lx refcnt=%ld")
FLIGHTPRINT(FL_TOOLERWAT, "End ResetWatch Address1=%016lx refcnt=%ld Address2=%016lx refcnt=%ld")
FLIGHTFUNCT(FL_DELIVRIPI, Flight_IPIDecoder)
FLIGHTFUNCT(FL_RECEIVIPI, Flight_IPIDecoder)

FLIGHTPRINT(FL_SPCALCDOM, "TM/SE allocated domain %ld")
FLIGHTPRINT(FL_SPCGETDOM, "TM/SE got %ld domains active")
FLIGHTPRINT(FL_SPCSETDOM, "TM/SE set %ld domains active")
FLIGHTPRINT(FL_CNVGOTWCE, "Got work completion 0x%lx from completion queue %ld for hw thread %lu from status callback  index=%ld")
FLIGHTPRINT(FL_NDINTALND, "Allowed ND interrupt. Non-fatal ND dcr=%016lx value=%016lx previously seen=%016lx")
FLIGHTPRINT(FL_NDINTALMU, "Allowed MU interrupt. Non-fatal MU dcr=%016lx value=%016lx")
FLIGHTPRINT(FL_MIGRATETH, "Thread migration in IPI handler. TID=%016lx from hwt=%ld to hwt=%ld status=%ld")
FLIGHTFUNCT(FL_SCHEDPRIO,  Flight_SchedPriorityChange)
FLIGHTPRINT(FL_SPCFEPENA, "TM/SE fast exception paths enabled")
FLIGHTPRINT(FL_SPCFEPDIS, "TM/SE fast exception paths disabled")

FLIGHTFUNCT(FL_SYSMSGSND,Flight_SysioMsgDecoder)
FLIGHTFUNCT(FL_SYSMSGRCV,Flight_SysioMsgDecoder)
FLIGHTFUNCT(FL_JCLMSGSND,Flight_SysioMsgDecoder)
FLIGHTFUNCT(FL_JCLMSGRCV,Flight_SysioMsgDecoder)
FLIGHTFUNCT(FL_STDMSGSND,Flight_SysioMsgDecoder)
FLIGHTFUNCT(FL_STDMSGRCV,Flight_SysioMsgDecoder)
FLIGHTFUNCT(FL_UNKMSGRCV,Flight_SysioMsgDecoder)
FLIGHTPRINT(FL_BADQP_VAL, "QP_PTR=%016lx __KERNEL_END=%016lx qp_handle=%016lx msg->ionet_hdr.dest_qpn=%016lx")
FLIGHTPRINT(FL_GETCOMPLT, "sysio::getCompletion requested=%ld found=%ld index=%ld  wr_id=%lx")
FLIGHTPRINT(FL_SSETUPACK,"sysio::SetupjobAck info pid=%ld serviceId=%ld posix/stats=%016lx fshiperr/shortCircuitPath=%016lx")
FLIGHTPRINT(RDMAFS_INITC,"_queuePair.qp_num=%d, _queuePair.handle=%ld, err=%d, FD_RDMA=%d")
FLIGHTPRINT(RDMAFS_TERMS,"_queuePair.qp_num=%d, _queuePair.handle=%ld, _fd=%d, FD_RDMA=%d")


FLIGHTPRINT(FL_L2ATOMICS, "L2 atomic allocate vaddr=%lx length=%lx sizeMapped=%lx state=%ld")

FLIGHTPRINT(FL_SPCPNDRST, "Pending Speculation restart for JMV. specid=%ld")

FLIGHTFUNCT(FL_IPIORPHAN, Flight_IPIDecoder)
FLIGHTFUNCT(FL_FLUSHTCMD, Flight_IPIDecoder) 

FLIGHTPRINT(FL_MALLCFRFL, "Memory deallocation failed. No free chunk available for address 0x%p for %ld bytes.")

FLIGHTPRINT(FL_SCRUBSETP, "Setting up background scrub.  Virtual Address at 0x%p, size %ld bytes, using TLB slot %ld")
FLIGHTPRINT(FL_SCRUBSTRT, "Performing background DDR CE scrub at physical address 0x%p for %ld bytes.")
FLIGHTPRINT(FL_SCRUBSTOP, "Finished background scrub.")

FLIGHTFUNCT(FL_ADLOCKIPI, Flight_IPIDecoder)

FLIGHTPRINT(FL_CRCEXCHD1, "CRC exchange stuck.  Link %ld  tokens=%lx expected_tokens=%lx")
FLIGHTPRINT(FL_CRCEXCHD2, "CRC exchange stuck.  Link %ld  re_vc_state=%lx")
FLIGHTPRINT(FL_CRCEXCHD3, "CRC exchange stuck.  Link %ld  RE_VC1_STATE=%lx.  VC1_STATE__CTRL_SLOT_VALID=%lx")
FLIGHTPRINT(FL_CRCEXCHD4, "CRC exchange stuck.  Link %ld  RE_VC2_STATE=%lx.  VC2_STATE__CTRL_SLOT_VALID=%lx")
FLIGHTPRINT(FL_CRCEXCHD5, "CRC exchange stuck.  Link %ld  RE_VC4_STATE=%lx.  VC4_STATE__CTRL_SLOT_VALID=%lx")
FLIGHTPRINT(FL_CRCEXCHD6, "CRC exchange stuck.  Link %ld  RE_VC5_STATE=%lx   VC5_STATE__CTRL_SLOT_VALID=%lx")

FLIGHTPRINT(FL_MAPFILEOP, "Mapfile opened, track %ld")
FLIGHTPRINT(FL_MAPFILECL, "Mapfile closed, track %ld")
FLIGHTPRINT(FL_MAPFILEWR, "Mapfile write chunk %ld for %ld bytes to node %lx, track %ld")
FLIGHTPRINT(FL_MAPFILERD, "Mapfile read chunk %ld for %ld bytes to node %lx, track %ld")
FLIGHTPRINT(FL_MAPFILESM, "Mapfile read semaphore busy, node %lx while reading offset %lx via track %ld")

FLIGHTPRINT(FL_CLRTSET13, "Classroute generation: setting classroute 13 to input=%lx  output=%lx")
FLIGHTPRINT(FL_CLRTSNDUP, "Classroute generation: sending up injection to output_links=%lx")
FLIGHTPRINT(FL_MPMDSEQST, "ELF load iteration %ld.  My load iteration is %ld, max iteration is %ld")
FLIGHTPRINT(FL_MPMDSEQMX, "ELF load max iterations is %ld.")
FLIGHTPRINT(FL_MPMDSETUP, "MPMD configuration.  nodeCount=%ld  isLoadLeader=%ld  loadLeaderTorusCoord=%lx")

#undef FLIGHTPRINT
#undef FLIGHTFUNCT
