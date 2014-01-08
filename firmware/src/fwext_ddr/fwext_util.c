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


#include <firmware/include/fwext/fwext.h>
#include <firmware/include/mailbox.h>
#include <hwi/include/bqc/testint_tdr.h>
#include <hwi/include/bqc/testint_inlines.h>
#include <firmware/include/fwext/fwext_lib.h>
#include <stdio.h>

__INLINE__ int fw_isspace( char c ) {
  return ( ( c == ' ' ) || ( c == '\t') ) ? 1 : 0;
}

__INLINE__ int fw_isdigit( char c ) {
  return ( ( c >= '0' ) && ( c <= '9') ) ? 1 : 0;
}

__INLINE__ int fw_ishexlower( char c ) {
  return ( (c >= 'a') && (c <= 'f') ) ? 1 : 0;
}

__INLINE__ int fw_ishexupper( char c ) {
  return ( (c >= 'A') && (c <= 'F') ) ? 1 : 0;
}


// Warning: Assumes "force load/store alignment" is off, i.e., misaligned ld/st is ok!
__INLINE__ void *memcpy_Inline( void *dest, const void *src, size_t n )
{
   uint64_t *s64 = (uint64_t *)src;
   uint64_t *d64 = (uint64_t *)dest;
   dcache_block_touch( (void *)s64 );
   size_t h32  = (n >> 5);
   size_t t8, t1;
   register uint64_t ra, rb, rc, rd;

   while ( h32-- )
      {
      dcache_block_touch( (void *)(s64 + 4) );
      ra     = *s64++;
      rb     = *s64++;
      rc     = *s64++;
      rd     = *s64++;
      *d64++ = ra;
      *d64++ = rb;
      *d64++ = rc;
      *d64++ = rd;
      }

   if ( (t8 = ((n & 0x18) >> 3)) )
      {
      while ( t8-- )
         *d64++ = *s64++;
      }

   if ( (t1 = (n & 0x7)) )
      {
      uint8_t *s8 = (uint8_t *)s64;
      uint8_t *d8 = (uint8_t *)d64;

      while ( t1-- )
         *d8++ = *s8++;
      }

   return dest;
}


__INLINE__ void bzero_Inline( void *dest, size_t n )
{
    size_t h32  = (n >> 5);
    size_t t8, t1;
    uint64_t *d64 = (uint64_t *)dest;

    while ( h32-- )
    {
        *d64++ = 0;
        *d64++ = 0;
        *d64++ = 0;
        *d64++ = 0;
    }

    if ( (t8 = ((n & 0x18) >> 3)) )
    {
        while ( t8-- )
            *d64++ = 0;
    }

    if ( (t1 = (n & 0x7)) )
    {
        uint8_t *d8 = (uint8_t *)d64;

        while ( t1-- )
            *d8++ = 0;
    }
}



//
// NOTE: See util.h which contains #defines that work better!
//
//
// Misc. utility functions
//

int fwext_strcmp(const char* s1, const char* s2) {

  while (1) {

    if ( *s1 == *s2 ) {

      if (*s1 == 0) {
	return 0;
      }
      
      s1++;
      s2++;
    }
    else {
      return (*s1 < *s2) ? -1 : 1;
    }
  }
}

int fwext_strncpy(char* to, const char* from, int n) {

  int count = 0;
  
  while ( (*from != 0) && (count < n) ) {
    *to++ = *from++;
    count++;
  }

  return count;
}

void __NORETURN Kernel_Crash( int status )
{
    FWExtState.firmware->terminate( status );

    while (1);
}



void fwext_udelay( uint64_t microseconds ) {
  uint64_t end = GetTimeBase() + ( microseconds * FWExtState.personality->Kernel_Config.FreqMHz );
  while ( GetTimeBase() < end ) 
    /* spin */
    ;

}


void __NORETURN exit( int status ) {

    // Livelock counter detection
    if ( ( status == 0 ) && ( fwext_getenv("BG_LLBCHECK") != 0 ) && ( ! TI_isDD1() ) ) {

	size_t cntIndex = BgGetRealLockIndex(FWEXT_LOCKNUM_TEST_EXIT_CNT);
	int cnt = BeDRAM_ReadIncWrap(cntIndex) + 1;
	
	while ( cnt < BgGetNumThreads() ) {
	    fwext_udelay(1000);
	    cnt = BeDRAM_Read(FWEXT_LOCKNUM_TEST_EXIT_CNT);
	}

	if ( ProcessorThreadID() == 0 ) {
	    extern void fwext_check_llb_firs();
	    fwext_check_llb_firs();
	}
    }

    FWExtState.firmware->exit( status ); // never returns
    while (1); // shut up the compiler
}

void __NORETURN Terminate( int status )
{
  FWExtState.firmware->terminate(status); // never returns
  while (1); // shut up the compiler
}

int putchar(int c)
{
   char str[2];
   str[0] = c;
   str[1] = 0;
   int rc = FWExtState.firmware->putn( str, 2 );
   return rc;
}

int puts( const char *s )
{
  char buffer[4096+1];
  int n = fwext_strncpy(buffer, s, sizeof(buffer) - 1);
  buffer[n] = '\n';
  buffer[n+1] = 0;
  int rc = FWExtState.firmware->putn( buffer, n+1 );

  if ((n+1) > sizeof(buffer))
  {
    (void) FWExtState.firmware->putn( "PUTS OVERFLOWED\n", 16 );
    Terminate(__LINE__);
  }

  return( rc );
}

int printf( const char *fmt, ... )
{
    va_list args;
    char buffer[4096];
    char* buffp = buffer;

    va_start( args, fmt );
    
    //DELETEME int len = FWExtState.firmware->vprintf( fmt, args );
    int len = vsprintf( buffp, fmt, args );
    FWExtState.firmware->putn( buffer, len );

    va_end(args);

    if ( len > sizeof(buffer))
    {
      (void) FWExtState.firmware->putn( "PRINTF OVERFLOWED\n", 18 );
      Terminate(__LINE__);
    }

    return len;
}

int sprintf(char* buf, const char *fmt, ... )
{
    va_list args;
    va_start( args, fmt );
    int len = vsprintf( buf, fmt, args );
    va_end( args );
    return len;
}

int writeRASEvent(uint32_t message_id, uint16_t number_of_details, const uint64_t details[])  
{
  return FWExtState.firmware->writeRASEvent(message_id, number_of_details, (fw_uint64_t *)details);
}

int Kernel_InjectRAWRAS(uint32_t message_id, size_t raslength, const uint64_t* rasdata) {
    return writeRASEvent(message_id, raslength, rasdata);
}

void *memcpy( void *dest, const void *src, size_t n )
{
   uint64_t *s64 = (uint64_t *)src;
   uint64_t *d64 = (uint64_t *)dest;
   dcache_block_touch( (void *)s64 );
   size_t h32  = (n >> 5);
   size_t t8, t1;
   register uint64_t ra, rb, rc, rd;

   while ( h32-- )
      {
      dcache_block_touch( (void *)(s64 + 4) );
      ra     = *s64++;
      rb     = *s64++;
      rc     = *s64++;
      rd     = *s64++;
      *d64++ = ra;
      *d64++ = rb;
      *d64++ = rc;
      *d64++ = rd;
      }

   if ( (t8 = ((n & 0x18) >> 3)) )
      {
      while ( t8-- )
         *d64++ = *s64++;
      }

   if ( (t1 = (n & 0x7)) )
      {
      uint8_t *s8 = (uint8_t *)s64;
      uint8_t *d8 = (uint8_t *)d64;

      while ( t1-- )
         *d8++ = *s8++;
      }

   return dest;
}

void* memset(void* dest, int value, size_t n )
{
    if ( !value )
       {
       bzero_Inline( dest, n );
       }
    else
       {

	 if ( (n > sizeof(uint64_t)) && ( ((uint64_t)dest & 0xF) == 0 ) ) {
	   uint64_t v = (uint64_t)(value & 0xFF);
	   uint64_t fill = (v << 0) | (v << 8) | (v << 16) | (v << 24) | (v<<32) | (v<<40) | (v << 48) | (v << 56);
	   for ( ; n > sizeof(uint64_t) ; dest += sizeof(uint64_t), n -= sizeof(uint64_t) ) 
	     *((uint64_t*)dest) = fill;
	 }

       uint8_t* d = (uint8_t*)dest;
       while (n--)
           *d++ = value;
       }
    return dest;
}

//
// ascii to integer conversion
//
int atoi( const char *str )
{
    int is_neg = 0;
    int value = 0;
    char *p = (char *)str;
    char *q = NULL;

    if ( !p )
        return(0); // terminated string. exit with zero
    while ( fw_isspace(*p) )
        p++;
    if ( !*p )
        return(0); // terminated string. exit with zero
    if ( *p == '-' )
    {
        is_neg++;
        p++;
    }
    else if ( *p == '+' )
    {
        p++;
    }
    if ( !*p )
        return( 0 ); // terminated string. exit with zero
    q = p;
    while ( fw_isdigit(*q) )
        q++;
    while ( p < q )
    {
        value *= 10;
        value += (*p - '0');
        p++;
    }
    return(is_neg ? (-(value)) : value);
}

void __NORETURN __assert_fail(const char* assertion, const char* file, unsigned int line, const char* function)
{
    printf("assertion failed: file=%s line=%d function=%s  assertion=%s\n", file, line, function, assertion);
    Terminate(1);
}

extern uint64_t __KERNEL_END;
uint64_t _malloc_pointer = (uint64_t) &__KERNEL_END;

void* malloc(size_t size)
{
   void *ptr = NULL;

   while(BeDRAM_ReadIncSat(BeDRAM_LOCKNUM_MALLOCLOCK) != 0) /* spin */;
   _malloc_pointer = (_malloc_pointer + 31) & (~0x1f); // 32-byte alignment
   if ((_malloc_pointer + size) <= FWExtState.vaddrLimit)
   {
       ptr = (void *) _malloc_pointer;
       _malloc_pointer += size;
   }
   ppc_msync();
   BeDRAM_ReadClear(BeDRAM_LOCKNUM_MALLOCLOCK);

   // For now at least, terminate the calling program if malloc failed.
   if (ptr == NULL)
   {
       (void) FWExtState.firmware->putn("MALLOC FAILED - OUT OF MEMORY\n", 30);
       Terminate(1);
   }
   
   return ptr;
}

void free(void* ptr)
{
   return;
}

int FWEXT_DATA __gxx_personality_v0;

char FWEXT_ENV_VARS fwext_EnvVars[1024 *4];

char* fwext_nextString(char** bufp) {

  char* result = *bufp;

  if ( **bufp != 0 ) {
    while ( *((*bufp)++) != 0 );
  }

  return result;
}

char* fwext_getenv(const char* key) {

  /* ####################################################
   * For now, we simply perform a linear search of
   * the list
   * #################################################### */

  char* bufp = &(fwext_EnvVars[0]);
  char* name;

  do {
    name = fwext_nextString(&bufp);
    char* value = fwext_nextString(&bufp);
    
    if ( ! fwext_strcmp(key,name) ) {
      return value;
    }
  } while ( name[0] != 0 );
  return 0;
}


unsigned long fwext_strtoul(const char* s, char** endp, int base) {
  
  unsigned long result = 0xBADC0DE;

  if (s == 0)
    goto strtoul_out;

  if ( ( base == 0  ) || (base == 10) || (base == 16) ) {

    // Ignore leading white space:
    while ( fw_isspace(*s) ) s++;

    if (base == 0) {

      if ( s[0] == '0' ) {
	if ( ( s[1] == 'x' ) || ( s[1] == 'X' ) ) {
	  base = 16;
	  s += 2;
	}
	else if ( s[1] == 0 ) { // Just plain "0"
	  result = 0;
	  goto strtoul_out;
	}
	else 
	  goto strtoul_out; // base 8 is unsupported
      }
      else {
	base = 10;
      }
    
    }

    result = 0;
      
    if (base == 10) {
      while (1) {
	
	unsigned long digit = 0;
	
	if ( fw_isdigit(*s) ) {
	  digit = *s - '0';
	}
	else {
	  goto strtoul_out;
	}

	result = (result*10) + digit;
	s++;
      }
    }
    else if (base == 16) {

      // Leading 0x or 0X is ok:
      if ( ( s[1] == 'x' ) || ( s[1] == 'X' ) ) {
	s += 2;
      }
      
      while (1) {
	
	unsigned long nibble = 0;

	if ( fw_isdigit(*s) ) {
	  nibble = *s - '0';
	}
	else if ( fw_ishexlower(*s) ) {
	  nibble = *s - 'a' + 10;
	}
	else if ( fw_ishexupper(*s) ) {
	  nibble = *s - 'A' + 10;
	}
	else {
	  goto strtoul_out;
	}

	result = (result << 4) | nibble;
	s++;
      }
    }
  }

  strtoul_out:

    if (endp != 0) {
      *endp = (char*)s;
    }

    return result;
}

long fwext_strtol(const char* s, char** endp, int base) {
  
  unsigned long result = 0xBADC0DE;

  if (s == 0)
    goto strtol_out;

  if ( ( base == 0  ) || (base == 10) || (base == 16) ) {

    // Ignore leading white space:
    while ( fw_isspace(*s) ) s++;

    int negative = (s[0] == '-') ? (s++,1) : 0;

    unsigned long value = fwext_strtoul( s, endp, base );

    result = negative ? (0 - (long)value) : value;
  }

  strtol_out:
    return result;
}

int fwext_termCheck( void ) {
  return FWExtState.firmware->termCheck();
}


int fwext_get( const char* buffer, unsigned bufferLength ) {

  int rc;
  
  fw_uint32_t messageType = 0;

  struct {
      MailBoxPayload_Stdin_t header;
      char buffer[1024*4];
  } stage;
  
  do {
      //rc = FWExtState.firmware->get( buffer, bufferLength );
      rc = FWExtState.firmware->pollInbox( (void*)&stage, &messageType, sizeof(stage) );
  } while ( ( rc == FW_EAGAIN ) || ( messageType != JMB_CMD2CORE_STDIN ) );

  if ( ( messageType == JMB_CMD2CORE_STDIN ) && ( rc > 0 ) ) {
      if ( rc > bufferLength ) {
	  rc = FW_TOO_BIG;
      }
      else {
	  memcpy( (void*)buffer, stage.buffer, rc );
      }
  }

  return rc;
}

void FW_Warning( const char* fmt, ... ) {

    char buffer[1024];

   va_list args;
   va_start( args, fmt );

   int len = 0;
   len = vsprintf( buffer, fmt, args );
   va_end( args );

   printf("(W) %s\n", buffer);
}

void FW_Error( const char* fmt, ... ) {

    char buffer[1024];

   va_list args;
   va_start( args, fmt );

   int len = 0;
   len = vsprintf( buffer, fmt, args );
   va_end( args );

   printf("(E) %s\n", buffer);
}


void FW_RAS_printf( const uint32_t msg_id, const char* fmt, ... ) {

   va_list args;
   va_start( args, fmt );

   int len = 0;
   char buffer[256];
    

   len = vsprintf( buffer, fmt, args );
   va_end( args );
   buffer[len] = 0;

   extern int _fw_writeRASString(uint32_t , const char*  );
   fwext_getFwInterface()->writeRASString( msg_id, buffer);

   if (len > sizeof(buffer)) {
       printf("PRINTF OVERFLOWED\n");
       asm volatile( "b 0"); //crash(__LINE__);
   }

}
