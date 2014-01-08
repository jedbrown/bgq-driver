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
/* (C) Copyright IBM Corp.  2007, 2011                              */
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


OUTPUT_FORMAT("elf64-powerpc", "elf64-powerpc", "elf64-powerpc")
OUTPUT_ARCH(powerpc)

PHDRS
{
   headers          PT_PHDR PHDRS ;
   text_kernel      PT_LOAD ;
   kuncached        PT_LOAD ;
   ktransient       PT_LOAD ;
   kstack           PT_LOAD ;
   data_kernel      PT_LOAD ;
   text             PT_LOAD ;
   data             PT_LOAD ;
   debug            PT_NOTE ;
}

SECTIONS
{
  /* Kernel Code starts at base of eDRAM Scratch and includes Read-Only Data sections */
  . = 0x7FFF8000;
  .ktext :
  {
     __BGP_KERNEL_TEXT_START = .;
     *(.kernel_checksum)
     *(.ktext.start)
     *(.ktext.exceptions)
     *(.ktext)
     . = ALIGN(32);
     _ektext = .;
     *(.krodata)
     *(.krodata1)
     . = ALIGN(32);
     __initktext_begin = .;
     *(.ktext.init)
     __initktext_end   = .;
     . = ALIGN(4096);
     *(.text)
     _etext = .;
     PROVIDE (etext = .);
    *(.rodata*)
    *(.rodata1)
    CONSTRUCTORS
    . = ALIGN(32);
    __CTORS_START = .;
    *(.ctors.*)
    *(.ctors)
    __CTORS_END = .;
    __DTORS_START = .;
    *(.dtors.*)
    __DTORS_END = .;
    *(.except)
    KEEP(*(.gnu.linkonce.t.*))
    KEEP(*(.text_pad_for_sim))
    __ktext_end = .;
    __BGP_KERNEL_TEXT_END = .;
  } :text_kernel

  . = ALIGN(1048576);
  /* Read-write section, merged into data segment: */
  /*  Kernel Data starts at multiple of 1MB        */
  .kdata      :
  {
     __BGP_KERNEL_DATA_START = .;
     *(.kdata.nodestate)
     __kconfig_start = .;
     *(.kdata.config)
     __kconfig_end   = .;
     __kbss_start = .;
     *(.sbss) *(.scommon)
     *(.dynbss)
     *(.kbss)
     *(COMMON)
     __kbss_end = .;
    . = ALIGN(32);
    *(.kdata.cacheline_aligned)
    *(.kdata)
    *(.kdata1)
    *(.ksdata)
    *(.ksdata2)
    __initkdata_begin = .;
    *(kdata.init)
    __initkdata_end   = .;

     __bss_start = .;
     *(.sbss) *(.scommon)
     *(.dynbss)
     *(.bss)
     *(COMMON)
     __bss_end = .;
    . = ALIGN(32);
    *(.data.cacheline_aligned)
    *(.data)
    *(.data1)
    __SDA_BASE = .;
    *(.sdata)
    __SDA2_BASE = .;
    *(.sdata2)
    *(.got.plt) *(.got)
    *(.dynamic)
    *(.eh_frame)
    KEEP(*(.gnu.linkonce.r.*))
    PROVIDE (ekdata = .);
    KEEP(*(.data_pad_for_sim))
    __ekdata  =  .;
    __kdata_end = .;
    __BGP_KERNEL_DATA_END = .;
  } :data_kernel

  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  .comment        0 : { *(.comment) }
}

