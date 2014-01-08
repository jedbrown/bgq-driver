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

#ifndef   _WU_DCR__H_ // Prevent multiple inclusion
#define   _WU_DCR__H_
  
__BEGIN_DECLS
  
#include <hwi/include/common/bgq_bitnumbers.h>
#include <hwi/include/bqc/dcr_support.h>
  
#define WU_DCR_base    (0x0)  // Base address for all instances
#define WU_DCR_size    (0x03FD)    // Size of instance (largest offset + 1 )
  
#define WU_DCR(reg) _DCR_REG(WU_DCR, reg)
  
// Register:
//   Name:        wac_match0
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Wakeup Address Compare Match register 0
#define WU_DCR__WAC_MATCH0_offset  (0x0380) // Offset of register in instance 
#define WU_DCR__WAC_MATCH0_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address
//   Description: 64 Byte aligned compare address used by address comparator 0
#define WU_DCR__WAC_MATCH0__ADDRESS_width        36
#define WU_DCR__WAC_MATCH0__ADDRESS_position     57
#define WU_DCR__WAC_MATCH0__ADDRESS_get(x)       _BGQ_GET(36,57,x)
#define WU_DCR__WAC_MATCH0__ADDRESS_set(v)       _BGQ_SET(36,57,v)
#define WU_DCR__WAC_MATCH0__ADDRESS_insert(x,v)  _BGQ_INSERT(36,57,x,v)

  
// Register:
//   Name:        wac_mask0
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Wakeup Address Compare Mask register 0
#define WU_DCR__WAC_MASK0_offset  (0x0384) // Offset of register in instance 
#define WU_DCR__WAC_MASK0_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address
//   Description: Defines which bits are examined by address comparator 0. Setting a bit to a 1 causes the bit to be compared to the incoming address. Setting a bit to 0 causes the address comparator to ignore that address bit when performing compares.
#define WU_DCR__WAC_MASK0__ADDRESS_width        36
#define WU_DCR__WAC_MASK0__ADDRESS_position     57
#define WU_DCR__WAC_MASK0__ADDRESS_get(x)       _BGQ_GET(36,57,x)
#define WU_DCR__WAC_MASK0__ADDRESS_set(v)       _BGQ_SET(36,57,v)
#define WU_DCR__WAC_MASK0__ADDRESS_insert(x,v)  _BGQ_INSERT(36,57,x,v)

  
// Register:
//   Name:        wac_match1
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Wakeup Address Compare Match register 1
#define WU_DCR__WAC_MATCH1_offset  (0x0388) // Offset of register in instance 
#define WU_DCR__WAC_MATCH1_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address
//   Description: 64 Byte aligned compare address used by address comparator 1
#define WU_DCR__WAC_MATCH1__ADDRESS_width        36
#define WU_DCR__WAC_MATCH1__ADDRESS_position     57
#define WU_DCR__WAC_MATCH1__ADDRESS_get(x)       _BGQ_GET(36,57,x)
#define WU_DCR__WAC_MATCH1__ADDRESS_set(v)       _BGQ_SET(36,57,v)
#define WU_DCR__WAC_MATCH1__ADDRESS_insert(x,v)  _BGQ_INSERT(36,57,x,v)

  
// Register:
//   Name:        wac_mask1
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Wakeup Address Compare Mask register 1
#define WU_DCR__WAC_MASK1_offset  (0x038C) // Offset of register in instance 
#define WU_DCR__WAC_MASK1_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address
//   Description: Defines which bits are examined by address comparator 1. Setting a bit to a 1 causes the bit to be compared to the incoming address. Setting a bit to 0 causes the address comparator to ignore that address bit when performing compares.
#define WU_DCR__WAC_MASK1__ADDRESS_width        36
#define WU_DCR__WAC_MASK1__ADDRESS_position     57
#define WU_DCR__WAC_MASK1__ADDRESS_get(x)       _BGQ_GET(36,57,x)
#define WU_DCR__WAC_MASK1__ADDRESS_set(v)       _BGQ_SET(36,57,v)
#define WU_DCR__WAC_MASK1__ADDRESS_insert(x,v)  _BGQ_INSERT(36,57,x,v)

  
// Register:
//   Name:        wac_match2
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Wakeup Address Compare Match register 2
#define WU_DCR__WAC_MATCH2_offset  (0x0390) // Offset of register in instance 
#define WU_DCR__WAC_MATCH2_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address
//   Description: 64 Byte aligned compare address used by address comparator 2
#define WU_DCR__WAC_MATCH2__ADDRESS_width        36
#define WU_DCR__WAC_MATCH2__ADDRESS_position     57
#define WU_DCR__WAC_MATCH2__ADDRESS_get(x)       _BGQ_GET(36,57,x)
#define WU_DCR__WAC_MATCH2__ADDRESS_set(v)       _BGQ_SET(36,57,v)
#define WU_DCR__WAC_MATCH2__ADDRESS_insert(x,v)  _BGQ_INSERT(36,57,x,v)

  
// Register:
//   Name:        wac_mask2
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Wakeup Address Compare Mask register 2
#define WU_DCR__WAC_MASK2_offset  (0x0394) // Offset of register in instance 
#define WU_DCR__WAC_MASK2_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address
//   Description: Defines which bits are examined by address comparator 2. Setting a bit to a 1 causes the bit to be compared to the incoming address. Setting a bit to 0 causes the address comparator to ignore that address bit when performing compares.
#define WU_DCR__WAC_MASK2__ADDRESS_width        36
#define WU_DCR__WAC_MASK2__ADDRESS_position     57
#define WU_DCR__WAC_MASK2__ADDRESS_get(x)       _BGQ_GET(36,57,x)
#define WU_DCR__WAC_MASK2__ADDRESS_set(v)       _BGQ_SET(36,57,v)
#define WU_DCR__WAC_MASK2__ADDRESS_insert(x,v)  _BGQ_INSERT(36,57,x,v)

  
// Register:
//   Name:        wac_match3
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Wakeup Address Compare Match register 3
#define WU_DCR__WAC_MATCH3_offset  (0x0398) // Offset of register in instance 
#define WU_DCR__WAC_MATCH3_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address
//   Description: 64 Byte aligned compare address used by address comparator 3
#define WU_DCR__WAC_MATCH3__ADDRESS_width        36
#define WU_DCR__WAC_MATCH3__ADDRESS_position     57
#define WU_DCR__WAC_MATCH3__ADDRESS_get(x)       _BGQ_GET(36,57,x)
#define WU_DCR__WAC_MATCH3__ADDRESS_set(v)       _BGQ_SET(36,57,v)
#define WU_DCR__WAC_MATCH3__ADDRESS_insert(x,v)  _BGQ_INSERT(36,57,x,v)

  
// Register:
//   Name:        wac_mask3
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Wakeup Address Compare Mask register 3
#define WU_DCR__WAC_MASK3_offset  (0x039C) // Offset of register in instance 
#define WU_DCR__WAC_MASK3_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address
//   Description: Defines which bits are examined by address comparator 3. Setting a bit to a 1 causes the bit to be compared to the incoming address. Setting a bit to 0 causes the address comparator to ignore that address bit when performing compares.
#define WU_DCR__WAC_MASK3__ADDRESS_width        36
#define WU_DCR__WAC_MASK3__ADDRESS_position     57
#define WU_DCR__WAC_MASK3__ADDRESS_get(x)       _BGQ_GET(36,57,x)
#define WU_DCR__WAC_MASK3__ADDRESS_set(v)       _BGQ_SET(36,57,v)
#define WU_DCR__WAC_MASK3__ADDRESS_insert(x,v)  _BGQ_INSERT(36,57,x,v)

  
// Register:
//   Name:        wac_match4
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Wakeup Address Compare Match register 4
#define WU_DCR__WAC_MATCH4_offset  (0x03A0) // Offset of register in instance 
#define WU_DCR__WAC_MATCH4_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address
//   Description: 64 Byte aligned compare address used by address comparator 4
#define WU_DCR__WAC_MATCH4__ADDRESS_width        36
#define WU_DCR__WAC_MATCH4__ADDRESS_position     57
#define WU_DCR__WAC_MATCH4__ADDRESS_get(x)       _BGQ_GET(36,57,x)
#define WU_DCR__WAC_MATCH4__ADDRESS_set(v)       _BGQ_SET(36,57,v)
#define WU_DCR__WAC_MATCH4__ADDRESS_insert(x,v)  _BGQ_INSERT(36,57,x,v)

  
// Register:
//   Name:        wac_mask4
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Wakeup Address Compare Mask register 4
#define WU_DCR__WAC_MASK4_offset  (0x03A4) // Offset of register in instance 
#define WU_DCR__WAC_MASK4_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address
//   Description: Defines which bits are examined by address comparator 4. Setting a bit to a 1 causes the bit to be compared to the incoming address. Setting a bit to 0 causes the address comparator to ignore that address bit when performing compares.
#define WU_DCR__WAC_MASK4__ADDRESS_width        36
#define WU_DCR__WAC_MASK4__ADDRESS_position     57
#define WU_DCR__WAC_MASK4__ADDRESS_get(x)       _BGQ_GET(36,57,x)
#define WU_DCR__WAC_MASK4__ADDRESS_set(v)       _BGQ_SET(36,57,v)
#define WU_DCR__WAC_MASK4__ADDRESS_insert(x,v)  _BGQ_INSERT(36,57,x,v)

  
// Register:
//   Name:        wac_match5
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Wakeup Address Compare Match register 5
#define WU_DCR__WAC_MATCH5_offset  (0x03A8) // Offset of register in instance 
#define WU_DCR__WAC_MATCH5_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address
//   Description: 64 Byte aligned compare address used by address comparator 5
#define WU_DCR__WAC_MATCH5__ADDRESS_width        36
#define WU_DCR__WAC_MATCH5__ADDRESS_position     57
#define WU_DCR__WAC_MATCH5__ADDRESS_get(x)       _BGQ_GET(36,57,x)
#define WU_DCR__WAC_MATCH5__ADDRESS_set(v)       _BGQ_SET(36,57,v)
#define WU_DCR__WAC_MATCH5__ADDRESS_insert(x,v)  _BGQ_INSERT(36,57,x,v)

  
// Register:
//   Name:        wac_mask5
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Wakeup Address Compare Mask register 5
#define WU_DCR__WAC_MASK5_offset  (0x03AC) // Offset of register in instance 
#define WU_DCR__WAC_MASK5_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address
//   Description: Defines which bits are examined by address comparator 5. Setting a bit to a 1 causes the bit to be compared to the incoming address. Setting a bit to 0 causes the address comparator to ignore that address bit when performing compares.
#define WU_DCR__WAC_MASK5__ADDRESS_width        36
#define WU_DCR__WAC_MASK5__ADDRESS_position     57
#define WU_DCR__WAC_MASK5__ADDRESS_get(x)       _BGQ_GET(36,57,x)
#define WU_DCR__WAC_MASK5__ADDRESS_set(v)       _BGQ_SET(36,57,v)
#define WU_DCR__WAC_MASK5__ADDRESS_insert(x,v)  _BGQ_INSERT(36,57,x,v)

  
// Register:
//   Name:        wac_match6
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Wakeup Address Compare Match register 6
#define WU_DCR__WAC_MATCH6_offset  (0x03B0) // Offset of register in instance 
#define WU_DCR__WAC_MATCH6_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address
//   Description: 64 Byte aligned compare address used by address comparator 6
#define WU_DCR__WAC_MATCH6__ADDRESS_width        36
#define WU_DCR__WAC_MATCH6__ADDRESS_position     57
#define WU_DCR__WAC_MATCH6__ADDRESS_get(x)       _BGQ_GET(36,57,x)
#define WU_DCR__WAC_MATCH6__ADDRESS_set(v)       _BGQ_SET(36,57,v)
#define WU_DCR__WAC_MATCH6__ADDRESS_insert(x,v)  _BGQ_INSERT(36,57,x,v)

  
// Register:
//   Name:        wac_mask6
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Wakeup Address Compare Mask register 6
#define WU_DCR__WAC_MASK6_offset  (0x03B4) // Offset of register in instance 
#define WU_DCR__WAC_MASK6_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address
//   Description: Defines which bits are examined by address comparator 6. Setting a bit to a 1 causes the bit to be compared to the incoming address. Setting a bit to 0 causes the address comparator to ignore that address bit when performing compares.
#define WU_DCR__WAC_MASK6__ADDRESS_width        36
#define WU_DCR__WAC_MASK6__ADDRESS_position     57
#define WU_DCR__WAC_MASK6__ADDRESS_get(x)       _BGQ_GET(36,57,x)
#define WU_DCR__WAC_MASK6__ADDRESS_set(v)       _BGQ_SET(36,57,v)
#define WU_DCR__WAC_MASK6__ADDRESS_insert(x,v)  _BGQ_INSERT(36,57,x,v)

  
// Register:
//   Name:        wac_match7
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Wakeup Address Compare match register 7
#define WU_DCR__WAC_MATCH7_offset  (0x03B8) // Offset of register in instance 
#define WU_DCR__WAC_MATCH7_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address
//   Description: 64 Byte aligned compare address used by address comparator 7
#define WU_DCR__WAC_MATCH7__ADDRESS_width        36
#define WU_DCR__WAC_MATCH7__ADDRESS_position     57
#define WU_DCR__WAC_MATCH7__ADDRESS_get(x)       _BGQ_GET(36,57,x)
#define WU_DCR__WAC_MATCH7__ADDRESS_set(v)       _BGQ_SET(36,57,v)
#define WU_DCR__WAC_MATCH7__ADDRESS_insert(x,v)  _BGQ_INSERT(36,57,x,v)

  
// Register:
//   Name:        wac_mask7
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Wakeup Address Compare mask register 7
#define WU_DCR__WAC_MASK7_offset  (0x03BC) // Offset of register in instance 
#define WU_DCR__WAC_MASK7_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address
//   Description: Defines which bits are examined by address comparator 7. Setting a bit to a 1 causes the bit to be compared to the incoming address. Setting a bit to 0 causes the address comparator to ignore that address bit when performing compares.
#define WU_DCR__WAC_MASK7__ADDRESS_width        36
#define WU_DCR__WAC_MASK7__ADDRESS_position     57
#define WU_DCR__WAC_MASK7__ADDRESS_get(x)       _BGQ_GET(36,57,x)
#define WU_DCR__WAC_MASK7__ADDRESS_set(v)       _BGQ_SET(36,57,v)
#define WU_DCR__WAC_MASK7__ADDRESS_insert(x,v)  _BGQ_INSERT(36,57,x,v)

  
// Register:
//   Name:        wac_match8
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Wakeup Address Compare match register 8
#define WU_DCR__WAC_MATCH8_offset  (0x03C0) // Offset of register in instance 
#define WU_DCR__WAC_MATCH8_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address
//   Description: 64 Byte aligned compare address used by address comparator 8
#define WU_DCR__WAC_MATCH8__ADDRESS_width        36
#define WU_DCR__WAC_MATCH8__ADDRESS_position     57
#define WU_DCR__WAC_MATCH8__ADDRESS_get(x)       _BGQ_GET(36,57,x)
#define WU_DCR__WAC_MATCH8__ADDRESS_set(v)       _BGQ_SET(36,57,v)
#define WU_DCR__WAC_MATCH8__ADDRESS_insert(x,v)  _BGQ_INSERT(36,57,x,v)

  
// Register:
//   Name:        wac_mask8
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Wakeup Address Compare mask register 8
#define WU_DCR__WAC_MASK8_offset  (0x03C4) // Offset of register in instance 
#define WU_DCR__WAC_MASK8_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address
//   Description: Defines which bits are examined by address comparator 8. Setting a bit to a 1 causes the bit to be compared to the incoming address. Setting a bit to 0 causes the address comparator to ignore that address bit when performing compares.
#define WU_DCR__WAC_MASK8__ADDRESS_width        36
#define WU_DCR__WAC_MASK8__ADDRESS_position     57
#define WU_DCR__WAC_MASK8__ADDRESS_get(x)       _BGQ_GET(36,57,x)
#define WU_DCR__WAC_MASK8__ADDRESS_set(v)       _BGQ_SET(36,57,v)
#define WU_DCR__WAC_MASK8__ADDRESS_insert(x,v)  _BGQ_INSERT(36,57,x,v)

  
// Register:
//   Name:        wac_match9
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Wakeup Address Compare match register 9
#define WU_DCR__WAC_MATCH9_offset  (0x03C8) // Offset of register in instance 
#define WU_DCR__WAC_MATCH9_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address
//   Description: 64 Byte aligned compare address used by address comparator 9
#define WU_DCR__WAC_MATCH9__ADDRESS_width        36
#define WU_DCR__WAC_MATCH9__ADDRESS_position     57
#define WU_DCR__WAC_MATCH9__ADDRESS_get(x)       _BGQ_GET(36,57,x)
#define WU_DCR__WAC_MATCH9__ADDRESS_set(v)       _BGQ_SET(36,57,v)
#define WU_DCR__WAC_MATCH9__ADDRESS_insert(x,v)  _BGQ_INSERT(36,57,x,v)

  
// Register:
//   Name:        wac_mask9
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Wakeup Address Compare mask register 9
#define WU_DCR__WAC_MASK9_offset  (0x03CC) // Offset of register in instance 
#define WU_DCR__WAC_MASK9_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address
//   Description: Defines which bits are examined by address comparator 9. Setting a bit to a 1 causes the bit to be compared to the incoming address. Setting a bit to 0 causes the address comparator to ignore that address bit when performing compares.
#define WU_DCR__WAC_MASK9__ADDRESS_width        36
#define WU_DCR__WAC_MASK9__ADDRESS_position     57
#define WU_DCR__WAC_MASK9__ADDRESS_get(x)       _BGQ_GET(36,57,x)
#define WU_DCR__WAC_MASK9__ADDRESS_set(v)       _BGQ_SET(36,57,v)
#define WU_DCR__WAC_MASK9__ADDRESS_insert(x,v)  _BGQ_INSERT(36,57,x,v)

  
// Register:
//   Name:        wac_match10
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Wakeup Address Compare match register 10
#define WU_DCR__WAC_MATCH10_offset  (0x03D0) // Offset of register in instance 
#define WU_DCR__WAC_MATCH10_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address
//   Description: 64 Byte aligned compare address used by address comparator 10
#define WU_DCR__WAC_MATCH10__ADDRESS_width        36
#define WU_DCR__WAC_MATCH10__ADDRESS_position     57
#define WU_DCR__WAC_MATCH10__ADDRESS_get(x)       _BGQ_GET(36,57,x)
#define WU_DCR__WAC_MATCH10__ADDRESS_set(v)       _BGQ_SET(36,57,v)
#define WU_DCR__WAC_MATCH10__ADDRESS_insert(x,v)  _BGQ_INSERT(36,57,x,v)

  
// Register:
//   Name:        wac_mask10
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Wakeup Address Compare mask register 10
#define WU_DCR__WAC_MASK10_offset  (0x03D4) // Offset of register in instance 
#define WU_DCR__WAC_MASK10_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address
//   Description: Defines which bits are examined by address comparator 10. Setting a bit to a 1 causes the bit to be compared to the incoming address. Setting a bit to 0 causes the address comparator to ignore that address bit when performing compares.
#define WU_DCR__WAC_MASK10__ADDRESS_width        36
#define WU_DCR__WAC_MASK10__ADDRESS_position     57
#define WU_DCR__WAC_MASK10__ADDRESS_get(x)       _BGQ_GET(36,57,x)
#define WU_DCR__WAC_MASK10__ADDRESS_set(v)       _BGQ_SET(36,57,v)
#define WU_DCR__WAC_MASK10__ADDRESS_insert(x,v)  _BGQ_INSERT(36,57,x,v)

  
// Register:
//   Name:        wac_match11
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Wakeup Address Compare match register 11
#define WU_DCR__WAC_MATCH11_offset  (0x03D8) // Offset of register in instance 
#define WU_DCR__WAC_MATCH11_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address
//   Description: 64 Byte aligned compare address used by address comparator 11
#define WU_DCR__WAC_MATCH11__ADDRESS_width        36
#define WU_DCR__WAC_MATCH11__ADDRESS_position     57
#define WU_DCR__WAC_MATCH11__ADDRESS_get(x)       _BGQ_GET(36,57,x)
#define WU_DCR__WAC_MATCH11__ADDRESS_set(v)       _BGQ_SET(36,57,v)
#define WU_DCR__WAC_MATCH11__ADDRESS_insert(x,v)  _BGQ_INSERT(36,57,x,v)

  
// Register:
//   Name:        wac_mask11
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Wakeup Address Compare mask register 11
#define WU_DCR__WAC_MASK11_offset  (0x03DC) // Offset of register in instance 
#define WU_DCR__WAC_MASK11_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address
//   Description: Defines which bits are examined by address comparator 11. Setting a bit to a 1 causes the bit to be compared to the incoming address. Setting a bit to 0 causes the address comparator to ignore that address bit when performing compares.
#define WU_DCR__WAC_MASK11__ADDRESS_width        36
#define WU_DCR__WAC_MASK11__ADDRESS_position     57
#define WU_DCR__WAC_MASK11__ADDRESS_get(x)       _BGQ_GET(36,57,x)
#define WU_DCR__WAC_MASK11__ADDRESS_set(v)       _BGQ_SET(36,57,v)
#define WU_DCR__WAC_MASK11__ADDRESS_insert(x,v)  _BGQ_INSERT(36,57,x,v)

  
// Register:
//   Name:        thread0_wu_event_set
//   Type:        Read/Write External
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Thread 0 Write-one-to-set wakup event configuration/status register.
#define WU_DCR__THREAD0_WU_EVENT_SET_offset  (0x03E0) // Offset of register in instance 
#define WU_DCR__THREAD0_WU_EVENT_SET_range  (0x1) // Range of external address space
#define WU_DCR__THREAD0_WU_EVENT_SET_reset  UNSIGNED64(0x0000000000000000)

//   Field:       wac_status
//   Description: Indicates that address comparators have detected a match, This bit remains set until cleared by software via the thread0_wu_event_clear register (W1S).
#define WU_DCR__THREAD0_WU_EVENT_SET__WAC_STATUS_width        12
#define WU_DCR__THREAD0_WU_EVENT_SET__WAC_STATUS_position     11
#define WU_DCR__THREAD0_WU_EVENT_SET__WAC_STATUS_get(x)       _BGQ_GET(12,11,x)
#define WU_DCR__THREAD0_WU_EVENT_SET__WAC_STATUS_set(v)       _BGQ_SET(12,11,v)
#define WU_DCR__THREAD0_WU_EVENT_SET__WAC_STATUS_insert(x,v)  _BGQ_INSERT(12,11,x,v)

//   Field:       t0_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 0. This bit reflects the equivalent bit in the BIC (R0).
#define WU_DCR__THREAD0_WU_EVENT_SET__T0_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD0_WU_EVENT_SET__T0_0_15_C2C_WU_STATUS_position     12
#define WU_DCR__THREAD0_WU_EVENT_SET__T0_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,12,x)
#define WU_DCR__THREAD0_WU_EVENT_SET__T0_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,12,v)
#define WU_DCR__THREAD0_WU_EVENT_SET__T0_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,12,x,v)

//   Field:       t0_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 0. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD0_WU_EVENT_SET__T0_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD0_WU_EVENT_SET__T0_16_C2C_WU_STATUS_position     13
#define WU_DCR__THREAD0_WU_EVENT_SET__T0_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,13,x)
#define WU_DCR__THREAD0_WU_EVENT_SET__T0_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,13,v)
#define WU_DCR__THREAD0_WU_EVENT_SET__T0_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,13,x,v)

//   Field:       t1_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 1. This bit reflects the equivalent bit in the BIC (RO)
#define WU_DCR__THREAD0_WU_EVENT_SET__T1_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD0_WU_EVENT_SET__T1_0_15_C2C_WU_STATUS_position     14
#define WU_DCR__THREAD0_WU_EVENT_SET__T1_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,14,x)
#define WU_DCR__THREAD0_WU_EVENT_SET__T1_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,14,v)
#define WU_DCR__THREAD0_WU_EVENT_SET__T1_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,14,x,v)

//   Field:       t1_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 1. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD0_WU_EVENT_SET__T1_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD0_WU_EVENT_SET__T1_16_C2C_WU_STATUS_position     15
#define WU_DCR__THREAD0_WU_EVENT_SET__T1_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,15,x)
#define WU_DCR__THREAD0_WU_EVENT_SET__T1_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,15,v)
#define WU_DCR__THREAD0_WU_EVENT_SET__T1_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,15,x,v)

//   Field:       t2_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 2. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD0_WU_EVENT_SET__T2_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD0_WU_EVENT_SET__T2_0_15_C2C_WU_STATUS_position     16
#define WU_DCR__THREAD0_WU_EVENT_SET__T2_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,16,x)
#define WU_DCR__THREAD0_WU_EVENT_SET__T2_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,16,v)
#define WU_DCR__THREAD0_WU_EVENT_SET__T2_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,16,x,v)

//   Field:       t2_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 2. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD0_WU_EVENT_SET__T2_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD0_WU_EVENT_SET__T2_16_C2C_WU_STATUS_position     17
#define WU_DCR__THREAD0_WU_EVENT_SET__T2_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,17,x)
#define WU_DCR__THREAD0_WU_EVENT_SET__T2_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,17,v)
#define WU_DCR__THREAD0_WU_EVENT_SET__T2_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,17,x,v)

//   Field:       t3_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 3. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD0_WU_EVENT_SET__T3_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD0_WU_EVENT_SET__T3_0_15_C2C_WU_STATUS_position     18
#define WU_DCR__THREAD0_WU_EVENT_SET__T3_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,18,x)
#define WU_DCR__THREAD0_WU_EVENT_SET__T3_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,18,v)
#define WU_DCR__THREAD0_WU_EVENT_SET__T3_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,18,x,v)

//   Field:       t3_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 3. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD0_WU_EVENT_SET__T3_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD0_WU_EVENT_SET__T3_16_C2C_WU_STATUS_position     19
#define WU_DCR__THREAD0_WU_EVENT_SET__T3_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,19,x)
#define WU_DCR__THREAD0_WU_EVENT_SET__T3_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,19,v)
#define WU_DCR__THREAD0_WU_EVENT_SET__T3_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,19,x,v)

//   Field:       mu_wu_status
//   Description: Indicates wakeup request from Message Unit. This field reflects the incoming wakeup request from the MU, and is cleared in the MU (RO).
#define WU_DCR__THREAD0_WU_EVENT_SET__MU_WU_STATUS_width        4
#define WU_DCR__THREAD0_WU_EVENT_SET__MU_WU_STATUS_position     23
#define WU_DCR__THREAD0_WU_EVENT_SET__MU_WU_STATUS_get(x)       _BGQ_GET(4,23,x)
#define WU_DCR__THREAD0_WU_EVENT_SET__MU_WU_STATUS_set(v)       _BGQ_SET(4,23,v)
#define WU_DCR__THREAD0_WU_EVENT_SET__MU_WU_STATUS_insert(x,v)  _BGQ_INSERT(4,23,x,v)

//   Field:       gea_wu_status
//   Description: Indicates wakeup request from GEA. This field reflects the incoming wakeup request from the GEA, and is cleared in the GEA (RO).
#define WU_DCR__THREAD0_WU_EVENT_SET__GEA_WU_STATUS_width        4
#define WU_DCR__THREAD0_WU_EVENT_SET__GEA_WU_STATUS_position     27
#define WU_DCR__THREAD0_WU_EVENT_SET__GEA_WU_STATUS_get(x)       _BGQ_GET(4,27,x)
#define WU_DCR__THREAD0_WU_EVENT_SET__GEA_WU_STATUS_set(v)       _BGQ_SET(4,27,v)
#define WU_DCR__THREAD0_WU_EVENT_SET__GEA_WU_STATUS_insert(x,v)  _BGQ_INSERT(4,27,x,v)

//   Field:       sw_wu_status
//   Description: Used to generate wakeup events via software (W1S).
#define WU_DCR__THREAD0_WU_EVENT_SET__SW_WU_STATUS_width        4
#define WU_DCR__THREAD0_WU_EVENT_SET__SW_WU_STATUS_position     31
#define WU_DCR__THREAD0_WU_EVENT_SET__SW_WU_STATUS_get(x)       _BGQ_GET(4,31,x)
#define WU_DCR__THREAD0_WU_EVENT_SET__SW_WU_STATUS_set(v)       _BGQ_SET(4,31,v)
#define WU_DCR__THREAD0_WU_EVENT_SET__SW_WU_STATUS_insert(x,v)  _BGQ_INSERT(4,31,x,v)

//   Field:       wac_en
//   Description: Enables wakeup of Thread 0 on each wakeup address compare match (W1S).
#define WU_DCR__THREAD0_WU_EVENT_SET__WAC_EN_width        12
#define WU_DCR__THREAD0_WU_EVENT_SET__WAC_EN_position     43
#define WU_DCR__THREAD0_WU_EVENT_SET__WAC_EN_get(x)       _BGQ_GET(12,43,x)
#define WU_DCR__THREAD0_WU_EVENT_SET__WAC_EN_set(v)       _BGQ_SET(12,43,v)
#define WU_DCR__THREAD0_WU_EVENT_SET__WAC_EN_insert(x,v)  _BGQ_INSERT(12,43,x,v)

//   Field:       t0_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 0 on c2c interrupt from cores 0-15, targeted to thread 0 (W1S).
#define WU_DCR__THREAD0_WU_EVENT_SET__T0_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD0_WU_EVENT_SET__T0_0_15_C2C_WU_EN_position     44
#define WU_DCR__THREAD0_WU_EVENT_SET__T0_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,44,x)
#define WU_DCR__THREAD0_WU_EVENT_SET__T0_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,44,v)
#define WU_DCR__THREAD0_WU_EVENT_SET__T0_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,44,x,v)

//   Field:       t0_16_c2c_wu_en
//   Description: Enables wakeup of Thread 0 on c2c interrupt from core 16, targeted to thread 0 (W1S).
#define WU_DCR__THREAD0_WU_EVENT_SET__T0_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD0_WU_EVENT_SET__T0_16_C2C_WU_EN_position     45
#define WU_DCR__THREAD0_WU_EVENT_SET__T0_16_C2C_WU_EN_get(x)       _BGQ_GET(1,45,x)
#define WU_DCR__THREAD0_WU_EVENT_SET__T0_16_C2C_WU_EN_set(v)       _BGQ_SET(1,45,v)
#define WU_DCR__THREAD0_WU_EVENT_SET__T0_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,45,x,v)

//   Field:       t1_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 0 on c2c interrupt from cores 0-15, targeted to thread 1 (W1S).
#define WU_DCR__THREAD0_WU_EVENT_SET__T1_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD0_WU_EVENT_SET__T1_0_15_C2C_WU_EN_position     46
#define WU_DCR__THREAD0_WU_EVENT_SET__T1_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,46,x)
#define WU_DCR__THREAD0_WU_EVENT_SET__T1_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,46,v)
#define WU_DCR__THREAD0_WU_EVENT_SET__T1_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,46,x,v)

//   Field:       t1_16_c2c_wu_en
//   Description: Enables wakeup of Thread 0 on c2c interrupt from core 16, targeted to thread 1 (W1S).
#define WU_DCR__THREAD0_WU_EVENT_SET__T1_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD0_WU_EVENT_SET__T1_16_C2C_WU_EN_position     47
#define WU_DCR__THREAD0_WU_EVENT_SET__T1_16_C2C_WU_EN_get(x)       _BGQ_GET(1,47,x)
#define WU_DCR__THREAD0_WU_EVENT_SET__T1_16_C2C_WU_EN_set(v)       _BGQ_SET(1,47,v)
#define WU_DCR__THREAD0_WU_EVENT_SET__T1_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,47,x,v)

//   Field:       t2_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 0 on c2c interrupt from cores 0-15, targeted to thread 2 (W1S).
#define WU_DCR__THREAD0_WU_EVENT_SET__T2_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD0_WU_EVENT_SET__T2_0_15_C2C_WU_EN_position     48
#define WU_DCR__THREAD0_WU_EVENT_SET__T2_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,48,x)
#define WU_DCR__THREAD0_WU_EVENT_SET__T2_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,48,v)
#define WU_DCR__THREAD0_WU_EVENT_SET__T2_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,48,x,v)

//   Field:       t2_16_c2c_wu_en
//   Description: Enables wakeup of Thread 0 on c2c interrupt from core 16, targeted to thread 2 (W1S).
#define WU_DCR__THREAD0_WU_EVENT_SET__T2_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD0_WU_EVENT_SET__T2_16_C2C_WU_EN_position     49
#define WU_DCR__THREAD0_WU_EVENT_SET__T2_16_C2C_WU_EN_get(x)       _BGQ_GET(1,49,x)
#define WU_DCR__THREAD0_WU_EVENT_SET__T2_16_C2C_WU_EN_set(v)       _BGQ_SET(1,49,v)
#define WU_DCR__THREAD0_WU_EVENT_SET__T2_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,49,x,v)

//   Field:       t3_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 0 on c2c interrupt from cores 0-15, targeted to thread 3 (W1S).
#define WU_DCR__THREAD0_WU_EVENT_SET__T3_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD0_WU_EVENT_SET__T3_0_15_C2C_WU_EN_position     50
#define WU_DCR__THREAD0_WU_EVENT_SET__T3_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,50,x)
#define WU_DCR__THREAD0_WU_EVENT_SET__T3_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,50,v)
#define WU_DCR__THREAD0_WU_EVENT_SET__T3_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,50,x,v)

//   Field:       t3_16_c2c_wu_en
//   Description: Enables wakeup of Thread 0 on c2c interrupt from core 16, targeted to thread 3 (W1S).
#define WU_DCR__THREAD0_WU_EVENT_SET__T3_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD0_WU_EVENT_SET__T3_16_C2C_WU_EN_position     51
#define WU_DCR__THREAD0_WU_EVENT_SET__T3_16_C2C_WU_EN_get(x)       _BGQ_GET(1,51,x)
#define WU_DCR__THREAD0_WU_EVENT_SET__T3_16_C2C_WU_EN_set(v)       _BGQ_SET(1,51,v)
#define WU_DCR__THREAD0_WU_EVENT_SET__T3_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,51,x,v)

//   Field:       mu_wu_en
//   Description: Enables wakeup of Thread 0 from Message Unit (W1S).
#define WU_DCR__THREAD0_WU_EVENT_SET__MU_WU_EN_width        4
#define WU_DCR__THREAD0_WU_EVENT_SET__MU_WU_EN_position     55
#define WU_DCR__THREAD0_WU_EVENT_SET__MU_WU_EN_get(x)       _BGQ_GET(4,55,x)
#define WU_DCR__THREAD0_WU_EVENT_SET__MU_WU_EN_set(v)       _BGQ_SET(4,55,v)
#define WU_DCR__THREAD0_WU_EVENT_SET__MU_WU_EN_insert(x,v)  _BGQ_INSERT(4,55,x,v)

//   Field:       gea_wu_en
//   Description: Enables wakeup of Thread 0 from GEA (W1S).
#define WU_DCR__THREAD0_WU_EVENT_SET__GEA_WU_EN_width        4
#define WU_DCR__THREAD0_WU_EVENT_SET__GEA_WU_EN_position     59
#define WU_DCR__THREAD0_WU_EVENT_SET__GEA_WU_EN_get(x)       _BGQ_GET(4,59,x)
#define WU_DCR__THREAD0_WU_EVENT_SET__GEA_WU_EN_set(v)       _BGQ_SET(4,59,v)
#define WU_DCR__THREAD0_WU_EVENT_SET__GEA_WU_EN_insert(x,v)  _BGQ_INSERT(4,59,x,v)

//   Field:       sw_wu_en
//   Description: Enables wakeup of Thread 0 when corresponding bits of sw_wu_status in this register are set (W1S).
#define WU_DCR__THREAD0_WU_EVENT_SET__SW_WU_EN_width        4
#define WU_DCR__THREAD0_WU_EVENT_SET__SW_WU_EN_position     63
#define WU_DCR__THREAD0_WU_EVENT_SET__SW_WU_EN_get(x)       _BGQ_GET(4,63,x)
#define WU_DCR__THREAD0_WU_EVENT_SET__SW_WU_EN_set(v)       _BGQ_SET(4,63,v)
#define WU_DCR__THREAD0_WU_EVENT_SET__SW_WU_EN_insert(x,v)  _BGQ_INSERT(4,63,x,v)

  
// Register:
//   Name:        thread0_wu_event_clr
//   Type:        Read/Write External
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Thread 0 Write-one-to-clear wakup event configuration/status register.
#define WU_DCR__THREAD0_WU_EVENT_CLR_offset  (0x03E4) // Offset of register in instance 
#define WU_DCR__THREAD0_WU_EVENT_CLR_range  (0x1) // Range of external address space
#define WU_DCR__THREAD0_WU_EVENT_CLR_reset  UNSIGNED64(0x0000000000000000)

//   Field:       wac_status
//   Description: Indicates that address comparators have detected a match, This bit remains set until cleared by software via the thread0_wu_event_clear register (W1C). 
#define WU_DCR__THREAD0_WU_EVENT_CLR__WAC_STATUS_width        12
#define WU_DCR__THREAD0_WU_EVENT_CLR__WAC_STATUS_position     11
#define WU_DCR__THREAD0_WU_EVENT_CLR__WAC_STATUS_get(x)       _BGQ_GET(12,11,x)
#define WU_DCR__THREAD0_WU_EVENT_CLR__WAC_STATUS_set(v)       _BGQ_SET(12,11,v)
#define WU_DCR__THREAD0_WU_EVENT_CLR__WAC_STATUS_insert(x,v)  _BGQ_INSERT(12,11,x,v)

//   Field:       t0_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 0. This bit reflects the equivalent bit in the BIC (R0).
#define WU_DCR__THREAD0_WU_EVENT_CLR__T0_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD0_WU_EVENT_CLR__T0_0_15_C2C_WU_STATUS_position     12
#define WU_DCR__THREAD0_WU_EVENT_CLR__T0_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,12,x)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T0_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,12,v)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T0_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,12,x,v)

//   Field:       t0_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 0. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD0_WU_EVENT_CLR__T0_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD0_WU_EVENT_CLR__T0_16_C2C_WU_STATUS_position     13
#define WU_DCR__THREAD0_WU_EVENT_CLR__T0_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,13,x)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T0_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,13,v)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T0_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,13,x,v)

//   Field:       t1_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 1. This bit reflects the equivalent bit in the BIC (RO)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T1_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD0_WU_EVENT_CLR__T1_0_15_C2C_WU_STATUS_position     14
#define WU_DCR__THREAD0_WU_EVENT_CLR__T1_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,14,x)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T1_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,14,v)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T1_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,14,x,v)

//   Field:       t1_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 1. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD0_WU_EVENT_CLR__T1_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD0_WU_EVENT_CLR__T1_16_C2C_WU_STATUS_position     15
#define WU_DCR__THREAD0_WU_EVENT_CLR__T1_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,15,x)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T1_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,15,v)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T1_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,15,x,v)

//   Field:       t2_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 2. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD0_WU_EVENT_CLR__T2_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD0_WU_EVENT_CLR__T2_0_15_C2C_WU_STATUS_position     16
#define WU_DCR__THREAD0_WU_EVENT_CLR__T2_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,16,x)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T2_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,16,v)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T2_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,16,x,v)

//   Field:       t2_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 2. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD0_WU_EVENT_CLR__T2_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD0_WU_EVENT_CLR__T2_16_C2C_WU_STATUS_position     17
#define WU_DCR__THREAD0_WU_EVENT_CLR__T2_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,17,x)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T2_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,17,v)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T2_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,17,x,v)

//   Field:       t3_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 3. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD0_WU_EVENT_CLR__T3_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD0_WU_EVENT_CLR__T3_0_15_C2C_WU_STATUS_position     18
#define WU_DCR__THREAD0_WU_EVENT_CLR__T3_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,18,x)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T3_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,18,v)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T3_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,18,x,v)

//   Field:       t3_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 3. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD0_WU_EVENT_CLR__T3_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD0_WU_EVENT_CLR__T3_16_C2C_WU_STATUS_position     19
#define WU_DCR__THREAD0_WU_EVENT_CLR__T3_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,19,x)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T3_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,19,v)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T3_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,19,x,v)

//   Field:       mu_wu_status
//   Description: Indicates wakeup request from Message Unit. This field reflects the incoming wakeup request from the MU, and is cleared in the MU (RO).
#define WU_DCR__THREAD0_WU_EVENT_CLR__MU_WU_STATUS_width        4
#define WU_DCR__THREAD0_WU_EVENT_CLR__MU_WU_STATUS_position     23
#define WU_DCR__THREAD0_WU_EVENT_CLR__MU_WU_STATUS_get(x)       _BGQ_GET(4,23,x)
#define WU_DCR__THREAD0_WU_EVENT_CLR__MU_WU_STATUS_set(v)       _BGQ_SET(4,23,v)
#define WU_DCR__THREAD0_WU_EVENT_CLR__MU_WU_STATUS_insert(x,v)  _BGQ_INSERT(4,23,x,v)

//   Field:       gea_wu_status
//   Description: Indicates wakeup request from GEA. This field reflects the incoming wakeup request from the GEA, and is cleared in the GEA (RO).
#define WU_DCR__THREAD0_WU_EVENT_CLR__GEA_WU_STATUS_width        4
#define WU_DCR__THREAD0_WU_EVENT_CLR__GEA_WU_STATUS_position     27
#define WU_DCR__THREAD0_WU_EVENT_CLR__GEA_WU_STATUS_get(x)       _BGQ_GET(4,27,x)
#define WU_DCR__THREAD0_WU_EVENT_CLR__GEA_WU_STATUS_set(v)       _BGQ_SET(4,27,v)
#define WU_DCR__THREAD0_WU_EVENT_CLR__GEA_WU_STATUS_insert(x,v)  _BGQ_INSERT(4,27,x,v)

//   Field:       sw_wu_status
//   Description: Used to generate wakeup events via software (W1C).
#define WU_DCR__THREAD0_WU_EVENT_CLR__SW_WU_STATUS_width        4
#define WU_DCR__THREAD0_WU_EVENT_CLR__SW_WU_STATUS_position     31
#define WU_DCR__THREAD0_WU_EVENT_CLR__SW_WU_STATUS_get(x)       _BGQ_GET(4,31,x)
#define WU_DCR__THREAD0_WU_EVENT_CLR__SW_WU_STATUS_set(v)       _BGQ_SET(4,31,v)
#define WU_DCR__THREAD0_WU_EVENT_CLR__SW_WU_STATUS_insert(x,v)  _BGQ_INSERT(4,31,x,v)

//   Field:       wac_en
//   Description: Enables wakeup of Thread 0 on each wakeup address compare match (W1C).
#define WU_DCR__THREAD0_WU_EVENT_CLR__WAC_EN_width        12
#define WU_DCR__THREAD0_WU_EVENT_CLR__WAC_EN_position     43
#define WU_DCR__THREAD0_WU_EVENT_CLR__WAC_EN_get(x)       _BGQ_GET(12,43,x)
#define WU_DCR__THREAD0_WU_EVENT_CLR__WAC_EN_set(v)       _BGQ_SET(12,43,v)
#define WU_DCR__THREAD0_WU_EVENT_CLR__WAC_EN_insert(x,v)  _BGQ_INSERT(12,43,x,v)

//   Field:       t0_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 0 on c2c interrupt from cores 0-15, targeted to thread 0 (W1C).
#define WU_DCR__THREAD0_WU_EVENT_CLR__T0_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD0_WU_EVENT_CLR__T0_0_15_C2C_WU_EN_position     44
#define WU_DCR__THREAD0_WU_EVENT_CLR__T0_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,44,x)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T0_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,44,v)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T0_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,44,x,v)

//   Field:       t0_16_c2c_wu_en
//   Description: Enables wakeup of Thread 0 on c2c interrupt from core 16, targeted to thread 0 (W1C).
#define WU_DCR__THREAD0_WU_EVENT_CLR__T0_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD0_WU_EVENT_CLR__T0_16_C2C_WU_EN_position     45
#define WU_DCR__THREAD0_WU_EVENT_CLR__T0_16_C2C_WU_EN_get(x)       _BGQ_GET(1,45,x)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T0_16_C2C_WU_EN_set(v)       _BGQ_SET(1,45,v)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T0_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,45,x,v)

//   Field:       t1_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 0 on c2c interrupt from cores 0-15, targeted to thread 1 (W1C).
#define WU_DCR__THREAD0_WU_EVENT_CLR__T1_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD0_WU_EVENT_CLR__T1_0_15_C2C_WU_EN_position     46
#define WU_DCR__THREAD0_WU_EVENT_CLR__T1_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,46,x)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T1_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,46,v)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T1_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,46,x,v)

//   Field:       t1_16_c2c_wu_en
//   Description: Enables wakeup of Thread 0 on c2c interrupt from core 16, targeted to thread 1 (W1C).
#define WU_DCR__THREAD0_WU_EVENT_CLR__T1_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD0_WU_EVENT_CLR__T1_16_C2C_WU_EN_position     47
#define WU_DCR__THREAD0_WU_EVENT_CLR__T1_16_C2C_WU_EN_get(x)       _BGQ_GET(1,47,x)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T1_16_C2C_WU_EN_set(v)       _BGQ_SET(1,47,v)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T1_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,47,x,v)

//   Field:       t2_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 0 on c2c interrupt from cores 0-15, targeted to thread 2 (W1C).
#define WU_DCR__THREAD0_WU_EVENT_CLR__T2_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD0_WU_EVENT_CLR__T2_0_15_C2C_WU_EN_position     48
#define WU_DCR__THREAD0_WU_EVENT_CLR__T2_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,48,x)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T2_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,48,v)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T2_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,48,x,v)

//   Field:       t2_16_c2c_wu_en
//   Description: Enables wakeup of Thread 0 on c2c interrupt from core 16, targeted to thread 2 (W1C).
#define WU_DCR__THREAD0_WU_EVENT_CLR__T2_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD0_WU_EVENT_CLR__T2_16_C2C_WU_EN_position     49
#define WU_DCR__THREAD0_WU_EVENT_CLR__T2_16_C2C_WU_EN_get(x)       _BGQ_GET(1,49,x)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T2_16_C2C_WU_EN_set(v)       _BGQ_SET(1,49,v)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T2_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,49,x,v)

//   Field:       t3_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 0 on c2c interrupt from cores 0-15, targeted to thread 3 (W1C).
#define WU_DCR__THREAD0_WU_EVENT_CLR__T3_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD0_WU_EVENT_CLR__T3_0_15_C2C_WU_EN_position     50
#define WU_DCR__THREAD0_WU_EVENT_CLR__T3_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,50,x)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T3_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,50,v)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T3_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,50,x,v)

//   Field:       t3_16_c2c_wu_en
//   Description: Enables wakeup of Thread 0 on c2c interrupt from core 16, targeted to thread 3 (W1C).
#define WU_DCR__THREAD0_WU_EVENT_CLR__T3_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD0_WU_EVENT_CLR__T3_16_C2C_WU_EN_position     51
#define WU_DCR__THREAD0_WU_EVENT_CLR__T3_16_C2C_WU_EN_get(x)       _BGQ_GET(1,51,x)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T3_16_C2C_WU_EN_set(v)       _BGQ_SET(1,51,v)
#define WU_DCR__THREAD0_WU_EVENT_CLR__T3_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,51,x,v)

//   Field:       mu_wu_en
//   Description: Enables wakeup of Thread 0 from Message Unit (W1C).
#define WU_DCR__THREAD0_WU_EVENT_CLR__MU_WU_EN_width        4
#define WU_DCR__THREAD0_WU_EVENT_CLR__MU_WU_EN_position     55
#define WU_DCR__THREAD0_WU_EVENT_CLR__MU_WU_EN_get(x)       _BGQ_GET(4,55,x)
#define WU_DCR__THREAD0_WU_EVENT_CLR__MU_WU_EN_set(v)       _BGQ_SET(4,55,v)
#define WU_DCR__THREAD0_WU_EVENT_CLR__MU_WU_EN_insert(x,v)  _BGQ_INSERT(4,55,x,v)

//   Field:       gea_wu_en
//   Description: Enables wakeup of Thread 0 from GEA (W1C).
#define WU_DCR__THREAD0_WU_EVENT_CLR__GEA_WU_EN_width        4
#define WU_DCR__THREAD0_WU_EVENT_CLR__GEA_WU_EN_position     59
#define WU_DCR__THREAD0_WU_EVENT_CLR__GEA_WU_EN_get(x)       _BGQ_GET(4,59,x)
#define WU_DCR__THREAD0_WU_EVENT_CLR__GEA_WU_EN_set(v)       _BGQ_SET(4,59,v)
#define WU_DCR__THREAD0_WU_EVENT_CLR__GEA_WU_EN_insert(x,v)  _BGQ_INSERT(4,59,x,v)

//   Field:       sw_wu_en
//   Description: Enables wakeup of Thread 0 when corresponding bits of sw_wu_status in this register are set (W1C).
#define WU_DCR__THREAD0_WU_EVENT_CLR__SW_WU_EN_width        4
#define WU_DCR__THREAD0_WU_EVENT_CLR__SW_WU_EN_position     63
#define WU_DCR__THREAD0_WU_EVENT_CLR__SW_WU_EN_get(x)       _BGQ_GET(4,63,x)
#define WU_DCR__THREAD0_WU_EVENT_CLR__SW_WU_EN_set(v)       _BGQ_SET(4,63,v)
#define WU_DCR__THREAD0_WU_EVENT_CLR__SW_WU_EN_insert(x,v)  _BGQ_INSERT(4,63,x,v)

  
// Register:
//   Name:        thread1_wu_event_set
//   Type:        Read/Write External
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Thread 1 Write-one-to-set wakup event configuration/status register.
#define WU_DCR__THREAD1_WU_EVENT_SET_offset  (0x03E8) // Offset of register in instance 
#define WU_DCR__THREAD1_WU_EVENT_SET_range  (0x1) // Range of external address space
#define WU_DCR__THREAD1_WU_EVENT_SET_reset  UNSIGNED64(0x0000000000000000)

//   Field:       wac_status
//   Description: Indicates that address comparators have detected a match, This bit remains set until cleared by software via the thread0_wu_event_clear register (W1S).
#define WU_DCR__THREAD1_WU_EVENT_SET__WAC_STATUS_width        12
#define WU_DCR__THREAD1_WU_EVENT_SET__WAC_STATUS_position     11
#define WU_DCR__THREAD1_WU_EVENT_SET__WAC_STATUS_get(x)       _BGQ_GET(12,11,x)
#define WU_DCR__THREAD1_WU_EVENT_SET__WAC_STATUS_set(v)       _BGQ_SET(12,11,v)
#define WU_DCR__THREAD1_WU_EVENT_SET__WAC_STATUS_insert(x,v)  _BGQ_INSERT(12,11,x,v)

//   Field:       t0_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 0. This bit reflects the equivalent bit in the BIC (R0).
#define WU_DCR__THREAD1_WU_EVENT_SET__T0_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD1_WU_EVENT_SET__T0_0_15_C2C_WU_STATUS_position     12
#define WU_DCR__THREAD1_WU_EVENT_SET__T0_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,12,x)
#define WU_DCR__THREAD1_WU_EVENT_SET__T0_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,12,v)
#define WU_DCR__THREAD1_WU_EVENT_SET__T0_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,12,x,v)

//   Field:       t0_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 0. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD1_WU_EVENT_SET__T0_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD1_WU_EVENT_SET__T0_16_C2C_WU_STATUS_position     13
#define WU_DCR__THREAD1_WU_EVENT_SET__T0_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,13,x)
#define WU_DCR__THREAD1_WU_EVENT_SET__T0_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,13,v)
#define WU_DCR__THREAD1_WU_EVENT_SET__T0_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,13,x,v)

//   Field:       t1_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 1. This bit reflects the equivalent bit in the BIC (RO)
#define WU_DCR__THREAD1_WU_EVENT_SET__T1_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD1_WU_EVENT_SET__T1_0_15_C2C_WU_STATUS_position     14
#define WU_DCR__THREAD1_WU_EVENT_SET__T1_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,14,x)
#define WU_DCR__THREAD1_WU_EVENT_SET__T1_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,14,v)
#define WU_DCR__THREAD1_WU_EVENT_SET__T1_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,14,x,v)

//   Field:       t1_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 1. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD1_WU_EVENT_SET__T1_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD1_WU_EVENT_SET__T1_16_C2C_WU_STATUS_position     15
#define WU_DCR__THREAD1_WU_EVENT_SET__T1_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,15,x)
#define WU_DCR__THREAD1_WU_EVENT_SET__T1_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,15,v)
#define WU_DCR__THREAD1_WU_EVENT_SET__T1_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,15,x,v)

//   Field:       t2_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 2. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD1_WU_EVENT_SET__T2_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD1_WU_EVENT_SET__T2_0_15_C2C_WU_STATUS_position     16
#define WU_DCR__THREAD1_WU_EVENT_SET__T2_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,16,x)
#define WU_DCR__THREAD1_WU_EVENT_SET__T2_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,16,v)
#define WU_DCR__THREAD1_WU_EVENT_SET__T2_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,16,x,v)

//   Field:       t2_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 2. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD1_WU_EVENT_SET__T2_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD1_WU_EVENT_SET__T2_16_C2C_WU_STATUS_position     17
#define WU_DCR__THREAD1_WU_EVENT_SET__T2_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,17,x)
#define WU_DCR__THREAD1_WU_EVENT_SET__T2_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,17,v)
#define WU_DCR__THREAD1_WU_EVENT_SET__T2_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,17,x,v)

//   Field:       t3_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 3. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD1_WU_EVENT_SET__T3_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD1_WU_EVENT_SET__T3_0_15_C2C_WU_STATUS_position     18
#define WU_DCR__THREAD1_WU_EVENT_SET__T3_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,18,x)
#define WU_DCR__THREAD1_WU_EVENT_SET__T3_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,18,v)
#define WU_DCR__THREAD1_WU_EVENT_SET__T3_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,18,x,v)

//   Field:       t3_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 3. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD1_WU_EVENT_SET__T3_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD1_WU_EVENT_SET__T3_16_C2C_WU_STATUS_position     19
#define WU_DCR__THREAD1_WU_EVENT_SET__T3_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,19,x)
#define WU_DCR__THREAD1_WU_EVENT_SET__T3_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,19,v)
#define WU_DCR__THREAD1_WU_EVENT_SET__T3_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,19,x,v)

//   Field:       mu_wu_status
//   Description: Indicates wakeup request from Message Unit. This field reflects the incoming wakeup request from the MU, and is cleared in the MU (RO).
#define WU_DCR__THREAD1_WU_EVENT_SET__MU_WU_STATUS_width        4
#define WU_DCR__THREAD1_WU_EVENT_SET__MU_WU_STATUS_position     23
#define WU_DCR__THREAD1_WU_EVENT_SET__MU_WU_STATUS_get(x)       _BGQ_GET(4,23,x)
#define WU_DCR__THREAD1_WU_EVENT_SET__MU_WU_STATUS_set(v)       _BGQ_SET(4,23,v)
#define WU_DCR__THREAD1_WU_EVENT_SET__MU_WU_STATUS_insert(x,v)  _BGQ_INSERT(4,23,x,v)

//   Field:       gea_wu_status
//   Description: Indicates wakeup request from GEA. This field reflects the incoming wakeup request from the GEA, and is cleared in the GEA (RO).
#define WU_DCR__THREAD1_WU_EVENT_SET__GEA_WU_STATUS_width        4
#define WU_DCR__THREAD1_WU_EVENT_SET__GEA_WU_STATUS_position     27
#define WU_DCR__THREAD1_WU_EVENT_SET__GEA_WU_STATUS_get(x)       _BGQ_GET(4,27,x)
#define WU_DCR__THREAD1_WU_EVENT_SET__GEA_WU_STATUS_set(v)       _BGQ_SET(4,27,v)
#define WU_DCR__THREAD1_WU_EVENT_SET__GEA_WU_STATUS_insert(x,v)  _BGQ_INSERT(4,27,x,v)

//   Field:       sw_wu_status
//   Description: Used to generate wakeup events via software (W1S).
#define WU_DCR__THREAD1_WU_EVENT_SET__SW_WU_STATUS_width        4
#define WU_DCR__THREAD1_WU_EVENT_SET__SW_WU_STATUS_position     31
#define WU_DCR__THREAD1_WU_EVENT_SET__SW_WU_STATUS_get(x)       _BGQ_GET(4,31,x)
#define WU_DCR__THREAD1_WU_EVENT_SET__SW_WU_STATUS_set(v)       _BGQ_SET(4,31,v)
#define WU_DCR__THREAD1_WU_EVENT_SET__SW_WU_STATUS_insert(x,v)  _BGQ_INSERT(4,31,x,v)

//   Field:       wac_en
//   Description: Enables wakeup of Thread 1 on each wakeup address compare match (W1S).
#define WU_DCR__THREAD1_WU_EVENT_SET__WAC_EN_width        12
#define WU_DCR__THREAD1_WU_EVENT_SET__WAC_EN_position     43
#define WU_DCR__THREAD1_WU_EVENT_SET__WAC_EN_get(x)       _BGQ_GET(12,43,x)
#define WU_DCR__THREAD1_WU_EVENT_SET__WAC_EN_set(v)       _BGQ_SET(12,43,v)
#define WU_DCR__THREAD1_WU_EVENT_SET__WAC_EN_insert(x,v)  _BGQ_INSERT(12,43,x,v)

//   Field:       t0_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 1 on c2c interrupt from cores 0-15, targeted to thread 0 (W1S).
#define WU_DCR__THREAD1_WU_EVENT_SET__T0_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD1_WU_EVENT_SET__T0_0_15_C2C_WU_EN_position     44
#define WU_DCR__THREAD1_WU_EVENT_SET__T0_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,44,x)
#define WU_DCR__THREAD1_WU_EVENT_SET__T0_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,44,v)
#define WU_DCR__THREAD1_WU_EVENT_SET__T0_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,44,x,v)

//   Field:       t0_16_c2c_wu_en
//   Description: Enables wakeup of Thread 1 on c2c interrupt from core 16, targeted to thread 0 (W1S).
#define WU_DCR__THREAD1_WU_EVENT_SET__T0_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD1_WU_EVENT_SET__T0_16_C2C_WU_EN_position     45
#define WU_DCR__THREAD1_WU_EVENT_SET__T0_16_C2C_WU_EN_get(x)       _BGQ_GET(1,45,x)
#define WU_DCR__THREAD1_WU_EVENT_SET__T0_16_C2C_WU_EN_set(v)       _BGQ_SET(1,45,v)
#define WU_DCR__THREAD1_WU_EVENT_SET__T0_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,45,x,v)

//   Field:       t1_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 1 on c2c interrupt from cores 0-15, targeted to thread 1 (W1S).
#define WU_DCR__THREAD1_WU_EVENT_SET__T1_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD1_WU_EVENT_SET__T1_0_15_C2C_WU_EN_position     46
#define WU_DCR__THREAD1_WU_EVENT_SET__T1_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,46,x)
#define WU_DCR__THREAD1_WU_EVENT_SET__T1_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,46,v)
#define WU_DCR__THREAD1_WU_EVENT_SET__T1_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,46,x,v)

//   Field:       t1_16_c2c_wu_en
//   Description: Enables wakeup of Thread 1 on c2c interrupt from core 16, targeted to thread 1 (W1S).
#define WU_DCR__THREAD1_WU_EVENT_SET__T1_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD1_WU_EVENT_SET__T1_16_C2C_WU_EN_position     47
#define WU_DCR__THREAD1_WU_EVENT_SET__T1_16_C2C_WU_EN_get(x)       _BGQ_GET(1,47,x)
#define WU_DCR__THREAD1_WU_EVENT_SET__T1_16_C2C_WU_EN_set(v)       _BGQ_SET(1,47,v)
#define WU_DCR__THREAD1_WU_EVENT_SET__T1_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,47,x,v)

//   Field:       t2_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 1 on c2c interrupt from cores 0-15, targeted to thread 2 (W1S).
#define WU_DCR__THREAD1_WU_EVENT_SET__T2_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD1_WU_EVENT_SET__T2_0_15_C2C_WU_EN_position     48
#define WU_DCR__THREAD1_WU_EVENT_SET__T2_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,48,x)
#define WU_DCR__THREAD1_WU_EVENT_SET__T2_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,48,v)
#define WU_DCR__THREAD1_WU_EVENT_SET__T2_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,48,x,v)

//   Field:       t2_16_c2c_wu_en
//   Description: Enables wakeup of Thread 1 on c2c interrupt from core 16, targeted to thread 2 (W1S).
#define WU_DCR__THREAD1_WU_EVENT_SET__T2_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD1_WU_EVENT_SET__T2_16_C2C_WU_EN_position     49
#define WU_DCR__THREAD1_WU_EVENT_SET__T2_16_C2C_WU_EN_get(x)       _BGQ_GET(1,49,x)
#define WU_DCR__THREAD1_WU_EVENT_SET__T2_16_C2C_WU_EN_set(v)       _BGQ_SET(1,49,v)
#define WU_DCR__THREAD1_WU_EVENT_SET__T2_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,49,x,v)

//   Field:       t3_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 1 on c2c interrupt from cores 0-15, targeted to thread 3 (W1S).
#define WU_DCR__THREAD1_WU_EVENT_SET__T3_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD1_WU_EVENT_SET__T3_0_15_C2C_WU_EN_position     50
#define WU_DCR__THREAD1_WU_EVENT_SET__T3_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,50,x)
#define WU_DCR__THREAD1_WU_EVENT_SET__T3_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,50,v)
#define WU_DCR__THREAD1_WU_EVENT_SET__T3_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,50,x,v)

//   Field:       t3_16_c2c_wu_en
//   Description: Enables wakeup of Thread 1 on c2c interrupt from core 16, targeted to thread 3 (W1S).
#define WU_DCR__THREAD1_WU_EVENT_SET__T3_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD1_WU_EVENT_SET__T3_16_C2C_WU_EN_position     51
#define WU_DCR__THREAD1_WU_EVENT_SET__T3_16_C2C_WU_EN_get(x)       _BGQ_GET(1,51,x)
#define WU_DCR__THREAD1_WU_EVENT_SET__T3_16_C2C_WU_EN_set(v)       _BGQ_SET(1,51,v)
#define WU_DCR__THREAD1_WU_EVENT_SET__T3_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,51,x,v)

//   Field:       mu_wu_en
//   Description: Enables wakeup of Thread 1 from Message Unit (W1S).
#define WU_DCR__THREAD1_WU_EVENT_SET__MU_WU_EN_width        4
#define WU_DCR__THREAD1_WU_EVENT_SET__MU_WU_EN_position     55
#define WU_DCR__THREAD1_WU_EVENT_SET__MU_WU_EN_get(x)       _BGQ_GET(4,55,x)
#define WU_DCR__THREAD1_WU_EVENT_SET__MU_WU_EN_set(v)       _BGQ_SET(4,55,v)
#define WU_DCR__THREAD1_WU_EVENT_SET__MU_WU_EN_insert(x,v)  _BGQ_INSERT(4,55,x,v)

//   Field:       gea_wu_en
//   Description: Enables wakeup of Thread 1 from GEA (W1S).
#define WU_DCR__THREAD1_WU_EVENT_SET__GEA_WU_EN_width        4
#define WU_DCR__THREAD1_WU_EVENT_SET__GEA_WU_EN_position     59
#define WU_DCR__THREAD1_WU_EVENT_SET__GEA_WU_EN_get(x)       _BGQ_GET(4,59,x)
#define WU_DCR__THREAD1_WU_EVENT_SET__GEA_WU_EN_set(v)       _BGQ_SET(4,59,v)
#define WU_DCR__THREAD1_WU_EVENT_SET__GEA_WU_EN_insert(x,v)  _BGQ_INSERT(4,59,x,v)

//   Field:       sw_wu_en
//   Description: Enables wakeup of Thread 1 when corresponding bits of sw_wu_status in this register are set (W1S).
#define WU_DCR__THREAD1_WU_EVENT_SET__SW_WU_EN_width        4
#define WU_DCR__THREAD1_WU_EVENT_SET__SW_WU_EN_position     63
#define WU_DCR__THREAD1_WU_EVENT_SET__SW_WU_EN_get(x)       _BGQ_GET(4,63,x)
#define WU_DCR__THREAD1_WU_EVENT_SET__SW_WU_EN_set(v)       _BGQ_SET(4,63,v)
#define WU_DCR__THREAD1_WU_EVENT_SET__SW_WU_EN_insert(x,v)  _BGQ_INSERT(4,63,x,v)

  
// Register:
//   Name:        thread1_wu_event_clr
//   Type:        Read/Write External
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Thread 1 Write-one-to-clear wakup event configuration/status register.
#define WU_DCR__THREAD1_WU_EVENT_CLR_offset  (0x03EC) // Offset of register in instance 
#define WU_DCR__THREAD1_WU_EVENT_CLR_range  (0x1) // Range of external address space
#define WU_DCR__THREAD1_WU_EVENT_CLR_reset  UNSIGNED64(0x0000000000000000)

//   Field:       wac_status
//   Description: Indicates that address comparators have detected a match, This bit remains set until cleared by software via the thread0_wu_event_clear register (W1C).
#define WU_DCR__THREAD1_WU_EVENT_CLR__WAC_STATUS_width        12
#define WU_DCR__THREAD1_WU_EVENT_CLR__WAC_STATUS_position     11
#define WU_DCR__THREAD1_WU_EVENT_CLR__WAC_STATUS_get(x)       _BGQ_GET(12,11,x)
#define WU_DCR__THREAD1_WU_EVENT_CLR__WAC_STATUS_set(v)       _BGQ_SET(12,11,v)
#define WU_DCR__THREAD1_WU_EVENT_CLR__WAC_STATUS_insert(x,v)  _BGQ_INSERT(12,11,x,v)

//   Field:       t0_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 0. This bit reflects the equivalent bit in the BIC (R0).
#define WU_DCR__THREAD1_WU_EVENT_CLR__T0_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD1_WU_EVENT_CLR__T0_0_15_C2C_WU_STATUS_position     12
#define WU_DCR__THREAD1_WU_EVENT_CLR__T0_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,12,x)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T0_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,12,v)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T0_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,12,x,v)

//   Field:       t0_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 0. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD1_WU_EVENT_CLR__T0_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD1_WU_EVENT_CLR__T0_16_C2C_WU_STATUS_position     13
#define WU_DCR__THREAD1_WU_EVENT_CLR__T0_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,13,x)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T0_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,13,v)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T0_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,13,x,v)

//   Field:       t1_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 1. This bit reflects the equivalent bit in the BIC (RO)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T1_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD1_WU_EVENT_CLR__T1_0_15_C2C_WU_STATUS_position     14
#define WU_DCR__THREAD1_WU_EVENT_CLR__T1_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,14,x)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T1_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,14,v)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T1_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,14,x,v)

//   Field:       t1_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 1. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD1_WU_EVENT_CLR__T1_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD1_WU_EVENT_CLR__T1_16_C2C_WU_STATUS_position     15
#define WU_DCR__THREAD1_WU_EVENT_CLR__T1_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,15,x)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T1_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,15,v)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T1_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,15,x,v)

//   Field:       t2_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 2. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD1_WU_EVENT_CLR__T2_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD1_WU_EVENT_CLR__T2_0_15_C2C_WU_STATUS_position     16
#define WU_DCR__THREAD1_WU_EVENT_CLR__T2_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,16,x)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T2_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,16,v)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T2_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,16,x,v)

//   Field:       t2_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 2. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD1_WU_EVENT_CLR__T2_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD1_WU_EVENT_CLR__T2_16_C2C_WU_STATUS_position     17
#define WU_DCR__THREAD1_WU_EVENT_CLR__T2_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,17,x)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T2_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,17,v)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T2_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,17,x,v)

//   Field:       t3_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 3. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD1_WU_EVENT_CLR__T3_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD1_WU_EVENT_CLR__T3_0_15_C2C_WU_STATUS_position     18
#define WU_DCR__THREAD1_WU_EVENT_CLR__T3_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,18,x)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T3_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,18,v)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T3_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,18,x,v)

//   Field:       t3_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 3. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD1_WU_EVENT_CLR__T3_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD1_WU_EVENT_CLR__T3_16_C2C_WU_STATUS_position     19
#define WU_DCR__THREAD1_WU_EVENT_CLR__T3_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,19,x)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T3_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,19,v)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T3_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,19,x,v)

//   Field:       mu_wu_status
//   Description: Indicates wakeup request from Message Unit. This field reflects the incoming wakeup request from the MU, and is cleared in the MU (RO).
#define WU_DCR__THREAD1_WU_EVENT_CLR__MU_WU_STATUS_width        4
#define WU_DCR__THREAD1_WU_EVENT_CLR__MU_WU_STATUS_position     23
#define WU_DCR__THREAD1_WU_EVENT_CLR__MU_WU_STATUS_get(x)       _BGQ_GET(4,23,x)
#define WU_DCR__THREAD1_WU_EVENT_CLR__MU_WU_STATUS_set(v)       _BGQ_SET(4,23,v)
#define WU_DCR__THREAD1_WU_EVENT_CLR__MU_WU_STATUS_insert(x,v)  _BGQ_INSERT(4,23,x,v)

//   Field:       gea_wu_status
//   Description: Indicates wakeup request from GEA. This field reflects the incoming wakeup request from the GEA, and is cleared in the GEA (RO).
#define WU_DCR__THREAD1_WU_EVENT_CLR__GEA_WU_STATUS_width        4
#define WU_DCR__THREAD1_WU_EVENT_CLR__GEA_WU_STATUS_position     27
#define WU_DCR__THREAD1_WU_EVENT_CLR__GEA_WU_STATUS_get(x)       _BGQ_GET(4,27,x)
#define WU_DCR__THREAD1_WU_EVENT_CLR__GEA_WU_STATUS_set(v)       _BGQ_SET(4,27,v)
#define WU_DCR__THREAD1_WU_EVENT_CLR__GEA_WU_STATUS_insert(x,v)  _BGQ_INSERT(4,27,x,v)

//   Field:       sw_wu_status
//   Description: Used to generate wakeup events via software (W1C).
#define WU_DCR__THREAD1_WU_EVENT_CLR__SW_WU_STATUS_width        4
#define WU_DCR__THREAD1_WU_EVENT_CLR__SW_WU_STATUS_position     31
#define WU_DCR__THREAD1_WU_EVENT_CLR__SW_WU_STATUS_get(x)       _BGQ_GET(4,31,x)
#define WU_DCR__THREAD1_WU_EVENT_CLR__SW_WU_STATUS_set(v)       _BGQ_SET(4,31,v)
#define WU_DCR__THREAD1_WU_EVENT_CLR__SW_WU_STATUS_insert(x,v)  _BGQ_INSERT(4,31,x,v)

//   Field:       wac_en
//   Description: Enables wakeup of Thread 1 on each wakeup address compare match (W1C).
#define WU_DCR__THREAD1_WU_EVENT_CLR__WAC_EN_width        12
#define WU_DCR__THREAD1_WU_EVENT_CLR__WAC_EN_position     43
#define WU_DCR__THREAD1_WU_EVENT_CLR__WAC_EN_get(x)       _BGQ_GET(12,43,x)
#define WU_DCR__THREAD1_WU_EVENT_CLR__WAC_EN_set(v)       _BGQ_SET(12,43,v)
#define WU_DCR__THREAD1_WU_EVENT_CLR__WAC_EN_insert(x,v)  _BGQ_INSERT(12,43,x,v)

//   Field:       t0_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 1 on c2c interrupt from cores 0-15, targeted to thread 0 (W1C).
#define WU_DCR__THREAD1_WU_EVENT_CLR__T0_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD1_WU_EVENT_CLR__T0_0_15_C2C_WU_EN_position     44
#define WU_DCR__THREAD1_WU_EVENT_CLR__T0_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,44,x)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T0_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,44,v)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T0_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,44,x,v)

//   Field:       t0_16_c2c_wu_en
//   Description: Enables wakeup of Thread 1 on c2c interrupt from core 16, targeted to thread 0 (W1C).
#define WU_DCR__THREAD1_WU_EVENT_CLR__T0_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD1_WU_EVENT_CLR__T0_16_C2C_WU_EN_position     45
#define WU_DCR__THREAD1_WU_EVENT_CLR__T0_16_C2C_WU_EN_get(x)       _BGQ_GET(1,45,x)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T0_16_C2C_WU_EN_set(v)       _BGQ_SET(1,45,v)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T0_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,45,x,v)

//   Field:       t1_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 1 on c2c interrupt from cores 0-15, targeted to thread 1 (W1C).
#define WU_DCR__THREAD1_WU_EVENT_CLR__T1_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD1_WU_EVENT_CLR__T1_0_15_C2C_WU_EN_position     46
#define WU_DCR__THREAD1_WU_EVENT_CLR__T1_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,46,x)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T1_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,46,v)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T1_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,46,x,v)

//   Field:       t1_16_c2c_wu_en
//   Description: Enables wakeup of Thread 1 on c2c interrupt from core 16, targeted to thread 1 (W1C).
#define WU_DCR__THREAD1_WU_EVENT_CLR__T1_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD1_WU_EVENT_CLR__T1_16_C2C_WU_EN_position     47
#define WU_DCR__THREAD1_WU_EVENT_CLR__T1_16_C2C_WU_EN_get(x)       _BGQ_GET(1,47,x)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T1_16_C2C_WU_EN_set(v)       _BGQ_SET(1,47,v)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T1_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,47,x,v)

//   Field:       t2_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 1 on c2c interrupt from cores 0-15, targeted to thread 2 (W1C).
#define WU_DCR__THREAD1_WU_EVENT_CLR__T2_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD1_WU_EVENT_CLR__T2_0_15_C2C_WU_EN_position     48
#define WU_DCR__THREAD1_WU_EVENT_CLR__T2_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,48,x)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T2_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,48,v)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T2_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,48,x,v)

//   Field:       t2_16_c2c_wu_en
//   Description: Enables wakeup of Thread 1 on c2c interrupt from core 16, targeted to thread 2 (W1C).
#define WU_DCR__THREAD1_WU_EVENT_CLR__T2_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD1_WU_EVENT_CLR__T2_16_C2C_WU_EN_position     49
#define WU_DCR__THREAD1_WU_EVENT_CLR__T2_16_C2C_WU_EN_get(x)       _BGQ_GET(1,49,x)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T2_16_C2C_WU_EN_set(v)       _BGQ_SET(1,49,v)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T2_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,49,x,v)

//   Field:       t3_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 1 on c2c interrupt from cores 0-15, targeted to thread 3 (W1C).
#define WU_DCR__THREAD1_WU_EVENT_CLR__T3_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD1_WU_EVENT_CLR__T3_0_15_C2C_WU_EN_position     50
#define WU_DCR__THREAD1_WU_EVENT_CLR__T3_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,50,x)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T3_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,50,v)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T3_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,50,x,v)

//   Field:       t3_16_c2c_wu_en
//   Description: Enables wakeup of Thread 1 on c2c interrupt from core 16, targeted to thread 3 (W1C).
#define WU_DCR__THREAD1_WU_EVENT_CLR__T3_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD1_WU_EVENT_CLR__T3_16_C2C_WU_EN_position     51
#define WU_DCR__THREAD1_WU_EVENT_CLR__T3_16_C2C_WU_EN_get(x)       _BGQ_GET(1,51,x)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T3_16_C2C_WU_EN_set(v)       _BGQ_SET(1,51,v)
#define WU_DCR__THREAD1_WU_EVENT_CLR__T3_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,51,x,v)

//   Field:       mu_wu_en
//   Description: Enables wakeup of Thread 1 from Message Unit (W1C).
#define WU_DCR__THREAD1_WU_EVENT_CLR__MU_WU_EN_width        4
#define WU_DCR__THREAD1_WU_EVENT_CLR__MU_WU_EN_position     55
#define WU_DCR__THREAD1_WU_EVENT_CLR__MU_WU_EN_get(x)       _BGQ_GET(4,55,x)
#define WU_DCR__THREAD1_WU_EVENT_CLR__MU_WU_EN_set(v)       _BGQ_SET(4,55,v)
#define WU_DCR__THREAD1_WU_EVENT_CLR__MU_WU_EN_insert(x,v)  _BGQ_INSERT(4,55,x,v)

//   Field:       gea_wu_en
//   Description: Enables wakeup of Thread 1 from GEA (W1C).
#define WU_DCR__THREAD1_WU_EVENT_CLR__GEA_WU_EN_width        4
#define WU_DCR__THREAD1_WU_EVENT_CLR__GEA_WU_EN_position     59
#define WU_DCR__THREAD1_WU_EVENT_CLR__GEA_WU_EN_get(x)       _BGQ_GET(4,59,x)
#define WU_DCR__THREAD1_WU_EVENT_CLR__GEA_WU_EN_set(v)       _BGQ_SET(4,59,v)
#define WU_DCR__THREAD1_WU_EVENT_CLR__GEA_WU_EN_insert(x,v)  _BGQ_INSERT(4,59,x,v)

//   Field:       sw_wu_en
//   Description: Enables wakeup of Thread 1 when corresponding bits of sw_wu_status in this register are set (W1C).
#define WU_DCR__THREAD1_WU_EVENT_CLR__SW_WU_EN_width        4
#define WU_DCR__THREAD1_WU_EVENT_CLR__SW_WU_EN_position     63
#define WU_DCR__THREAD1_WU_EVENT_CLR__SW_WU_EN_get(x)       _BGQ_GET(4,63,x)
#define WU_DCR__THREAD1_WU_EVENT_CLR__SW_WU_EN_set(v)       _BGQ_SET(4,63,v)
#define WU_DCR__THREAD1_WU_EVENT_CLR__SW_WU_EN_insert(x,v)  _BGQ_INSERT(4,63,x,v)

  
// Register:
//   Name:        thread2_wu_event_set
//   Type:        Read/Write External
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Thread 2 Write-one-to-set wakup event configuration/status register.
#define WU_DCR__THREAD2_WU_EVENT_SET_offset  (0x03F0) // Offset of register in instance 
#define WU_DCR__THREAD2_WU_EVENT_SET_range  (0x1) // Range of external address space
#define WU_DCR__THREAD2_WU_EVENT_SET_reset  UNSIGNED64(0x0000000000000000)

//   Field:       wac_status
//   Description: Indicates that address comparators have detected a match, This bit remains set until cleared by software via the thread0_wu_event_clear register (W1S).
#define WU_DCR__THREAD2_WU_EVENT_SET__WAC_STATUS_width        12
#define WU_DCR__THREAD2_WU_EVENT_SET__WAC_STATUS_position     11
#define WU_DCR__THREAD2_WU_EVENT_SET__WAC_STATUS_get(x)       _BGQ_GET(12,11,x)
#define WU_DCR__THREAD2_WU_EVENT_SET__WAC_STATUS_set(v)       _BGQ_SET(12,11,v)
#define WU_DCR__THREAD2_WU_EVENT_SET__WAC_STATUS_insert(x,v)  _BGQ_INSERT(12,11,x,v)

//   Field:       t0_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 0. This bit reflects the equivalent bit in the BIC (R0).
#define WU_DCR__THREAD2_WU_EVENT_SET__T0_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD2_WU_EVENT_SET__T0_0_15_C2C_WU_STATUS_position     12
#define WU_DCR__THREAD2_WU_EVENT_SET__T0_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,12,x)
#define WU_DCR__THREAD2_WU_EVENT_SET__T0_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,12,v)
#define WU_DCR__THREAD2_WU_EVENT_SET__T0_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,12,x,v)

//   Field:       t0_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 0. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD2_WU_EVENT_SET__T0_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD2_WU_EVENT_SET__T0_16_C2C_WU_STATUS_position     13
#define WU_DCR__THREAD2_WU_EVENT_SET__T0_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,13,x)
#define WU_DCR__THREAD2_WU_EVENT_SET__T0_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,13,v)
#define WU_DCR__THREAD2_WU_EVENT_SET__T0_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,13,x,v)

//   Field:       t1_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 1. This bit reflects the equivalent bit in the BIC (RO)
#define WU_DCR__THREAD2_WU_EVENT_SET__T1_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD2_WU_EVENT_SET__T1_0_15_C2C_WU_STATUS_position     14
#define WU_DCR__THREAD2_WU_EVENT_SET__T1_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,14,x)
#define WU_DCR__THREAD2_WU_EVENT_SET__T1_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,14,v)
#define WU_DCR__THREAD2_WU_EVENT_SET__T1_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,14,x,v)

//   Field:       t1_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 1. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD2_WU_EVENT_SET__T1_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD2_WU_EVENT_SET__T1_16_C2C_WU_STATUS_position     15
#define WU_DCR__THREAD2_WU_EVENT_SET__T1_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,15,x)
#define WU_DCR__THREAD2_WU_EVENT_SET__T1_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,15,v)
#define WU_DCR__THREAD2_WU_EVENT_SET__T1_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,15,x,v)

//   Field:       t2_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 2. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD2_WU_EVENT_SET__T2_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD2_WU_EVENT_SET__T2_0_15_C2C_WU_STATUS_position     16
#define WU_DCR__THREAD2_WU_EVENT_SET__T2_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,16,x)
#define WU_DCR__THREAD2_WU_EVENT_SET__T2_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,16,v)
#define WU_DCR__THREAD2_WU_EVENT_SET__T2_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,16,x,v)

//   Field:       t2_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 2. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD2_WU_EVENT_SET__T2_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD2_WU_EVENT_SET__T2_16_C2C_WU_STATUS_position     17
#define WU_DCR__THREAD2_WU_EVENT_SET__T2_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,17,x)
#define WU_DCR__THREAD2_WU_EVENT_SET__T2_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,17,v)
#define WU_DCR__THREAD2_WU_EVENT_SET__T2_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,17,x,v)

//   Field:       t3_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 3. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD2_WU_EVENT_SET__T3_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD2_WU_EVENT_SET__T3_0_15_C2C_WU_STATUS_position     18
#define WU_DCR__THREAD2_WU_EVENT_SET__T3_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,18,x)
#define WU_DCR__THREAD2_WU_EVENT_SET__T3_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,18,v)
#define WU_DCR__THREAD2_WU_EVENT_SET__T3_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,18,x,v)

//   Field:       t3_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 3. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD2_WU_EVENT_SET__T3_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD2_WU_EVENT_SET__T3_16_C2C_WU_STATUS_position     19
#define WU_DCR__THREAD2_WU_EVENT_SET__T3_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,19,x)
#define WU_DCR__THREAD2_WU_EVENT_SET__T3_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,19,v)
#define WU_DCR__THREAD2_WU_EVENT_SET__T3_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,19,x,v)

//   Field:       mu_wu_status
//   Description: Indicates wakeup request from Message Unit. This field reflects the incoming wakeup request from the MU, and is cleared in the MU (RO).
#define WU_DCR__THREAD2_WU_EVENT_SET__MU_WU_STATUS_width        4
#define WU_DCR__THREAD2_WU_EVENT_SET__MU_WU_STATUS_position     23
#define WU_DCR__THREAD2_WU_EVENT_SET__MU_WU_STATUS_get(x)       _BGQ_GET(4,23,x)
#define WU_DCR__THREAD2_WU_EVENT_SET__MU_WU_STATUS_set(v)       _BGQ_SET(4,23,v)
#define WU_DCR__THREAD2_WU_EVENT_SET__MU_WU_STATUS_insert(x,v)  _BGQ_INSERT(4,23,x,v)

//   Field:       gea_wu_status
//   Description: Indicates wakeup request from GEA. This field reflects the incoming wakeup request from the GEA, and is cleared in the GEA (RO).
#define WU_DCR__THREAD2_WU_EVENT_SET__GEA_WU_STATUS_width        4
#define WU_DCR__THREAD2_WU_EVENT_SET__GEA_WU_STATUS_position     27
#define WU_DCR__THREAD2_WU_EVENT_SET__GEA_WU_STATUS_get(x)       _BGQ_GET(4,27,x)
#define WU_DCR__THREAD2_WU_EVENT_SET__GEA_WU_STATUS_set(v)       _BGQ_SET(4,27,v)
#define WU_DCR__THREAD2_WU_EVENT_SET__GEA_WU_STATUS_insert(x,v)  _BGQ_INSERT(4,27,x,v)

//   Field:       sw_wu_status
//   Description: Used to generate wakeup events via software (W1S).
#define WU_DCR__THREAD2_WU_EVENT_SET__SW_WU_STATUS_width        4
#define WU_DCR__THREAD2_WU_EVENT_SET__SW_WU_STATUS_position     31
#define WU_DCR__THREAD2_WU_EVENT_SET__SW_WU_STATUS_get(x)       _BGQ_GET(4,31,x)
#define WU_DCR__THREAD2_WU_EVENT_SET__SW_WU_STATUS_set(v)       _BGQ_SET(4,31,v)
#define WU_DCR__THREAD2_WU_EVENT_SET__SW_WU_STATUS_insert(x,v)  _BGQ_INSERT(4,31,x,v)

//   Field:       wac_en
//   Description: Enables wakeup of Thread 2 on each wakeup address compare match (W1S).
#define WU_DCR__THREAD2_WU_EVENT_SET__WAC_EN_width        12
#define WU_DCR__THREAD2_WU_EVENT_SET__WAC_EN_position     43
#define WU_DCR__THREAD2_WU_EVENT_SET__WAC_EN_get(x)       _BGQ_GET(12,43,x)
#define WU_DCR__THREAD2_WU_EVENT_SET__WAC_EN_set(v)       _BGQ_SET(12,43,v)
#define WU_DCR__THREAD2_WU_EVENT_SET__WAC_EN_insert(x,v)  _BGQ_INSERT(12,43,x,v)

//   Field:       t0_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 2 on c2c interrupt from cores 0-15, targeted to thread 0 (W1S).
#define WU_DCR__THREAD2_WU_EVENT_SET__T0_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD2_WU_EVENT_SET__T0_0_15_C2C_WU_EN_position     44
#define WU_DCR__THREAD2_WU_EVENT_SET__T0_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,44,x)
#define WU_DCR__THREAD2_WU_EVENT_SET__T0_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,44,v)
#define WU_DCR__THREAD2_WU_EVENT_SET__T0_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,44,x,v)

//   Field:       t0_16_c2c_wu_en
//   Description: Enables wakeup of Thread 2 on c2c interrupt from core 16, targeted to thread 0 (W1S).
#define WU_DCR__THREAD2_WU_EVENT_SET__T0_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD2_WU_EVENT_SET__T0_16_C2C_WU_EN_position     45
#define WU_DCR__THREAD2_WU_EVENT_SET__T0_16_C2C_WU_EN_get(x)       _BGQ_GET(1,45,x)
#define WU_DCR__THREAD2_WU_EVENT_SET__T0_16_C2C_WU_EN_set(v)       _BGQ_SET(1,45,v)
#define WU_DCR__THREAD2_WU_EVENT_SET__T0_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,45,x,v)

//   Field:       t1_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 2 on c2c interrupt from cores 0-15, targeted to thread 1 (W1S).
#define WU_DCR__THREAD2_WU_EVENT_SET__T1_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD2_WU_EVENT_SET__T1_0_15_C2C_WU_EN_position     46
#define WU_DCR__THREAD2_WU_EVENT_SET__T1_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,46,x)
#define WU_DCR__THREAD2_WU_EVENT_SET__T1_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,46,v)
#define WU_DCR__THREAD2_WU_EVENT_SET__T1_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,46,x,v)

//   Field:       t1_16_c2c_wu_en
//   Description: Enables wakeup of Thread 2 on c2c interrupt from core 16, targeted to thread 1 (W1S).
#define WU_DCR__THREAD2_WU_EVENT_SET__T1_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD2_WU_EVENT_SET__T1_16_C2C_WU_EN_position     47
#define WU_DCR__THREAD2_WU_EVENT_SET__T1_16_C2C_WU_EN_get(x)       _BGQ_GET(1,47,x)
#define WU_DCR__THREAD2_WU_EVENT_SET__T1_16_C2C_WU_EN_set(v)       _BGQ_SET(1,47,v)
#define WU_DCR__THREAD2_WU_EVENT_SET__T1_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,47,x,v)

//   Field:       t2_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 2 on c2c interrupt from cores 0-15, targeted to thread 2 (W1S).
#define WU_DCR__THREAD2_WU_EVENT_SET__T2_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD2_WU_EVENT_SET__T2_0_15_C2C_WU_EN_position     48
#define WU_DCR__THREAD2_WU_EVENT_SET__T2_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,48,x)
#define WU_DCR__THREAD2_WU_EVENT_SET__T2_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,48,v)
#define WU_DCR__THREAD2_WU_EVENT_SET__T2_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,48,x,v)

//   Field:       t2_16_c2c_wu_en
//   Description: Enables wakeup of Thread 2 on c2c interrupt from core 16, targeted to thread 2 (W1S).
#define WU_DCR__THREAD2_WU_EVENT_SET__T2_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD2_WU_EVENT_SET__T2_16_C2C_WU_EN_position     49
#define WU_DCR__THREAD2_WU_EVENT_SET__T2_16_C2C_WU_EN_get(x)       _BGQ_GET(1,49,x)
#define WU_DCR__THREAD2_WU_EVENT_SET__T2_16_C2C_WU_EN_set(v)       _BGQ_SET(1,49,v)
#define WU_DCR__THREAD2_WU_EVENT_SET__T2_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,49,x,v)

//   Field:       t3_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 2 on c2c interrupt from cores 0-15, targeted to thread 3 (W1S).
#define WU_DCR__THREAD2_WU_EVENT_SET__T3_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD2_WU_EVENT_SET__T3_0_15_C2C_WU_EN_position     50
#define WU_DCR__THREAD2_WU_EVENT_SET__T3_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,50,x)
#define WU_DCR__THREAD2_WU_EVENT_SET__T3_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,50,v)
#define WU_DCR__THREAD2_WU_EVENT_SET__T3_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,50,x,v)

//   Field:       t3_16_c2c_wu_en
//   Description: Enables wakeup of Thread 2 on c2c interrupt from core 16, targeted to thread 3 (W1S).
#define WU_DCR__THREAD2_WU_EVENT_SET__T3_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD2_WU_EVENT_SET__T3_16_C2C_WU_EN_position     51
#define WU_DCR__THREAD2_WU_EVENT_SET__T3_16_C2C_WU_EN_get(x)       _BGQ_GET(1,51,x)
#define WU_DCR__THREAD2_WU_EVENT_SET__T3_16_C2C_WU_EN_set(v)       _BGQ_SET(1,51,v)
#define WU_DCR__THREAD2_WU_EVENT_SET__T3_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,51,x,v)

//   Field:       mu_wu_en
//   Description: Enables wakeup of Thread 2 from Message Unit (W1S).
#define WU_DCR__THREAD2_WU_EVENT_SET__MU_WU_EN_width        4
#define WU_DCR__THREAD2_WU_EVENT_SET__MU_WU_EN_position     55
#define WU_DCR__THREAD2_WU_EVENT_SET__MU_WU_EN_get(x)       _BGQ_GET(4,55,x)
#define WU_DCR__THREAD2_WU_EVENT_SET__MU_WU_EN_set(v)       _BGQ_SET(4,55,v)
#define WU_DCR__THREAD2_WU_EVENT_SET__MU_WU_EN_insert(x,v)  _BGQ_INSERT(4,55,x,v)

//   Field:       gea_wu_en
//   Description: Enables wakeup of Thread 2 from GEA (W1S).
#define WU_DCR__THREAD2_WU_EVENT_SET__GEA_WU_EN_width        4
#define WU_DCR__THREAD2_WU_EVENT_SET__GEA_WU_EN_position     59
#define WU_DCR__THREAD2_WU_EVENT_SET__GEA_WU_EN_get(x)       _BGQ_GET(4,59,x)
#define WU_DCR__THREAD2_WU_EVENT_SET__GEA_WU_EN_set(v)       _BGQ_SET(4,59,v)
#define WU_DCR__THREAD2_WU_EVENT_SET__GEA_WU_EN_insert(x,v)  _BGQ_INSERT(4,59,x,v)

//   Field:       sw_wu_en
//   Description: Enables wakeup of Thread 2 when corresponding bits of sw_wu_status in this register are set (W1S).
#define WU_DCR__THREAD2_WU_EVENT_SET__SW_WU_EN_width        4
#define WU_DCR__THREAD2_WU_EVENT_SET__SW_WU_EN_position     63
#define WU_DCR__THREAD2_WU_EVENT_SET__SW_WU_EN_get(x)       _BGQ_GET(4,63,x)
#define WU_DCR__THREAD2_WU_EVENT_SET__SW_WU_EN_set(v)       _BGQ_SET(4,63,v)
#define WU_DCR__THREAD2_WU_EVENT_SET__SW_WU_EN_insert(x,v)  _BGQ_INSERT(4,63,x,v)

  
// Register:
//   Name:        thread2_wu_event_clr
//   Type:        Read/Write External
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Thread 2 Write-one-to-clear wakup event configuration/status register.
#define WU_DCR__THREAD2_WU_EVENT_CLR_offset  (0x03F4) // Offset of register in instance 
#define WU_DCR__THREAD2_WU_EVENT_CLR_range  (0x1) // Range of external address space
#define WU_DCR__THREAD2_WU_EVENT_CLR_reset  UNSIGNED64(0x0000000000000000)

//   Field:       wac_status
//   Description: Indicates that address comparators have detected a match, This bit remains set until cleared by software via the thread0_wu_event_clear register (W1C).
#define WU_DCR__THREAD2_WU_EVENT_CLR__WAC_STATUS_width        12
#define WU_DCR__THREAD2_WU_EVENT_CLR__WAC_STATUS_position     11
#define WU_DCR__THREAD2_WU_EVENT_CLR__WAC_STATUS_get(x)       _BGQ_GET(12,11,x)
#define WU_DCR__THREAD2_WU_EVENT_CLR__WAC_STATUS_set(v)       _BGQ_SET(12,11,v)
#define WU_DCR__THREAD2_WU_EVENT_CLR__WAC_STATUS_insert(x,v)  _BGQ_INSERT(12,11,x,v)

//   Field:       t0_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 0. This bit reflects the equivalent bit in the BIC (R0).
#define WU_DCR__THREAD2_WU_EVENT_CLR__T0_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD2_WU_EVENT_CLR__T0_0_15_C2C_WU_STATUS_position     12
#define WU_DCR__THREAD2_WU_EVENT_CLR__T0_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,12,x)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T0_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,12,v)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T0_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,12,x,v)

//   Field:       t0_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 0. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD2_WU_EVENT_CLR__T0_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD2_WU_EVENT_CLR__T0_16_C2C_WU_STATUS_position     13
#define WU_DCR__THREAD2_WU_EVENT_CLR__T0_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,13,x)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T0_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,13,v)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T0_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,13,x,v)

//   Field:       t1_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 1. This bit reflects the equivalent bit in the BIC (RO)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T1_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD2_WU_EVENT_CLR__T1_0_15_C2C_WU_STATUS_position     14
#define WU_DCR__THREAD2_WU_EVENT_CLR__T1_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,14,x)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T1_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,14,v)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T1_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,14,x,v)

//   Field:       t1_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 1. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD2_WU_EVENT_CLR__T1_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD2_WU_EVENT_CLR__T1_16_C2C_WU_STATUS_position     15
#define WU_DCR__THREAD2_WU_EVENT_CLR__T1_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,15,x)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T1_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,15,v)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T1_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,15,x,v)

//   Field:       t2_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 2. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD2_WU_EVENT_CLR__T2_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD2_WU_EVENT_CLR__T2_0_15_C2C_WU_STATUS_position     16
#define WU_DCR__THREAD2_WU_EVENT_CLR__T2_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,16,x)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T2_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,16,v)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T2_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,16,x,v)

//   Field:       t2_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 2. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD2_WU_EVENT_CLR__T2_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD2_WU_EVENT_CLR__T2_16_C2C_WU_STATUS_position     17
#define WU_DCR__THREAD2_WU_EVENT_CLR__T2_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,17,x)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T2_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,17,v)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T2_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,17,x,v)

//   Field:       t3_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 3. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD2_WU_EVENT_CLR__T3_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD2_WU_EVENT_CLR__T3_0_15_C2C_WU_STATUS_position     18
#define WU_DCR__THREAD2_WU_EVENT_CLR__T3_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,18,x)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T3_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,18,v)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T3_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,18,x,v)

//   Field:       t3_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 3. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD2_WU_EVENT_CLR__T3_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD2_WU_EVENT_CLR__T3_16_C2C_WU_STATUS_position     19
#define WU_DCR__THREAD2_WU_EVENT_CLR__T3_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,19,x)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T3_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,19,v)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T3_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,19,x,v)

//   Field:       mu_wu_status
//   Description: Indicates wakeup of request from Message Unit. This field reflects the incoming wakeup request from the MU, and is cleared in the MU (RO).
#define WU_DCR__THREAD2_WU_EVENT_CLR__MU_WU_STATUS_width        4
#define WU_DCR__THREAD2_WU_EVENT_CLR__MU_WU_STATUS_position     23
#define WU_DCR__THREAD2_WU_EVENT_CLR__MU_WU_STATUS_get(x)       _BGQ_GET(4,23,x)
#define WU_DCR__THREAD2_WU_EVENT_CLR__MU_WU_STATUS_set(v)       _BGQ_SET(4,23,v)
#define WU_DCR__THREAD2_WU_EVENT_CLR__MU_WU_STATUS_insert(x,v)  _BGQ_INSERT(4,23,x,v)

//   Field:       gea_wu_status
//   Description: Indicates wakeup request from GEA. This field reflects the incoming wakeup request from the GEA, and is cleared in the GEA (RO).
#define WU_DCR__THREAD2_WU_EVENT_CLR__GEA_WU_STATUS_width        4
#define WU_DCR__THREAD2_WU_EVENT_CLR__GEA_WU_STATUS_position     27
#define WU_DCR__THREAD2_WU_EVENT_CLR__GEA_WU_STATUS_get(x)       _BGQ_GET(4,27,x)
#define WU_DCR__THREAD2_WU_EVENT_CLR__GEA_WU_STATUS_set(v)       _BGQ_SET(4,27,v)
#define WU_DCR__THREAD2_WU_EVENT_CLR__GEA_WU_STATUS_insert(x,v)  _BGQ_INSERT(4,27,x,v)

//   Field:       sw_wu_status
//   Description: Used to generate wakeup events via software (W1C).
#define WU_DCR__THREAD2_WU_EVENT_CLR__SW_WU_STATUS_width        4
#define WU_DCR__THREAD2_WU_EVENT_CLR__SW_WU_STATUS_position     31
#define WU_DCR__THREAD2_WU_EVENT_CLR__SW_WU_STATUS_get(x)       _BGQ_GET(4,31,x)
#define WU_DCR__THREAD2_WU_EVENT_CLR__SW_WU_STATUS_set(v)       _BGQ_SET(4,31,v)
#define WU_DCR__THREAD2_WU_EVENT_CLR__SW_WU_STATUS_insert(x,v)  _BGQ_INSERT(4,31,x,v)

//   Field:       wac_en
//   Description: Enables wakeup of Thread 2 on each wakeup address compare match (W1C).
#define WU_DCR__THREAD2_WU_EVENT_CLR__WAC_EN_width        12
#define WU_DCR__THREAD2_WU_EVENT_CLR__WAC_EN_position     43
#define WU_DCR__THREAD2_WU_EVENT_CLR__WAC_EN_get(x)       _BGQ_GET(12,43,x)
#define WU_DCR__THREAD2_WU_EVENT_CLR__WAC_EN_set(v)       _BGQ_SET(12,43,v)
#define WU_DCR__THREAD2_WU_EVENT_CLR__WAC_EN_insert(x,v)  _BGQ_INSERT(12,43,x,v)

//   Field:       t0_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 2 on c2c interrupt from cores 0-15, targeted to thread 0 (W1C).
#define WU_DCR__THREAD2_WU_EVENT_CLR__T0_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD2_WU_EVENT_CLR__T0_0_15_C2C_WU_EN_position     44
#define WU_DCR__THREAD2_WU_EVENT_CLR__T0_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,44,x)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T0_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,44,v)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T0_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,44,x,v)

//   Field:       t0_16_c2c_wu_en
//   Description: Enables wakeup of Thread 2 on c2c interrupt from core 16, targeted to thread 0 (W1C).
#define WU_DCR__THREAD2_WU_EVENT_CLR__T0_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD2_WU_EVENT_CLR__T0_16_C2C_WU_EN_position     45
#define WU_DCR__THREAD2_WU_EVENT_CLR__T0_16_C2C_WU_EN_get(x)       _BGQ_GET(1,45,x)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T0_16_C2C_WU_EN_set(v)       _BGQ_SET(1,45,v)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T0_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,45,x,v)

//   Field:       t1_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 2 on c2c interrupt from cores 0-15, targeted to thread 1 (W1C).
#define WU_DCR__THREAD2_WU_EVENT_CLR__T1_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD2_WU_EVENT_CLR__T1_0_15_C2C_WU_EN_position     46
#define WU_DCR__THREAD2_WU_EVENT_CLR__T1_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,46,x)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T1_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,46,v)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T1_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,46,x,v)

//   Field:       t1_16_c2c_wu_en
//   Description: Enables wakeup of Thread 2 on c2c interrupt from core 16, targeted to thread 1 (W1C).
#define WU_DCR__THREAD2_WU_EVENT_CLR__T1_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD2_WU_EVENT_CLR__T1_16_C2C_WU_EN_position     47
#define WU_DCR__THREAD2_WU_EVENT_CLR__T1_16_C2C_WU_EN_get(x)       _BGQ_GET(1,47,x)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T1_16_C2C_WU_EN_set(v)       _BGQ_SET(1,47,v)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T1_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,47,x,v)

//   Field:       t2_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 2 on c2c interrupt from cores 0-15, targeted to thread 2 (W1C).
#define WU_DCR__THREAD2_WU_EVENT_CLR__T2_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD2_WU_EVENT_CLR__T2_0_15_C2C_WU_EN_position     48
#define WU_DCR__THREAD2_WU_EVENT_CLR__T2_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,48,x)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T2_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,48,v)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T2_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,48,x,v)

//   Field:       t2_16_c2c_wu_en
//   Description: Enables wakeup of Thread 2 on c2c interrupt from core 16, targeted to thread 2 (W1C).
#define WU_DCR__THREAD2_WU_EVENT_CLR__T2_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD2_WU_EVENT_CLR__T2_16_C2C_WU_EN_position     49
#define WU_DCR__THREAD2_WU_EVENT_CLR__T2_16_C2C_WU_EN_get(x)       _BGQ_GET(1,49,x)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T2_16_C2C_WU_EN_set(v)       _BGQ_SET(1,49,v)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T2_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,49,x,v)

//   Field:       t3_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 2 on c2c interrupt from cores 0-15, targeted to thread 3 (W1C).
#define WU_DCR__THREAD2_WU_EVENT_CLR__T3_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD2_WU_EVENT_CLR__T3_0_15_C2C_WU_EN_position     50
#define WU_DCR__THREAD2_WU_EVENT_CLR__T3_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,50,x)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T3_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,50,v)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T3_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,50,x,v)

//   Field:       t3_16_c2c_wu_en
//   Description: Enables wakeup of Thread 2 on c2c interrupt from core 16, targeted to thread 3 (W1C).
#define WU_DCR__THREAD2_WU_EVENT_CLR__T3_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD2_WU_EVENT_CLR__T3_16_C2C_WU_EN_position     51
#define WU_DCR__THREAD2_WU_EVENT_CLR__T3_16_C2C_WU_EN_get(x)       _BGQ_GET(1,51,x)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T3_16_C2C_WU_EN_set(v)       _BGQ_SET(1,51,v)
#define WU_DCR__THREAD2_WU_EVENT_CLR__T3_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,51,x,v)

//   Field:       mu_wu_en
//   Description: Enables wakeup of Thread 2 from Message Unit (W1C).
#define WU_DCR__THREAD2_WU_EVENT_CLR__MU_WU_EN_width        4
#define WU_DCR__THREAD2_WU_EVENT_CLR__MU_WU_EN_position     55
#define WU_DCR__THREAD2_WU_EVENT_CLR__MU_WU_EN_get(x)       _BGQ_GET(4,55,x)
#define WU_DCR__THREAD2_WU_EVENT_CLR__MU_WU_EN_set(v)       _BGQ_SET(4,55,v)
#define WU_DCR__THREAD2_WU_EVENT_CLR__MU_WU_EN_insert(x,v)  _BGQ_INSERT(4,55,x,v)

//   Field:       gea_wu_en
//   Description: Enables wakeup of Thread 2 from GEA (W1C).
#define WU_DCR__THREAD2_WU_EVENT_CLR__GEA_WU_EN_width        4
#define WU_DCR__THREAD2_WU_EVENT_CLR__GEA_WU_EN_position     59
#define WU_DCR__THREAD2_WU_EVENT_CLR__GEA_WU_EN_get(x)       _BGQ_GET(4,59,x)
#define WU_DCR__THREAD2_WU_EVENT_CLR__GEA_WU_EN_set(v)       _BGQ_SET(4,59,v)
#define WU_DCR__THREAD2_WU_EVENT_CLR__GEA_WU_EN_insert(x,v)  _BGQ_INSERT(4,59,x,v)

//   Field:       sw_wu_en
//   Description: Enables wakeup of Thread 2 when corresponding bits of sw_wu_status in this register are set (W1C).
#define WU_DCR__THREAD2_WU_EVENT_CLR__SW_WU_EN_width        4
#define WU_DCR__THREAD2_WU_EVENT_CLR__SW_WU_EN_position     63
#define WU_DCR__THREAD2_WU_EVENT_CLR__SW_WU_EN_get(x)       _BGQ_GET(4,63,x)
#define WU_DCR__THREAD2_WU_EVENT_CLR__SW_WU_EN_set(v)       _BGQ_SET(4,63,v)
#define WU_DCR__THREAD2_WU_EVENT_CLR__SW_WU_EN_insert(x,v)  _BGQ_INSERT(4,63,x,v)

  
// Register:
//   Name:        thread3_wu_event_set
//   Type:        Read/Write External
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Thread 3 Write-one-to-set wakup event configuration/status register.
#define WU_DCR__THREAD3_WU_EVENT_SET_offset  (0x03F8) // Offset of register in instance 
#define WU_DCR__THREAD3_WU_EVENT_SET_range  (0x1) // Range of external address space
#define WU_DCR__THREAD3_WU_EVENT_SET_reset  UNSIGNED64(0x0000000000000000)

//   Field:       wac_status
//   Description: Indicates that address comparators have detected a match, This bit remains set until cleared by software via the thread0_wu_event_clear register (W1S).
#define WU_DCR__THREAD3_WU_EVENT_SET__WAC_STATUS_width        12
#define WU_DCR__THREAD3_WU_EVENT_SET__WAC_STATUS_position     11
#define WU_DCR__THREAD3_WU_EVENT_SET__WAC_STATUS_get(x)       _BGQ_GET(12,11,x)
#define WU_DCR__THREAD3_WU_EVENT_SET__WAC_STATUS_set(v)       _BGQ_SET(12,11,v)
#define WU_DCR__THREAD3_WU_EVENT_SET__WAC_STATUS_insert(x,v)  _BGQ_INSERT(12,11,x,v)

//   Field:       t0_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 0. This bit reflects the equivalent bit in the BIC (R0).
#define WU_DCR__THREAD3_WU_EVENT_SET__T0_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD3_WU_EVENT_SET__T0_0_15_C2C_WU_STATUS_position     12
#define WU_DCR__THREAD3_WU_EVENT_SET__T0_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,12,x)
#define WU_DCR__THREAD3_WU_EVENT_SET__T0_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,12,v)
#define WU_DCR__THREAD3_WU_EVENT_SET__T0_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,12,x,v)

//   Field:       t0_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 0. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD3_WU_EVENT_SET__T0_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD3_WU_EVENT_SET__T0_16_C2C_WU_STATUS_position     13
#define WU_DCR__THREAD3_WU_EVENT_SET__T0_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,13,x)
#define WU_DCR__THREAD3_WU_EVENT_SET__T0_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,13,v)
#define WU_DCR__THREAD3_WU_EVENT_SET__T0_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,13,x,v)

//   Field:       t1_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 1. This bit reflects the equivalent bit in the BIC (RO)
#define WU_DCR__THREAD3_WU_EVENT_SET__T1_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD3_WU_EVENT_SET__T1_0_15_C2C_WU_STATUS_position     14
#define WU_DCR__THREAD3_WU_EVENT_SET__T1_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,14,x)
#define WU_DCR__THREAD3_WU_EVENT_SET__T1_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,14,v)
#define WU_DCR__THREAD3_WU_EVENT_SET__T1_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,14,x,v)

//   Field:       t1_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 1. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD3_WU_EVENT_SET__T1_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD3_WU_EVENT_SET__T1_16_C2C_WU_STATUS_position     15
#define WU_DCR__THREAD3_WU_EVENT_SET__T1_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,15,x)
#define WU_DCR__THREAD3_WU_EVENT_SET__T1_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,15,v)
#define WU_DCR__THREAD3_WU_EVENT_SET__T1_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,15,x,v)

//   Field:       t2_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 2. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD3_WU_EVENT_SET__T2_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD3_WU_EVENT_SET__T2_0_15_C2C_WU_STATUS_position     16
#define WU_DCR__THREAD3_WU_EVENT_SET__T2_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,16,x)
#define WU_DCR__THREAD3_WU_EVENT_SET__T2_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,16,v)
#define WU_DCR__THREAD3_WU_EVENT_SET__T2_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,16,x,v)

//   Field:       t2_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 2. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD3_WU_EVENT_SET__T2_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD3_WU_EVENT_SET__T2_16_C2C_WU_STATUS_position     17
#define WU_DCR__THREAD3_WU_EVENT_SET__T2_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,17,x)
#define WU_DCR__THREAD3_WU_EVENT_SET__T2_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,17,v)
#define WU_DCR__THREAD3_WU_EVENT_SET__T2_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,17,x,v)

//   Field:       t3_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 3. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD3_WU_EVENT_SET__T3_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD3_WU_EVENT_SET__T3_0_15_C2C_WU_STATUS_position     18
#define WU_DCR__THREAD3_WU_EVENT_SET__T3_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,18,x)
#define WU_DCR__THREAD3_WU_EVENT_SET__T3_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,18,v)
#define WU_DCR__THREAD3_WU_EVENT_SET__T3_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,18,x,v)

//   Field:       t3_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 3. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD3_WU_EVENT_SET__T3_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD3_WU_EVENT_SET__T3_16_C2C_WU_STATUS_position     19
#define WU_DCR__THREAD3_WU_EVENT_SET__T3_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,19,x)
#define WU_DCR__THREAD3_WU_EVENT_SET__T3_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,19,v)
#define WU_DCR__THREAD3_WU_EVENT_SET__T3_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,19,x,v)

//   Field:       mu_wu_status
//   Description: Indicates wakeup of request from Message Unit. This field reflects the incoming wakeup request from the MU, and is cleared in the MU (RO).
#define WU_DCR__THREAD3_WU_EVENT_SET__MU_WU_STATUS_width        4
#define WU_DCR__THREAD3_WU_EVENT_SET__MU_WU_STATUS_position     23
#define WU_DCR__THREAD3_WU_EVENT_SET__MU_WU_STATUS_get(x)       _BGQ_GET(4,23,x)
#define WU_DCR__THREAD3_WU_EVENT_SET__MU_WU_STATUS_set(v)       _BGQ_SET(4,23,v)
#define WU_DCR__THREAD3_WU_EVENT_SET__MU_WU_STATUS_insert(x,v)  _BGQ_INSERT(4,23,x,v)

//   Field:       gea_wu_status
//   Description: Indicates wakeup request from GEA. This field reflects the incoming wakeup request from the GEA, and is cleared in the GEA (RO).
#define WU_DCR__THREAD3_WU_EVENT_SET__GEA_WU_STATUS_width        4
#define WU_DCR__THREAD3_WU_EVENT_SET__GEA_WU_STATUS_position     27
#define WU_DCR__THREAD3_WU_EVENT_SET__GEA_WU_STATUS_get(x)       _BGQ_GET(4,27,x)
#define WU_DCR__THREAD3_WU_EVENT_SET__GEA_WU_STATUS_set(v)       _BGQ_SET(4,27,v)
#define WU_DCR__THREAD3_WU_EVENT_SET__GEA_WU_STATUS_insert(x,v)  _BGQ_INSERT(4,27,x,v)

//   Field:       sw_wu_status
//   Description: Used to generate wakeup events via software (W1S).
#define WU_DCR__THREAD3_WU_EVENT_SET__SW_WU_STATUS_width        4
#define WU_DCR__THREAD3_WU_EVENT_SET__SW_WU_STATUS_position     31
#define WU_DCR__THREAD3_WU_EVENT_SET__SW_WU_STATUS_get(x)       _BGQ_GET(4,31,x)
#define WU_DCR__THREAD3_WU_EVENT_SET__SW_WU_STATUS_set(v)       _BGQ_SET(4,31,v)
#define WU_DCR__THREAD3_WU_EVENT_SET__SW_WU_STATUS_insert(x,v)  _BGQ_INSERT(4,31,x,v)

//   Field:       wac_en
//   Description: Enables wakeup of Thread 3 on each wakeup address compare match (W1S).
#define WU_DCR__THREAD3_WU_EVENT_SET__WAC_EN_width        12
#define WU_DCR__THREAD3_WU_EVENT_SET__WAC_EN_position     43
#define WU_DCR__THREAD3_WU_EVENT_SET__WAC_EN_get(x)       _BGQ_GET(12,43,x)
#define WU_DCR__THREAD3_WU_EVENT_SET__WAC_EN_set(v)       _BGQ_SET(12,43,v)
#define WU_DCR__THREAD3_WU_EVENT_SET__WAC_EN_insert(x,v)  _BGQ_INSERT(12,43,x,v)

//   Field:       t0_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 3 on c2c interrupt from cores 0-15, targeted to thread 0 (W1S).
#define WU_DCR__THREAD3_WU_EVENT_SET__T0_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD3_WU_EVENT_SET__T0_0_15_C2C_WU_EN_position     44
#define WU_DCR__THREAD3_WU_EVENT_SET__T0_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,44,x)
#define WU_DCR__THREAD3_WU_EVENT_SET__T0_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,44,v)
#define WU_DCR__THREAD3_WU_EVENT_SET__T0_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,44,x,v)

//   Field:       t0_16_c2c_wu_en
//   Description: Enables wakeup of Thread 3 on c2c interrupt from core 16, targeted to thread 0 (W1S).
#define WU_DCR__THREAD3_WU_EVENT_SET__T0_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD3_WU_EVENT_SET__T0_16_C2C_WU_EN_position     45
#define WU_DCR__THREAD3_WU_EVENT_SET__T0_16_C2C_WU_EN_get(x)       _BGQ_GET(1,45,x)
#define WU_DCR__THREAD3_WU_EVENT_SET__T0_16_C2C_WU_EN_set(v)       _BGQ_SET(1,45,v)
#define WU_DCR__THREAD3_WU_EVENT_SET__T0_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,45,x,v)

//   Field:       t1_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 3 on c2c interrupt from cores 0-15, targeted to thread 1 (W1S).
#define WU_DCR__THREAD3_WU_EVENT_SET__T1_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD3_WU_EVENT_SET__T1_0_15_C2C_WU_EN_position     46
#define WU_DCR__THREAD3_WU_EVENT_SET__T1_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,46,x)
#define WU_DCR__THREAD3_WU_EVENT_SET__T1_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,46,v)
#define WU_DCR__THREAD3_WU_EVENT_SET__T1_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,46,x,v)

//   Field:       t1_16_c2c_wu_en
//   Description: Enables wakeup of Thread 3 on c2c interrupt from core 16, targeted to thread 1 (W1S).
#define WU_DCR__THREAD3_WU_EVENT_SET__T1_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD3_WU_EVENT_SET__T1_16_C2C_WU_EN_position     47
#define WU_DCR__THREAD3_WU_EVENT_SET__T1_16_C2C_WU_EN_get(x)       _BGQ_GET(1,47,x)
#define WU_DCR__THREAD3_WU_EVENT_SET__T1_16_C2C_WU_EN_set(v)       _BGQ_SET(1,47,v)
#define WU_DCR__THREAD3_WU_EVENT_SET__T1_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,47,x,v)

//   Field:       t2_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 3 on c2c interrupt from cores 0-15, targeted to thread 2 (W1S).
#define WU_DCR__THREAD3_WU_EVENT_SET__T2_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD3_WU_EVENT_SET__T2_0_15_C2C_WU_EN_position     48
#define WU_DCR__THREAD3_WU_EVENT_SET__T2_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,48,x)
#define WU_DCR__THREAD3_WU_EVENT_SET__T2_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,48,v)
#define WU_DCR__THREAD3_WU_EVENT_SET__T2_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,48,x,v)

//   Field:       t2_16_c2c_wu_en
//   Description: Enables wakeup of Thread 3 on c2c interrupt from core 16, targeted to thread 2 (W1S).
#define WU_DCR__THREAD3_WU_EVENT_SET__T2_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD3_WU_EVENT_SET__T2_16_C2C_WU_EN_position     49
#define WU_DCR__THREAD3_WU_EVENT_SET__T2_16_C2C_WU_EN_get(x)       _BGQ_GET(1,49,x)
#define WU_DCR__THREAD3_WU_EVENT_SET__T2_16_C2C_WU_EN_set(v)       _BGQ_SET(1,49,v)
#define WU_DCR__THREAD3_WU_EVENT_SET__T2_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,49,x,v)

//   Field:       t3_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 3 on c2c interrupt from cores 0-15, targeted to thread 3 (W1S).
#define WU_DCR__THREAD3_WU_EVENT_SET__T3_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD3_WU_EVENT_SET__T3_0_15_C2C_WU_EN_position     50
#define WU_DCR__THREAD3_WU_EVENT_SET__T3_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,50,x)
#define WU_DCR__THREAD3_WU_EVENT_SET__T3_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,50,v)
#define WU_DCR__THREAD3_WU_EVENT_SET__T3_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,50,x,v)

//   Field:       t3_16_c2c_wu_en
//   Description: Enables wakeup of Thread 3 on c2c interrupt from core 16, targeted to thread 3 (W1S).
#define WU_DCR__THREAD3_WU_EVENT_SET__T3_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD3_WU_EVENT_SET__T3_16_C2C_WU_EN_position     51
#define WU_DCR__THREAD3_WU_EVENT_SET__T3_16_C2C_WU_EN_get(x)       _BGQ_GET(1,51,x)
#define WU_DCR__THREAD3_WU_EVENT_SET__T3_16_C2C_WU_EN_set(v)       _BGQ_SET(1,51,v)
#define WU_DCR__THREAD3_WU_EVENT_SET__T3_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,51,x,v)

//   Field:       mu_wu_en
//   Description: Enables wakeup of Thread 3 from Message Unit (W1S).
#define WU_DCR__THREAD3_WU_EVENT_SET__MU_WU_EN_width        4
#define WU_DCR__THREAD3_WU_EVENT_SET__MU_WU_EN_position     55
#define WU_DCR__THREAD3_WU_EVENT_SET__MU_WU_EN_get(x)       _BGQ_GET(4,55,x)
#define WU_DCR__THREAD3_WU_EVENT_SET__MU_WU_EN_set(v)       _BGQ_SET(4,55,v)
#define WU_DCR__THREAD3_WU_EVENT_SET__MU_WU_EN_insert(x,v)  _BGQ_INSERT(4,55,x,v)

//   Field:       gea_wu_en
//   Description: Enables wakeup of Thread 3 from GEA (W1S).
#define WU_DCR__THREAD3_WU_EVENT_SET__GEA_WU_EN_width        4
#define WU_DCR__THREAD3_WU_EVENT_SET__GEA_WU_EN_position     59
#define WU_DCR__THREAD3_WU_EVENT_SET__GEA_WU_EN_get(x)       _BGQ_GET(4,59,x)
#define WU_DCR__THREAD3_WU_EVENT_SET__GEA_WU_EN_set(v)       _BGQ_SET(4,59,v)
#define WU_DCR__THREAD3_WU_EVENT_SET__GEA_WU_EN_insert(x,v)  _BGQ_INSERT(4,59,x,v)

//   Field:       sw_wu_en
//   Description: Enables wakeup of Thread 3 when corresponding bits of sw_wu_status in this register are set (W1S).
#define WU_DCR__THREAD3_WU_EVENT_SET__SW_WU_EN_width        4
#define WU_DCR__THREAD3_WU_EVENT_SET__SW_WU_EN_position     63
#define WU_DCR__THREAD3_WU_EVENT_SET__SW_WU_EN_get(x)       _BGQ_GET(4,63,x)
#define WU_DCR__THREAD3_WU_EVENT_SET__SW_WU_EN_set(v)       _BGQ_SET(4,63,v)
#define WU_DCR__THREAD3_WU_EVENT_SET__SW_WU_EN_insert(x,v)  _BGQ_INSERT(4,63,x,v)

  
// Register:
//   Name:        thread3_wu_event_clr
//   Type:        Read/Write External
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Thread 3 Write-one-to-clear wakup event configuration/status register.
#define WU_DCR__THREAD3_WU_EVENT_CLR_offset  (0x03FC) // Offset of register in instance 
#define WU_DCR__THREAD3_WU_EVENT_CLR_range  (0x1) // Range of external address space
#define WU_DCR__THREAD3_WU_EVENT_CLR_reset  UNSIGNED64(0x0000000000000000)

//   Field:       wac_status
//   Description: Indicates that address comparators have detected a match, This bit remains set until cleared by software via the thread0_wu_event_clear register (W1C). 
#define WU_DCR__THREAD3_WU_EVENT_CLR__WAC_STATUS_width        12
#define WU_DCR__THREAD3_WU_EVENT_CLR__WAC_STATUS_position     11
#define WU_DCR__THREAD3_WU_EVENT_CLR__WAC_STATUS_get(x)       _BGQ_GET(12,11,x)
#define WU_DCR__THREAD3_WU_EVENT_CLR__WAC_STATUS_set(v)       _BGQ_SET(12,11,v)
#define WU_DCR__THREAD3_WU_EVENT_CLR__WAC_STATUS_insert(x,v)  _BGQ_INSERT(12,11,x,v)

//   Field:       t0_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 0. This bit reflects the equivalent bit in the BIC (R0).
#define WU_DCR__THREAD3_WU_EVENT_CLR__T0_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD3_WU_EVENT_CLR__T0_0_15_C2C_WU_STATUS_position     12
#define WU_DCR__THREAD3_WU_EVENT_CLR__T0_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,12,x)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T0_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,12,v)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T0_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,12,x,v)

//   Field:       t0_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 0. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD3_WU_EVENT_CLR__T0_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD3_WU_EVENT_CLR__T0_16_C2C_WU_STATUS_position     13
#define WU_DCR__THREAD3_WU_EVENT_CLR__T0_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,13,x)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T0_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,13,v)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T0_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,13,x,v)

//   Field:       t1_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 1. This bit reflects the equivalent bit in the BIC (RO)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T1_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD3_WU_EVENT_CLR__T1_0_15_C2C_WU_STATUS_position     14
#define WU_DCR__THREAD3_WU_EVENT_CLR__T1_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,14,x)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T1_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,14,v)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T1_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,14,x,v)

//   Field:       t1_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 1. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD3_WU_EVENT_CLR__T1_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD3_WU_EVENT_CLR__T1_16_C2C_WU_STATUS_position     15
#define WU_DCR__THREAD3_WU_EVENT_CLR__T1_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,15,x)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T1_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,15,v)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T1_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,15,x,v)

//   Field:       t2_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 2. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD3_WU_EVENT_CLR__T2_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD3_WU_EVENT_CLR__T2_0_15_C2C_WU_STATUS_position     16
#define WU_DCR__THREAD3_WU_EVENT_CLR__T2_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,16,x)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T2_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,16,v)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T2_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,16,x,v)

//   Field:       t2_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 2. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD3_WU_EVENT_CLR__T2_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD3_WU_EVENT_CLR__T2_16_C2C_WU_STATUS_position     17
#define WU_DCR__THREAD3_WU_EVENT_CLR__T2_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,17,x)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T2_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,17,v)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T2_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,17,x,v)

//   Field:       t3_0_15_c2c_wu_status
//   Description: Indicates c2c interrupt from cores 0-15, targeted to thread 3. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD3_WU_EVENT_CLR__T3_0_15_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD3_WU_EVENT_CLR__T3_0_15_C2C_WU_STATUS_position     18
#define WU_DCR__THREAD3_WU_EVENT_CLR__T3_0_15_C2C_WU_STATUS_get(x)       _BGQ_GET(1,18,x)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T3_0_15_C2C_WU_STATUS_set(v)       _BGQ_SET(1,18,v)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T3_0_15_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,18,x,v)

//   Field:       t3_16_c2c_wu_status
//   Description: Indicates c2c interrupt from core 16, targeted to thread 3. This bit reflects the equivalent bit in the BIC (RO).
#define WU_DCR__THREAD3_WU_EVENT_CLR__T3_16_C2C_WU_STATUS_width        1
#define WU_DCR__THREAD3_WU_EVENT_CLR__T3_16_C2C_WU_STATUS_position     19
#define WU_DCR__THREAD3_WU_EVENT_CLR__T3_16_C2C_WU_STATUS_get(x)       _BGQ_GET(1,19,x)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T3_16_C2C_WU_STATUS_set(v)       _BGQ_SET(1,19,v)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T3_16_C2C_WU_STATUS_insert(x,v)  _BGQ_INSERT(1,19,x,v)

//   Field:       mu_wu_status
//   Description: Indicates request from Message Unit. This field reflects the incoming wakeup request from the MU, and is cleared in the MU (RO).
#define WU_DCR__THREAD3_WU_EVENT_CLR__MU_WU_STATUS_width        4
#define WU_DCR__THREAD3_WU_EVENT_CLR__MU_WU_STATUS_position     23
#define WU_DCR__THREAD3_WU_EVENT_CLR__MU_WU_STATUS_get(x)       _BGQ_GET(4,23,x)
#define WU_DCR__THREAD3_WU_EVENT_CLR__MU_WU_STATUS_set(v)       _BGQ_SET(4,23,v)
#define WU_DCR__THREAD3_WU_EVENT_CLR__MU_WU_STATUS_insert(x,v)  _BGQ_INSERT(4,23,x,v)

//   Field:       gea_wu_status
//   Description: Indicates wakeup request from GEA. This field reflects the incoming wakeup request from the GEA, and is cleared in the GEA (RO).
#define WU_DCR__THREAD3_WU_EVENT_CLR__GEA_WU_STATUS_width        4
#define WU_DCR__THREAD3_WU_EVENT_CLR__GEA_WU_STATUS_position     27
#define WU_DCR__THREAD3_WU_EVENT_CLR__GEA_WU_STATUS_get(x)       _BGQ_GET(4,27,x)
#define WU_DCR__THREAD3_WU_EVENT_CLR__GEA_WU_STATUS_set(v)       _BGQ_SET(4,27,v)
#define WU_DCR__THREAD3_WU_EVENT_CLR__GEA_WU_STATUS_insert(x,v)  _BGQ_INSERT(4,27,x,v)

//   Field:       sw_wu_status
//   Description: Used to generate wakeup events via software (W1C).
#define WU_DCR__THREAD3_WU_EVENT_CLR__SW_WU_STATUS_width        4
#define WU_DCR__THREAD3_WU_EVENT_CLR__SW_WU_STATUS_position     31
#define WU_DCR__THREAD3_WU_EVENT_CLR__SW_WU_STATUS_get(x)       _BGQ_GET(4,31,x)
#define WU_DCR__THREAD3_WU_EVENT_CLR__SW_WU_STATUS_set(v)       _BGQ_SET(4,31,v)
#define WU_DCR__THREAD3_WU_EVENT_CLR__SW_WU_STATUS_insert(x,v)  _BGQ_INSERT(4,31,x,v)

//   Field:       wac_en
//   Description: Enables wakeup of Thread 3 on each wakeup address compare match (W1C).
#define WU_DCR__THREAD3_WU_EVENT_CLR__WAC_EN_width        12
#define WU_DCR__THREAD3_WU_EVENT_CLR__WAC_EN_position     43
#define WU_DCR__THREAD3_WU_EVENT_CLR__WAC_EN_get(x)       _BGQ_GET(12,43,x)
#define WU_DCR__THREAD3_WU_EVENT_CLR__WAC_EN_set(v)       _BGQ_SET(12,43,v)
#define WU_DCR__THREAD3_WU_EVENT_CLR__WAC_EN_insert(x,v)  _BGQ_INSERT(12,43,x,v)

//   Field:       t0_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 3 on c2c interrupt from cores 0-15, targeted to thread 0 (W1C).
#define WU_DCR__THREAD3_WU_EVENT_CLR__T0_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD3_WU_EVENT_CLR__T0_0_15_C2C_WU_EN_position     44
#define WU_DCR__THREAD3_WU_EVENT_CLR__T0_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,44,x)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T0_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,44,v)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T0_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,44,x,v)

//   Field:       t0_16_c2c_wu_en
//   Description: Enables wakeup of Thread 3 on c2c interrupt from core 16, targeted to thread 0 (W1C).
#define WU_DCR__THREAD3_WU_EVENT_CLR__T0_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD3_WU_EVENT_CLR__T0_16_C2C_WU_EN_position     45
#define WU_DCR__THREAD3_WU_EVENT_CLR__T0_16_C2C_WU_EN_get(x)       _BGQ_GET(1,45,x)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T0_16_C2C_WU_EN_set(v)       _BGQ_SET(1,45,v)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T0_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,45,x,v)

//   Field:       t1_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 3 on c2c interrupt from cores 0-15, targeted to thread 1 (W1C).
#define WU_DCR__THREAD3_WU_EVENT_CLR__T1_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD3_WU_EVENT_CLR__T1_0_15_C2C_WU_EN_position     46
#define WU_DCR__THREAD3_WU_EVENT_CLR__T1_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,46,x)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T1_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,46,v)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T1_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,46,x,v)

//   Field:       t1_16_c2c_wu_en
//   Description: Enables wakeup of Thread 3 on c2c interrupt from core 16, targeted to thread 1 (W1C).
#define WU_DCR__THREAD3_WU_EVENT_CLR__T1_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD3_WU_EVENT_CLR__T1_16_C2C_WU_EN_position     47
#define WU_DCR__THREAD3_WU_EVENT_CLR__T1_16_C2C_WU_EN_get(x)       _BGQ_GET(1,47,x)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T1_16_C2C_WU_EN_set(v)       _BGQ_SET(1,47,v)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T1_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,47,x,v)

//   Field:       t2_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 3 on c2c interrupt from cores 0-15, targeted to thread 2 (W1C).
#define WU_DCR__THREAD3_WU_EVENT_CLR__T2_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD3_WU_EVENT_CLR__T2_0_15_C2C_WU_EN_position     48
#define WU_DCR__THREAD3_WU_EVENT_CLR__T2_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,48,x)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T2_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,48,v)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T2_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,48,x,v)

//   Field:       t2_16_c2c_wu_en
//   Description: Enables wakeup of Thread 3 on c2c interrupt from core 16, targeted to thread 2 (W1C).
#define WU_DCR__THREAD3_WU_EVENT_CLR__T2_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD3_WU_EVENT_CLR__T2_16_C2C_WU_EN_position     49
#define WU_DCR__THREAD3_WU_EVENT_CLR__T2_16_C2C_WU_EN_get(x)       _BGQ_GET(1,49,x)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T2_16_C2C_WU_EN_set(v)       _BGQ_SET(1,49,v)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T2_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,49,x,v)

//   Field:       t3_0_15_c2c_wu_en
//   Description: Enables wakeup of Thread 3 on c2c interrupt from cores 0-15, targeted to thread 3 (W1C).
#define WU_DCR__THREAD3_WU_EVENT_CLR__T3_0_15_C2C_WU_EN_width        1
#define WU_DCR__THREAD3_WU_EVENT_CLR__T3_0_15_C2C_WU_EN_position     50
#define WU_DCR__THREAD3_WU_EVENT_CLR__T3_0_15_C2C_WU_EN_get(x)       _BGQ_GET(1,50,x)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T3_0_15_C2C_WU_EN_set(v)       _BGQ_SET(1,50,v)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T3_0_15_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,50,x,v)

//   Field:       t3_16_c2c_wu_en
//   Description: Enables wakeup of Thread 3 on c2c interrupt from core 16, targeted to thread 3 (W1C).
#define WU_DCR__THREAD3_WU_EVENT_CLR__T3_16_C2C_WU_EN_width        1
#define WU_DCR__THREAD3_WU_EVENT_CLR__T3_16_C2C_WU_EN_position     51
#define WU_DCR__THREAD3_WU_EVENT_CLR__T3_16_C2C_WU_EN_get(x)       _BGQ_GET(1,51,x)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T3_16_C2C_WU_EN_set(v)       _BGQ_SET(1,51,v)
#define WU_DCR__THREAD3_WU_EVENT_CLR__T3_16_C2C_WU_EN_insert(x,v)  _BGQ_INSERT(1,51,x,v)

//   Field:       mu_wu_en
//   Description: Enables wakeup of Thread 3 from Message Unit (W1C).
#define WU_DCR__THREAD3_WU_EVENT_CLR__MU_WU_EN_width        4
#define WU_DCR__THREAD3_WU_EVENT_CLR__MU_WU_EN_position     55
#define WU_DCR__THREAD3_WU_EVENT_CLR__MU_WU_EN_get(x)       _BGQ_GET(4,55,x)
#define WU_DCR__THREAD3_WU_EVENT_CLR__MU_WU_EN_set(v)       _BGQ_SET(4,55,v)
#define WU_DCR__THREAD3_WU_EVENT_CLR__MU_WU_EN_insert(x,v)  _BGQ_INSERT(4,55,x,v)

//   Field:       gea_wu_en
//   Description: Enables wakeup of Thread 3 from GEA (W1C).
#define WU_DCR__THREAD3_WU_EVENT_CLR__GEA_WU_EN_width        4
#define WU_DCR__THREAD3_WU_EVENT_CLR__GEA_WU_EN_position     59
#define WU_DCR__THREAD3_WU_EVENT_CLR__GEA_WU_EN_get(x)       _BGQ_GET(4,59,x)
#define WU_DCR__THREAD3_WU_EVENT_CLR__GEA_WU_EN_set(v)       _BGQ_SET(4,59,v)
#define WU_DCR__THREAD3_WU_EVENT_CLR__GEA_WU_EN_insert(x,v)  _BGQ_INSERT(4,59,x,v)

//   Field:       sw_wu_en
//   Description: Enables wakeup of Thread 3 when corresponding bits of sw_wu_status in this register are set (W1C).
#define WU_DCR__THREAD3_WU_EVENT_CLR__SW_WU_EN_width        4
#define WU_DCR__THREAD3_WU_EVENT_CLR__SW_WU_EN_position     63
#define WU_DCR__THREAD3_WU_EVENT_CLR__SW_WU_EN_get(x)       _BGQ_GET(4,63,x)
#define WU_DCR__THREAD3_WU_EVENT_CLR__SW_WU_EN_set(v)       _BGQ_SET(4,63,v)
#define WU_DCR__THREAD3_WU_EVENT_CLR__SW_WU_EN_insert(x,v)  _BGQ_INSERT(4,63,x,v)

  
// Register:
//   Name:        int_event_set
//   Type:        Read/Write External
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Write-one-to-set interrupt event configuration/status register.
#define WU_DCR__INT_EVENT_SET_offset  (0x03BD) // Offset of register in instance 
#define WU_DCR__INT_EVENT_SET_range  (0x1) // Range of external address space
#define WU_DCR__INT_EVENT_SET_reset  UNSIGNED64(0x0000000000000000)

//   Field:       wac_status
//   Description: Indicates that address comparators 0-3 have detected a match, This bit remains set until cleared by software via the thread0_int_event_clear register (W1S). 
#define WU_DCR__INT_EVENT_SET__WAC_STATUS_width        12
#define WU_DCR__INT_EVENT_SET__WAC_STATUS_position     11
#define WU_DCR__INT_EVENT_SET__WAC_STATUS_get(x)       _BGQ_GET(12,11,x)
#define WU_DCR__INT_EVENT_SET__WAC_STATUS_set(v)       _BGQ_SET(12,11,v)
#define WU_DCR__INT_EVENT_SET__WAC_STATUS_insert(x,v)  _BGQ_INSERT(12,11,x,v)

//   Field:       wac_en
//   Description: Enables interrupt to BIC on each wakeup address compare match 0-11 (W1S).
#define WU_DCR__INT_EVENT_SET__WAC_EN_width        12
#define WU_DCR__INT_EVENT_SET__WAC_EN_position     43
#define WU_DCR__INT_EVENT_SET__WAC_EN_get(x)       _BGQ_GET(12,43,x)
#define WU_DCR__INT_EVENT_SET__WAC_EN_set(v)       _BGQ_SET(12,43,v)
#define WU_DCR__INT_EVENT_SET__WAC_EN_insert(x,v)  _BGQ_INSERT(12,43,x,v)

  
// Register:
//   Name:        int_event_clr
//   Type:        Read/Write External
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Write-one-to-clear interrupt event configuration/status register.
#define WU_DCR__INT_EVENT_CLR_offset  (0x03BE) // Offset of register in instance 
#define WU_DCR__INT_EVENT_CLR_range  (0x1) // Range of external address space
#define WU_DCR__INT_EVENT_CLR_reset  UNSIGNED64(0x0000000000000000)

//   Field:       wac_status
//   Description: Indicates that address comparators 0-3 have detected a match, This bit remains set until cleared by software via the thread0_wu_event_clear register (W1C). 
#define WU_DCR__INT_EVENT_CLR__WAC_STATUS_width        12
#define WU_DCR__INT_EVENT_CLR__WAC_STATUS_position     11
#define WU_DCR__INT_EVENT_CLR__WAC_STATUS_get(x)       _BGQ_GET(12,11,x)
#define WU_DCR__INT_EVENT_CLR__WAC_STATUS_set(v)       _BGQ_SET(12,11,v)
#define WU_DCR__INT_EVENT_CLR__WAC_STATUS_insert(x,v)  _BGQ_INSERT(12,11,x,v)

//   Field:       wac_en
//   Description: Enables interrupt to BIC on each wakeup address compare match 0-11 (W1C).
#define WU_DCR__INT_EVENT_CLR__WAC_EN_width        12
#define WU_DCR__INT_EVENT_CLR__WAC_EN_position     43
#define WU_DCR__INT_EVENT_CLR__WAC_EN_get(x)       _BGQ_GET(12,43,x)
#define WU_DCR__INT_EVENT_CLR__WAC_EN_set(v)       _BGQ_SET(12,43,v)
#define WU_DCR__INT_EVENT_CLR__WAC_EN_insert(x,v)  _BGQ_INSERT(12,43,x,v)

  
// Register:
//   Name:        wac_ttype_config
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Ttype Mask/Match configuration register
#define WU_DCR__WAC_TTYPE_CONFIG_offset  (0x03DA) // Offset of register in instance 
#define WU_DCR__WAC_TTYPE_CONFIG_reset  UNSIGNED64(0x0000000000000000)

//   Field:       invalidate_en
//   Description: Allows wac to compare incoming invalidate events when set. When 0, invalidates are ignored by all WAC units
#define WU_DCR__WAC_TTYPE_CONFIG__INVALIDATE_EN_width        1
#define WU_DCR__WAC_TTYPE_CONFIG__INVALIDATE_EN_position     33
#define WU_DCR__WAC_TTYPE_CONFIG__INVALIDATE_EN_get(x)       _BGQ_GET(1,33,x)
#define WU_DCR__WAC_TTYPE_CONFIG__INVALIDATE_EN_set(v)       _BGQ_SET(1,33,v)
#define WU_DCR__WAC_TTYPE_CONFIG__INVALIDATE_EN_insert(x,v)  _BGQ_INSERT(1,33,x,v)

//   Field:       ttype_include_match
//   Description: Match value for include ttype compare.
#define WU_DCR__WAC_TTYPE_CONFIG__TTYPE_INCLUDE_MATCH_width        6
#define WU_DCR__WAC_TTYPE_CONFIG__TTYPE_INCLUDE_MATCH_position     39
#define WU_DCR__WAC_TTYPE_CONFIG__TTYPE_INCLUDE_MATCH_get(x)       _BGQ_GET(6,39,x)
#define WU_DCR__WAC_TTYPE_CONFIG__TTYPE_INCLUDE_MATCH_set(v)       _BGQ_SET(6,39,v)
#define WU_DCR__WAC_TTYPE_CONFIG__TTYPE_INCLUDE_MATCH_insert(x,v)  _BGQ_INSERT(6,39,x,v)

//   Field:       ttype_include_mask
//   Description: Mask value for include ttype compare. Set a bit 1 one to include  bit in comparison, 0 to ignore.
#define WU_DCR__WAC_TTYPE_CONFIG__TTYPE_INCLUDE_MASK_width        6
#define WU_DCR__WAC_TTYPE_CONFIG__TTYPE_INCLUDE_MASK_position     47
#define WU_DCR__WAC_TTYPE_CONFIG__TTYPE_INCLUDE_MASK_get(x)       _BGQ_GET(6,47,x)
#define WU_DCR__WAC_TTYPE_CONFIG__TTYPE_INCLUDE_MASK_set(v)       _BGQ_SET(6,47,v)
#define WU_DCR__WAC_TTYPE_CONFIG__TTYPE_INCLUDE_MASK_insert(x,v)  _BGQ_INSERT(6,47,x,v)

//   Field:       ttype_exclude_match
//   Description: Match value for exclude ttype compare
#define WU_DCR__WAC_TTYPE_CONFIG__TTYPE_EXCLUDE_MATCH_width        6
#define WU_DCR__WAC_TTYPE_CONFIG__TTYPE_EXCLUDE_MATCH_position     55
#define WU_DCR__WAC_TTYPE_CONFIG__TTYPE_EXCLUDE_MATCH_get(x)       _BGQ_GET(6,55,x)
#define WU_DCR__WAC_TTYPE_CONFIG__TTYPE_EXCLUDE_MATCH_set(v)       _BGQ_SET(6,55,v)
#define WU_DCR__WAC_TTYPE_CONFIG__TTYPE_EXCLUDE_MATCH_insert(x,v)  _BGQ_INSERT(6,55,x,v)

//   Field:       ttype_exclude_mask
//   Description: Mask value for exclude ttype compare. Set a bit 1 one to include bit in comparison, 0 to ignore.
#define WU_DCR__WAC_TTYPE_CONFIG__TTYPE_EXCLUDE_MASK_width        6
#define WU_DCR__WAC_TTYPE_CONFIG__TTYPE_EXCLUDE_MASK_position     63
#define WU_DCR__WAC_TTYPE_CONFIG__TTYPE_EXCLUDE_MASK_get(x)       _BGQ_GET(6,63,x)
#define WU_DCR__WAC_TTYPE_CONFIG__TTYPE_EXCLUDE_MASK_set(v)       _BGQ_SET(6,63,v)
#define WU_DCR__WAC_TTYPE_CONFIG__TTYPE_EXCLUDE_MASK_insert(x,v)  _BGQ_INSERT(6,63,x,v)

  
// Register:
//   Name:        access_config
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged Only
//   Reset:       0000000000000000
//   Description: Controls user mode access to registers within the Wakeup Unit
#define WU_DCR__ACCESS_CONFIG_offset  (0x03D9) // Offset of register in instance 
#define WU_DCR__ACCESS_CONFIG_reset  UNSIGNED64(0x0000000000000000)

//   Field:       wac_user_en
//   Description: Allows user access to wac_match/mask{0-11}. When set, user mode access is allowed. 
#define WU_DCR__ACCESS_CONFIG__WAC_USER_EN_width        12
#define WU_DCR__ACCESS_CONFIG__WAC_USER_EN_position     11
#define WU_DCR__ACCESS_CONFIG__WAC_USER_EN_get(x)       _BGQ_GET(12,11,x)
#define WU_DCR__ACCESS_CONFIG__WAC_USER_EN_set(v)       _BGQ_SET(12,11,v)
#define WU_DCR__ACCESS_CONFIG__WAC_USER_EN_insert(x,v)  _BGQ_INSERT(12,11,x,v)

//   Field:       thread_user_en
//   Description: Allows user access to thread{0-3}_wu_event_{set/clr}. When set, user mode access is allowed.
#define WU_DCR__ACCESS_CONFIG__THREAD_USER_EN_width        4
#define WU_DCR__ACCESS_CONFIG__THREAD_USER_EN_position     15
#define WU_DCR__ACCESS_CONFIG__THREAD_USER_EN_get(x)       _BGQ_GET(4,15,x)
#define WU_DCR__ACCESS_CONFIG__THREAD_USER_EN_set(v)       _BGQ_SET(4,15,v)
#define WU_DCR__ACCESS_CONFIG__THREAD_USER_EN_insert(x,v)  _BGQ_INSERT(4,15,x,v)

  
// Register:
//   Name:        interrupt_internal_error_sw_info
//   Type:        Interrupt Information
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Contains status for software type internal errors
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO_offset  (0x0061) // Offset of register in instance 
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO_reset  UNSIGNED64(0x0000000000000000)

//   Field:       read
//   Description: set if the transaction was a read
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_width        1
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_position     0
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_get(x)       _BGQ_GET(1,0,x)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_set(v)       _BGQ_SET(1,0,v)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_insert(x,v)  _BGQ_INSERT(1,0,x,v)

//   Field:       write
//   Description: set if the transaction was a write
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_width        1
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_position     1
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_get(x)       _BGQ_GET(1,1,x)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_set(v)       _BGQ_SET(1,1,v)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_insert(x,v)  _BGQ_INSERT(1,1,x,v)

//   Field:       privileged
//   Description: set if the transaction had privileged permission
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_width        1
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_position     2
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_get(x)       _BGQ_GET(1,2,x)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_set(v)       _BGQ_SET(1,2,v)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_insert(x,v)  _BGQ_INSERT(1,2,x,v)

//   Field:       test_int
//   Description: set if the transaction had test_int permission
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_width        1
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_position     3
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_get(x)       _BGQ_GET(1,3,x)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_set(v)       _BGQ_SET(1,3,v)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_insert(x,v)  _BGQ_INSERT(1,3,x,v)

//   Field:       address
//   Description: reflects the address at the time of error
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_width        10
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_position     63
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_get(x)       _BGQ_GET(10,63,x)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_set(v)       _BGQ_SET(10,63,v)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_insert(x,v)  _BGQ_INSERT(10,63,x,v)

  
// Register:
//   Name:        interrupt_internal_error_hw_info
//   Type:        Interrupt Information
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Contains status for hardware type internal errors
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO_offset  (0x0062) // Offset of register in instance 
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address_parity_good
//   Description: reflects each individual parity status for address
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_width        2
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_position     5
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_get(x)       _BGQ_GET(2,5,x)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_set(v)       _BGQ_SET(2,5,v)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_insert(x,v)  _BGQ_INSERT(2,5,x,v)

//   Field:       data_parity_good
//   Description: reflects each individual parity status for data
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_width        8
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_position     14
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_get(x)       _BGQ_GET(8,14,x)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_set(v)       _BGQ_SET(8,14,v)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_insert(x,v)  _BGQ_INSERT(8,14,x,v)

//   Field:       address
//   Description: reflects the address at the time of error
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_width        10
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_position     63
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_get(x)       _BGQ_GET(10,63,x)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_set(v)       _BGQ_SET(10,63,v)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_insert(x,v)  _BGQ_INSERT(10,63,x,v)

  
// Register:
//   Name:        interrupt_internal_error_data_info
//   Type:        Interrupt Information
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Contains status for hardware type internal errors
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO_offset  (0x0063) // Offset of register in instance 
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO_reset  UNSIGNED64(0x0000000000000000)

//   Field:       data
//   Description: reflects the local data at the time of error
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_width        64
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_position     63
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_get(x)       _BGQ_GET(64,63,x)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_set(v)       _BGQ_SET(64,63,v)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_insert(x,v)  _BGQ_INSERT(64,63,x,v)

  
// Interrupt State Register: interrupt_internal_error
//   State Permission: Read: All; Write: Privileged Only
//   First Permission: All
//   Force Permission: Privileged Only
#define WU_DCR__INTERRUPT_INTERNAL_ERROR__STATE_offset         (0x005A) // Offset of State register in instance
#define WU_DCR__INTERRUPT_INTERNAL_ERROR__FIRST_offset         (0x005D) // Offset of First register in instance
#define WU_DCR__INTERRUPT_INTERNAL_ERROR__FORCE_offset         (0x005C) // Offset of Force register in instance
#define WU_DCR__INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK_offset (0x005E) // Offset of Machine Check register in instance
#define WU_DCR__INTERRUPT_INTERNAL_ERROR__CRITICAL_offset      (0x005F) // Offset of Critical register in instance
#define WU_DCR__INTERRUPT_INTERNAL_ERROR__NONCRITICAL_offset   (0x0060) // Offset of Noncritical register in instance

#define WU_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_width        1
#define WU_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_position     0
#define WU_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_get(x)       _BGQ_GET(1,0,x)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_set(v)       _BGQ_SET(1,0,v)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_insert(x,v)  _BGQ_INSERT(1,0,x,v)


#define WU_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_width        1
#define WU_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_position     1
#define WU_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_get(x)       _BGQ_GET(1,1,x)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_set(v)       _BGQ_SET(1,1,v)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_insert(x,v)  _BGQ_INSERT(1,1,x,v)


#define WU_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_width        1
#define WU_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_position     2
#define WU_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_get(x)       _BGQ_GET(1,2,x)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_set(v)       _BGQ_SET(1,2,v)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_insert(x,v)  _BGQ_INSERT(1,2,x,v)


#define WU_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_width        1
#define WU_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_position     3
#define WU_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_get(x)       _BGQ_GET(1,3,x)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_set(v)       _BGQ_SET(1,3,v)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_insert(x,v)  _BGQ_INSERT(1,3,x,v)

  
// Interrupt Control Register: interrupt_internal_error_control_high
//   Permission: Read: All; Write: Privileged Only
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH_offset         (0x005B) // Offset of Control register in instance

#define WU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_width        2
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_position     1
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_get(x)       _BGQ_GET(2,1,x)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(v)       _BGQ_SET(2,1,v)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_insert(x,v)  _BGQ_INSERT(2,1,x,v)


#define WU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_width        2
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_position     3
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_get(x)       _BGQ_GET(2,3,x)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(v)       _BGQ_SET(2,3,v)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_insert(x,v)  _BGQ_INSERT(2,3,x,v)


#define WU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_width        2
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_position     5
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_get(x)       _BGQ_GET(2,5,x)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(v)       _BGQ_SET(2,5,v)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_insert(x,v)  _BGQ_INSERT(2,5,x,v)


#define WU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_width        2
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_position     7
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_get(x)       _BGQ_GET(2,7,x)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(v)       _BGQ_SET(2,7,v)
#define WU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_insert(x,v)  _BGQ_INSERT(2,7,x,v)

#ifndef __ASSEMBLY__

typedef struct wu_dcr { 
    /* 0x0000 */ uint64_t reserve_0000[0x005A];
    /* 0x005A */ uint64_t interrupt_internal_error__state;
    /* 0x005B */ uint64_t interrupt_internal_error_control_high;
    /* 0x005C */ uint64_t interrupt_internal_error__force;
    /* 0x005D */ uint64_t interrupt_internal_error__first;
    /* 0x005E */ uint64_t interrupt_internal_error__machine_check;
    /* 0x005F */ uint64_t interrupt_internal_error__critical;
    /* 0x0060 */ uint64_t interrupt_internal_error__noncritical;
    /* 0x0061 */ uint64_t interrupt_internal_error_sw_info;
    /* 0x0062 */ uint64_t interrupt_internal_error_hw_info;
    /* 0x0063 */ uint64_t interrupt_internal_error_data_info;
    /* 0x0064 */ uint64_t reserve_0064[0x031C];
    /* 0x0380 */ uint64_t wac_match0;
    /* 0x0381 */ uint64_t reserve_0381[0x0003];
    /* 0x0384 */ uint64_t wac_mask0;
    /* 0x0385 */ uint64_t reserve_0385[0x0003];
    /* 0x0388 */ uint64_t wac_match1;
    /* 0x0389 */ uint64_t reserve_0389[0x0003];
    /* 0x038C */ uint64_t wac_mask1;
    /* 0x038D */ uint64_t reserve_038D[0x0003];
    /* 0x0390 */ uint64_t wac_match2;
    /* 0x0391 */ uint64_t reserve_0391[0x0003];
    /* 0x0394 */ uint64_t wac_mask2;
    /* 0x0395 */ uint64_t reserve_0395[0x0003];
    /* 0x0398 */ uint64_t wac_match3;
    /* 0x0399 */ uint64_t reserve_0399[0x0003];
    /* 0x039C */ uint64_t wac_mask3;
    /* 0x039D */ uint64_t reserve_039D[0x0003];
    /* 0x03A0 */ uint64_t wac_match4;
    /* 0x03A1 */ uint64_t reserve_03A1[0x0003];
    /* 0x03A4 */ uint64_t wac_mask4;
    /* 0x03A5 */ uint64_t reserve_03A5[0x0003];
    /* 0x03A8 */ uint64_t wac_match5;
    /* 0x03A9 */ uint64_t reserve_03A9[0x0003];
    /* 0x03AC */ uint64_t wac_mask5;
    /* 0x03AD */ uint64_t reserve_03AD[0x0003];
    /* 0x03B0 */ uint64_t wac_match6;
    /* 0x03B1 */ uint64_t reserve_03B1[0x0003];
    /* 0x03B4 */ uint64_t wac_mask6;
    /* 0x03B5 */ uint64_t reserve_03B5[0x0003];
    /* 0x03B8 */ uint64_t wac_match7;
    /* 0x03B9 */ uint64_t reserve_03B9[0x0003];
    /* 0x03BC */ uint64_t wac_mask7;
    /* 0x03BD */ uint64_t int_event_set;
    /* 0x03BE */ uint64_t int_event_clr;
    /* 0x03BF */ uint64_t reserve_03BF;
    /* 0x03C0 */ uint64_t wac_match8;
    /* 0x03C1 */ uint64_t reserve_03C1[0x0003];
    /* 0x03C4 */ uint64_t wac_mask8;
    /* 0x03C5 */ uint64_t reserve_03C5[0x0003];
    /* 0x03C8 */ uint64_t wac_match9;
    /* 0x03C9 */ uint64_t reserve_03C9[0x0003];
    /* 0x03CC */ uint64_t wac_mask9;
    /* 0x03CD */ uint64_t reserve_03CD[0x0003];
    /* 0x03D0 */ uint64_t wac_match10;
    /* 0x03D1 */ uint64_t reserve_03D1[0x0003];
    /* 0x03D4 */ uint64_t wac_mask10;
    /* 0x03D5 */ uint64_t reserve_03D5[0x0003];
    /* 0x03D8 */ uint64_t wac_match11;
    /* 0x03D9 */ uint64_t access_config;
    /* 0x03DA */ uint64_t wac_ttype_config;
    /* 0x03DB */ uint64_t reserve_03DB;
    /* 0x03DC */ uint64_t wac_mask11;
    /* 0x03DD */ uint64_t reserve_03DD[0x0003];
    /* 0x03E0 */ uint64_t thread0_wu_event_set;
    /* 0x03E1 */ uint64_t reserve_03E1[0x0003];
    /* 0x03E4 */ uint64_t thread0_wu_event_clr;
    /* 0x03E5 */ uint64_t reserve_03E5[0x0003];
    /* 0x03E8 */ uint64_t thread1_wu_event_set;
    /* 0x03E9 */ uint64_t reserve_03E9[0x0003];
    /* 0x03EC */ uint64_t thread1_wu_event_clr;
    /* 0x03ED */ uint64_t reserve_03ED[0x0003];
    /* 0x03F0 */ uint64_t thread2_wu_event_set;
    /* 0x03F1 */ uint64_t reserve_03F1[0x0003];
    /* 0x03F4 */ uint64_t thread2_wu_event_clr;
    /* 0x03F5 */ uint64_t reserve_03F5[0x0003];
    /* 0x03F8 */ uint64_t thread3_wu_event_set;
    /* 0x03F9 */ uint64_t reserve_03F9[0x0003];
    /* 0x03FC */ uint64_t thread3_wu_event_clr;
} wu_dcr_t;
  
#define WU_DCR_USER_PTR _DCR_USER_PTR(wu_dcr_t, WU_DCR)
#define WU_DCR_PRIV_PTR _DCR_PRIV_PTR(wu_dcr_t, WU_DCR)
  
#endif // !__ASSEMBLY__
  
__END_DECLS
  
#endif // add nothing below this line.
