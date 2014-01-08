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


#ifndef _SVC_HOST_INTERNAL_H // Prevent multiple inclusion.
#define _SVC_HOST_INTERNAL_H

#include <firmware/tools/include/svc_host.h>

#define __BGP_HOST_COMPILED__

//
// NB: Things called "templates" are in Host Byte Order and may
//     need byte-swapping before use.
//

#if defined( _AIX )
#include <inttypes.h>
#include <memory.h>   // memmove (on Linux memmove is in string.h)
#else
#include <stdint.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>     // memset, memmove
#include <strings.h>    // strncasecmp, strcasecmp
#include <netinet/in.h> // byte swapping
#include <sys/mman.h>
#include <errno.h>


#include <elf.h>


#include <firmware/include/personality.h>

#ifndef offsetof
// Obtain the offset of an element of a data structure
#define offsetof(TYPE, MEMBER) ((unsigned long) &((TYPE *)0)->MEMBER)
#endif

#define  SVC_DEFAULT_KERNEL_NAME       "cnk"

#define SVC_FIRMWARE_PERSONALITY_SYMBOL "FW_Personality"
#define SVC_FIRMWARE_ENV_VARS_SYMBOL    "fwext_EnvVars"
#define SVC_FIRMWARE_CRC_VAL_SYMBOL     "_sram_CRC"
#define SVC_FIRMWARE_CRC_START_SYMBOL   "__SRAM_CRC_START"
#define SVC_FIRMWARE_CRC_STOP_SYMBOL    "__SRAM_CRC_END"

#define SVC_STANDALONE_APP_LOAD_SYMBOL "_bgp_StandAloneLoadInfo"

#define SVC_MAX_FILENAME_LEN (4096)

#define BGQ_PS_BeDRAM (256 * 1024)
#define BGQ_VA_BeDRAM 0x000003fffffc0000ull

#define SVC_MAX_ENV_VARS    256
#define SVC_MAX_ENV_VAR_LEN 128

// Error codes

#define SVC_MAX_ERROR_MESSAGE_LEN (512) 

extern int verbose;
extern int debug;


typedef struct T_TraceControl {
  char    *name;
  uint32_t value;
} TraceControl_T;

typedef struct T_DDR_Chip {
  char    *spec;
  uint8_t type;
}  DDR_Chip_T;

typedef struct T_RAS_Verbosity {
  char* spec;
  uint32_t value;
} RAS_Verbosity_T;

typedef struct T_Svc_Control {
  char*     name;
  uint64_t  bits;
  char*     help;
} Svc_Control_T;

typedef struct T_Svc_FieldEditor {
  char*     id;
  unsigned  offset;
  unsigned  width;
  char*     help;
  int       lower_bound;
  int       upper_bound;
} Svc_FieldEditor_T;

typedef struct T_Svc_IPAddrEditor {
  char*    id;
  unsigned offset;
  char*    help;
} Svc_IPAddrEditor_T;

typedef int (*Svc_HostArgHandler_T)(char* source, char* target, int maximum_length, char* error_message);

typedef struct {
  Svc_HostArgHandler_T  handler;
  char*                 id;
  char*                 target;
  int                   maximum_length;
  char*                 error_message;
  char*                 help_message;
} Svc_HostArg_T;

typedef int (*Svc_SimpleEditorHandler_T)(char* source, Personality_t* personality);
typedef struct {
  Svc_SimpleEditorHandler_T  handler;
  char*                      id;
  char*                      help_message;
  char*                      help_prefix;
  char*                      help_suffix;
} Svc_SimpleEditor_T;
  

typedef struct T_Svc_Elf_Info {
  int    fileDescriptor;           // filehandle for elf executable
  void   *mappedAddress; // address at which elf file is memory mapped
  size_t mappedSize;  // bytes of mapped memory.
  
  void *entryPointAddress;   // program entry point virtual address
  
  void *symbolTableAddress;       // memory addr of Symbol Table Shdr
  void *stringTableAddress;       // memory addr of String Table Shdr

  int  imageIsLoaded;
  int  isWriteable;
  char fileName[    SVC_MAX_FILENAME_LEN ];
  char patchedFileName[ SVC_MAX_FILENAME_LEN ];
} Svc_Elf_Info_T;


typedef struct T_Svc_Firmware_Info {

  Svc_Elf_Info_T elfInfo;
  
  int    canBePatched;
  size_t personalityAddress; 
  size_t personalitySize; 
  size_t personalityFilePosition;

  int    canBeCRCd;
  size_t crcAddress; 
  size_t crcSize; 
  size_t crcFilePos; 
  size_t crcStartAddress; 
  size_t crcStopAddress;


  uint8_t BeDRAM_Image[ BGQ_PS_BeDRAM ];

} Svc_Firmware_Info_T;

typedef struct T_Svc_EnvVars {
  char     name[SVC_MAX_ENV_VARS][SVC_MAX_ENV_VAR_LEN]; 
  char     value[SVC_MAX_ENV_VARS][SVC_MAX_ENV_VAR_LEN];
  size_t   environmentVariablesAddress;     // The ELF address of the env variables table
  size_t   environmentVariablesSize;        // The actual size of the env variables table
  size_t   environmentVariablesFilePosition; // The position within the ELF file of the table

} Svc_EnvVars_T;


char* svc_host_error_message();
int svc_Usage( char *msg, char *msg2 );
void svc_Warning( _BGP_Svc_Host_ErrCodes code, char *msg );
void svc_ShowPersonality( Personality_t *p );

int  svc_find_ctrl( char *str, uint32_t *sets, uint32_t *clrs );
int  svc_find_ddr_chips( char *str, uint16_t *chip_type, char *msg );
int  svc_getString( char *src, char *dst, int maxlen, char *msg );
int  svc_getInt8( char *str, uint8_t *var, uint8_t vmin, uint8_t vmax, char *fmsg, char *rmsg );
int  svc_getInt16( char *str, uint16_t *var, uint16_t vmin, uint16_t vmax, char *fmsg, char *rmsg );
int  svc_getInt32( char *str, uint32_t *var, uint32_t vmin, uint32_t vmax, char *fmsg, char *rmsg );
int  svc_getMAC( char *str, uint8_t *var, char *msg );
void svc_help( void );
int  svc_doFirmware( Svc_Firmware_Info_T  *bl_info, char* bl_in_fn, char* bl_out_fn );



// svc_elf.c:

typedef Elf64_Ehdr ElfFileHeader_T;
typedef Elf64_Shdr ElfSectionHeader_T;
typedef Elf64_Sym  ElfSymbolTableEntry_T;
typedef Elf64_Phdr ElfProgramSegmentHeader_T;

#if __APPLE__
#define swap16(x) _OSSwapInt16(x)
#define swap32(x) _OSSwapInt32(x)
#define swap64(x) _OSSwapInt64(x)
#elif __INTEL__
#define swap16(x) __bswap_16(x)
#define swap32(x) __bswap_32(x)
#define swap64(x) __bswap_64(x)
#else
#define swap16(x) x
#define swap32(x) x
#define swap64(x) x
#endif

int svc_getElfSymbolByName(Svc_Elf_Info_T *efi,  char *symbol, unsigned long *vaddr, unsigned long *size, unsigned long *filepos );
ElfProgramSegmentHeader_T *svc_getElfPhdr( ElfSectionHeader_T *shdr, void *mapped_addr );
unsigned long svc_getElfSegmentPaddr( ElfSectionHeader_T *shdr, void *mapped_addr );
unsigned long svc_getElfPhdrType( ElfSectionHeader_T *shdr, void *mapped_addr );
char *svc_getElfSegmentName( int snum, void *mapped_addr );
int svc_makeFirmwareImage( Svc_Firmware_Info_T *bl_info );

int svc_loadElfFile( Svc_Elf_Info_T *elf_info, char* input_file_name, char* output_file_name );
int svc_unloadElfFile( Svc_Elf_Info_T* elf_info );
int svc_loadFirmware( Svc_Firmware_Info_T *sbi, char *fn_in, char *fn_out );

// svc_pers.c:
void svc_pers_CopySwapCrc( Personality_t *in, Personality_t *out );

// svc_elf_debug.c:
void svc_showElfFileHeader_T( ElfFileHeader_T *eh );
void svc_showElfProgramHeader( ElfProgramSegmentHeader_T *ph, int pnum );
void svc_showElfSectionHeader_T( ElfSectionHeader_T *sh, int snum, void *mapped_addr );
void svc_showElfSymbolTable( ElfSectionHeader_T *shdr, void *mapped_addr );
void svc_showElfStringTable( ElfSectionHeader_T *shdr, void *mapped_addr );

// svc_crc16n.c
uint16_t _bgp_Crc16n( unsigned short usInitialCrc, unsigned char *pData, unsigned long ulLen );


#endif // Add nothing below this line.

