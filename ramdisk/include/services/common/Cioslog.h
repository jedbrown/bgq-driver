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

//! \file  Cioslog.h
//! \brief Declaration and inline methods for bgcios::jobctl::Job class.

#ifndef CIOSLOG_H
#define CIOSLOG_H
#include <ramdisk/include/services/MessageHeader.h>

extern char Flight_log_table[][256];
enum
{
    FL_INVALID=0,
#define STANDARD(name) name,
#define FLIGHTPRINT(name, format)   name,
#define FLIGHTFUNCT(name, function) name,
#include <ramdisk/include/services/common/flightlog.h>
#undef STANDARD
#undef FLIGHTPRINT
#undef FLIGHTFUNCT
    FL_NUMENTRIES
};

typedef struct BG_FlightRecorderFormatter
{
    const char* id_str;
} BG_FlightRecorderFormatter_t;

//#define CIOSLOGMSG(ID,msg) printMsg(ID,(bgcios::MessageHeader *)msg)
#define CIOSLOGMSG(ID,msg) logMsg(ID,(bgcios::MessageHeader *)msg)
#define CIOSLOGMSG_RECV_WC(ID,msg,completion) logMsgWC(ID,(bgcios::MessageHeader *)msg, (struct ibv_wc *)completion)
#define CIOSLOGMSG_QP(ID,msg,qpNum) logMsgQpNum(ID,(bgcios::MessageHeader *)msg, qpNum)

void printMsg(const char * ID, bgcios::MessageHeader *mh);

typedef union cios_connection {
  uint32_t BGV_recv[2];
  uint64_t other;
} cios_connection_t;


typedef struct BG_FlightRecorderLog
{
    uint32_t entry_num;
    uint32_t id;
    cios_connection_t ci;
    uint64_t data[4];
} BG_FlightRecorderLog_t;

#define FlightLogSize         4096 

uint32_t logMsg(uint32_t ID,bgcios::MessageHeader *mh);
uint32_t logMsgWC(uint32_t ID,bgcios::MessageHeader *mh,struct ibv_wc *wc);
uint32_t logMsgQpNum(uint32_t ID,bgcios::MessageHeader *mh,uint32_t qp_num);
void printLogMsg(const char * pathname);

#endif

