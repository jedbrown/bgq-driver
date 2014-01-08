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

//! \file  CoreDump.cc

// Includes
#include "Kernel.h"
#include "CoreDump.h"
#include "syscalls/internalsyscalls.h"
#include "ToolControl.h"
#include <fcntl.h>
#include <hwi/include/bqc/wu_mmio.h>
#include <elf.h>
#include <sys/procfs.h>

// Binary core file support
#define MILLION 1000000
#define CORE_NAME "CORE"
#define ROUND_UP_WORD(p)  (((p) + 0x7)  & ~0x7)
#define ROUND_UP_512B(p) (((p) + 0x1ff)  & ~0x1ff)
#define BINARY_MAX_BUFFER  0x10000 // For dumping directly from memory

#define max(a,b) (((a)>(b))?(a):(b))

// Buffer for core dump
CoreBuffer buff;

static const char *TSIZE[] = {
    "?????", // [0]
    "4KB ",  // [1]
    "?????", // [2]
    "64KB ", // [3]
    "?????", // [4]
    "1MB  ", // [5]
    "?????", // [6]
    "16MB ", // [7]
    "?????", // [8]
    "256MB", // [9]
    "1GB  ", // [10]
    "4GB  ", // [11]
    "16GB ", // [12]
    "?????", // [13]
    "256GB", // [14]
    "1TB  ", // [15]
};

static uint64_t TSIZE_MASK[] = {
    0,
    0x1000ul,      // 4KB
    0,
    0x10000ul,     // 646KB
    0,
    0x100000ul,    // 1MB
    0,
    0x1000000ul,   //16MB
    0,
    0x10000000ul,  // 256MB
    0x40000000ul,  // 1GB
    0x100000000ul, // 4GB
    0,
    0x4000000000ul,
};


static char* format_sched_state(int state, char *buffer, int length)
{
    switch (state & ~SCHED_STATE_APPEXIT)
    {
    case SCHED_STATE_FREE:
        snprintf(buffer, length, "FREE");
        break;
    case SCHED_STATE_SLEEP:
        snprintf(buffer, length, "SLEEP");
        break;
    case SCHED_STATE_FUTEX:
        snprintf(buffer, length, "FUTEX_WAIT");
        break;
    case SCHED_STATE_FLOCK:
        snprintf(buffer, length, "FLOCK");
        break;
    case SCHED_STATE_RESET:
        snprintf(buffer, length, "RESET");
        break;
    case SCHED_STATE_HOLD:
        snprintf(buffer, length, "HOLD");
        break;
    case 0:
        snprintf(buffer, length, "RUN");
        break;
    default:
        snprintf(buffer, length, "UNKNOWN");
    }
    return buffer;
}

static char* format_interrupt_code(int code, Regs_t *regs, char *buffer, int length)
{

    switch (code)
    {

    case DEBUG_CODE_DSI:
        snprintf(buffer, length, "Data Storage Interrupt IP=0x%016lx DEAR=0x%016lx ESR=0x%016lx TLS=0x%016lx",
                 regs->ip, regs->dear, regs->esr, regs->gpr[2]);
        break;

    case DEBUG_CODE_ISI:
        snprintf(buffer, length, "Instruction Storage Interrupt IP=0x%016lx ESR=0x%016lx",
                 regs->ip, regs->esr);
        break;

    case DEBUG_CODE_ALGN:
        snprintf(buffer, length, "Alignment Exception DEAR=0x%016lx ESR=0x%016lx CCR0=0x%016lx",
                 regs->dear, regs->esr, mfspr(SPRN_CCR0));
        break;

    case DEBUG_CODE_PROG:
        snprintf(buffer, length, "Program Exception IP=0x%016lx  ESR=0x%016lx: %s%s%s%s",
                 regs->ip,
                 regs->esr,
                 (regs->esr & ESR_PIL  ? "Illegal Instruction "    : ""),
                 (regs->esr & ESR_PPR  ? "Privileged Instruction " : ""),
                 (regs->esr & ESR_PTR  ? "Trap "                   : ""),
                 (regs->esr & ESR_FP   ? "Floating Point "         : "")
                 );
        break;

    case DEBUG_CODE_FPU:
        snprintf(buffer, length, "FPU Unavailable Exception");
        break;

    case DEBUG_CODE_APU:
        snprintf(buffer, length, "APU Unavailable Exception");
        break;

    case DEBUG_CODE_DTLB:
        snprintf(buffer, length, "Data TLB Miss Exception DEAR=0x%016lx ESR=0x%016lx",
                 regs->dear, regs->esr);
        break;

    case DEBUG_CODE_ITLB:
        snprintf(buffer, length, "Instruction TLB Miss Exception");
        break;

    case DEBUG_CODE_DEBUG:
        snprintf(buffer, length, "Debug Exception DBSR=0x%016lx", regs->dbsr);
        break;

    case DEBUG_CODE_VECT:
        snprintf(buffer, length, "Vector Unavailable Interrupt");
        break;

    case DEBUG_CODE_PDBI:
        snprintf(buffer, length, "Processor DoorBell Interrupt");
        break;

    case DEBUG_CODE_PDBCI:
        snprintf(buffer, length, "Processor DoorBell Critical Interrupt");
        break;

    case DEBUG_CODE_GDBI:
        snprintf(buffer, length, "Guest DoorBell Interrupt");
        break;

    case DEBUG_CODE_GDBCI:
        snprintf(buffer, length, "Guest DoorBell Critical or Machine Check Interrupt");
        break;

    case DEBUG_CODE_EHVSC:
        snprintf(buffer, length, "Embedded Hypervisor System Call");
        break;

    case DEBUG_CODE_EHVPRIV:
        snprintf(buffer, length, "Embedded Hypervisor Privilege");
        break;

    case DEBUG_CODE_LRATE:
        snprintf(buffer, length, "Embedded Hypervisor LRAT Exception");
        break;

    case DEBUG_CODE_NONE:
    default :
        break;

    }

    return buffer;
}


static char* signal_to_label(int signal, char *buf, int length)
{
    switch (signal)
    {
    case SIGSEGV :
        snprintf(buf, length, "SIGSEGV");
        break;
    case SIGILL  :
        snprintf(buf, length, "SIGILL");
        break;
    case SIGTRAP :
        snprintf(buf, length, "SIGTRAP");
        break;
    case SIGFPE  :
        snprintf(buf, length, "SIGFPE");
        break;
    case SIGBUS  :
        snprintf(buf, length, "SIGBUS");
        break;
    case SIGSTKFLT:
        snprintf(buf, length, "SIGSTKFLT");
        break;
    case SIGABRT:
        snprintf(buf, length, "SIGABRT");
        break;
    case SIGQUIT:
        snprintf(buf, length, "SIGQUIT");
        break;
    case SIGMUNDFATAL:
        snprintf(buf, length, "SIGMUNDFATAL");
        break;
    case SIGMUFIFOFULL:
        snprintf(buf, length, "SIGMUFIFOFULL");
        break;
    case SIGDCRVIOLATION:
        snprintf(buf, length, "SIGDCRVIOLATION");
        break;
    default      :
        snprintf(buf, length, "???");
        break;
    }

    return buf;
}


static int coredump_open(CoreBuffer *buffer, AppProcess_t *process)
{
    uint32_t tmp;
    memset(buffer, 0, sizeof(CoreBuffer));
    buffer->fileFd = -1;
    int result = 0;

#define SETREGISTER(parm, variable, default) if(App_GetEnvValue(parm, &tmp)) buffer->flags.variable = tmp; else buffer->flags.variable = default;
    SETREGISTER("BG_COREDUMP_MAILBOX",  dumpToMailbox,         NodeState.DumpCoreToMailbox);
    SETREGISTER("BG_COREDUMP_MAILBOX",  disableBuffering,      NodeState.DumpCoreToMailbox);

    SETREGISTER("BG_COREDUMPREGS",     dumpRegisters,         1);
    SETREGISTER("BG_COREDUMPPERS",     dumpPersonality,       1);
    SETREGISTER("BG_COREDUMPINTCOUNT", dumpInterruptCounters, 1);
    SETREGISTER("BG_COREDUMPTLBS",     dumpTLBs,              1);
    SETREGISTER("BG_COREDUMPSTACK",    dumpStack,             0);
    SETREGISTER("BG_COREDUMPGPR",      dumpGPR,               1);
    SETREGISTER("BG_COREDUMPFPR",      dumpFPR,               1);
    SETREGISTER("BG_COREDUMPSPR",      dumpSPR,               1);
#undef SETREGISTER

    if (Personality_IsMambo()) // Force core files to the mailbox when running on mambo
    {
        buffer->flags.dumpToMailbox = 1;
    }

    // If we are are not function shipping to CIOS, route the core file via the JTAG
    // mailbox.  Also, simply flush every line since the mailbox has
    // relatively limited packet size.

    if (!Personality_CiosEnabled() || AppAgentIdentifier(process))
    {
        buffer->flags.dumpToMailbox = 1;
        buffer->flags.disableBuffering = 1;
    } else if (buffer->flags.dumpToMailbox == 0)
    {
        // Get the path and file prefix for core file from environment variables.
        const char *corepath  = ".";
        const char *coreprefix = "core";
        App_GetEnvString("BG_COREDUMPPATH", &corepath);
        App_GetEnvString("BG_COREDUMPFILEPREFIX", &coreprefix);

        // Generate the path to the core file.
        char filename[550];
        snprintf(filename, sizeof(filename), "%s/%s.%u", corepath, coreprefix, process->Rank);

        // Have we detected error on a previous attempt to write a core file from this node?
        if (GetMyAppState()->disableCorefileWrite)
        {
            return -1;
        }

        // Open the core file.
        uint64_t rc = internal_open(filename, O_CREAT | O_TRUNC | O_WRONLY | O_LARGEFILE, 0666);

        // If there was an error opening the core file using the generated path, try to open a file
        // in the current working directory.
        if (CNK_RC_IS_FAILURE(rc))
        {
            snprintf(filename, sizeof(filename), "core.%u", process->Rank);
            rc = internal_open(filename, O_CREAT | O_TRUNC | O_WRONLY | O_LARGEFILE, 0666);
        }

        if (CNK_RC_IS_SUCCESS(rc))
        {
            buffer->fileFd = CNK_RC_VALUE(rc);
        } else
        {
            //printf("(E) Error opening core file %s: %s\n", filename, CNK_RC_STRING(rc));
            result = -1;
            GetMyAppState()->disableCorefileWrite = 1; // Shut down all subsequent attempts to write core files from this node.
        }

        // For a binary core files, the following segments are always dumped.
        buffer->dumpSegment[IS_TEXT] = true;
        buffer->dumpSegment[IS_DATA] = true;
        buffer->dumpSegment[IS_HEAP] = true;
    }
    return result;
}

// Check to see if a binary core file was requested for this rank
// This code is modified from what was in debug.c to set the initail
// value for the dump option from the environment variable.
// The value may be modified dynamically with the
//   Kernel_EnableBinaryCoreFile()
// SPI
int coredump_binary_for_rank(AppProcess_t *pProc)
{
    const char *binaryCoreRanks;
    const char *tmpPtr;
    char value[25];
    int badParmFound = 0;
    uint32_t rank = pProc->Rank;

    // User wants a binary dump?
    if (App_GetEnvString("BG_COREDUMPBINARY", &binaryCoreRanks))
    {
        // All ranks?
        if (binaryCoreRanks[0] == '*')
        {
            return 2;
        }
        // check to see if this rank was in the list
        tmpPtr = binaryCoreRanks;
        while (*tmpPtr)
        {
            for (; *tmpPtr; tmpPtr++)
            {
                if ((*tmpPtr == ','))
                {
                    break;
                }
            }  // End loop looking for a delimiter

            int badValue_local = 0;
            uint32_t str_size = tmpPtr - binaryCoreRanks;
            // limit damage of bogus length rank value and leave space for null terminator
            if (sizeof(value) <= str_size)
            {
                str_size = (sizeof(value) - 1);
                badValue_local = 1;
                badParmFound++;
            }
            // the above check should seldom fail so just fall through
            // as we have already marked the value as an error
            strncpy(value, binaryCoreRanks, str_size);
            *(value + str_size) = 0; // null terminate the string to prep for atoi conversion

            // do we have a match and was the conversion valid
            uint32_t rank_from_string = (uint32_t)atoi_(value);

            // the supplied string may have been bogus
            uint32_t i;
            for (i = 0; i < str_size; i++)
            {
                if ((value[i] < 0x30) || (value[i] > 0x39))
                {
                    badValue_local = 1;
                    badParmFound++;
                }
            }
            if ((rank == (uint32_t)rank_from_string) && !badValue_local)
            {
                return 1;
            }

            if (!*tmpPtr)
            {
                break;
            }
            // Move base pointer and try next value
            tmpPtr++;
            binaryCoreRanks = tmpPtr;
        }  // End loop looking for rank in env vars
    }

    if (badParmFound)
    {
        return (-1);   // indicate short form dump with error message
    }

    return 0;
}

uint64_t coredump_internal_write(int fd, const void* buffer, size_t cnt)
{
    uint64_t rc = CNK_RC_FAILURE(EINTR);
    AppState_t *app = GetMyAppState();
    if (!app->disableCorefileWrite)
    {
        rc = internal_write(fd, buffer, cnt);
        if (CNK_RC_IS_FAILURE(rc))
        {
            app->disableCorefileWrite = 1;
        }
    }
    return rc;
}

static void coredump_flush(CoreBuffer *buffer)
{
    if (buffer->flags.dumpToMailbox)
    {
        NodeState.FW_Interface.putn(buffer->buffer, buffer->length);
    } else
    {
        coredump_internal_write(buffer->fileFd, buffer->buffer, buffer->length);
    }

    buffer->buffer[0] = 0; // clear the buffer
    buffer->length = 0;
    return;
}

void coredump_printf(CoreBuffer *buffer, const char *fmt, ...)
{
    char line[256];
    va_list args;

    va_start(args, fmt);
    int length = vsnprintf(line, sizeof(line), fmt, args); // returns the number of desired bytes in formatted string assuming no limit
    if (length > (int)sizeof(line)) // would the formatted string have exceed the size of our line buffer?
    {
        // Yes, we have an overflow. Truncate the line. This will be a serious problem in a binary file.
        length = (int)sizeof(line);
        printf("(E) coredump_printf: Formatted line exceeds allocated buffer size of 256 bytes\n");
    }
    va_end(args);

    if ((buffer->flags.disableBuffering == 0) && (buffer->length + length + 2 >= CONFIG_CORE_BUFFER_SIZE))
    {
        coredump_flush(buffer);
    }
    int lsize = sizeof(buffer->buffer);
    buffer->length += length;
    strncat(buffer->buffer, line, lsize);

    if (buffer->flags.disableBuffering)
    {
        coredump_flush(buffer);
    }

    return;
}

static void coredump_close(CoreBuffer *buffer)
{
    if (!buffer->flags.dumpToMailbox)
    {
        if (!buffer->flags.dumpBinary)
        {
            coredump_flush(buffer);
        }
        internal_close(buffer->fileFd);
    }

    return;
}

static void coredump_traceback(CoreBuffer *buff, KThread_t *kthread)
{
    HWThreadState_t *hwt = GetHWThreadStateByProcessorID(kthread->ProcessorID);
    uint64_t *kernel_stack_start = (uint64_t *)(&(hwt->StandardStack));
    uint64_t *kernel_stack_end   = (uint64_t *)((uint64_t) & (hwt->StandardStack)) + sizeof(hwt->StandardStack);
    Regs_t *regs = &(kthread->Reg_State);
    uint64_t *stkptr = (uint64_t *)(regs->gpr[1]);
    int numStackFrames = 0; // initialize to zero

    coredump_printf(buff, "+++STACK\n");
    coredump_printf(buff, "Frame Address     Saved Link Reg\n");
    while (stkptr &&   // stack pointer is not NULL
           (numStackFrames < CONFIG_CORE_MAX_STKTRC_DEPTH) &&  // did not yet store the max number of frames
           (VMM_IsAppAddress(stkptr, sizeof(uint64_t[3])) || // Stack pointer is a valid Application address or
            ((stkptr > kernel_stack_start) && (stkptr < kernel_stack_end)))) // stack pointer is a kernel stack address
    {
        coredump_printf(buff, "%016lx  %016lx\n",  stkptr, *(stkptr + 2));
        numStackFrames++;
        stkptr = (uint64_t *)(*stkptr);
    }
    coredump_printf(buff, "---STACK\n");

    return;
}

static void coredump_raw_memory(CoreBuffer *buff, uint64_t start, uint64_t end)
{

    if (start > end)
    {
        return;
    }

    coredump_printf(buff, "Stack Contents:\n");
    for (uint64_t addr = start & ~0xF; addr < end; addr += 32)
    {
        uint32_t *word = (uint32_t *)addr;
        coredump_printf(buff, "  %016lx : %08x %08x %08x %08x %08x %08x %08x %08x\n",
                        addr, word[0], word[1], word[2], word[3], word[4], word[5], word[6], word[7]);
    }

    return;
}

static void coredump_mmap_allocations(CoreBuffer *buff, AppProcess_t *process)
{
    MMapChunk_t *ch = process->MmapMgr.ByAddr_anchor;
    uint64_t addr = 0;

    coredump_printf(buff, "    Chunk Status Address            Size\n");
    for (int i = 0; ch; ch = ch->byaddr_next, i++)
    {
        coredump_printf(buff, "    [%2d]: %s   0x%016lx %d\n", i,
                        ((ch->addr & MMAPCHUNK_ADDR_FREE) ? "Free" : "Busy"),
                        ch->addr & ~(1UL), ch->size);
        addr = ch->addr;
    }

    return;
}

static void coredump_guard(CoreBuffer *buff, AppProcess_t *process, KThread_t *kthread)
{
    if (process->Guard_Enable)
    {
        int wac_num = (kthread->ProcessorID & 0x03) + CNK_STACK_GUARD_FIRST_WAC;
        uint64_t guardbase_virt = process->Heap_VStart + (kthread->GuardBaseAddress - process->Heap_PStart);
        uint64_t guard_end = guardbase_virt | (~(kthread->GuardEnableMask));
        coredump_printf(buff, "  Stack Guard  : 0x%016lx - 0x%016lx (WAC%d)\n", guardbase_virt, guard_end, wac_num);
    } else
    {
        coredump_printf(buff, "  Stack Guard disabled\n");
    }
}

static void coredump_memory(CoreBuffer *buff, AppProcess_t *process, KThread_t *kthread, bool includeProcessData)
{
    coredump_printf(buff, "Memory:\n");
    if (includeProcessData)
    {
        if (NodeState.SharedMemory.VStart && NodeState.SharedMemory.Size)
        {
            coredump_printf(buff, "  Shared memory: 0x%016lx - 0x%016lx\n",
                            NodeState.SharedMemory.VStart, NodeState.SharedMemory.VStart + NodeState.SharedMemory.Size - 1);
        }
        if (NodeState.PersistentMemory.VStart && NodeState.PersistentMemory.Size)
        {
            coredump_printf(buff, "  Persistent: 0x%016lx - 0x%016lx\n",
                            NodeState.PersistentMemory.VStart, NodeState.PersistentMemory.VStart + NodeState.PersistentMemory.Size - 1);
        }
        coredump_printf(buff, "  Heap: 0x%016lx - 0x%016lx (brk at 0x%016lx)\n",
                        process->Heap_Start, process->Heap_End, process->Heap_Break);
        coredump_printf(buff, "  MMap: 0x%016lx - 0x%016lx\n",
                        process->MMap_Start, process->MMap_End);
        coredump_mmap_allocations(buff, process);
    }
    coredump_guard(buff, process, kthread);

    coredump_printf(buff, "  Stack pointer: 0x%016lx (bottom at 0x%016lx with %d bytes in use)\n",
                    kthread->Reg_State.gpr[1], (uint64_t)kthread->pUserStack_Bot, (uint64_t)kthread->pUserStack_Bot - kthread->Reg_State.gpr[1]);

    if ((buff->flags.dumpTLBs) &&
        (ProcessorID() == kthread->ProcessorID) &&
        includeProcessData)
    {
        coredump_printf(buff, "                                                                                             SSSUUU  GAI        \n");
        coredump_printf(buff, "TLB   V EA Range                          RA Range                          Size  ThID WIMGE RWXRWX XSSP TID  TL\n");
        coredump_printf(buff, "----- - ---------------- ---------------- ---------------- ---------------- ----- ---- ----- ------ ---- ---- --\n");

        for (uint64_t i = 0; i < 512; i++)
        {
            uint64_t mas1, mas2, mas7_3, mas8, mmucr3;

            mtspr(SPRN_MAS2, (i * 4096) / 4);
            mtspr(SPRN_MAS1, MAS1_V(1) | MAS1_TID(0) | MAS1_TS(0) | MAS1_TSIZE_4KB);
            tlbre(i, &mas1, &mas2, &mas7_3, &mas8, &mmucr3);
            if (mas1 & MAS1_V(1))
            {

                int size = (mas1 & MAS1_TSIZE(-1)) >> (63 - 55);


                uint64_t pageMask = TSIZE_MASK[size] - 1;
                uint64_t xMask    = mas2 & MAS2_EPN(-1) & pageMask;
                uint8_t  ThdID    = (mmucr3 & MMUCR3_ThdID(-1)) >> (63 - 63);
                uint64_t xOffset  = (mmucr3 & MMUCR3_X(-1)) ? (xMask + 0x1000) : 0;

                coredump_printf(buff,
                                "%3d:%d V %016llX-%016llX %016llX-%016llX %s %d%d%d%d %c%c%c%c%c %c%c%c%c%c%c %c%c%c%c %04llX %02llX\n",
                                i / 4, i % 4,
                                (mas2 & MAS2_EPN(-1) & ~pageMask) + xOffset, (mas2 & MAS2_EPN(-1)) + (xOffset | pageMask),
                                (mas7_3 & MAS7_3_RPN(-1) & ~pageMask) + xOffset, (mas7_3 & MAS7_3_RPN(-1)) + (xOffset | pageMask),
                                TSIZE[size],

                                (ThdID & 0x8) ? 1 : 0,
                                (ThdID & 0x4) ? 1 : 0,
                                (ThdID & 0x2) ? 1 : 0,
                                (ThdID & 0x1) ? 1 : 0,

                                (mas2 & MAS2_W(1)) ? 'W' : '-',
                                (mas2 & MAS2_I(1)) ? 'I' : '-',
                                (mas2 & MAS2_M(1)) ? 'M' : '-',
                                (mas2 & MAS2_G(1)) ? 'G' : '-',
                                (mas2 & MAS2_E(1)) ? 'E' : '-',
                                (mas7_3 & MAS3_SR(-1)) ? 'R' : '-',
                                (mas7_3 & MAS3_SW(-1)) ? 'W' : '-',
                                (mas7_3 & MAS3_SX(-1)) ? 'X' : '-',
                                (mas7_3 & MAS3_UR(-1)) ? 'r' : '-',
                                (mas7_3 & MAS3_UW(-1)) ? 'w' : '-',
                                (mas7_3 & MAS3_UX(-1)) ? 'x' : '-',
                                (mmucr3 & MMUCR3_X(-1)) ? 'X' : '-',
                                (mas8 & MAS8_TGS(-1)) ? 'H' : '-',
                                (mas1 & MAS1_TS(-1)) ? '1' : '-',
                                (mas1 & MAS1_IPROT(-1)) ? '1' : '-',

                                (mas1 & MAS1_TID(-1)) >> (63 - 47),
                                (mas8 & MAS8_TLPID(-1)) >> (63 - 63)

                                );
            }
        }
    }

    if (buff->flags.dumpStack)
    {
        coredump_raw_memory(buff, kthread->Reg_State.gpr[1], (uint64_t)kthread->pUserStack_Bot);
    }

    return;
}

const char *ExceptionCtrName[] = {
    "System Calls                     : ", // EXC_CTR_SYSCALL
    "External Input Interrupts        : ", // EXC_CTR_STANDARD_INT
    "Critical Input Interrupts        : ", // EXC_CTR_CRITICAL_INT
    "Decrementer Interrupts           : ", // EXC_CTR_DEC
    "Fixed Interval Timer Interrupts  : ", // EXC_CTR_FIT
    "Watchdog Timer Interrupts        : ", // EXC_CTR_WDT
    "User Decrementer Interrupts      : ", // EXC_CTR_UDEC
    "Performance Monitor Interrupts   : ", // EXC_CTR_PERFMON
    "Unknown/Invalid Interrupts       : ", // EXC_CTR_DEBUG+DEBUG_CODE_NONE
    "Debug Interrupts                 : ", // EXC_CTR_DEBUG+DEBUG_CODE_DEBUG
    "Data Storage Interrupts          : ", // EXC_CTR_DEBUG+DEBUG_CODE_DSI
    "Instruction Storage Interrupts   : ", // EXC_CTR_DEBUG+DEBUG_CODE_ISI
    "Alignment Interrupts             : ", // EXC_CTR_DEBUG+DEBUG_CODE_ALGN
    "Program Interrupts               : ", // EXC_CTR_DEBUG+DEBUG_CODE_PROG
    "FPU Unavailable Interrupts       : ", // EXC_CTR_DEBUG+DEBUG_CODE_FPU
    "APU Unavailable Interrupts       : ", // EXC_CTR_DEBUG+DEBUG_CODE_APU
    "Data TLB Interrupts              : ", // EXC_CTR_DEBUG+DEBUG_CODE_DTLB
    "Instruction TLB Interrupts       : ", // EXC_CTR_DEBUG+DEBUG_CODE_ITLB
    "Vector Unavailable Interrupts    : ", // EXC_CTR_DEBUG+DEBUG_CODE_VECT
    "Undefined Interrupts             : ", // EXC_CTR_DEBUG+DEBUG_CODE_UNDEF
    "Processor Doorbell Interrupts    : ", // EXC_CTR_DEBUG+DEBUG_CODE_PDBI
    "Processor Doorbell Critical Ints : ", // EXC_CTR_DEBUG+DEBUG_CODE_PDBCI
    "Guest Doorbell Interrupts        : ", // EXC_CTR_DEBUG+DEBUG_CODE_GDBI
    "Guest Doorbell Crit or MChk Ints : ", // EXC_CTR_DEBUG+DEBUG_CODE_GDBCI
    "Embedded Hypervisor System Calls : ", // EXC_CTR_DEBUG+DEBUG_CODE_EHVSC
    "Embedded Hypervisor Priv Ints    : ", // EXC_CTR_DEBUG+DEBUG_CODE_EHVPRIV
    "Embedded Hypervisor LRAT Ints    : "  // EXC_CTR_DEBUG+DEBUG_CODE_LRATE
};

static void coredump_interrupt_counters(CoreBuffer *buff, int core, int hwthdid)
{
    if (!buff->flags.dumpInterruptCounters)
    {
        return;
    }

    HWThreadState_t *hwt = &NodeState.CoreState[core].HWThreads[hwthdid];

    coredump_printf(buff, "Interrupt Summary:\n");
#define HWTPERFCOUNTER(id, name) if(hwt->PerformanceCounter[id] > 0) coredump_printf(buff, "  %s %lu\n", name, hwt->PerformanceCounter[id]);
#include "cnk/include/kcounters.h"

    return;
}

static void coredump_registers(CoreBuffer *buff, Regs_t *regs)
{
    if (!buff->flags.dumpRegisters)
    {
        return;
    }

    if (buff->flags.dumpGPR)
    {
        coredump_printf(buff, "General Purpose Registers:\n");
        coredump_printf(buff, "  r00=%016lx r01=%016lx r02=%016lx r03=%016lx r04=%016lx r05=%016lx r06=%016lx r07=%016lx\n",
                        regs->gpr[0], regs->gpr[1], regs->gpr[2], regs->gpr[3], regs->gpr[4], regs->gpr[5], regs->gpr[6], regs->gpr[7]);
        coredump_printf(buff, "  r08=%016lx r09=%016lx r10=%016lx r11=%016lx r12=%016lx r13=%016lx r14=%016lx r15=%016lx\n",
                        regs->gpr[8], regs->gpr[9], regs->gpr[10], regs->gpr[11], regs->gpr[12], regs->gpr[13], regs->gpr[14], regs->gpr[15]);
        coredump_printf(buff, "  r16=%016lx r17=%016lx r18=%016lx r19=%016lx r20=%016lx r21=%016lx r22=%016lx r23=%016lx\n",
                        regs->gpr[16], regs->gpr[17], regs->gpr[18], regs->gpr[19], regs->gpr[20], regs->gpr[21], regs->gpr[22], regs->gpr[23]);
        coredump_printf(buff, "  r24=%016lx r25=%016lx r26=%016lx r27=%016lx r28=%016lx r29=%016lx r30=%016lx r31=%016lx\n",
                        regs->gpr[24], regs->gpr[25], regs->gpr[26], regs->gpr[27], regs->gpr[28], regs->gpr[29], regs->gpr[30], regs->gpr[31]);
    }

    if (buff->flags.dumpSPR)
    {
        coredump_printf(buff, "Special Purpose Registers:\n");
        coredump_printf(buff, "  lr=%016lx cr=%016lx xer=%016lx ctr=%016lx\n", regs->lr, regs->cr, regs->xer, regs->ctr);
        coredump_printf(buff, "  msr=%016lx dear=%016lx esr=%016lx fpscr=%016lx\n", regs->msr, regs->dear, regs->esr, regs->fpscr);
        coredump_printf(buff, "  sprg0=%016lx sprg1=%016lx sprg2=%016lx sprg3=%016lx sprg4=%016lx\n",
                        regs->sprg[0], regs->sprg[1], regs->sprg[2], regs->sprg[3], regs->sprg[4]);
        coredump_printf(buff, "  sprg5=%016lx sprg6=%016lx sprg7=%016lx sprg8=%016lx\n", regs->sprg[5], regs->sprg[6], regs->sprg[7], regs->sprg[8]);
        coredump_printf(buff, "  srr0=%016lx srr1=%016lx csrr0=%016lx csrr1=%016lx  mcsrr0=%016lx mcsrr1=%016lx\n",
                        mfspr(SPRN_SRR0_IP), mfspr(SPRN_SRR1_MSR), mfspr(SPRN_CSRR0_IP), mfspr(SPRN_CSRR1_MSR),
                        mfspr(SPRN_MCSRR0_IP), mfspr(SPRN_MCSRR1_MSR));
        coredump_printf(buff, "  dbcr0=%016lx dbcr1=%016lx dbcr2=%016lx dbcr3=%016lx dbsr=%016lx\n",
                        regs->dbcr0, regs->dbcr1, regs->dbcr2, regs->dbcr3, regs->dbsr);
    }

    if (buff->flags.dumpFPR)
    {
        coredump_printf(buff, "Floating Point Registers:\n");
        for (int i = 0; i < NUM_QVRS; i += 2)
        {
            coredump_printf(buff, "  f%02d=%016lx %016lx  %016lx %016lx  f%02d=%016lx %016lx  %016lx %016lx\n",
                            i,   regs->qvr[i].ll[0],   regs->qvr[i].ll[1],   regs->qvr[i].ll[2],   regs->qvr[i].ll[3],
                            i + 1, regs->qvr[i + 1].ll[0], regs->qvr[i + 1].ll[1], regs->qvr[i + 1].ll[2], regs->qvr[i + 1].ll[3]);
        }
    }

    return;
}

static void coredump_personality(CoreBuffer *buff, AppProcess_t *process)
{
    if (!buff->flags.dumpPersonality)
    {
        return;
    }

    Personality_t *personality = GetPersonality();
    coredump_printf(buff, "Personality:\n");
    coredump_printf(buff, "   ABCDET coordinates : %d,%d,%d,%d,%d,%d\n",
                    personality->Network_Config.Acoord,
                    personality->Network_Config.Bcoord,
                    personality->Network_Config.Ccoord,
                    personality->Network_Config.Dcoord,
                    personality->Network_Config.Ecoord,
                    process->Tcoord);
    coredump_printf(buff, "   Rank               : %d\n", process->Rank);
    coredump_printf(buff, "   Ranks per node     : %d\n", process->app->ranksPerNode);
    coredump_printf(buff, "   DDR Size (MB)      : %d\n", personality->DDR_Config.DDRSizeMB);

    return;
}

static void coredump_thread(CoreBuffer *buffer, AppProcess_t *process, KThread_t *kthread, bool includeProcessData, bool includeHwThreadData)
{
    char str[256];

    Regs_t *regs = &(kthread->Reg_State);
    int thread_id = GetTID(kthread);
    int core = kthread->ProcessorID >> 2;
    int hwthdid = kthread->ProcessorID & 0x03;
    TRACE(TRACE_CoreDump, ("%s: dumping thread tgid=%d core=%d hwthdid=%d tid=%d include %d %d\n",
                           __func__, process->PID, core, hwthdid, thread_id, includeProcessData, includeHwThreadData));
    coredump_printf(buffer, "+++ID Rank: %d, TGID: %d, Core: %d, HWTID:%d TID: %d State: %s \n",
                    process->Rank, process->PID, core, hwthdid, thread_id, format_sched_state(kthread->State, str, sizeof(str)));

    if (kthread->SigInfoSigno)
    {
        int sig = kthread->SigInfoSigno;
        coredump_printf(buffer, "***FAULT Encountered unhandled signal 0x%08x (%d) (%s)\n",
                        sig, sig, signal_to_label(sig, str, sizeof(str)));
        if (kthread->ExceptionCode)
        {
            coredump_printf(buffer, "Generated by interrupt..................0x%08x (%s)\n",
                            kthread->ExceptionCode, format_interrupt_code(kthread->ExceptionCode, regs, str, sizeof(str)));
        }
        coredump_printf(buffer, "While executing instruction at..........0x%016lx\n", regs->ip);
        coredump_printf(buffer, "Dereferencing memory at.................0x%016lx\n", regs->dear);
        if (kthread->ExceptionTime)
        {
            coredump_printf(buffer, "Fault occurred at timebase..............0x%016lx\n", kthread->ExceptionTime);
        }
        coredump_printf(buffer, "Tools attached (list of tool ids).......");
        uint32_t toolIds[4];
        int numTools = toolControl.getAttachedTools(process, 4, toolIds);
        if (numTools > 0)
        {
            coredump_printf(buffer, "%u", toolIds[0]);
            for (int i = 1; i < numTools; ++i)
            {
                coredump_printf(buffer, ", %u", toolIds[i]);
            }
            coredump_printf(buffer, "\n");
        } else
        {
            coredump_printf(buffer, "None\n");
        }
    }
    coredump_printf(buffer, "Currently running on hardware thread....%c\n",
                    (NodeState.CoreState[core].HWThreads[hwthdid].pCurrentThread == kthread) ? 'Y' : 'N');
    coredump_registers(buffer, regs);
    coredump_memory(buffer, process, kthread, includeProcessData);
    coredump_traceback(buffer, kthread);
    if (includeHwThreadData)
    {
        coredump_interrupt_counters(buffer, core, hwthdid);
    }

    // If the failure was due to fatal ND or MU interrupts, dump additional information from the ND and MU unit
    if (kthread->SigInfoSigno == SIGMUNDFATAL)
    {
        coredump_ND(buffer);
    }

    coredump_printf(buffer, "---ID\n");

    return;
}

static uint64_t coredump_binary_write(CoreBuffer *buffer, void *data, uint64_t length)
{
    uint64_t rc;
    uint64_t bytesLeft = length;
    char *datap = (char *)data;
    do
    {
        rc = coredump_internal_write(buffer->fileFd, datap, bytesLeft);
        if (CNK_RC_IS_FAILURE(rc))
        {
            TRACE(TRACE_CoreDump, ("%s: write failed for %lu bytes, %d\n", __func__, length, CNK_RC_ERRNO(rc)));
            return rc;
        }

        uint64_t nbytes = CNK_RC_VALUE(rc);
        bytesLeft -= nbytes;
        datap += nbytes;
    }
    while (bytesLeft > 0);

    return length;
}

static int coredump_binary_pad_zeroes(CoreBuffer *buffer, int length)
{
    int bytesWritten = length;
    int bytesToWrite = 0;
    uint64_t rc;

    memset(buffer->buffer, 0, CONFIG_CORE_BUFFER_SIZE);

    while (length > 0)
    {
        if (length >= CONFIG_CORE_BUFFER_SIZE)
        {
            bytesToWrite = CONFIG_CORE_BUFFER_SIZE;
        } else
        {
            bytesToWrite = length;
        }
        rc = coredump_internal_write(buffer->fileFd, buffer->buffer, bytesToWrite);
        if (CNK_RC_IS_SUCCESS(rc))
        {
            length -= CNK_RC_VALUE(rc);
        }
    }

    TRACE(TRACE_CoreDump, ("(I) %s: wrote 0x%x bytes for zero pad\n", __func__, bytesWritten));
    return bytesWritten;
}

static int coredump_binary_auxv_size(void)
{
    int auxEntryCount = 0;
    int auxVecSize = 0;
    uint64_t *src = GetMyProcess()->pAuxVectors;

    while (((*src != 0) || (*(src + 1) != 0)))
    {
        auxEntryCount++;
        src += 2;
    }

    // All entries are counted, return the size in bytes for each entry plus the AT_NULL
    auxVecSize = (auxEntryCount + 1) * 2 * sizeof(uint64_t);

    return auxVecSize;
}

static int coredump_binary_num_threads(AppProcess_t *process)
{
    int numThreads = 0;
    int first_core = process->ProcessLeader_ProcessorID >> 2;
    int first_hwthd = process->ProcessLeader_ProcessorID & 0x3;
    int core_end = CONFIG_MAX_CORES;
    int thd_end = CONFIG_HWTHREADS_PER_CORE;

    // Count the number of active threads in this process.
    for (int core = first_core; core < core_end; core++)
    {
        for (int hwthdid = first_hwthd; hwthdid < thd_end; hwthdid++)
        {
            for (int kindex = 0; kindex < CONFIG_SCHED_KERNEL_SLOT_INDEX; kindex++)
            {
                // get the kthread pointer
                KThread_t *kthread = NodeState.CoreState[core].HWThreads[hwthdid].SchedSlot[kindex];
                if (kthread && (kthread->pAppProc == process))
                {
                    // Only dump kthreads that exist
                    if (!(kthread->State & SCHED_STATE_FREE) &&
                        !(kthread->State & SCHED_STATE_RESET) &&
                        !(kthread->State & SCHED_STATE_POOF))
                    {
                        ++numThreads;
                    }
                }
            }
        }
    }

    TRACE(TRACE_CoreDump, ("(I) %s: there are %d active threads in the process\n", __func__, numThreads));
    return numThreads;
}

static int coredump_binary_elf_phdr(CoreBuffer *buffer, Elf64_Word type, Elf64_Word flags, Elf64_Off offset, Elf64_Addr vaddr, Elf64_Xword filesz, Elf64_Xword align)
{
    // Build the program header for the section.
    Elf64_Phdr *phdr = (Elf64_Phdr *)&buffer->buffer;
    memset((void *)phdr, 0, sizeof(Elf64_Phdr));
    phdr->p_type = type;
    phdr->p_flags = flags;
    phdr->p_offset = offset;
    phdr->p_vaddr = vaddr;
    phdr->p_filesz = filesz;
    phdr->p_memsz = phdr->p_filesz;
    phdr->p_align = align;

    TRACE(TRACE_CoreDump, ("(I) %s: adding program header, offset=0x%lx vaddr=0x%lx filesz=0x%lx memsz=0x%lx\n",
                           __func__, phdr->p_offset, phdr->p_vaddr, phdr->p_filesz, phdr->p_memsz));

    // Write the program header for the section to the core file.
    int bytesWritten = 0;
    uint64_t rc = coredump_internal_write(buffer->fileFd, buffer->buffer, sizeof(Elf64_Phdr));
    if (CNK_RC_IS_SUCCESS(rc))
    {
        bytesWritten += CNK_RC_VALUE(rc);
        TRACE(TRACE_CoreDump, ("(I) %s: wrote 0x%lx bytes for program header type %d\n", __func__, CNK_RC_VALUE(rc), type));
    }

    return bytesWritten;
}

static int coredump_binary_elf_headers(CoreBuffer *buffer, AppProcess_t *process)
{
    uint64_t rc;
    uint64_t vaddr;
    uint64_t paddr;
    uint64_t vsize;
    uint64_t allocSize;
    uint64_t stackAddr;
    uint64_t stackSize;

    int bytesWritten = 0;
    int elfOffset = 0;
    Elf64_Half numPhdr = 0;

    // Check if there is a shared segment.
    if (0 == vmm_getSegment(process->Tcoord, IS_SHAR, &vaddr, &paddr, &vsize))
    {
        if (vsize > 0)
        {
            buffer->dumpSegment[IS_SHAR] = true;
        }
    }
    // Check if there is a dynamic segment.
    if (0 == vmm_getSegment(process->Tcoord, IS_DYNAM, &vaddr, &paddr, &vsize))
    {
        if (vsize > 0)
        {
            buffer->dumpSegment[IS_DYNAM] = true;
        }
    }

    // Count up the number of segments to dump into the core file.
    for (int type = IS_TEXT; type < IS_SEGMENTTYPECOUNT; ++type)
    {
        if (buffer->dumpSegment[type] == true)
        {
            TRACE(TRACE_CoreDump, ("(I) %s: segment type %d is included in core file\n", __func__, type));
            ++numPhdr;
        }
    }
    numPhdr += 2; // Add one for the PT_LOAD for the stack and one for the PT_NOTE for program data

    // Build the elf header for the core file.
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)&buffer->buffer;
    ehdr->e_ident[EI_MAG0] = ELFMAG0;
    ehdr->e_ident[EI_MAG1] = ELFMAG1;
    ehdr->e_ident[EI_MAG2] = ELFMAG2;
    ehdr->e_ident[EI_MAG3] = ELFMAG3;
    ehdr->e_ident[EI_CLASS] = ELFCLASS64; // 64 bit elf
    ehdr->e_ident[EI_DATA] = ELFDATA2MSB; // 2's comp. big endian
    ehdr->e_ident[EI_VERSION] = EV_CURRENT; // Must be EV_CURRENT
    ehdr->e_ident[EI_OSABI] = ELFOSABI_SYSV; // Unix SYS V type elf
    ehdr->e_ident[EI_ABIVERSION] = 0;
    ehdr->e_type = ET_CORE; // Core file
    ehdr->e_machine = EM_PPC64; // PPC64 architecture
    ehdr->e_version = EV_CURRENT; // Current elf version
    ehdr->e_entry = 0;
    ehdr->e_phoff = sizeof(Elf64_Ehdr); // Header table offset is after elf header
    ehdr->e_shoff = 0; // No sections
    ehdr->e_flags = 0;
    ehdr->e_ehsize = sizeof(Elf64_Ehdr); // Size of the elf header
    ehdr->e_phentsize = sizeof(Elf64_Phdr); // Size of program header
    ehdr->e_phnum = numPhdr;
    ehdr->e_shentsize = 0;
    ehdr->e_shnum = 0;
    ehdr->e_shstrndx = 0;

    // Write the elf header to the core file.
    rc = coredump_internal_write(buffer->fileFd, buffer->buffer, sizeof(Elf64_Ehdr));
    if (CNK_RC_IS_SUCCESS(rc))
    {
        bytesWritten = CNK_RC_VALUE(rc);
    }

    TRACE(TRACE_CoreDump, ("(I) %s: wrote 0x%lx bytes for elf header\n", __func__, CNK_RC_VALUE(rc)));

    // PT_NOTE data is placed after the program headers.
    elfOffset = ROUND_UP_512B(sizeof(Elf64_Ehdr) + (ehdr->e_phnum * sizeof(Elf64_Phdr)));

    // NOTE: We re-use the buffer in the CoreBuffer structure after each write to the core file so
    // the elf header is no longer available.

    // The note section contains the following:
    //  NT_PRPSINFO - Exe name, some of args etc
    //  NT_AUXV - auxiliary vectors
    //  NT_PRSTATUS - GPRS and process id (one for each active thread)
    //  NT_FPREGSET - Floating regs (one for each active thread)

    // Calculate the total size of the headers and data for the all of the notes.
    int numThreads = coredump_binary_num_threads(process);
    int numNotes = 2 + (numThreads * 2);
    Elf64_Xword noteSize =
       (numNotes * sizeof(Elf64_Nhdr)) +                                // Header for every note
       (numNotes * ROUND_UP_WORD(sizeof(CORE_NAME))) +                  // Name for every note
       sizeof(prpsinfo_t) + coredump_binary_auxv_size() +               // 1 PRPSINFO note and 1 AUX note
       (numThreads * sizeof(prstatus_t)) +                              // 1 PRSTATUS note for each thread
       (numThreads * (sizeof(prfpregset_t) * 4 - (3 * sizeof(elf_fpreg_t)))); // 1 FPREGSET (quad size) note for each thread

    bytesWritten += coredump_binary_elf_phdr(buffer, PT_NOTE, 0, elfOffset, 0, noteSize, 0);

    // PT_LOAD data is placed after PT_NOTE data.
    elfOffset = ROUND_UP_4K(elfOffset + noteSize);

    // Calculate and write out the PT_LOAD program headers
    // NOTE: Might need to scan if section contains all zeroes, currently dumping regardless
    for (int type = IS_TEXT; type <= IS_SEGMENTTYPECOUNT; ++type)
    {
        if (buffer->dumpSegment[type] == false)
        {
            continue;
        }

        // Get info about the segment.
        if (0 == vmm_getSegment(process->Tcoord, (enum SegmentType)type, &vaddr, &paddr, &vsize))
        {
            if (vsize > 0)
            {
                switch (type)
                {
                case IS_TEXT:
                case IS_DYNAM:
                    bytesWritten += coredump_binary_elf_phdr(buffer, PT_LOAD, PF_R | PF_X, elfOffset, vaddr, vsize, 0x1000);
                    elfOffset += ROUND_UP_4K(vsize);
                    break;

                case IS_DATA:
                case IS_SHAR:
                    bytesWritten += coredump_binary_elf_phdr(buffer, PT_LOAD, PF_R | PF_W, elfOffset, vaddr, vsize, 0x1000);
                    elfOffset += ROUND_UP_4K(vsize);
                    break;

                case IS_HEAP:
                    // Write the mmap section of the heap.
                    allocSize = MAX(process->Heap_Break, process->MmapMgr.high_mark) - vaddr;
                    bytesWritten += coredump_binary_elf_phdr(buffer, PT_LOAD, PF_R | PF_W, elfOffset, vaddr, allocSize, 0x1000);
                    elfOffset += ROUND_UP_4K(allocSize);

                    // Write the stack section of the heap.
                    stackAddr = ROUND_DN_4K(process->ProcessLeader_KThread->Reg_State.gpr[1]);
                    stackSize = vaddr + vsize - stackAddr;
                    bytesWritten += coredump_binary_elf_phdr(buffer, PT_LOAD, PF_R | PF_W, elfOffset, stackAddr, stackSize, 0x1000);
                    elfOffset += ROUND_UP_4K(stackSize);
                    break;

                default:
                    printf("(E) %s: type %d was not written to core file\n", __func__, type);
                    break;
                }
            }
        }
    }

    return bytesWritten;
}

static int coredump_binary_prstatus(CoreBuffer *buffer, KThread_t *kthread, int tid)
{
    uint64_t now = GetCurrentTimeInMicroseconds() - GetMyAppState()->JobStartTime;

    // Clear the buffer.
    memset(buffer->buffer, 0, CONFIG_CORE_BUFFER_SIZE);

    // Build the header for the PRSTATUS note.
    Elf64_Nhdr *nhdr = (Elf64_Nhdr *)&buffer->buffer;
    nhdr->n_namesz = sizeof(CORE_NAME);
    nhdr->n_descsz = sizeof(prstatus_t);
    nhdr->n_type = NT_PRSTATUS;

    // Put the name right after the header.
    char *name = (char *)((buffer->buffer) + sizeof(Elf64_Nhdr));
    int nameLength = ROUND_UP_WORD(nhdr->n_namesz);
    memset(name, 0, nameLength);
    strncpy(name, CORE_NAME, nameLength);

    // Put the data right after the name.
    prstatus_t *prstatus = (prstatus_t *)(name + nameLength);
    memset((void *)prstatus, 0, sizeof(prstatus_t));

    Regs_t *regs = &kthread->Reg_State;
    if (kthread->SigInfoSigno != 0)
    {
        prstatus->pr_info.si_signo = kthread->SigInfoSigno;  // Signal number
        prstatus->pr_info.si_code  = kthread->SigInfoCode;   // Extra code
    }
    prstatus->pr_cursig = kthread->SigInfoSigno;  // Current signal
    prstatus->pr_sigpend = 0;  // Pending signals
    prstatus->pr_sighold = 0;  // Held signals
    prstatus->pr_pid = tid;  //process->TGID;  // Pid
    prstatus->pr_ppid = tid; //process->TGID;  // Parent pid same as pid since can't create child
    prstatus->pr_pgrp = 0;  // No group since can't fork
    prstatus->pr_sid = 0;
    prstatus->pr_utime.tv_sec  = now / MILLION;
    prstatus->pr_utime.tv_usec = now % MILLION;
    // also pr_stime, pr_cutime, pr_sutime
    memcpy(prstatus->pr_reg, regs->gpr, sizeof(regs->gpr)); // First 32 registers in pr_reg are the GPRs
    prstatus->pr_reg[PT_NIP] = regs->ip;
    prstatus->pr_reg[PT_MSR] = regs->msr;
    prstatus->pr_reg[PT_ORIG_R3] = 0;
    prstatus->pr_reg[PT_CTR] = regs->ctr;
    prstatus->pr_reg[PT_LNK] = regs->lr;
    prstatus->pr_reg[PT_XER] = regs->xer;
    prstatus->pr_reg[PT_CCR] = regs->cr;
    prstatus->pr_reg[PT_SOFTE] = 0;
    prstatus->pr_reg[PT_TRAP] = 0;
    prstatus->pr_reg[PT_DAR] = regs->dear;
    prstatus->pr_reg[PT_DSISR] = regs->esr;
    prstatus->pr_reg[PT_RESULT] = 0;
    prstatus->pr_fpvalid = 0;

    // Write the PRSTATUS note to the core file.
    TRACE(TRACE_CoreDump, ("(I) %s: adding prstatus note for thread %d, signal %d\n", __func__, prstatus->pr_pid, prstatus->pr_cursig));
    int bytesWritten = 0;
    uint64_t rc = coredump_internal_write(buffer->fileFd, buffer->buffer, (sizeof(Elf64_Nhdr) + sizeof(prstatus_t) + nameLength));
    if (CNK_RC_IS_SUCCESS(rc))
    {
        bytesWritten += CNK_RC_VALUE(rc);
        TRACE(TRACE_CoreDump, ("(I) %s: wrote 0x%lx bytes for prstatus note for thread %d, signal %d\n",
                               __func__, CNK_RC_VALUE(rc), prstatus->pr_pid, prstatus->pr_cursig));
    }

    return bytesWritten;
}

static int coredump_binary_prpsinfo(CoreBuffer *buffer, AppProcess_t *process)
{
    // Clear the buffer.
    memset(buffer->buffer, 0, CONFIG_CORE_BUFFER_SIZE);

    // Build the header for the PRPSINFO note.
    Elf64_Nhdr *nhdr = (Elf64_Nhdr *)&buffer->buffer;
    nhdr->n_namesz = sizeof(CORE_NAME);
    nhdr->n_descsz = sizeof(prpsinfo_t);
    nhdr->n_type = NT_PRPSINFO;

    // Put the name right after the header.
    char *name = (char *)((buffer->buffer) + sizeof(Elf64_Nhdr));
    int nameLength = ROUND_UP_WORD(nhdr->n_namesz);
    memset(name, 0, nameLength);
    strncpy(name, CORE_NAME, nameLength);

    // Put the data right after the name.
    AppState_t *pAppState = GetMyAppState();
    prpsinfo_t *prpsinfo = (prpsinfo_t *)(name + nameLength);
    prpsinfo->pr_state = 0;
    prpsinfo->pr_sname = 'R';
    prpsinfo->pr_zomb = 0;
    prpsinfo->pr_nice = 0;
    prpsinfo->pr_flag = 0; // Not sure what would make sense for this field
    prpsinfo->pr_uid = (pAppState->UserID ? pAppState->UserID : 1);
    prpsinfo->pr_gid = (pAppState->GroupID ? pAppState->GroupID : 1);
    prpsinfo->pr_pid = process->PID;
    prpsinfo->pr_ppid = 1; // Pretend process was forked from init process
    prpsinfo->pr_pgrp = process->PID;
    prpsinfo->pr_sid = process->Rank; // Communicate the rank using session id

    // Copy the arguments.
    char *args;
    if (process->DYN_VStart != 0)
    {
        // For a dynamically linked program, skip the first argument which is the path to the interpreter.
        args = &(pAppState->App_Args[0]) + strlen(pAppState->App_Args) + 1;
    } else
    {
        args = &(pAppState->App_Args[0]);
    }
    memcpy(prpsinfo->pr_psargs, args, sizeof(prpsinfo->pr_psargs));

    // Find the file name in the path to the executable.
    char *filename = args;
    char *p = filename;
    while (*p != '\0') // Find the last slash in the path.
    {
        while ((*p != '\0') && (*p != '/')) ++p;
        if (*p == '/') filename = ++p;
    }
    size_t filenameLength = strlen(filename);
    if (filenameLength > sizeof(prpsinfo->pr_fname))
    {
        filenameLength = sizeof(prpsinfo->pr_fname);
    }
    memcpy(prpsinfo->pr_fname, filename, filenameLength);

    // Write the PRPSINFO note to the core file.
    TRACE(TRACE_CoreDump, ("(I) %s: adding prpsinfo note for program %s\n", __func__, prpsinfo->pr_psargs));
    int bytesWritten = 0;
    uint64_t rc = coredump_internal_write(buffer->fileFd, buffer->buffer, (sizeof(Elf64_Nhdr) + nameLength + nhdr->n_descsz));
    if (CNK_RC_IS_SUCCESS(rc))
    {
        bytesWritten += CNK_RC_VALUE(rc);
        TRACE(TRACE_CoreDump, ("(I) %s: wrote 0x%lx bytes for prpsinfo note\n", __func__, CNK_RC_VALUE(rc)));
    }

    return bytesWritten;
}

static int coredump_binary_auxv(CoreBuffer *buffer, AppProcess_t *process)
{
    // Clear the buffer.
    memset(buffer->buffer, 0, CONFIG_CORE_BUFFER_SIZE);

    // Build the header for the AUXV note.
    Elf64_Nhdr *nhdr = (Elf64_Nhdr *)&buffer->buffer;
    nhdr->n_namesz = sizeof(CORE_NAME);
    nhdr->n_descsz = coredump_binary_auxv_size();
    nhdr->n_type = NT_AUXV;

    // Put the name right after the header.
    char *name = (char *)((buffer->buffer) + sizeof(Elf64_Nhdr));
    int nameLength = ROUND_UP_WORD(nhdr->n_namesz);
    memset(name, 0, nameLength);
    strncpy(name, CORE_NAME, nameLength);

    // Put the data right after the name.
    memcpy((void *)((buffer->buffer) + sizeof(Elf64_Nhdr) + nameLength), process->pAuxVectors, nhdr->n_descsz);

    // Write the AUXV note to the core file.
    TRACE(TRACE_CoreDump, ("(I) %s: adding auxv note with %d bytes of auxv data\n", __func__, nhdr->n_descsz));
    int bytesWritten = 0;
    uint64_t rc = coredump_internal_write(buffer->fileFd, buffer->buffer, (sizeof(Elf64_Nhdr) + nameLength + nhdr->n_descsz));
    if (CNK_RC_IS_SUCCESS(rc))
    {
        bytesWritten += CNK_RC_VALUE(rc);
        TRACE(TRACE_CoreDump, ("(I) %s: wrote 0x%lx bytes for auxv note\n", __func__, CNK_RC_VALUE(rc)));
    }

    return bytesWritten;
}

static int coredump_binary_prfpregs(CoreBuffer *buffer, KThread_t *kthread, int tid)
{
    Regs_t *regs = &kthread->Reg_State;
    int bytesWritten = 0;
    uint64_t *floatRegs;
    Elf64_Nhdr *nhdr;
    int bytesProcessed = 0;
    int i = 0;
    int j = 0;
    char *pNoteName;
    uint64_t rc;

    // Set up and write the NT_PRFPREG
    nhdr = (Elf64_Nhdr *)&buffer->buffer;
    memset((void *)nhdr, 0, sizeof(Elf64_Nhdr));
    nhdr->n_namesz = sizeof(CORE_NAME);

    // Include qpx floating regs.  So quadruple the fpregset and take away three leaving room for the one fpscr
    nhdr->n_descsz = (4 * sizeof(prfpregset_t)) - (3 * sizeof(elf_fpreg_t));
    nhdr->n_type = NT_FPREGSET;

    // Set up and write the note name
    pNoteName = (char *)((buffer->buffer) + sizeof(Elf64_Nhdr));
    memset(pNoteName, 0, ROUND_UP_WORD(nhdr->n_namesz));
    strncpy(pNoteName, CORE_NAME, ROUND_UP_WORD(nhdr->n_namesz));

    floatRegs = (uint64_t *)(pNoteName + ROUND_UP_WORD(nhdr->n_namesz));

    // initialize bytesProcessed to write the name header and name on first iteration
    bytesProcessed = sizeof(Elf64_Nhdr) + ROUND_UP_WORD(nhdr->n_namesz);

    TRACE(TRACE_CoreDump, ("(I) %s: adding prfpregs note for thread %d\n", __func__, tid));
    for (i = 0; i < (ELF_NFPREG - 1); i++)
    {
        for (j = 0; j < 4; j++)
        {
            *floatRegs++ = regs->qvr[i].ll[j];
            bytesProcessed += sizeof(elf_fpreg_t);
        }
        if (!(i % 8)) // break up the writing of the registers so we do not exceed the buffer size
        {
            rc = coredump_internal_write(buffer->fileFd, buffer->buffer, bytesProcessed);
            if (CNK_RC_IS_SUCCESS(rc))
            {
                bytesWritten += CNK_RC_VALUE(rc);
            }
            // reset for next set of registers
            bytesProcessed = 0;
            floatRegs = (uint64_t *)(buffer->buffer);
        }
    }
    // The fpscr needs to be adjusted
    *floatRegs = (uint64_t)regs->fpscr;
    bytesProcessed += sizeof(elf_fpreg_t);

    // Write out the remaing floats and the fpscr
    rc = coredump_internal_write(buffer->fileFd, buffer->buffer, bytesProcessed);
    if (CNK_RC_IS_SUCCESS(rc))
    {
        bytesWritten += CNK_RC_VALUE(rc);
        TRACE(TRACE_CoreDump, ("(I) %s: wrote 0x%lx bytes for prfpregs note\n", __func__, CNK_RC_VALUE(rc)));
    }
    return (bytesWritten);
}

static int coredump_binary_threads(CoreBuffer *buffer, AppProcess_t *process)
{
    int bytesWritten = 0;

    // Add notes for the other threads.
    KThread_t *failedThread =  (process->coredump_kthread ? process->coredump_kthread : process->ProcessLeader_KThread);
    int first_core = process->ProcessLeader_ProcessorID >> 2;
    int first_hwthd = process->ProcessLeader_ProcessorID & 0x3;
    int core_end = CONFIG_MAX_CORES;
    int thd_end = CONFIG_HWTHREADS_PER_CORE;
    for (int core = first_core; core < core_end; core++)
    {
        for (int hwthdid = first_hwthd; hwthdid < thd_end; hwthdid++)
        {
            for (int kindex = 0; kindex < CONFIG_SCHED_KERNEL_SLOT_INDEX; kindex++)
            {
                // get the kthread pointer
                KThread_t *kthread = NodeState.CoreState[core].HWThreads[hwthdid].SchedSlot[kindex];
                if (kthread && (kthread != failedThread) && (kthread->pAppProc == process))
                {
                    // Only dump kthreads that exist
                    if (!(kthread->State & SCHED_STATE_FREE) &&
                        !(kthread->State & SCHED_STATE_RESET) &&
                        !(kthread->State & SCHED_STATE_POOF))
                    {
                        bytesWritten += coredump_binary_prstatus(buffer, kthread, GetTID(kthread));
                        bytesWritten += coredump_binary_prfpregs(buffer, kthread, GetTID(kthread));
                    }
                }
            }
        }
    }

    return bytesWritten;
}

static int coredump_binary_notes(CoreBuffer *buffer, AppProcess_t *process)
{
    int bytesWritten = 0;

    // First, write the PRSTATUS note for the failed thread to the core file. If no failed thread,
    // then treat the process leader thread as the failed thread.
    KThread_t *failedThread =  (process->coredump_kthread ? process->coredump_kthread : process->ProcessLeader_KThread);

    bytesWritten += coredump_binary_prstatus(buffer, failedThread, GetTID(failedThread));

    // Second, write the PRPSINFO note with the process state info to the core file.
    bytesWritten += coredump_binary_prpsinfo(buffer, process);

    // Third, write the AUXV note with auxiliary vector to the core file.
    bytesWritten += coredump_binary_auxv(buffer, process);

    bytesWritten += coredump_binary_prfpregs(buffer, failedThread, GetTID(failedThread));

    // Fifth, write PRSTATUS and PRFPREGS notes for the other threads.
    bytesWritten += coredump_binary_threads(buffer, process);

    return bytesWritten;
}

static uint64_t coredump_binary_memory(CoreBuffer *buffer, AppProcess_t *process)
{
    uint64_t bytesWritten = 0;
    uint64_t vaddr = 0;
    uint64_t paddr = 0;
    uint64_t vsize = 0;
    uint64_t rc;

    // Loop thru all of the segments writing out memory.
    for (int type = IS_TEXT; type < IS_SEGMENTTYPECOUNT; ++type)
    {
        // Make sure this is a segment type we should put in the core file.
        if (buffer->dumpSegment[type] == false)
        {
            continue;
        }
        // Does the segment exist?
        if (0 == vmm_getSegment(process->Tcoord, (enum SegmentType)type, &vaddr, &paddr, &vsize))
        {
            if (vsize == 0)
            {
                continue;
            }
        } else
        {
            continue;
        }
        // Special processing for the heap segment -- only dump out the mmap'ed region at the
        // beginning of the segment, and the stack, starting from r1 to the end.
        // Assume vaddr is the start of the segment.

        if (type == IS_HEAP)
        {
            uint64_t mmapSize = max(process->MmapMgr.high_mark, process->Heap_Break) - vaddr;
            TRACE(TRACE_CoreDump, ("(I) %s: adding heap from vaddr 0x%lx size 0x%lx\n", __func__, vaddr, mmapSize));
            rc = coredump_binary_write(buffer, (void *)vaddr, mmapSize);
            if (CNK_RC_IS_SUCCESS(rc))
            {
                bytesWritten += CNK_RC_VALUE(rc);
                TRACE(TRACE_CoreDump, ("(I) %s: wrote 0x%lx bytes for heap\n", __func__, CNK_RC_VALUE(rc)));
            }

            int bytesToPad = ROUND_UP_4K(mmapSize) - mmapSize;
            if (bytesToPad > 0)
            {
                bytesWritten += coredump_binary_pad_zeroes(buffer, bytesToPad);
            }

            uint64_t stackAddr = ROUND_DN_4K(process->ProcessLeader_KThread->Reg_State.gpr[1]);
            uint64_t stackSize = vaddr + vsize - stackAddr;
            TRACE(TRACE_CoreDump, ("(I) %s: adding stack from vaddr 0x%lx size 0x%lx\n", __func__, stackAddr, stackSize));
            rc = coredump_binary_write(buffer, (void *)stackAddr, stackSize);
            if (CNK_RC_IS_SUCCESS(rc))
            {
                bytesWritten += CNK_RC_VALUE(rc);
                TRACE(TRACE_CoreDump, ("(I) %s: wrote 0x%lx bytes for stack\n", __func__, CNK_RC_VALUE(rc)));
            }

            bytesToPad = ROUND_UP_4K(stackSize) - stackSize;
            if (bytesToPad > 0)
            {
                bytesWritten += coredump_binary_pad_zeroes(buffer, bytesToPad);
            }
        } else
        {
            TRACE(TRACE_CoreDump, ("(I) %s: adding segment type %d from vaddr 0x%lx size 0x%lx\n", __func__, type, vaddr, vsize));
            rc = coredump_binary_write(buffer, (void *)vaddr, vsize);
            if (CNK_RC_IS_SUCCESS(rc))
            {
                bytesWritten += CNK_RC_VALUE(rc);
                TRACE(TRACE_CoreDump, ("(I) %s: wrote 0x%lx bytes for segment type %d\n", __func__, CNK_RC_VALUE(rc), type));
            }
            int bytesToPad = ROUND_UP_4K(vsize) - vsize;
            if (bytesToPad > 0)
            {
                bytesWritten += coredump_binary_pad_zeroes(buffer, bytesToPad);
            }
        }

    }  // End for loop to write out memory segments

    return (bytesWritten);
}

static void coredump_binary(CoreBuffer *buffer)
{
    AppProcess_t *process = GetMyProcess();

    // Write the elf headers to the core file.
    int bytesWritten = coredump_binary_elf_headers(buffer, process);

    // Write pad to the core file to get to 512 byte boundary.
    bytesWritten += coredump_binary_pad_zeroes(buffer, (ROUND_UP_512B(bytesWritten) - bytesWritten));
    TRACE(TRACE_CoreDump, ("(I) %s: wrote elf headers, core file offset is 0x%x\n", __func__, bytesWritten));

    // Write the various notes in the PT_NOTE section.
    bytesWritten += coredump_binary_notes(buffer, process);

    // Write pad to the core file to get to 4K byte boundary.
    bytesWritten += coredump_binary_pad_zeroes(buffer, (ROUND_UP_4K(bytesWritten) - bytesWritten));
    TRACE(TRACE_CoreDump, ("(I) %s: wrote PT_NOTE section, core file offset is 0x%x\n", __func__, bytesWritten));

    // Write the PT_LOAD sections.
    bytesWritten += coredump_binary_memory(buffer, process);
    TRACE(TRACE_CoreDump, ("(I) %s: wrote PT_LOAD sections, core file offset is 0x%x\n", __func__, bytesWritten));

    return;
}

void DumpCore(void)
{
    Kernel_Lock(&NodeState.coredumpLock); // Protect the signal coredump buffer
    AppProcess_t *process = GetMyProcess();
    KThread_t *kthread;


    if (!AppAgentIdentifier(process) && (process->Rank >= GetMyAppState()->ranksActive))   // Cannot use ProcessState_RankInactive since this may be called during ExitPending
        return;

    const char *programname = &(GetMyAppState()->App_Args[0]);
    if (programname[0] == 0) programname = "<Unknown>";

    if (coredump_open(&buff, process) == 0)
    {

        // Set the dumpBinary flag. Note that the binaryCoredump field in the process can have a value of 0:no binary, 1:explicit rank, 2:all ranks binary.
        buff.flags.dumpBinary = process->binaryCoredump ? 1 : 0;
        if (buff.flags.dumpBinary)
        {
            if (!Personality_CiosEnabled())
            {
                printf("(E) Binary core files are only supported when CIOS is enabled\n");
                Kernel_Unlock(&NodeState.coredumpLock);
                return;
            }
            coredump_binary(&buff);
            coredump_close(&buff);
            Kernel_Unlock(&NodeState.coredumpLock);
            return;
        }

        // Write the light-weight header.
        coredump_printf(&buff, "+++PARALLEL TOOLS CONSORTIUM LIGHTWEIGHT COREFILE FORMAT version 1.0\n");
        coredump_printf(&buff, "+++LCB 1.0\n");
        coredump_printf(&buff, "Program   : %s\n", programname);
        coredump_printf(&buff, "Job ID    : %lu\n", GetMyAppState()->JobID);
        coredump_personality(&buff, process);

        // Get a list of hardware threads associated with the current process
        int core, hwthdid;
        int first_core = process->ProcessLeader_ProcessorID >> 2;
        int first_hwthd = process->ProcessLeader_ProcessorID & 0x3;
        int core_end = CONFIG_MAX_CORES;
        int thd_end = CONFIG_HWTHREADS_PER_CORE;
        KThread_t *failedThread =  (process->coredump_kthread ? process->coredump_kthread : GetMyKThread());

        // If we are writing in a simulation mode we may want to limit the amount of data we are dumping to just the current hwthread
        uint32_t coredumpscope = CONFIG_CORE_SCOPE_DEFAULT;
        App_GetEnvValue("BG_COREDUMPSCOPE", &coredumpscope);
        if (coredumpscope == CONFIG_CORE_SCOPE_THREAD)
        {
            // Set these variables so the for loop below does not run the list.
            first_core = 0;
            core_end = 0;
        }

        // Dump the failed thread first and include the process data.
        coredump_thread(&buff, process, failedThread, true, true);

        // Dump the rest of the valid threads in the process.
        for (core = first_core; core < core_end; core++)
        {
            for (hwthdid = first_hwthd; (hwthdid < thd_end); hwthdid++)
            {
                bool includeHwThreadData = true;
                int kindex;
                for (kindex = 0; (kindex < CONFIG_SCHED_KERNEL_SLOT_INDEX); kindex++)
                {
                    // get the kthread pointer
                    kthread = NodeState.CoreState[core].HWThreads[hwthdid].SchedSlot[kindex];
                    if (kthread && (kthread != failedThread) && (kthread->pAppProc == process))
                    {
                        // Only dump kthreads that exist
                        if (!(kthread->State & SCHED_STATE_FREE) &&
                            !(kthread->State & SCHED_STATE_RESET) &&
                            !(kthread->State & SCHED_STATE_POOF))
                        {
                            coredump_thread(&buff, process, kthread, false, includeHwThreadData);
                            includeHwThreadData = false;
                        }
                    }
                }
            }
        }
        // Write the trailer and close the core dump.
        coredump_printf(&buff, "---LCB\n");
        coredump_close(&buff);
    }
    else
    {
        //printf("DumpCore open failed for rank:%d\n", GetMyProcess()->Rank);
    }
    Kernel_Unlock(&NodeState.coredumpLock);
}
void DumpInterruptCounters(void)
{

    CoreBuffer buff;
    coredump_open(&buff, GetMyProcess());

    // Force dump to the mailbox.
    buff.flags.dumpToMailbox = 1;
    buff.flags.disableBuffering = 1;
    buff.flags.dumpInterruptCounters = 1;

    coredump_interrupt_counters(&buff, ProcessorCoreID(), ProcessorThreadID());

    coredump_close(&buff);
    return;
}

int coredump_for_rank(AppProcess_t *pProc)
{
    const char *ranks;
    const char *tmpPtr;
    char value[25];
    int badParmFound = 0;
    uint32_t rank = pProc->Rank;

    // User wants a dump of a specific rank?
    if (App_GetEnvString("BG_COREDUMPRANK", &ranks))
    {
        // check to see if this rank was in the list
        tmpPtr = ranks;
        while (*tmpPtr)
        {
            for (; *tmpPtr; tmpPtr++)
            {
                if ((*tmpPtr == ','))
                {
                    break;
                }
            }  // End loop looking for a delimiter
            int badValue_local = 0;
            uint32_t str_size = tmpPtr - ranks;
            // limit damage of bogus length rank value and leave space for null terminator
            if (sizeof(value) <= str_size)
            {
                str_size = (sizeof(value) - 1);
                badValue_local = 1;
                badParmFound++;
            }
            // the above check should seldom fail so just fall through
            // as we have already marked the value as an error
            strncpy(value, ranks, str_size);
            *(value + str_size) = 0; // null terminate the string to prep for atoi conversion

            // do we have a match and was the conversion valid
            uint32_t rank_from_string = (uint32_t)atoi_(value);
            // the supplied string may have been bogus
            uint32_t i;
            for (i = 0; i < str_size; i++)
            {
                if ((value[i] < 0x30) || (value[i] > 0x39))
                {
                    badValue_local = 1;
                    badParmFound++;
                }
            }
            if ((rank == (uint32_t)rank_from_string) && !badValue_local)
            {
                return 1;
            }
            if (!*tmpPtr)
            {
                break;
            }
            // Move base pointer and try next value
            tmpPtr++;
            ranks = tmpPtr;
        }  // End loop looking for rank in env vars
    }
    return 0;
}
