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

#define MAX_VAR_LEN 128
#define MAX_VARS     64

#define OPTION_ELF          "--el[f]"
#define OPTION_ELF_LEN      4
#define OPTION_PATCHED      "--p[atched]"
#define OPTION_PATCHED_LEN  3
#define OPTION_ENV_FILE     "--f"
#define OPTION_ENV_FILE_LEN 3
#define OPTION_ENV_VAR      "--env" 
#define OPTION_ENV_VAR_LEN  3
#define OPTION_DEBUG        "--debug"
#define OPTION_DEBUG_LEN    7
#define OPTION_VERBOSE      "--verbose"
#define OPTION_VERBOSE_LEN  3
#define OPTION_SYMBOL       "--s[ymbol]"
#define OPTION_SYMBOL_LEN   3

char  env[MAX_VARS][MAX_VAR_LEN];
int   envPointer = -1;
char* elfFile = 0;
char* patchedElfFile = 0;
char* propertiesFile = 0;

char  symbol[MAX_VARS][MAX_VAR_LEN];
int   symbolPointer = -1;

extern int debug;

void  parseArgs(int argc, char* argv[]);
char* svc_nextString(char** bufp);
int   svc_getEnvVars( Svc_Elf_Info_T *elfInfo, Svc_EnvVars_T* envVars );
int   svc_bloader_setEnvVars( Svc_Elf_Info_T *elfInfo, Svc_EnvVars_T* env_vars);
int   svc_editEnvVars( char* p, Svc_EnvVars_T* env_vars ); 
int   svc_processPropertiesFile(char* file, Svc_EnvVars_T* envVars );
void  svc_ShowEnvVars( Svc_Elf_Info_T* elfInfo, Svc_EnvVars_T* env_vars );
int   svc_editSymbol(char* symbol, Svc_Elf_Info_T*);

int main( int argc, char *argv[], char **envp ) {

  int i;
  Svc_Elf_Info_T elfInfo;
  Svc_EnvVars_T  envVars;

  parseArgs(argc, argv);

  /* -------------------------------------
   * Load the ELF image ...
   * ------------------------------------- */

  if ( svc_loadElfFile( &elfInfo, elfFile, patchedElfFile ) != 0 ) {
    fprintf(stderr, "(E) error loading file.\n");
    exit(-1);
  }

  /* ----------------------------------------------
   *  Locate the existing environment variables 
   *  section (only if needed):
   * ---------------------------------------------- */

  if ( ( propertiesFile != 0 ) || ( envPointer >= 0 ) ) {
    if ( svc_getEnvVars( &elfInfo, &envVars ) != 0 ) {
      exit(-1);
    }
  }

  /* ------------------------------------------------
   * Process the properties (env) file, if specified:
   * ------------------------------------------------ */

  if ( propertiesFile != 0 ) {
    if ( svc_processPropertiesFile( propertiesFile, &envVars ) != 0 ) {
      exit(-1);
    }
  }

  /* ------------------------------------------------
   * Process any command line environment variables:
   * ------------------------------------------------ */
  
  for (i = 0; i <= envPointer; i++) {
    svc_editEnvVars( env[i], &envVars );
  }

  /* -------------------------------------
   * Process any symbol table edits:
   * ------------------------------------- */

  for ( i = 0; i <= symbolPointer; i++ ) {

    if (debug)
      printf("Editing symbol %s ...\n", symbol[i]);

    if ( svc_editSymbol( symbol[i], &elfInfo ) ) {
      exit(-1);
    }
  }

  /* -------------------------------------
   * Write out the updated ELF image:
   * ------------------------------------- */

  if ( ( propertiesFile != 0 ) || ( envPointer >= 0 ) ) {
    if ( svc_bloader_setEnvVars( &elfInfo, &envVars ) != 0 ) {
      exit(-1);
    }
  }

  if (verbose) {
    if ( ( propertiesFile != 0 ) || ( envPointer >= 0 ) ) {
      svc_ShowEnvVars( &elfInfo, &envVars );
    }
  }

  if (svc_unloadElfFile( &elfInfo ) ) {
    exit(-1);
  }

  return 0;
}


void parseArgs(int argc, char* argv[]) {

  int i;

  for (i = 1; i < argc; i++) {

    if ( ! strncasecmp( argv[i], OPTION_ELF, OPTION_ELF_LEN ) ) {
      elfFile = argv[++i];
    }
    else if ( ! strncasecmp( argv[i], OPTION_PATCHED, OPTION_PATCHED_LEN ) ) {
      patchedElfFile = argv[++i];
    }
    else if ( !strncasecmp( argv[i], OPTION_ENV_FILE, OPTION_ENV_FILE_LEN ) ) {
      propertiesFile = argv[++i];
    }
    else if ( !strncasecmp( argv[i], OPTION_ENV_VAR, OPTION_ENV_VAR_LEN ) ) {
      strncpy( env[++envPointer], argv[++i], MAX_VAR_LEN );
      if (envPointer > MAX_VARS) {
	fprintf(stderr, "(E) Too many command line environment variables specifed (try using the --f option)\n");
	exit(-1);
      }
    }
    else if ( !strncasecmp( argv[i], OPTION_SYMBOL, OPTION_SYMBOL_LEN ) ) {
      strncpy( symbol[++symbolPointer], argv[++i], MAX_VAR_LEN );
      
      if (debug)
	printf(" --symbol --> %s\n", symbol[symbolPointer]);

      if (symbolPointer > MAX_VARS) {
	fprintf(stderr, "(E) Too many command line symbol table entries specifed.\n");
	exit(-1);
      }
    }
    else if ( ! strncasecmp( argv[i], OPTION_VERBOSE, OPTION_VERBOSE_LEN ) ) {
      verbose = 1;
    }
    else if ( ! strncasecmp( argv[i], OPTION_DEBUG, OPTION_DEBUG_LEN ) ) {
      debug++;
    }
    else {
      printf(
	     "Usage: fw_ddr_update %s <fw-ddr-image> [%s <fw-ddr-output-image>] [%s <properties-file>] [%s name=value] [%s] [%s] [%s name=value]\n", 
	     OPTION_ELF, 
	     OPTION_PATCHED, 
	     OPTION_ENV_FILE, 
	     OPTION_ENV_VAR,
	     OPTION_VERBOSE,
	     OPTION_DEBUG,
	     OPTION_SYMBOL
	     );
      exit(-1);
    }
  }
}


char* svc_nextString(char** bufp) {

  char* result = *bufp;

  if ( **bufp != 0 ) {
    while ( *((*bufp)++) != 0 );
  }

  return result;
}


int svc_getEnvVars( Svc_Elf_Info_T *elfInfo, Svc_EnvVars_T* envVars ) {

  /*
   * Locate the environment variables via the symbol table and ensure
   * that it is the appropriate size, version, etc.
   */
  
  envVars->environmentVariablesAddress = 0;
  envVars->environmentVariablesSize    = 0;
  envVars->environmentVariablesFilePosition = 0;

  int rc = svc_getElfSymbolByName( elfInfo,
				SVC_FIRMWARE_ENV_VARS_SYMBOL,
				&(envVars->environmentVariablesAddress),
				&(envVars->environmentVariablesSize),
				&(envVars->environmentVariablesFilePosition) );

  if (rc != 0) {
    fprintf(stderr, "(E) No envvars section found in elf file.\n");
    return -1;
  }

  if (verbose) {
      printf("(I) Firmware Environment Variables : addr=0x%0lX size=%ld fpos:0x%0lX\n",   envVars->environmentVariablesAddress, envVars->environmentVariablesSize, envVars->environmentVariablesFilePosition );
  }

  /* clear out the name/value pointers */
  memset( envVars->name,  0, sizeof(envVars->name));
  memset( envVars->value, 0, sizeof(envVars->value));

  /* copy the data ... its all string data so there are no endian issues to worry about */

  int n = 0;
  char* bufp = elfInfo->mappedAddress + envVars->environmentVariablesFilePosition;

  while (*bufp != 0 ) {
    strcpy(envVars->name[n], svc_nextString(&bufp));
    strcpy(envVars->value[n], svc_nextString(&bufp));
    if (debug) printf("Found environment variable %d : %s = %s [%s:%d]\n", n, envVars->name[n], envVars->value[n], __func__, __LINE__);
    n++;
  }

  return 0;
}

int svc_bloader_setEnvVars( Svc_Elf_Info_T *elfInfo, Svc_EnvVars_T* envVars) {

  //memcpy( elfInfo->mappedAddress + envVars->environmentVariablesFilePosition, envVars->buffer, SVC_ENV_VAR_BUFFER_SIZE );
  char* bufp = elfInfo->mappedAddress + envVars->environmentVariablesFilePosition;
  int n = 0;

  while ( envVars->name[n][0] != 0 ) {
    strcpy( bufp, envVars->name[n] );
    bufp += strlen(envVars->name[n]) + 1;
    strcpy( bufp, envVars->value[n] );
    bufp += strlen(envVars->value[n]) + 1;
    n++;
  }

  *bufp++ = 0;

  return 0;
}

int svc_editEnvVars( char* p, Svc_EnvVars_T* env_vars ) {

  unsigned n = 0;

  char* name = p;
  char* value;

  /*
   * p contains a string of the format "name=value".  Find the "=" sign
   * to split into name and value:
   */

  while ( (*p) != 0  && (*p != '=') ) {
    p++;
  }

  *p = 0;
  value = p+1;



  for (n = 0; n < SVC_MAX_ENV_VARS; n++) {
    
    if ( env_vars->name[n][0] == 0 ) {
      strcpy(env_vars->name[n], name);
      strcpy(env_vars->value[n], value);
      if (debug) printf("Added env variable %d) %s = %s\n", n, name, value);
      break;
    }
    else if ( strcmp(name, env_vars->name[n]) == 0 ) { // override?
      strcpy(env_vars->value[n], value);
      if (debug) printf("Overrode env variable %d) %s = %s\n", n, name, value);
      break;
    }
  }

   return 0;
}

int svc_processPropertiesFile(char* file, Svc_EnvVars_T* envVars ) {

  const int BUFSIZE = 1024;
  char  buff[BUFSIZE];
  FILE* fp = fopen(file, "r");

  if ( fp == 0 ) {
    fprintf(stderr,"(E) couldnt open properties file \"%s\"\n", file);
    return -1;
  }

  while ( fgets(buff, BUFSIZE, fp) != 0 ) {
    
    /* strip off the trailing new-line */
    if ( buff[strlen(buff)-1] == '\n' ) {
      buff[strlen(buff)-1] = 0;
    }

    if (debug) printf("Process property : %s [%s:%d]\n", buff, __func__, __LINE__);

    if (strlen(buff) > 0 ) {
      if ( buff[0] != '#' ) {
	svc_editEnvVars(buff, envVars);
      }
    }
  }

  fclose(fp);
  return 0;
}

void svc_ShowEnvVars( Svc_Elf_Info_T* elfInfo, Svc_EnvVars_T* envVars ) {

  int n;

  printf("Environment variables:\n");
  for (n = 0; envVars->name[n][0] != 0; n++) {
    printf("  %-16s = %s\n", envVars->name[n], envVars->value[n] );
  }

  if (debug) {
    unsigned* w = elfInfo->mappedAddress + envVars->environmentVariablesFilePosition;
    int i;
    int toggle = 0;

    for (i = 0; i < 0x1000/sizeof(unsigned); i+= 4) {

      printf("  %04X : %08X-%08X-%08X-%08X  /*", i*4, swap32(w[i]), swap32(w[i+1]), swap32(w[i+2]), swap32(w[i+3]) );

      char* c = (char*)&(w[i]);
      int j;

      for (j = 0; j < 16; j++) {
	char cc;
	if (isprint(c[j]) )
	  cc = c[j];
	else if ( c[j] == 0) {
	  cc = toggle ? ' ' : '=';
	  toggle ^= 1;
	}
	else {
	  cc = '?';
	}

	printf( "%c", cc);
      }
      printf(" */\n");
    }
  }
  
}

int svc_editSymbol(char* symbol, Svc_Elf_Info_T* elfInfo ) {
  
  char* name = symbol;
  char* type;
  char* value;

  /*
   * p contains a string of the format "name=value".  Find the "=" sign
   * to split into name and value:
   */

  while ( (*symbol) != 0  && (*symbol != '=') ) {
    symbol++;
  }

  *symbol++ = 0;
  type = symbol;

  /*
   * The value is of the form type:val
   */

  while ( (*symbol) != 0  && (*symbol != ':') ) {
    symbol++;
  }

  *symbol++ = 0;
  value = symbol;

  unsigned long vaddr = 0, size = 0, fpos = 0;

  int rc = svc_getElfSymbolByName( elfInfo, name, &vaddr, &size, &fpos );

  if (verbose) {
    printf("(I) Editing symbol \"%s\" of type %s value=%s address(0x%lX,0x%lX)\n", name, type, value, vaddr, fpos);
  }

  if ( rc == 0 ) {
    
    if (strcmp(type, "i32") == 0 ) {
      uint32_t  v = strtoul( value, 0, 0 );
      uint32_t* ptr = (uint32_t*)(elfInfo->mappedAddress + fpos);
      *ptr = swap32(v);
    }
    else if (strcmp(type, "i64") == 0 ) {
      uint64_t  v = strtoull( value, 0, 0 );
      uint64_t* ptr = (uint64_t*)(elfInfo->mappedAddress + fpos);
      *ptr = swap64(v);
    }
    else {
      fprintf(stderr, "(E) unknown symbol type \"%s\" (must be one of {i32, i64, s}\n", type);
      rc = -1;
    }
  }


  return rc;

}
