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

#ifndef   _EN_DCR__H_ // Prevent multiple inclusion
#define   _EN_DCR__H_
  
__BEGIN_DECLS
  
#include <hwi/include/common/bgq_bitnumbers.h>
#include <hwi/include/bqc/dcr_support.h>
  
#define EN_DCR_base    (0x0DC000)  // Base address for all instances
#define EN_DCR_size    (0x0049)    // Size of instance (largest offset + 1 )
  
#define EN_DCR(reg) _DCR_REG(EN_DCR, reg)
  
// Register:
//   Name:        en0_thresholds
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged and TestInt
//   Reset:       00000000000003FF
//   Description:  This register control the thresholds for the env0 register
#define EN_DCR__EN0_THRESHOLDS_offset  (0x0000) // Offset of register in instance 
#define EN_DCR__EN0_THRESHOLDS_reset  UNSIGNED64(0x00000000000003FF)

//   Field:       en0_max_threshold
//   Description:  This is upper threshold is applied to the value in the input en0 register. If the value is above this upper threshold then an interrupt is generated (if enabled)  
#define EN_DCR__EN0_THRESHOLDS__EN0_MAX_THRESHOLD_width        10
#define EN_DCR__EN0_THRESHOLDS__EN0_MAX_THRESHOLD_position     53
#define EN_DCR__EN0_THRESHOLDS__EN0_MAX_THRESHOLD_get(x)       _BGQ_GET(10,53,x)
#define EN_DCR__EN0_THRESHOLDS__EN0_MAX_THRESHOLD_set(v)       _BGQ_SET(10,53,v)
#define EN_DCR__EN0_THRESHOLDS__EN0_MAX_THRESHOLD_insert(x,v)  _BGQ_INSERT(10,53,x,v)

//   Field:       en0_min_threshold
//   Description:  This is lower threshold is applied to the value in the input en0 register. If the value is below this lower threshold then an interrupt is generated (if enabled)  
#define EN_DCR__EN0_THRESHOLDS__EN0_MIN_THRESHOLD_width        10
#define EN_DCR__EN0_THRESHOLDS__EN0_MIN_THRESHOLD_position     63
#define EN_DCR__EN0_THRESHOLDS__EN0_MIN_THRESHOLD_get(x)       _BGQ_GET(10,63,x)
#define EN_DCR__EN0_THRESHOLDS__EN0_MIN_THRESHOLD_set(v)       _BGQ_SET(10,63,v)
#define EN_DCR__EN0_THRESHOLDS__EN0_MIN_THRESHOLD_insert(x,v)  _BGQ_INSERT(10,63,x,v)

  
// Register:
//   Name:        en1_thresholds
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged and TestInt
//   Reset:       00000000000003FF
//   Description:  This register control the thresholds for the env1 register
#define EN_DCR__EN1_THRESHOLDS_offset  (0x0001) // Offset of register in instance 
#define EN_DCR__EN1_THRESHOLDS_reset  UNSIGNED64(0x00000000000003FF)

//   Field:       en1_max_threshold
//   Description:  This is upper threshold is applied to the value in the input en1 register. If the value is above this upper threshold then an interrupt is generated (if enabled)  
#define EN_DCR__EN1_THRESHOLDS__EN1_MAX_THRESHOLD_width        10
#define EN_DCR__EN1_THRESHOLDS__EN1_MAX_THRESHOLD_position     53
#define EN_DCR__EN1_THRESHOLDS__EN1_MAX_THRESHOLD_get(x)       _BGQ_GET(10,53,x)
#define EN_DCR__EN1_THRESHOLDS__EN1_MAX_THRESHOLD_set(v)       _BGQ_SET(10,53,v)
#define EN_DCR__EN1_THRESHOLDS__EN1_MAX_THRESHOLD_insert(x,v)  _BGQ_INSERT(10,53,x,v)

//   Field:       en1_min_threshold
//   Description:  This is lower threshold is applied to the value in the input en1 register. If the value is below this lower threshold then an interrupt is generated (if enabled)  
#define EN_DCR__EN1_THRESHOLDS__EN1_MIN_THRESHOLD_width        10
#define EN_DCR__EN1_THRESHOLDS__EN1_MIN_THRESHOLD_position     63
#define EN_DCR__EN1_THRESHOLDS__EN1_MIN_THRESHOLD_get(x)       _BGQ_GET(10,63,x)
#define EN_DCR__EN1_THRESHOLDS__EN1_MIN_THRESHOLD_set(v)       _BGQ_SET(10,63,v)
#define EN_DCR__EN1_THRESHOLDS__EN1_MIN_THRESHOLD_insert(x,v)  _BGQ_INSERT(10,63,x,v)

  
// Register:
//   Name:        en2_thresholds
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged and TestInt
//   Reset:       00000000000003FF
//   Description:  This register control the thresholds for the env0 register
#define EN_DCR__EN2_THRESHOLDS_offset  (0x0002) // Offset of register in instance 
#define EN_DCR__EN2_THRESHOLDS_reset  UNSIGNED64(0x00000000000003FF)

//   Field:       en2_max_threshold
//   Description:  This is upper threshold is applied to the value in the input en2 register. If the value is above this upper threshold then an interrupt is generated (if enabled)  
#define EN_DCR__EN2_THRESHOLDS__EN2_MAX_THRESHOLD_width        10
#define EN_DCR__EN2_THRESHOLDS__EN2_MAX_THRESHOLD_position     53
#define EN_DCR__EN2_THRESHOLDS__EN2_MAX_THRESHOLD_get(x)       _BGQ_GET(10,53,x)
#define EN_DCR__EN2_THRESHOLDS__EN2_MAX_THRESHOLD_set(v)       _BGQ_SET(10,53,v)
#define EN_DCR__EN2_THRESHOLDS__EN2_MAX_THRESHOLD_insert(x,v)  _BGQ_INSERT(10,53,x,v)

//   Field:       en2_min_threshold
//   Description:  This is lower threshold is applied to the value in the input en2 register. If the value is below this lower threshold then an interrupt is generated (if enabled)  
#define EN_DCR__EN2_THRESHOLDS__EN2_MIN_THRESHOLD_width        10
#define EN_DCR__EN2_THRESHOLDS__EN2_MIN_THRESHOLD_position     63
#define EN_DCR__EN2_THRESHOLDS__EN2_MIN_THRESHOLD_get(x)       _BGQ_GET(10,63,x)
#define EN_DCR__EN2_THRESHOLDS__EN2_MIN_THRESHOLD_set(v)       _BGQ_SET(10,63,v)
#define EN_DCR__EN2_THRESHOLDS__EN2_MIN_THRESHOLD_insert(x,v)  _BGQ_INSERT(10,63,x,v)

  
// Register:
//   Name:        en3_thresholds
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged and TestInt
//   Reset:       00000000000003FF
//   Description:  This register control the thresholds for the env3 register
#define EN_DCR__EN3_THRESHOLDS_offset  (0x0003) // Offset of register in instance 
#define EN_DCR__EN3_THRESHOLDS_reset  UNSIGNED64(0x00000000000003FF)

//   Field:       en3_max_threshold
//   Description:  This is upper threshold is applied to the value in the input en3 register. If the value is above this upper threshold then an interrupt is generated (if enabled)  
#define EN_DCR__EN3_THRESHOLDS__EN3_MAX_THRESHOLD_width        10
#define EN_DCR__EN3_THRESHOLDS__EN3_MAX_THRESHOLD_position     53
#define EN_DCR__EN3_THRESHOLDS__EN3_MAX_THRESHOLD_get(x)       _BGQ_GET(10,53,x)
#define EN_DCR__EN3_THRESHOLDS__EN3_MAX_THRESHOLD_set(v)       _BGQ_SET(10,53,v)
#define EN_DCR__EN3_THRESHOLDS__EN3_MAX_THRESHOLD_insert(x,v)  _BGQ_INSERT(10,53,x,v)

//   Field:       en3_min_threshold
//   Description:  This is lower threshold is applied to the value in the input en3 register. If the value is below this lower threshold then an interrupt is generated (if enabled)  
#define EN_DCR__EN3_THRESHOLDS__EN3_MIN_THRESHOLD_width        10
#define EN_DCR__EN3_THRESHOLDS__EN3_MIN_THRESHOLD_position     63
#define EN_DCR__EN3_THRESHOLDS__EN3_MIN_THRESHOLD_get(x)       _BGQ_GET(10,63,x)
#define EN_DCR__EN3_THRESHOLDS__EN3_MIN_THRESHOLD_set(v)       _BGQ_SET(10,63,v)
#define EN_DCR__EN3_THRESHOLDS__EN3_MIN_THRESHOLD_insert(x,v)  _BGQ_INSERT(10,63,x,v)

  
// Register:
//   Name:        en4_thresholds
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged and TestInt
//   Reset:       00000000000003FF
//   Description:  This register control the thresholds for the env4 register
#define EN_DCR__EN4_THRESHOLDS_offset  (0x0004) // Offset of register in instance 
#define EN_DCR__EN4_THRESHOLDS_reset  UNSIGNED64(0x00000000000003FF)

//   Field:       en4_max_threshold
//   Description:  This is upper threshold is applied to the value in the input en4 register. If the value is above this upper threshold then an interrupt is generated (if enabled)  
#define EN_DCR__EN4_THRESHOLDS__EN4_MAX_THRESHOLD_width        10
#define EN_DCR__EN4_THRESHOLDS__EN4_MAX_THRESHOLD_position     53
#define EN_DCR__EN4_THRESHOLDS__EN4_MAX_THRESHOLD_get(x)       _BGQ_GET(10,53,x)
#define EN_DCR__EN4_THRESHOLDS__EN4_MAX_THRESHOLD_set(v)       _BGQ_SET(10,53,v)
#define EN_DCR__EN4_THRESHOLDS__EN4_MAX_THRESHOLD_insert(x,v)  _BGQ_INSERT(10,53,x,v)

//   Field:       en4_min_threshold
//   Description:  This is lower threshold is applied to the value in the input en4 register. If the value is below this lower threshold then an interrupt is generated (if enabled)  
#define EN_DCR__EN4_THRESHOLDS__EN4_MIN_THRESHOLD_width        10
#define EN_DCR__EN4_THRESHOLDS__EN4_MIN_THRESHOLD_position     63
#define EN_DCR__EN4_THRESHOLDS__EN4_MIN_THRESHOLD_get(x)       _BGQ_GET(10,63,x)
#define EN_DCR__EN4_THRESHOLDS__EN4_MIN_THRESHOLD_set(v)       _BGQ_SET(10,63,v)
#define EN_DCR__EN4_THRESHOLDS__EN4_MIN_THRESHOLD_insert(x,v)  _BGQ_INSERT(10,63,x,v)

  
// Register:
//   Name:        en5_mask
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged and TestInt
//   Reset:       0000000000000000
//   Description:  This register controls the mask for the env5 register
#define EN_DCR__EN5_MASK_offset  (0x0005) // Offset of register in instance 
#define EN_DCR__EN5_MASK_reset  UNSIGNED64(0x0000000000000000)

//   Field:       en5_mask_register
//   Description:  Each bit in this register controls the mask for each of 10 independent interrupt bits If this mask bit is 0 then the bit will not generate an interrupt. The bits that these mask bits are applied to are the env5 status bits   
#define EN_DCR__EN5_MASK__EN5_MASK_REGISTER_width        10
#define EN_DCR__EN5_MASK__EN5_MASK_REGISTER_position     63
#define EN_DCR__EN5_MASK__EN5_MASK_REGISTER_get(x)       _BGQ_GET(10,63,x)
#define EN_DCR__EN5_MASK__EN5_MASK_REGISTER_set(v)       _BGQ_SET(10,63,v)
#define EN_DCR__EN5_MASK__EN5_MASK_REGISTER_insert(x,v)  _BGQ_INSERT(10,63,x,v)

  
// Register:
//   Name:        en_control
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged and TestInt
//   Reset:       0000000000000000
//   Description:  This register holds control information
#define EN_DCR__EN_CONTROL_offset  (0x0006) // Offset of register in instance 
#define EN_DCR__EN_CONTROL_reset  UNSIGNED64(0x0000000000000000)

//   Field:       FSM_reset
//   Description:  If enabled (='1'), this level will reset the FSM . This is a level so this must be '0' to allow the FSM to sync onto the incoming data. The FSM is also reset by the usual reset for the envmon module
#define EN_DCR__EN_CONTROL__FSM_RESET_width        1
#define EN_DCR__EN_CONTROL__FSM_RESET_position     62
#define EN_DCR__EN_CONTROL__FSM_RESET_get(x)       _BGQ_GET(1,62,x)
#define EN_DCR__EN_CONTROL__FSM_RESET_set(v)       _BGQ_SET(1,62,v)
#define EN_DCR__EN_CONTROL__FSM_RESET_insert(x,v)  _BGQ_INSERT(1,62,x,v)

//   Field:       skip_checksum_requirement
//   Description:  If enabled (='1'), the FSM that receives the incoming data stream will not require a correct checksum to update the data in the en input registers 
#define EN_DCR__EN_CONTROL__SKIP_CHECKSUM_REQUIREMENT_width        1
#define EN_DCR__EN_CONTROL__SKIP_CHECKSUM_REQUIREMENT_position     63
#define EN_DCR__EN_CONTROL__SKIP_CHECKSUM_REQUIREMENT_get(x)       _BGQ_GET(1,63,x)
#define EN_DCR__EN_CONTROL__SKIP_CHECKSUM_REQUIREMENT_set(v)       _BGQ_SET(1,63,v)
#define EN_DCR__EN_CONTROL__SKIP_CHECKSUM_REQUIREMENT_insert(x,v)  _BGQ_INSERT(1,63,x,v)

  
// Register:
//   Name:        en0_input
//   Type:        Read Only
//   Permission:  All
//   Reset:       0000000000000000
//   Description:  This register contains the value that has been received from the FPGA 
#define EN_DCR__EN0_INPUT_offset  (0x0010) // Offset of register in instance 
#define EN_DCR__EN0_INPUT_reset  UNSIGNED64(0x0000000000000000)

//   Field:       en0_input
//   Description:  This is the en0 input register   
#define EN_DCR__EN0_INPUT__EN0_INPUT_width        10
#define EN_DCR__EN0_INPUT__EN0_INPUT_position     63
#define EN_DCR__EN0_INPUT__EN0_INPUT_get(x)       _BGQ_GET(10,63,x)
#define EN_DCR__EN0_INPUT__EN0_INPUT_set(v)       _BGQ_SET(10,63,v)
#define EN_DCR__EN0_INPUT__EN0_INPUT_insert(x,v)  _BGQ_INSERT(10,63,x,v)

  
// Register:
//   Name:        en1_input
//   Type:        Read Only
//   Permission:  All
//   Reset:       0000000000000000
//   Description:  This register contains the value that has been received from the FPGA 
#define EN_DCR__EN1_INPUT_offset  (0x0011) // Offset of register in instance 
#define EN_DCR__EN1_INPUT_reset  UNSIGNED64(0x0000000000000000)

//   Field:       en1_input
//   Description:  This is the en1 input register   
#define EN_DCR__EN1_INPUT__EN1_INPUT_width        10
#define EN_DCR__EN1_INPUT__EN1_INPUT_position     63
#define EN_DCR__EN1_INPUT__EN1_INPUT_get(x)       _BGQ_GET(10,63,x)
#define EN_DCR__EN1_INPUT__EN1_INPUT_set(v)       _BGQ_SET(10,63,v)
#define EN_DCR__EN1_INPUT__EN1_INPUT_insert(x,v)  _BGQ_INSERT(10,63,x,v)

  
// Register:
//   Name:        en2_input
//   Type:        Read Only
//   Permission:  All
//   Reset:       0000000000000000
//   Description:  This register contains the value that has been received from the FPGA 
#define EN_DCR__EN2_INPUT_offset  (0x0012) // Offset of register in instance 
#define EN_DCR__EN2_INPUT_reset  UNSIGNED64(0x0000000000000000)

//   Field:       en2_input
//   Description:  This is the en2 input register   
#define EN_DCR__EN2_INPUT__EN2_INPUT_width        10
#define EN_DCR__EN2_INPUT__EN2_INPUT_position     63
#define EN_DCR__EN2_INPUT__EN2_INPUT_get(x)       _BGQ_GET(10,63,x)
#define EN_DCR__EN2_INPUT__EN2_INPUT_set(v)       _BGQ_SET(10,63,v)
#define EN_DCR__EN2_INPUT__EN2_INPUT_insert(x,v)  _BGQ_INSERT(10,63,x,v)

  
// Register:
//   Name:        en3_input
//   Type:        Read Only
//   Permission:  All
//   Reset:       0000000000000000
//   Description:  This register contains the value that has been received from the FPGA 
#define EN_DCR__EN3_INPUT_offset  (0x0013) // Offset of register in instance 
#define EN_DCR__EN3_INPUT_reset  UNSIGNED64(0x0000000000000000)

//   Field:       en3_input
//   Description:  This is the en3 input register   
#define EN_DCR__EN3_INPUT__EN3_INPUT_width        10
#define EN_DCR__EN3_INPUT__EN3_INPUT_position     63
#define EN_DCR__EN3_INPUT__EN3_INPUT_get(x)       _BGQ_GET(10,63,x)
#define EN_DCR__EN3_INPUT__EN3_INPUT_set(v)       _BGQ_SET(10,63,v)
#define EN_DCR__EN3_INPUT__EN3_INPUT_insert(x,v)  _BGQ_INSERT(10,63,x,v)

  
// Register:
//   Name:        en4_input
//   Type:        Read Only
//   Permission:  All
//   Reset:       0000000000000000
//   Description:  This register contains the value that has been received from the FPGA 
#define EN_DCR__EN4_INPUT_offset  (0x0014) // Offset of register in instance 
#define EN_DCR__EN4_INPUT_reset  UNSIGNED64(0x0000000000000000)

//   Field:       en4_input
//   Description:  This is the en4 input register   
#define EN_DCR__EN4_INPUT__EN4_INPUT_width        10
#define EN_DCR__EN4_INPUT__EN4_INPUT_position     63
#define EN_DCR__EN4_INPUT__EN4_INPUT_get(x)       _BGQ_GET(10,63,x)
#define EN_DCR__EN4_INPUT__EN4_INPUT_set(v)       _BGQ_SET(10,63,v)
#define EN_DCR__EN4_INPUT__EN4_INPUT_insert(x,v)  _BGQ_INSERT(10,63,x,v)

  
// Register:
//   Name:        en5_input
//   Type:        Read Only
//   Permission:  All
//   Reset:       0000000000000000
//   Description:  This register contains the value that has been received from the FPGA 
#define EN_DCR__EN5_INPUT_offset  (0x0015) // Offset of register in instance 
#define EN_DCR__EN5_INPUT_reset  UNSIGNED64(0x0000000000000000)

//   Field:       en5_input
//   Description:  This is the en5 input register : These are all used as general interrupt bits   
#define EN_DCR__EN5_INPUT__EN5_INPUT_width        10
#define EN_DCR__EN5_INPUT__EN5_INPUT_position     63
#define EN_DCR__EN5_INPUT__EN5_INPUT_get(x)       _BGQ_GET(10,63,x)
#define EN_DCR__EN5_INPUT__EN5_INPUT_set(v)       _BGQ_SET(10,63,v)
#define EN_DCR__EN5_INPUT__EN5_INPUT_insert(x,v)  _BGQ_INSERT(10,63,x,v)

  
// Register:
//   Name:        parity
//   Type:        Read Only
//   Permission:  All
//   Reset:       0000000000000000
//   Description:  This register contains the value that has been received from the FPGA 
#define EN_DCR__PARITY_offset  (0x0016) // Offset of register in instance 
#define EN_DCR__PARITY_reset  UNSIGNED64(0x0000000000000000)

//   Field:       parity_input
//   Description:  This is the parity from the 6 en registers that came from the fpga   
#define EN_DCR__PARITY__PARITY_INPUT_width        10
#define EN_DCR__PARITY__PARITY_INPUT_position     63
#define EN_DCR__PARITY__PARITY_INPUT_get(x)       _BGQ_GET(10,63,x)
#define EN_DCR__PARITY__PARITY_INPUT_set(v)       _BGQ_SET(10,63,v)
#define EN_DCR__PARITY__PARITY_INPUT_insert(x,v)  _BGQ_INSERT(10,63,x,v)

  
// Register:
//   Name:        tvsense
//   Type:        Read Only
//   Permission:  All
//   Reset:       0000000000000000
//   Description:  This register contains the tvsense data 
#define EN_DCR__TVSENSE_offset  (0x0017) // Offset of register in instance 
#define EN_DCR__TVSENSE_reset  UNSIGNED64(0x0000000000000000)

//   Field:       tvsense_valid
//   Description:  this bit indicates that the TVSENSE data is valid 
#define EN_DCR__TVSENSE__TVSENSE_VALID_width        1
#define EN_DCR__TVSENSE__TVSENSE_VALID_position     54
#define EN_DCR__TVSENSE__TVSENSE_VALID_get(x)       _BGQ_GET(1,54,x)
#define EN_DCR__TVSENSE__TVSENSE_VALID_set(v)       _BGQ_SET(1,54,v)
#define EN_DCR__TVSENSE__TVSENSE_VALID_insert(x,v)  _BGQ_INSERT(1,54,x,v)

//   Field:       tvsense_data
//   Description:  This is the 8 bit tvsense data   
#define EN_DCR__TVSENSE__TVSENSE_DATA_width        9
#define EN_DCR__TVSENSE__TVSENSE_DATA_position     63
#define EN_DCR__TVSENSE__TVSENSE_DATA_get(x)       _BGQ_GET(9,63,x)
#define EN_DCR__TVSENSE__TVSENSE_DATA_set(v)       _BGQ_SET(9,63,v)
#define EN_DCR__TVSENSE__TVSENSE_DATA_insert(x,v)  _BGQ_INSERT(9,63,x,v)

  
// Register:
//   Name:        FSM_debug
//   Type:        Read Only
//   Permission:  All
//   Reset:       0000000000000000
//   Description:  This register contains the FSM status information 
#define EN_DCR__FSM_DEBUG_offset  (0x0018) // Offset of register in instance 
#define EN_DCR__FSM_DEBUG_reset  UNSIGNED64(0x0000000000000000)

//   Field:       fsm_status
//   Description:  bits (52 to 59)The sampled state bit 0 is state 0, bit 1 is state 1 etc.. bit 60 : the parity of the incoming stream bit 61 : This bit when '1' indicated a stop error in the incoming pattern bit 62 : This bit when '1' indicated a start error in the incoming pattern bit 63 :  This bit when '1' indicates that a sync was achieved with the incoming stream and data in input registers is therefore valid
#define EN_DCR__FSM_DEBUG__FSM_STATUS_width        12
#define EN_DCR__FSM_DEBUG__FSM_STATUS_position     63
#define EN_DCR__FSM_DEBUG__FSM_STATUS_get(x)       _BGQ_GET(12,63,x)
#define EN_DCR__FSM_DEBUG__FSM_STATUS_set(v)       _BGQ_SET(12,63,v)
#define EN_DCR__FSM_DEBUG__FSM_STATUS_insert(x,v)  _BGQ_INSERT(12,63,x,v)

  
// Register:
//   Name:        user
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged and TestInt
//   Reset:       00000000AAAA7777
//   Description:  This register contains user data that is constantly update to the FPGA
#define EN_DCR__USER_offset  (0x0024) // Offset of register in instance 
#define EN_DCR__USER_reset  UNSIGNED64(0x00000000AAAA7777)

//   Field:       spare
//   Description:  spares for workarounds 
#define EN_DCR__USER__SPARE_width        13
#define EN_DCR__USER__SPARE_position     12
#define EN_DCR__USER__SPARE_get(x)       _BGQ_GET(13,12,x)
#define EN_DCR__USER__SPARE_set(v)       _BGQ_SET(13,12,v)
#define EN_DCR__USER__SPARE_insert(x,v)  _BGQ_INSERT(13,12,x,v)

//   Field:       data
//   Description:  This is the data sent to the FPGA 
#define EN_DCR__USER__DATA_width        51
#define EN_DCR__USER__DATA_position     63
#define EN_DCR__USER__DATA_get(x)       _BGQ_GET(51,63,x)
#define EN_DCR__USER__DATA_set(v)       _BGQ_SET(51,63,v)
#define EN_DCR__USER__DATA_insert(x,v)  _BGQ_INSERT(51,63,x,v)

  
// Register:
//   Name:        tvsense_thresholds
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged and TestInt
//   Reset:       00000000000000FF
//   Description:  This register contains threshoild adta that the tvsense data is checked against
#define EN_DCR__TVSENSE_THRESHOLDS_offset  (0x0025) // Offset of register in instance 
#define EN_DCR__TVSENSE_THRESHOLDS_reset  UNSIGNED64(0x00000000000000FF)

//   Field:       low_threshold
//   Description:  the lower threshold 
#define EN_DCR__TVSENSE_THRESHOLDS__LOW_THRESHOLD_width        9
#define EN_DCR__TVSENSE_THRESHOLDS__LOW_THRESHOLD_position     54
#define EN_DCR__TVSENSE_THRESHOLDS__LOW_THRESHOLD_get(x)       _BGQ_GET(9,54,x)
#define EN_DCR__TVSENSE_THRESHOLDS__LOW_THRESHOLD_set(v)       _BGQ_SET(9,54,v)
#define EN_DCR__TVSENSE_THRESHOLDS__LOW_THRESHOLD_insert(x,v)  _BGQ_INSERT(9,54,x,v)

//   Field:       high_theshold
//   Description:  the upper threshold 
#define EN_DCR__TVSENSE_THRESHOLDS__HIGH_THESHOLD_width        9
#define EN_DCR__TVSENSE_THRESHOLDS__HIGH_THESHOLD_position     63
#define EN_DCR__TVSENSE_THRESHOLDS__HIGH_THESHOLD_get(x)       _BGQ_GET(9,63,x)
#define EN_DCR__TVSENSE_THRESHOLDS__HIGH_THESHOLD_set(v)       _BGQ_SET(9,63,v)
#define EN_DCR__TVSENSE_THRESHOLDS__HIGH_THESHOLD_insert(x,v)  _BGQ_INSERT(9,63,x,v)

  
// Register:
//   Name:        interrupt_internal_error_sw_info
//   Type:        Interrupt Information
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Contains status for software type internal errors
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO_offset  (0x0047) // Offset of register in instance 
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO_reset  UNSIGNED64(0x0000000000000000)

//   Field:       read
//   Description: set if the transaction was a read
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_width        1
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_position     0
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_get(x)       _BGQ_GET(1,0,x)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_set(v)       _BGQ_SET(1,0,v)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_insert(x,v)  _BGQ_INSERT(1,0,x,v)

//   Field:       write
//   Description: set if the transaction was a write
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_width        1
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_position     1
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_get(x)       _BGQ_GET(1,1,x)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_set(v)       _BGQ_SET(1,1,v)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_insert(x,v)  _BGQ_INSERT(1,1,x,v)

//   Field:       privileged
//   Description: set if the transaction had privileged permission
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_width        1
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_position     2
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_get(x)       _BGQ_GET(1,2,x)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_set(v)       _BGQ_SET(1,2,v)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_insert(x,v)  _BGQ_INSERT(1,2,x,v)

//   Field:       test_int
//   Description: set if the transaction had test_int permission
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_width        1
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_position     3
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_get(x)       _BGQ_GET(1,3,x)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_set(v)       _BGQ_SET(1,3,v)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_insert(x,v)  _BGQ_INSERT(1,3,x,v)

//   Field:       address
//   Description: reflects the address at the time of error
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_width        24
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_position     63
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_get(x)       _BGQ_GET(24,63,x)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_set(v)       _BGQ_SET(24,63,v)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_insert(x,v)  _BGQ_INSERT(24,63,x,v)

  
// Register:
//   Name:        interrupt_internal_error_hw_info
//   Type:        Interrupt Information
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Contains status for hardware type internal errors
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO_offset  (0x0048) // Offset of register in instance 
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address_parity_good
//   Description: reflects each individual parity status for address
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_width        3
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_position     6
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_get(x)       _BGQ_GET(3,6,x)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_set(v)       _BGQ_SET(3,6,v)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_insert(x,v)  _BGQ_INSERT(3,6,x,v)

//   Field:       data_parity_good
//   Description: reflects each individual parity status for data
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_width        8
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_position     14
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_get(x)       _BGQ_GET(8,14,x)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_set(v)       _BGQ_SET(8,14,v)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_insert(x,v)  _BGQ_INSERT(8,14,x,v)

//   Field:       address
//   Description: reflects the address at the time of error
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_width        24
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_position     63
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_get(x)       _BGQ_GET(24,63,x)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_set(v)       _BGQ_SET(24,63,v)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_insert(x,v)  _BGQ_INSERT(24,63,x,v)

  
// Register:
//   Name:        interrupt_internal_error_data_info
//   Type:        Interrupt Information
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Contains status for hardware type internal errors
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO_offset  (0x0049) // Offset of register in instance 
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO_reset  UNSIGNED64(0x0000000000000000)

//   Field:       data
//   Description: reflects the local data at the time of error
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_width        64
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_position     63
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_get(x)       _BGQ_GET(64,63,x)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_set(v)       _BGQ_SET(64,63,v)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_insert(x,v)  _BGQ_INSERT(64,63,x,v)

  
// Interrupt State Register: envmon_interrupt_state
//   State Permission: Read: All; Write: Privileged and TestInt
//   First Permission: Privileged and TestInt
//   Force Permission: Privileged and TestInt
#define EN_DCR__ENVMON_INTERRUPT_STATE__STATE_offset         (0x0030) // Offset of State register in instance
#define EN_DCR__ENVMON_INTERRUPT_STATE__FIRST_offset         (0x0034) // Offset of First register in instance
#define EN_DCR__ENVMON_INTERRUPT_STATE__FORCE_offset         (0x0033) // Offset of Force register in instance
#define EN_DCR__ENVMON_INTERRUPT_STATE__MACHINE_CHECK_offset (0x0035) // Offset of Machine Check register in instance
#define EN_DCR__ENVMON_INTERRUPT_STATE__CRITICAL_offset      (0x0036) // Offset of Critical register in instance
#define EN_DCR__ENVMON_INTERRUPT_STATE__NONCRITICAL_offset   (0x0037) // Offset of Noncritical register in instance

#define EN_DCR__ENVMON_INTERRUPT_STATE__LOCAL_RING_width        1
#define EN_DCR__ENVMON_INTERRUPT_STATE__LOCAL_RING_position     0
#define EN_DCR__ENVMON_INTERRUPT_STATE__LOCAL_RING_get(x)       _BGQ_GET(1,0,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE__LOCAL_RING_set(v)       _BGQ_SET(1,0,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE__LOCAL_RING_insert(x,v)  _BGQ_INSERT(1,0,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN0_width        1
#define EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN0_position     1
#define EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN0_get(x)       _BGQ_GET(1,1,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN0_set(v)       _BGQ_SET(1,1,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN0_insert(x,v)  _BGQ_INSERT(1,1,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN1_width        1
#define EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN1_position     2
#define EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN1_get(x)       _BGQ_GET(1,2,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN1_set(v)       _BGQ_SET(1,2,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN1_insert(x,v)  _BGQ_INSERT(1,2,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN2_width        1
#define EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN2_position     3
#define EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN2_get(x)       _BGQ_GET(1,3,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN2_set(v)       _BGQ_SET(1,3,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN2_insert(x,v)  _BGQ_INSERT(1,3,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN3_width        1
#define EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN3_position     4
#define EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN3_get(x)       _BGQ_GET(1,4,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN3_set(v)       _BGQ_SET(1,4,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN3_insert(x,v)  _BGQ_INSERT(1,4,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN4_width        1
#define EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN4_position     5
#define EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN4_get(x)       _BGQ_GET(1,5,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN4_set(v)       _BGQ_SET(1,5,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN4_insert(x,v)  _BGQ_INSERT(1,5,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE__GENERAL_INTERRUPT_EN5_width        1
#define EN_DCR__ENVMON_INTERRUPT_STATE__GENERAL_INTERRUPT_EN5_position     6
#define EN_DCR__ENVMON_INTERRUPT_STATE__GENERAL_INTERRUPT_EN5_get(x)       _BGQ_GET(1,6,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE__GENERAL_INTERRUPT_EN5_set(v)       _BGQ_SET(1,6,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE__GENERAL_INTERRUPT_EN5_insert(x,v)  _BGQ_INSERT(1,6,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE__LESSTHAN_EN0_width        1
#define EN_DCR__ENVMON_INTERRUPT_STATE__LESSTHAN_EN0_position     7
#define EN_DCR__ENVMON_INTERRUPT_STATE__LESSTHAN_EN0_get(x)       _BGQ_GET(1,7,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE__LESSTHAN_EN0_set(v)       _BGQ_SET(1,7,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE__LESSTHAN_EN0_insert(x,v)  _BGQ_INSERT(1,7,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE__LESSTHAN_EN1_width        1
#define EN_DCR__ENVMON_INTERRUPT_STATE__LESSTHAN_EN1_position     8
#define EN_DCR__ENVMON_INTERRUPT_STATE__LESSTHAN_EN1_get(x)       _BGQ_GET(1,8,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE__LESSTHAN_EN1_set(v)       _BGQ_SET(1,8,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE__LESSTHAN_EN1_insert(x,v)  _BGQ_INSERT(1,8,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE__LESSTHAN_EN2_width        1
#define EN_DCR__ENVMON_INTERRUPT_STATE__LESSTHAN_EN2_position     9
#define EN_DCR__ENVMON_INTERRUPT_STATE__LESSTHAN_EN2_get(x)       _BGQ_GET(1,9,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE__LESSTHAN_EN2_set(v)       _BGQ_SET(1,9,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE__LESSTHAN_EN2_insert(x,v)  _BGQ_INSERT(1,9,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE__LESSTHAN_EN3_width        1
#define EN_DCR__ENVMON_INTERRUPT_STATE__LESSTHAN_EN3_position     10
#define EN_DCR__ENVMON_INTERRUPT_STATE__LESSTHAN_EN3_get(x)       _BGQ_GET(1,10,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE__LESSTHAN_EN3_set(v)       _BGQ_SET(1,10,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE__LESSTHAN_EN3_insert(x,v)  _BGQ_INSERT(1,10,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE__LESSTHAN_EN4_width        1
#define EN_DCR__ENVMON_INTERRUPT_STATE__LESSTHAN_EN4_position     11
#define EN_DCR__ENVMON_INTERRUPT_STATE__LESSTHAN_EN4_get(x)       _BGQ_GET(1,11,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE__LESSTHAN_EN4_set(v)       _BGQ_SET(1,11,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE__LESSTHAN_EN4_insert(x,v)  _BGQ_INSERT(1,11,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE__FSM_CHECKSUM_FAIL_width        1
#define EN_DCR__ENVMON_INTERRUPT_STATE__FSM_CHECKSUM_FAIL_position     12
#define EN_DCR__ENVMON_INTERRUPT_STATE__FSM_CHECKSUM_FAIL_get(x)       _BGQ_GET(1,12,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE__FSM_CHECKSUM_FAIL_set(v)       _BGQ_SET(1,12,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE__FSM_CHECKSUM_FAIL_insert(x,v)  _BGQ_INSERT(1,12,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE__FSM_SEQ_FAIL_width        1
#define EN_DCR__ENVMON_INTERRUPT_STATE__FSM_SEQ_FAIL_position     13
#define EN_DCR__ENVMON_INTERRUPT_STATE__FSM_SEQ_FAIL_get(x)       _BGQ_GET(1,13,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE__FSM_SEQ_FAIL_set(v)       _BGQ_SET(1,13,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE__FSM_SEQ_FAIL_insert(x,v)  _BGQ_INSERT(1,13,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE__TVSENSE_EXCEEDS_MAX_width        1
#define EN_DCR__ENVMON_INTERRUPT_STATE__TVSENSE_EXCEEDS_MAX_position     14
#define EN_DCR__ENVMON_INTERRUPT_STATE__TVSENSE_EXCEEDS_MAX_get(x)       _BGQ_GET(1,14,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE__TVSENSE_EXCEEDS_MAX_set(v)       _BGQ_SET(1,14,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE__TVSENSE_EXCEEDS_MAX_insert(x,v)  _BGQ_INSERT(1,14,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE__TVSENSE_LESSTHAN_MIN_width        1
#define EN_DCR__ENVMON_INTERRUPT_STATE__TVSENSE_LESSTHAN_MIN_position     15
#define EN_DCR__ENVMON_INTERRUPT_STATE__TVSENSE_LESSTHAN_MIN_get(x)       _BGQ_GET(1,15,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE__TVSENSE_LESSTHAN_MIN_set(v)       _BGQ_SET(1,15,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE__TVSENSE_LESSTHAN_MIN_insert(x,v)  _BGQ_INSERT(1,15,x,v)

  
// Interrupt State Register: interrupt_internal_error
//   State Permission: Read: All; Write: Privileged and TestInt
//   First Permission: Privileged and TestInt
//   Force Permission: Privileged and TestInt
#define EN_DCR__INTERRUPT_INTERNAL_ERROR__STATE_offset         (0x0040) // Offset of State register in instance
#define EN_DCR__INTERRUPT_INTERNAL_ERROR__FIRST_offset         (0x0043) // Offset of First register in instance
#define EN_DCR__INTERRUPT_INTERNAL_ERROR__FORCE_offset         (0x0042) // Offset of Force register in instance
#define EN_DCR__INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK_offset (0x0044) // Offset of Machine Check register in instance
#define EN_DCR__INTERRUPT_INTERNAL_ERROR__CRITICAL_offset      (0x0045) // Offset of Critical register in instance
#define EN_DCR__INTERRUPT_INTERNAL_ERROR__NONCRITICAL_offset   (0x0046) // Offset of Noncritical register in instance

#define EN_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_width        1
#define EN_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_position     0
#define EN_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_get(x)       _BGQ_GET(1,0,x)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_set(v)       _BGQ_SET(1,0,v)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_insert(x,v)  _BGQ_INSERT(1,0,x,v)


#define EN_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_width        1
#define EN_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_position     1
#define EN_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_get(x)       _BGQ_GET(1,1,x)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_set(v)       _BGQ_SET(1,1,v)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_insert(x,v)  _BGQ_INSERT(1,1,x,v)


#define EN_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_width        1
#define EN_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_position     2
#define EN_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_get(x)       _BGQ_GET(1,2,x)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_set(v)       _BGQ_SET(1,2,v)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_insert(x,v)  _BGQ_INSERT(1,2,x,v)


#define EN_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_width        1
#define EN_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_position     3
#define EN_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_get(x)       _BGQ_GET(1,3,x)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_set(v)       _BGQ_SET(1,3,v)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_insert(x,v)  _BGQ_INSERT(1,3,x,v)

  
// Interrupt Control Register: envmon_interrupt_state_control_low
//   Permission: Read: All; Write: Privileged and TestInt
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_LOW_offset         (0x0031) // Offset of Control register in instance
  
// Interrupt Control Register: envmon_interrupt_state_control_high
//   Permission: Read: All; Write: Privileged and TestInt
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH_offset         (0x0032) // Offset of Control register in instance

#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_width        2
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_position     1
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_get(x)       _BGQ_GET(2,1,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_set(v)       _BGQ_SET(2,1,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_insert(x,v)  _BGQ_INSERT(2,1,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN0_width        2
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN0_position     3
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN0_get(x)       _BGQ_GET(2,3,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN0_set(v)       _BGQ_SET(2,3,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN0_insert(x,v)  _BGQ_INSERT(2,3,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN1_width        2
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN1_position     5
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN1_get(x)       _BGQ_GET(2,5,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN1_set(v)       _BGQ_SET(2,5,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN1_insert(x,v)  _BGQ_INSERT(2,5,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN2_width        2
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN2_position     7
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN2_get(x)       _BGQ_GET(2,7,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN2_set(v)       _BGQ_SET(2,7,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN2_insert(x,v)  _BGQ_INSERT(2,7,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN3_width        2
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN3_position     9
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN3_get(x)       _BGQ_GET(2,9,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN3_set(v)       _BGQ_SET(2,9,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN3_insert(x,v)  _BGQ_INSERT(2,9,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN4_width        2
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN4_position     11
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN4_get(x)       _BGQ_GET(2,11,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN4_set(v)       _BGQ_SET(2,11,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN4_insert(x,v)  _BGQ_INSERT(2,11,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__GENERAL_INTERRUPT_EN5_width        2
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__GENERAL_INTERRUPT_EN5_position     13
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__GENERAL_INTERRUPT_EN5_get(x)       _BGQ_GET(2,13,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__GENERAL_INTERRUPT_EN5_set(v)       _BGQ_SET(2,13,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__GENERAL_INTERRUPT_EN5_insert(x,v)  _BGQ_INSERT(2,13,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN0_width        2
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN0_position     15
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN0_get(x)       _BGQ_GET(2,15,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN0_set(v)       _BGQ_SET(2,15,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN0_insert(x,v)  _BGQ_INSERT(2,15,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN1_width        2
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN1_position     17
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN1_get(x)       _BGQ_GET(2,17,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN1_set(v)       _BGQ_SET(2,17,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN1_insert(x,v)  _BGQ_INSERT(2,17,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN2_width        2
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN2_position     19
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN2_get(x)       _BGQ_GET(2,19,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN2_set(v)       _BGQ_SET(2,19,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN2_insert(x,v)  _BGQ_INSERT(2,19,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN3_width        2
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN3_position     21
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN3_get(x)       _BGQ_GET(2,21,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN3_set(v)       _BGQ_SET(2,21,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN3_insert(x,v)  _BGQ_INSERT(2,21,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN4_width        2
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN4_position     23
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN4_get(x)       _BGQ_GET(2,23,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN4_set(v)       _BGQ_SET(2,23,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN4_insert(x,v)  _BGQ_INSERT(2,23,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__FSM_CHECKSUM_FAIL_width        2
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__FSM_CHECKSUM_FAIL_position     25
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__FSM_CHECKSUM_FAIL_get(x)       _BGQ_GET(2,25,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__FSM_CHECKSUM_FAIL_set(v)       _BGQ_SET(2,25,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__FSM_CHECKSUM_FAIL_insert(x,v)  _BGQ_INSERT(2,25,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__FSM_SEQ_FAIL_width        2
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__FSM_SEQ_FAIL_position     27
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__FSM_SEQ_FAIL_get(x)       _BGQ_GET(2,27,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__FSM_SEQ_FAIL_set(v)       _BGQ_SET(2,27,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__FSM_SEQ_FAIL_insert(x,v)  _BGQ_INSERT(2,27,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__TVSENSE_EXCEEDS_MAX_width        2
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__TVSENSE_EXCEEDS_MAX_position     29
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__TVSENSE_EXCEEDS_MAX_get(x)       _BGQ_GET(2,29,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__TVSENSE_EXCEEDS_MAX_set(v)       _BGQ_SET(2,29,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__TVSENSE_EXCEEDS_MAX_insert(x,v)  _BGQ_INSERT(2,29,x,v)


#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__TVSENSE_LESSTHAN_MIN_width        2
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__TVSENSE_LESSTHAN_MIN_position     31
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__TVSENSE_LESSTHAN_MIN_get(x)       _BGQ_GET(2,31,x)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__TVSENSE_LESSTHAN_MIN_set(v)       _BGQ_SET(2,31,v)
#define EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__TVSENSE_LESSTHAN_MIN_insert(x,v)  _BGQ_INSERT(2,31,x,v)

  
// Interrupt Control Register: interrupt_internal_error_control_high
//   Permission: Read: All; Write: Privileged and TestInt
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH_offset         (0x0041) // Offset of Control register in instance

#define EN_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_width        2
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_position     1
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_get(x)       _BGQ_GET(2,1,x)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(v)       _BGQ_SET(2,1,v)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_insert(x,v)  _BGQ_INSERT(2,1,x,v)


#define EN_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_width        2
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_position     3
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_get(x)       _BGQ_GET(2,3,x)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(v)       _BGQ_SET(2,3,v)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_insert(x,v)  _BGQ_INSERT(2,3,x,v)


#define EN_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_width        2
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_position     5
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_get(x)       _BGQ_GET(2,5,x)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(v)       _BGQ_SET(2,5,v)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_insert(x,v)  _BGQ_INSERT(2,5,x,v)


#define EN_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_width        2
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_position     7
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_get(x)       _BGQ_GET(2,7,x)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(v)       _BGQ_SET(2,7,v)
#define EN_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_insert(x,v)  _BGQ_INSERT(2,7,x,v)

#ifndef __ASSEMBLY__

typedef struct en_dcr { 
    /* 0x0000 */ uint64_t en0_thresholds;
    /* 0x0001 */ uint64_t en1_thresholds;
    /* 0x0002 */ uint64_t en2_thresholds;
    /* 0x0003 */ uint64_t en3_thresholds;
    /* 0x0004 */ uint64_t en4_thresholds;
    /* 0x0005 */ uint64_t en5_mask;
    /* 0x0006 */ uint64_t en_control;
    /* 0x0007 */ uint64_t reserve_0007[0x0009];
    /* 0x0010 */ uint64_t en0_input;
    /* 0x0011 */ uint64_t en1_input;
    /* 0x0012 */ uint64_t en2_input;
    /* 0x0013 */ uint64_t en3_input;
    /* 0x0014 */ uint64_t en4_input;
    /* 0x0015 */ uint64_t en5_input;
    /* 0x0016 */ uint64_t parity;
    /* 0x0017 */ uint64_t tvsense;
    /* 0x0018 */ uint64_t FSM_debug;
    /* 0x0019 */ uint64_t reserve_0019[0x000B];
    /* 0x0024 */ uint64_t user;
    /* 0x0025 */ uint64_t tvsense_thresholds;
    /* 0x0026 */ uint64_t reserve_0026[0x000A];
    /* 0x0030 */ uint64_t envmon_interrupt_state__state;
    /* 0x0031 */ uint64_t envmon_interrupt_state_control_low;
    /* 0x0032 */ uint64_t envmon_interrupt_state_control_high;
    /* 0x0033 */ uint64_t envmon_interrupt_state__force;
    /* 0x0034 */ uint64_t envmon_interrupt_state__first;
    /* 0x0035 */ uint64_t envmon_interrupt_state__machine_check;
    /* 0x0036 */ uint64_t envmon_interrupt_state__critical;
    /* 0x0037 */ uint64_t envmon_interrupt_state__noncritical;
    /* 0x0038 */ uint64_t reserve_0038[0x0008];
    /* 0x0040 */ uint64_t interrupt_internal_error__state;
    /* 0x0041 */ uint64_t interrupt_internal_error_control_high;
    /* 0x0042 */ uint64_t interrupt_internal_error__force;
    /* 0x0043 */ uint64_t interrupt_internal_error__first;
    /* 0x0044 */ uint64_t interrupt_internal_error__machine_check;
    /* 0x0045 */ uint64_t interrupt_internal_error__critical;
    /* 0x0046 */ uint64_t interrupt_internal_error__noncritical;
    /* 0x0047 */ uint64_t interrupt_internal_error_sw_info;
    /* 0x0048 */ uint64_t interrupt_internal_error_hw_info;
} en_dcr_t;
  
#define EN_DCR_USER_PTR _DCR_USER_PTR(en_dcr_t, EN_DCR)
#define EN_DCR_PRIV_PTR _DCR_PRIV_PTR(en_dcr_t, EN_DCR)
  
#endif // !__ASSEMBLY__
  
__END_DECLS
  
#endif // add nothing below this line.
