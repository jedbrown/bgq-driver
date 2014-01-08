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

#include <stdarg.h>
#include "fwext_lib.h"

// get the goodies
#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/bqc/A2_inlines.h>




extern int vsprintf( char *buf, const char* fmt, va_list args );


int STRLEN(const char* s) {
  int len = 0;
  while (*s++)
    len++;
  return len;
}

int STRNCMP(const char* a, const char* b, int n) {
  int i;
  for (i = 0; i < n; i++) {
    if ( a[i] != b[i] ) 
      return -1-i; // "add" one to detect miscompares in the first character
  }
  return 0;
}

void runTest( int lineNo, const char* expected, const char* fmt, ... ) {

  /* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
   * @                                                               @
   * @ NOTE: in order to have a self-checking test, we run things    @
   * @       through the internal vsprintf routine (versus actually  @
   * @       printing them).  The length and content of the output   @
   * @       is compared against expected values.  Since printf is   @
   * @       inherently untrusted in this test, we encode            @
   * @       failure in an exit (crash) code.                        @
   * @                                                               @
   * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */

  char buffer[256];

  va_list args;
  va_start( args, fmt );
  int actual = vsprintf( buffer, fmt, args );
  va_end( args );

#if 0
  // Useful for debugging
  puts(expected);
  puts(buffer);
#endif


  int len = STRLEN(expected);
  
  if ( actual != len ) {

    /*
     * Error code format:
     *
     *   +----+----+--------+
     *   | 01 | // | lineNo |
     *   +----+----+--------+
     *   0   7 8 15 16    31
     */

    unsigned long code = (1 << 24) | lineNo;
    Terminate(code);
  }

  int cmp = STRNCMP( buffer, expected, STRLEN(expected) );

  if (cmp < 0) {

    /*
     * Error code format:
     *
     *   +-----+------+--------+
     *   | idx | char | lineNo |
     *   +-----+------+--------+
     *   0    7 8   15 16    31
     */

    int code = 
      (-cmp << 24) | 
      (buffer[-cmp] << 16) |
      lineNo
      ;

    Terminate(code);
  } 
}
  
//int main( int argc, char *argv[], char **envp ) {

int test_main(void) {

  if (ProcessorID() ==0) {

    runTest( __LINE__, "hello",      "hello" );
    runTest( __LINE__, "%s:hello",               "%%s:%s",         "hello"                     ); // string
    runTest( __LINE__, "%c:@",                   "%%c:%c",         '@'                         ); // char
    runTest( __LINE__, "%u:1234567",             "%%u:%u",         1234567ul                   ); // unsigned
    runTest( __LINE__, "%d:-1234567",            "%%d:%d",         -1234567l                   ); // int
    runTest( __LINE__, "%016X:0123456789ABCDEF", "%%016X:%016X",   0x123456789abcdefUL         ); // hex
    runTest( __LINE__, "%x:123456789abcdef",     "%%x:%x",         0x0123456789ABCDEFUL        ); // another hex
    runTest( __LINE__, "%p:1122334455667788",    "%%p:%p",         (void*)0x1122334455667788UL ); // pointer
    runTest( __LINE__, "%lx:123456789abcdef",    "%%lx:%lx",       0x123456789abcdefUL         ); // long
    runTest( __LINE__, "%llx:123456789abcdef",   "%%llx:%llx",     0x123456789abcdefULL        ); // long long

    // Padding and justification tests 

    runTest( __LINE__, "%12d:     1234567",      "%%12d:%12d",     1234567  ); // padded (positive)
    runTest( __LINE__, "%12d:    -1234567",      "%%12d:%12d",    -1234567  ); // padded (negative)
    runTest( __LINE__, "%-12d:1234567     |",    "%%-12d:%-12d|",  1234567  ); // left justified (positive)
    runTest( __LINE__, "%-12d:-1234567    |",    "%%-12d:%-12d|", -1234567  ); // left justified (negative)
    runTest( __LINE__, "%012d:000001234567",     "%%012d:%012d",   1234567  ); // zero-padded (postiive)
    runTest( __LINE__, "%012d:-00001234567",     "%%012d:%012d",  -1234567  ); // zero-padded (negative)
    runTest( __LINE__, "%+d:+1234567",           "%%+d:%+d",       1234567  ); // forced sign (positive)
    runTest( __LINE__, "%+d:-1234567",           "%%+d:%+d",      -1234567  ); // forced sign (negative)

    // Grouping tests (decimal and hexadecimal)

    runTest( __LINE__, "%\\d:123,456,789", "%%\\d:%\\d", 123456789 );
    runTest( __LINE__, "%\\lX:1122_3344_5566_7788", "%%\\lX:%\\lX", 0x1122334455667788ul);


    runTest( __LINE__, "0x1.921f9f01b866ep+000001", "%g", 3.14159 );
    runTest( __LINE__, "0x1.921f9f01b866ep+000001", "%G", 3.14159 );
    runTest( __LINE__, "0x1.921f9f01b866ep+000001", "%e", 3.14159 );
    runTest( __LINE__, "0x1.921f9f01b866ep+000001", "%E", 3.14159 );
    Terminate(0);
  }
  else {
    _Bg_Sleep_Forever();
  }

  return 0; // shut up the compiler
}


