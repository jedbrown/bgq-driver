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

/* Script for -z combreloc: combine and sort reloc sections */
OUTPUT_FORMAT("elf64-powerpc", "elf64-powerpc",
	      "elf64-powerpc")
OUTPUT_ARCH(powerpc:common64)
ENTRY(_ResetVector)
SEARCH_DIR("/auto/BGQ/releases/bgqdev/floor-x86_64-2008-05-06/crosstools/powerpc64-quattro-linux/lib");

SECTIONS
{
  . = 0x03fffffc0000;
  .fw_data :
     {
     /* Allocate space for 128 BeDRAM Locks (128 * 8B = 1KB) */
     *(.fw_locks)
     *(.fw_suart)
     *(.fw_termstatus)
          
     /* Personality: 256 Bytes */
     *(.fw_personality)

     /* Mailbox to Core:  4KB: 0x3fffffc0500 - 0x3fffffc14ff */
     *(.MailboxToCore)

     /* Mailbox to Host: 16KB: 0x3fffffc1500 - 0x3fffffc54ff */
     *(.MailboxToHost)

     /* Per-Thread Stacks: 68 * 1024B: size = 0x11000: 0x3fffffc5500 - 0x3fffffd64ff */
     *(.fw_stack0)
     *(.fw_stacks)
     } =0x00000000

  .text :
     {
     /* Interrupt Vectors: (lower 12bits of vector start must be zero) */
     . = ALIGN(0x1000);
     *(.fw_vectors)
     *(.text)
     }

  .rodata :
     {
     . = ALIGN(64);
     *(.rodata .rodata.* .gnu.linkonce.r.*)
     *(.rodata1)
     }

  .data :
     {
     *(.data)
     }

  .opd ALIGN(64) :
     {
     *(.opd)
     }

  PROVIDE( __BeDRAM_TOC_BASE = (. + 0x8000) );
  .toc :
     {
     *(.toc)
     }

  .bss :
     {
     *(.bss)
     }

  /* Reset Page: uppermost 4KB area (partially used by firmware) for initial boot code */
  /*   Note: Incease the start address until this just fits above the next section.    */
  . = 0x03fffffff000;
  .reset_page :
     {
     *(.fw_text.reset)
     *(.fw_data.reset)
     *(.fw_tlb_table)
     }

  /* Firmware State: Reset Counter, BeDRAM CRC, and various flags  */
  . = 0x03fffffffff0;
  .fw_state :
     {
     *(.fw_state)
     }

  /* Single Instruction to Branch to text.reset entry point. */
  . = 0x03fffffffffc;
  .fw_resetvector  :
     {
     *(.fw_resetvector)
     }
}

