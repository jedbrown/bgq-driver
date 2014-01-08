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

#ifndef   _L1P_DCR__H_ // Prevent multiple inclusion
#define   _L1P_DCR__H_
  
__BEGIN_DECLS
  
#include <hwi/include/common/bgq_bitnumbers.h>
#include <hwi/include/bqc/dcr_support.h>
  
#define L1P_DCR_base    (0x000000)  // Base address for all instances
#define L1P_DCR_size    (0x4000)    // Size of instance (largest offset + 1 )
#define L1P_DCR_stride  (0x4000)    // Distance between instances
#define L1P_DCR_num     (0x0011)    // Number of instances
  
#define L1P_DCR__Core00_base  (0x0)  // Base address for instance Core00
#define L1P_DCR__Core01_base  (0x4000)  // Base address for instance Core01
#define L1P_DCR__Core02_base  (0x8000)  // Base address for instance Core02
#define L1P_DCR__Core03_base  (0xC000)  // Base address for instance Core03
#define L1P_DCR__Core04_base  (0x10000)  // Base address for instance Core04
#define L1P_DCR__Core05_base  (0x14000)  // Base address for instance Core05
#define L1P_DCR__Core06_base  (0x18000)  // Base address for instance Core06
#define L1P_DCR__Core07_base  (0x1C000)  // Base address for instance Core07
#define L1P_DCR__Core08_base  (0x20000)  // Base address for instance Core08
#define L1P_DCR__Core09_base  (0x24000)  // Base address for instance Core09
#define L1P_DCR__Core10_base  (0x28000)  // Base address for instance Core10
#define L1P_DCR__Core11_base  (0x2C000)  // Base address for instance Core11
#define L1P_DCR__Core12_base  (0x30000)  // Base address for instance Core12
#define L1P_DCR__Core13_base  (0x34000)  // Base address for instance Core13
#define L1P_DCR__Core14_base  (0x38000)  // Base address for instance Core14
#define L1P_DCR__Core15_base  (0x3C000)  // Base address for instance Core15
#define L1P_DCR__Core16_base  (0x40000)  // Base address for instance Core16
  
#define L1P_DCR(i,reg) _DCR_REG__INDEX(L1P_DCR, i, reg)
  
#define L1P_DCR__Core00(reg) _DCR_REG__NAME(L1P_DCR, Core00, reg)
#define L1P_DCR__Core01(reg) _DCR_REG__NAME(L1P_DCR, Core01, reg)
#define L1P_DCR__Core02(reg) _DCR_REG__NAME(L1P_DCR, Core02, reg)
#define L1P_DCR__Core03(reg) _DCR_REG__NAME(L1P_DCR, Core03, reg)
#define L1P_DCR__Core04(reg) _DCR_REG__NAME(L1P_DCR, Core04, reg)
#define L1P_DCR__Core05(reg) _DCR_REG__NAME(L1P_DCR, Core05, reg)
#define L1P_DCR__Core06(reg) _DCR_REG__NAME(L1P_DCR, Core06, reg)
#define L1P_DCR__Core07(reg) _DCR_REG__NAME(L1P_DCR, Core07, reg)
#define L1P_DCR__Core08(reg) _DCR_REG__NAME(L1P_DCR, Core08, reg)
#define L1P_DCR__Core09(reg) _DCR_REG__NAME(L1P_DCR, Core09, reg)
#define L1P_DCR__Core10(reg) _DCR_REG__NAME(L1P_DCR, Core10, reg)
#define L1P_DCR__Core11(reg) _DCR_REG__NAME(L1P_DCR, Core11, reg)
#define L1P_DCR__Core12(reg) _DCR_REG__NAME(L1P_DCR, Core12, reg)
#define L1P_DCR__Core13(reg) _DCR_REG__NAME(L1P_DCR, Core13, reg)
#define L1P_DCR__Core14(reg) _DCR_REG__NAME(L1P_DCR, Core14, reg)
#define L1P_DCR__Core15(reg) _DCR_REG__NAME(L1P_DCR, Core15, reg)
#define L1P_DCR__Core16(reg) _DCR_REG__NAME(L1P_DCR, Core16, reg)
  
// Register:
//   Name:        l1p_mmio_image
//   Type:        Read/Write External
//   Permission:  Permissions are externally decoded
//   Reset:       0000000000000000
//   Description: This block of registers provides DCR                    access to the L1P MMIO space.
#define L1P_DCR__L1P_MMIO_IMAGE_offset  (0x2000) // Offset of register in instance 
#define L1P_DCR__L1P_MMIO_IMAGE_range  (0x2000) // Range of external address space
#define L1P_DCR__L1P_MMIO_IMAGE_reset  UNSIGNED64(0x0000000000000000)

//   Field:       mmio_image
//   Description: DCR image of MMIO register
#define L1P_DCR__L1P_MMIO_IMAGE__MMIO_IMAGE_width        64
#define L1P_DCR__L1P_MMIO_IMAGE__MMIO_IMAGE_position     63
#define L1P_DCR__L1P_MMIO_IMAGE__MMIO_IMAGE_get(x)       _BGQ_GET(64,63,x)
#define L1P_DCR__L1P_MMIO_IMAGE__MMIO_IMAGE_set(v)       _BGQ_SET(64,63,v)
#define L1P_DCR__L1P_MMIO_IMAGE__MMIO_IMAGE_insert(x,v)  _BGQ_INSERT(64,63,x,v)

  
// Register:
//   Name:        dcr_test
//   Type:        Read/Write Internal
//   Permission:  All
//   Reset:       0000000000000000
//   Description: For read and write tests of DCR master
#define L1P_DCR__DCR_TEST_offset  (0x0001) // Offset of register in instance 
#define L1P_DCR__DCR_TEST_reset  UNSIGNED64(0x0000000000000000)

//   Field:       reg_contents
//   Description: 64 bit space for testing
#define L1P_DCR__DCR_TEST__REG_CONTENTS_width        64
#define L1P_DCR__DCR_TEST__REG_CONTENTS_position     63
#define L1P_DCR__DCR_TEST__REG_CONTENTS_get(x)       _BGQ_GET(64,63,x)
#define L1P_DCR__DCR_TEST__REG_CONTENTS_set(v)       _BGQ_SET(64,63,v)
#define L1P_DCR__DCR_TEST__REG_CONTENTS_insert(x,v)  _BGQ_INSERT(64,63,x,v)

  
// Register:
//   Name:        interrupt_internal_error_sw_info
//   Type:        Interrupt Information
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Contains status for software type internal errors
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO_offset  (0x0197) // Offset of register in instance 
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO_reset  UNSIGNED64(0x0000000000000000)

//   Field:       read
//   Description: set if the transaction was a read
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_width        1
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_position     0
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_get(x)       _BGQ_GET(1,0,x)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_set(v)       _BGQ_SET(1,0,v)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_insert(x,v)  _BGQ_INSERT(1,0,x,v)

//   Field:       write
//   Description: set if the transaction was a write
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_width        1
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_position     1
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_get(x)       _BGQ_GET(1,1,x)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_set(v)       _BGQ_SET(1,1,v)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_insert(x,v)  _BGQ_INSERT(1,1,x,v)

//   Field:       privileged
//   Description: set if the transaction had privileged permission
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_width        1
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_position     2
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_get(x)       _BGQ_GET(1,2,x)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_set(v)       _BGQ_SET(1,2,v)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_insert(x,v)  _BGQ_INSERT(1,2,x,v)

//   Field:       test_int
//   Description: set if the transaction had test_int permission
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_width        1
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_position     3
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_get(x)       _BGQ_GET(1,3,x)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_set(v)       _BGQ_SET(1,3,v)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_insert(x,v)  _BGQ_INSERT(1,3,x,v)

//   Field:       address
//   Description: reflects the address at the time of error
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_width        24
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_position     63
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_get(x)       _BGQ_GET(24,63,x)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_set(v)       _BGQ_SET(24,63,v)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_insert(x,v)  _BGQ_INSERT(24,63,x,v)

  
// Register:
//   Name:        interrupt_internal_error_hw_info
//   Type:        Interrupt Information
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Contains status for hardware type internal errors
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO_offset  (0x0198) // Offset of register in instance 
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address_parity_good
//   Description: reflects each individual parity status for address
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_width        3
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_position     6
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_get(x)       _BGQ_GET(3,6,x)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_set(v)       _BGQ_SET(3,6,v)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_insert(x,v)  _BGQ_INSERT(3,6,x,v)

//   Field:       data_parity_good
//   Description: reflects each individual parity status for data
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_width        8
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_position     14
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_get(x)       _BGQ_GET(8,14,x)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_set(v)       _BGQ_SET(8,14,v)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_insert(x,v)  _BGQ_INSERT(8,14,x,v)

//   Field:       address
//   Description: reflects the address at the time of error
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_width        24
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_position     63
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_get(x)       _BGQ_GET(24,63,x)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_set(v)       _BGQ_SET(24,63,v)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_insert(x,v)  _BGQ_INSERT(24,63,x,v)

  
// Register:
//   Name:        interrupt_internal_error_data_info
//   Type:        Interrupt Information
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Contains status for hardware type internal errors
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO_offset  (0x0199) // Offset of register in instance 
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO_reset  UNSIGNED64(0x0000000000000000)

//   Field:       data
//   Description: reflects the local data at the time of error
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_width        64
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_position     63
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_get(x)       _BGQ_GET(64,63,x)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_set(v)       _BGQ_SET(64,63,v)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_insert(x,v)  _BGQ_INSERT(64,63,x,v)

  
// Interrupt State Register: interrupt_state_a
//   State Permission: All
//   First Permission: All
//   Force Permission: Privileged and TestInt
#define L1P_DCR__INTERRUPT_STATE_A__STATE_offset         (0x0123) // Offset of State register in instance
#define L1P_DCR__INTERRUPT_STATE_A__FIRST_offset         (0x0127) // Offset of First register in instance
#define L1P_DCR__INTERRUPT_STATE_A__FORCE_offset         (0x0126) // Offset of Force register in instance
#define L1P_DCR__INTERRUPT_STATE_A__MACHINE_CHECK_offset (0x0128) // Offset of Machine Check register in instance
#define L1P_DCR__INTERRUPT_STATE_A__CRITICAL_offset      (0x0129) // Offset of Critical register in instance
#define L1P_DCR__INTERRUPT_STATE_A__NONCRITICAL_offset   (0x012A) // Offset of Noncritical register in instance

#define L1P_DCR__INTERRUPT_STATE_A__LOCAL_RING_width        1
#define L1P_DCR__INTERRUPT_STATE_A__LOCAL_RING_position     4
#define L1P_DCR__INTERRUPT_STATE_A__LOCAL_RING_get(x)       _BGQ_GET(1,4,x)
#define L1P_DCR__INTERRUPT_STATE_A__LOCAL_RING_set(v)       _BGQ_SET(1,4,v)
#define L1P_DCR__INTERRUPT_STATE_A__LOCAL_RING_insert(x,v)  _BGQ_INSERT(1,4,x,v)

  
// Interrupt State Register: interrupt_internal_error
//   State Permission: All
//   First Permission: All
//   Force Permission: Privileged and TestInt
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR__STATE_offset         (0x0190) // Offset of State register in instance
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR__FIRST_offset         (0x0193) // Offset of First register in instance
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR__FORCE_offset         (0x0192) // Offset of Force register in instance
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK_offset (0x0194) // Offset of Machine Check register in instance
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR__CRITICAL_offset      (0x0195) // Offset of Critical register in instance
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR__NONCRITICAL_offset   (0x0196) // Offset of Noncritical register in instance

#define L1P_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_width        1
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_position     0
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_get(x)       _BGQ_GET(1,0,x)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_set(v)       _BGQ_SET(1,0,v)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_insert(x,v)  _BGQ_INSERT(1,0,x,v)


#define L1P_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_width        1
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_position     1
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_get(x)       _BGQ_GET(1,1,x)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_set(v)       _BGQ_SET(1,1,v)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_insert(x,v)  _BGQ_INSERT(1,1,x,v)


#define L1P_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_width        1
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_position     2
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_get(x)       _BGQ_GET(1,2,x)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_set(v)       _BGQ_SET(1,2,v)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_insert(x,v)  _BGQ_INSERT(1,2,x,v)


#define L1P_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_width        1
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_position     3
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_get(x)       _BGQ_GET(1,3,x)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_set(v)       _BGQ_SET(1,3,v)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_insert(x,v)  _BGQ_INSERT(1,3,x,v)

  
// Interrupt Control Register: interrupt_state_a_control_low
//   Permission: Read: All; Write: Privileged and TestInt
#define L1P_DCR__INTERRUPT_STATE_A_CONTROL_LOW_offset         (0x0124) // Offset of Control register in instance
  
// Interrupt Control Register: interrupt_state_a_control_high
//   Permission: Read: All; Write: Privileged and TestInt
#define L1P_DCR__INTERRUPT_STATE_A_CONTROL_HIGH_offset         (0x0125) // Offset of Control register in instance

#define L1P_DCR__INTERRUPT_STATE_A_CONTROL_HIGH__LOCAL_RING_width        2
#define L1P_DCR__INTERRUPT_STATE_A_CONTROL_HIGH__LOCAL_RING_position     9
#define L1P_DCR__INTERRUPT_STATE_A_CONTROL_HIGH__LOCAL_RING_get(x)       _BGQ_GET(2,9,x)
#define L1P_DCR__INTERRUPT_STATE_A_CONTROL_HIGH__LOCAL_RING_set(v)       _BGQ_SET(2,9,v)
#define L1P_DCR__INTERRUPT_STATE_A_CONTROL_HIGH__LOCAL_RING_insert(x,v)  _BGQ_INSERT(2,9,x,v)

  
// Interrupt Control Register: interrupt_internal_error_control_high
//   Permission: Read: All; Write: Privileged and TestInt
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH_offset         (0x0191) // Offset of Control register in instance

#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_width        2
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_position     1
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_get(x)       _BGQ_GET(2,1,x)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(v)       _BGQ_SET(2,1,v)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_insert(x,v)  _BGQ_INSERT(2,1,x,v)


#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_width        2
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_position     3
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_get(x)       _BGQ_GET(2,3,x)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(v)       _BGQ_SET(2,3,v)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_insert(x,v)  _BGQ_INSERT(2,3,x,v)


#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_width        2
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_position     5
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_get(x)       _BGQ_GET(2,5,x)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(v)       _BGQ_SET(2,5,v)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_insert(x,v)  _BGQ_INSERT(2,5,x,v)


#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_width        2
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_position     7
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_get(x)       _BGQ_GET(2,7,x)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(v)       _BGQ_SET(2,7,v)
#define L1P_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_insert(x,v)  _BGQ_INSERT(2,7,x,v)

#ifndef __ASSEMBLY__

typedef struct l1p_dcr { 
    /* 0x0000 */ uint64_t reserve_0000;
    /* 0x0001 */ uint64_t dcr_test;
    /* 0x0002 */ uint64_t reserve_0002[0x0121];
    /* 0x0123 */ uint64_t interrupt_state_a__state;
    /* 0x0124 */ uint64_t interrupt_state_a_control_low;
    /* 0x0125 */ uint64_t interrupt_state_a_control_high;
    /* 0x0126 */ uint64_t interrupt_state_a__force;
    /* 0x0127 */ uint64_t interrupt_state_a__first;
    /* 0x0128 */ uint64_t interrupt_state_a__machine_check;
    /* 0x0129 */ uint64_t interrupt_state_a__critical;
    /* 0x012A */ uint64_t interrupt_state_a__noncritical;
    /* 0x012B */ uint64_t reserve_012B[0x0065];
    /* 0x0190 */ uint64_t interrupt_internal_error__state;
    /* 0x0191 */ uint64_t interrupt_internal_error_control_high;
    /* 0x0192 */ uint64_t interrupt_internal_error__force;
    /* 0x0193 */ uint64_t interrupt_internal_error__first;
    /* 0x0194 */ uint64_t interrupt_internal_error__machine_check;
    /* 0x0195 */ uint64_t interrupt_internal_error__critical;
    /* 0x0196 */ uint64_t interrupt_internal_error__noncritical;
    /* 0x0197 */ uint64_t interrupt_internal_error_sw_info;
    /* 0x0198 */ uint64_t interrupt_internal_error_hw_info;
    /* 0x0199 */ uint64_t interrupt_internal_error_data_info;
    /* 0x019A */ uint64_t reserve_019A[0x1E66];
    /* 0x2000 */ uint64_t l1p_mmio_image[L1P_DCR__L1P_MMIO_IMAGE_range];
} l1p_dcr_t;
  
#define L1P_DCR_USER_PTR(i) _DCR_USER_PTR__INDEX(l1p_dcr_t, L1P_DCR, i)
#define L1P_DCR_PRIV_PTR(i) _DCR_PRIV_PTR__INDEX(l1p_dcr_t, L1P_DCR, i)
  
#define L1P_DCR_USER_PTR__Core00 _DCR_USER_PTR__NAME(l1p_dcr_t, L1P_DCR, Core00)
#define L1P_DCR_USER_PTR__Core01 _DCR_USER_PTR__NAME(l1p_dcr_t, L1P_DCR, Core01)
#define L1P_DCR_USER_PTR__Core02 _DCR_USER_PTR__NAME(l1p_dcr_t, L1P_DCR, Core02)
#define L1P_DCR_USER_PTR__Core03 _DCR_USER_PTR__NAME(l1p_dcr_t, L1P_DCR, Core03)
#define L1P_DCR_USER_PTR__Core04 _DCR_USER_PTR__NAME(l1p_dcr_t, L1P_DCR, Core04)
#define L1P_DCR_USER_PTR__Core05 _DCR_USER_PTR__NAME(l1p_dcr_t, L1P_DCR, Core05)
#define L1P_DCR_USER_PTR__Core06 _DCR_USER_PTR__NAME(l1p_dcr_t, L1P_DCR, Core06)
#define L1P_DCR_USER_PTR__Core07 _DCR_USER_PTR__NAME(l1p_dcr_t, L1P_DCR, Core07)
#define L1P_DCR_USER_PTR__Core08 _DCR_USER_PTR__NAME(l1p_dcr_t, L1P_DCR, Core08)
#define L1P_DCR_USER_PTR__Core09 _DCR_USER_PTR__NAME(l1p_dcr_t, L1P_DCR, Core09)
#define L1P_DCR_USER_PTR__Core10 _DCR_USER_PTR__NAME(l1p_dcr_t, L1P_DCR, Core10)
#define L1P_DCR_USER_PTR__Core11 _DCR_USER_PTR__NAME(l1p_dcr_t, L1P_DCR, Core11)
#define L1P_DCR_USER_PTR__Core12 _DCR_USER_PTR__NAME(l1p_dcr_t, L1P_DCR, Core12)
#define L1P_DCR_USER_PTR__Core13 _DCR_USER_PTR__NAME(l1p_dcr_t, L1P_DCR, Core13)
#define L1P_DCR_USER_PTR__Core14 _DCR_USER_PTR__NAME(l1p_dcr_t, L1P_DCR, Core14)
#define L1P_DCR_USER_PTR__Core15 _DCR_USER_PTR__NAME(l1p_dcr_t, L1P_DCR, Core15)
#define L1P_DCR_USER_PTR__Core16 _DCR_USER_PTR__NAME(l1p_dcr_t, L1P_DCR, Core16)
  
#define L1P_DCR_PRIV_PTR__Core00 _DCR_PRIV_PTR__NAME(l1p_dcr_t, L1P_DCR, Core00)
#define L1P_DCR_PRIV_PTR__Core01 _DCR_PRIV_PTR__NAME(l1p_dcr_t, L1P_DCR, Core01)
#define L1P_DCR_PRIV_PTR__Core02 _DCR_PRIV_PTR__NAME(l1p_dcr_t, L1P_DCR, Core02)
#define L1P_DCR_PRIV_PTR__Core03 _DCR_PRIV_PTR__NAME(l1p_dcr_t, L1P_DCR, Core03)
#define L1P_DCR_PRIV_PTR__Core04 _DCR_PRIV_PTR__NAME(l1p_dcr_t, L1P_DCR, Core04)
#define L1P_DCR_PRIV_PTR__Core05 _DCR_PRIV_PTR__NAME(l1p_dcr_t, L1P_DCR, Core05)
#define L1P_DCR_PRIV_PTR__Core06 _DCR_PRIV_PTR__NAME(l1p_dcr_t, L1P_DCR, Core06)
#define L1P_DCR_PRIV_PTR__Core07 _DCR_PRIV_PTR__NAME(l1p_dcr_t, L1P_DCR, Core07)
#define L1P_DCR_PRIV_PTR__Core08 _DCR_PRIV_PTR__NAME(l1p_dcr_t, L1P_DCR, Core08)
#define L1P_DCR_PRIV_PTR__Core09 _DCR_PRIV_PTR__NAME(l1p_dcr_t, L1P_DCR, Core09)
#define L1P_DCR_PRIV_PTR__Core10 _DCR_PRIV_PTR__NAME(l1p_dcr_t, L1P_DCR, Core10)
#define L1P_DCR_PRIV_PTR__Core11 _DCR_PRIV_PTR__NAME(l1p_dcr_t, L1P_DCR, Core11)
#define L1P_DCR_PRIV_PTR__Core12 _DCR_PRIV_PTR__NAME(l1p_dcr_t, L1P_DCR, Core12)
#define L1P_DCR_PRIV_PTR__Core13 _DCR_PRIV_PTR__NAME(l1p_dcr_t, L1P_DCR, Core13)
#define L1P_DCR_PRIV_PTR__Core14 _DCR_PRIV_PTR__NAME(l1p_dcr_t, L1P_DCR, Core14)
#define L1P_DCR_PRIV_PTR__Core15 _DCR_PRIV_PTR__NAME(l1p_dcr_t, L1P_DCR, Core15)
#define L1P_DCR_PRIV_PTR__Core16 _DCR_PRIV_PTR__NAME(l1p_dcr_t, L1P_DCR, Core16)
  
#endif // !__ASSEMBLY__
  
__END_DECLS
  
#endif // add nothing below this line.
