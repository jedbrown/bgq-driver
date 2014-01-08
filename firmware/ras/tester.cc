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
#include <sstream>
#include <iostream>
#include <ras/include/RasEvent.h>
#include <hwi/include/bqc/bedram_dcr.h>
#include <assert.h>

using namespace std;

#include "utils.h"
#include "_bedram_interrupt_status.cc"

extern "C" {

  void shouldNotGetHere( ostringstream& str, uint64_t data ) {
    assert(0);
  }

  uint64_t expectedDecoderData = 0;
  
  void testDecoder( ostringstream& str, uint64_t data ) {
    assert( data == expectedDecoderData );
    str << "hello";
  }

  Decoder TEST1[] = {
    { 0x99999, shouldNotGetHere },
    { 0x00077, shouldNotGetHere },
    { 0xD470C, testDecoder },
    { 0x01077, shouldNotGetHere },
  };

  uint64_t TEST1_ARGS[] = {
    0xD470C, 0xBABEFACE,
  };

  int test1( void ) {
    ostringstream details;
    expectedDecoderData = TEST1_ARGS[1];
    walkDecoders( TEST1, sizeof(TEST1)/sizeof(TEST1[0]), TEST1_ARGS, sizeof(TEST1_ARGS)/sizeof(TEST1_ARGS[0]), details, "DETAILS" );
    assert( details.str() == "hello" );
    return 0;
  }


  void test2Decoder1( ostringstream& str, uint64_t data ) {
    assert( data == 1 );
    str << "1,";
  }

  void test2Decoder2( ostringstream& str, uint64_t data ) {
    assert( data == 2 );
    str << "2,";
  }

  Decoder TEST2[] = {
    { 0x11111, test2Decoder1 },
    { 0x99999, shouldNotGetHere },
    { 0x22222, test2Decoder2 },
    { 0x49999, shouldNotGetHere },
  };

  uint64_t TEST2_ARGS[] = {
    0x11111, 0x1,
    0x22222, 0x2,
  };

  int test2(void ) {
    ostringstream details;
    walkDecoders( TEST2, sizeof(TEST2)/sizeof(TEST2[0]), TEST2_ARGS, sizeof(TEST2_ARGS)/sizeof(TEST2_ARGS[0]), details, "DETAILS" );
    assert( details.str() == "1,2," );
    return 0;
  }


  Decoder TEST3[] = {
    { BEDRAM_DCR( BEDRAM_INTERRUPT_STATUS__MACHINE_CHECK ), decode_BEDRAM_INTERRUPT_STATUS },
    { BEDRAM_DCR( INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK ), decode_INTERNAL_ERROR },
  };

  uint64_t TEST3_ARGS[] = {
    BEDRAM_DCR( BEDRAM_INTERRUPT_STATUS__MACHINE_CHECK ),  0x7FFC000000000000ul,
    BEDRAM_DCR( INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK ), 0xF000000000000000ul,
  };

  int test3(void ) {
    ostringstream details;
    walkDecoders( TEST3, sizeof(TEST3)/sizeof(TEST3[0]), TEST3_ARGS, sizeof(TEST3_ARGS)/sizeof(TEST3_ARGS[0]), details, "DETAILS" );
    cout << details.str();
    return 0;
  }


int main(int argc, char** argv) {
  assert( test1() == 0 );
  assert( test2() == 0 );
  assert( test3() == 0 );
  return 0;
}

}

