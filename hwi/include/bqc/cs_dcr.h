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

#ifndef   _CS_DCR__H_ // Prevent multiple inclusion
#define   _CS_DCR__H_
  
__BEGIN_DECLS
  
#include <hwi/include/common/bgq_bitnumbers.h>
#include <hwi/include/bqc/dcr_support.h>
  
#define CS_DCR_base    (0x0D8000)  // Base address for all instances
#define CS_DCR_size    (0x0029)    // Size of instance (largest offset + 1 )
  
#define CS_DCR(reg) _DCR_REG(CS_DCR, reg)
  
// Register:
//   Name:        controlreg_0
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged and TestInt
//   Reset:       000000000000000E
//   Description:  This register controls the soft reset, clear count and clear clockstop sticky output
#define CS_DCR__CONTROLREG_0_offset  (0x0000) // Offset of register in instance 
#define CS_DCR__CONTROLREG_0_reset  UNSIGNED64(0x000000000000000E)

//   Field:       enable_bic_cs_stop
//   Description:  When '1' allows for the bic input to be used to stop clocks. The enable_stop_output must also be '1' to actually stop clocks 
#define CS_DCR__CONTROLREG_0__ENABLE_BIC_CS_STOP_width        1
#define CS_DCR__CONTROLREG_0__ENABLE_BIC_CS_STOP_position     60
#define CS_DCR__CONTROLREG_0__ENABLE_BIC_CS_STOP_get(x)       _BGQ_GET(1,60,x)
#define CS_DCR__CONTROLREG_0__ENABLE_BIC_CS_STOP_set(v)       _BGQ_SET(1,60,v)
#define CS_DCR__CONTROLREG_0__ENABLE_BIC_CS_STOP_insert(x,v)  _BGQ_INSERT(1,60,x,v)

//   Field:       clear_count
//   Description: This will clear the current value of the counter when equal to 1. This is a level and must therefore be set to 0 as one of the conditions to increment.
#define CS_DCR__CONTROLREG_0__CLEAR_COUNT_width        1
#define CS_DCR__CONTROLREG_0__CLEAR_COUNT_position     61
#define CS_DCR__CONTROLREG_0__CLEAR_COUNT_get(x)       _BGQ_GET(1,61,x)
#define CS_DCR__CONTROLREG_0__CLEAR_COUNT_set(v)       _BGQ_SET(1,61,v)
#define CS_DCR__CONTROLREG_0__CLEAR_COUNT_insert(x,v)  _BGQ_INSERT(1,61,x,v)

//   Field:       clear_clock_stop
//   Description:  This will clear the sticky output bit precise_clock_stop when equal to 1. This is a level. 
#define CS_DCR__CONTROLREG_0__CLEAR_CLOCK_STOP_width        1
#define CS_DCR__CONTROLREG_0__CLEAR_CLOCK_STOP_position     62
#define CS_DCR__CONTROLREG_0__CLEAR_CLOCK_STOP_get(x)       _BGQ_GET(1,62,x)
#define CS_DCR__CONTROLREG_0__CLEAR_CLOCK_STOP_set(v)       _BGQ_SET(1,62,v)
#define CS_DCR__CONTROLREG_0__CLEAR_CLOCK_STOP_insert(x,v)  _BGQ_INSERT(1,62,x,v)

//   Field:       enable_stop_output
//   Description:  This bit when set to 1 enables the output precise_clock_stop. This is a level
#define CS_DCR__CONTROLREG_0__ENABLE_STOP_OUTPUT_width        1
#define CS_DCR__CONTROLREG_0__ENABLE_STOP_OUTPUT_position     63
#define CS_DCR__CONTROLREG_0__ENABLE_STOP_OUTPUT_get(x)       _BGQ_GET(1,63,x)
#define CS_DCR__CONTROLREG_0__ENABLE_STOP_OUTPUT_set(v)       _BGQ_SET(1,63,v)
#define CS_DCR__CONTROLREG_0__ENABLE_STOP_OUTPUT_insert(x,v)  _BGQ_INSERT(1,63,x,v)

  
// Register:
//   Name:        controlreg_1
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged and TestInt
//   Reset:       0000000000000000
//   Description:  This register allows the counter to increment one count every processor clock
#define CS_DCR__CONTROLREG_1_offset  (0x0001) // Offset of register in instance 
#define CS_DCR__CONTROLREG_1_reset  UNSIGNED64(0x0000000000000000)

//   Field:       increment
//   Description:  When '1' this allows the counter to increment. This is a level. Note: the  increment condition is that reset is false and bit 63 of this register is 1 and bit 61 of controlreg0 is 0  
#define CS_DCR__CONTROLREG_1__INCREMENT_width        1
#define CS_DCR__CONTROLREG_1__INCREMENT_position     63
#define CS_DCR__CONTROLREG_1__INCREMENT_get(x)       _BGQ_GET(1,63,x)
#define CS_DCR__CONTROLREG_1__INCREMENT_set(v)       _BGQ_SET(1,63,v)
#define CS_DCR__CONTROLREG_1__INCREMENT_insert(x,v)  _BGQ_INSERT(1,63,x,v)

  
// Register:
//   Name:        threshold
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged and TestInt
//   Reset:       0000000000000000
//   Description:  This register has the threshold that the counter will be compared against. Note that the actual pclk reference for stopping is actually threshold*48 + phase
#define CS_DCR__THRESHOLD_offset  (0x0002) // Offset of register in instance 
#define CS_DCR__THRESHOLD_reset  UNSIGNED64(0x0000000000000000)

//   Field:       threshold
//   Description:  39 bit threshold  
#define CS_DCR__THRESHOLD__THRESHOLD_width        39
#define CS_DCR__THRESHOLD__THRESHOLD_position     63
#define CS_DCR__THRESHOLD__THRESHOLD_get(x)       _BGQ_GET(39,63,x)
#define CS_DCR__THRESHOLD__THRESHOLD_set(v)       _BGQ_SET(39,63,v)
#define CS_DCR__THRESHOLD__THRESHOLD_insert(x,v)  _BGQ_INSERT(39,63,x,v)

  
// Register:
//   Name:        phase
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged and TestInt
//   Reset:       0000000000000000
//   Description:  This register has the phase that the counter will be compared against
#define CS_DCR__PHASE_offset  (0x0003) // Offset of register in instance 
#define CS_DCR__PHASE_reset  UNSIGNED64(0x0000000000000000)

//   Field:       phase
//   Description:  5 bit phase. The actual pclk that is stopped is threshold*48+phase  
#define CS_DCR__PHASE__PHASE_width        6
#define CS_DCR__PHASE__PHASE_position     63
#define CS_DCR__PHASE__PHASE_get(x)       _BGQ_GET(6,63,x)
#define CS_DCR__PHASE__PHASE_set(v)       _BGQ_SET(6,63,v)
#define CS_DCR__PHASE__PHASE_insert(x,v)  _BGQ_INSERT(6,63,x,v)

  
// Register:
//   Name:        status_1
//   Type:        Read Only
//   Permission:  All
//   Reset:       0000000000000000
//   Description:  This is the status1 register 
#define CS_DCR__STATUS_1_offset  (0x0004) // Offset of register in instance 
#define CS_DCR__STATUS_1_reset  UNSIGNED64(0x0000000000000000)

//   Field:       current_count
//   Description:  A snapshot of the current 39 bit slow count   
#define CS_DCR__STATUS_1__CURRENT_COUNT_width        39
#define CS_DCR__STATUS_1__CURRENT_COUNT_position     63
#define CS_DCR__STATUS_1__CURRENT_COUNT_get(x)       _BGQ_GET(39,63,x)
#define CS_DCR__STATUS_1__CURRENT_COUNT_set(v)       _BGQ_SET(39,63,v)
#define CS_DCR__STATUS_1__CURRENT_COUNT_insert(x,v)  _BGQ_INSERT(39,63,x,v)

  
// Register:
//   Name:        status_2
//   Type:        Read Only
//   Permission:  All
//   Reset:       0000000000000000
//   Description:  This is the status of the other states 
#define CS_DCR__STATUS_2_offset  (0x0005) // Offset of register in instance 
#define CS_DCR__STATUS_2_reset  UNSIGNED64(0x0000000000000000)

//   Field:       bic_cs_stop
//   Description:  This is the from the BIC. When   '1' if the enable for this enable_bic_stop is true and outputs are enabled it will unconditionally stop clocks. No check on threshold.
#define CS_DCR__STATUS_2__BIC_CS_STOP_width        1
#define CS_DCR__STATUS_2__BIC_CS_STOP_position     62
#define CS_DCR__STATUS_2__BIC_CS_STOP_get(x)       _BGQ_GET(1,62,x)
#define CS_DCR__STATUS_2__BIC_CS_STOP_set(v)       _BGQ_SET(1,62,v)
#define CS_DCR__STATUS_2__BIC_CS_STOP_insert(x,v)  _BGQ_INSERT(1,62,x,v)

//   Field:       precise_output
//   Description:  The value of the precise clock stop output '1' means asserted   
#define CS_DCR__STATUS_2__PRECISE_OUTPUT_width        1
#define CS_DCR__STATUS_2__PRECISE_OUTPUT_position     63
#define CS_DCR__STATUS_2__PRECISE_OUTPUT_get(x)       _BGQ_GET(1,63,x)
#define CS_DCR__STATUS_2__PRECISE_OUTPUT_set(v)       _BGQ_SET(1,63,v)
#define CS_DCR__STATUS_2__PRECISE_OUTPUT_insert(x,v)  _BGQ_INSERT(1,63,x,v)

  
// Register:
//   Name:        interrupt_internal_error_sw_info
//   Type:        Interrupt Information
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Contains status for software type internal errors
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO_offset  (0x0027) // Offset of register in instance 
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO_reset  UNSIGNED64(0x0000000000000000)

//   Field:       read
//   Description: set if the transaction was a read
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_width        1
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_position     0
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_get(x)       _BGQ_GET(1,0,x)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_set(v)       _BGQ_SET(1,0,v)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_insert(x,v)  _BGQ_INSERT(1,0,x,v)

//   Field:       write
//   Description: set if the transaction was a write
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_width        1
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_position     1
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_get(x)       _BGQ_GET(1,1,x)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_set(v)       _BGQ_SET(1,1,v)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_insert(x,v)  _BGQ_INSERT(1,1,x,v)

//   Field:       privileged
//   Description: set if the transaction had privileged permission
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_width        1
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_position     2
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_get(x)       _BGQ_GET(1,2,x)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_set(v)       _BGQ_SET(1,2,v)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_insert(x,v)  _BGQ_INSERT(1,2,x,v)

//   Field:       test_int
//   Description: set if the transaction had test_int permission
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_width        1
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_position     3
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_get(x)       _BGQ_GET(1,3,x)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_set(v)       _BGQ_SET(1,3,v)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_insert(x,v)  _BGQ_INSERT(1,3,x,v)

//   Field:       address
//   Description: reflects the address at the time of error
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_width        24
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_position     63
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_get(x)       _BGQ_GET(24,63,x)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_set(v)       _BGQ_SET(24,63,v)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_insert(x,v)  _BGQ_INSERT(24,63,x,v)

  
// Register:
//   Name:        interrupt_internal_error_hw_info
//   Type:        Interrupt Information
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Contains status for hardware type internal errors
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO_offset  (0x0028) // Offset of register in instance 
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address_parity_good
//   Description: reflects each individual parity status for address
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_width        3
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_position     6
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_get(x)       _BGQ_GET(3,6,x)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_set(v)       _BGQ_SET(3,6,v)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_insert(x,v)  _BGQ_INSERT(3,6,x,v)

//   Field:       data_parity_good
//   Description: reflects each individual parity status for data
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_width        8
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_position     14
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_get(x)       _BGQ_GET(8,14,x)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_set(v)       _BGQ_SET(8,14,v)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_insert(x,v)  _BGQ_INSERT(8,14,x,v)

//   Field:       address
//   Description: reflects the address at the time of error
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_width        24
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_position     63
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_get(x)       _BGQ_GET(24,63,x)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_set(v)       _BGQ_SET(24,63,v)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_insert(x,v)  _BGQ_INSERT(24,63,x,v)

  
// Register:
//   Name:        interrupt_internal_error_data_info
//   Type:        Interrupt Information
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Contains status for hardware type internal errors
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO_offset  (0x0029) // Offset of register in instance 
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO_reset  UNSIGNED64(0x0000000000000000)

//   Field:       data
//   Description: reflects the local data at the time of error
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_width        64
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_position     63
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_get(x)       _BGQ_GET(64,63,x)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_set(v)       _BGQ_SET(64,63,v)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_insert(x,v)  _BGQ_INSERT(64,63,x,v)

  
// Interrupt State Register: clockstop_interrupt_state
//   State Permission: Read: All; Write: Privileged and TestInt
//   First Permission: Privileged and TestInt
//   Force Permission: Privileged and TestInt
#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE__STATE_offset         (0x0010) // Offset of State register in instance
#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE__FIRST_offset         (0x0014) // Offset of First register in instance
#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE__FORCE_offset         (0x0013) // Offset of Force register in instance
#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE__MACHINE_CHECK_offset (0x0015) // Offset of Machine Check register in instance
#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE__CRITICAL_offset      (0x0016) // Offset of Critical register in instance
#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE__NONCRITICAL_offset   (0x0017) // Offset of Noncritical register in instance

#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE__LOCAL_RING_width        1
#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE__LOCAL_RING_position     0
#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE__LOCAL_RING_get(x)       _BGQ_GET(1,0,x)
#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE__LOCAL_RING_set(v)       _BGQ_SET(1,0,v)
#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE__LOCAL_RING_insert(x,v)  _BGQ_INSERT(1,0,x,v)


#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE__STOPPED_width        1
#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE__STOPPED_position     1
#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE__STOPPED_get(x)       _BGQ_GET(1,1,x)
#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE__STOPPED_set(v)       _BGQ_SET(1,1,v)
#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE__STOPPED_insert(x,v)  _BGQ_INSERT(1,1,x,v)

  
// Interrupt State Register: interrupt_internal_error
//   State Permission: Read: All; Write: Privileged and TestInt
//   First Permission: Privileged and TestInt
//   Force Permission: Privileged and TestInt
#define CS_DCR__INTERRUPT_INTERNAL_ERROR__STATE_offset         (0x0020) // Offset of State register in instance
#define CS_DCR__INTERRUPT_INTERNAL_ERROR__FIRST_offset         (0x0023) // Offset of First register in instance
#define CS_DCR__INTERRUPT_INTERNAL_ERROR__FORCE_offset         (0x0022) // Offset of Force register in instance
#define CS_DCR__INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK_offset (0x0024) // Offset of Machine Check register in instance
#define CS_DCR__INTERRUPT_INTERNAL_ERROR__CRITICAL_offset      (0x0025) // Offset of Critical register in instance
#define CS_DCR__INTERRUPT_INTERNAL_ERROR__NONCRITICAL_offset   (0x0026) // Offset of Noncritical register in instance

#define CS_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_width        1
#define CS_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_position     0
#define CS_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_get(x)       _BGQ_GET(1,0,x)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_set(v)       _BGQ_SET(1,0,v)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_insert(x,v)  _BGQ_INSERT(1,0,x,v)


#define CS_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_width        1
#define CS_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_position     1
#define CS_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_get(x)       _BGQ_GET(1,1,x)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_set(v)       _BGQ_SET(1,1,v)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_insert(x,v)  _BGQ_INSERT(1,1,x,v)


#define CS_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_width        1
#define CS_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_position     2
#define CS_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_get(x)       _BGQ_GET(1,2,x)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_set(v)       _BGQ_SET(1,2,v)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_insert(x,v)  _BGQ_INSERT(1,2,x,v)


#define CS_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_width        1
#define CS_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_position     3
#define CS_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_get(x)       _BGQ_GET(1,3,x)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_set(v)       _BGQ_SET(1,3,v)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_insert(x,v)  _BGQ_INSERT(1,3,x,v)

  
// Interrupt Control Register: clockstop_interrupt_state_control_low
//   Permission: Read: All; Write: Privileged and TestInt
#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE_CONTROL_LOW_offset         (0x0011) // Offset of Control register in instance
  
// Interrupt Control Register: clockstop_interrupt_state_control_high
//   Permission: Read: All; Write: Privileged and TestInt
#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE_CONTROL_HIGH_offset         (0x0012) // Offset of Control register in instance

#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_width        2
#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_position     1
#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_get(x)       _BGQ_GET(2,1,x)
#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_set(v)       _BGQ_SET(2,1,v)
#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_insert(x,v)  _BGQ_INSERT(2,1,x,v)


#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE_CONTROL_HIGH__STOPPED_width        2
#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE_CONTROL_HIGH__STOPPED_position     3
#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE_CONTROL_HIGH__STOPPED_get(x)       _BGQ_GET(2,3,x)
#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE_CONTROL_HIGH__STOPPED_set(v)       _BGQ_SET(2,3,v)
#define CS_DCR__CLOCKSTOP_INTERRUPT_STATE_CONTROL_HIGH__STOPPED_insert(x,v)  _BGQ_INSERT(2,3,x,v)

  
// Interrupt Control Register: interrupt_internal_error_control_high
//   Permission: Read: All; Write: Privileged and TestInt
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH_offset         (0x0021) // Offset of Control register in instance

#define CS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_width        2
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_position     1
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_get(x)       _BGQ_GET(2,1,x)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(v)       _BGQ_SET(2,1,v)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_insert(x,v)  _BGQ_INSERT(2,1,x,v)


#define CS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_width        2
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_position     3
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_get(x)       _BGQ_GET(2,3,x)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(v)       _BGQ_SET(2,3,v)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_insert(x,v)  _BGQ_INSERT(2,3,x,v)


#define CS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_width        2
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_position     5
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_get(x)       _BGQ_GET(2,5,x)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(v)       _BGQ_SET(2,5,v)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_insert(x,v)  _BGQ_INSERT(2,5,x,v)


#define CS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_width        2
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_position     7
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_get(x)       _BGQ_GET(2,7,x)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(v)       _BGQ_SET(2,7,v)
#define CS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_insert(x,v)  _BGQ_INSERT(2,7,x,v)

#ifndef __ASSEMBLY__

typedef struct cs_dcr { 
    /* 0x0000 */ uint64_t controlreg_0;
    /* 0x0001 */ uint64_t controlreg_1;
    /* 0x0002 */ uint64_t threshold;
    /* 0x0003 */ uint64_t phase;
    /* 0x0004 */ uint64_t status_1;
    /* 0x0005 */ uint64_t status_2;
    /* 0x0006 */ uint64_t reserve_0006[0x000A];
    /* 0x0010 */ uint64_t clockstop_interrupt_state__state;
    /* 0x0011 */ uint64_t clockstop_interrupt_state_control_low;
    /* 0x0012 */ uint64_t clockstop_interrupt_state_control_high;
    /* 0x0013 */ uint64_t clockstop_interrupt_state__force;
    /* 0x0014 */ uint64_t clockstop_interrupt_state__first;
    /* 0x0015 */ uint64_t clockstop_interrupt_state__machine_check;
    /* 0x0016 */ uint64_t clockstop_interrupt_state__critical;
    /* 0x0017 */ uint64_t clockstop_interrupt_state__noncritical;
    /* 0x0018 */ uint64_t reserve_0018[0x0008];
    /* 0x0020 */ uint64_t interrupt_internal_error__state;
    /* 0x0021 */ uint64_t interrupt_internal_error_control_high;
    /* 0x0022 */ uint64_t interrupt_internal_error__force;
    /* 0x0023 */ uint64_t interrupt_internal_error__first;
    /* 0x0024 */ uint64_t interrupt_internal_error__machine_check;
    /* 0x0025 */ uint64_t interrupt_internal_error__critical;
    /* 0x0026 */ uint64_t interrupt_internal_error__noncritical;
    /* 0x0027 */ uint64_t interrupt_internal_error_sw_info;
    /* 0x0028 */ uint64_t interrupt_internal_error_hw_info;
} cs_dcr_t;
  
#define CS_DCR_USER_PTR _DCR_USER_PTR(cs_dcr_t, CS_DCR)
#define CS_DCR_PRIV_PTR _DCR_PRIV_PTR(cs_dcr_t, CS_DCR)
  
#endif // !__ASSEMBLY__
  
__END_DECLS
  
#endif // add nothing below this line.
