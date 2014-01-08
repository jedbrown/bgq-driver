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
ENTRY(Kernel_Start)
SEARCH_DIR("/auto/BGQ/releases/bgqdev/floor-x86_64-2008-05-06/crosstools/powerpc64-quattro-linux/lib");

SECTIONS
{
  . = 0x00000000;
  .text :
     {
      *(.ktext.abs0)
      *(.ktext.start)

     . = ALIGN(0x1000);  /* Interrupt Vectors: (lower 12bits of vector start must be zero) */
     *(.ktext.vectors)
     *(.ktext)
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

     /* Environment variable area */
     *(.fwext_env_vars)
     }

  .opd ALIGN(64) :
     {
     *(.opd)
     }

  .toc :
     {
     *(.toc)
     }

  .bss :
     {
     *(.bss)
     }

}

