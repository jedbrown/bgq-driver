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

#ifndef   _BEDRAM_DCR__H_ // Prevent multiple inclusion
#define   _BEDRAM_DCR__H_
  
__BEGIN_DECLS
  
#include <hwi/include/common/bgq_bitnumbers.h>
#include <hwi/include/bqc/dcr_support.h>
  
#define BEDRAM_DCR_base    (0x04C000)  // Base address for all instances
#define BEDRAM_DCR_size    (0x0012)    // Size of instance (largest offset + 1 )
  
#define BEDRAM_DCR(reg) _DCR_REG(BEDRAM_DCR, reg)
  
// Register:
//   Name:        bedram_control
//   Type:        Read/Write Internal
//   Permission:  All
//   Reset:       00E80000FFFFF000
//   Description: This register controls the bedram unit.
#define BEDRAM_DCR__BEDRAM_CONTROL_offset  (0x0000) // Offset of register in instance 
#define BEDRAM_DCR__BEDRAM_CONTROL_reset  UNSIGNED64(0x00E80000FFFFF000)

//   Field:       refresh_max
//   Description: This specifies the number of clk_x2 cycles+1 between each refresh
#define BEDRAM_DCR__BEDRAM_CONTROL__REFRESH_MAX_width        11
#define BEDRAM_DCR__BEDRAM_CONTROL__REFRESH_MAX_position     11
#define BEDRAM_DCR__BEDRAM_CONTROL__REFRESH_MAX_get(x)       _BGQ_GET(11,11,x)
#define BEDRAM_DCR__BEDRAM_CONTROL__REFRESH_MAX_set(v)       _BGQ_SET(11,11,v)
#define BEDRAM_DCR__BEDRAM_CONTROL__REFRESH_MAX_insert(x,v)  _BGQ_INSERT(11,11,x,v)

//   Field:       error_count
//   Description: This specifies the maximal number of correctable errors counted in error latch before interrupt.
#define BEDRAM_DCR__BEDRAM_CONTROL__ERROR_COUNT_width        20
#define BEDRAM_DCR__BEDRAM_CONTROL__ERROR_COUNT_position     31
#define BEDRAM_DCR__BEDRAM_CONTROL__ERROR_COUNT_get(x)       _BGQ_GET(20,31,x)
#define BEDRAM_DCR__BEDRAM_CONTROL__ERROR_COUNT_set(v)       _BGQ_SET(20,31,v)
#define BEDRAM_DCR__BEDRAM_CONTROL__ERROR_COUNT_insert(x,v)  _BGQ_INSERT(20,31,x,v)

//   Field:       err_status_clear
//   Description: If set, the internal error-status register gets cleared
#define BEDRAM_DCR__BEDRAM_CONTROL__ERR_STATUS_CLEAR_width        1
#define BEDRAM_DCR__BEDRAM_CONTROL__ERR_STATUS_CLEAR_position     32
#define BEDRAM_DCR__BEDRAM_CONTROL__ERR_STATUS_CLEAR_get(x)       _BGQ_GET(1,32,x)
#define BEDRAM_DCR__BEDRAM_CONTROL__ERR_STATUS_CLEAR_set(v)       _BGQ_SET(1,32,v)
#define BEDRAM_DCR__BEDRAM_CONTROL__ERR_STATUS_CLEAR_insert(x,v)  _BGQ_INSERT(1,32,x,v)

//   Field:       scb_delay
//   Description: This specifies the number of x16 clock cycles between scrubbing two locations
#define BEDRAM_DCR__BEDRAM_CONTROL__SCB_DELAY_width        19
#define BEDRAM_DCR__BEDRAM_CONTROL__SCB_DELAY_position     51
#define BEDRAM_DCR__BEDRAM_CONTROL__SCB_DELAY_get(x)       _BGQ_GET(19,51,x)
#define BEDRAM_DCR__BEDRAM_CONTROL__SCB_DELAY_set(v)       _BGQ_SET(19,51,v)
#define BEDRAM_DCR__BEDRAM_CONTROL__SCB_DELAY_insert(x,v)  _BGQ_INSERT(19,51,x,v)

  
// Register:
//   Name:        interrupt_internal_error_sw_info
//   Type:        Interrupt Information
//   Permission:  Privileged and TestInt
//   Reset:       0000000000000000
//   Description: Contains status for software type internal errors
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO_offset  (0x0010) // Offset of register in instance 
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO_reset  UNSIGNED64(0x0000000000000000)

//   Field:       read
//   Description: set if the transaction was a read
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_width        1
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_position     0
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_get(x)       _BGQ_GET(1,0,x)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_set(v)       _BGQ_SET(1,0,v)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_insert(x,v)  _BGQ_INSERT(1,0,x,v)

//   Field:       write
//   Description: set if the transaction was a write
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_width        1
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_position     1
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_get(x)       _BGQ_GET(1,1,x)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_set(v)       _BGQ_SET(1,1,v)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_insert(x,v)  _BGQ_INSERT(1,1,x,v)

//   Field:       privileged
//   Description: set if the transaction had privileged permission
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_width        1
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_position     2
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_get(x)       _BGQ_GET(1,2,x)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_set(v)       _BGQ_SET(1,2,v)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_insert(x,v)  _BGQ_INSERT(1,2,x,v)

//   Field:       test_int
//   Description: set if the transaction had test_int permission
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_width        1
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_position     3
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_get(x)       _BGQ_GET(1,3,x)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_set(v)       _BGQ_SET(1,3,v)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_insert(x,v)  _BGQ_INSERT(1,3,x,v)

//   Field:       address
//   Description: reflects the address at the time of error
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_width        24
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_position     63
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_get(x)       _BGQ_GET(24,63,x)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_set(v)       _BGQ_SET(24,63,v)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_insert(x,v)  _BGQ_INSERT(24,63,x,v)

  
// Register:
//   Name:        interrupt_internal_error_hw_info
//   Type:        Interrupt Information
//   Permission:  Privileged and TestInt
//   Reset:       0000000000000000
//   Description: Contains status for hardware type internal errors
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO_offset  (0x0011) // Offset of register in instance 
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address_parity_good
//   Description: reflects each individual parity status for address
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_width        3
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_position     6
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_get(x)       _BGQ_GET(3,6,x)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_set(v)       _BGQ_SET(3,6,v)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_insert(x,v)  _BGQ_INSERT(3,6,x,v)

//   Field:       data_parity_good
//   Description: reflects each individual parity status for data
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_width        8
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_position     14
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_get(x)       _BGQ_GET(8,14,x)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_set(v)       _BGQ_SET(8,14,v)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_insert(x,v)  _BGQ_INSERT(8,14,x,v)

//   Field:       address
//   Description: reflects the address at the time of error
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_width        24
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_position     63
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_get(x)       _BGQ_GET(24,63,x)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_set(v)       _BGQ_SET(24,63,v)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_insert(x,v)  _BGQ_INSERT(24,63,x,v)

  
// Register:
//   Name:        interrupt_internal_error_data_info
//   Type:        Interrupt Information
//   Permission:  Privileged and TestInt
//   Reset:       0000000000000000
//   Description: Contains status for hardware type internal errors
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO_offset  (0x0012) // Offset of register in instance 
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO_reset  UNSIGNED64(0x0000000000000000)

//   Field:       data
//   Description: reflects the local data at the time of error
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_width        64
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_position     63
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_get(x)       _BGQ_GET(64,63,x)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_set(v)       _BGQ_SET(64,63,v)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_insert(x,v)  _BGQ_INSERT(64,63,x,v)

  
// Interrupt State Register: bedram_interrupt_status
//   State Permission: All
//   First Permission: All
//   Force Permission: All
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__STATE_offset         (0x0001) // Offset of State register in instance
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__FIRST_offset         (0x0005) // Offset of First register in instance
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__FORCE_offset         (0x0004) // Offset of Force register in instance
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__MACHINE_CHECK_offset (0x0006) // Offset of Machine Check register in instance
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__CRITICAL_offset      (0x0007) // Offset of Critical register in instance
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__NONCRITICAL_offset   (0x0008) // Offset of Noncritical register in instance

#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_DB_PARITY_width        1
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_DB_PARITY_position     1
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_DB_PARITY_get(x)       _BGQ_GET(1,1,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_DB_PARITY_set(v)       _BGQ_SET(1,1,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_DB_PARITY_insert(x,v)  _BGQ_INSERT(1,1,x,v)


#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_PIPE_PARITY_width        1
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_PIPE_PARITY_position     2
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_PIPE_PARITY_get(x)       _BGQ_GET(1,2,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_PIPE_PARITY_set(v)       _BGQ_SET(1,2,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_PIPE_PARITY_insert(x,v)  _BGQ_INSERT(1,2,x,v)


#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_REF_PARITY_width        1
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_REF_PARITY_position     3
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_REF_PARITY_get(x)       _BGQ_GET(1,3,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_REF_PARITY_set(v)       _BGQ_SET(1,3,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_REF_PARITY_insert(x,v)  _BGQ_INSERT(1,3,x,v)


#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_RES_PARITY_width        1
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_RES_PARITY_position     4
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_RES_PARITY_get(x)       _BGQ_GET(1,4,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_RES_PARITY_set(v)       _BGQ_SET(1,4,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_RES_PARITY_insert(x,v)  _BGQ_INSERT(1,4,x,v)


#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_SCB_PARITY_width        1
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_SCB_PARITY_position     5
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_SCB_PARITY_get(x)       _BGQ_GET(1,5,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_SCB_PARITY_set(v)       _BGQ_SET(1,5,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_SCB_PARITY_insert(x,v)  _BGQ_INSERT(1,5,x,v)


#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_ERR_PARITY_width        1
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_ERR_PARITY_position     6
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_ERR_PARITY_get(x)       _BGQ_GET(1,6,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_ERR_PARITY_set(v)       _BGQ_SET(1,6,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_ERR_PARITY_insert(x,v)  _BGQ_INSERT(1,6,x,v)


#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_DB_ECC_width        1
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_DB_ECC_position     7
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_DB_ECC_get(x)       _BGQ_GET(1,7,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_DB_ECC_set(v)       _BGQ_SET(1,7,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_DB_ECC_insert(x,v)  _BGQ_INSERT(1,7,x,v)


#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_EDRAM_ECC_width        1
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_EDRAM_ECC_position     8
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_EDRAM_ECC_get(x)       _BGQ_GET(1,8,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_EDRAM_ECC_set(v)       _BGQ_SET(1,8,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_EDRAM_ECC_insert(x,v)  _BGQ_INSERT(1,8,x,v)


#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_PIPE_ECC_width        1
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_PIPE_ECC_position     9
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_PIPE_ECC_get(x)       _BGQ_GET(1,9,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_PIPE_ECC_set(v)       _BGQ_SET(1,9,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_PIPE_ECC_insert(x,v)  _BGQ_INSERT(1,9,x,v)


#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_ECC_CE_RAISE_width        1
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_ECC_CE_RAISE_position     10
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_ECC_CE_RAISE_get(x)       _BGQ_GET(1,10,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_ECC_CE_RAISE_set(v)       _BGQ_SET(1,10,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_ECC_CE_RAISE_insert(x,v)  _BGQ_INSERT(1,10,x,v)


#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_ALIGNED_width        1
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_ALIGNED_position     11
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_ALIGNED_get(x)       _BGQ_GET(1,11,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_ALIGNED_set(v)       _BGQ_SET(1,11,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_ALIGNED_insert(x,v)  _BGQ_INSERT(1,11,x,v)


#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_BYTE_SIZE_width        1
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_BYTE_SIZE_position     12
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_BYTE_SIZE_get(x)       _BGQ_GET(1,12,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_BYTE_SIZE_set(v)       _BGQ_SET(1,12,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_BYTE_SIZE_insert(x,v)  _BGQ_INSERT(1,12,x,v)


#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_NON_PRIV_width        1
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_NON_PRIV_position     13
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_NON_PRIV_get(x)       _BGQ_GET(1,13,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_NON_PRIV_set(v)       _BGQ_SET(1,13,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_NON_PRIV_insert(x,v)  _BGQ_INSERT(1,13,x,v)


#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__LOCAL_RING_width        1
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__LOCAL_RING_position     14
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__LOCAL_RING_get(x)       _BGQ_GET(1,14,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__LOCAL_RING_set(v)       _BGQ_SET(1,14,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__LOCAL_RING_insert(x,v)  _BGQ_INSERT(1,14,x,v)

  
// Interrupt State Register: interrupt_internal_error
//   State Permission: Privileged and TestInt
//   First Permission: Privileged and TestInt
//   Force Permission: Privileged and TestInt
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR__STATE_offset         (0x0009) // Offset of State register in instance
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR__FIRST_offset         (0x000C) // Offset of First register in instance
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR__FORCE_offset         (0x000B) // Offset of Force register in instance
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK_offset (0x000D) // Offset of Machine Check register in instance
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR__CRITICAL_offset      (0x000E) // Offset of Critical register in instance
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR__NONCRITICAL_offset   (0x000F) // Offset of Noncritical register in instance

#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_width        1
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_position     0
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_get(x)       _BGQ_GET(1,0,x)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_set(v)       _BGQ_SET(1,0,v)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_insert(x,v)  _BGQ_INSERT(1,0,x,v)


#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_width        1
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_position     1
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_get(x)       _BGQ_GET(1,1,x)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_set(v)       _BGQ_SET(1,1,v)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_insert(x,v)  _BGQ_INSERT(1,1,x,v)


#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_width        1
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_position     2
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_get(x)       _BGQ_GET(1,2,x)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_set(v)       _BGQ_SET(1,2,v)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_insert(x,v)  _BGQ_INSERT(1,2,x,v)


#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_width        1
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_position     3
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_get(x)       _BGQ_GET(1,3,x)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_set(v)       _BGQ_SET(1,3,v)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_insert(x,v)  _BGQ_INSERT(1,3,x,v)

  
// Interrupt Control Register: bedram_interrupt_status_control_low
//   Permission: All
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_LOW_offset         (0x0002) // Offset of Control register in instance
  
// Interrupt Control Register: bedram_interrupt_status_control_high
//   Permission: All
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH_offset         (0x0003) // Offset of Control register in instance

#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_DB_PARITY_width        2
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_DB_PARITY_position     3
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_DB_PARITY_get(x)       _BGQ_GET(2,3,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_DB_PARITY_set(v)       _BGQ_SET(2,3,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_DB_PARITY_insert(x,v)  _BGQ_INSERT(2,3,x,v)


#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_PIPE_PARITY_width        2
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_PIPE_PARITY_position     5
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_PIPE_PARITY_get(x)       _BGQ_GET(2,5,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_PIPE_PARITY_set(v)       _BGQ_SET(2,5,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_PIPE_PARITY_insert(x,v)  _BGQ_INSERT(2,5,x,v)


#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_REF_PARITY_width        2
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_REF_PARITY_position     7
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_REF_PARITY_get(x)       _BGQ_GET(2,7,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_REF_PARITY_set(v)       _BGQ_SET(2,7,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_REF_PARITY_insert(x,v)  _BGQ_INSERT(2,7,x,v)


#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_RES_PARITY_width        2
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_RES_PARITY_position     9
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_RES_PARITY_get(x)       _BGQ_GET(2,9,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_RES_PARITY_set(v)       _BGQ_SET(2,9,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_RES_PARITY_insert(x,v)  _BGQ_INSERT(2,9,x,v)


#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_SCB_PARITY_width        2
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_SCB_PARITY_position     11
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_SCB_PARITY_get(x)       _BGQ_GET(2,11,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_SCB_PARITY_set(v)       _BGQ_SET(2,11,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_SCB_PARITY_insert(x,v)  _BGQ_INSERT(2,11,x,v)


#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_ERR_PARITY_width        2
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_ERR_PARITY_position     13
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_ERR_PARITY_get(x)       _BGQ_GET(2,13,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_ERR_PARITY_set(v)       _BGQ_SET(2,13,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_ERR_PARITY_insert(x,v)  _BGQ_INSERT(2,13,x,v)


#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_DB_ECC_width        2
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_DB_ECC_position     15
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_DB_ECC_get(x)       _BGQ_GET(2,15,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_DB_ECC_set(v)       _BGQ_SET(2,15,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_DB_ECC_insert(x,v)  _BGQ_INSERT(2,15,x,v)


#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_EDRAM_ECC_width        2
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_EDRAM_ECC_position     17
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_EDRAM_ECC_get(x)       _BGQ_GET(2,17,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_EDRAM_ECC_set(v)       _BGQ_SET(2,17,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_EDRAM_ECC_insert(x,v)  _BGQ_INSERT(2,17,x,v)


#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_PIPE_ECC_width        2
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_PIPE_ECC_position     19
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_PIPE_ECC_get(x)       _BGQ_GET(2,19,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_PIPE_ECC_set(v)       _BGQ_SET(2,19,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_PIPE_ECC_insert(x,v)  _BGQ_INSERT(2,19,x,v)


#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_ECC_CE_RAISE_width        2
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_ECC_CE_RAISE_position     21
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_ECC_CE_RAISE_get(x)       _BGQ_GET(2,21,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_ECC_CE_RAISE_set(v)       _BGQ_SET(2,21,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_ECC_CE_RAISE_insert(x,v)  _BGQ_INSERT(2,21,x,v)


#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_ALIGNED_width        2
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_ALIGNED_position     23
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_ALIGNED_get(x)       _BGQ_GET(2,23,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_ALIGNED_set(v)       _BGQ_SET(2,23,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_ALIGNED_insert(x,v)  _BGQ_INSERT(2,23,x,v)


#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_BYTE_SIZE_width        2
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_BYTE_SIZE_position     25
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_BYTE_SIZE_get(x)       _BGQ_GET(2,25,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_BYTE_SIZE_set(v)       _BGQ_SET(2,25,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_BYTE_SIZE_insert(x,v)  _BGQ_INSERT(2,25,x,v)


#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_NON_PRIV_width        2
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_NON_PRIV_position     27
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_NON_PRIV_get(x)       _BGQ_GET(2,27,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_NON_PRIV_set(v)       _BGQ_SET(2,27,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_NON_PRIV_insert(x,v)  _BGQ_INSERT(2,27,x,v)


#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__LOCAL_RING_width        2
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__LOCAL_RING_position     29
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__LOCAL_RING_get(x)       _BGQ_GET(2,29,x)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__LOCAL_RING_set(v)       _BGQ_SET(2,29,v)
#define BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__LOCAL_RING_insert(x,v)  _BGQ_INSERT(2,29,x,v)

  
// Interrupt Control Register: interrupt_internal_error_control_high
//   Permission: Privileged and TestInt
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH_offset         (0x000A) // Offset of Control register in instance

#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_width        2
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_position     1
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_get(x)       _BGQ_GET(2,1,x)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(v)       _BGQ_SET(2,1,v)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_insert(x,v)  _BGQ_INSERT(2,1,x,v)


#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_width        2
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_position     3
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_get(x)       _BGQ_GET(2,3,x)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(v)       _BGQ_SET(2,3,v)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_insert(x,v)  _BGQ_INSERT(2,3,x,v)


#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_width        2
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_position     5
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_get(x)       _BGQ_GET(2,5,x)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(v)       _BGQ_SET(2,5,v)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_insert(x,v)  _BGQ_INSERT(2,5,x,v)


#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_width        2
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_position     7
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_get(x)       _BGQ_GET(2,7,x)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(v)       _BGQ_SET(2,7,v)
#define BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_insert(x,v)  _BGQ_INSERT(2,7,x,v)

#ifndef __ASSEMBLY__

typedef struct bedram_dcr { 
    /* 0x0000 */ uint64_t bedram_control;
    /* 0x0001 */ uint64_t bedram_interrupt_status__state;
    /* 0x0002 */ uint64_t bedram_interrupt_status_control_low;
    /* 0x0003 */ uint64_t bedram_interrupt_status_control_high;
    /* 0x0004 */ uint64_t bedram_interrupt_status__force;
    /* 0x0005 */ uint64_t bedram_interrupt_status__first;
    /* 0x0006 */ uint64_t bedram_interrupt_status__machine_check;
    /* 0x0007 */ uint64_t bedram_interrupt_status__critical;
    /* 0x0008 */ uint64_t bedram_interrupt_status__noncritical;
    /* 0x0009 */ uint64_t interrupt_internal_error__state;
    /* 0x000A */ uint64_t interrupt_internal_error_control_high;
    /* 0x000B */ uint64_t interrupt_internal_error__force;
    /* 0x000C */ uint64_t interrupt_internal_error__first;
    /* 0x000D */ uint64_t interrupt_internal_error__machine_check;
    /* 0x000E */ uint64_t interrupt_internal_error__critical;
    /* 0x000F */ uint64_t interrupt_internal_error__noncritical;
    /* 0x0010 */ uint64_t interrupt_internal_error_sw_info;
    /* 0x0011 */ uint64_t interrupt_internal_error_hw_info;
} bedram_dcr_t;
  
#define BEDRAM_DCR_USER_PTR _DCR_USER_PTR(bedram_dcr_t, BEDRAM_DCR)
#define BEDRAM_DCR_PRIV_PTR _DCR_PRIV_PTR(bedram_dcr_t, BEDRAM_DCR)
  
#endif // !__ASSEMBLY__
  
__END_DECLS
  
#endif // add nothing below this line.
