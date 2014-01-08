/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (c) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/*                                                                  */
/* US Government Users Restricted Rights - Use, duplication or      */
/* disclosure restricted by GSA ADP Schedule Contract with IBM      */
/* Corporation.                                                     */
/*                                                                  */
/* This software is available to you under either the GNU General   */
/* Public License (GPL) version 2 or the Eclipse Public License     */
/* (EPL) at your discretion.                                        */
/*                                                                  */

#ifndef _UCI_H_  // Prevent multiple inclusion.
#define _UCI_H_

#include <hwi/include/common/compiler_support.h>
#include <hwi/include/common/bgq_bitnumbers.h>

// A UCI (Universal Component Identifier) is a 64-bit encoded number
// that identifies parts within a Blue Gene system.

typedef uint64_t BG_UniversalComponentIdentifier;

#define BG_UCI_Component_ComputeRack                       ( 1)
#define BG_UCI_Component_IoRack                            ( 2)
#define BG_UCI_Component_Midplane                          ( 3)
#define BG_UCI_Component_BulkPowerSupplyOnComputeRack      ( 4)
#define BG_UCI_Component_PowerModuleOnComputeRack          ( 5)
#define BG_UCI_Component_PowerModuleOnIoRack               ( 6)
#define BG_UCI_Component_ClockCardOnComputeRack            ( 7)
#define BG_UCI_Component_ClockCardOnIoRack                 ( 8)
#define BG_UCI_Component_IoDrawerOnComputeRack             ( 9)
#define BG_UCI_Component_IoDrawerOnIoRack                  (10)
#define BG_UCI_Component_NodeBoard                         (11)
#define BG_UCI_Component_FanAssemblyOnIoRack               (12)
#define BG_UCI_Component_ComputeCardOnNodeBoard            (13)
#define BG_UCI_Component_ComputeCardOnIoBoardOnIoRack              (14)
#define BG_UCI_Component_ComputeCardCoreOnNodeBoard        (15)
#define BG_UCI_Component_ComputeCardCoreOnIoBoard          (16)
#define BG_UCI_Component_ServiceCard                       (17)
#define BG_UCI_Component_LinkModuleOnNodeBoard             (18)
#define BG_UCI_Component_LinkModuleOnIoBoard               (19)
#define BG_UCI_Component_OpticalModuleOnNodeBoard          (20)
#define BG_UCI_Component_OpticalModuleOnIoBoard            (21)
#define BG_UCI_Component_DcaOnNodeBoard                    (22)
#define BG_UCI_Component_PciAdapterCard                    (23)
#define BG_UCI_Component_BulkPowerSupplyOnIoRack           (24)
#define BG_UCI_Component_DcaOnIoBoard                      (25)
#define BG_UCI_Component_FanAssemblyOnComputeRack          (26)
#define BG_UCI_Component_FanOnComputeRack                  (27)
#define BG_UCI_Component_FanOnIoRack                       (28)
#define BG_UCI_Component_LeakDetectorOnComputeRack         (29)
#define BG_UCI_Component_ComputeCardOnIoBoardOnComputeRack (30)

#define BG_UCI_Pattern_ComputeRack                         "R**"
#define BG_UCI_Pattern_IoRack                              "Q**"
#define BG_UCI_Pattern_Midplane                            "R**-M*"
#define BG_UCI_Pattern_BulkPowerSupplyOnComputeRack        "R**-B*"
#define BG_UCI_Pattern_PowerModuleOnComputeRack            "R**-B*-P*"
#define BG_UCI_Pattern_PowerModuleOnIoRack                 "Q**-B-P*"
#define BG_UCI_Pattern_ClockCardOnComputeRack              "R**-K"
#define BG_UCI_Pattern_ClockCardOnIoRack                   "Q**-K*"
#define BG_UCI_Pattern_IoDrawerOnComputeRack               "R**-I*" 
#define BG_UCI_Pattern_IoDrawerOnIoRack                    "Q**-I*" 
#define BG_UCI_Pattern_NodeBoard                           "R**-M*-N**"
#define BG_UCI_Pattern_FanAssemblyOnIoRack                 "Q**-I*-H*"
#define BG_UCI_Pattern_ComputeCardOnNodeBoard              "R**-M*-N**-J**"
#define BG_UCI_Pattern_ComputeCardOnIoBoardOnIoRack                "Q**-I*-J**"
#define BG_UCI_Pattern_ComputeCardCoreOnNodeBoard          "R**-M*-N**-J**-C**"
#define BG_UCI_Pattern_ComputeCardCoreOnIoBoard            "Q**-I*-J**-C**"
#define BG_UCI_Pattern_ServiceCard                         "R**-M*-S"
#define BG_UCI_Pattern_LinkModuleOnNodeBoard               "R**-M*-N**-U**"
#define BG_UCI_Pattern_LinkModuleOnIoBoard                 "Q**-I*-U**"
#define BG_UCI_Pattern_OpticalModuleOnNodeBoard            "R**-M*-N**-O**"
#define BG_UCI_Pattern_OpticalModuleOnIoBoard              "Q**-I*-O**"
#define BG_UCI_Pattern_DcaOnNodeBoard                      "R**-M*-N**-D*"
#define BG_UCI_Pattern_PciAdapterCard                      "Q**-I*-A*"
#define BG_UCI_Pattern_BulkPowerSupplyOnIoRack             "Q**-B"
#define BG_UCI_Pattern_DcaOnIoBoard                        "Q**-I*-D*"
#define BG_UCI_Pattern_FanAssemblyOnComputeRack            "R**-I*-H*"
#define BG_UCI_Pattern_FanOnComputeRack                    "R**-I*-H*-F*"
#define BG_UCI_Pattern_FanOnIoRack                         "Q**-I*-H*-F*"
#define BG_UCI_Pattern_LeakDetectorOnComputeRack           "R**-L"
#define BG_UCI_Pattern_ComputeCardOnIoBoardOnComputeRack   "R**-I*-J**"


// Macros to set and get specific fields within the UCI

#define BG_UCI_SET_COMPONENT(x)          _B6(  5,  x )  
#define BG_UCI_SET_ROW(x)                _B5( 10,  x )  // 0..V
#define BG_UCI_SET_COLUMN(x)             _B5( 15,  x )  // 0..V
#define BG_UCI_SET_MIDPLANE(x)           _B1( 16,  x )  // 0=Bottom, 1=Top
#define BG_UCI_SET_BULK_POWER_SUPPLY(x)  _B2( 17,  x )  // 0..3
#define BG_UCI_SET_POWER_MODULE(x)       _B4( 21,  x )  // 0..8 (0 = Bottom, 8 = Top)
#define BG_UCI_SET_CLOCK_CARD(x)         _B1( 16,  x )  // 0..1 (0 = Bottom, 1 = Top), I/O racks only
#define BG_UCI_SET_IO_DRAWER(x)          _B4( 19,  x )  // 0..B (I/O rack), C..F (compute rack)
#define BG_UCI_SET_NODE_BOARD(x)         _B4( 20,  x )  // 0..F (I/O rack), C..F (compute rack
#define BG_UCI_SET_FAN_ASSEMBLY(x)       _B2( 21,  x )  // 0..2
#define BG_UCI_SET_FAN(x)                _B2( 23,  x )  // 0..3
#define BG_UCI_SET_LINK_MODULE(x)        _B4( 24,  x )  // 0..8 (node board), 0..5 (I/O board)
#define BG_UCI_SET_OPTICAL_MODULE(x)     _B6( 26,  x )  // 0..35
#define BG_UCI_SET_COMPUTE_CARD(x)       _B5( 25,  x )
#define BG_UCI_SET_CORE(x)               _B5( 30,  x )
#define BG_UCI_SET_PCI_ADAPTER(x)        _B1( 20,  x )
#define BG_UCI_SET_DCA(x)                _B1( 21,  x )

#define BG_UCI_GET_COMPONENT(x)          _G6(  x,  5 )
#define BG_UCI_GET_ROW(x)                _G5(  x, 10 )
#define BG_UCI_GET_COLUMN(x)             _G5(  x, 15 )
#define BG_UCI_GET_MIDPLANE(x)           _G1(  x, 16 )
#define BG_UCI_GET_BULK_POWER_SUPPLY(x)  _G2(  x, 17 )
#define BG_UCI_GET_POWER_MODULE(x)       _G4(  x, 21 )
#define BG_UCI_GET_CLOCK_CARD(x)         _G1(  x, 16 )
#define BG_UCI_GET_IO_DRAWER(x)          _G4(  x, 19 )
#define BG_UCI_GET_NODE_BOARD(x)         _G4(  x, 20 )
#define BG_UCI_GET_FAN_ASSEMBLY(x)       _G2(  x, 21 )
#define BG_UCI_GET_FAN(x)                _G2(  x, 23 )
#define BG_UCI_GET_LINK_MODULE(x)        _G4(  x, 24 )
#define BG_UCI_GET_COMPUTE_CARD(x)       _G5(  x, 25 )
#define BG_UCI_GET_CORE(x)               _G5(  x, 30 )
#define BG_UCI_GET_OPTICAL_MODULE(x)     _G6(  x, 26 )
#define BG_UCI_GET_PCI_ADAPTER(x)        _G1(  x, 20 )
#define BG_UCI_GET_DCA(x)                _G1(  x, 21 )


#define _BG_UCI_ASSERT( condition )


inline unsigned bg_decodeComponent( BG_UniversalComponentIdentifier uci ) {
  return BG_UCI_GET_COMPONENT(uci);
}

// --------------------------------------------------------------------------------
//
//  Rack: Rxy" or "Qxy", x=row, y=column,
//
//  +-----------+-----+--------+---------+
//  | Component | Row | Column |   ///   |
//  +-----------+-----+--------+---------+
//  0            6     11       16
//
// --------------------------------------------------------------------------------

inline BG_UniversalComponentIdentifier bg_encodeComputeRackUCI( unsigned row, unsigned column ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_ComputeRack );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  return uci;
}

inline int bg_decodeComputeRackUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_ComputeRack ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  return 0;
}

inline BG_UniversalComponentIdentifier bg_encodeIoRackUCI( unsigned row, unsigned column ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_IoRack );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  return uci;
}

inline int bg_decodeIoRackUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_IoRack ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  return 0;
}


// --------------------------------------------------------------------------------
//
//  Midplane: Rxy-Mm" x=row, y=column, m = midplane
//
//  +-----------+-----+--------+----------+--------+
//  | Component | Row | Column | Midplane |  ///   |
//  +-----------+-----+--------+----------+--------+
//  0            6     11       16         17
//
// --------------------------------------------------------------------------------

inline BG_UniversalComponentIdentifier bg_encodeMidplaneUCI( unsigned row, unsigned column, unsigned midplane ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_Midplane );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  uci |= BG_UCI_SET_MIDPLANE( midplane );
  return uci;
}

inline int bg_decodeMidplaneUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column, unsigned* midplane ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_IoRack ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  *midplane = BG_UCI_GET_MIDPLANE(uci);
  return 0;
}

// --------------------------------------------------------------------------------
//
//  Bulk Power Supply: Rxy-Bb" x=row, y=column, b=supply (0..3)
//                     Qxy-B
//
//  +-----------+-----+--------+--------+-------+
//  | Component | Row | Column | Supply |  ///  |
//  +-----------+-----+--------+--------+-------+
//  0            6     11       16       18
//
// --------------------------------------------------------------------------------

inline BG_UniversalComponentIdentifier bg_encodeBulkPowerSupplyOnComputeRackUCI( unsigned row, unsigned column, unsigned supply ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_BulkPowerSupplyOnComputeRack );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  uci |= BG_UCI_SET_BULK_POWER_SUPPLY(supply);
  return uci;
}

inline int bg_decodeBulkPowerSupplyOnComputeRackUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column, unsigned* supply ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_IoRack ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  *supply = BG_UCI_GET_BULK_POWER_SUPPLY(uci);
  return 0;
}

inline BG_UniversalComponentIdentifier bg_encodeBulkPowerSupplyOnIoRackUCI( unsigned row, unsigned column ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_BulkPowerSupplyOnIoRack );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  return uci;
}

inline int bg_decodeBulkPowerSupplyOnIoRackUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_IoRack ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  return 0;
}


// --------------------------------------------------------------------------------
//
//  Power Module : Rxy-Bb-Pp" or "Qxy=B-Pp" x=row, y=column, b=bulk-power-supply, 
//                 p=power-module
//
//  +-----------+-----+--------+-------------------+------------+---------+
//  | Component | Row | Column | Bulk Pwr Supp (*) | Pwr Module |   ///   |
//  +-----------+-----+--------+-------------------+------------+---------+
//  0            6     11       16                  18           22
//
//  (*) Bulk Power Supply field is valid only for compute racks
//
// --------------------------------------------------------------------------------



inline BG_UniversalComponentIdentifier bg_encodePowerModuleOnComputeRackUCI( unsigned row, unsigned column, unsigned bulkPowerSupply, unsigned powerModule ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_PowerModuleOnComputeRack );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  uci |= BG_UCI_SET_BULK_POWER_SUPPLY( bulkPowerSupply );
  uci |= BG_UCI_SET_POWER_MODULE( powerModule );
  return uci;
}

inline int bg_decodePowerModuleOnComputeRackUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column, unsigned* bulkPowerSupply, unsigned* powerModule ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_PowerModuleOnComputeRack ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  *bulkPowerSupply = BG_UCI_GET_BULK_POWER_SUPPLY(uci);
  *powerModule = BG_UCI_GET_POWER_MODULE(uci);
  return 0;
}

inline BG_UniversalComponentIdentifier bg_encodePowerModuleOnIoRackUCI( unsigned row, unsigned column, unsigned powerModule ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_PowerModuleOnIoRack );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  uci |= BG_UCI_SET_POWER_MODULE( powerModule );
  return uci;
}

inline int bg_decodePowerModuleOnIoRackUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column, unsigned* powerModule ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_PowerModuleOnIoRack ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  *powerModule = BG_UCI_GET_POWER_MODULE(uci);
  return 0;
}


// --------------------------------------------------------------------------------
//
//  Clock Card: Rxy-K" or "Qxy-Kk" x=row, y=column, k=clock-card
//
//  +-----------+-----+--------+----------------+--------+
//  | Component | Row | Column | Clock Card (*) |  ///   |
//  +-----------+-----+--------+----------------+--------+
//  0            6     11       16               17
//
//    (*) Clock Card field is valid on I/O racks only
//
// --------------------------------------------------------------------------------

inline BG_UniversalComponentIdentifier bg_encodeClockCardOnComputeRackUCI( unsigned row, unsigned column ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_ClockCardOnComputeRack );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  return uci;
}

inline int bg_decodeClockCardOnComputeRackUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_ClockCardOnComputeRack ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  return 0;
}

inline BG_UniversalComponentIdentifier bg_encodeClockCardOnIoRackUCI( unsigned row, unsigned column, unsigned clockCard ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_ClockCardOnIoRack );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  uci |= BG_UCI_SET_CLOCK_CARD( clockCard );
  return uci;
}

inline int bg_decodeClockCardOnIoRackUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column, unsigned* clockCard ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_ClockCardOnIoRack ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  *clockCard = BG_UCI_GET_CLOCK_CARD(uci);
  return 0;
}

// --------------------------------------------------------------------------------
//
//  IO Board: Rxy-Ii" or "Qxy-Ii" x=row, y=column, k=clock-card, i=io-board
//
//  +-----------+-----+--------+----------+--------+
//  | Component | Row | Column | IO Board |  ///   |
//  +-----------+-----+--------+----------+--------+
//  0            6     11       16         20
//
// --------------------------------------------------------------------------------

inline BG_UniversalComponentIdentifier bg_encodeIoDrawerOnComputeRackUCI( unsigned row, unsigned column, unsigned ioDrawer ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_IoDrawerOnComputeRack );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  uci |= BG_UCI_SET_IO_DRAWER( ioDrawer );
  return uci;
}

inline int bg_decodeIoDrawerOnComputeRackUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column, unsigned* ioDrawer ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_IoDrawerOnComputeRack ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  *ioDrawer = BG_UCI_GET_IO_DRAWER(uci);
  return 0;
}

inline BG_UniversalComponentIdentifier bg_encodeIoDrawerOnIoRackUCI( unsigned row, unsigned column, unsigned ioDrawer ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_IoDrawerOnIoRack );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  uci |= BG_UCI_SET_IO_DRAWER( ioDrawer );
  return uci;
}

inline int bg_decodeIoDrawerOnIoRackUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column, unsigned* ioDrawer ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_IoDrawerOnIoRack ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  *ioDrawer = BG_UCI_GET_IO_DRAWER(uci);
  return 0;
}

// --------------------------------------------------------------------------------
//
//  Node Board : "Rxy-Mm-Nn" x=row, y=column, m = midplane, n=node-board
//
//  +-----------+-----+--------+----------+------------+-------+
//  | Component | Row | Column | Midplane | Node Board | ///   |
//  +-----------+-----+--------+----------+------------+-------+
//  0            6     11       16         17           21
//
// --------------------------------------------------------------------------------

inline BG_UniversalComponentIdentifier bg_encodeNodeBoardUCI( unsigned row, unsigned column, unsigned midplane, unsigned nodeBoard ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_NodeBoard );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  uci |= BG_UCI_SET_MIDPLANE( midplane );
  uci |= BG_UCI_SET_NODE_BOARD( nodeBoard );
  return uci;
}

inline int bg_decodeNodeBoardUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column, unsigned* midplane, unsigned* nodeBoard ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_NodeBoard ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  *midplane = BG_UCI_GET_MIDPLANE(uci);
  *nodeBoard = BG_UCI_GET_NODE_BOARD(uci);
  return 0;
}

// --------------------------------------------------------------------------------
//
//  Fan Assenbly: "Qxy-Ii-Hh" x=row, y=column, i=io-drawer, h=fan-assembly
//                "Rxy-Ii-Hh"

//  +-----------+-----+--------+----------+-----+---------+
//  | Component | Row | Column | IO Board | Ass |   ///   |
//  +-----------+-----+--------+----------+-----+---------+
//  0            6     11       16         20    22
//
// --------------------------------------------------------------------------------


inline BG_UniversalComponentIdentifier bg_encodeFanAssemblyOnIoRackUCI( unsigned row, unsigned column, unsigned ioDrawer, unsigned assembly ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_FanAssemblyOnIoRack );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  uci |= BG_UCI_SET_IO_DRAWER( ioDrawer );
  uci |= BG_UCI_SET_FAN_ASSEMBLY( assembly );
  return uci;
}

inline int bg_decodeFanAssemblyOnIoRackUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column, unsigned* ioDrawer, unsigned* assembly ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_FanAssemblyOnIoRack ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  *ioDrawer = BG_UCI_GET_IO_DRAWER(uci);
  *assembly = BG_UCI_GET_FAN_ASSEMBLY(uci);
  return 0;
}

inline BG_UniversalComponentIdentifier bg_encodeFanAssemblyOnComputeRackUCI( unsigned row, unsigned column, unsigned ioDrawer, unsigned assembly ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_FanAssemblyOnComputeRack );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  uci |= BG_UCI_SET_IO_DRAWER( ioDrawer );
  uci |= BG_UCI_SET_FAN_ASSEMBLY( assembly );
  return uci;
}

inline int bg_decodeFanAssemblyOnComputeRackUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column, unsigned* ioDrawer, unsigned* assembly ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_FanAssemblyOnComputeRack ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  *ioDrawer = BG_UCI_GET_IO_DRAWER(uci);
  *assembly = BG_UCI_GET_FAN_ASSEMBLY(uci);
  return 0;
}

// --------------------------------------------------------------------------------
//
//  Fan         : "Qxy-Ii-Hh-Ff" x=row, y=column, i=io-drawer, h=fan-assembly, f=fan
//                "Rxy-Ii-Hh-Ff"

//  +-----------+-----+--------+----------+-----+-----+---------+
//  | Component | Row | Column | IO Board | Ass | Fan |   ///   |
//  +-----------+-----+--------+----------+-----+-----+---------+
//  0            6     11       16         20    22    24
//
// --------------------------------------------------------------------------------


inline BG_UniversalComponentIdentifier bg_encodeFanOnIoRackUCI( unsigned row, unsigned column, unsigned ioDrawer, unsigned assembly, unsigned fan ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_FanOnIoRack );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  uci |= BG_UCI_SET_IO_DRAWER( ioDrawer );
  uci |= BG_UCI_SET_FAN_ASSEMBLY( assembly );
  uci |= BG_UCI_SET_FAN( fan );
  return uci;
}

inline int bg_decodeFanOnIoRackUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column, unsigned* ioDrawer, unsigned* assembly, unsigned* fan ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_FanOnIoRack ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  *ioDrawer = BG_UCI_GET_IO_DRAWER(uci);
  *assembly = BG_UCI_GET_FAN_ASSEMBLY(uci);
  *fan =  BG_UCI_GET_FAN(uci);
  return 0;
}

inline BG_UniversalComponentIdentifier bg_encodeFanOnComputeRackUCI( unsigned row, unsigned column, unsigned ioDrawer, unsigned assembly, unsigned fan ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_FanOnComputeRack );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  uci |= BG_UCI_SET_IO_DRAWER( ioDrawer );
  uci |= BG_UCI_SET_FAN_ASSEMBLY( assembly );
  uci |= BG_UCI_SET_FAN( fan );
  return uci;
}

inline int bg_decodeFanOnComputeRackUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column, unsigned* ioDrawer, unsigned* assembly, unsigned* fan ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_FanOnComputeRack ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  *ioDrawer = BG_UCI_GET_IO_DRAWER(uci);
  *assembly = BG_UCI_GET_FAN_ASSEMBLY(uci);
  *fan =  BG_UCI_GET_FAN(uci);
  return 0;
}



// --------------------------------------------------------------------------------
//
//  Compute Card on Node Board : "Rxy-Mm-Nn-Jj" x=row, y=column, m = midplane, 
//                               n=node-board c=compute-card
//
//  +-----------+-----+--------+----------+------------+--------------+--------+
//  | Component | Row | Column | Midplane | Node Board | Compute Card |  ///   |
//  +-----------+-----+--------+----------+------------+--------------+--------+
//  0            6     11       16         17           21             26
//
// --------------------------------------------------------------------------------

inline BG_UniversalComponentIdentifier bg_encodeComputeCardOnNodeBoardUCI( unsigned row, unsigned column, unsigned midplane, unsigned nodeBoard, unsigned computeCard ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_ComputeCardOnNodeBoard );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  uci |= BG_UCI_SET_MIDPLANE( midplane );
  uci |= BG_UCI_SET_NODE_BOARD( nodeBoard );
  uci |= BG_UCI_SET_COMPUTE_CARD( computeCard );
  return uci;
}

inline int bg_decodeComputeCardOnNodeBoardUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column, unsigned* midplane, unsigned* nodeBoard, unsigned* computeCard ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_ComputeCardOnNodeBoard ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  *midplane = BG_UCI_GET_MIDPLANE(uci);
  *nodeBoard = BG_UCI_GET_NODE_BOARD(uci);
  *computeCard = BG_UCI_GET_COMPUTE_CARD(uci);
  return 0;
}

// --------------------------------------------------------------------------------
//
//  Compute Card on IO Board: "Qxy-Ii-Jj" x=row, y=column, i=io-board, j=compute-card
//
//  +-----------+-----+--------+----------+-----+--------------+---------+
//  | Component | Row | Column | IO Board | /// | Compute Card |   ///   |
//  +-----------+-----+--------+----------+-----+--------------+---------+
//  0            6     11       16         20    22             26
//
// --------------------------------------------------------------------------------


inline BG_UniversalComponentIdentifier bg_encodeComputeCardOnIoBoardOnIoRackUCI( unsigned row, unsigned column, unsigned ioDrawer, unsigned computeCard ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_ComputeCardOnIoBoardOnIoRack );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  uci |= BG_UCI_SET_IO_DRAWER( ioDrawer );
  uci |= BG_UCI_SET_COMPUTE_CARD( computeCard );
  return uci;
}

inline int bg_decodeComputeCardOnIoBoardOnIoRackUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column, unsigned* ioDrawer, unsigned* computeCard ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_ComputeCardOnIoBoardOnIoRack ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  *ioDrawer = BG_UCI_GET_IO_DRAWER(uci);
  *computeCard = BG_UCI_GET_COMPUTE_CARD(uci);
  return 0;
}

inline BG_UniversalComponentIdentifier bg_encodeComputeCardOnIoBoardOnComputeRackUCI( unsigned row, unsigned column, unsigned ioDrawer, unsigned computeCard ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_ComputeCardOnIoBoardOnComputeRack );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  uci |= BG_UCI_SET_IO_DRAWER( ioDrawer );
  uci |= BG_UCI_SET_COMPUTE_CARD( computeCard );
  return uci;
}

inline int bg_decodeComputeCardOnIoBoardOnComputeRackUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column, unsigned* ioDrawer, unsigned* computeCard ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_ComputeCardOnIoBoard ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  *ioDrawer = BG_UCI_GET_IO_DRAWER(uci);
  *computeCard = BG_UCI_GET_COMPUTE_CARD(uci);
  return 0;
}

// --------------------------------------------------------------------------------
//
//  Compute Card Core on Node Board : "Rxy-Mm-Nn-Jj-Cc" x=row, y=column, 
//               m = midplane, n=node-board j=compute-card cc=compute-core
//
//  +-----------+-----+--------+----------+------------+--------------+------+--------+
//  | Component | Row | Column | Midplane | Node Board | Compute Card | Core |  ///   |
//  +-----------+-----+--------+----------+------------+--------------+------+--------+
//  0            6     11       16         17           21             26     31
//
// --------------------------------------------------------------------------------

inline BG_UniversalComponentIdentifier bg_encodeComputeCardCoreOnNodeBoardUCI( unsigned row, unsigned column, unsigned midplane, unsigned nodeBoard, unsigned computeCard, unsigned core ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_ComputeCardCoreOnNodeBoard );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  uci |= BG_UCI_SET_MIDPLANE( midplane );
  uci |= BG_UCI_SET_NODE_BOARD( nodeBoard );
  uci |= BG_UCI_SET_COMPUTE_CARD( computeCard );
  uci |= BG_UCI_SET_CORE( core );
  return uci;
}

inline int bg_decodeComputeCardCoreOnNodeBoardUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column, unsigned* midplane, unsigned* nodeBoard, unsigned* computeCard, unsigned* core ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_ComputeCardOnNodeBoard ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  *midplane = BG_UCI_GET_MIDPLANE(uci);
  *nodeBoard = BG_UCI_GET_NODE_BOARD(uci);
  *computeCard = BG_UCI_GET_COMPUTE_CARD(uci);
  *core = BG_UCI_GET_CORE(uci);
  return 0;
}

// --------------------------------------------------------------------------------
//
//  Compute Card Core on IO Board: "Qxy-Ii-Jj-Cc" x=row, y=column, i=io-board, 
//                     j=compute-card c=compute-core
//
//  +-----------+-----+--------+----------+-----+--------------+------+---------+
//  | Component | Row | Column | IO Board | /// | Compute Card | Core |   ///   |
//  +-----------+-----+--------+----------+-----+--------------+------+---------+
//  0            6     11       16         20    22             26     31
//
// --------------------------------------------------------------------------------


inline BG_UniversalComponentIdentifier bg_encodeComputeCardCoreOnIoBoardUCI( unsigned row, unsigned column, unsigned ioDrawer, unsigned computeCard, unsigned core ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_ComputeCardCoreOnIoBoard );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  uci |= BG_UCI_SET_IO_DRAWER( ioDrawer );
  uci |= BG_UCI_SET_COMPUTE_CARD( computeCard );
  uci |= BG_UCI_SET_CORE( core );
  return uci;
}

inline int bg_decodeComputeCardCoreOnIoBoardUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column, unsigned* ioDrawer, unsigned* computeCard, unsigned* core ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_ComputeCardCoreOnIoBoard ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  *ioDrawer = BG_UCI_GET_IO_DRAWER(uci);
  *computeCard = BG_UCI_GET_COMPUTE_CARD(uci);
  *core = BG_UCI_GET_CORE(uci);
  return 0;
}

// --------------------------------------------------------------------------------
//
//  Service Card: Rxy-Mm-S" x=row, y=column, m = midplane
//
//  +-----------+-----+--------+----------+--------+
//  | Component | Row | Column | Midplane |  ///   |
//  +-----------+-----+--------+----------+--------+
//  0            6     11       16         17
//
// --------------------------------------------------------------------------------

inline BG_UniversalComponentIdentifier bg_encodeServiceCardUCI( unsigned row, unsigned column, unsigned midplane ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_ServiceCard );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  uci |= BG_UCI_SET_MIDPLANE( midplane );
  return uci;
}

inline int bg_decodeServiceCardUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column, unsigned* midplane ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_IoRack ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  *midplane = BG_UCI_GET_MIDPLANE(uci);
  return 0;
}

// --------------------------------------------------------------------------------
//
//  Link Module on Node Board : "Rxy-Mm-Nn-Ll" x=row, y=column, m = midplane, 
//            n=node-board l=link-module
//
//  +-----------+-----+--------+----------+------------+--------------+--------+
//  | Component | Row | Column | Midplane | Node Board | Link Module  |  ///   |
//  +-----------+-----+--------+----------+------------+--------------+--------+
//  0            6     11       16         17           21             26
//
// --------------------------------------------------------------------------------

inline BG_UniversalComponentIdentifier bg_encodeLinkModuleOnNodeBoardUCI( unsigned row, unsigned column, unsigned midplane, unsigned nodeBoard, unsigned linkModule ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_LinkModuleOnNodeBoard );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  uci |= BG_UCI_SET_MIDPLANE( midplane );
  uci |= BG_UCI_SET_NODE_BOARD( nodeBoard );
  uci |= BG_UCI_SET_LINK_MODULE( linkModule );
  return uci;
}

inline int bg_decodeLinkModuleOnNodeBoardUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column, unsigned* midplane, unsigned* nodeBoard, unsigned* linkModule ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_LinkModuleOnNodeBoard ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  *midplane = BG_UCI_GET_MIDPLANE(uci);
  *nodeBoard = BG_UCI_GET_NODE_BOARD(uci);
  *linkModule = BG_UCI_GET_LINK_MODULE(uci);
  return 0;
}

// --------------------------------------------------------------------------------
//
//  Link Module on IO Board: "Qxy-Ii-Jj" x=row, y=column, i=io-board, l=link-module
//
//  +-----------+-----+--------+----------+-----+--------------+---------+
//  | Component | Row | Column | IO Board | /// | Link Module  |   ///   |
//  +-----------+-----+--------+----------+-----+--------------+---------+
//  0            6     11       16         20    21             25
//
// --------------------------------------------------------------------------------


inline BG_UniversalComponentIdentifier bg_encodeLinkModuleOnIoBoardUCI( unsigned row, unsigned column, unsigned ioDrawer, unsigned linkModule ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_LinkModuleOnIoBoard );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  uci |= BG_UCI_SET_IO_DRAWER( ioDrawer );
  uci |= BG_UCI_SET_LINK_MODULE( linkModule );
  return uci;
}

inline int bg_decodeLinkModuleOnIoBoardUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column, unsigned* ioDrawer, unsigned* linkModule ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_LinkModuleOnIoBoard ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  *ioDrawer = BG_UCI_GET_IO_DRAWER(uci);
  *linkModule = BG_UCI_GET_LINK_MODULE(uci);
  return 0;
}

// --------------------------------------------------------------------------------
//
//  Optical Module on Node Board : "Rxy-Mm-Nn-Oo" x=row, y=column, m = midplane, 
//     n=node-board o=optical-module
//
//  +-----------+-----+--------+----------+------------+--------------+--------+
//  | Component | Row | Column | Midplane | Node Board | Opt  Module  |  ///   |
//  +-----------+-----+--------+----------+------------+--------------+--------+
//  0            6     11       16         17           21             27
//
// --------------------------------------------------------------------------------

inline BG_UniversalComponentIdentifier bg_encodeOpticalModuleOnNodeBoardUCI( unsigned row, unsigned column, unsigned midplane, unsigned nodeBoard, unsigned opticalModule ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_OpticalModuleOnNodeBoard );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  uci |= BG_UCI_SET_MIDPLANE( midplane );
  uci |= BG_UCI_SET_NODE_BOARD( nodeBoard );
  uci |= BG_UCI_SET_OPTICAL_MODULE( opticalModule );
  return uci;
}

inline int bg_decodeOpticalModuleOnNodeBoardUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column, unsigned* midplane, unsigned* nodeBoard, unsigned* opticalModule ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_OpticalModuleOnNodeBoard ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  *midplane = BG_UCI_GET_MIDPLANE(uci);
  *nodeBoard = BG_UCI_GET_NODE_BOARD(uci);
  *opticalModule = BG_UCI_GET_OPTICAL_MODULE(uci);
  return 0;
}

// --------------------------------------------------------------------------------
//
//  Optical Module on IO Board: "Qxy-Ii-Oo" x=row, y=column, i=io-board, o=optical-module
//
//  +-----------+-----+--------+----------+-----+--------------+---------+
//  | Component | Row | Column | IO Board | /// | Opt  Module  |   ///   |
//  +-----------+-----+--------+----------+-----+--------------+---------+
//  0            6     11       16         20    21             27
//
// --------------------------------------------------------------------------------


inline BG_UniversalComponentIdentifier bg_encodeOpticalModuleOnIoBoardUCI( unsigned row, unsigned column, unsigned ioDrawer, unsigned opticalModule ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_OpticalModuleOnIoBoard );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  uci |= BG_UCI_SET_IO_DRAWER( ioDrawer );
  uci |= BG_UCI_SET_OPTICAL_MODULE( opticalModule );
  return uci;
}

inline int bg_decodeOpticalModuleOnIoBoardUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column, unsigned* ioDrawer, unsigned* opticalModule ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_OpticalModuleOnIoBoard ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  *ioDrawer = BG_UCI_GET_IO_DRAWER(uci);
  *opticalModule = BG_UCI_GET_OPTICAL_MODULE(uci);
  return 0;
}

// --------------------------------------------------------------------------------
//
//  DCA Module on Node Board : "Rxy-Mm-Nn-Dd" x=row, y=column, m = midplane, n=node-board d=dca-module
//  DCA Module on IO Board   : "Qxy-Ii-Dd" x=row, y=column, m = midplane, n=node-board d=dca-module
//
//  +-----------+-----+--------+----------+------------+-----+--------+
//  | Component | Row | Column | Midplane | Node Board | DCA |  ///   |
//  +-----------+-----+--------+----------+------------+-----+--------+
//  0            6     11       16         17           21    22
//
// --------------------------------------------------------------------------------

inline BG_UniversalComponentIdentifier bg_encodeDcaOnNodeBoardUCI( unsigned row, unsigned column, unsigned midplane, unsigned nodeBoard, unsigned dca ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_DcaOnNodeBoard );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  uci |= BG_UCI_SET_MIDPLANE( midplane );
  uci |= BG_UCI_SET_NODE_BOARD( nodeBoard );
  uci |= BG_UCI_SET_DCA( dca );
  return uci;
}

inline int bg_decodeDcaOnNodeBoardUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column, unsigned* midplane, unsigned* nodeBoard, unsigned* dca ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_DcaOnNodeBoard ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  *midplane = BG_UCI_GET_MIDPLANE(uci);
  *nodeBoard = BG_UCI_GET_NODE_BOARD(uci);
  *dca = BG_UCI_GET_DCA(uci);
  return 0;
}

inline BG_UniversalComponentIdentifier bg_encodeDcaOnIoBoardUCI( unsigned row, unsigned column, unsigned ioDrawer, unsigned dca ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_DcaOnIoBoard );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  uci |= BG_UCI_SET_IO_DRAWER( ioDrawer );
  uci |= BG_UCI_SET_DCA( dca );
  return uci;
}

inline int bg_decodeDcaOnIoBoardUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column, unsigned* ioDrawer, unsigned* dca ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_DcaOnIoBoard ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  *ioDrawer = BG_UCI_GET_IO_DRAWER(uci);
  *dca = BG_UCI_GET_DCA(uci);
  return 0;
}

// --------------------------------------------------------------------------------
//
//  PCI Adapter Module on IO Board: "Qxy-Ii-Aa" x=row, y=column, i=io-board, a=pci-adapter
//
//  +-----------+-----+--------+----------+-----+-------+
//  | Component | Row | Column | IO Board | PCI |  ///  |
//  +-----------+-----+--------+----------+-----+-------+
//  0            6     11       16         20    21      
//
// --------------------------------------------------------------------------------


inline BG_UniversalComponentIdentifier bg_encodePciAdapterUCI( unsigned row, unsigned column, unsigned ioDrawer, unsigned pciAdapter ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_PciAdapterCard );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  uci |= BG_UCI_SET_IO_DRAWER( ioDrawer );
  uci |= BG_UCI_SET_PCI_ADAPTER( pciAdapter );
  return uci;
}

inline int bg_decodePciAdapterUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column, unsigned* ioDrawer, unsigned* pciAdapter ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_PciAdapterCard ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  *ioDrawer = BG_UCI_GET_IO_DRAWER(uci);
  *pciAdapter = BG_UCI_GET_PCI_ADAPTER(uci);
  return 0;
}

// --------------------------------------------------------------------------------
//
//  Leak Detector: Rxy-L" 
//
//  +-----------+-----+--------+---------+
//  | Component | Row | Column |   ///   |
//  +-----------+-----+--------+---------+
//  0            6     11       16
//
// --------------------------------------------------------------------------------

inline BG_UniversalComponentIdentifier bg_encodeLeakDetectorOnComputeRackUCI( unsigned row, unsigned column ) {
  BG_UniversalComponentIdentifier uci = 0;
  uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_LeakDetectorOnComputeRack );
  uci |= BG_UCI_SET_ROW( row );
  uci |= BG_UCI_SET_COLUMN( column );
  return uci;
}

inline int bg_decodeLeakDetectorOnComputeRackUCI( BG_UniversalComponentIdentifier uci, unsigned* row, unsigned* column ) {
  _BG_UCI_ASSERT( BG_UCI_GET_COMPONENT(uci) == BG_UCI_Component_LeakDetectorOnComputeRack ); 
  *row = BG_UCI_GET_ROW(uci);
  *column = BG_UCI_GET_COLUMN(uci);
  return 0;
}



#define _BG_ENCODE_COMPUTE_RACK(buf,i,row,col) (buf)[(i)++] = 'R'; (buf)[(i)++] = (row); (buf)[(i)++] = (col);
#define _BG_ENCODE_IO_RACK(buf,i,row,col)      (buf)[(i)++] = 'Q'; (buf)[(i)++] = (row); (buf)[(i)++] = (col);
#define _BG_ENCODE_MIDPLANE(buf,i,midp)        (buf)[(i)++] = '-'; (buf)[(i)++] = 'M'; (buf)[(i)++] = midp;
#define _BG_ENCODE_IO_DRAWER(buf,i,iob)        (buf)[(i)++] = '-'; (buf)[(i)++] = 'I'; (buf)[(i)++] = iob;
#define _BG_ENCODE_BULK_POWER_SUPPLY(buf,i,b)  (buf)[(i)++] = '-'; (buf)[(i)++] = 'B'; (buf)[(i)++] = b;
#define _BG_ENCODE_POWER_MODULE(buf,i,p)       (buf)[(i)++] = '-'; (buf)[(i)++] = 'P'; (buf)[(i)++] = p;
#define _BG_ENCODE_CLOCK_CARD(buf,i,c)         (buf)[(i)++] = '-'; (buf)[(i)++] = 'K'; (buf)[(i)++] = c;
#define _BG_ENCODE_NODE_BOARD(buf,i,n0,n1)     (buf)[(i)++] = '-'; (buf)[(i)++] = 'N'; (buf)[(i)++] = n0; (buf)[(i)++] = n1;
#define _BG_ENCODE_FAN(buf,i,f)                (buf)[(i)++] = '-'; (buf)[(i)++] = 'F'; (buf)[(i)++] = f;
#define _BG_ENCODE_FAN_ASSEMBLY(buf,i,f)       (buf)[(i)++] = '-'; (buf)[(i)++] = 'H'; (buf)[(i)++] = f;
#define _BG_ENCODE_COMPUTE_CARD(buf,i,c0,c1)   (buf)[(i)++] = '-'; (buf)[(i)++] = 'J'; (buf)[(i)++] = c0; (buf)[(i)++] = c1;
#define _BG_ENCODE_CORE(buf,i,c0,c1)           (buf)[(i)++] = '-'; (buf)[(i)++] = 'C'; (buf)[(i)++] = c0; (buf)[(i)++] = c1;
#define _BG_ENCODE_LINK_MODULE(buf,i,l)        (buf)[(i)++] = '-'; (buf)[(i)++] = 'U'; (buf)[(i)++] = '0'; (buf)[(i)++] = l;
#define _BG_ENCODE_OPTICAL_MODULE(buf,i,o0,o1) (buf)[(i)++] = '-'; (buf)[(i)++] = 'O'; (buf)[(i)++] = o0; (buf)[(i)++] = o1;
#define _BG_ENCODE_DCA(buf,i,l)                (buf)[(i)++] = '-'; (buf)[(i)++] = 'D'; (buf)[(i)++] = l;
#define _BG_ENCODE_PCI_ADAPTER(buf,i,l)        (buf)[(i)++] = '-'; (buf)[(i)++] = 'A'; (buf)[(i)++] = l;
#define _BG_ENCODE_LEAK_DETECTOR(buf,i)        (buf)[(i)++] = '-'; (buf)[(i)++] = 'L';

/*extern*/ inline int bg_uci_toString( BG_UniversalComponentIdentifier uci, char *buf ) {

  const char* digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  unsigned i = 0;

  switch ( BG_UCI_GET_COMPONENT(uci) ) {

  case BG_UCI_Component_ComputeRack : // "Rxx"
    {
      _BG_ENCODE_COMPUTE_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      break;
    }

  case BG_UCI_Component_IoRack : // "Qxx"
    {
      _BG_ENCODE_IO_RACK(buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      break;
    }

  case BG_UCI_Component_Midplane : // "Rxx-Mx"
    {
      _BG_ENCODE_COMPUTE_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_MIDPLANE(buf, i, digits[ BG_UCI_GET_MIDPLANE(uci) ] );
      break;
    }

  case BG_UCI_Component_BulkPowerSupplyOnComputeRack : // "Rxx-Bb"
    {
      _BG_ENCODE_COMPUTE_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_BULK_POWER_SUPPLY( buf, i, digits[ BG_UCI_GET_BULK_POWER_SUPPLY(uci)] ); 
      break;
    }

  case BG_UCI_Component_BulkPowerSupplyOnIoRack : // "Qxx-B"
    {
      _BG_ENCODE_IO_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_BULK_POWER_SUPPLY( buf, i, digits[ BG_UCI_GET_BULK_POWER_SUPPLY(uci)] ); 
      i--; // remove digit
      break;
    }

  case BG_UCI_Component_PowerModuleOnComputeRack : // "Rxx-Bx-Px"
    {
      _BG_ENCODE_COMPUTE_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_BULK_POWER_SUPPLY( buf, i, digits[ BG_UCI_GET_BULK_POWER_SUPPLY(uci) ] );
      _BG_ENCODE_POWER_MODULE( buf, i, digits[ BG_UCI_GET_POWER_MODULE(uci) ] );
      break;
    }

  case BG_UCI_Component_PowerModuleOnIoRack :      // "Qxx-B-Px"
    {
      _BG_ENCODE_IO_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_BULK_POWER_SUPPLY( buf, i, 0 );
      i--;
      _BG_ENCODE_POWER_MODULE( buf, i, digits[ BG_UCI_GET_POWER_MODULE(uci) ] );
      break;
    }

    
  case BG_UCI_Component_ClockCardOnComputeRack :
    {
      _BG_ENCODE_COMPUTE_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_CLOCK_CARD( buf, i, 0 );
      i--;
      break;
    }


  case BG_UCI_Component_ClockCardOnIoRack :
    {
      _BG_ENCODE_IO_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_CLOCK_CARD( buf, i, digits[ BG_UCI_GET_CLOCK_CARD(uci) ] );
      break;
    }


  case BG_UCI_Component_IoDrawerOnComputeRack :  // "Rxx-Ix"
    {
      _BG_ENCODE_COMPUTE_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_IO_DRAWER( buf, i, digits[ BG_UCI_GET_IO_DRAWER(uci) ] );
      break;
    }

  case BG_UCI_Component_IoDrawerOnIoRack : // "Qxx-Ix"
    {
      _BG_ENCODE_IO_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_IO_DRAWER( buf, i, digits[ BG_UCI_GET_IO_DRAWER(uci) ] );
      break;
    }

  case BG_UCI_Component_NodeBoard : // "Rxx-Mx-Nxx"
    {
      _BG_ENCODE_COMPUTE_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_MIDPLANE( buf, i, digits[ BG_UCI_GET_MIDPLANE(uci) ] );
      _BG_ENCODE_NODE_BOARD( buf, i, digits[ BG_UCI_GET_NODE_BOARD(uci) / 10 ], digits[ BG_UCI_GET_NODE_BOARD(uci) % 10 ] ); 
      break;
    }

  case  BG_UCI_Component_FanAssemblyOnIoRack : //"Qxx-Ix-Hx"
    {
      _BG_ENCODE_IO_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_IO_DRAWER( buf, i, digits[ BG_UCI_GET_IO_DRAWER(uci) ] );
      _BG_ENCODE_FAN_ASSEMBLY( buf, i, digits[ BG_UCI_GET_FAN_ASSEMBLY(uci) ] );
      break;
    }

  case  BG_UCI_Component_FanAssemblyOnComputeRack : //"Rxx-Ix-Hx"
    {
      _BG_ENCODE_COMPUTE_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_IO_DRAWER( buf, i, digits[ BG_UCI_GET_IO_DRAWER(uci) ] );
      _BG_ENCODE_FAN_ASSEMBLY( buf, i, digits[ BG_UCI_GET_FAN_ASSEMBLY(uci) ] );
      break;
    }

  case  BG_UCI_Component_FanOnIoRack : //"Qxx-Ix-Hx-Fx"
    {
      _BG_ENCODE_IO_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_IO_DRAWER( buf, i, digits[ BG_UCI_GET_IO_DRAWER(uci) ] );
      _BG_ENCODE_FAN_ASSEMBLY( buf, i, digits[ BG_UCI_GET_FAN_ASSEMBLY(uci) ] );
      _BG_ENCODE_FAN( buf, i, digits[ BG_UCI_GET_FAN(uci) ] );
      break;
    }

  case  BG_UCI_Component_FanOnComputeRack : //"Rxx-Ix-Hx"
    {
      _BG_ENCODE_COMPUTE_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_IO_DRAWER( buf, i, digits[ BG_UCI_GET_IO_DRAWER(uci) ] );
      _BG_ENCODE_FAN_ASSEMBLY( buf, i, digits[ BG_UCI_GET_FAN_ASSEMBLY(uci) ] );
      _BG_ENCODE_FAN( buf, i, digits[ BG_UCI_GET_FAN(uci) ] );
      break;
    }

  case  BG_UCI_Component_ComputeCardOnNodeBoard  : // "Rxx-Mx-Nx-Jxx"
    {
      _BG_ENCODE_COMPUTE_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_MIDPLANE( buf, i, digits[ BG_UCI_GET_MIDPLANE(uci) ] );
      _BG_ENCODE_NODE_BOARD( buf, i, digits[ BG_UCI_GET_NODE_BOARD(uci) / 10 ], digits[ BG_UCI_GET_NODE_BOARD(uci) % 10 ] ); 
      _BG_ENCODE_COMPUTE_CARD( buf, i, digits[ BG_UCI_GET_COMPUTE_CARD(uci) /10 ], digits[ BG_UCI_GET_COMPUTE_CARD(uci) % 10 ] );
      break;
    }
 
  case BG_UCI_Component_ComputeCardOnIoBoardOnIoRack : // "Qxx-Ix-Jxx"
    {
      _BG_ENCODE_IO_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_IO_DRAWER( buf, i, digits[ BG_UCI_GET_IO_DRAWER(uci) ] );
      _BG_ENCODE_COMPUTE_CARD( buf, i, digits[ BG_UCI_GET_COMPUTE_CARD(uci) /10 ], digits[ BG_UCI_GET_COMPUTE_CARD(uci) % 10 ] );
      break;
    }

  case BG_UCI_Component_ComputeCardOnIoBoardOnComputeRack : // "Rxx-Ix-Jxx"
    {
      _BG_ENCODE_COMPUTE_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_IO_DRAWER( buf, i, digits[ BG_UCI_GET_IO_DRAWER(uci) ] );
      _BG_ENCODE_COMPUTE_CARD( buf, i, digits[ BG_UCI_GET_COMPUTE_CARD(uci) /10 ], digits[ BG_UCI_GET_COMPUTE_CARD(uci) % 10 ] );
      break;
    }

  case BG_UCI_Component_ComputeCardCoreOnNodeBoard : // "Rxx-Mx-Nx-Jxx-Cxx"
    {
      _BG_ENCODE_COMPUTE_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_MIDPLANE( buf, i, digits[ BG_UCI_GET_MIDPLANE(uci) ] );
      _BG_ENCODE_NODE_BOARD( buf, i, digits[ BG_UCI_GET_NODE_BOARD(uci) / 10 ], digits[ BG_UCI_GET_NODE_BOARD(uci) % 10 ] ); 
      _BG_ENCODE_COMPUTE_CARD( buf, i, digits[ BG_UCI_GET_COMPUTE_CARD(uci) /10 ], digits[ BG_UCI_GET_COMPUTE_CARD(uci) % 10 ] );
      _BG_ENCODE_CORE( buf, i, digits[ BG_UCI_GET_CORE(uci) / 10 ], digits[ BG_UCI_GET_CORE(uci) % 10 ] );
      break;
    }

  case BG_UCI_Component_ComputeCardCoreOnIoBoard : // "Qxx-Ix-Jxx-Cxx
    {
      _BG_ENCODE_IO_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_IO_DRAWER( buf, i, digits[ BG_UCI_GET_IO_DRAWER(uci) ] );
      _BG_ENCODE_COMPUTE_CARD( buf, i, digits[ BG_UCI_GET_COMPUTE_CARD(uci) /10 ], digits[ BG_UCI_GET_COMPUTE_CARD(uci) % 10 ] );
      _BG_ENCODE_CORE( buf, i, digits[ BG_UCI_GET_CORE(uci) / 10 ], digits[ BG_UCI_GET_CORE(uci) % 10 ] );
      break;
    }

  case BG_UCI_Component_ServiceCard : // "Rxx-Mx-S"
    {
      _BG_ENCODE_COMPUTE_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_MIDPLANE( buf, i, digits[ BG_UCI_GET_MIDPLANE(uci) ] );
      buf[i++] = '-';
      buf[i++] = 'S';
      break;
    }

  case  BG_UCI_Component_LinkModuleOnNodeBoard : // "Rxx-Mx-Nxx-Uxx"
    {
      _BG_ENCODE_COMPUTE_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_MIDPLANE( buf, i, digits[ BG_UCI_GET_MIDPLANE(uci) ] );
      _BG_ENCODE_NODE_BOARD( buf, i, digits[ BG_UCI_GET_NODE_BOARD(uci) / 10 ], digits[ BG_UCI_GET_NODE_BOARD(uci) % 10 ] );
      _BG_ENCODE_LINK_MODULE( buf, i, digits[ BG_UCI_GET_LINK_MODULE(uci) ] );
      break;
    }

  case BG_UCI_Component_LinkModuleOnIoBoard  : // "Qxx-Ixx-Uxx"
    {
      _BG_ENCODE_IO_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_IO_DRAWER( buf, i, digits[ BG_UCI_GET_IO_DRAWER(uci) ] );
      _BG_ENCODE_LINK_MODULE( buf, i, digits[ BG_UCI_GET_LINK_MODULE(uci) ] );
      break;
    }

  case BG_UCI_Component_OpticalModuleOnNodeBoard : // "Rxx-Mx-Nxx-Oxx"
    {
      _BG_ENCODE_COMPUTE_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_MIDPLANE( buf, i, digits[ BG_UCI_GET_MIDPLANE(uci) ] );
      _BG_ENCODE_NODE_BOARD( buf, i, digits[ BG_UCI_GET_NODE_BOARD(uci) / 10 ], digits[ BG_UCI_GET_NODE_BOARD(uci) % 10 ] );
      _BG_ENCODE_OPTICAL_MODULE( buf, i, digits[ BG_UCI_GET_OPTICAL_MODULE(uci) / 10 ], digits[ BG_UCI_GET_OPTICAL_MODULE(uci) % 10] );
      break;
    }

  case BG_UCI_Component_OpticalModuleOnIoBoard : // "Qxx-Ixx-Oxx"
    {
      _BG_ENCODE_IO_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_IO_DRAWER( buf, i, digits[ BG_UCI_GET_IO_DRAWER(uci) ] )
      _BG_ENCODE_OPTICAL_MODULE( buf, i, digits[ BG_UCI_GET_OPTICAL_MODULE(uci) / 10 ], digits[ BG_UCI_GET_OPTICAL_MODULE(uci) % 10] );
      break;
    }

  case BG_UCI_Component_DcaOnNodeBoard : // "Rxx-Mx-Nxx-Dxx"
    {
      _BG_ENCODE_COMPUTE_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_MIDPLANE( buf, i, digits[ BG_UCI_GET_MIDPLANE(uci) ] );
      _BG_ENCODE_NODE_BOARD( buf, i, digits[ BG_UCI_GET_NODE_BOARD(uci) / 10 ], digits[ BG_UCI_GET_NODE_BOARD(uci) % 10 ] );
      _BG_ENCODE_DCA( buf, i, digits[ BG_UCI_GET_DCA(uci) ] );
      break;
    }

  case BG_UCI_Component_DcaOnIoBoard : // "Qxx-Ix-Dxx"
    {
      _BG_ENCODE_IO_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_IO_DRAWER( buf, i, digits[ BG_UCI_GET_IO_DRAWER(uci) ] );
      _BG_ENCODE_DCA( buf, i, digits[ BG_UCI_GET_DCA(uci) ] );
      break;
    }

  case BG_UCI_Component_PciAdapterCard : // "Qxx-Ixx-Axx"
    {
      _BG_ENCODE_IO_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_IO_DRAWER( buf, i, digits[ BG_UCI_GET_IO_DRAWER(uci) ] );
      _BG_ENCODE_PCI_ADAPTER( buf, i, digits[ BG_UCI_GET_PCI_ADAPTER(uci) ] );
      break;
    }

  case BG_UCI_Component_LeakDetectorOnComputeRack : // "Rxx-L"
    {
      _BG_ENCODE_COMPUTE_RACK( buf, i, digits[ BG_UCI_GET_ROW(uci) ], digits[ BG_UCI_GET_COLUMN(uci) ] );
      _BG_ENCODE_LEAK_DETECTOR( buf, i );
      break;
    }

  default:
    return -1;

  }

  buf[i++] = 0;
  return 0;

}

inline int _uci_pattern_matches( const char* s, const char* p ) {

  //  printf("---\n%s\n%s\n---\n", s, p);
  
  unsigned i = 0;

  while ( p[i] != 0 ) {

    if ( s[i] == 0 ) {
      return 0;
    }
    else if ( p[i] == '*' ) {
      i++;
    }
    else if ( p[i] == s[i] ) {
      i++;
    }
    else {
      return 0;
    }
  }

  return s[i] == 0;
}


inline int _parseRowCol( char** s, unsigned* rowcol ) {

  char c = **s;
  (*s)++;

  if ( ( c >= '0' ) && ( c <= '9') ) {
    *rowcol = c - '0';
    return 0;
  }
  else if ( ( c >= 'A' ) && ( c <= 'V') ) {
    *rowcol = c - 'A' + 10;
    return 0;
  }
  else {
    return -1;
  }
}

inline int _parseComputeRack( char** s, unsigned* row, unsigned* col ) {
  
 
  if ( **s == 'R' ) {
    int rc = 0;
    (*s)++;
    rc |= _parseRowCol( s, row );
    rc |= _parseRowCol( s, col );
    return rc;
  }

  return -1;
}

inline int _parseIoRack( char** s, unsigned* row, unsigned* col ) {
 
  if ( **s == 'Q' ) {
    int rc = 0;
    (*s)++;
    rc |= _parseRowCol( s, row );
    rc |= _parseRowCol( s, col );
    return rc;
  }

  return -1;
}

inline int _parseSingleDigit( char** s, unsigned* value, char id, unsigned lower, unsigned upper ) {
 
  if ( **s != '-' )
    return -1;

  (*s)++;

  if ( **s != id )
    return -1;

  (*s)++;
  
  char digit = **s;

  if ( (digit >= '0') && (digit <= '9') )
    *value = digit - '0';
  else if ( (digit >= 'A') && (digit <= 'Z') )
    *value = digit - 'A' + 10;
  else
    return -1;

  (*s)++;

  return (*value < lower) || (*value > upper);
}

inline int _parseMidPlane( char** s, unsigned* midp ) {
  return _parseSingleDigit( s, midp, 'M', 0, 1 );
}

inline int _parseBulkPowerSupply( char** s, unsigned* bps ) {
   return _parseSingleDigit( s, bps, 'B', 0, 3 );
}

inline int _parseDca( char** s, unsigned* dca ) {
    return _parseSingleDigit( s, dca, 'D', 0, 1 );
}

inline int _parsePciAdapter( char** s, unsigned* pci ) {
  return _parseSingleDigit( s, pci, 'A', 0, 7 );
}

inline int _parsePowerModule( char** s, unsigned* pm ) {
 return _parseSingleDigit( s, pm, 'P', 0, 8 );
}

inline int _parseClockCard( char** s, unsigned* clock ) {
  return _parseSingleDigit( s, clock, 'K', 0, 1 );
}

inline int _parseIoDrawer( char** s, unsigned* iod ) {
   return _parseSingleDigit( s, iod, 'I', 0, 0xF );
}

inline int _parseFanAssembly( char** s, unsigned* iod ) {
  return _parseSingleDigit( s, iod, 'H', 0, 2 );
}

inline int _parseFan( char** s, unsigned* iod ) {
  return _parseSingleDigit( s, iod, 'F', 0, 1 );
}


inline int _parseDoubleDecimalDigits( char** s, unsigned* value, char id, unsigned lower, unsigned upper ) {
 
  if ( **s != '-' )
    return -1;

  (*s)++;

  if ( **s != id )
    return -1;

  (*s)++;

  char n0 = **s; (*s)++;
  char n1 = **s; (*s)++;

  if ( ( n0 >= '0' ) && ( n0 <= '9' ) ) {
    *value = (n0 - '0') * 10;
  }
  else {
    return -1;
  }

  if ( ( n1 >= '0' ) && ( n1 <= '9' ) ) {
    *value += ( n1 - '0');
  }
  else {
    return -1;
  }

  return (*value < lower) || (*value > upper);
}

inline int _parseNodeBoard( char** s, unsigned* node ) {
  return _parseDoubleDecimalDigits( s, node, 'N', 0, 15);
} 

inline int _parseComputeCard( char** s, unsigned* compute ) {
  return _parseDoubleDecimalDigits( s, compute, 'J', 0, 31);
}

inline int _parseCore( char** s, unsigned* core ) {
  return _parseDoubleDecimalDigits( s, core, 'C', 0, 16);
}

inline int _parseOpticalModule( char** s, unsigned* optical ) {
  return _parseDoubleDecimalDigits( s, optical, 'O', 0, 35);
}

inline int _parseLinkModule( char** s, unsigned* link ) {
  return _parseDoubleDecimalDigits( s, link, 'U', 0, 8);
}


/*extern*/ inline int bg_string_to_uci( char* str, BG_UniversalComponentIdentifier* uci ) {

  unsigned 
    row = 0, 
    col = 0,
    midplane = 0,
    bps = 0,
    pm = 0,
    clock = 0,
    iod = 0,
    node = 0,
    compute = 0,
    core = 0,
    link = 0,
    optical = 0,
    dca = 0,
    pci = 0,
    assembly = 0,
    fan = 0
    //powerSupply, ioDrawer, clockCard, nodeBoard, computeCard;
  ;

  int rc = 0;

  *uci = 0;

  if ( _uci_pattern_matches( str,  BG_UCI_Pattern_ComputeRack ) ) {
    rc = _parseComputeRack( &str, &row, &col );
    *uci = bg_encodeComputeRackUCI( row, col );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_IoRack ) ) {
    rc = _parseIoRack( &str, &row, &col );
    *uci = bg_encodeIoRackUCI( row, col );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_Midplane ) ) {
    rc = _parseComputeRack( &str, &row, &col );
    rc |= _parseMidPlane( &str, &midplane );
    *uci = bg_encodeMidplaneUCI( row, col, midplane );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_BulkPowerSupplyOnComputeRack  ) ) {
    rc = _parseComputeRack( &str, &row, &col );
    rc |= _parseBulkPowerSupply( &str, &bps );
    *uci = bg_encodeBulkPowerSupplyOnComputeRackUCI( row, col, bps );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_PowerModuleOnComputeRack  ) ) {
    rc = _parseComputeRack( &str, &row, &col );
    rc |= _parseBulkPowerSupply( &str, &bps );
    rc |= _parsePowerModule( &str, &pm );
    *uci = bg_encodePowerModuleOnComputeRackUCI( row, col, bps, pm );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_PowerModuleOnIoRack       ) ) {
    rc = _parseIoRack( &str, &row, &col );
    str += 2; // skip over "-B"
    rc |= _parsePowerModule( &str, &pm );
    *uci = bg_encodePowerModuleOnIoRackUCI( row, col, pm );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_ClockCardOnComputeRack    ) ) {
    rc = _parseComputeRack( &str, &row, &col );
    *uci = bg_encodeClockCardOnComputeRackUCI( row, col );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_ClockCardOnIoRack         ) ) {
    rc = _parseIoRack( &str, &row, &col );
    rc = _parseClockCard( &str, &clock );
    *uci = bg_encodeClockCardOnIoRackUCI( row, col, clock );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_IoDrawerOnComputeRack     ) ) {
    rc = _parseComputeRack( &str, &row, &col );
    rc |= _parseIoDrawer( &str, &iod );
    *uci = bg_encodeIoDrawerOnComputeRackUCI( row, col, iod );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_IoDrawerOnIoRack          ) ) {
    rc = _parseIoRack( &str, &row, &col );
    rc |= _parseIoDrawer( &str, &iod );
    *uci = bg_encodeIoDrawerOnIoRackUCI( row, col, iod );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_NodeBoard                 ) ) {
    rc = _parseComputeRack( &str, &row, &col );
    rc |= _parseMidPlane( &str, &midplane );
    rc |= _parseNodeBoard( &str, &node );
    *uci = bg_encodeNodeBoardUCI( row, col, midplane, node );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_FanAssemblyOnComputeRack       ) ) {
    rc = _parseComputeRack( &str, &row, &col );
    rc |= _parseIoDrawer( &str, &iod );
    rc |= _parseFanAssembly( &str, &assembly );
    *uci = bg_encodeFanAssemblyOnComputeRackUCI( row, col, iod, assembly );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_FanAssemblyOnIoRack       ) ) {
    rc = _parseIoRack( &str, &row, &col );
    rc |= _parseIoDrawer( &str, &iod );
    rc |= _parseFanAssembly( &str, &assembly );
    *uci = bg_encodeFanAssemblyOnIoRackUCI( row, col, iod, assembly );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_FanOnComputeRack       ) ) {
    rc = _parseComputeRack( &str, &row, &col );
    rc |= _parseIoDrawer( &str, &iod );
    rc |= _parseFanAssembly( &str, &assembly );
    rc |= _parseFan( &str, &fan );
    *uci = bg_encodeFanOnComputeRackUCI( row, col, iod, assembly, fan );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_FanOnIoRack       ) ) {
    rc = _parseIoRack( &str, &row, &col );
    rc |= _parseIoDrawer( &str, &iod );
    rc |= _parseFanAssembly( &str, &assembly );
    rc |= _parseFan( &str, &fan );
    *uci = bg_encodeFanOnIoRackUCI( row, col, iod, assembly, fan );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_ComputeCardOnNodeBoard    ) ) {
    rc = _parseComputeRack( &str, &row, &col );
    rc |= _parseMidPlane( &str, &midplane );
    rc |= _parseNodeBoard( &str, &node );
    rc |= _parseComputeCard( &str, &compute );
    *uci = bg_encodeComputeCardOnNodeBoardUCI( row, col, midplane, node, compute );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_ComputeCardOnIoBoardOnIoRack      ) ) {
    rc = _parseIoRack( &str, &row, &col );
    rc |= _parseIoDrawer( &str, &iod );
    rc |= _parseComputeCard( &str, &compute );
    *uci = bg_encodeComputeCardOnIoBoardOnIoRackUCI( row, col, iod, compute );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_ComputeCardOnIoBoardOnComputeRack      ) ) {
    rc = _parseComputeRack( &str, &row, &col );
    rc |= _parseIoDrawer( &str, &iod );
    rc |= _parseComputeCard( &str, &compute );
    *uci = bg_encodeComputeCardOnIoBoardOnComputeRackUCI( row, col, iod, compute );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_ComputeCardCoreOnNodeBoard ) ) {
    rc = _parseComputeRack( &str, &row, &col );
    rc |= _parseMidPlane( &str, &midplane );
    rc |= _parseNodeBoard( &str, &node );
    rc |= _parseComputeCard( &str, &compute );
    rc |= _parseCore( &str, &core );
    *uci = bg_encodeComputeCardCoreOnNodeBoardUCI( row, col, midplane, node, compute, core );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_ComputeCardCoreOnIoBoard   ) ) {
    rc = _parseIoRack( &str, &row, &col );
    rc |= _parseIoDrawer( &str, &iod );
    rc |= _parseComputeCard( &str, &compute );
    rc |= _parseCore( &str, &core );
    *uci = bg_encodeComputeCardCoreOnIoBoardUCI( row, col, iod, compute, core );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_ServiceCard                ) ) {
    rc = _parseComputeRack( &str, &row, &col );
    rc |= _parseMidPlane( &str, &midplane );
    *uci = bg_encodeServiceCardUCI( row, col, midplane );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_LinkModuleOnNodeBoard      ) ) {
    rc = _parseComputeRack( &str, &row, &col );
    rc |= _parseMidPlane( &str, &midplane );
    rc |= _parseNodeBoard( &str, &node );
    rc |= _parseLinkModule( &str, &link );
    *uci = bg_encodeLinkModuleOnNodeBoardUCI( row, col, midplane, node, link );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_LinkModuleOnIoBoard        ) ) {
    rc = _parseIoRack( &str, &row, &col );
    rc |= _parseIoDrawer( &str, &iod );
    rc |= _parseLinkModule( &str, &link );
    *uci = bg_encodeLinkModuleOnIoBoardUCI( row, col, iod, link );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_OpticalModuleOnNodeBoard   ) ) {
    rc = _parseComputeRack( &str, &row, &col );
    rc |= _parseMidPlane( &str, &midplane );
    rc |= _parseNodeBoard( &str, &node );
    rc |= _parseOpticalModule( &str, &optical );
    *uci = bg_encodeOpticalModuleOnNodeBoardUCI( row, col, midplane, node, optical );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_OpticalModuleOnIoBoard     ) ) {
    rc = _parseIoRack( &str, &row, &col );
    rc |= _parseIoDrawer( &str, &iod );
    rc |= _parseOpticalModule( &str, &optical );
    *uci = bg_encodeOpticalModuleOnIoBoardUCI( row, col, iod, optical );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_DcaOnNodeBoard             ) ) {
    rc = _parseComputeRack( &str, &row, &col );
    rc |= _parseMidPlane( &str, &midplane );
    rc |= _parseNodeBoard( &str, &node );
    rc |= _parseDca( &str, &dca );
    *uci = bg_encodeDcaOnNodeBoardUCI( row, col, midplane, node, dca );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_PciAdapterCard             ) ) {
    rc = _parseIoRack( &str, &row, &col );
    rc |= _parseIoDrawer( &str, &iod );
    rc |= _parsePciAdapter( &str, &pci );
    *uci = bg_encodePciAdapterUCI( row, col, iod, pci );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_BulkPowerSupplyOnIoRack    ) ) {
    rc = _parseIoRack(&str, &row, &col );
    *uci = bg_encodeBulkPowerSupplyOnIoRackUCI( row, col );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_DcaOnIoBoard               ) ) {
    rc = _parseIoRack( &str, &row, &col );
    rc |= _parseIoDrawer( &str, &iod );
    rc |= _parseDca( &str, &dca );
    *uci = bg_encodeDcaOnIoBoardUCI( row, col, iod, dca );
  }
  else if ( _uci_pattern_matches( str,  BG_UCI_Pattern_LeakDetectorOnComputeRack ) ) {
    rc = _parseComputeRack( &str, &row, &col );
    *uci = bg_encodeLeakDetectorOnComputeRackUCI( row, col );
  }
  else {
    rc = -1;
  }
  
  return rc;
}

#endif // Add nothing below this line.
