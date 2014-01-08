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

void svc_showElfFileHeader_T( ElfFileHeader_T *eh )
{

   printf("svc_showElfFileHeader_T: ");
   if ( (eh->e_ident[ EI_MAG0 ] != ELFMAG0) ||
        (eh->e_ident[ EI_MAG1 ] != ELFMAG1) ||
        (eh->e_ident[ EI_MAG2 ] != ELFMAG2) ||
        (eh->e_ident[ EI_MAG3 ] != ELFMAG3)   )
      {
      printf("Not elf.\n");
      return;
      }
   printf("ELF ");

   switch( eh->e_ident[ EI_CLASS ] )
      {
      case ELFCLASSNONE: printf("ELFCLASSNONE "); break;
      case ELFCLASS32  : printf("ELFCLASS32 ");   break;
      case ELFCLASS64  : printf("ELFCLASS64 ");   break;
      case ELFCLASSNUM : printf("ELFCLASSNUM ");  break;
      default:           printf("BadClass ");     break;
      }

   switch( eh->e_ident[ EI_DATA ] )
      {
      case ELFDATANONE: printf("ELFDATANONE "); break;
      case ELFDATA2LSB: printf("ELFDATA2LSB "); break;
      case ELFDATA2MSB: printf("ELFDATA2MSB "); break;
      case ELFDATANUM : printf("ELFDATANUM  "); break;
      default:          printf("BadData ");     break;
      }

   switch( eh->e_ident[ EI_VERSION ] )
      {
      case 1:   printf("EV_CURRENT "); break;
      default:  printf("BadVersion "); break;
      }

   switch( eh->e_ident[ EI_OSABI ] )
      {
      case ELFOSABI_SYSV      : printf("ELFOSABI_SYSV ");       break;
      case ELFOSABI_HPUX      : printf("ELFOSABI_HPUX ");       break;
      case ELFOSABI_NETBSD    : printf("ELFOSABI_NETBSD ");     break;
      case ELFOSABI_LINUX     : printf("ELFOSABI_LINUX ");      break;
      case ELFOSABI_SOLARIS   : printf("ELFOSABI_SOLARIS ");    break;
      case ELFOSABI_AIX       : printf("ELFOSABI_AIX ");        break;
      case ELFOSABI_IRIX      : printf("ELFOSABI_IRIX ");       break;
      case ELFOSABI_FREEBSD   : printf("ELFOSABI_FREEBSD ");    break;
      case ELFOSABI_TRU64     : printf("ELFOSABI_TRU64 ");      break;
      case ELFOSABI_MODESTO   : printf("ELFOSABI_MODESTO ");    break;
      case ELFOSABI_OPENBSD   : printf("ELFOSABI_OPENBSD ");    break;
      case ELFOSABI_ARM       : printf("ELFOSABI_ARM ");        break;
      case ELFOSABI_STANDALONE: printf("ELFOSABI_STANDALONE "); break;
      default                 : printf("BadABI ");              break;
      }

   printf("ABI_VERSION=%d\n", eh->e_ident[ EI_ABIVERSION ] );

   printf("   e_type: "); /* Object file type */
   switch( ntohs(eh->e_type) )
      {
      case ET_NONE  : printf("ET_NONE   "); break;
      case ET_REL   : printf("ET_REL    "); break;
      case ET_EXEC  : printf("ET_EXEC   "); break;
      case ET_DYN   : printf("ET_DYN    "); break;
      case ET_CORE  : printf("ET_CORE   "); break;
      case ET_NUM   : printf("ET_NUM    "); break;
      case ET_LOOS  : printf("ET_LOOS   "); break;
      case ET_HIOS  : printf("ET_HIOS   "); break;
      case ET_LOPROC: printf("ET_LOPROC "); break;
      case ET_HIPROC: printf("ET_HIPROC "); break;
      default:        printf("Bad Type %d.", ntohs(eh->e_type));  break;
      }
   printf("\n");

   printf("   e_machine: "); /* Architecture */
   switch( ntohs(eh->e_machine) )
      {
      case EM_PPC   : printf("EM_PPC "); break;
      case EM_PPC64 : printf("EM_PPC64 "); break;
      default:      printf("Bad Architecture %d.", ntohs(eh->e_machine) ); break;
      }
   printf("\n");

   printf("   e_version: "); /* Object file version */
   switch( ntohl(eh->e_version) )
      {
      case  EV_CURRENT: printf("EV_CURRENT "); break;
      default:          printf("Bad Version %d.", ntohl(eh->e_version) ); break;
      }
   printf("\n");



   printf("   e_entry:     0x%016lx\n",  swap64(eh->e_entry)     ); /* Entry point virtual address */
   printf("   e_phoff:     0x%016lx\n",  swap64(eh->e_phoff)     ); /* Program header table file offset */
   printf("   e_shoff:     0x%016lx\n",  swap64(eh->e_shoff)     ); /* Section header table file offset */
   printf("   e_flags:     0x%08x\n",    swap32(eh->e_flags    ) ); /* Processor-specific flags */
   printf("   e_ehsize:    0x%04x\n",    swap16(eh->e_ehsize   ) ); /* ELF header size in bytes */
   printf("   e_phentsize: 0x%04x\n",    swap16(eh->e_phentsize) ); /* Program header table entry size */
   printf("   e_phnum:     0x%04x\n",    swap16(eh->e_phnum    ) ); /* Program header table entry count */
   printf("   e_shentsize: 0x%04x\n",    swap16(eh->e_shentsize) ); /* Section header table entry size */
   printf("   e_shnum:     0x%04x\n",    swap16(eh->e_shnum    ) ); /* Section header table entry count */
   printf("   e_shstrndx:  0x%04x\n",    swap16(eh->e_shstrndx ) ); /* Section header string table index */
}


void svc_showElfProgramHeader( ElfProgramSegmentHeader_T *ph, int pnum )
{
   printf("Elf64_Phdr[%2d] at 0x%08lx\n", pnum, (unsigned long)ph );

   printf("   p_type:   0x%08x =", swap32(ph->p_type) ); /* Elf32_Word Segment type */
   switch( swap32(ph->p_type) )
      {
      case PT_NULL        :  printf(" PT_NULL");         break; /* Program header table entry unused */
      case PT_LOAD        :  printf(" PT_LOAD");         break; /* Loadable program segment */
      case PT_DYNAMIC     :  printf(" PT_DYNAMIC");      break; /* Dynamic linking information */
      case PT_INTERP      :  printf(" PT_INTERP");       break; /* Program interpreter */
      case PT_NOTE        :  printf(" PT_NOTE");         break; /* Auxiliary information */
      case PT_SHLIB       :  printf(" PT_SHLIB");        break; /* Reserved */
      case PT_PHDR        :  printf(" PT_PHDR");         break; /* Entry for header table itself */
      case PT_TLS         :  printf(" PT_TLS");          break; /* Thread-local storage segment */
      case PT_NUM         :  printf(" PT_NUM");          break; /* Number of defined types */
      case PT_LOOS        :  printf(" PT_LOOS");         break; /* Start of OS-specific */
      case PT_GNU_EH_FRAME:  printf(" PT_GNU_EH_FRAME"); break; /* GCC .eh_frame_hdr segment */
      case PT_HIOS        :  printf(" PT_HIOS");         break; /* End of OS-specific */
      case PT_LOPROC      :  printf(" PT_LOPROC");       break; /* Start of processor-specific */
      case PT_HIPROC      :  printf(" PT_HIPROC");       break; /* End of processor-specific */
      default:               printf(" BadType");         break;
      }
   printf(".\n");

   printf("   p_offset: 0x%016lx\n", swap64(ph->p_offset) ); /* Elf32_Off  Segment file offset */
   printf("   p_vaddr:  0x%016lx\n", swap64(ph->p_vaddr ) ); /* Elf32_Addr Segment virtual address */
   printf("   p_paddr:  0x%016lx\n", swap64(ph->p_paddr ) ); /* Elf32_Addr Segment physical address */
   printf("   p_filesz: 0x%016lx\n", swap64(ph->p_filesz) ); /* Elf32_Word Segment size in file */
   printf("   p_memsz:  0x%016lx\n", swap64(ph->p_memsz ) ); /* Elf32_Word Segment size in memory */

   printf("   p_flags:  0x%08x =", swap32(ph->p_flags ) ); /* Elf32_Word Segment flags */

   if ( swap32(ph->p_flags) & PF_X        ) printf(" PF_X");  /* Segment is executable */
   if ( swap32(ph->p_flags) & PF_W        ) printf(" PF_W");  /* Segment is writable */
   if ( swap32(ph->p_flags) & PF_R        ) printf(" PF_R");  /* Segment is readable */
   if ( swap32(ph->p_flags) & PF_MASKOS   )                   /* OS-specific */
      printf(" PF_MASKOS=0x%02x",  ((swap32(ph->p_flags) & PF_MASKOS   ) >> 20) );
   if ( swap32(ph->p_flags) & PF_MASKPROC )                   /* Processor-specific */
      printf(" PF_MASKPROC=0x%02x", ((swap32(ph->p_flags) & PF_MASKPROC) >> 28) );
   printf(".\n");

   printf("   p_align:  0x%016lx\n", swap64(ph->p_align ) ); /* Elf64_XWord Segment alignment */
}


void svc_showElfSectionHeader_T( ElfSectionHeader_T *sh, int snum, void *mapped_addr )
{
   printf("Elf64_Shdr[%2d] at 0x%08lx\n", snum, (unsigned long)sh );
   printf("   sh_name:      0x%08x = \"%s\"\n", swap32(sh->sh_name), svc_getElfSegmentName( snum, mapped_addr ) ); /* Elf32_Word Section name (string tbl index) */
   printf("   sh_type:      0x%08x = ", swap32(sh->sh_type) ); /* Elf32_Word Section type */
   switch( swap32(sh->sh_type) ) {
      case SHT_NULL         :  printf("SHT_NULL");          break; /* Section header table entry unused */
      case SHT_PROGBITS     :  printf("SHT_PROGBITS");      break; /* Program data */
      case SHT_SYMTAB       :  printf("SHT_SYMTAB");        break; /* Symbol table */
      case SHT_STRTAB       :  printf("SHT_STRTAB");        break; /* String table */
      case SHT_RELA         :  printf("SHT_RELA");          break; /* Relocation entries with addends */
      case SHT_HASH         :  printf("SHT_HASH");          break; /* Symbol hash table */
      case SHT_DYNAMIC      :  printf("SHT_DYNAMIC");       break; /* Dynamic linking information */
      case SHT_NOTE         :  printf("SHT_NOTE");          break; /* Notes */
      case SHT_NOBITS       :  printf("SHT_NOBITS");        break; /* Program space with no data (bss) */
      case SHT_REL          :  printf("SHT_REL");           break; /* Relocation entries, no addends */
      case SHT_SHLIB        :  printf("SHT_SHLIB");         break; /* Reserved */
      case SHT_DYNSYM       :  printf("SHT_DYNSYM");        break; /* Dynamic linker symbol table */
      case SHT_INIT_ARRAY   :  printf("SHT_INIT_ARRAY");    break; /* Array of constructors */
      case SHT_FINI_ARRAY   :  printf("SHT_FINI_ARRAY");    break; /* Array of destructors */
      case SHT_PREINIT_ARRAY:  printf("SHT_PREINIT_ARRAY"); break; /* Array of pre-constructors */
      case SHT_GROUP        :  printf("SHT_GROUP");         break; /* Section group */
      case SHT_SYMTAB_SHNDX :  printf("SHT_SYMTAB_SHNDX");  break; /* Extended section indices */
      case SHT_NUM          :  printf("SHT_NUM");           break; /* Number of defined types.  */
      case SHT_LOOS         :  printf("SHT_LOOS");          break; /* Start OS-specific */
      default: printf("Bad Type");                          break;
   }
   printf(".\n");

   printf("   sh_flags:     0x%016lx =", swap64(sh->sh_flags) ); /* Elf32_Word Section flags */
   if ( swap64(sh->sh_flags) & SHF_WRITE            ) printf(" SHF_WRITE");            /* Writable */
   if ( swap64(sh->sh_flags) & SHF_ALLOC            ) printf(" SHF_ALLOC");            /* Occupies memory during execution */
   if ( swap64(sh->sh_flags) & SHF_EXECINSTR        ) printf(" SHF_EXECINSTR");        /* Executable */
   if ( swap64(sh->sh_flags) & SHF_MERGE            ) printf(" SHF_MERGE");            /* Might be merged */
   if ( swap64(sh->sh_flags) & SHF_STRINGS          ) printf(" SHF_STRINGS");          /* Contains nul-terminated strings */
   if ( swap64(sh->sh_flags) & SHF_INFO_LINK        ) printf(" SHF_INFO_LINK");        /* `sh_info' contains SHT index */
   if ( swap64(sh->sh_flags) & SHF_LINK_ORDER       ) printf(" SHF_LINK_ORDER");       /* Preserve order after combining */
   if ( swap64(sh->sh_flags) & SHF_OS_NONCONFORMING ) printf(" SHF_OS_NONCONFORMING"); /* Non-standard OS specific handling required */
   if ( swap64(sh->sh_flags) & SHF_GROUP            ) printf(" SHF_GROUP");            /* Section is member of a group.  */
   if ( swap64(sh->sh_flags) & SHF_TLS              ) printf(" SHF_TLS");              /* Section hold thread-local data.  */
   if ( swap64(sh->sh_flags) & SHF_MASKOS )
      printf(" SHF_MASKOS=(0x%02lx)",   ((swap64(sh->sh_flags) & SHF_MASKOS)   >> 20) );   /* OS-specific.  */
   if ( swap64(sh->sh_flags) & SHF_MASKPROC )
      printf(" SHF_MASKPROC=(0x%02lx)", ((swap64(sh->sh_flags) & SHF_MASKPROC) >> 28) );   /* Processor-specific.  */
   printf(".\n");

   printf("   sh_addr:      0x%016lx\n", swap64(sh->sh_addr)      ); /* Elf32_Addr Section virtual addr at execution */
   printf("   sh_offset:    0x%016lx\n", swap64(sh->sh_offset)    ); /* Elf32_Off  Section file offset */
   printf("   sh_size:      0x%016lx\n", swap64(sh->sh_size)      ); /* Elf32_Word Section size in bytes */
   printf("   sh_link:      0x%08x\n",   swap32(sh->sh_link     ) ); /* Elf32_Word Link to another section */
   printf("   sh_info:      0x%08x\n",   swap32(sh->sh_info     ) ); /* Elf32_Word Additional section information */
   printf("   sh_addralign: 0x%016lx\n", swap64(sh->sh_addralign) ); /* Elf32_Word Section alignment */
   printf("   sh_entsize:   0x%016lx\n", swap64(sh->sh_entsize)   ); /* Elf32_Word Entry size if section holds table */
}


void svc_showElfSymbolTable( ElfSectionHeader_T *shdr, void *mapped_addr )
{
   ElfSymbolTableEntry_T *sym = (ElfSymbolTableEntry_T *)(mapped_addr + swap64(shdr->sh_offset));
   Elf64_Xword total_syms = swap64(shdr->sh_size) / sizeof(ElfSymbolTableEntry_T);
   int i;

   printf("Total_Syms = %ld.\n", total_syms );

   for ( i = 0 ; i < total_syms ; i++, sym++ ) {
     printf(" st_name  = %ld",         (unsigned long )swap32(sym->st_name ) ); /* Elf32_Word     Symbol name (string tbl index) */
     printf(" st_value = 0x%016lx", swap64(sym->st_value)                            ); /* Elf32_Addr     Symbol value */
     printf(" st_size  = 0x%016lx", swap64(sym->st_size)                             ); /* Elf32_Word     Symbol size */
     printf(" st_info  = 0x%02x",                        sym->st_info   ); /* unsigned char  Symbol type and binding */
     printf(" st_other = 0x%02x",                        sym->st_other  ); /* unsigned char  Symbol visibility */
     printf(" st_shndx = 0x%04x\n",  (unsigned short)swap16(sym->st_shndx) ); /* Elf32_Section  Section index 16bits */
   }
}

void svc_showElfStringTable( ElfSectionHeader_T *shdr, void *mapped_addr )
{
   char *str_start = (char *)((Elf64_Addr)mapped_addr + swap64(shdr->sh_offset));
   char *str_end   = (char *)((Elf64_Addr)str_start   + swap64(shdr->sh_size));
   int i;
   char *str = str_start;

   printf("String Table:\n");
   for( i = 0 ; str < str_end ; i++ ) {
     printf("[%4d:%4ld] : %s\n", i, (str-str_start), str );
     str += (strlen(str) + 1);
   }

}


