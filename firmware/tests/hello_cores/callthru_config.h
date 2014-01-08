/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q                                                      */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/* US Government Users Restricted Rights - Use, duplication or      */
/*   disclosure restricted by GSA ADP Schedule Contract with IBM    */
/*   Corp.                                                          */
/*                                                                  */
/* This software is available to you under the Eclipse Public       */
/* License (EPL).                                                   */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/* -------------------------------------------------------
 *
 * Licensed Materials - Property of IBM.
 * (C) Copyright IBM Corporation 2001, 2008
 * All Rights Reserved.
 *
 * US Government Users Restricted Rights -
 * Use, duplication or disclosure restricted by
 * GSA ADP Schedule Contract with IBM Corporation.
 *
 * Filename : callthru_config.h
 *
 * Purpose  :   */
/** \file
 *
 *   \brief
 *   Mambo callthru functions that can be invoked by the simulated program.
 *
 *   This file is compiled with programs that are run under the Mambo
 *   simulator.  For example, stand-alone programs or operating
 *   systems.  (This file is not compiled into Mambo!)  The programs
 *   call the callthru wrapper functions which use an illegal PowerPC
 *   instruction to signal the Mambo simulator to emulate special
 *   support.
 */
/* ------------------------------------------------------- */

#ifndef _CALLTHRU_CONFIG_H_
#define _CALLTHRU_CONFIG_H_

#include "sim_support_code.h"

#ifndef VOID
#define VOID char
#endif /* #ifndef VOID */
#ifndef CAST
#define CAST(t,e) ((t)(e))
#endif /* #ifndef CAST */

/* The AIX xlc compiler does not seem to like the gcc
   "static inline" declaration. */
#if defined(_AIX) && defined(__IBMC__)
#define INLINE static
#else /* #if defined(_AIX) && defined(__IBMC__) */
#define INLINE static inline
#endif /* #if defined(_AIX) && defined(__IBMC__) */

/* The functions callthru0 to callthru7 setup up the arguments for the
 * Mambo callthru and then use the callthru instruction.  Note that
 * 0-7 specify the number of arguments after the command */

/* Note: Arguments are cast as unsigned long to prevent casting by the
   compiler.  This way, you can pass pointers, integers, etc. in
   machine register and have the Mambo simulator interpret what the
   register is supposed to be.  To help with typing errors when using
   callthrus, we provide wrapper functions for each callthru.  The
   wrappers cast all arguments to unsigned long.  Unfortunately, this results
   in a lot of compiler warnings that I do not know how to remove.  If
   you modify this code, be aware that we are trying to pick a type
   that is the size of the registers (32-bit or 64-bit) and that is
   why are choosing to cast to a VOID *(it should be the size of a
   machine register) */

#ifdef __GNUC__

INLINE long callthru0(int command)
{
    register long c asm ("r3") = command;
    asm volatile (".long 0x000EAEB0" : "=r" (c): "r" (c) : "memory");
    return((c));
}
INLINE long callthru1(int command, unsigned long arg1)
{
    register long c asm ("r3") = command;
    register unsigned long a1 asm ("r4") = arg1;
    asm volatile (".long 0x000EAEB0" : "=r" (c): "r" (c), "r" (a1) : "memory");
    return((c));
}
INLINE long callthru2(int command, unsigned long arg1, unsigned long arg2)
{
    register long c asm ("r3") = command;
    register unsigned long a1 asm ("r4") = arg1;
    register unsigned long a2 asm ("r5") = arg2;
    asm volatile (".long 0x000EAEB0" : "=r" (c): "r" (c), "r" (a1), "r" (a2) : "memory");
    return((c));
}
INLINE long callthru3(int command, unsigned long arg1, unsigned long arg2, unsigned long arg3)
{
    register long c asm ("r3") = command;
    register unsigned long a1 asm ("r4") = arg1;
    register unsigned long a2 asm ("r5") = arg2;
    register unsigned long a3 asm ("r6") = arg3;
    asm volatile (".long 0x000EAEB0" : "=r" (c): "r" (c), "r" (a1), "r" (a2), "r" (a3) : "memory");
    return((c));
}
INLINE long callthru4(int command, unsigned long arg1, unsigned long arg2, unsigned long arg3, unsigned long arg4)
{
    register long c asm ("r3") = command;
    register unsigned long a1 asm ("r4") = arg1;
    register unsigned long a2 asm ("r5") = arg2;
    register unsigned long a3 asm ("r6") = arg3;
    register unsigned long a4 asm ("r7") = arg4;
    asm volatile (".long 0x000EAEB0" : "=r" (c): "r" (c), "r" (a1), "r" (a2), "r" (a3), "r" (a4) : "memory");
    return((c));
}
INLINE long callthru5(int command, unsigned long arg1, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5)
{
    register long c asm ("r3") = command;
    register unsigned long a1 asm ("r4") = arg1;
    register unsigned long a2 asm ("r5") = arg2;
    register unsigned long a3 asm ("r6") = arg3;
    register unsigned long a4 asm ("r7") = arg4;
    register unsigned long a5 asm ("r8") = arg5;
    asm volatile (".long 0x000EAEB0" : "=r" (c): "r" (c), "r" (a1), "r" (a2), "r" (a3), "r" (a4), "r" (a5) : "memory");
    return((c));
}
INLINE long callthru6(int command, unsigned long arg1, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5, unsigned long arg6)
{
    register long c asm ("r3") = command;
    register unsigned long a1 asm ("r4") = arg1;
    register unsigned long a2 asm ("r5") = arg2;
    register unsigned long a3 asm ("r6") = arg3;
    register unsigned long a4 asm ("r7") = arg4;
    register unsigned long a5 asm ("r8") = arg5;
    register unsigned long a6 asm ("r9") = arg6;
    asm volatile (".long 0x000EAEB0" : "=r" (c): "r" (c), "r" (a1), "r" (a2), "r" (a3), "r" (a4), "r" (a5), "r" (a6) : "memory");
    return((c));
}
INLINE long callthru7(int command, unsigned long arg1, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5, unsigned long arg6, unsigned long arg7)
{
    register long c asm ("r3") = command;
    register unsigned long a1 asm ("r4") = arg1;
    register unsigned long a2 asm ("r5") = arg2;
    register unsigned long a3 asm ("r6") = arg3;
    register unsigned long a4 asm ("r7") = arg4;
    register unsigned long a5 asm ("r8") = arg5;
    register unsigned long a6 asm ("r9") = arg6;
    register unsigned long a7 asm ("r10") = arg7;
    asm volatile (".long 0x000EAEB0" : "=r" (c): "r" (c), "r" (a1), "r" (a2), "r" (a3), "r" (a4), "r" (a5), "r" (a6), "r" (a7) : "memory");
    return((c));
}

#elif defined(__IBMC__)

#pragma mc_func cthru_mambo { "000EAEB0" } /* mambo */

INLINE long callthru0(int command)
{
    return(cthru_mambo(command));
}
INLINE long callthru1(int command, unsigned long arg1)
{
    return(cthru_mambo(command, arg1));
}
INLINE long callthru2(int command, unsigned long arg1, unsigned long arg2)
{
    return(cthru_mambo(command, arg1, arg2));
}
INLINE long callthru3(int command, unsigned long arg1, unsigned long arg2, unsigned long arg3)
{
    return(cthru_mambo(command, arg1, arg2, arg3));
}
INLINE long callthru4(int command, unsigned long arg1, unsigned long arg2, unsigned long arg3, unsigned long arg4)
{
    return(cthru_mambo(command, arg1, arg2, arg3, arg4));
}
INLINE long callthru5(int command, unsigned long arg1, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5)
{
    return(cthru_mambo(command, arg1, arg2, arg3, arg4, arg5));
}
INLINE long callthru6(int command, unsigned long arg1, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5, unsigned long arg6)
{
    return(cthru_mambo(command, arg1, arg2, arg3, arg4, arg5, arg6));
}
INLINE long callthru7(int command, unsigned long arg1, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5, unsigned long arg6, unsigned long arg7)
{
    return(cthru_mambo(command, arg1, arg2, arg3, arg4, arg5, arg6, arg7));
}

#else /* #ifdef __GNUC__ */

#error Unsupported compiler (need GNUC or IBMC)

#endif /* #ifdef __GNUC__ */


/* Wrappers for simulator callthrus */

/**
 *  Write a string to the console.
 *
 * \param string  string written to Mambo's output console.
 */
INLINE int MamboWriteConsole(char *string)
{
    return(callthru3(SimWriteConsoleCode, CAST(unsigned long,string), CAST(unsigned long,strlen(string)), CAST(unsigned long,1)));
}




/**
 * Associate an operating system interrupt
 * "level" number with a device type.
 *
 *  \param  id    device type code
 *  \param  level interrupt level number
 */
INLINE int MamboDeviceIntRegister(SimDeviceID id, int level)
{
    return(callthru2(SimDeviceIntRegisterCode, CAST(unsigned long,id), CAST(unsigned long,level)));
}

/**
 * Disassociate an operating system interrupt
 * "level" number with a device type.
 *
 *  \param  id  device type code
 */
INLINE int MamboDeviceIntDeregister(SimDeviceID id)
{
    return(callthru1(SimDeviceIntDeregisterCode, CAST(unsigned long,id)));
}

/**
 * Return number of interrupting
 * stream/console/actual device
 * for the interrupting level
 *
 *  \param  id   device type code
 */
INLINE int MamboGetIntStreamForDevice(SimDeviceID id)
{
    return(callthru1(SimGetIntStreamForDeviceCode, CAST(unsigned long,id)));
}

/**
 * Tell the OS which interrupt level is causing an interrupt
 */
INLINE int MamboGetInterruptingLevel(void)
{
    return(callthru0(SimGetInterruptingLevelCode));
}

/**
 *  Set mask bits for external interrupt levels
 *
 *  \param  mask   new mask value     (1s mean a level is enabled)
 */
INLINE int MamboSetExternalLevelMask(unsigned long mask)
{
    return(callthru1(SimSetExternalLevelMaskCode, mask));
}

/**
 *  Turn on pending interrupt flag bit(s)
 *
 *  \param  mask  Mask to OR with pending_external_interrupts
 */
INLINE int MamboSetInterrupt(unsigned long mask)
{
    return(callthru1(SimSetInterruptCode, mask));
}

/**
 *  Set the processor mask field for a given level
 *
 *  \param  level    level number (0..31)
 *  \param  mask     new mask value      (1s mean a processor can receive ints on that level)
 */
INLINE int MamboMapLevel(int level, unsigned int mask)
{
    return(callthru2(SimMapLevelCode, CAST(unsigned long,level), CAST(unsigned long,mask)));
}

/**
 * Turn off pending interrupt flag bit(s)
 *
 *  \param  mask   Mask to be complemented and ANDed with pending_external_interrupts
 */
INLINE int MamboResetInterrupt(unsigned long mask)
{
    return(callthru1(SimResetInterruptCode, mask));
}

/**
 * Open a file on the simulator side
 *
 * File copy between simulator <-> Simulated OS, for debugging and bring-up.
 *
 * For these routines, the interface is via a one-page buffer in
 * simulated user space.  The caller has pinned this page via a
 * call to the simulated OS.  The buffer is on a page boundary,
 * and so does not span multiple pages.  simulator can address
 * the buffer via a single call to address translation.
 *
 *  \param  filename virtual address of buffer containing file name
 *  \param  rw   char indicating how to open file:  'r' for read, 'w' for write(a la fopen)
 *  \return =0, success; =-1, failure
 */
INLINE int MamboFileOpen(char *filename, char rw)
{
    return(callthru2(SimFileOpenCode, CAST(unsigned long,filename), CAST(unsigned long,rw)));
}

/**
 * Close the open file
 *
 * There may be only one file open at a time.
 */
INLINE int MamboFileClose(void)
{
    return(callthru0(SimFileCloseCode));
}

/**
 * Read from the open file
 *
 * Read up to 4096 bytes of current open file.  Call multiple times to read entire file.
 *
 *  \param  buffer   virtual address of 4096-byte buffer on a page boundary
 *
 *  \return number of bytes read, or 0 if end-of-file reached
 */
INLINE int MamboFileRead(VOID *buffer)
{
    return(callthru1(SimFileReadCode, CAST(unsigned long,buffer)));
}
/**
 * Write to the open file
 *
 *  \param  buffer   virtual address of 4096-byte buffer on a page boundary
 *  \param  length   number of bytes to write
 *
 *  \return number of bytes written, or 0 if error
 */
INLINE int MamboFileWrite(VOID *buffer, unsigned int length)
{
    return(callthru2(SimFileWriteCode, CAST(unsigned long,buffer), CAST(unsigned long,length)));
}

/**
 * Extended File support functions : more functionality than the
 * generic sim_support file utilities including support for multiple
 * open files, variable length read/writes, error checking etc.
 * Due to differences in API, these have been separated out.
 */

/**
 * Open a file on the host filesystem
 *
 *  \param  filename virtual address of buffer containing file name
 *  \param  rw   char indicating how to open file:  'r' for read, 'w' for write(a la fopen)
 *  \return a file handle which can be passed on subsequent read / write / close operations
 */
INLINE FILE *MamboFileOpenExtended(char *filename, char* rw)
{
    return(CAST(FILE *, callthru2(SimFileOpenExtendedCode, CAST(unsigned long,filename), CAST(unsigned long,rw))));
}

/**
 * Close an open file
 *
 *  \param  fp a file handle returned from an earlier call to MamboFileOpenExtended
 *  \return 0 for success, otherwise error
 */
INLINE int MamboFileCloseExtended(FILE *fp)
{
    return(callthru1(SimFileCloseExtendedCode, CAST(unsigned long,fp)));
}

/**
 * Read from an open file
 *
 * Read nmemb elements of size size from file stream into buffer at location ptr.
 *
 *  \param  ptr virtual address of buffer to receive file data
 *  \param  size of elements to read from file
 *  \param  mmemb number of elements to read from file
 *  \param  stream a file handle returned from an earlier call to MamboFileOpenExtended
 *  \return number of elements read, or 0 if end-of-file reached
 */
INLINE size_t MamboFileReadExtended(VOID *ptr, size_t size, size_t nmemb, FILE *stream)
{
    return(callthru4(SimFileReadExtendedCode, CAST(unsigned long,ptr),CAST(unsigned long,size),CAST(unsigned long,nmemb),CAST(unsigned long,stream)));
}

/**
 * Write to an open file
 *
 * Write nmemb elements of size size into file stream from buffer at location ptr.
 *
 *  \param  ptr virtual address of buffer containing data to be written
 *  \param  size of elements to write to file
 *  \param  mmemb number of elements to write to file
 *  \param  stream a file handle returned from an earlier call to MamboFileOpenExtended
 *  \return number of elements written, or 0 if end-of-file reached
 */
INLINE size_t MamboFileWriteExtended(VOID *ptr, size_t size, size_t nmemb, FILE *stream)
{
    return(callthru4(SimFileWriteExtendedCode, CAST(unsigned long,ptr),CAST(unsigned long,size),CAST(unsigned long,nmemb),CAST(unsigned long,stream)));
}

/**
 * Notice an interrupt has been handled.
 *
 * OS tells us that an interrupt has been handled,
 * and that we can now remove our temporary mask for its level
 *
 *  \param  level   level number that's been handled
 */
INLINE int MamboInterruptEOI(int level)
{
    return(callthru1(SimInterruptEOICode, CAST(unsigned long,level)));
}

/**
 * Emit a PATH_INFO emitter record (TAG_STATS_FILENAME)
 *
 * Copies filename to the emitter PATH_INFO structure and
 * emits the resulting record
 *
 * \param   filename    pathname component
 * \return  0 if success, -1 on failure
 */
INLINE int MamboStatsFilename(char *filename)
{
    return(callthru1(SimStatsFilenameCode,CAST(unsigned long,filename)));
}

/**
 * Begin/resume producing emitter records (TAG_START_STATS)
 *
 * Causes Mambo to begin or resume producing emitter records
 *
 * \return  undefined
 */
INLINE int MamboStartStats(void)
{
    return(callthru0(SimStartStatsCode));
}

/**
 * Cease producing emitter records (TAG_STOP_STATS)
 *
 * Causes Mambo to stop producing emitter records
 *
 * \return  undefined
 */
INLINE int MamboStopStats(void)
{
    return(callthru0(SimStopStatsCode));
}

/**
 * Emit a HEADER emitter record (TAG_DUMP_STATS)
 *
 * Causes Mambo to emit a HEADER record with tag TAG_DUMP_STATS
 *
 * \return  undefined
 */
INLINE int MamboDumpStats(void)
{
    return(callthru0(SimDumpStatsCode));
}

/**
 * Emit an ANNOTATION emitter record (TAG_ANNOTATE_STATS)
 *
 * Causes Mambo to emit a STATS_ANNOTATION_INFO structure whose
 * text component is the string given (truncated to 60 characters)
 *
 * \param   string  Annotation text string
 * \return  (boolean semantics) 0 if failure, 1 if success
 */
INLINE int MamboAnnotateStats(char *string)
{
    return(callthru2(SimAnnotateStatsCode, CAST(unsigned long,string), CAST(unsigned long,strlen(string))));
}

/**
 * Create an emitter annotation tag stack
 *
 * Creates a new emitter annotation tag stack. The stack
 * handle (an integer) is returned for use in subsequent
 * tag stack calls. The stack name is truncated to 20
 * characters, if provided
 *
 * \param   name    Name of new stack (is set to "<handle>" if NULL)
 *
 * \return  New tag stack handle (integer)
 */
INLINE int MamboStatsCreateTagStack(char *name)
{
    if (name != NULL)
        {
            return(callthru2(SimStatsCreateTagStackCode,
                             CAST(unsigned long,name), CAST(unsigned long,strlen(name))));
        }
    else
        {
            return(callthru2(SimStatsCreateTagStackCode,
                             CAST(unsigned long,name), CAST(unsigned long,0)));
        }
}

/**
 * Retrieve the last referenced tag stack handle for the current thread
 *
 * \return  Last referenced tag stack handle for the current thread
 */
INLINE int MamboStatsLastTagStackHandle(void)
{
    return(callthru0(SimStatsGetTagStackHandleCode));
}

/**
 * Emit an ANNOTATION emitter record (TAG_ANNOTATE_STATS) and push onto given stack
 *
 * Causes Mambo to emit a STATS_ANNOTATION_INFO structure whose
 * text component is the tag string given (truncated to 60 characters).
 * Pushes this tag onto the given tag stack
 *
 * \param   stack   Annotation stack handle (see MamboStatsCreateTagStack)
 * \param   tag     Annotation text string
 *
 * \return  (boolean semantics) 0 if failure, 1 if success
 */
INLINE int MamboStatsPushTag(unsigned int stack, char *tag)
{
    if (tag != NULL)
        {
            return(callthru3(SimStatsPushTagCode, CAST(unsigned long,stack),
                             CAST(unsigned long,tag), CAST(unsigned long,strlen(tag))));
        }
    else
        {
            return(callthru3(SimStatsPushTagCode, CAST(unsigned long,stack),
                             CAST(unsigned long,tag), CAST(unsigned long,0)));
        }
}

/**
 * Emit the top of stack ANNOTATION emitter record (TAG_ANNOTATE_STATS)
 *
 * Causes Mambo to emit a STATS_ANNOTATION_INFO structure whose
 * text component is the top of the given stack. If the tag pointer
 * given is non-zero, the top of stack tag will be copied to the
 * address given (to the maximum length specified). The top of stack
 * is popped from the given tag stack. If the stack is empty, 0 (failure)
 * is returned.
 *
 * \param   stack       Annotation stack handle (see MamboStatsCreateTagStack)
 * \param   tag         RETURNED tag if non-zero
 * \param   max_tag_len Maximum number of bytes of the TOS tag to copy to tag (including 0-byte)
 *
 * \return  (boolean semantics) 0 if failure, 1 if success
 */
INLINE int MamboStatsPopTag(unsigned int stack, char *tag, unsigned int max_tag_len)
{
    return(callthru3(SimStatsPopTagCode, CAST(unsigned long,stack),
                     CAST(unsigned long,tag), CAST(unsigned long,max_tag_len)));
}

/**
 * Retrieve the top of stack ANNOTATION
 *
 * If the tag pointer given is non-zero, the top of stack tag text will be
 * copied to the address given (to the maximum length specified). If the stack is
 * empty, 0 is returned (failure).
 *
 * \param   stack       Annotation stack handle (see MamboStatsCreateTagStack)
 * \param   tag         RETURNED tag if non-zero
 * \param   max_tag_len Maximum number of bytes of the TOS tag to copy to tag (including 0-byte)
 *
 * \return  (boolean semantics) 0 if failure, 1 if success
 */
INLINE int MamboStatsTOSTag(unsigned int stack, char *tag, unsigned int max_tag_len)
{
    return(callthru3(SimStatsTOSTagCode, CAST(unsigned long,stack),
                     CAST(unsigned long,tag), CAST(unsigned long,max_tag_len)));
}

/**
 * Retrieve the current depth for the given annotation stack
 *
 * \param   stack       Annotation stack handle (see MamboStatsCreateTagStack)
 *
 * \return  The depth of the given stack
 */
INLINE int MamboStatsTagStackDepth(unsigned int stack)
{
    return(callthru1(SimStatsGetTagStackDepthCode, CAST(unsigned long,stack)));
}

/**
 * Destroy a given emitter annotation tag stack, releasing its resources
 *
 * \param   stack       Annotation stack handle (see MamboStatsCreateTagStack)
 *
 * \return  (boolean semantics) 1 (always succeeds)
 */
INLINE int MamboStatsDeleteTagStack(unsigned int stack)
{
    return(callthru1(SimStatsDeleteTagStackCode, CAST(unsigned long,stack)));
}

/**
 * Destroy all emitter annotation tag stacks, releasing all associated resources
 *
 * \return  (boolean semantics) 1 (always succeeds)
 */
INLINE int MamboStatsDeleteAllTagStacks(void)
{
    return(callthru0(SimStatsDeleteTagPoolCode));
}

/**
 * Return simulator statistics
 *
 *  \param  stats   EA of buffer containing stats struct
 */
INLINE int MamboReturnStats(sim_stats *stats)
{
    return(callthru1(SimReturnStatsCode, CAST(unsigned long,stats)));
}

/**
 * stop the simulation
 *
 * Cause the simulator to stop as if requested by the user.
 *
 */
INLINE int MamboStopSimulation(void)
{
    return(callthru0(SimExitCode));
}

/**
 * Dump the PPC statistics
 *
 */
INLINE int MamboDumpPPCStats(void)
{
    return(callthru0(SimDumpPPCStatsCode));
}

/**
 * Clear the PPC statistics
 *
 */
INLINE int MamboClearPPCStats(void)
{
    return(callthru0(SimClearPPCStatsCode));
}

/**
 *  copies physical memory
 *
 *  \param  dest     real address of destination
 *  \param  src      real address of source
 *  \param  length   length
 */
INLINE int MamboCopyQuick(VOID *dest, VOID *src, unsigned int length)
{
    return(callthru3(SimCopyQuickCode, CAST(unsigned long,dest), CAST(unsigned long,src),CAST(unsigned long,length)));
}

/**
 * Dump the System statistics
 *
 */
INLINE int MamboDumpSystemStats(void)
{
    return(callthru0(SimDumpSystemStatsCode));
}

/**
 * Clear the system statistics
 *
 * Clears the PPC statistics as well as system level statistics.
 *
 */
INLINE int MamboClearSystemStats(void)
{
    return(callthru0(SimClearSystemStatsCode));
}

/**
 * enable memory tracing
 *
 */
INLINE int MamboSystemMemTraceEnable(void)
{
    return(callthru0(SimSystemMemTraceEnableCode));
}

/**
 * disable memory tracing
 *
 */
INLINE int MamboSystemMemTraceDisable(void)
{
    return(callthru0(SimSystemMemTraceDisableCode));
}
/**
 * Get simulator version
 *
 * \return Word containing mambo version information of the form:
 (MAMBO_VERSION1 << 24) |
 (MAMBO_VERSION2 << 16) |
 (MAMBO_VERSION3 << 8)  |
 2      (Note: 0=real hardware, 2=Mambo)
*/
INLINE int MamboGetSimInfo(void)
{
    return(callthru0(SimGetSimInfo));
}

/**
 * Invoke CALLTHRU associative trigger
 *
 */
INLINE int MamboCallthruTrigger(unsigned long val4, unsigned long val5, unsigned long val6)
{
    return(callthru3(SimGetSimInfo, val4, val5, val6));
}

/**
 * Dump the instruction address and link register.
 *
 * When this code is received, the state of the processor is dumped
 * to the output as a log entry.
 *
 */
INLINE int MamboDumpState(void)
{
    return(callthru0(SimDumpStateCode));
}

/**
 *  read a char from console's stdin
 *
 *  \return  character read, or -1 if nothing was read
 */
INLINE int MamboReadConsole(void)
{
    return(callthru0(SimReadConsoleCode));
}

/**
 * asynchronously reads a string from thinwire
 *
 *  \param  string  address of buffer for string
 *  \param  length  maximum Length of string
 *  \return length read
 */
INLINE int MamboThinIPRead(char *string, int length)
{
    return(callthru2(SimThinIPReadCode, CAST(unsigned long,string), CAST(unsigned long,length)));
}

/**
 * asynchronously writes a string to thinwire
 *
 *  \param  string    address of string to be printed
 *  \param  length    Length of string
 *  \param  isvirtual =1, address is virtual, =0 address is real
 *  \return length written
 */
INLINE int MamboThinIPWrite(char *string, int length, int isvirtual)
{
    return(callthru3(SimThinIPWriteCode, CAST(unsigned long,string), CAST(unsigned long,length),CAST(unsigned long,isvirtual)));
}

/**
 * start a new cpu
 *
 * The callthru instruction executed on one CPU can be used to start another CPU on the same machine.
 *
 *  \param  cpu_num    number of CPU to start
 *  \param  startaddr    virtual address of code to start executing
 *  \param  toc
 */
INLINE int MamboStartCPU(int cpu_num, VOID *startaddr, unsigned long toc)
{
    return(callthru3(SimStartCPUCode, CAST(unsigned long,cpu_num), CAST(unsigned long,startaddr),CAST(unsigned long,toc)));
}

/**
 * get the instruction count on the cpu which made this call
 *
 * \return  instruction count
 */
INLINE int MamboGetInstrCount(void)
{
    return(callthru0(SimGetInstrCountCode));
}

/**
 * get the number of CPUs for the machine
 *
 * \return  R3 = number of CPUs
 */
INLINE int MamboGetNumbPhysProcs(void)
{
    return(callthru0(SimGetNumbPhysProcsCode));
}

/**
 * get machine attribute
 *
 *  \param  attrib   type of machine attribute to get (see MachineAttr)
 *
 * \return  value of machine attribute
 */
INLINE int MamboGetMachAttr(MachineAttr attrib)
{
    return(callthru1(SimGetMachAttrCode, CAST(unsigned long,attrib)));
}

/**
 *  copies physical memory
 *
 *  \param  dest     real address of destination
 *  \param  src      real address of source
 *  \param  length   length
 */
INLINE int MamboPhysMemCopy(VOID *dest, VOID *src, unsigned int length)
{
    return(callthru3(SimPhysMemCopyCode, CAST(unsigned long,dest), CAST(unsigned long,src),CAST(unsigned long,length)));
}

/**
 *
 * get current time of day
 *
 * \return(sec << 32) + usec
 */
INLINE int MamboGetTime(void)
{
    return(callthru0(SimGetTimeCode));
}

/**
 * set physical memory to a constant byte value
 *
 *  \param  dest   real address of destination
 *  \param  value   byte value to set
 *  \param  length
 *
 * \return  length set
 */
INLINE int MamboPhysMemSet(VOID *dest, unsigned char value, unsigned int length)
{
    return(callthru3(SimPhysMemSetCode, CAST(unsigned long,dest), CAST(unsigned long,value),CAST(unsigned long,length)));
}

/**
 * Get Configuration information
 *
 * Put configuration information into structure
 *
 *  \param  config    Pointer to CallthruConfig structure
 */
INLINE int MamboGetConfig(CallthruConfig *config)
{
    return(callthru1(SimGetConfigCode, CAST(unsigned long,config)));
}

/**
 * Output the log string
 *
 *  \param  comment User defined comment string (Effective Address)
 *  \param  id      User defined ID
 */
INLINE int MamboLogStats(char *comment, int id)
{
    return(callthru2(SimLogStatsCode, CAST(unsigned long,comment), CAST(unsigned long,id)));
}


/**
 * Execute the string as a TCL command
 *
 *  \param  string   pointer to Tcl command string
 */
INLINE int MamboCallTCL(char *string)
{
    return(callthru2(SimCallTCLCode, CAST(unsigned long,string), CAST(unsigned long,strlen(string))));
}

/**
 * Open Firmware Handler
 *
 * OpenFirmware only runs in 32-bit mode
 *
 *  \param  service_strptr   Pointer to OpenFirmware structure
 *
 */
INLINE int MamboPROM(VOID *service_strptr)
{
    return(callthru1(SimPROMCode, CAST(unsigned long,service_strptr)));
}

/**
 *
 * process creation
 *
 * An OS may call this to notify Mambo when a new process has been created.
 *
 *  \param  parent   parent process id
 *  \param  pid      process id
 *  \param  flags    clone_flags
 */
INLINE int MamboPIDCreate(int parent, int pid, int flags)
{
    return(callthru3(SimPIDCreateCode,CAST(unsigned long,parent),CAST(unsigned long,pid),CAST(unsigned long,flags)));
}

/**
 *
 * process normal exit
 *
 * An OS may call this to notify Mambo when a process exits.
 *
 *  \param  pid   process id
 */
INLINE int MamboPIDExecExit(int pid)
{
    return(callthru1(SimPIDExecExitCode, CAST(unsigned long,pid)));
}

/**
 * PID Exec
 *
 * Notice that OS exec'd a file
 *
 *  \param  pid      pid
 *  \param  filename filename address
 *  \param  len      filename length
 *
 */
INLINE int MamboPIDExec(int pid, char *filename, int len)
{
    return(callthru3(SimPIDExecCode, CAST(unsigned long,pid), CAST(unsigned long,filename), CAST(unsigned long,len)));
}

/**
 *  PID Resume
 *
 * An OS may call this to notify Mambo when a process resumes.
 *
 *  \param  processor   processor number
 *  \param  pid   pid
 */
INLINE int MamboPIDResume(int processor, int pid)
{
    return(callthru3(SimPIDResumeCode, CAST(unsigned long,processor), CAST(unsigned long,pid), CAST(unsigned long,0)));
}

/**
 *  PID Kill
 *
 * An OS may call this to notify Mambo when a process is killed.
 *
 *  \param  pid   pid
 */
INLINE int MamboPIDKill(int pid)
{
    return(callthru1(SimPIDKillCode,CAST(unsigned long,pid)));
}

/**
 * Kernel Thread Create
 *
 * Notice of a Kernel Thread Create
 *
 *  \param  parent  ppid
 *  \param  pid     pid
 *  \param  flags   clone_flags
 *
 */
INLINE int MamboKernelThreadCreate(int parent, int pid, int flags)
{
    return(callthru3(SimKernelThreadCreateCode,CAST(unsigned long,parent),CAST(unsigned long,pid),CAST(unsigned long,flags)));
}

/**
 * signals kernel has completed booting
 *
 */
INLINE int MamboKernelStarted(void)
{
    return(callthru0(SimKernelStartedCode));
}

/**
 * signals a shared library has been loaded
 *
 */
INLINE int MamboLinuxLoadShlib(void)
{
    return(callthru0(SimLinuxLoadShlibCode));
}

/**
 * Open a connection
 *
 * openConnection opens a connection of the type specified by the connectionType
 * parameter and makes the connection current. Possible connections types include:
 * producer and consumer. Consumer connection must be opened (created) before a
 * producer connection can be opened.
 *
 * All connections are identified by their string name and connectionType.
 *
 * Open connection
 *  \param  name   Name of connection
 *  \param  type   Type of connection 0=producer, 1=consumer
 */
INLINE int MamboOpenConnection(char *name, int type)
{
    return(callthru2(SimOpenConnectionCode, CAST(unsigned long,name), CAST(unsigned long,type)));
}

/**
 * Close a connection
 *
 *  \param  name      (char *)Name of connection
 *  \param  type     (int)Type of connection
 */
INLINE int MamboCloseConnection(char *name, int type)
{
    return(callthru2(SimCloseConnectionCode, CAST(unsigned long,name), CAST(unsigned long,type)));
}

/**
 *
 * Read data from a connection
 *
 * Data is received from the connection and copied to the address specified.
 *
 *  \param  buffer  (int)address
 *  \param  len   (int)length
 */
INLINE int MamboReadConnection(char *buffer, int len)
{
    return(callthru2(SimReadConnectionCode, CAST(unsigned long,buffer), CAST(unsigned long,len)));
}

/**
 * Write connection
 *
 * Data at input address is written to connection.
 *
 *  \param  buffer  address
 *  \param  len    length
 */
INLINE int MamboWriteConnection(char *buffer, int len)
{
    return(callthru2(SimWriteConnectionCode, CAST(unsigned long,buffer), CAST(unsigned long,len)));
}

/**
 * pthread create request
 *
 *  \param  startaddr    EA of start routine
 *  \param  stacksize
 *  \param  threadid
 *
 */
INLINE int MamboPthreadCreate(VOID *startaddr, unsigned int stacksize, int threadid)
{
    return(callthru3(SimPthreadCreateCode,CAST(unsigned long,startaddr),CAST(unsigned long,stacksize),CAST(unsigned long,threadid)));
}

/**
 *  Read from bogus disk
 *
 *  \param devno    device number
 *  \param buf      address of buffer
 *  \param sect     starting sector
 *  \param nrsect   number of sectors
 *
 *  \return  0 if success, -1 on failure
 */
INLINE int MamboBogusDiskRead(int devno, VOID *buf, unsigned long sect, unsigned long nrsect)
{
    return callthru3(SimBogusDiskReadCode, CAST(unsigned long,buf), CAST(unsigned long,sect),
                     CAST(unsigned long,(nrsect<<16)|devno));
}

/**
 * Write to bogus disk
 *
 *  \param devno    device number
 *  \param buf      address of buffer
 *  \param sect     starting sector
 *  \param nrsect   number of sectors
 *
 *  \return  0 if success, -1 on failure
 */
INLINE int MamboBogusDiskWrite(int devno, VOID *buf, unsigned long sect, unsigned long nrsect)
{
    return callthru3(SimBogusDiskWriteCode, CAST(unsigned long,buf), CAST(unsigned long,sect),
                     CAST(unsigned long,(nrsect<<16)|devno));
}

/**
 * Get bogus disk info
 *
 *  \param op     bogus disk operation
 *                    0 -> Sync all buffers to disk
 *                    1 -> Query Device Status
 *                    2 -> Query Device Block Size
 *                    3 -> Query Device Size
 *  \param devno  device number
 *
 *  \return  for Sync, 0 if success, -1 on failure; for Query, requested info
 */
INLINE int MamboBogusDiskInfo(bogus_disk_operation op, int devno)
{
    return(callthru2(SimBogusDiskInfoCode, CAST(unsigned long,op), CAST(unsigned long,devno)));
}


/**
 * Probe for device
 *
 * Probe for device and return MAC address in buffer
 *
 *  \param    devno device number
 *  \param    buf    mac address buffer
 *
 *  \return  0 if success, -1 on failure
 */
INLINE int MamboBogusNetProbe(int devno, VOID *buf, VOID *pktbufs)
{
    return(callthru3(SimBogusNetProbeCode, CAST(unsigned long,devno), CAST(unsigned long,buf), CAST(unsigned long, pktbufs)));
}

/**
 * Transmit a packet
 *
 * \param    devno device number
 * \param    buf    data to send
 * \param    size   data length
 *
 *  \return  0 if success, -1 on failure
 */
INLINE int MamboBogusNetSend(int devno, VOID *buf, unsigned long size)
{
    return(callthru3(SimBogusNetSendCode, CAST(unsigned long,devno), CAST(unsigned long,buf), CAST(unsigned long,size)));
}

/**
 * Transmit a packet with DMA
 *
 * \param    devno device number
 * \param    buf    data to send
 * \param    parms  data struct holding parms
 * \param    frags  number of fragments
 *
 *  \return  0 if success, -1 on failure
 */
INLINE int MamboBogusNetDMASend(int devno, VOID *buf, VOID *parms, unsigned long frags)
{
    return(callthru4(SimBogusNetDMASendCode, CAST(unsigned long,devno), CAST(unsigned long,buf),
                     CAST(unsigned long,parms), CAST(unsigned long,frags)));
}

/**
 * Receive a packet
 *
 *  \param    devno      device number
 *  \param    buf        buffer address
 *  \param    size       buffer length
 *
 *  \return  -1 on failure, 0 on no packet, num_bytes_received on packet
 */
INLINE int MamboBogusNetRecv(int devno, VOID *buf, unsigned long size)
{
    return(callthru3(SimBogusNetRecvCode, CAST(unsigned long,devno), CAST(unsigned long,buf), CAST(unsigned long,size)));
}

/**
 * Receive a packet with csum
 *
 *  \param    devno      device number
 *  \param    buf        buffer address
 *  \param    size       buffer length
 *
 *  \return  -1 on failure, 0 on no packet, num_bytes_received on packet
 */
INLINE int MamboBogusDMANetRecv(int devno, VOID *buf, unsigned long size, VOID *csum)
{
    return(callthru4(SimBogusNetRecvCode, CAST(unsigned long,devno), CAST(unsigned long,buf),
                     CAST(unsigned long,size), CAST(unsigned long,csum)));
}

/**
 * Transmit data
 *
 * \param    devno device number
 * \param    buf    data to send
 * \param   size   data length
 *
 * \return  0 if success, -1 on failure
 */
INLINE int MamboBogusSocketSend(int devno, VOID *buf, unsigned long size)
{
    return(callthru3(SimBogusSocketSendCode, CAST(unsigned long,devno), CAST(unsigned long,buf), CAST(unsigned long,size)));
}

/**
 * Receive data
 *
 *  \param    devno      device number
 *  \param    buf        buffer address
 *  \param    size       buffer length
 *
 *  \return  -1 on failure, 0 on no packet, num_bytes_received on packet
 */
INLINE int MamboBogusSocketRecv(int devno, VOID *buf, unsigned long size)
{
    return(callthru3(SimBogusSocketRecvCode, CAST(unsigned long,devno), CAST(unsigned long,buf), CAST(unsigned long,size)));
}

/**
 * Kernel Thread Call
 *
 * Notice of a Kernel Thread Call
 *
 * Info is sent to emitter or debug output for PTRACK.
 *
 *  \param  pid
 *  \param  func function pointer
 *
 */
INLINE int MamboKernelThreadCall(int pid, VOID *func)
{
    return(callthru2(SimKernelThreadCallCode, CAST(unsigned long,pid), CAST(unsigned long, func)));
}
/*
  Kernel Thread Call
  used by the bogus net driver to free a DMA buffer
*/


INLINE int MamboBogusNetFreeBuf(int devno, int index)
{
    return(callthru2(SimBogusNetFreeBufCode, CAST(unsigned long, devno), CAST(unsigned long, index)));
}


/**
 * Dump contents of memory
 *
 *  \param  startaddr starting address to print
 *  \param  len  number of bytes to dump
 */
INLINE int MamboDumpMemory(int startaddr, int len)
{
    return(callthru2(SimDumpMemoryCode, CAST(unsigned long,startaddr), CAST(unsigned long, len)));
}

/**
 * notice a memory mapped file has been created
 *
 *  \param  filename        name of mapped file
 *  \param  filenamelen     the length of the filename parameter
 *  \param  addr            mapped address
 *  \param  len             mapped length
 *  \param  prot
 *  \param  flag
 *  \param  offset
 *
 */
INLINE int MamboKernelMmapFile(char *filename, int filenamelen, unsigned long addr, unsigned long len, unsigned long prot, unsigned long flag, unsigned long offset)
{
    return(callthru7(SimKernelMmapFileCode, CAST(unsigned long,filename), CAST(unsigned long,filenamelen), CAST(unsigned long,addr), CAST(unsigned long,len), CAST(unsigned long,prot), CAST(unsigned long,flag), CAST(unsigned long,offset)));
}

/**
 * halt the current thread
 *
 */
INLINE void MamboHaltThread(void)
{
    callthru0(SimBogusHaltCode);
}

/**
 *  get the cycle count on the cpu which made this call
 *
 */
INLINE int MamboGetCycleCount(void)
{
    return(callthru0(SimGetCycleCountCode));
}

/*
  Bogus net is triggered to return a packet and raise an interrupt from a user application.
*/

INLINE int MamboGetNextpacket(int pktsize, int coalfactor)
{
    return(callthru2(SimGetNextPacketCode, CAST(unsigned long, pktsize), CAST(unsigned long, coalfactor)));
}


/**
 * Obtain a TVU object handle by name
 *
 * \param   name    Name of the TVU object to search for
 *
 * \return  TVU object handle (integer), 0 if not found
 */
INLINE int MamboLookupTVUObject(char *name)
{
    return(callthru2(SimLookupTVUObjectByNameCode, CAST(unsigned long,name), CAST(unsigned long,strlen(name))));
}

/**
 * Enable tracking for a TVU object
 *
 * \param   tvu     TVU object handle (see MamboLookupTVUObject)
 *
 * \return  1 if succeeded, 0 if failed
 */
INLINE int MamboEnableTVUObject(unsigned int tvu)
{
    return(callthru1(SimEnableTVUObjectCode, CAST(unsigned long,tvu)));
}

/**
 * Enable tracking for all TVU objects
 */
INLINE void MamboEnableAllTVUObjects(void)
{
    callthru0(SimEnableAllTVUObjectsCode);
}

/**
 * Disable tracking for a TVU object
 *
 * \param   tvu     TVU object handle (see MamboLookupTVUObject)
 *
 * \return  1 if succeeded, 0 if failed
 */
INLINE int MamboDisableTVUObject(unsigned int tvu)
{
    return(callthru1(SimDisableTVUObjectCode, CAST(unsigned long,tvu)));
}

/**
 * Disable tracking for all TVU objects
 */
INLINE void MamboDisableAllTVUObjects(void)
{
    callthru0(SimDisableAllTVUObjectsCode);
}

/**
 *  Route print through mambo to stdout
 *
 * \param       msg     String to print
 */
INLINE int MamboPrint(const char *msg)
{
    return(callthru1(SimPrintCode, CAST(unsigned long, msg)));
}




/**
 *  Write a hex 64 bit value to the console.
 *
 * \param unsigned long
 */
INLINE int MamboPrintInt(unsigned long long value)
{
    return(callthru1(SimPrintIntCode, CAST(unsigned long,value)));
}


#endif /* #ifndef _CALLTHRU_CONFIG_H_ */
