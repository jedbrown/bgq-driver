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

//! \file  mudm_trace.h 
//! \brief Internal trace types and function prototypes and defines
//! \remarks primarily intended for OFED or CN verbs implementers over torus

#ifndef	_MUDM_TRACE_H_ /* Prevent multiple inclusion */
#define	_MUDM_TRACE_H_


#include <hwi/include/common/compiler_support.h>


#ifdef __LINUX__
#include <linux/kernel.h>
#include <linux/errno.h>
#else
#include <stdio.h>
#include <errno.h>
#endif

#include <spi/include/kernel/trace.h>
#include "mudm_macro.h"
#include "common.h"



// typedef from spi/include/kernel/trace.h
//typedef void (*BG_FlightRecorderFunc_t)(size_t bufsize, char* buffer, const BG_FlightRecorderLog_t* logentry, void* extraDataPtr);

int dump_flight_entry(BG_FlightRecorderRegistry_t* logregistry, uint64_t entry_num);
void dump_flightlog_leadup(BG_FlightRecorderRegistry_t* logregistry, uint64_t entry_num,uint64_t num2dump);
uint64_t log_injfifo_info (MUDM_InjFifo_t   * injfifo_ctls, BG_FlightRecorderRegistry_t * logreg, uint64_t postRAS);

typedef struct overlay_BG_FlightRecorderLog
{
    uint64_t timestamp;
    uint32_t hwthread;
    uint32_t id;
    uint32_t data[8];
} overlay_BG_FlightRecorderLog_t;


#define FlightLogSize_debug             16   // for debug parsing only, not passed to flight log dump utility 

#ifdef __LINUX__ 
#define FlightLogSize_hiwrap          4096  // 48 bytes per entry => 196608 bytes
#define FlightLogSize_nowrap           512  // 48 bytes per entry =>  24576 bytes


//handle CNK differently 
#else 
#define FlightLogSize_hiwrap          1024  // 48 bytes per entry => 49152  bytes
#define FlightLogSize_nowrap           128  // 48 bytes per entry =>  6144  bytes
#endif

extern uint64_t               FlightLock_debug;// for debug input
extern BG_FlightRecorderLog_t FlightLog_debug[FlightLogSize_debug];
extern BG_FlightRecorderRegistry_t mudm_debug_flight_recorder __attribute__ ((aligned (64)));

extern uint64_t               FlightLock_hiwrap;//data traffic at high wrap rat
extern uint64_t               FlightLock_nowrap;// for static input
extern BG_FlightRecorderLog_t FlightLog_nowrap[FlightLogSize_nowrap];
extern BG_FlightRecorderLog_t FlightLog_hiwrap[FlightLogSize_hiwrap];


extern char Flight_log_table[][256];
enum
{
    FL_INVALID=0,
#define STANDARD(name) name,
#define FLIGHTPRINT(name, format)   name,
#define FLIGHTFUNCT(name, function) name,
#include "flightlog.h"
#undef STANDARD
#undef FLIGHTPRINT
#undef FLIGHTFUNCT
    FL_NUMENTRIES
};
/*********************************/
/* Decoder definitions
**********************************/
#define FLIGHTDECODER(name) extern void name(size_t bufsize, char* buffer, const BG_FlightRecorderLog_t* logentry, void* ptr)
FLIGHTDECODER(MUDM_formatFunction);
FLIGHTDECODER(MUDM_parsePK);
FLIGHTDECODER(MUDM_parsePKbcast);
FLIGHTDECODER(MUDM_parseDirectPut);
FLIGHTDECODER(MUDM_parse_connect_state);
FLIGHTDECODER(MUDM_parseInjNetHdr);
FLIGHTDECODER(MUDM_parseC0BCASTRDMA);
FLIGHTDECODER(MUDM_personality_info);
FLIGHTDECODER(MUDM_receiveFifoInfo);
FLIGHTDECODER(MUDM_injectFifoInfo);

int dump_flight_log(uint64_t* loglock, BG_FlightRecorderLog_t* log, uint64_t logsize);
void mudm_dumphex(unsigned char *dmpPtr, int len);


__INLINE__
void log_string(uint32_t ID,uint64_t* flightlock, BG_FlightRecorderLog_t flightlog[], uint64_t flightsize, char * info){
  char cinfo[]= "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
  size_t strlength = strlen(info);
  if (strlength > 32) strlength = 32;
  snprintf(cinfo,strlength,"%s",info);
  Kernel_WriteFlightLog(flightlock, flightlog, flightsize,ID, (uint64_t)(cinfo), (uint64_t)(cinfo+8),(uint64_t)(cinfo+16),(uint64_t)(cinfo+24));
}

__INLINE__
void log_line(uint32_t ID,BG_FlightRecorderRegistry_t* logregistry, const char * info, uint64_t line){
  uint64_t val[3];
  char * cinfo = (char *)val;
  uint32_t strlength = strlen(info);
  uint64_t entry_num;
  val[0]=val[1]=val[2]=0;
  strlength++;
  if (strlength > 24) strlength = 24;
  snprintf(cinfo,strlength,"%s",info);
  entry_num = Kernel_WriteFlightLog(logregistry->flightlock, logregistry->flightlog, logregistry->flightsize,ID, line, (uint64_t)(cinfo), (uint64_t)(cinfo+8),(uint64_t)(cinfo+16));
#ifdef PRINT_FLIGHT_LOGGING
   dump_flight_entry(logregistry, entry_num );
#endif
}

__INLINE__
void WriteFlightLog64x4(uint32_t ID,BG_FlightRecorderRegistry_t* logregistry,uint64_t * data){
#ifdef PRINT_FLIGHT_LOGGING
   dump_flight_entry(logregistry,  Kernel_WriteFlightLog(logregistry->flightlock, logregistry->flightlog, logregistry->flightsize,ID, *data, *(data+1), *(data+2), *(data+3)) );
#else
   Kernel_WriteFlightLog(logregistry->flightlock, logregistry->flightlog, logregistry->flightsize,ID, *data, *(data+1), *(data+2), *(data+3)); 
#endif
}
__INLINE__
void DB_WriteFlightLog64x4(uint32_t ID,BG_FlightRecorderRegistry_t* logregistry,uint64_t * data){
   dump_flight_entry(logregistry,  Kernel_WriteFlightLog(logregistry->flightlock, logregistry->flightlog, logregistry->flightsize,ID, *data, *(data+1), *(data+2), *(data+3)) );
}

__INLINE__
void WriteFlightLogx4(uint32_t ID,BG_FlightRecorderRegistry_t* logregistry,uint64_t data0,uint64_t data1,uint64_t data2,uint64_t data3){
 
#ifdef PRINT_FLIGHT_LOGGING
   dump_flight_entry(logregistry,  Kernel_WriteFlightLog(logregistry->flightlock, logregistry->flightlog, logregistry->flightsize,ID, data0, data1, data2, data3) );
#else 
   Kernel_WriteFlightLog(logregistry->flightlock, logregistry->flightlog, logregistry->flightsize,ID, data0, data1, data2, data3); 
#endif
}

__INLINE__
uint64_t Timestamped_WriteFlightLog64x4(uint32_t ID,BG_FlightRecorderRegistry_t* logregistry,uint64_t * data){
  uint64_t entry_num = Kernel_WriteFlightLog(logregistry->flightlock, logregistry->flightlog, logregistry->flightsize,ID, *data, *(data+1), *(data+2), *(data+3));
#ifdef PRINT_FLIGHT_LOGGING
   dump_flight_entry(logregistry, entry_num );
#endif
   return logregistry->flightlog[entry_num].timestamp;
}

__INLINE__
uint64_t Timestamped_WriteFlightLogx4(uint32_t ID,BG_FlightRecorderRegistry_t* logregistry,uint64_t data0,uint64_t data1,uint64_t data2,uint64_t data3){
   uint64_t entry_num = Kernel_WriteFlightLog(logregistry->flightlock, logregistry->flightlog, logregistry->flightsize,ID, data0, data1, data2, data3);
#ifdef PRINT_FLIGHT_LOGGING
   dump_flight_entry(logregistry, entry_num );
#endif
   return logregistry->flightlog[entry_num].timestamp;
}
__INLINE__
uint64_t DB_WriteFlightLogx4(uint32_t ID,BG_FlightRecorderRegistry_t* logregistry,uint64_t data0,uint64_t data1,uint64_t data2,uint64_t data3){
   uint64_t entry_num = Kernel_WriteFlightLog(logregistry->flightlock, logregistry->flightlog, logregistry->flightsize,ID, data0, data1, data2, data3);
   dump_flight_entry(logregistry, entry_num );
   return entry_num;
}


__INLINE__
uint64_t entrynum_WriteFlightLogx4(uint32_t ID,BG_FlightRecorderRegistry_t* logregistry,uint64_t data0,uint64_t data1,uint64_t data2,uint64_t data3){
   return Kernel_WriteFlightLog(logregistry->flightlock, logregistry->flightlog, logregistry->flightsize,ID, data0, data1, data2, data3);
}

#if 1
#define LOG_DESCR_HEADER(logregistry,pointer) WriteFlightLog64x4(MU_INJNETHDR,logregistry,(uint64_t *)pointer)
#define LOG_INJECT_DESCR(logregistry,pointer) WriteFlightLog64x4(PK_INJHEADER,logregistry,(uint64_t *)pointer)
#define INJECT_DIRECTPUT(logregistry,pointer) WriteFlightLog64x4(DP_INJHEADER,logregistry,(uint64_t *)pointer)
#define INJ_MEMFIFO_CONN(logregistry,pointer) WriteFlightLog64x4(MF_INJHEADER,logregistry,(uint64_t *)pointer)
#define RCV_PACKET_DESCR(logregistry,pointer) WriteFlightLog64x4(PK_RCVHEADER,logregistry,(uint64_t *)pointer)

//DEBUG
#define DB_DESCR_HEADER_STUCK(logregistry,pointer) DB_WriteFlightLog64x4(DB_NH__STUCK,logregistry,(uint64_t *)pointer)
#define DB_INJECT_DESCR_STUCK(logregistry,pointer) DB_WriteFlightLog64x4(DB_PK__STUCK,logregistry,(uint64_t *)(pointer))

#define LOG_MF_TEMPLATE(logregistry,pointer) DB_WriteFlightLog64x4(DB_MF_TMPLAT,logregistry,(uint64_t *)pointer)
#define LOG_DP_TEMPLATE(logregistry,pointer) DB_WriteFlightLog64x4(DB_DP_TMPLAT,logregistry,(uint64_t *)pointer)
#define LOG_RGET_DP_TEMPLATE(logregistry,pointer) DB_WriteFlightLog64x4(DB_RGETDP_TM,logregistry,(uint64_t *)pointer)

#define DB_INJECT_FIFO_POINTERS(logregistry,data0,data1,data2,data3) DB_WriteFlightLogx4(DB_IFIFO_PTR,logregistry,(uint64_t)data0,(uint64_t)data1,(uint64_t)data2,(uint64_t)data3)
#define DB_INJECT_FIFO_NUMBERS(logregistry,data0,data1,data2,data3) DB_WriteFlightLogx4(DB_IFIFO_NUM,logregistry,(uint64_t)data0,(uint64_t)data1,(uint64_t)data2,(uint64_t)data3)
#define DB_RAW_DATA(logregistry,pointer) DB_WriteFlightLog64x4(DB_RAWPKTDTA,logregistry,(uint64_t *)pointer)

#define DB_RECV_CALLBACK(logregistry,data0,data1,data2,data3) DB_WriteFlightLogx4(DB_RECVBADRC,logregistry,(uint64_t)data0,(uint64_t)data1,(uint64_t)data2,(uint64_t)data3)
//FLIGHTPRINT(DB_RECVPKTQQ,"pointer=%p, allocated=%llu, free=%llu, active=%llu")
#define DB_PKT_CHECK(logregistry,data0,data1,data2,data3) DB_WriteFlightLogx4(DB_RECVPKTQQ,logregistry,(uint64_t)data0,(uint64_t)data1,(uint64_t)data2,(uint64_t)data3)

#define PKT_CHECK(logregistry,data0,data1,data2,data3) WriteFlightLogx4(DB_RECVPKTQQ,logregistry,(uint64_t)data0,(uint64_t)data1,(uint64_t)data2,(uint64_t)data3)

#define DB_PKT_CHECK_PREV(logregistry,data0,data1,data2,data3) DB_WriteFlightLogx4(DB_RECVPKTQQ,logregistry,(uint64_t)data0,(uint64_t)data1,(uint64_t)data2,(uint64_t)data3)



//RDMA 
#define RDMA_COUNTER_HIT(logregistry,obj)  WriteFlightLogx4(C0_RDMACOUNT,logregistry,obj->local_request_id, obj->rdma_object_pa,(uint64_t)obj->local_connect,obj->mue->error)
#define RDMA_COUNTER_STUCK(logregistry,obj)  DB_WriteFlightLogx4(C0_RDMASTUCK,logregistry,obj->local_request_id, obj->rdma_object_pa,obj->local_connect,obj->error)

//Broadcasts
#define INJ_MEMFIFO_BCAST(logregistry,pointer) WriteFlightLog64x4(MF_INJBCAST_,logregistry,(uint64_t *)pointer)
#define INJ_REDUCE_BCAST(logregistry,pointer)  WriteFlightLog64x4(RD_INJBCAST_,logregistry,(uint64_t *)pointer)
#define RCV_BCAST_DESCR(logregistry,pointer)   WriteFlightLog64x4(PK_RCVBCAST_,logregistry,(uint64_t *)pointer)
#define INJ_RDMA_BCAST_DPUT(logregistry,pointer)  WriteFlightLog64x4(DP_BCASTRDMA,logregistry,(uint64_t *)pointer)

//Monitoring state, etc.
#define MUDM_CONN_RCV_PAYLOAD(logregistry,pointer) WriteFlightLog64x4(CN_RCV_PAYLO,logregistry,(uint64_t *)pointer) 
#define MUDM_CONN_INJ_PAYLOAD(logregistry,pointer) WriteFlightLog64x4(CN_INJ_PAYLO,logregistry,(uint64_t *)pointer)

#define MUDM_CONN_GETFREE(logregistry,pointer) Timestamped_WriteFlightLog64x4(CN_GETFREE__,logregistry,(uint64_t *)pointer)
#define MUDM_STATUS_CONNECTION(logregistry,pointer) DB_WriteFlightLog64x4(CN_STATUS___,logregistry,(uint64_t *)pointer)
#define MUDM_CONN_START(logregistry,pointer) Timestamped_WriteFlightLog64x4(CN_STARTING_,logregistry,(uint64_t *)pointer)
#define MUDM_CONN_ERROR(logregistry,data0,data1,data2,data3)  WriteFlightLogx4(CN_START_ERR,logregistry,data0,data1,data2,data3)
#define MUDM_DISC_ERROR(logregistry,data0,data1,data2,data3)  WriteFlightLogx4(CN_DISC__ERR,logregistry,data0,data1,data2,data3)
#define MUDM_FREED_CONNECTION(logregistry,pointer) WriteFlightLog64x4(CN_FREED_CTX,logregistry,(uint64_t *)pointer)
#define MUDM_DISC_INDICATION(logregistry,pointer) WriteFlightLog64x4(CN_DISC_INDI,logregistry,(uint64_t *)pointer)

#define MUDM_BAD_RECV_CONN_CALLBACK(logregistry,d0,d1,d2,d3) WriteFlightLogx4(CN_BDCALLBAK,logregistry,(uint64_t)d0,(uint64_t)d1,(uint64_t)d2,(uint64_t)d3)
#define MUDM_CONN_LIST(logregistry,d0,d1,d2,d3) WriteFlightLogx4(CN_CONNLISTS,logregistry,(uint64_t)d0,(uint64_t)d1,(uint64_t)d2,(uint64_t)d3)
#define MUDM_IO_LINKRESET(logregistry,d0,d1,d2,d3) entrynum_WriteFlightLogx4(IO_LINKRESET,logregistry,(uint64_t)d0,(uint64_t)d1,(uint64_t)d2,(uint64_t)d3)
#define MUDM_IO_LINKSKIPPED(logregistry,d0,d1,d2,d3) entrynum_WriteFlightLogx4(IO_LINKSKIPD,logregistry,(uint64_t)d0,(uint64_t)d1,(uint64_t)d2,(uint64_t)d3)
#define MUDM_IO_LINKFLUSHED(logregistry,d0,d1,d2,d3) entrynum_WriteFlightLogx4(IO_LINKFLUSH,logregistry,(uint64_t)d0,(uint64_t)d1,(uint64_t)d2,(uint64_t)d3)
#define MUDM_IO_LINKRSETC(logregistry,d0,d1,d2,d3) entrynum_WriteFlightLogx4(IO_LINKRSETC,logregistry,(uint64_t)d0,(uint64_t)d1,(uint64_t)d2,(uint64_t)d3)

#define MUDM_TERM_START(logregistry,d0,d1,d2,d3) Timestamped_WriteFlightLogx4(DM_TERMSTART,logregistry,(uint64_t)d0,(uint64_t)d1,(uint64_t)d2,(uint64_t)d3)

#define MUDM_MU_PKT_2POLL(logregistry,d0,d1,d2,d3) WriteFlightLogx4(MU_PKT_2POLL,logregistry,(uint64_t)d0,(uint64_t)d1,(uint64_t)d2,(uint64_t)d3)
#define MUDM_MU_PKT_PCOMP(logregistry,d0,d1,d2,d3) WriteFlightLogx4(MU_PKT_PCOMP,logregistry,(uint64_t)d0,(uint64_t)d1,(uint64_t)d2,(uint64_t)d3)

#define MUDM_STUCK_PKT(logregistry,d0,d1,d2,d3)     entrynum_WriteFlightLogx4(MU_STUCK_PKT,logregistry,(uint64_t)d0,(uint64_t)d1,(uint64_t)d2,(uint64_t)d3)
#define MUDM_ERROR_HIT(logregistry,d0,d1,d2,d3)  WriteFlightLogx4(DM_ERROR_HIT,logregistry,(uint64_t)d0,(uint64_t)d1,(uint64_t)d2,(uint64_t)d3)
#define MUDM_REQUEST_CANCELED(logregistry,d0,d1,d2,d3)  WriteFlightLogx4(RQ_CANCELED_,logregistry,(uint64_t)d0,(uint64_t)d1,(uint64_t)d2,(uint64_t)d3)

#define MUDM_LOG_RDMA_READ(logregistry,d0,d1,d2,d3) WriteFlightLogx4(CN_CALL_READ,logregistry,(uint64_t)d0,(uint64_t)d1,(uint64_t)d2,(uint64_t)d3)
#define MUDM_LOG_RDMA_WRITE(logregistry,d0,d1,d2,d3) WriteFlightLogx4(CN_CALLWRITE,logregistry,(uint64_t)d0,(uint64_t)d1,(uint64_t)d2,(uint64_t)d3)
#define MUDM_LOG_RDMA_WRITE_BCAST(logregistry,d0,d1,d2,d3) WriteFlightLogx4(RQ_BCASTRDMA,logregistry,(uint64_t)d0,(uint64_t)d1,(uint64_t)d2,(uint64_t)d3)
#define MUDM_LOG_RDMA_C0_BCAST(logregistry,d0,d1,d2,d3) WriteFlightLogx4(C0_BCASTRDMA,logregistry,(uint64_t)d0,(uint64_t)d1,(uint64_t)d2,(uint64_t)d3)

#define MUDM_CN_TOOK2LONG(logregistry,pointer) DB_WriteFlightLog64x4(CN_TOOK2LONG,logregistry,(uint64_t *)pointer)
#define MUDM_DM_USEC_DIFF(logregistry,data0,data1,data2,data3)  WriteFlightLogx4(DM_USEC_DIFF,logregistry,data0,data1,data2,data3)

//no-wrap static entries 
#define MUDM_INIT_DONE(logregistry,d0,d1,d2,d3) entrynum_WriteFlightLogx4(DM_INITDONE_,logregistry,(uint64_t)d0,(uint64_t)d1,(uint64_t)d2,(uint64_t)d3)
#define MUDM_DB_RECF_INFO(logregistry,d0,d1,d2,d3) entrynum_WriteFlightLogx4(DB_RECF_INFO,logregistry,(uint64_t)d0,(uint64_t)d1,(uint64_t)d2,(uint64_t)d3)
#define MUDM_DB_INJF_INFO(logregistry,d0,d1,d2,d3) entrynum_WriteFlightLogx4(DB_INJF_INFO,logregistry,(uint64_t)d0,(uint64_t)d1,(uint64_t)d2,(uint64_t)d3)
#define MUDM_DM_PERS_INFO(logregistry,d0,d1,d2,d3) entrynum_WriteFlightLogx4(DB_INJF_INFO,logregistry,(uint64_t)d0,(uint64_t)d1,(uint64_t)d2,(uint64_t)d3)

#define RESET_SEQNUM_TO_ZERO(logregistry,obj)  DB_WriteFlightLogx4(DB_RESETTO_0,logregistry,__LINE__, obj->memfifo_sent,(LLUS)obj,obj->my_index)

#endif

#endif /* _MUDM_TRACE_H_ */
