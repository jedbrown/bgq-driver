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

#ifndef	_KERNEL_IMPL_TRACE_H_ /* Prevent multiple inclusion */
#define	_KERNEL_IMPL_TRACE_H_

#include "kernel_impl.h" 
#include "hwi/include/common/compiler_support.h"
//#include "hwi/include/bqc/A2_core.h"

#define  SPRN_TBRO                (0x10C)          // Time Base 64-bit                             User Read-only
#ifndef SPRN_PIR
#define SPRN_PIR                  (0x11E)          // Processor ID Register (See GPIR)
#endif

__BEGIN_DECLS

__INLINE__
    uint64_t Kernel_WriteFlightLog(uint64_t* flightlock, BG_FlightRecorderLog_t flightlog[], uint64_t flightsize,
                               uint32_t ID, uint64_t data0, uint64_t data1, uint64_t data2, uint64_t data3)
{
    uint64_t myentry;
    uint64_t pir;
    uint64_t timebase;
    asm volatile("1: ldarx   %0,0,%3;"
                 "addi 3, %0, 1;"
                 "stdcx. 3, 0, %3;"
                 "bne 1b;"
                 "mfspr %1,%4;"
                 "mfspr %2,%5;"
                 : "=&b" (myentry), "=&r" (timebase), "=&r" (pir) : "b" (flightlock), "i" (SPRN_TBRO), "i" (SPRN_PIR) : "cc", "memory", "r3");

    myentry %= flightsize;
    flightlog[myentry].timestamp = timebase;
    flightlog[myentry].id      = ID;
    flightlog[myentry].hwthread= pir&0x3ff;
    flightlog[myentry].data[0] = data0;
    flightlog[myentry].data[1] = data1;
    flightlog[myentry].data[2] = data2;
    flightlog[myentry].data[3] = data3;
    return myentry;
}

__INLINE__
int Kernel_DecodeFlightLogs(uint32_t numlogs, BG_FlightRecorderRegistry_t* logregistry, size_t bufferSize, char* buffer, uint64_t* moreData)
{
    int      set;
    uint64_t x;
    uint64_t l;
    uint64_t lsel;
    uint64_t length;
    uint64_t nexttime;
    uint64_t offset;
    uint64_t count[8];
    uint64_t counttotal[8];
    BG_FlightRecorderFormatter_t* fmt;
    BG_FlightRecorderLog_t* log;
    
    // Acquire current flight recorder lock pointers
    for(l=0; l<numlogs; l++)
    {
        if(logregistry[l].lastStateSet == 0)
        {
#if 0
            offset = L2_AtomicLoad(logregistry[l].flightlock);
            count[l] = offset % logregistry[l].flightsize;
#else
            offset = *logregistry[l].flightlock;
            count[l] = offset % logregistry[l].flightsize;
#endif
            logregistry[l].lastOffset = offset;
            counttotal[l] = 0;
            
            for(x=0; x<logregistry[l].flightsize; x++)
            {
                if((logregistry[l].flightlog[ count[l] ].id) != 0)
                    break;
                count[l] = (count[l]+1) % logregistry[l].flightsize;
                counttotal[l]++;
            }
        }
        else
        {
            count[l] = logregistry[l].lastState;
            counttotal[l] = logregistry[l].lastStateTotal;
        }
    }
    
    do
    {
        *moreData = 0;
        nexttime = 0xffffffffffffffffull;
        lsel     = 0xffffffffffffffffull;
        for(l=0; l<numlogs; l++)
        {
            if((logregistry[l].flightlog[count[l]].timestamp < nexttime) && (counttotal[l] < logregistry[l].flightsize))
            {
                nexttime = logregistry[l].flightlog[count[l]].timestamp;
                lsel = l;
            }
        }
        
        if(lsel != 0xffffffffffffffffull)
        {
            *moreData = logregistry[lsel].lastOffset + counttotal[lsel] + (lsel<<48) + 1;
            if(bufferSize < 256)
            {
                return 0;
            }
            
            set = logregistry[lsel].lastStateSet;
            log = &logregistry[lsel].flightlog[count[lsel]];
            count[lsel] = (count[lsel]+1) % logregistry[lsel].flightsize;
            counttotal[lsel]++;
            logregistry[lsel].lastStateSet = 1;
            logregistry[lsel].lastState = count[lsel];
            logregistry[lsel].lastStateTotal = counttotal[lsel];
            
            if(log->id >= logregistry[lsel].num_ids)
            {
                length = snprintf(buffer, bufferSize, "TB=%016llx FL_INVALDLOG:%-2d An invalid entry with registry=\"%s\"  id=%d was detected (valid ID range 0-%d)\n", log->timestamp, log->hwthread, logregistry[lsel].registryName, log->id, logregistry[lsel].num_ids);
                bufferSize -= length;
                buffer     += length;
                return 0;
            }
            
            fmt = &logregistry[lsel].flightformatter[log->id];
            if(set == 0)
            {
                length = snprintf(buffer, bufferSize, "TB=%016llx FL_BEGIN_LOG:-- Starting log \"%s\"\n", log->timestamp, logregistry[lsel].registryName);
                bufferSize -= length;
                buffer     += length;
            }
            
            length = snprintf(buffer, bufferSize, "TB=%016llx %s:%-2d ", log->timestamp, fmt->id_str, log->hwthread);
            bufferSize -= length;
            buffer     += length;
            
            switch(fmt->type)
            {
                case FLIGHTRECORDER_PRINTF:
                    length = snprintf(buffer, bufferSize, fmt->formatString, log->data[0], log->data[1], log->data[2], log->data[3]);
                    bufferSize -= length;
                    buffer += length;
                    break;
                case FLIGHTRECORDER_FUNC:
                    fmt->formatFunction(bufferSize, buffer, log, fmt->anchorPtr);
                    length = strlen(buffer);
                    bufferSize -= length;
                    buffer     += length;
                    break;
                default:
                    break;
            }
        }
    }
    while(*moreData);
    return 0;
}


__END_DECLS

#endif /* _KERNEL_TRACE_H_ */
