/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
#include <stdio.h>
#include <string.h>
#include <hwi/include/common/uci.h>


#define ASSERT( condition, message ) \
  if ( ! (condition) ) { printf( "[ERROR] : Assertion failed at  %s:%s():%d\n", __FILE__, __func__, __LINE__); \
    printf message ; \
    return -1;	     \
  }


char digits[] = "0123456789ABCDEFGHIJKLMNOPQRTSUVWXYZ";

char rowchar(unsigned row) {
  return digits[row];
}

char colchar(unsigned col) {
  return digits[col];
}

char midplanechar(unsigned col) {
  return digits[col];
}

char digit(unsigned n) {
  return digits[n];
}
  
int test_compute_rack() {
  
  unsigned row = 6;
  unsigned col = 7;
  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeComputeRackUCI( row, col );
  

  unsigned drow, dcol;

  ASSERT( bg_decodeComputeRackUCI( uci, &drow, &dcol ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );

  sprintf( str, "R%c%c", rowchar(row), colchar(col) );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );

  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d\n", rc ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}

int test_io_rack() {
  
  unsigned row = 6;
  unsigned col = 7;
  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeIoRackUCI( row, col );
  

  unsigned drow, dcol;

  ASSERT( bg_decodeIoRackUCI( uci, &drow, &dcol ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );

  sprintf( str, "Q%c%c", rowchar(row), colchar(col) );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) \"%s\" vs. \"%s\" (actual)\n", str, dstr ) );
  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d\n", rc ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}


int test_midplane() {
  
  unsigned row = 31;
  unsigned col = 15;
  unsigned midplane = 1;

  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeMidplaneUCI( row, col, midplane );
  

  unsigned drow, dcol, dmid;

  ASSERT( bg_decodeMidplaneUCI( uci, &drow, &dcol, &dmid ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );
  ASSERT( midplane == dmid, ( "Error in midplane field: (expected) %d vs. %d (actual)\n", midplane, dmid ) );

  sprintf( str, "R%c%c-M%c", rowchar(row), colchar(col), midplanechar(midplane) );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );
  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d\n", rc ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}

int test_bulk_power_supply_on_compute_rack() {
  
  unsigned row = 5;
  unsigned col = 23;
  unsigned supply = 3;
  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeBulkPowerSupplyOnComputeRackUCI( row, col, supply );
  

  unsigned drow, dcol, dsupply;

  ASSERT( bg_decodeBulkPowerSupplyOnComputeRackUCI( uci, &drow, &dcol, &dsupply ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );
  ASSERT( supply == dsupply, ( "Error in supply field: (expected) %d vs. %d (actual)\n", supply, dsupply ) );

  sprintf( str, "R%c%c-B%c", rowchar(row), colchar(col), digit(supply) );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );
  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d\n", rc ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}

int test_bulk_power_supply_on_io_rack() {
  
  unsigned row = 5;
  unsigned col = 23;
  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeBulkPowerSupplyOnIoRackUCI( row, col );
  

  unsigned drow, dcol;

  ASSERT( bg_decodeBulkPowerSupplyOnIoRackUCI( uci, &drow, &dcol ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );

  sprintf( str, "Q%c%c-B", rowchar(row), colchar(col) );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );

  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d\n", rc ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}

int test_power_module_on_compute_rack() {
  
  unsigned row = 5;
  unsigned col = 23;
  unsigned powerSupply = 3;
  unsigned powerModule = 8;

  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodePowerModuleOnComputeRackUCI( row, col, powerSupply, powerModule );
  

  unsigned drow, dcol, dps, dpm;

  ASSERT( bg_decodePowerModuleOnComputeRackUCI( uci, &drow, &dcol, &dps, &dpm  ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );
  ASSERT( powerSupply == dps, ( "Error in power supply field: (expected) %d vs. %d (actual)\n", powerSupply, dps ) );
  ASSERT( powerModule == dpm, ( "Error in power module field: (expected) %d vs. %d (actual)\n", powerModule, dpm ) );

  sprintf( str, "R%c%c-B%c-P%c", rowchar(row), colchar(col), digit(powerSupply), digit(powerModule) );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );

  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d\n", rc ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}

int test_power_module_on_io_rack() {
  
  unsigned row = 5;
  unsigned col = 23;
  unsigned powerModule = 8;

  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodePowerModuleOnIoRackUCI( row, col, powerModule );
  

  unsigned drow, dcol, dpm;

  ASSERT( bg_decodePowerModuleOnIoRackUCI( uci, &drow, &dcol, &dpm  ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );
  ASSERT( powerModule == dpm, ( "Error in power module field: (expected) %d vs. %d (actual)\n", powerModule, dpm ) );

  sprintf( str, "Q%c%c-B-P%c", rowchar(row), colchar(col), digit(powerModule) );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );

  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d\n", rc ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}

int test_clock_card_on_compute_rack() {
  
  unsigned row = 6;
  unsigned col = 9;

  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeClockCardOnComputeRackUCI( row, col );
  

  unsigned drow, dcol;

  ASSERT( bg_decodeClockCardOnComputeRackUCI( uci, &drow, &dcol ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );

  sprintf( str, "R%c%c-K", rowchar(row), colchar(col) );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );

  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d\n", rc ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}

int test_clock_card_on_io_rack() {
  
  unsigned row = 6;
  unsigned col = 9;
  unsigned clockCard = 1;

  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeClockCardOnIoRackUCI( row, col, clockCard );
  

  unsigned drow, dcol, dcc;

  ASSERT( bg_decodeClockCardOnIoRackUCI( uci, &drow, &dcol, &dcc ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );
  ASSERT( clockCard == dcc, ( "Error in clock-card field: (expected) %d vs. %d (actual)\n", clockCard, dcc ) );

  sprintf( str, "Q%c%c-K%c", rowchar(row), colchar(col), digit(clockCard) );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );

  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d\n", rc ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}

int test_io_board_on_compute_rack() {
  
  unsigned row = 4;
  unsigned col = 13;
  unsigned ioBoard = 8;

  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeIoDrawerOnComputeRackUCI( row, col, ioBoard );
  

  unsigned drow, dcol, dio;

  ASSERT( bg_decodeIoDrawerOnComputeRackUCI( uci, &drow, &dcol, &dio ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );
  ASSERT( ioBoard == dio, ( "Error in IO Board field: (expected) %d vs. %d (actual)\n", ioBoard, dio ) );

  sprintf( str, "R%c%c-I%c", rowchar(row), colchar(col), digit(ioBoard) );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );

  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d\n", rc ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}

int test_io_board_on_io_rack() {
  
  unsigned row = 4;
  unsigned col = 13;
  unsigned ioBoard = 8;

  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeIoDrawerOnIoRackUCI( row, col, ioBoard );
  

  unsigned drow, dcol, dio;

  ASSERT( bg_decodeIoDrawerOnIoRackUCI( uci, &drow, &dcol, &dio ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );
  ASSERT( ioBoard == dio, ( "Error in IO Board field: (expected) %d vs. %d (actual)\n", ioBoard, dio ) );

  sprintf( str, "Q%c%c-I%c", rowchar(row), colchar(col), digit(ioBoard) );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );

  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d\n", rc ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}

int test_node_board() {
  
  unsigned row = 31;
  unsigned col = 15;
  unsigned midplane = 1;
  unsigned nodeBoard = 5;

  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeNodeBoardUCI( row, col, midplane, nodeBoard );
 

  unsigned drow, dcol, dmid, dnb;


  ASSERT( bg_decodeNodeBoardUCI( uci, &drow, &dcol, &dmid, &dnb ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );
  ASSERT( midplane == dmid, ( "Error in midplane field: (expected) %d vs. %d (actual)\n", midplane, dmid ) );
  ASSERT( nodeBoard == dnb, ( "Error in node board field: (expected) %d vs. %d (actual)\n", nodeBoard, dnb ) );

  sprintf( str, "R%c%c-M%c-N%02d", rowchar(row), colchar(col), midplanechar(midplane), nodeBoard );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );

  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d\n", rc ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}



int test_fan_assembly_on_compute_rack() {
  
  unsigned row = 4;
  unsigned col = 13;
  unsigned ioBoard = 8;
  unsigned assembly = 2;

  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeFanAssemblyOnComputeRackUCI( row, col, ioBoard, assembly );
  

  unsigned drow, dcol, dio, dassembly;

  ASSERT( bg_decodeFanAssemblyOnComputeRackUCI( uci, &drow, &dcol, &dio, &dassembly ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );
  ASSERT( ioBoard == dio, ( "Error in IO Board field: (expected) %d vs. %d (actual)\n", ioBoard, dio ) );
  ASSERT( assembly == dassembly, ( "Error in assembly field: (expected) %d vs. %d (actual)\n", assembly, dassembly ) );

  sprintf( str, "R%c%c-I%c-H%c", rowchar(row), colchar(col), digit(ioBoard), digit(assembly) );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );

  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d\n", rc ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}

int test_fan_assembly_on_io_rack() {
  
  unsigned row = 4;
  unsigned col = 13;
  unsigned ioBoard = 8;
  unsigned assembly = 2;

  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeFanAssemblyOnIoRackUCI( row, col, ioBoard, assembly );
  

  unsigned drow, dcol, dio, dassembly;

  ASSERT( bg_decodeFanAssemblyOnIoRackUCI( uci, &drow, &dcol, &dio, &dassembly ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );
  ASSERT( ioBoard == dio, ( "Error in IO Board field: (expected) %d vs. %d (actual)\n", ioBoard, dio ) );
  ASSERT( assembly == dassembly, ( "Error in assembly field: (expected) %d vs. %d (actual)\n", assembly, dassembly ) );

  sprintf( str, "Q%c%c-I%c-H%c", rowchar(row), colchar(col), digit(ioBoard), digit(assembly) );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );

  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d\n", rc ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}

int test_fan_on_compute_rack() {
  
  unsigned row = 4;
  unsigned col = 13;
  unsigned ioBoard = 8;
  unsigned assembly = 2;
  unsigned fan = 1;

  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeFanOnComputeRackUCI( row, col, ioBoard, assembly, fan );
  

  unsigned drow, dcol, dio, dassembly, dfan;

  ASSERT( bg_decodeFanOnComputeRackUCI( uci, &drow, &dcol, &dio, &dassembly, &dfan ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );
  ASSERT( ioBoard == dio, ( "Error in IO Board field: (expected) %d vs. %d (actual)\n", ioBoard, dio ) );
  ASSERT( assembly == dassembly, ( "Error in assembly field: (expected) %d vs. %d (actual)\n", assembly, dassembly ) );
  ASSERT( fan == dfan, ( "Error in fan field: (expected) %d vs. %d (actual)\n", fan, dfan ) );

  sprintf( str, "R%c%c-I%c-H%c-F%c", rowchar(row), colchar(col), digit(ioBoard), digit(assembly), digit(fan) );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );

  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d\n", rc ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}

int test_fan_on_io_rack() {
  
  unsigned row = 4;
  unsigned col = 13;
  unsigned ioBoard = 8;
  unsigned assembly = 2;
  unsigned fan = 1;

  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeFanOnIoRackUCI( row, col, ioBoard, assembly, fan );
  
  unsigned drow, dcol, dio, dassembly, dfan;

  ASSERT( bg_decodeFanOnIoRackUCI( uci, &drow, &dcol, &dio, &dassembly, &dfan ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );
  ASSERT( ioBoard == dio, ( "Error in IO Board field: (expected) %d vs. %d (actual)\n", ioBoard, dio ) );
  ASSERT( assembly == dassembly, ( "Error in assembly field: (expected) %d vs. %d (actual)\n", assembly, dassembly ) );
  ASSERT( fan == dfan, ( "Error in fan field: (expected) %d vs. %d (actual)\n", fan, dfan ) );

  sprintf( str, "Q%c%c-I%c-H%c-F%c", rowchar(row), colchar(col), digit(ioBoard), digit(assembly), digit(fan) );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );

  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d\n", rc ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}


int test_compute_card_on_node_board() {
  
  unsigned row = 31;
  unsigned col = 15;
  unsigned midplane = 1;
  unsigned nodeBoard = 5;
  unsigned computeCard = 23;

  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeComputeCardOnNodeBoardUCI( row, col, midplane, nodeBoard, computeCard );
  

  unsigned drow, dcol, dmid, dnb, dcc;

  ASSERT( bg_decodeComputeCardOnNodeBoardUCI( uci, &drow, &dcol, &dmid, &dnb, &dcc ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );
  ASSERT( midplane == dmid, ( "Error in midplane field: (expected) %d vs. %d (actual)\n", midplane, dmid ) );
  ASSERT( nodeBoard == dnb, ( "Error in node board field: (expected) %d vs. %d (actual)\n", nodeBoard, dnb ) );
  ASSERT( computeCard == dcc, ( "Error in compute card field: (expected) %d vs. %d (actual)\n", computeCard, dcc ) );


  sprintf( str, "R%c%c-M%c-N%02d-J%02d", rowchar(row), colchar(col), midplanechar(midplane), nodeBoard, computeCard );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );

  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d str=%s\n", rc, str ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}

int test_compute_card_on_io_board_on_io_rack() {
  
  unsigned row = 4;
  unsigned col = 13;
  unsigned ioBoard = 8;
  unsigned computeCard = 19;

  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeComputeCardOnIoBoardOnIoRackUCI( row, col, ioBoard, computeCard );
  

  unsigned drow, dcol, dio, dcc;

  ASSERT( bg_decodeComputeCardOnIoBoardOnIoRackUCI( uci, &drow, &dcol, &dio, &dcc ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );
  ASSERT( ioBoard == dio, ( "Error in IO Board field: (expected) %d vs. %d (actual)\n", ioBoard, dio ) );
  ASSERT( computeCard == dcc, ( "Error in compute card field: (expected) %d vs. %d (actual)\n", computeCard, dcc ) );

  sprintf( str, "Q%c%c-I%c-J%02d", rowchar(row), colchar(col), digit(ioBoard), computeCard );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );

  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d str=%s\n", rc, str ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}

int test_compute_card_on_io_board_on_compute_rack() {
  
  unsigned row = 4;
  unsigned col = 13;
  unsigned ioBoard = 0xF;
  unsigned computeCard = 7;

  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeComputeCardOnIoBoardOnComputeRackUCI( row, col, ioBoard, computeCard );
  

  unsigned drow, dcol, dio, dcc;

  ASSERT( bg_decodeComputeCardOnIoBoardOnComputeRackUCI( uci, &drow, &dcol, &dio, &dcc ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );
  ASSERT( ioBoard == dio, ( "Error in IO Board field: (expected) %d vs. %d (actual)\n", ioBoard, dio ) );
  ASSERT( computeCard == dcc, ( "Error in compute card field: (expected) %d vs. %d (actual)\n", computeCard, dcc ) );

  sprintf( str, "R%c%c-I%c-J%02d", rowchar(row), colchar(col), digit(ioBoard), computeCard );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );

  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d str=%s\n", rc, str ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}

int test_compute_card_core_on_node_board() {
  
  unsigned row = 31;
  unsigned col = 15;
  unsigned midplane = 1;
  unsigned nodeBoard = 15;
  unsigned computeCard = 23;
  unsigned core = 7;

  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeComputeCardCoreOnNodeBoardUCI( row, col, midplane, nodeBoard, computeCard, core );
  

  unsigned drow, dcol, dmid, dnb, dcc, dcore;

  ASSERT( bg_decodeComputeCardCoreOnNodeBoardUCI( uci, &drow, &dcol, &dmid, &dnb, &dcc, &dcore ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );
  ASSERT( midplane == dmid, ( "Error in midplane field: (expected) %d vs. %d (actual)\n", midplane, dmid ) );
  ASSERT( nodeBoard == dnb, ( "Error in node board field: (expected) %d vs. %d (actual)\n", nodeBoard, dnb ) );
  ASSERT( computeCard == dcc, ( "Error in compute card field: (expected) %d vs. %d (actual)\n", computeCard, dcc ) );
  ASSERT( core == dcore, ( "Error in core field: (expected) %d vs. %d (actual)\n", core, dcore ) );

  sprintf( str, "R%c%c-M%c-N%02d-J%02d-C%02d", rowchar(row), colchar(col), midplanechar(midplane), nodeBoard, computeCard, core );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );
  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d str=%s\n", rc, str ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}

int test_compute_card_core_on_io_board() {
  
  unsigned row = 4;
  unsigned col = 13;
  unsigned ioBoard = 8;
  unsigned computeCard = 19;
  unsigned core = 15;

  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeComputeCardCoreOnIoBoardUCI( row, col, ioBoard, computeCard, core );
  

  unsigned drow, dcol, dio, dcc, dcore;

  ASSERT( bg_decodeComputeCardCoreOnIoBoardUCI( uci, &drow, &dcol, &dio, &dcc, &dcore ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );
  ASSERT( ioBoard == dio, ( "Error in IO Board field: (expected) %d vs. %d (actual)\n", ioBoard, dio ) );
  ASSERT( computeCard == dcc, ( "Error in compute card field: (expected) %d vs. %d (actual)\n", computeCard, dcc ) );
  ASSERT( core == dcore, ( "Error in core field: (expected) %d vs. %d (actual)\n", core, dcore ) );

  sprintf( str, "Q%c%c-I%c-J%02d-C%02d", rowchar(row), colchar(col), digit(ioBoard), computeCard, core );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );
  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d\n", rc ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}

int test_service_card() {
  
  unsigned row = 14;
  unsigned col = 5;
  unsigned midplane = 0;

  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeServiceCardUCI( row, col, midplane );
  

  unsigned drow, dcol, dmid;

  ASSERT( bg_decodeServiceCardUCI( uci, &drow, &dcol, &dmid ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );
  ASSERT( midplane == dmid, ( "Error in midplane field: (expected) %d vs. %d (actual)\n", midplane, dmid ) );

  sprintf( str, "R%c%c-M%c-S", rowchar(row), colchar(col), midplanechar(midplane) );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );
  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d\n", rc ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}



int test_link_module_on_node_board() {
  
  unsigned row = 31;
  unsigned col = 15;
  unsigned midplane = 1;
  unsigned nodeBoard = 5;
  unsigned linkModule = 6;

  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeLinkModuleOnNodeBoardUCI( row, col, midplane, nodeBoard, linkModule );
  

  unsigned drow, dcol, dmid, dnb, dlm;

  ASSERT( bg_decodeLinkModuleOnNodeBoardUCI( uci, &drow, &dcol, &dmid, &dnb, &dlm ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );
  ASSERT( midplane == dmid, ( "Error in midplane field: (expected) %d vs. %d (actual)\n", midplane, dmid ) );
  ASSERT( nodeBoard == dnb, ( "Error in node board field: (expected) %d vs. %d (actual)\n", nodeBoard, dnb ) );
  ASSERT( linkModule == dlm, ( "Error in link module field: (expected) %d vs. %d (actual)\n", linkModule, dlm ) );

  sprintf( str, "R%c%c-M%c-N%02d-U%02d", rowchar(row), colchar(col), midplanechar(midplane), nodeBoard, linkModule );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );
  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d\n", rc ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}

int test_link_module_on_io_board() {
  
  unsigned row = 4;
  unsigned col = 13;
  unsigned ioBoard = 8;
  unsigned linkModule = 2;

  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeLinkModuleOnIoBoardUCI( row, col, ioBoard, linkModule );
  

  unsigned drow, dcol, dio, dlm;

  ASSERT( bg_decodeLinkModuleOnIoBoardUCI( uci, &drow, &dcol, &dio, &dlm ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );
  ASSERT( ioBoard == dio, ( "Error in IO Board field: (expected) %d vs. %d (actual)\n", ioBoard, dio ) );
  ASSERT( linkModule == dlm, ( "Error in link module  field: (expected) %d vs. %d (actual)\n", linkModule, dlm ) );

  sprintf( str, "Q%c%c-I%c-U%02d", rowchar(row), colchar(col), digit(ioBoard), linkModule );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );

  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d\n", rc ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}




int test_optical_module_on_node_board() {
  
  unsigned row = 31;
  unsigned col = 15;
  unsigned midplane = 1;
  unsigned nodeBoard = 5;
  unsigned opticalModule = 6;

  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeOpticalModuleOnNodeBoardUCI( row, col, midplane, nodeBoard, opticalModule );
  

  unsigned drow, dcol, dmid, dnb, dom;

  ASSERT( bg_decodeOpticalModuleOnNodeBoardUCI( uci, &drow, &dcol, &dmid, &dnb, &dom ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );
  ASSERT( midplane == dmid, ( "Error in midplane field: (expected) %d vs. %d (actual)\n", midplane, dmid ) );
  ASSERT( nodeBoard == dnb, ( "Error in node board field: (expected) %d vs. %d (actual)\n", nodeBoard, dnb ) );
  ASSERT( opticalModule == dom, ( "Error in link module field: (expected) %d vs. %d (actual)\n", opticalModule, dom ) );

  sprintf( str, "R%c%c-M%c-N%02d-O%02d", rowchar(row), colchar(col), midplanechar(midplane), nodeBoard, opticalModule );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );
  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d str=%s\n", rc, str ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}

int test_optical_module_on_io_board() {
  
  unsigned row = 4;
  unsigned col = 13;
  unsigned ioBoard = 8;
  unsigned opticalModule = 2;

  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeOpticalModuleOnIoBoardUCI( row, col, ioBoard, opticalModule );
  

  unsigned drow, dcol, dio, dom;

  ASSERT( bg_decodeOpticalModuleOnIoBoardUCI( uci, &drow, &dcol, &dio, &dom ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );
  ASSERT( ioBoard == dio, ( "Error in IO Board field: (expected) %d vs. %d (actual)\n", ioBoard, dio ) );
  ASSERT( opticalModule == dom, ( "Error in link module  field: (expected) %d vs. %d (actual)\n", opticalModule, dom ) );

  sprintf( str, "Q%c%c-I%c-O%02d", rowchar(row), colchar(col), digit(ioBoard), opticalModule );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );

  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d\n", rc ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}

int test_dca_on_node_board() {
  
  unsigned row = 13;
  unsigned col = 1;
  unsigned midplane = 1;
  unsigned nodeBoard = 2;
  unsigned dca = 1;

  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeDcaOnNodeBoardUCI( row, col, midplane, nodeBoard, dca );
  

  unsigned drow, dcol, dmid, dnb, ddca;

  ASSERT( bg_decodeDcaOnNodeBoardUCI( uci, &drow, &dcol, &dmid, &dnb, &ddca ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );
  ASSERT( midplane == dmid, ( "Error in midplane field: (expected) %d vs. %d (actual)\n", midplane, dmid ) );
  ASSERT( nodeBoard == dnb, ( "Error in node board field: (expected) %d vs. %d (actual)\n", nodeBoard, dnb ) );
  ASSERT( dca == ddca, ( "Error in dca field: (expected) %d vs. %d (actual)\n", dca, ddca ) );

  sprintf( str, "R%c%c-M%c-N%02d-D%d", rowchar(row), colchar(col), midplanechar(midplane), nodeBoard, dca );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );

  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d\n", rc ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}

int test_dca_on_io_board() {
  
  unsigned row = 13;
  unsigned col = 1;
  unsigned ioDrawer = 2;
  unsigned dca = 0;

  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeDcaOnIoBoardUCI( row, col, ioDrawer, dca );
  

  unsigned drow, dcol, diod, ddca;

  ASSERT( bg_decodeDcaOnIoBoardUCI( uci, &drow, &dcol, &diod, &ddca ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );
  ASSERT( ioDrawer == diod, ( "Error in IO drawer field: (expected) %d vs. %d (actual)\n", ioDrawer, diod) );
  ASSERT( dca == ddca, ( "Error in dca field: (expected) %d vs. %d (actual)\n", dca, ddca ) );

  sprintf( str, "Q%c%c-I%c-D%c", rowchar(row), colchar(col), digit(ioDrawer), digit(dca) );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );

  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d\n", rc ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}

int test_pci_adapter() {
  
  unsigned row = 3;
  unsigned col = 12;
  unsigned ioBoard = 7;
  unsigned pci = 1;

  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodePciAdapterUCI( row, col, ioBoard, pci );
  
  unsigned drow, dcol, dio, dpci;

  ASSERT( bg_decodePciAdapterUCI( uci, &drow, &dcol, &dio, &dpci ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );
  ASSERT( ioBoard == dio, ( "Error in IO Board field: (expected) %d vs. %d (actual)\n", ioBoard, dio ) );
  ASSERT( pci == dpci, ( "Error in link module  field: (expected) %d vs. %d (actual)\n", pci, dpci ) );

  sprintf( str, "Q%c%c-I%c-A%c", rowchar(row), colchar(col), digit(ioBoard), digit(pci) );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );
  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );

  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d\n", rc ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}

int test_leak_detector_on_compute_rack() {
  
  unsigned row = 6;
  unsigned col = 7;
  char str[256], dstr[256];

  BG_UniversalComponentIdentifier uci = bg_encodeLeakDetectorOnComputeRackUCI( row, col );
  

  unsigned drow, dcol;

  ASSERT( bg_decodeLeakDetectorOnComputeRackUCI( uci, &drow, &dcol ) == 0, ( "Unexpected return code\n") );
  ASSERT( row == drow, ( "Error in row field: (expected) %d vs. %d (actual)\n", row, drow ) );
  ASSERT( col == dcol, ( "Error in column field: (expected) %d vs. %d (actual)\n", col, dcol ) );

  sprintf( str, "R%c%c-L", rowchar(row), colchar(col) );
  
  ASSERT( bg_uci_toString( uci, dstr ) == 0, ("Error in bg_uci_toString\n") );

  ASSERT( strcmp( str, dstr ) == 0, ( "String error: (expected) %s vs. %s (actual)\n", str, dstr ) );

  BG_UniversalComponentIdentifier uci2;

  int rc = bg_string_to_uci(str, &uci2 );
  ASSERT( rc == 0, ( "non-zero rc from str_to_uci %d\n", rc ) );
  ASSERT( uci == uci2, ( "string_to_uci error : %s %016lX vs %016lX\n", str, uci, uci2 ) );
  return 0;
}


typedef int (*Test_t)( void );

Test_t TESTS[] = {
  test_compute_rack,
  test_io_rack,
  test_midplane,
  test_bulk_power_supply_on_compute_rack,
  test_bulk_power_supply_on_io_rack,
  test_power_module_on_compute_rack,
  test_power_module_on_io_rack,
  test_clock_card_on_compute_rack,
  test_clock_card_on_io_rack,
  test_io_board_on_compute_rack,
  test_io_board_on_io_rack,
  test_node_board,
  test_fan_assembly_on_compute_rack,
  test_fan_assembly_on_io_rack,
  test_fan_on_compute_rack,
  test_fan_on_io_rack,
  test_compute_card_on_node_board,
  test_compute_card_on_io_board_on_io_rack,
  test_compute_card_on_io_board_on_compute_rack,
  test_compute_card_core_on_node_board,
  test_compute_card_core_on_io_board,
  test_service_card,
  test_link_module_on_node_board,
  test_link_module_on_io_board,
  test_optical_module_on_node_board,
  test_optical_module_on_io_board,
  test_dca_on_node_board,
  test_dca_on_io_board,
  test_pci_adapter,
  test_leak_detector_on_compute_rack,
};


int main(int argc, char** argv) {

  int rc = 0;
  unsigned i;
  for ( i = 0; i < sizeof(TESTS)/sizeof(TESTS[0]); i++ ) {
    rc |= TESTS[i]();
    printf("."); fflush(stdout);
  }
  printf("\n%d tests complete\n", i);
  return rc;
}
