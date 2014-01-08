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


#include "svc_host.h"

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#define SVC_MAX_SINGLE_FILEIO (16 * 1024)


int svc_getElfSymbolByName(Svc_Elf_Info_T *elfInfo,
                         char *symbol,
                         unsigned long *vaddr,
                         unsigned long *size,
                         unsigned long *filepos )
{

  // Elf Segment Headers
  ElfSectionHeader_T* sym_shdr = (ElfSectionHeader_T*)elfInfo->symbolTableAddress;
  ElfSectionHeader_T* str_shdr = (ElfSectionHeader_T*)elfInfo->stringTableAddress;
  ElfFileHeader_T*    ehdr     = (ElfFileHeader_T*)elfInfo->mappedAddress;

  // symbol table info
  ElfSymbolTableEntry_T* sym        = (ElfSymbolTableEntry_T *)(elfInfo->mappedAddress + swap64(sym_shdr->sh_offset));
  Elf64_Xword            total_syms = (swap64(sym_shdr->sh_size) / sizeof(ElfSymbolTableEntry_T));

  // string table info
  char *str_start = (char *)((Elf64_Addr)elfInfo->mappedAddress + swap64(str_shdr->sh_offset));
  char *str_end   = (char *)((Elf64_Addr)str_start + swap64(str_shdr->sh_size));

  int i, found;
  char *str = str_start;
  unsigned long symbol_offset = 0;

  *vaddr   = 0;
  *size    = 0;
  *filepos = 0;

  // find the index of the symbol name:

  for( i = 0 ; str < str_end ; i++ ) {
    if ( ! strcmp( str, symbol ) ) {
      symbol_offset = (unsigned long)(str - str_start);
      break;
    }
    str += (strlen(str) + 1);
  }

  if ( ! symbol_offset ) {
    if ( debug ) {
      printf("(W) svc_getElfSymbolByName(%s): Symbol Not in String Table.\n", symbol );
    }
    return -1;
  }

   // search symbols for matching symbol name offset:
   for ( i = 0, found = 0 ; i < total_syms ; i++, sym++ )  {
     if ( swap32(sym->st_name) != symbol_offset ) {
       continue;
     }

     *vaddr = swap64(sym->st_value);
     *size  = swap64(sym->st_size );
     found  = 1;
     break;
   }

   if ( ! found ) {
     if ( debug ) {
       printf("(W) svc_getElfSymbolByName(%s): Symbol Not in Symbol Offset Table.\n", symbol );
     }
     return -1;
   }
   else if ( debug ) {
     printf("(D) svc_getElfSymbolByName(%s): vaddr=0x%08lx, size=0x%08lx.\n", symbol, *vaddr, *size );
   }

   // given a vaddr, find enclosing segment, and calculate the offset to vaddr
   for ( i = 0, found = 0; i < swap16(ehdr->e_shnum) ; i++ )  {

     ElfSectionHeader_T *shdr = (ElfSectionHeader_T *)(elfInfo->mappedAddress + swap64(ehdr->e_shoff) + (swap16(ehdr->e_shentsize) * i));
     unsigned long s_vaddr;
     unsigned long s_offset;
     unsigned long s_size;

     if ( ! (swap64(shdr->sh_flags) & (SHF_WRITE | SHF_ALLOC | SHF_EXECINSTR)) ) {
         continue;
     }

      // only PROGBITS segments are actually loaded 
      if ( swap32(shdr->sh_type) != SHT_PROGBITS ) {
         continue;
      }

      s_vaddr   = swap64( shdr->sh_addr   );
      s_offset  = swap64( shdr->sh_offset );
      s_size    = swap64( shdr->sh_size   );

      if ( *vaddr < s_vaddr ) {
         continue;
      }

      if ( *vaddr > (s_vaddr + s_size) ) {
         continue;
      }

      if ( (*vaddr + *size) > (s_vaddr + s_size) ) {
         continue;
      }

      found = 1;

      if ( debug ) {
         svc_showElfSectionHeader_T( shdr, i, elfInfo->mappedAddress );
      }

      *filepos = ((*vaddr - s_vaddr) + s_offset);

      if ( debug || verbose )  {
	printf("(I) Elf file offset of symbol \"%s\" at 0x%08lx.\n", symbol, *filepos );
	//printf("%s = 0x%04x\n", symbol, *((unsigned short *)(_svc_host_state.bs_elf_addr + *filepos)) );
      }
      break;
   }
   
   if ( ! found ) {
     if ( debug ) {
       printf("(D) svc_getElfSymbolByName(%s): Symbol Segment Not Found.\n", symbol );
     }
     return -1;
   }

   return 0;
}



/**
 * @brief Given an section header, return the matching program segment header (or NULL if one doesn't exist)
 */

ElfProgramSegmentHeader_T *svc_getElfPhdr( ElfSectionHeader_T *shdr, void *mapped_addr ) {

  int i;
  ElfFileHeader_T *ehdr = (ElfFileHeader_T *)mapped_addr;
  ElfProgramSegmentHeader_T *phdr;
  int tot_phdrs = swap16(ehdr->e_phnum);

  if ( tot_phdrs <= 0 )
    return( NULL );

  for ( i = 0; i < tot_phdrs ; i++ ) {

    phdr = (ElfProgramSegmentHeader_T *)(mapped_addr + swap64(ehdr->e_phoff) + (swap16(ehdr->e_phentsize) * i));

      // eliminate non-loadable phdrs
      // if ( swap32(phdr->p_type) != PT_LOAD )
      //    continue;

      // match RWX flags:
      if ( (swap64(shdr->sh_flags) & SHF_WRITE)     && !(swap32(phdr->p_flags) & PF_W) )
	continue;
      if ( (swap64(shdr->sh_flags) & SHF_ALLOC)     && !(swap32(phdr->p_flags) & PF_R) )
	continue;
      if ( (swap64(shdr->sh_flags) & SHF_EXECINSTR) && !(swap32(phdr->p_flags) & PF_X) )
	continue;

      // match virtual address:
      if ( swap64(shdr->sh_addr) != swap64(phdr->p_vaddr) )
	continue;

      // match segment size:
      if ( swap64(shdr->sh_size) != swap64(phdr->p_memsz) )
	continue;

      // This is a match:
      return( phdr );
    }

  // no match
  return( NULL );
}



/**
 * @brief Physical Address equal to Virtual Address unless overridden by a Phdr.
 */

unsigned long svc_getElfSegmentPaddr( ElfSectionHeader_T *shdr, void *mapped_addr ) {

  ElfProgramSegmentHeader_T *phdr = svc_getElfPhdr( shdr, mapped_addr );

  if ( phdr )
    return( swap64(phdr->p_paddr) );
  else
    return( swap64(shdr->sh_addr) );
}



/**
 * @brief Return type of segment as indicated by it's PHdr
 */

unsigned long svc_getElfPhdrType( ElfSectionHeader_T *shdr, void *mapped_addr ) {
  
  ElfProgramSegmentHeader_T *phdr = svc_getElfPhdr( shdr, mapped_addr );

  if ( phdr ) {
    return( swap32(phdr->p_type) );
  }
  else {
    return( PT_NULL );
  }
}
 

char* svc_getElfSegmentName( int snum, void* mapped_addr ) {

  ElfFileHeader_T* ehdr = (ElfFileHeader_T*)mapped_addr;
  ElfSectionHeader_T* shdr;
  ElfSectionHeader_T* name_shdr;
  int tot_segs  = swap16(ehdr->e_shnum);
  int name_snum = swap16(ehdr->e_shstrndx);
  int snum_name_offset;
  Elf64_Addr name_table;

  if ( snum > tot_segs ) {
    return("OutofRange");
  }

  if ( (name_snum <= 0) || (name_snum >= tot_segs) ) {
    return("NoNames");
  }

  shdr      = (ElfSectionHeader_T* )(mapped_addr + swap64(ehdr->e_shoff) + (swap16(ehdr->e_shentsize) * snum ));
  name_shdr = (ElfSectionHeader_T *)(mapped_addr + swap64(ehdr->e_shoff) + (swap16(ehdr->e_shentsize) * name_snum));

  snum_name_offset = swap32(shdr->sh_name);

  name_table = (Elf64_Addr)mapped_addr + swap64(name_shdr->sh_offset);

  return( (char *)(name_table + snum_name_offset) );
}


int svc_unloadElfFile( Svc_Elf_Info_T* elfInfo ) {

  int rc = 0;

  if ( ! elfInfo->imageIsLoaded ) {
    fprintf(stderr, "(E) svc_unloadElfFile: No ELF file loaded.\n");
    return -1;
  }

  if ( elfInfo->patchedFileName[0] ==0 ) {
    fprintf(stderr, "(W) Output file not specified  [%s:%d]\n", __func__, __LINE__);
    rc = -1;
  }
  else if ( ! elfInfo->isWriteable ) {
    fprintf(stderr, "(W) Output file is not writeable  [%s;%d]\n", __func__, __LINE__);
    rc = -1;
  }
  else {

    char* p      = elfInfo->mappedAddress;
    size_t bytes = elfInfo->mappedSize;

    if ( (elfInfo->fileDescriptor = creat( elfInfo->patchedFileName, (S_IRWXU | S_IRWXG) )) < 0 ) {
      fprintf(stderr,"(E) svc_unloadElfFile: open of %s fails. Errno=%d\n", elfInfo->patchedFileName, errno );
      return( _svc_error_elf_create_fail );
    }

    for ( ; bytes > 0 ; ) {
      size_t left = MIN( SVC_MAX_SINGLE_FILEIO, bytes );
      size_t wrc = write( elfInfo->fileDescriptor, p, left );
      if ( wrc == -1 )
	break;

      bytes -= wrc;
      p     += wrc;
    }

    if ( bytes > 0 )
      rc = _svc_error_elf_create_fail;

    if ( verbose )
      printf("(I) svc_unloadElfFile: RC=%d, Wrote %ld bytes to %s.\n", rc, elfInfo->mappedSize, elfInfo->patchedFileName );
  }


  free( elfInfo->mappedAddress );
  close( elfInfo->fileDescriptor );

  elfInfo->imageIsLoaded  = 0;
  elfInfo->fileDescriptor = -1;
  elfInfo->mappedAddress  = NULL;
  elfInfo->mappedSize     = 0;

  return rc;
}


int svc_makeFirmwareImage( Svc_Firmware_Info_T* firmware ) {

  Svc_Elf_Info_T*  elfInfo = &(firmware->elfInfo);
  ElfFileHeader_T*  ehdr     = (ElfFileHeader_T* )(elfInfo->mappedAddress);
  int             i;

  if ( !elfInfo->imageIsLoaded ) {
    fprintf(stderr,"(E) Internal Error : Patched elf file not open. [%s:%s:%d]\n", __FILE__, __func__, __LINE__);
    return -1;
  }

  if ( debug ) {
    printf("(D) Creating BootLoader SRAM Image.\n");
  }

  // Clear out the existing BeDRAM image buffer:

  memset( &(firmware->BeDRAM_Image[0]), 0, sizeof(firmware->BeDRAM_Image) );

  // Shdrs: go section by section, loading the bootstrapper into our BeDRAM image:

  unsigned totalBytes = 0;

  for ( i = 0; i < swap16(ehdr->e_shnum) ; i++ ) {

    unsigned long s_vaddr, s_size, s_offset, phys_addr, sp_type;
    ElfSectionHeader_T* shdr = (ElfSectionHeader_T* )(elfInfo->mappedAddress + swap64(ehdr->e_shoff) + (swap16(ehdr->e_shentsize) * i));
    uint32_t sram_ofs;

    if ( debug > 1 ) {
      svc_showElfSectionHeader_T( shdr, i, elfInfo->mappedAddress );
    }

    // Skip over all but the really important sections
    if ( ! (swap64(shdr->sh_flags) & (SHF_WRITE | SHF_ALLOC | SHF_EXECINSTR)) ) {
      continue;
    }

    // only PROGBITS segments are actually placed in SRAM.

    switch( swap32(shdr->sh_type) ) {
    
    case SHT_PROGBITS: // code or data
      {
	s_vaddr   = swap64( shdr->sh_addr   );
	s_offset  = swap64( shdr->sh_offset );
	s_size    = swap64( shdr->sh_size   );
	phys_addr = svc_getElfSegmentPaddr( shdr, elfInfo->mappedAddress );
	sp_type   = svc_getElfPhdrType( shdr, elfInfo->mappedAddress );
	
	if ( verbose )
	  printf("(I) %-20s - %8ld bytes at V=0x%08lx P=0x%08lx.\n", svc_getElfSegmentName(i, elfInfo->mappedAddress), s_size, s_vaddr, phys_addr );
	
	if ( sp_type != PT_LOAD ) {
	  if ( verbose ) printf("(D) Ignoring non-PT_LOAD segment. Type = %ld\n", sp_type );
	  continue;
	}

	if ( ! s_size ) {
	  if (debug) printf("(D) Ignoring zero size segment.\n");
	  continue;
	}

	  sram_ofs = (s_vaddr - (uint32_t)BGQ_VA_BeDRAM);
	  if ( (sram_ofs + s_size) > BGQ_PS_BeDRAM ) {
	    fprintf(stderr,"(E) svc_makeFirmwareImage: segment %s VA=0x%08lx out of range.\n",  svc_getElfSegmentName(i, elfInfo->mappedAddress), s_vaddr );
	    return -2;
	  }

	  memcpy( &(firmware->BeDRAM_Image[sram_ofs]), (elfInfo->mappedAddress + s_offset), s_size );

	  totalBytes += s_size;
	  break;
      }
	
    case SHT_NOBITS:   // bss (or maybe .copyright): Not used by bootstrapper.
      {
	  sp_type   = svc_getElfPhdrType( shdr, elfInfo->mappedAddress );
	  if ( sp_type == PT_LOAD )
	    printf("(W) Bootstrapper should not contain a BSS Segment. Ignoring.\n");
	  break;
      }
	
    default:           // ignore/discard other types
      break;
    }
  }

  if ( verbose ) printf("(I) %d bytes total (%d%% full)\n", totalBytes, totalBytes*100/(256*1024));

  return 0;
}


int svc_loadElfFile( Svc_Elf_Info_T* elfInfo,
                       char* inputFileName,
                       char* outputFileName  // ok to be NULL
		       )
{
  struct stat sbuf;
  int i;
  ElfFileHeader_T* ehdr;
  ElfProgramSegmentHeader_T* phdr;
  ElfSectionHeader_T* shdr;
  unsigned long s_vaddr, s_size, s_offset, s_flags, phys_addr, sp_type;


  unsigned memory_bytes    = 0;
 

  if (debug) {
    printf( "(D) %s elfInfo=%p in-file=%s out-file=%s\n", __func__, elfInfo, inputFileName, outputFileName);
  }

  // Start with a clean slate:
  memset( elfInfo, 0, sizeof(Svc_Elf_Info_T) );

  // check/set filenames:
  if ( !inputFileName || !*inputFileName ) {
    fprintf(stderr,"(E) svc_loadElfFile: No input file given.\n");
    return( _svc_error_elf_not_found );
  }

  strncpy( elfInfo->fileName, inputFileName, SVC_MAX_FILENAME_LEN );

  if ( outputFileName && *outputFileName ) {
    elfInfo->isWriteable = 1;
    strncpy( elfInfo->patchedFileName, outputFileName, SVC_MAX_FILENAME_LEN );
  }

  if ( stat( inputFileName, &sbuf ) ) {
    fprintf(stderr,"(E) svc_loadElfFile: could not stat %s.\n", inputFileName );
    return( _svc_error_elf_not_found );
  }

  if ( (elfInfo->mappedSize = sbuf.st_size) < sizeof(ElfFileHeader_T) ) {
    fprintf(stderr,"(E) svc_loadElfFile: %s is invalid elf file.\n", inputFileName );
    return( _svc_error_invalid_elf );
  }
 
  if ( (elfInfo->fileDescriptor = open( inputFileName, O_RDONLY, 0 )) <= 1 ) {
    fprintf(stderr,"(E) svc_loadElfFile: open of %s fails. Errno=%d\n", inputFileName, errno );
    return( _svc_error_elf_not_found );
  }


  elfInfo->mappedAddress = malloc( elfInfo->mappedSize );

  if ( elfInfo->mappedAddress == NULL ) {
    fprintf(stderr, "(E) Couldnt malloc buffer of size %ld\n", elfInfo->mappedSize);
    close( elfInfo->fileDescriptor );
    elfInfo->fileDescriptor          = -1;
    elfInfo->mappedSize = 0;
    return( _svc_error_mmap_failed  );
  }


  // Read the contents into memory:
  char* p      = elfInfo->mappedAddress;
  size_t bytes = elfInfo->mappedSize;

  for ( ; bytes > 0 ; ) {
    size_t left = MIN( SVC_MAX_SINGLE_FILEIO, bytes );
    ssize_t rrc = read( elfInfo->fileDescriptor, p, left );
    if ( rrc == -1 )
      break;

    bytes -= rrc;
    p     += rrc;
  }

  if ( bytes > 0 ) {
    fprintf(stderr, "(E) ELF file not completely read (%ld of %ld bytes remaining)\n", bytes, elfInfo->mappedSize );
    free( elfInfo->mappedAddress );
    close( elfInfo->fileDescriptor );
    elfInfo->fileDescriptor          = -1;
    elfInfo->mappedSize = 0;
    elfInfo->mappedAddress = NULL;
    return( _svc_error_mmap_failed  );
  }

  // We are now done with the file:
  close( elfInfo->fileDescriptor );
  elfInfo->fileDescriptor = -1;
  
  if ( debug ) {
    printf("(D) %s : %ld bytes mapped at 0x%08lx.\n", inputFileName, elfInfo->mappedSize, (unsigned long)elfInfo->mappedAddress );
  }

  // The beginning of the buffer now should contain an elf file header, including
  // the magic numbers and such.  Let's confirm that:

  ehdr = (ElfFileHeader_T* )(elfInfo->mappedAddress);

  if ( (ehdr->e_ident[ EI_MAG0  ] != ELFMAG0      ) ||
       (ehdr->e_ident[ EI_MAG1  ] != ELFMAG1      ) ||
       (ehdr->e_ident[ EI_MAG2  ] != ELFMAG2      ) ||
       (ehdr->e_ident[ EI_MAG3  ] != ELFMAG3      ) ||
       (ehdr->e_ident[ EI_CLASS ] != ELFCLASS64   ) ||
       (ehdr->e_ident[ EI_DATA  ] != ELFDATA2MSB  ) ||
       (ehdr->e_ident[ EI_OSABI ] != ELFOSABI_SYSV) ||
       (ntohs(ehdr->e_machine)    != EM_PPC64     )    )
    {
      fprintf(stderr,"(E) \"%s\" is not a valid ELF file for BG/Q.\n", inputFileName );

      if (debug) {

	if ( (ehdr->e_ident[ EI_MAG0  ] != ELFMAG0 ) ||
	     (ehdr->e_ident[ EI_MAG1  ] != ELFMAG1 ) ||
	     (ehdr->e_ident[ EI_MAG2  ] != ELFMAG2 ) ||
	     (ehdr->e_ident[ EI_MAG3  ] != ELFMAG3 )
	     )
	  {
	    printf("(D) ELF magic number mismatch.  File is probably not an ELF file.\n");
	  }

	if (ehdr->e_ident[ EI_CLASS ] != ELFCLASS64 ) {
	  printf("(D) ELF class mismatch. (EI_CLASS=%d versus %d (expected)\nPerhaps this is not a 64-bit binary.\n", ehdr->e_ident[EI_CLASS], ELFCLASS64);
	}

	if ( ehdr->e_ident[ EI_OSABI ] != ELFOSABI_SYSV ) {
	  printf("(D) ELF ABI mismatch.\n");
	}

	if (ntohs(ehdr->e_machine) != EM_PPC64 ) {
	  printf("(D) ELF machine mismatch (probably a PPC32 instead of PPC64?)\n");
	}
      }
	
      return _svc_error_invalid_elf;
    }

  if ( debug ) {
    svc_showElfFileHeader_T( ehdr );
  }

  elfInfo->entryPointAddress = (void* )(swap64( ehdr->e_entry ));

  // If we are in debug mode, dump the program segment headers:
  if ( debug ) {
    for ( i = 0; i < swap16(ehdr->e_phnum) ; i++ ) {
      phdr = (ElfProgramSegmentHeader_T* )(elfInfo->mappedAddress + swap64(ehdr->e_phoff) + (swap16(ehdr->e_phentsize) * i));
      svc_showElfProgramHeader( phdr, i );
    }
  }

  // Shdrs: Quick first pass to locate the symbol and string tables:

  for ( i = 0; i < swap16(ehdr->e_shnum) ; i++ ) {

    shdr = (ElfSectionHeader_T*)(elfInfo->mappedAddress + swap64(ehdr->e_shoff) + (swap16(ehdr->e_shentsize) * i));

    if (debug) {
      printf( "(D) processing section type %d (%d of %d)\n", swap32(shdr->sh_type), i, swap16(ehdr->e_shnum) );
    }
  
    if ( swap32(shdr->sh_type) == SHT_SYMTAB ) {
      char* seg_name = svc_getElfSegmentName( i, elfInfo->mappedAddress );
      if ( ! strcmp( seg_name, ".symtab" ) ) {
	elfInfo->symbolTableAddress = shdr;
	if (debug) {
	  svc_showElfSymbolTable(shdr, elfInfo->mappedAddress);
	}
      }
    }

    if ( swap32(shdr->sh_type) == SHT_STRTAB ) {
      char* seg_name = svc_getElfSegmentName( i, elfInfo->mappedAddress );
      if ( ! strcmp( seg_name, ".strtab" ) ) {
	elfInfo->stringTableAddress = shdr;

	if (debug) {
	  svc_showElfStringTable( shdr, elfInfo->mappedAddress);
	}
      }
    }
  }


  // Section Headers:
  //   We immediately load the bootstrapper into our sram image
  //   Later, this image will be personalized as it is loaded into each node.

  for ( i = 0; i < swap16(ehdr->e_shnum) ; i++ ) {

    shdr = (ElfSectionHeader_T*)(elfInfo->mappedAddress + swap64(ehdr->e_shoff) + (swap16(ehdr->e_shentsize) * i));

    if ( debug > 1 ) {
      svc_showElfSectionHeader_T( shdr, i, elfInfo->mappedAddress );
    }

    // Only keep "allocate" segments (discard .debug, .comment, etc.)
    if ( ! (swap64(shdr->sh_flags) & SHF_ALLOC) )
      continue;

    // NOTE: only PROGBITS segments are actually placed in SRAM:

    switch( swap32(shdr->sh_type) ) {

    case SHT_PROGBITS: // code or data
    case SHT_NOBITS:   // BSS
      {
	s_vaddr   = swap64( shdr->sh_addr   );
	s_offset  = swap64( shdr->sh_offset );
	s_size    = swap64( shdr->sh_size   );
	s_flags   = swap64( shdr->sh_flags  );
	phys_addr = svc_getElfSegmentPaddr( shdr, elfInfo->mappedAddress );
	sp_type   = svc_getElfPhdrType( shdr, elfInfo->mappedAddress );
	char* s_name = svc_getElfSegmentName(i, elfInfo->mappedAddress);

	if ( verbose ) {
	  printf("(I) %-20s %c%c%c%c %8ld bytes at V=0x%08lx P=0x%08lx.\n",
		 (s_name ? s_name : "NULL"),
		 (s_flags & SHF_ALLOC     ? 'a' : '-'),
		 (s_flags & SHF_WRITE     ? 'w' : '-'),
		 (s_flags & SHF_EXECINSTR ? 'x' : '-'),
		 (s_flags & SHF_TLS       ? 'T' : '-'),
		 s_size, s_vaddr, phys_addr );
	}

	// PT_NULL means there was no PHDR for this section
	if ( (sp_type != PT_LOAD) && (sp_type != PT_TLS) && (sp_type != PT_NULL) ) {
	  if ( debug ) {
	    printf("(D) Ignoring non-PT_LOAD segment %s. Type = %ld\n", svc_getElfSegmentName(i, elfInfo->mappedAddress), sp_type );
	  }
	  continue;
	}

	if ( ! s_size ) {
	  if ( debug ) {
	    printf("(D) Ignoring zero size segment.\n");
	  }
	  continue;
	}

	//memcpy( sram_addr2img( s_vaddr ), (elfInfo->mappedAddress + s_offset), s_size );
	memory_bytes += s_size;
	break;
      }
   
    default:           // ignore/discard other types
      break;
    }
  }

  if ( verbose ) {
    printf("(I) Elf Entry Point at 0x%p.\n", elfInfo->entryPointAddress );
  }

  printf("(I) Elf Total BeDRAM Image Bytes = %u (%d%% full).\n", memory_bytes, memory_bytes*100/(1024*256) );

  elfInfo->imageIsLoaded = 1;

  return 0;
}


int svc_loadFirmware( Svc_Firmware_Info_T* fw, char* inputFileName, char* outputFileName ) {

  int rc;

  uint64_t
    fw_pers_vaddr   = 0,
    fw_pers_size    = 0,
    fw_pers_filepos = 0,
    fw_crc_vaddr    = 0,
    fw_crc_size     = 0,
    fw_crc_filepos  = 0;

  if ( debug ) {
    printf("(D) Load Elf Bootloader: %s\n", inputFileName );
  }

  /*
   * Load the ELF file:
   */

  if ( (rc = svc_loadElfFile( &(fw->elfInfo), inputFileName, outputFileName ) ) ) {
    return rc;
  }


  /*
   * Locate the personality via the symbol table and ensure
   * that it is the appropriate size, version, etc.
   */

  rc = svc_getElfSymbolByName( &(fw->elfInfo),
			     SVC_FIRMWARE_PERSONALITY_SYMBOL,
			     &fw_pers_vaddr,
			     &fw_pers_size,
			     &fw_pers_filepos );

  if ( rc == 0 ) {

    Personality_t* pPers;
    fw->personalityAddress      = fw_pers_vaddr;
    fw->personalitySize         = fw_pers_size;
    fw->personalityFilePosition = fw_pers_filepos;
    fw->canBePatched = 1;

    if ( fw_pers_size != sizeof(Personality_t) ) {
      fprintf(stderr,"(E) Error: BootLoader SRAM Personality size %ld != %ld.\n", fw_pers_size, (unsigned long)sizeof(Personality_t) );
      exit(1);
    }

    pPers = (Personality_t* )((uint64_t)fw->elfInfo.mappedAddress + (uint64_t)fw_pers_filepos);

    if ( pPers->Version != PERSONALITY_VERSION ) {
      fprintf(stderr,"(E) Error: BootLoader SRAM Personality Version Mismatch. Expected %d, Got %d.\n", PERSONALITY_VERSION, pPers->Version );
      exit(1);
    }

    if ( verbose ) {
      printf("(I) BootLoader SRAM Personality: %ld bytes at 0x%08lx.\n", fw_pers_size, fw_pers_vaddr );
    }
  }
  else  {
    // Not Fatal for now.
    fw->canBePatched = 0;
    printf("(W) BootLoader SRAM Personality: Not present.\n");
  }

  rc = svc_getElfSymbolByName( &(fw->elfInfo),
			     SVC_FIRMWARE_CRC_VAL_SYMBOL,
			     &fw_crc_vaddr,
			     &fw_crc_size,
			     &fw_crc_filepos );

  if ( rc == 0 ) {
    fw->canBeCRCd  = 1;
    fw->crcAddress  = fw_crc_vaddr;
    fw->crcSize     = fw_crc_size;
    fw->crcFilePos  = fw_crc_filepos;
  }
  else {
    fw->canBeCRCd = 0;
    printf("(W) BootLoader Missing CRC Value Symbol. CRC disabled.\n");
    rc = 0; //! @todo : fix this ... CRC stuff disabled for now
  }

  if ( fw->canBeCRCd ) {

    unsigned long fw_crc_start;
    unsigned long fw_crc_stop;
    unsigned long fw_crc_size;
    unsigned long fw_crc_filepos;

    int rc1 = svc_getElfSymbolByName( &(fw->elfInfo),
				    SVC_FIRMWARE_CRC_START_SYMBOL,
				    &fw_crc_start,
				    &fw_crc_size,
				    &fw_crc_filepos );

    int rc2 = svc_getElfSymbolByName( &(fw->elfInfo),
				    SVC_FIRMWARE_CRC_STOP_SYMBOL,
				    &fw_crc_stop,
				    &fw_crc_size,
				    &fw_crc_filepos );
    if ( rc1 || rc2 ) {
      printf("(W) BootLoader Missing start/stop symbols for CRC calculation. CRC disabled.\n");
      fw->canBeCRCd  = 0;
      fw->crcAddress  = 0;
      fw->crcSize     = 0;
      fw->crcFilePos  = 0;
    }
    else {
      fw->crcStartAddress = fw_crc_start;
      fw->crcStopAddress  = fw_crc_stop;

      if ( verbose ) {
	printf("(I) BootLoader: CRC Start=0x%08lx, Stop=0x%08lx.\n", fw_crc_start, fw_crc_stop );
      }
    }
  }

  return rc;
}
