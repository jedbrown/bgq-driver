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

#ifndef   _MS_GENCT_DCR__H_ // Prevent multiple inclusion
#define   _MS_GENCT_DCR__H_
  
__BEGIN_DECLS
  
#include <hwi/include/common/bgq_bitnumbers.h>
#include <hwi/include/bqc/dcr_support.h>
  
#define MS_GENCT_DCR_base    (0x15C000)  // Base address for all instances
#define MS_GENCT_DCR_size    (0x0029)    // Size of instance (largest offset + 1 )
  
#define MS_GENCT_DCR(reg) _DCR_REG(MS_GENCT_DCR, reg)
  
// Register:
//   Name:        ctrl
//   Type:        Read/Write Internal
//   Permission:  Privileged and TestInt
//   Reset:       0000000000000008
//   Description: Control Reg
#define MS_GENCT_DCR__CTRL_offset  (0x0000) // Offset of register in instance 
#define MS_GENCT_DCR__CTRL_reset  UNSIGNED64(0x0000000000000008)

//   Field:       reuse_delay
//   Description: Delay between stepping away from one generation until it us available for reuse
#define MS_GENCT_DCR__CTRL__REUSE_DELAY_width        6
#define MS_GENCT_DCR__CTRL__REUSE_DELAY_position     63
#define MS_GENCT_DCR__CTRL__REUSE_DELAY_get(x)       _BGQ_GET(6,63,x)
#define MS_GENCT_DCR__CTRL__REUSE_DELAY_set(v)       _BGQ_SET(6,63,v)
#define MS_GENCT_DCR__CTRL__REUSE_DELAY_insert(x,v)  _BGQ_INSERT(6,63,x,v)

  
// Register:
//   Name:        timeout
//   Type:        Read/Write Internal
//   Permission:  Privileged and TestInt
//   Reset:       00000000FFFFFFFF
//   Description: Timeout Reg
#define MS_GENCT_DCR__TIMEOUT_offset  (0x0001) // Offset of register in instance 
#define MS_GENCT_DCR__TIMEOUT_reset  UNSIGNED64(0x00000000FFFFFFFF)

//   Field:       init
//   Description: Timeout between step request and actual step
#define MS_GENCT_DCR__TIMEOUT__INIT_width        32
#define MS_GENCT_DCR__TIMEOUT__INIT_position     63
#define MS_GENCT_DCR__TIMEOUT__INIT_get(x)       _BGQ_GET(32,63,x)
#define MS_GENCT_DCR__TIMEOUT__INIT_set(v)       _BGQ_SET(32,63,v)
#define MS_GENCT_DCR__TIMEOUT__INIT_insert(x,v)  _BGQ_INSERT(32,63,x,v)

  
// Register:
//   Name:        interrupt_internal_error_sw_info
//   Type:        Interrupt Information
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Contains status for software type internal errors
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO_offset  (0x0027) // Offset of register in instance 
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO_reset  UNSIGNED64(0x0000000000000000)

//   Field:       read
//   Description: set if the transaction was a read
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_width        1
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_position     0
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_get(x)       _BGQ_GET(1,0,x)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_set(v)       _BGQ_SET(1,0,v)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_insert(x,v)  _BGQ_INSERT(1,0,x,v)

//   Field:       write
//   Description: set if the transaction was a write
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_width        1
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_position     1
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_get(x)       _BGQ_GET(1,1,x)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_set(v)       _BGQ_SET(1,1,v)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_insert(x,v)  _BGQ_INSERT(1,1,x,v)

//   Field:       privileged
//   Description: set if the transaction had privileged permission
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_width        1
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_position     2
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_get(x)       _BGQ_GET(1,2,x)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_set(v)       _BGQ_SET(1,2,v)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_insert(x,v)  _BGQ_INSERT(1,2,x,v)

//   Field:       test_int
//   Description: set if the transaction had test_int permission
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_width        1
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_position     3
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_get(x)       _BGQ_GET(1,3,x)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_set(v)       _BGQ_SET(1,3,v)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_insert(x,v)  _BGQ_INSERT(1,3,x,v)

//   Field:       address
//   Description: reflects the address at the time of error
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_width        24
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_position     63
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_get(x)       _BGQ_GET(24,63,x)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_set(v)       _BGQ_SET(24,63,v)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_insert(x,v)  _BGQ_INSERT(24,63,x,v)

  
// Register:
//   Name:        interrupt_internal_error_hw_info
//   Type:        Interrupt Information
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Contains status for hardware type internal errors
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO_offset  (0x0028) // Offset of register in instance 
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address_parity_good
//   Description: reflects each individual parity status for address
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_width        3
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_position     6
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_get(x)       _BGQ_GET(3,6,x)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_set(v)       _BGQ_SET(3,6,v)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_insert(x,v)  _BGQ_INSERT(3,6,x,v)

//   Field:       data_parity_good
//   Description: reflects each individual parity status for data
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_width        8
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_position     14
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_get(x)       _BGQ_GET(8,14,x)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_set(v)       _BGQ_SET(8,14,v)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_insert(x,v)  _BGQ_INSERT(8,14,x,v)

//   Field:       address
//   Description: reflects the address at the time of error
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_width        24
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_position     63
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_get(x)       _BGQ_GET(24,63,x)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_set(v)       _BGQ_SET(24,63,v)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_insert(x,v)  _BGQ_INSERT(24,63,x,v)

  
// Register:
//   Name:        interrupt_internal_error_data_info
//   Type:        Interrupt Information
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Contains status for hardware type internal errors
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO_offset  (0x0029) // Offset of register in instance 
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO_reset  UNSIGNED64(0x0000000000000000)

//   Field:       data
//   Description: reflects the local data at the time of error
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_width        64
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_position     63
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_get(x)       _BGQ_GET(64,63,x)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_set(v)       _BGQ_SET(64,63,v)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_insert(x,v)  _BGQ_INSERT(64,63,x,v)

  
// Interrupt State Register: l2_interrupt_state
//   State Permission: Read: All; Write: Privileged and TestInt
//   First Permission: Privileged and TestInt
//   Force Permission: Privileged and TestInt
#define MS_GENCT_DCR__L2_INTERRUPT_STATE__STATE_offset         (0x0016) // Offset of State register in instance
#define MS_GENCT_DCR__L2_INTERRUPT_STATE__FIRST_offset         (0x001A) // Offset of First register in instance
#define MS_GENCT_DCR__L2_INTERRUPT_STATE__FORCE_offset         (0x0019) // Offset of Force register in instance
#define MS_GENCT_DCR__L2_INTERRUPT_STATE__MACHINE_CHECK_offset (0x001B) // Offset of Machine Check register in instance
#define MS_GENCT_DCR__L2_INTERRUPT_STATE__CRITICAL_offset      (0x001C) // Offset of Critical register in instance
#define MS_GENCT_DCR__L2_INTERRUPT_STATE__NONCRITICAL_offset   (0x001D) // Offset of Noncritical register in instance

#define MS_GENCT_DCR__L2_INTERRUPT_STATE__LOCAL_RING_width        1
#define MS_GENCT_DCR__L2_INTERRUPT_STATE__LOCAL_RING_position     0
#define MS_GENCT_DCR__L2_INTERRUPT_STATE__LOCAL_RING_get(x)       _BGQ_GET(1,0,x)
#define MS_GENCT_DCR__L2_INTERRUPT_STATE__LOCAL_RING_set(v)       _BGQ_SET(1,0,v)
#define MS_GENCT_DCR__L2_INTERRUPT_STATE__LOCAL_RING_insert(x,v)  _BGQ_INSERT(1,0,x,v)


#define MS_GENCT_DCR__L2_INTERRUPT_STATE__TIMEOUT_E_width        1
#define MS_GENCT_DCR__L2_INTERRUPT_STATE__TIMEOUT_E_position     1
#define MS_GENCT_DCR__L2_INTERRUPT_STATE__TIMEOUT_E_get(x)       _BGQ_GET(1,1,x)
#define MS_GENCT_DCR__L2_INTERRUPT_STATE__TIMEOUT_E_set(v)       _BGQ_SET(1,1,v)
#define MS_GENCT_DCR__L2_INTERRUPT_STATE__TIMEOUT_E_insert(x,v)  _BGQ_INSERT(1,1,x,v)

  
// Interrupt State Register: interrupt_internal_error
//   State Permission: Read: All; Write: Privileged and TestInt
//   First Permission: Privileged and TestInt
//   Force Permission: Privileged and TestInt
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR__STATE_offset         (0x0020) // Offset of State register in instance
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR__FIRST_offset         (0x0023) // Offset of First register in instance
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR__FORCE_offset         (0x0022) // Offset of Force register in instance
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK_offset (0x0024) // Offset of Machine Check register in instance
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR__CRITICAL_offset      (0x0025) // Offset of Critical register in instance
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR__NONCRITICAL_offset   (0x0026) // Offset of Noncritical register in instance

#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_width        1
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_position     0
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_get(x)       _BGQ_GET(1,0,x)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_set(v)       _BGQ_SET(1,0,v)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_insert(x,v)  _BGQ_INSERT(1,0,x,v)


#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_width        1
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_position     1
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_get(x)       _BGQ_GET(1,1,x)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_set(v)       _BGQ_SET(1,1,v)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_insert(x,v)  _BGQ_INSERT(1,1,x,v)


#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_width        1
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_position     2
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_get(x)       _BGQ_GET(1,2,x)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_set(v)       _BGQ_SET(1,2,v)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_insert(x,v)  _BGQ_INSERT(1,2,x,v)


#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_width        1
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_position     3
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_get(x)       _BGQ_GET(1,3,x)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_set(v)       _BGQ_SET(1,3,v)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_insert(x,v)  _BGQ_INSERT(1,3,x,v)

  
// Interrupt Control Register: l2_interrupt_state_control_low
//   Permission: Read: All; Write: Privileged and TestInt
#define MS_GENCT_DCR__L2_INTERRUPT_STATE_CONTROL_LOW_offset         (0x0017) // Offset of Control register in instance
  
// Interrupt Control Register: l2_interrupt_state_control_high
//   Permission: Read: All; Write: Privileged and TestInt
#define MS_GENCT_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH_offset         (0x0018) // Offset of Control register in instance

#define MS_GENCT_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_width        2
#define MS_GENCT_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_position     1
#define MS_GENCT_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_get(x)       _BGQ_GET(2,1,x)
#define MS_GENCT_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_set(v)       _BGQ_SET(2,1,v)
#define MS_GENCT_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_insert(x,v)  _BGQ_INSERT(2,1,x,v)


#define MS_GENCT_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__TIMEOUT_E_width        2
#define MS_GENCT_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__TIMEOUT_E_position     3
#define MS_GENCT_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__TIMEOUT_E_get(x)       _BGQ_GET(2,3,x)
#define MS_GENCT_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__TIMEOUT_E_set(v)       _BGQ_SET(2,3,v)
#define MS_GENCT_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__TIMEOUT_E_insert(x,v)  _BGQ_INSERT(2,3,x,v)

  
// Interrupt Control Register: interrupt_internal_error_control_high
//   Permission: Read: All; Write: Privileged and TestInt
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH_offset         (0x0021) // Offset of Control register in instance

#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_width        2
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_position     1
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_get(x)       _BGQ_GET(2,1,x)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(v)       _BGQ_SET(2,1,v)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_insert(x,v)  _BGQ_INSERT(2,1,x,v)


#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_width        2
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_position     3
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_get(x)       _BGQ_GET(2,3,x)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(v)       _BGQ_SET(2,3,v)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_insert(x,v)  _BGQ_INSERT(2,3,x,v)


#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_width        2
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_position     5
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_get(x)       _BGQ_GET(2,5,x)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(v)       _BGQ_SET(2,5,v)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_insert(x,v)  _BGQ_INSERT(2,5,x,v)


#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_width        2
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_position     7
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_get(x)       _BGQ_GET(2,7,x)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(v)       _BGQ_SET(2,7,v)
#define MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_insert(x,v)  _BGQ_INSERT(2,7,x,v)

#ifndef __ASSEMBLY__

typedef struct ms_genct_dcr { 
    /* 0x0000 */ uint64_t ctrl;
    /* 0x0001 */ uint64_t timeout;
    /* 0x0002 */ uint64_t reserve_0002[0x0014];
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
} ms_genct_dcr_t;
  
#define MS_GENCT_DCR_USER_PTR _DCR_USER_PTR(ms_genct_dcr_t, MS_GENCT_DCR)
#define MS_GENCT_DCR_PRIV_PTR _DCR_PRIV_PTR(ms_genct_dcr_t, MS_GENCT_DCR)
  
#endif // !__ASSEMBLY__
  
__END_DECLS
  
#endif // add nothing below this line.
