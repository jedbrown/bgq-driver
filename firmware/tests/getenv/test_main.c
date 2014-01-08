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


#include <stdlib.h>
#include <stdio.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <firmware/include/fwext/fwext_lib.h>

//extern void Terminate(int);

#define ASSERT( condition, message ) \
  if ( ! (condition) ) { printf( "[ERROR] : Assertion failed at  %s:%s():%d\n", __FILE__, __func__, __LINE__); \
    printf message ; \
    Terminate(-__LINE__); \
  }

int test_main( void ) {

  if (ProcessorID() > 0) {
    _Bg_Sleep_Forever();
  }


  char* key; 
  char* value;
  unsigned long converted;

  
  /*
   * Test basic base 16 parsing
   */

  key =  "BABEFACE";
  value = fwext_getenv(key);
  converted = fwext_strtoul(value, 0, 16);

  ASSERT( fwext_strcmp(key,value) == 0, ("Error: fwext_getenv(%s)=%s (expected %s)\n", key, value, key)  );
  ASSERT( converted == 0xBABEFACE, ( "Error: strtoul(%s,0,16)=0x%08lX\n", value, converted ) );

  
  /*
   * Test parsing of explicitly identifed hexadecimal values (using both explicit
   * and implicit base identification)
   */

  key =  "0xBABEFACE";
  value = fwext_getenv(key);
  converted = fwext_strtoul(value, 0, 16);

  ASSERT( fwext_strcmp(key,value) == 0, ("Error: fwext_getenv(%s)=%s (expected %s)\n", key, value, key)  );
  ASSERT( converted == 0xBABEFACE, ( "Error: strtoul(%s,0,16)=0x%08lX\n", value, converted ) );

  converted = fwext_strtoul(value, 0, 0);

  ASSERT( fwext_strcmp(key,value) == 0, ("Error: fwext_getenv(%s)=%s (expected %s)\n", key, value, key)  );
  ASSERT( converted == 0xBABEFACE, ( "Error: strtoul(%s,0,)=0x%08lX\n", value, converted ) );


  key =  "0XBABEFACE";
  value = fwext_getenv(key);
  converted = fwext_strtoul(value, 0, 16);

  ASSERT( fwext_strcmp(key,value) == 0, ("Error: fwext_getenv(%s)=%s (expected %s)\n", key, value, key)  );
  ASSERT( converted == 0xBABEFACE, ( "Error: strtoul(%s,0,16)=0x%08lX\n", value, converted ) );

  converted = fwext_strtoul(value, 0, 0);

  ASSERT( fwext_strcmp(key,value) == 0, ("Error: fwext_getenv(%s)=%s (expected %s)\n", key, value, key)  );
  ASSERT( converted == 0xBABEFACE, ( "Error: strtoul(%s,0,)=0x%08lX\n", value, converted ) );


  /*
   * Test parsing of base 10
   */

  key =  "12345678";
  value = fwext_getenv(key);
  converted = fwext_strtoul(value, 0, 10);

  ASSERT( fwext_strcmp(key,value) == 0, ("Error: fwext_getenv(%s)=%s (expected %s)\n", key, value, key)  );
  ASSERT( converted == 12345678, ( "Error: strtoul(%s,0,10)=%ld\n", value, converted ) );

  converted = fwext_strtoul(value, 0, 0);

  ASSERT( fwext_strcmp(key,value) == 0, ("Error: fwext_getenv(%s)=%s (expected %s)\n", key, value, key)  );
  ASSERT( converted == 12345678, ( "Error: strtoul(%s,0,0)=%ld\n", value, converted ) );

  
  /*
   * Test the handling of the second argumenter (remainder string pointer)
   */
  
  key   = "10remainder";
  value = fwext_getenv(key);

  ASSERT( fwext_strcmp(key,value) == 0, ("Error: fwext_getenv(%s)=%s (expected %s)\n", key, value, key)  );
  converted = fwext_strtoul(value, &value, 10);
  ASSERT( converted == 10, ( "Error: strtoul(%s,ptr,10)=%ld\n", key, converted ) );
  ASSERT( fwext_strcmp(value,"remainder") == 0, ("Error: 2nd arg result is %p -> %s\n", value, value ) );
  
  
  Terminate(0);

  return 0; // shut up the compiler
}



