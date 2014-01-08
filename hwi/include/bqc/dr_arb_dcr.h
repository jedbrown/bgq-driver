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
 
#ifndef   _DR_ARB_DCR__H_ // Prevent multiple inclusion
#define   _DR_ARB_DCR__H_
  
__BEGIN_DECLS
  
#include <hwi/include/common/bgq_bitnumbers.h>
#include <hwi/include/bqc/dcr_support.h>
  
#define DR_ARB_DCR_base    (0x160000)  // Base address for all instances
#define DR_ARB_DCR_size    (0x0501)    // Size of instance (largest offset + 1 )
#define DR_ARB_DCR_stride  (0x4000)    // Distance between instances
#define DR_ARB_DCR_num     (0x0002)    // Number of instances
  
#define DR_ARB_DCR__A_base  (0x160000)  // Base address for instance A
#define DR_ARB_DCR__B_base  (0x164000)  // Base address for instance B
  
#define DR_ARB_DCR(i,reg) _DCR_REG__INDEX(DR_ARB_DCR, i, reg)
  
#define DR_ARB_DCR__A(reg) _DCR_REG__NAME(DR_ARB_DCR, A, reg)
#define DR_ARB_DCR__B(reg) _DCR_REG__NAME(DR_ARB_DCR, B, reg)
  
// Register:
//   Name:        scom_mc
//   Type:        Read/Write External
//   Permission:  Privileged and TestInt
//   Reset:       0000000000000000
//   Description: SCOM MC
#define DR_ARB_DCR__SCOM_MC_offset  (0x0200) // Offset of register in instance 
#define DR_ARB_DCR__SCOM_MC_range  (0x100) // Range of external address space
#define DR_ARB_DCR__SCOM_MC_reset  UNSIGNED64(0x0000000000000000)

//   Field:       data
//   Description: SCOM data
#define DR_ARB_DCR__SCOM_MC__DATA_width        64
#define DR_ARB_DCR__SCOM_MC__DATA_position     63
#define DR_ARB_DCR__SCOM_MC__DATA_get(x)       _BGQ_GET(64,63,x)
#define DR_ARB_DCR__SCOM_MC__DATA_set(v)       _BGQ_SET(64,63,v)
#define DR_ARB_DCR__SCOM_MC__DATA_insert(x,v)  _BGQ_INSERT(64,63,x,v)

  
// Register:
//   Name:        scom_mcs
//   Type:        Read/Write External
//   Permission:  Privileged and TestInt
//   Reset:       0000000000000000
//   Description: SCOM MCS
#define DR_ARB_DCR__SCOM_MCS_offset  (0x0300) // Offset of register in instance 
#define DR_ARB_DCR__SCOM_MCS_range  (0x80) // Range of external address space
#define DR_ARB_DCR__SCOM_MCS_reset  UNSIGNED64(0x0000000000000000)

//   Field:       data
//   Description: SCOM data
#define DR_ARB_DCR__SCOM_MCS__DATA_width        64
#define DR_ARB_DCR__SCOM_MCS__DATA_position     63
#define DR_ARB_DCR__SCOM_MCS__DATA_get(x)       _BGQ_GET(64,63,x)
#define DR_ARB_DCR__SCOM_MCS__DATA_set(v)       _BGQ_SET(64,63,v)
#define DR_ARB_DCR__SCOM_MCS__DATA_insert(x,v)  _BGQ_INSERT(64,63,x,v)

  
// Register:
//   Name:        scom_iom
//   Type:        Read/Write External
//   Permission:  Privileged and TestInt
//   Reset:       0000000000000000
//   Description: SCOM IOM
#define DR_ARB_DCR__SCOM_IOM_offset  (0x0400) // Offset of register in instance 
#define DR_ARB_DCR__SCOM_IOM_range  (0x100) // Range of external address space
#define DR_ARB_DCR__SCOM_IOM_reset  UNSIGNED64(0x0000000000000000)

//   Field:       data
//   Description: SCOM data
#define DR_ARB_DCR__SCOM_IOM__DATA_width        64
#define DR_ARB_DCR__SCOM_IOM__DATA_position     63
#define DR_ARB_DCR__SCOM_IOM__DATA_get(x)       _BGQ_GET(64,63,x)
#define DR_ARB_DCR__SCOM_IOM__DATA_set(v)       _BGQ_SET(64,63,v)
#define DR_ARB_DCR__SCOM_IOM__DATA_insert(x,v)  _BGQ_INSERT(64,63,x,v)

  
// Register:
//   Name:        map_address
//   Type:        Read/Write Internal
//   Permission:  Privileged and TestInt
//   Reset:       0000000000000002
//   Description: Address Map Configuration
#define DR_ARB_DCR__MAP_ADDRESS_offset  (0x0500) // Offset of register in instance 
#define DR_ARB_DCR__MAP_ADDRESS_reset  UNSIGNED64(0x0000000000000002)

//   Field:       rank_bit_sel_cfg
//   Description: Rank Bit Sel Config
#define DR_ARB_DCR__MAP_ADDRESS__RANK_BIT_SEL_CFG_width        3
#define DR_ARB_DCR__MAP_ADDRESS__RANK_BIT_SEL_CFG_position     63
#define DR_ARB_DCR__MAP_ADDRESS__RANK_BIT_SEL_CFG_get(x)       _BGQ_GET(3,63,x)
#define DR_ARB_DCR__MAP_ADDRESS__RANK_BIT_SEL_CFG_set(v)       _BGQ_SET(3,63,v)
#define DR_ARB_DCR__MAP_ADDRESS__RANK_BIT_SEL_CFG_insert(x,v)  _BGQ_INSERT(3,63,x,v)

  
// Register:
//   Name:        interrupt_internal_error_sw_info
//   Type:        Interrupt Information
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Contains status for software type internal errors
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO_offset  (0x0027) // Offset of register in instance 
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO_reset  UNSIGNED64(0x0000000000000000)

//   Field:       read
//   Description: set if the transaction was a read
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_width        1
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_position     0
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_get(x)       _BGQ_GET(1,0,x)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_set(v)       _BGQ_SET(1,0,v)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_insert(x,v)  _BGQ_INSERT(1,0,x,v)

//   Field:       write
//   Description: set if the transaction was a write
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_width        1
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_position     1
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_get(x)       _BGQ_GET(1,1,x)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_set(v)       _BGQ_SET(1,1,v)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_insert(x,v)  _BGQ_INSERT(1,1,x,v)

//   Field:       privileged
//   Description: set if the transaction had privileged permission
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_width        1
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_position     2
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_get(x)       _BGQ_GET(1,2,x)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_set(v)       _BGQ_SET(1,2,v)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_insert(x,v)  _BGQ_INSERT(1,2,x,v)

//   Field:       test_int
//   Description: set if the transaction had test_int permission
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_width        1
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_position     3
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_get(x)       _BGQ_GET(1,3,x)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_set(v)       _BGQ_SET(1,3,v)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_insert(x,v)  _BGQ_INSERT(1,3,x,v)

//   Field:       address
//   Description: reflects the address at the time of error
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_width        24
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_position     63
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_get(x)       _BGQ_GET(24,63,x)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_set(v)       _BGQ_SET(24,63,v)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_insert(x,v)  _BGQ_INSERT(24,63,x,v)

  
// Register:
//   Name:        interrupt_internal_error_hw_info
//   Type:        Interrupt Information
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Contains status for hardware type internal errors
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO_offset  (0x0028) // Offset of register in instance 
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address_parity_good
//   Description: reflects each individual parity status for address
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_width        3
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_position     6
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_get(x)       _BGQ_GET(3,6,x)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_set(v)       _BGQ_SET(3,6,v)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_insert(x,v)  _BGQ_INSERT(3,6,x,v)

//   Field:       data_parity_good
//   Description: reflects each individual parity status for data
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_width        8
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_position     14
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_get(x)       _BGQ_GET(8,14,x)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_set(v)       _BGQ_SET(8,14,v)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_insert(x,v)  _BGQ_INSERT(8,14,x,v)

//   Field:       address
//   Description: reflects the address at the time of error
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_width        24
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_position     63
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_get(x)       _BGQ_GET(24,63,x)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_set(v)       _BGQ_SET(24,63,v)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_insert(x,v)  _BGQ_INSERT(24,63,x,v)

  
// Register:
//   Name:        interrupt_internal_error_data_info
//   Type:        Interrupt Information
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Contains status for hardware type internal errors
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO_offset  (0x0029) // Offset of register in instance 
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO_reset  UNSIGNED64(0x0000000000000000)

//   Field:       data
//   Description: reflects the local data at the time of error
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_width        64
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_position     63
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_get(x)       _BGQ_GET(64,63,x)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_set(v)       _BGQ_SET(64,63,v)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_insert(x,v)  _BGQ_INSERT(64,63,x,v)

  
// Interrupt State Register: l2_interrupt_state
//   State Permission: Read: All; Write: Privileged and TestInt
//   First Permission: Privileged and TestInt
//   Force Permission: Privileged and TestInt
#define DR_ARB_DCR__L2_INTERRUPT_STATE__STATE_offset         (0x0016) // Offset of State register in instance
#define DR_ARB_DCR__L2_INTERRUPT_STATE__FIRST_offset         (0x001A) // Offset of First register in instance
#define DR_ARB_DCR__L2_INTERRUPT_STATE__FORCE_offset         (0x0019) // Offset of Force register in instance
#define DR_ARB_DCR__L2_INTERRUPT_STATE__MACHINE_CHECK_offset (0x001B) // Offset of Machine Check register in instance
#define DR_ARB_DCR__L2_INTERRUPT_STATE__CRITICAL_offset      (0x001C) // Offset of Critical register in instance
#define DR_ARB_DCR__L2_INTERRUPT_STATE__NONCRITICAL_offset   (0x001D) // Offset of Noncritical register in instance

#define DR_ARB_DCR__L2_INTERRUPT_STATE__LOCAL_RING_width        1
#define DR_ARB_DCR__L2_INTERRUPT_STATE__LOCAL_RING_position     0
#define DR_ARB_DCR__L2_INTERRUPT_STATE__LOCAL_RING_get(x)       _BGQ_GET(1,0,x)
#define DR_ARB_DCR__L2_INTERRUPT_STATE__LOCAL_RING_set(v)       _BGQ_SET(1,0,v)
#define DR_ARB_DCR__L2_INTERRUPT_STATE__LOCAL_RING_insert(x,v)  _BGQ_INSERT(1,0,x,v)


#define DR_ARB_DCR__L2_INTERRUPT_STATE__MC_ERR_width        1
#define DR_ARB_DCR__L2_INTERRUPT_STATE__MC_ERR_position     1
#define DR_ARB_DCR__L2_INTERRUPT_STATE__MC_ERR_get(x)       _BGQ_GET(1,1,x)
#define DR_ARB_DCR__L2_INTERRUPT_STATE__MC_ERR_set(v)       _BGQ_SET(1,1,v)
#define DR_ARB_DCR__L2_INTERRUPT_STATE__MC_ERR_insert(x,v)  _BGQ_INSERT(1,1,x,v)


#define DR_ARB_DCR__L2_INTERRUPT_STATE__MCS_ERR_width        1
#define DR_ARB_DCR__L2_INTERRUPT_STATE__MCS_ERR_position     2
#define DR_ARB_DCR__L2_INTERRUPT_STATE__MCS_ERR_get(x)       _BGQ_GET(1,2,x)
#define DR_ARB_DCR__L2_INTERRUPT_STATE__MCS_ERR_set(v)       _BGQ_SET(1,2,v)
#define DR_ARB_DCR__L2_INTERRUPT_STATE__MCS_ERR_insert(x,v)  _BGQ_INSERT(1,2,x,v)


#define DR_ARB_DCR__L2_INTERRUPT_STATE__IOM_ERR_width        1
#define DR_ARB_DCR__L2_INTERRUPT_STATE__IOM_ERR_position     3
#define DR_ARB_DCR__L2_INTERRUPT_STATE__IOM_ERR_get(x)       _BGQ_GET(1,3,x)
#define DR_ARB_DCR__L2_INTERRUPT_STATE__IOM_ERR_set(v)       _BGQ_SET(1,3,v)
#define DR_ARB_DCR__L2_INTERRUPT_STATE__IOM_ERR_insert(x,v)  _BGQ_INSERT(1,3,x,v)


#define DR_ARB_DCR__L2_INTERRUPT_STATE__XSTOP_width        1
#define DR_ARB_DCR__L2_INTERRUPT_STATE__XSTOP_position     4
#define DR_ARB_DCR__L2_INTERRUPT_STATE__XSTOP_get(x)       _BGQ_GET(1,4,x)
#define DR_ARB_DCR__L2_INTERRUPT_STATE__XSTOP_set(v)       _BGQ_SET(1,4,v)
#define DR_ARB_DCR__L2_INTERRUPT_STATE__XSTOP_insert(x,v)  _BGQ_INSERT(1,4,x,v)


#define DR_ARB_DCR__L2_INTERRUPT_STATE__SPEC_ATTN_width        1
#define DR_ARB_DCR__L2_INTERRUPT_STATE__SPEC_ATTN_position     5
#define DR_ARB_DCR__L2_INTERRUPT_STATE__SPEC_ATTN_get(x)       _BGQ_GET(1,5,x)
#define DR_ARB_DCR__L2_INTERRUPT_STATE__SPEC_ATTN_set(v)       _BGQ_SET(1,5,v)
#define DR_ARB_DCR__L2_INTERRUPT_STATE__SPEC_ATTN_insert(x,v)  _BGQ_INSERT(1,5,x,v)


#define DR_ARB_DCR__L2_INTERRUPT_STATE__RECOV_ERR_width        1
#define DR_ARB_DCR__L2_INTERRUPT_STATE__RECOV_ERR_position     6
#define DR_ARB_DCR__L2_INTERRUPT_STATE__RECOV_ERR_get(x)       _BGQ_GET(1,6,x)
#define DR_ARB_DCR__L2_INTERRUPT_STATE__RECOV_ERR_set(v)       _BGQ_SET(1,6,v)
#define DR_ARB_DCR__L2_INTERRUPT_STATE__RECOV_ERR_insert(x,v)  _BGQ_INSERT(1,6,x,v)

  
// Interrupt State Register: interrupt_internal_error
//   State Permission: Read: All; Write: Privileged and TestInt
//   First Permission: All
//   Force Permission: Privileged and TestInt
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR__STATE_offset         (0x0020) // Offset of State register in instance
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR__FIRST_offset         (0x0023) // Offset of First register in instance
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR__FORCE_offset         (0x0022) // Offset of Force register in instance
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK_offset (0x0024) // Offset of Machine Check register in instance
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR__CRITICAL_offset      (0x0025) // Offset of Critical register in instance
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR__NONCRITICAL_offset   (0x0026) // Offset of Noncritical register in instance

#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_width        1
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_position     0
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_get(x)       _BGQ_GET(1,0,x)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_set(v)       _BGQ_SET(1,0,v)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_insert(x,v)  _BGQ_INSERT(1,0,x,v)


#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_width        1
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_position     1
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_get(x)       _BGQ_GET(1,1,x)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_set(v)       _BGQ_SET(1,1,v)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_insert(x,v)  _BGQ_INSERT(1,1,x,v)


#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_width        1
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_position     2
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_get(x)       _BGQ_GET(1,2,x)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_set(v)       _BGQ_SET(1,2,v)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_insert(x,v)  _BGQ_INSERT(1,2,x,v)


#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_width        1
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_position     3
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_get(x)       _BGQ_GET(1,3,x)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_set(v)       _BGQ_SET(1,3,v)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_insert(x,v)  _BGQ_INSERT(1,3,x,v)

  
// Interrupt Control Register: l2_interrupt_state_control_low
//   Permission: Read: All; Write: Privileged and TestInt
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_LOW_offset         (0x0017) // Offset of Control register in instance
  
// Interrupt Control Register: l2_interrupt_state_control_high
//   Permission: Read: All; Write: Privileged and TestInt
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH_offset         (0x0018) // Offset of Control register in instance

#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_width        2
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_position     1
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_get(x)       _BGQ_GET(2,1,x)
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_set(v)       _BGQ_SET(2,1,v)
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_insert(x,v)  _BGQ_INSERT(2,1,x,v)


#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__MC_ERR_width        2
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__MC_ERR_position     3
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__MC_ERR_get(x)       _BGQ_GET(2,3,x)
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__MC_ERR_set(v)       _BGQ_SET(2,3,v)
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__MC_ERR_insert(x,v)  _BGQ_INSERT(2,3,x,v)


#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__MCS_ERR_width        2
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__MCS_ERR_position     5
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__MCS_ERR_get(x)       _BGQ_GET(2,5,x)
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__MCS_ERR_set(v)       _BGQ_SET(2,5,v)
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__MCS_ERR_insert(x,v)  _BGQ_INSERT(2,5,x,v)


#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__IOM_ERR_width        2
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__IOM_ERR_position     7
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__IOM_ERR_get(x)       _BGQ_GET(2,7,x)
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__IOM_ERR_set(v)       _BGQ_SET(2,7,v)
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__IOM_ERR_insert(x,v)  _BGQ_INSERT(2,7,x,v)


#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__XSTOP_width        2
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__XSTOP_position     9
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__XSTOP_get(x)       _BGQ_GET(2,9,x)
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__XSTOP_set(v)       _BGQ_SET(2,9,v)
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__XSTOP_insert(x,v)  _BGQ_INSERT(2,9,x,v)


#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__SPEC_ATTN_width        2
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__SPEC_ATTN_position     11
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__SPEC_ATTN_get(x)       _BGQ_GET(2,11,x)
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__SPEC_ATTN_set(v)       _BGQ_SET(2,11,v)
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__SPEC_ATTN_insert(x,v)  _BGQ_INSERT(2,11,x,v)


#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__RECOV_ERR_width        2
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__RECOV_ERR_position     13
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__RECOV_ERR_get(x)       _BGQ_GET(2,13,x)
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__RECOV_ERR_set(v)       _BGQ_SET(2,13,v)
#define DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__RECOV_ERR_insert(x,v)  _BGQ_INSERT(2,13,x,v)

  
// Interrupt Control Register: interrupt_internal_error_control_high
//   Permission: Read: All; Write: Privileged and TestInt
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH_offset         (0x0021) // Offset of Control register in instance

#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_width        2
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_position     1
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_get(x)       _BGQ_GET(2,1,x)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(v)       _BGQ_SET(2,1,v)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_insert(x,v)  _BGQ_INSERT(2,1,x,v)


#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_width        2
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_position     3
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_get(x)       _BGQ_GET(2,3,x)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(v)       _BGQ_SET(2,3,v)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_insert(x,v)  _BGQ_INSERT(2,3,x,v)


#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_width        2
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_position     5
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_get(x)       _BGQ_GET(2,5,x)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(v)       _BGQ_SET(2,5,v)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_insert(x,v)  _BGQ_INSERT(2,5,x,v)


#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_width        2
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_position     7
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_get(x)       _BGQ_GET(2,7,x)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(v)       _BGQ_SET(2,7,v)
#define DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_insert(x,v)  _BGQ_INSERT(2,7,x,v)

#ifndef __ASSEMBLY__

typedef struct dr_arb_dcr { 
    /* 0x0000 */ uint64_t reserve_0000[0x0016];
    /* 0x0016 */ uint64_t l2_interrupt_state__state;
    /* 0x0017 */ uint64_t l2_interrupt_state_control_low;
    /* 0x0018 */ uint64_t l2_interrupt_state_control_high;
    /* 0x0019 */ uint64_t l2_interrupt_state__force;
    /* 0x001A */ uint64_t l2_interrupt_state__first;
    /* 0x001B */ uint64_t l2_interrupt_state__machine_check;
    /* 0x001C */ uint64_t l2_interrupt_state__critical;
    /* 0x001D */ uint64_t l2_interrupt_state__noncritical;
    /* 0x001E */ uint64_t reserve_001E[0x0002];
    /* 0x0020 */ uint64_t interrupt_internal_error__state;
    /* 0x0021 */ uint64_t interrupt_internal_error_control_high;
    /* 0x0022 */ uint64_t interrupt_internal_error__force;
    /* 0x0023 */ uint64_t interrupt_internal_error__first;
    /* 0x0024 */ uint64_t interrupt_internal_error__machine_check;
    /* 0x0025 */ uint64_t interrupt_internal_error__critical;
    /* 0x0026 */ uint64_t interrupt_internal_error__noncritical;
    /* 0x0027 */ uint64_t interrupt_internal_error_sw_info;
    /* 0x0028 */ uint64_t interrupt_internal_error_hw_info;
    /* 0x0029 */ uint64_t interrupt_internal_error_data_info;
    /* 0x002A */ uint64_t reserve_002A[0x01D6];
    /* 0x0200 */ uint64_t scom_mc[DR_ARB_DCR__SCOM_MC_range];
    /* 0x0300 */ uint64_t scom_mcs[DR_ARB_DCR__SCOM_MCS_range];
    /* 0x0380 */ uint64_t reserve_0380[0x0080];
    /* 0x0400 */ uint64_t scom_iom[DR_ARB_DCR__SCOM_IOM_range];
    /* 0x0500 */ uint64_t map_address;
} dr_arb_dcr_t;
  
#define DR_ARB_DCR_USER_PTR(i) _DCR_USER_PTR__INDEX(dr_arb_dcr_t, DR_ARB_DCR, i)
#define DR_ARB_DCR_PRIV_PTR(i) _DCR_PRIV_PTR__INDEX(dr_arb_dcr_t, DR_ARB_DCR, i)
  
#define DR_ARB_DCR_USER_PTR__A _DCR_USER_PTR__NAME(dr_arb_dcr_t, DR_ARB_DCR, A)
#define DR_ARB_DCR_USER_PTR__B _DCR_USER_PTR__NAME(dr_arb_dcr_t, DR_ARB_DCR, B)
  
#define DR_ARB_DCR_PRIV_PTR__A _DCR_PRIV_PTR__NAME(dr_arb_dcr_t, DR_ARB_DCR, A)
#define DR_ARB_DCR_PRIV_PTR__B _DCR_PRIV_PTR__NAME(dr_arb_dcr_t, DR_ARB_DCR, B)
  
#endif // !__ASSEMBLY__
  
__END_DECLS
  
#endif // add nothing below this line.
