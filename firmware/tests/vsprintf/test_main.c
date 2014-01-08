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

// get the goodies
#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/bqc/A2_inlines.h>

#define SIMPLE_TEST      "hello"
#define SIMPLE_TEST_EXP  "hello"
#define S_TEST           "%%s:%s\n"
#define S_TEST_ARG       "hello"
#define S_TEST_EXP       "%s:hello\n"
#define C_TEST           "%%c:%c\n"
#define C_TEST_ARG       '@'
#define C_TEST_EXP       "%c:@\n"
#define U_TEST           "%%u:%u"
#define U_TEST_ARG       1234567ul
#define U_TEST_EXP       "%u:1234567"
#define D_TEST           "%%d:%d"
#define D_TEST_ARG       -1234567l
#define D_TEST_EXP       "%d:-1234567"
#define X_TEST           "%%x:%x\n"
#define X_TEST_ARG       0x123456789abcdefUL
#define X_TEST_EXP       "%x:0123456789ABCDEF\n"
#define P_TEST           "%%p:%p"
#define P_TEST_ARG       (void*)0x1122334455667788UL
#define P_TEST_EXP       "%p:1122334455667788"
#define LX_TEST           "%%lx:%lx\n"
#define LX_TEST_ARG       0x123456789abcdefUL
#define LX_TEST_EXP       "%lx:0123456789ABCDEF\n"
#define LLX_TEST           "%%llx:%llx\n"
#define LLX_TEST_ARG       0x123456789abcdefUL
#define LLX_TEST_EXP       "%llx:0123456789ABCDEF\n"

extern void crash(int);
extern int fw_vsprintf( char *buf, const char* fmt, va_list args );

char buffer[128];

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

  va_list args;
  va_start( args, fmt );
  int actual = fw_vsprintf( buffer, fmt, args );
  va_end( args );

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
    crash(code);
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

    crash(code);
  } 
}
  
int main( int argc, char *argv[], char **envp ) {

  if (ProcessorID() ==0) {

    runTest( __LINE__, SIMPLE_TEST_EXP, SIMPLE_TEST );

    runTest( __LINE__, S_TEST_EXP, S_TEST, S_TEST_ARG );
    runTest( __LINE__, C_TEST_EXP, C_TEST, C_TEST_ARG );
    runTest( __LINE__, U_TEST_EXP, U_TEST, U_TEST_ARG );
    runTest( __LINE__, D_TEST_EXP, D_TEST, D_TEST_ARG );
    runTest( __LINE__, X_TEST_EXP, X_TEST, X_TEST_ARG );
    runTest( __LINE__, P_TEST_EXP, P_TEST, P_TEST_ARG );
    runTest( __LINE__, LX_TEST_EXP,  LX_TEST,  LX_TEST_ARG );
    runTest( __LINE__, LLX_TEST_EXP, LLX_TEST, LLX_TEST_ARG );

    crash(0);
  }
  else {
    
    // Go to sleep on all other threads:

    uint64_t mask = (1<<(ProcessorThreadID()));
    mtspr(SPRN_TENC,((mask) & 0xf));
  }

  return 0; // shut up the compiler
}


