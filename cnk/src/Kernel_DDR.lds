/*------------------------------------------------------------------*/
/*                                                                  */
/* (C) Copyright IBM Corp.  2007, 2007                              */
/* IBM CPL License                                                  */
/*                                                                  */
/*------------------------------------------------------------------*/
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
      PROVIDE( __KERNEL_TEXT_START = . );
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
     PROVIDE( __KERNEL_TEXT_END = . );
     }

     . = ALIGN(0x100000);  /* Align up to the next page boundary */

  .atomics :
     {
     PROVIDE( __KERNEL_DATA_START = . );
     PROVIDE( __KERNEL_ATOMICS_START = . );
     *(.atomics)
     PROVIDE( __KERNEL_ATOMICS_END = . );
     } 

  .data ALIGN(64) :
     {
     *(.data)
     }

  .opd ALIGN(64) :
     {
     *(.opd)
     }

  PROVIDE( __CNK_TOC_BASE = (. + 0x8000) );
  .toc :
     {
     *(.toc)
     }
  PROVIDE( __KERNEL_DATA_END = . );

  .bss :
     {
     PROVIDE( __KERNEL_BSS_START = . );
     *(.bss)
     PROVIDE( __KERNEL_BSS_END = . );
     }

  PROVIDE( __KERNEL_END = . );
}

