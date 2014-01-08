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

/*
  Flight recorder log IDs and Format strings
  IDs must be unique.
  Please keep IDs the same length, as the ID names are included in the output.  You can add underscores as a suffix if you'd like to pad the names.
  
  Format strings should take 4 64-bit integer arguments.  
  e.g., const char FORMAT4byHex16[]=" %016llx %016llx %016llx %016llx";
 */
#ifndef FLIGHTPRINT
#define FLIGHTPRINT(a,b)
#endif
#ifndef FLIGHTFUNCT
#define FLIGHTFUNCT(a,b)
#endif
#ifndef STANDARD
#define STANDARD(a)
#endif


//Prefetch_Only and Interrupt [on packet arrival] bits at bit 31 and 63
FLIGHTFUNCT(MU_INJNETHDR,MUDM_parseInjNetHdr)
FLIGHTFUNCT(PK_INJHEADER,MUDM_parsePK)
FLIGHTFUNCT(MF_INJHEADER,MUDM_parsePK)
FLIGHTFUNCT(DP_INJHEADER,MUDM_parseDirectPut)
FLIGHTFUNCT(PK_RCVHEADER,MUDM_parsePK)


//RDMA items
FLIGHTPRINT(C0_RDMACOUNT, "MUDM requestID=%llx, rdmaObject(counter)=%llx,localConnect=%llx, error=%lld")
FLIGHTPRINT(C0_RDMASTUCK, "MUDM requestID=%llx, rdmaObject(counter)=%llx,localConnect=%llx, error=%lld")

//Broadcasts
FLIGHTFUNCT(MF_INJBCAST_,MUDM_parsePKbcast)
FLIGHTFUNCT(RD_INJBCAST_,MUDM_parsePKbcast)
FLIGHTFUNCT(PK_RCVBCAST_,MUDM_parsePKbcast)
FLIGHTFUNCT(DP_BCASTRDMA,MUDM_parseDirectPut)
FLIGHTPRINT(RQ_BCASTRDMA, "MUDM bcast requestID=%llx, dest_payload_paddr=0x%llx, source_payload_paddr=0x%llx, payload_length=%llu")
FLIGHTFUNCT(C0_BCASTRDMA, MUDM_parseC0BCASTRDMA)

//Debug
FLIGHTFUNCT(DB_MF_TMPLAT,MUDM_parsePK)
FLIGHTFUNCT(DB_DP_TMPLAT,MUDM_parseDirectPut)
FLIGHTFUNCT(DB_RGETDP_TM,MUDM_parseDirectPut)
FLIGHTFUNCT(CN_STATUS___,MUDM_parse_connect_state)

FLIGHTFUNCT(DB_NH__STUCK,MUDM_parseInjNetHdr)
FLIGHTFUNCT(DB_PK__STUCK,MUDM_parsePK)
FLIGHTPRINT(DB_IFIFO_PTR, "inject fifo: va_start=%llx va_end=%llx va_head=%llx va_tail=%llx")
FLIGHTPRINT(DB_IFIFO_NUM, "inject fifo: descCount=%llu, freeSpace=%0llx, Max numDescriptors=%llu,Undone Descriptors=%llu")
FLIGHTPRINT(DB_RAWPKTDTA, "RAW: %016llx %016llx %016llx %016llx")

//Info items 
STANDARD(CN_RCV_PAYLO)
STANDARD(CN_INJ_PAYLO)
STANDARD(CN_GETFREE__)
FLIGHTFUNCT(CN_STARTING_,MUDM_parse_connect_state)
STANDARD(CN_START_ERR)
STANDARD(CN_DISC__ERR)
FLIGHTFUNCT(CN_FREED_CTX,MUDM_parse_connect_state)
FLIGHTFUNCT(CN_DISC_INDI,MUDM_parsePK)
STANDARD(CN_BDCALLBAK)
STANDARD(CN_CONNLISTS)

FLIGHTPRINT(DM_INITDONE_, "MUDM initialization completed at line %ld, protocol version %ld.  %ld links, contextptr=%p")
FLIGHTPRINT(DM_TERMSTART, "MUDM termination started at line %ld, protocol version %ld.  %ld links, contextptr=%p")
FLIGHTPRINT(IO_LINKRESET, "MUDM link reset starting.  i=%llu input BLOCKID=%llu remote_BlockID[i]=%llu link4reset[i]=%llu")
FLIGHTPRINT(IO_LINKSKIPD, "MUDM link skipped for reset.  i=%llu input BLOCKID=%llu remote_BlockID[i]=%llu link[i]=%llu")
FLIGHTPRINT(IO_LINKFLUSH, "MUDM link packets/RDMAs flushed.  i=%llu input BLOCKID=%llu remote_BlockID[i]=%llu link[i]=%llu")
FLIGHTPRINT(IO_LINKRSETC, "MUDM link reset complete.  i=%llu input BLOCKID=%llu remote_BlockID[i]=%llu link4reset[i]=%llu")
FLIGHTPRINT(MU_PKT_2POLL,"MUDM poll start request pktd=%p, ccontext=%p, desc_count=%llu, pktd->req_id=%p")
FLIGHTPRINT(MU_PKT_PCOMP,"MUDM poll completion pktd=%p, ccontext=%p, desc_count=%llu, pktd->req_id=%p")
FLIGHTPRINT(MU_STUCK_PKT, "MUDM stuck packet.  PacketPostedAtTime=%lx  CurrentTime=%lx  Context=%lx  ReqID=%lx")
STANDARD(DM_ERROR_HIT)
FLIGHTPRINT(CN_CALL_READ,"MUDM RDMA Read  remote_requestID=%p requestID=%p rdma_object=%p.  Tranferring %llu bytes")
FLIGHTPRINT(CN_CALLWRITE,"MUDM RDMA Write remote_requestID=%p  ccontext=%p rdma_object=%p.  Tranferring %llu bytes")

FLIGHTPRINT(RQ_CANCELED_,"Request canceled errno=%llu type=%llu, ccontext=%p, requestID=%p")
//FLIGHTFUNCT(DB_NHCN2LONG,MUDM_parseInjNetHdr)
FLIGHTFUNCT(CN_TOOK2LONG,MUDM_parsePK)
FLIGHTPRINT(DM_WKUP_EXIT, "MUDM wakeup poll completed at line %ld, protocol version %ld.  %ld links, contextptr=%p")
FLIGHTPRINT(DM_USEC_DIFF, "time difference %lld usec line= %ld cyclesstart=%lld cyclesend=%lld")  

FLIGHTFUNCT(DM_PERS_INFO,MUDM_personality_info)
FLIGHTFUNCT(DB_RECF_INFO,MUDM_receiveFifoInfo)
FLIGHTFUNCT(DB_INJF_INFO,MUDM_injectFifoInfo)
FLIGHTPRINT(DB_RESETTO_0,"Line=%llu, value was=%llu, ccontext=%p, ccontext_index=%llu")
FLIGHTPRINT(DB_RECVBADRC,"rc=%ld, -rc=%ld, type=%lx, line=%llu")
FLIGHTPRINT(DB_RECVPKTQQ,"pointer=%p, allocated=%llu, free=%llu, forward-chain-active=%llu")
FLIGHTPRINT(DB_RECVPKTBQ,"pointer=%p, allocated=%llu, free=%llu, backward-chain-active=%llu")
