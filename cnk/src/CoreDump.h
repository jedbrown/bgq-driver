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

//! \file  CoreDump.h

#ifndef COREDUMP_H
#define COREDUMP_H

typedef struct _CoreBuffer 
{
    char buffer[CONFIG_CORE_BUFFER_SIZE]; //! We also use this buffer for Elf structures in binary core dumps.  Is it big enough?
    int length; // length of the dat 
    struct 
    {
        unsigned int dumpToMailbox : 1;
        unsigned int disableBuffering : 1;
        unsigned int dumpTLBs : 1;
        unsigned int dumpRegisters : 1;
        unsigned int dumpInterruptCounters : 1;
        unsigned int dumpPersonality : 1;
        unsigned int dumpStack : 1;                // Raw stack dump
        unsigned int dumpFPR : 1;
        unsigned int dumpGPR : 1;
        unsigned int dumpSPR : 1;
        unsigned int dumpBinary : 1;
    } flags;
    int fileFd;
    bool dumpSegment[IS_SEGMENTTYPECOUNT];
} CoreBuffer;


void DumpCore(void);
void DumpInterruptCounters(void);
int  coredump_binary_for_rank(AppProcess_t* pProc);
void coredump_printf(CoreBuffer* buffer, const char *fmt, ...);
void coredump_ND(CoreBuffer*);
int  coredump_for_rank(AppProcess_t* pProc);

#endif // COREDUMP_H
