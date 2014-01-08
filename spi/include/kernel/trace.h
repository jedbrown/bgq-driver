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
/* (C) Copyright IBM Corp.  2008, 2012                              */
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

#ifndef	_KERNEL_TRACE_H_ /* Prevent multiple inclusion */
#define	_KERNEL_TRACE_H_

#include "kernel_impl.h" 
#include "hwi/include/common/compiler_support.h"


#ifndef __INLINE__
#define __INLINE__ extern inline
#endif

/*!
 * \brief Flight recorder log entry
 */
typedef struct BG_FlightRecorderLog
{
    uint64_t timestamp;
    uint32_t hwthread;
    uint32_t id;
    uint64_t data[4];
} BG_FlightRecorderLog_t;

typedef enum
{
    FLIGHTRECORDER_PRINTF=0x100,
    FLIGHTRECORDER_FUNC
} BG_FlightRecorderFormatterType_t;

typedef void (*BG_FlightRecorderFunc_t)(size_t bufsize, char* buffer, const BG_FlightRecorderLog_t* logentry, void* extraDataPtr);

typedef struct BG_FlightRecorderFormatter
{
    const char* id_str;
    BG_FlightRecorderFormatterType_t type;
    const char*              formatString;
    BG_FlightRecorderFunc_t  formatFunction;
    void*                    anchorPtr;
} BG_FlightRecorderFormatter_t;

typedef struct BG_FlightRecorderRegistry
{
    BG_FlightRecorderLog_t*       flightlog;
    uint64_t                      flightsize;
    uint64_t*                     flightlock;
    BG_FlightRecorderFormatter_t* flightformatter;
    uint32_t                      num_ids;
    uint32_t                      lastStateSet;
    uint32_t                      lastState;
    uint32_t                      lastStateTotal;
    uint64_t                      lastOffset;
    const char*                   registryName;
} BG_FlightRecorderRegistry_t;


__BEGIN_DECLS

/*!
 * \brief Write information to a lightweight flight recorder log
 *
 * The following SPI routine is intended to provide a lightweight means of writing data into
 * a log.  The implementation should ideally make use of BGQ's L2 atomic support.
 *
 * The buffer is circular, so once 'flightsize' entries have been created, the next used log
 * entry is 0.  The timebase field in BG_FlightRecorderLog_t can be used to sort the entries
 * in a chronological order.  
 *
 * \param[in]   flightlock  Pointer to some memory for atomic access
 * \param[out]  flightlog   Array of log entries
 * \param[in]   flightsize  The number of log entries
 * \param[in]   ID          A unique identifier for the entry.  Caller defines what this means.
 * \param[in]   data0       Caller-specific data field
 * \param[in]   data1       Caller-specific data field
 * \param[in]   data2       Caller-specific data field
 * \param[in]   data3       Caller-specific data field
 *
 */
__INLINE__
uint64_t  Kernel_WriteFlightLog(uint64_t* flightlock, BG_FlightRecorderLog_t flightlog[], uint64_t flightsize, 
                           uint32_t ID, uint64_t data0, uint64_t data1, uint64_t data2, uint64_t data3);

/*!
 * \brief Decode the flight recorder into a buffer (for subsequent printing/fileoutput/post-processing by the calling function)
 */

__INLINE__
int Kernel_DecodeFlightLogs(uint32_t numlogs, BG_FlightRecorderRegistry_t* logregistry, size_t bufferSize, char* buffer, uint64_t* moreData);

/*!
 * \brief Snapshot the current flight recorder log and place it into a buffer.
 */
__INLINE__
int Kernel_GetFlightLog(size_t bufferSize, char* buffer);

#include "trace_impl.h"

__END_DECLS

#endif /* _KERNEL_TRACE_H_ */
