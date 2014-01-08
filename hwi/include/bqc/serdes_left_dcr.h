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

#ifndef   _SERDES_LEFT_DCR__H_ // Prevent multiple inclusion
#define   _SERDES_LEFT_DCR__H_
  
__BEGIN_DECLS
  
#include <hwi/include/common/bgq_bitnumbers.h>
#include <hwi/include/bqc/dcr_support.h>
  
#define SERDES_LEFT_DCR_base    (0x140000)  // Base address for all instances
#define SERDES_LEFT_DCR_size    (0x0159)    // Size of instance (largest offset + 1 )
  
#define SERDES_LEFT_DCR(reg) _DCR_REG(SERDES_LEFT_DCR, reg)
  
// Register:
//   Name:        rcbus
//   Type:        Read/Write External
//   Permission:  Read: All; Write: Privileged and TestInt
//   Reset:       0000000000000000
//   Description: Allows access to a 256 byte window of RCBus address space
#define SERDES_LEFT_DCR__RCBUS_offset  (0x0000) // Offset of register in instance 
#define SERDES_LEFT_DCR__RCBUS_range  (0x100) // Range of external address space
#define SERDES_LEFT_DCR__RCBUS_reset  UNSIGNED64(0x0000000000000000)

//   Field:       wmask
//   Description: Write mask for RCBus masked write transactions
#define SERDES_LEFT_DCR__RCBUS__WMASK_width        16
#define SERDES_LEFT_DCR__RCBUS__WMASK_position     39
#define SERDES_LEFT_DCR__RCBUS__WMASK_get(x)       _BGQ_GET(16,39,x)
#define SERDES_LEFT_DCR__RCBUS__WMASK_set(v)       _BGQ_SET(16,39,v)
#define SERDES_LEFT_DCR__RCBUS__WMASK_insert(x,v)  _BGQ_INSERT(16,39,x,v)

//   Field:       masked_write
//   Description: When set, the RCbus write transaction will be a masked transaction
#define SERDES_LEFT_DCR__RCBUS__MASKED_WRITE_width        1
#define SERDES_LEFT_DCR__RCBUS__MASKED_WRITE_position     40
#define SERDES_LEFT_DCR__RCBUS__MASKED_WRITE_get(x)       _BGQ_GET(1,40,x)
#define SERDES_LEFT_DCR__RCBUS__MASKED_WRITE_set(v)       _BGQ_SET(1,40,v)
#define SERDES_LEFT_DCR__RCBUS__MASKED_WRITE_insert(x,v)  _BGQ_INSERT(1,40,x,v)

//   Field:       data
//   Description: RCbus read/write data
#define SERDES_LEFT_DCR__RCBUS__DATA_width        16
#define SERDES_LEFT_DCR__RCBUS__DATA_position     63
#define SERDES_LEFT_DCR__RCBUS__DATA_get(x)       _BGQ_GET(16,63,x)
#define SERDES_LEFT_DCR__RCBUS__DATA_set(v)       _BGQ_SET(16,63,v)
#define SERDES_LEFT_DCR__RCBUS__DATA_insert(x,v)  _BGQ_INSERT(16,63,x,v)

  
// Register:
//   Name:        rcbus_config
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged and TestInt
//   Reset:       0000000000000040
//   Description: Sets the upper address bits and transaction qualifiers for RCbus transactions
#define SERDES_LEFT_DCR__RCBUS_CONFIG_offset  (0x0100) // Offset of register in instance 
#define SERDES_LEFT_DCR__RCBUS_CONFIG_reset  UNSIGNED64(0x0000000000000040)

//   Field:       address
//   Description: Sets RCbus address bits 31:8
#define SERDES_LEFT_DCR__RCBUS_CONFIG__ADDRESS_width        24
#define SERDES_LEFT_DCR__RCBUS_CONFIG__ADDRESS_position     55
#define SERDES_LEFT_DCR__RCBUS_CONFIG__ADDRESS_get(x)       _BGQ_GET(24,55,x)
#define SERDES_LEFT_DCR__RCBUS_CONFIG__ADDRESS_set(v)       _BGQ_SET(24,55,v)
#define SERDES_LEFT_DCR__RCBUS_CONFIG__ADDRESS_insert(x,v)  _BGQ_INSERT(24,55,x,v)

//   Field:       reset
//   Description: Holds the RCBUS in reset
#define SERDES_LEFT_DCR__RCBUS_CONFIG__RESET_width        1
#define SERDES_LEFT_DCR__RCBUS_CONFIG__RESET_position     56
#define SERDES_LEFT_DCR__RCBUS_CONFIG__RESET_get(x)       _BGQ_GET(1,56,x)
#define SERDES_LEFT_DCR__RCBUS_CONFIG__RESET_set(v)       _BGQ_SET(1,56,v)
#define SERDES_LEFT_DCR__RCBUS_CONFIG__RESET_insert(x,v)  _BGQ_INSERT(1,56,x,v)

//   Field:       clk_en
//   Description: Enables dynamic clock control for PCS/PMA RCCLK
#define SERDES_LEFT_DCR__RCBUS_CONFIG__CLK_EN_width        1
#define SERDES_LEFT_DCR__RCBUS_CONFIG__CLK_EN_position     57
#define SERDES_LEFT_DCR__RCBUS_CONFIG__CLK_EN_get(x)       _BGQ_GET(1,57,x)
#define SERDES_LEFT_DCR__RCBUS_CONFIG__CLK_EN_set(v)       _BGQ_SET(1,57,v)
#define SERDES_LEFT_DCR__RCBUS_CONFIG__CLK_EN_insert(x,v)  _BGQ_INSERT(1,57,x,v)

  
// Register:
//   Name:        sd_config
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged and TestInt
//   Reset:       0000000000000000
//   Description: Serdes unit global configuration
#define SERDES_LEFT_DCR__SD_CONFIG_offset  (0x0101) // Offset of register in instance 
#define SERDES_LEFT_DCR__SD_CONFIG_reset  UNSIGNED64(0x0000000000000000)

//   Field:       hssl2pllon
//   Description: keeps pll on, even in L2 state
#define SERDES_LEFT_DCR__SD_CONFIG__HSSL2PLLON_width        1
#define SERDES_LEFT_DCR__SD_CONFIG__HSSL2PLLON_position     39
#define SERDES_LEFT_DCR__SD_CONFIG__HSSL2PLLON_get(x)       _BGQ_GET(1,39,x)
#define SERDES_LEFT_DCR__SD_CONFIG__HSSL2PLLON_set(v)       _BGQ_SET(1,39,v)
#define SERDES_LEFT_DCR__SD_CONFIG__HSSL2PLLON_insert(x,v)  _BGQ_INSERT(1,39,x,v)

//   Field:       pcs_testmode
//   Description: PCS testmode signal, should be set to 0
#define SERDES_LEFT_DCR__SD_CONFIG__PCS_TESTMODE_width        1
#define SERDES_LEFT_DCR__SD_CONFIG__PCS_TESTMODE_position     40
#define SERDES_LEFT_DCR__SD_CONFIG__PCS_TESTMODE_get(x)       _BGQ_GET(1,40,x)
#define SERDES_LEFT_DCR__SD_CONFIG__PCS_TESTMODE_set(v)       _BGQ_SET(1,40,v)
#define SERDES_LEFT_DCR__SD_CONFIG__PCS_TESTMODE_insert(x,v)  _BGQ_INSERT(1,40,x,v)

//   Field:       halfrate
//   Description: When set, Torus runs at half rate
#define SERDES_LEFT_DCR__SD_CONFIG__HALFRATE_width        1
#define SERDES_LEFT_DCR__SD_CONFIG__HALFRATE_position     41
#define SERDES_LEFT_DCR__SD_CONFIG__HALFRATE_get(x)       _BGQ_GET(1,41,x)
#define SERDES_LEFT_DCR__SD_CONFIG__HALFRATE_set(v)       _BGQ_SET(1,41,v)
#define SERDES_LEFT_DCR__SD_CONFIG__HALFRATE_insert(x,v)  _BGQ_INSERT(1,41,x,v)

  
// Register:
//   Name:        ts_ctl_a
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged and TestInt
//   Reset:       002FF420000001FE
//   Description: Torus dimension A control register
#define SERDES_LEFT_DCR__TS_CTL_A_offset  (0x0102) // Offset of register in instance 
#define SERDES_LEFT_DCR__TS_CTL_A_reset  UNSIGNED64(0x002FF420000001FE)

//   Field:       spare
//   Description: Spare control signals, no function assigned
#define SERDES_LEFT_DCR__TS_CTL_A__SPARE_width        4
#define SERDES_LEFT_DCR__TS_CTL_A__SPARE_position     9
#define SERDES_LEFT_DCR__TS_CTL_A__SPARE_get(x)       _BGQ_GET(4,9,x)
#define SERDES_LEFT_DCR__TS_CTL_A__SPARE_set(v)       _BGQ_SET(4,9,v)
#define SERDES_LEFT_DCR__TS_CTL_A__SPARE_insert(x,v)  _BGQ_INSERT(4,9,x,v)

//   Field:       pcs_reset
//   Description: Drives the HSSPCSRESET signal on PCS Core
#define SERDES_LEFT_DCR__TS_CTL_A__PCS_RESET_width        1
#define SERDES_LEFT_DCR__TS_CTL_A__PCS_RESET_position     10
#define SERDES_LEFT_DCR__TS_CTL_A__PCS_RESET_get(x)       _BGQ_GET(1,10,x)
#define SERDES_LEFT_DCR__TS_CTL_A__PCS_RESET_set(v)       _BGQ_SET(1,10,v)
#define SERDES_LEFT_DCR__TS_CTL_A__PCS_RESET_insert(x,v)  _BGQ_INSERT(1,10,x,v)

//   Field:       pipe_resetn
//   Description: Drives the PIPE0RESETN signal on PCS Core. Note that the PIPE0RESETN signal is active low.
#define SERDES_LEFT_DCR__TS_CTL_A__PIPE_RESETN_width        1
#define SERDES_LEFT_DCR__TS_CTL_A__PIPE_RESETN_position     11
#define SERDES_LEFT_DCR__TS_CTL_A__PIPE_RESETN_get(x)       _BGQ_GET(1,11,x)
#define SERDES_LEFT_DCR__TS_CTL_A__PIPE_RESETN_set(v)       _BGQ_SET(1,11,v)
#define SERDES_LEFT_DCR__TS_CTL_A__PIPE_RESETN_insert(x,v)  _BGQ_INSERT(1,11,x,v)

//   Field:       rx_reset_m
//   Description: Resets the RX logic in the PCS layer (PRBS checker). This field should not be set to zero until rx lane clocks are active.
#define SERDES_LEFT_DCR__TS_CTL_A__RX_RESET_M_width        4
#define SERDES_LEFT_DCR__TS_CTL_A__RX_RESET_M_position     15
#define SERDES_LEFT_DCR__TS_CTL_A__RX_RESET_M_get(x)       _BGQ_GET(4,15,x)
#define SERDES_LEFT_DCR__TS_CTL_A__RX_RESET_M_set(v)       _BGQ_SET(4,15,v)
#define SERDES_LEFT_DCR__TS_CTL_A__RX_RESET_M_insert(x,v)  _BGQ_INSERT(4,15,x,v)

//   Field:       rx_reset_p
//   Description: Resets the RX logic in the PCS layer (PRBS checker). This field should not be set to zero until rx lane clocks are active.
#define SERDES_LEFT_DCR__TS_CTL_A__RX_RESET_P_width        4
#define SERDES_LEFT_DCR__TS_CTL_A__RX_RESET_P_position     19
#define SERDES_LEFT_DCR__TS_CTL_A__RX_RESET_P_get(x)       _BGQ_GET(4,19,x)
#define SERDES_LEFT_DCR__TS_CTL_A__RX_RESET_P_set(v)       _BGQ_SET(4,19,v)
#define SERDES_LEFT_DCR__TS_CTL_A__RX_RESET_P_insert(x,v)  _BGQ_INSERT(4,19,x,v)

//   Field:       rx_elastic_buf_run_m
//   Description: Enables elastic buffer. This signal is muxed with the automatic state machine. Set the rx_elastic_buf_run_ovr_m bit to enable this field.
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_M_position     20
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_M_get(x)       _BGQ_GET(1,20,x)
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_M_set(v)       _BGQ_SET(1,20,v)
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_M_insert(x,v)  _BGQ_INSERT(1,20,x,v)

//   Field:       rx_elastic_buf_run_ovr_m
//   Description: Allows SW control of sync buffer. When this bit is set, the rx_elastic_buf_run_m field controls the sync buffer. When reset, the rcv state machine controls the sync buffer. 
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_OVR_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_OVR_M_position     21
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_OVR_M_get(x)       _BGQ_GET(1,21,x)
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_OVR_M_set(v)       _BGQ_SET(1,21,v)
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_OVR_M_insert(x,v)  _BGQ_INSERT(1,21,x,v)

//   Field:       rx_elastic_buf_lat_m
//   Description: Latency of elastic buffer. Sets the capture point of data in the sync buffer. Fast is lower latency than Slow, and is the preferred setting. Only values of Slow and Fast are allowed. All other settings will produce undefined results.
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_LAT_M_width        3
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_LAT_M_position     24
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_LAT_M_get(x)       _BGQ_GET(3,24,x)
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_LAT_M_set(v)       _BGQ_SET(3,24,v)
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_LAT_M_insert(x,v)  _BGQ_INSERT(3,24,x,v)

//   Field:       rx_elastic_buf_run_p
//   Description: Enables elastic buffer. This signal is muxed with the automatic state machine. Set the rx_elastic_buf_run_ovr_p bit to enable this field.
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_P_position     25
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_P_get(x)       _BGQ_GET(1,25,x)
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_P_set(v)       _BGQ_SET(1,25,v)
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_P_insert(x,v)  _BGQ_INSERT(1,25,x,v)

//   Field:       rx_elastic_buf_run_ovr_p
//   Description: Allows SW control of sync buffer. When this bit is set, the rx_elastic_buf_run_p field controls the sync buffer. When reset, the rcv state machine controls the sync buffer. 
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_OVR_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_OVR_P_position     26
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_OVR_P_get(x)       _BGQ_GET(1,26,x)
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_OVR_P_set(v)       _BGQ_SET(1,26,v)
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_OVR_P_insert(x,v)  _BGQ_INSERT(1,26,x,v)

//   Field:       rx_elastic_buf_lat_p
//   Description: Latency of elastic buffer. Sets the capture point of data in the sync buffer. Fast is lower latency than Slow, and is the preferred setting. Only values of Slow and Fast are allowed. All other settings will produce undefined results.
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_LAT_P_width        3
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_LAT_P_position     29
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_LAT_P_get(x)       _BGQ_GET(3,29,x)
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_LAT_P_set(v)       _BGQ_SET(3,29,v)
#define SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_LAT_P_insert(x,v)  _BGQ_INSERT(3,29,x,v)

//   Field:       tx_elastic_buf_run_m
//   Description: Enables the elastic buffer. When 0, the elastic buffer is disabled.
#define SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_RUN_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_RUN_M_position     30
#define SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_RUN_M_get(x)       _BGQ_GET(1,30,x)
#define SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_RUN_M_set(v)       _BGQ_SET(1,30,v)
#define SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_RUN_M_insert(x,v)  _BGQ_INSERT(1,30,x,v)

//   Field:       tx_elastic_buf_lat_m
//   Description: Latency of elastic buffer. Sets the capture point of data in the sync buffer. Fast is lower latency than Slow, and is the preferred setting. Only values of Slow and Fast are allowed. All other settings will produce undefined results.
#define SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_LAT_M_width        3
#define SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_LAT_M_position     34
#define SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_LAT_M_get(x)       _BGQ_GET(3,34,x)
#define SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_LAT_M_set(v)       _BGQ_SET(3,34,v)
#define SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_LAT_M_insert(x,v)  _BGQ_INSERT(3,34,x,v)

//   Field:       tx_elastic_buf_run_p
//   Description: Enables the elastic buffer. When 0, the elastic buffer is disabled.
#define SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_RUN_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_RUN_P_position     35
#define SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_RUN_P_get(x)       _BGQ_GET(1,35,x)
#define SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_RUN_P_set(v)       _BGQ_SET(1,35,v)
#define SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_RUN_P_insert(x,v)  _BGQ_INSERT(1,35,x,v)

//   Field:       tx_elastic_buf_lat_p
//   Description: Latency of elastic buffer. Sets the capture point of data in the sync buffer. Fast is lower latency than Slow, and is the preferred setting. Only values of Slow and Fast are allowed. All other settings will produce undefined results.
#define SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_LAT_P_width        3
#define SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_LAT_P_position     39
#define SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_LAT_P_get(x)       _BGQ_GET(3,39,x)
#define SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_LAT_P_set(v)       _BGQ_SET(3,39,v)
#define SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_LAT_P_insert(x,v)  _BGQ_INSERT(3,39,x,v)

//   Field:       xmit_scramble_m
//   Description: When set, send data is scrambled
#define SERDES_LEFT_DCR__TS_CTL_A__XMIT_SCRAMBLE_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_A__XMIT_SCRAMBLE_M_position     40
#define SERDES_LEFT_DCR__TS_CTL_A__XMIT_SCRAMBLE_M_get(x)       _BGQ_GET(1,40,x)
#define SERDES_LEFT_DCR__TS_CTL_A__XMIT_SCRAMBLE_M_set(v)       _BGQ_SET(1,40,v)
#define SERDES_LEFT_DCR__TS_CTL_A__XMIT_SCRAMBLE_M_insert(x,v)  _BGQ_INSERT(1,40,x,v)

//   Field:       xmit_scramble_p
//   Description: When set, send data is scrambled
#define SERDES_LEFT_DCR__TS_CTL_A__XMIT_SCRAMBLE_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_A__XMIT_SCRAMBLE_P_position     41
#define SERDES_LEFT_DCR__TS_CTL_A__XMIT_SCRAMBLE_P_get(x)       _BGQ_GET(1,41,x)
#define SERDES_LEFT_DCR__TS_CTL_A__XMIT_SCRAMBLE_P_set(v)       _BGQ_SET(1,41,v)
#define SERDES_LEFT_DCR__TS_CTL_A__XMIT_SCRAMBLE_P_insert(x,v)  _BGQ_INSERT(1,41,x,v)

//   Field:       rcv_descramble_m
//   Description: When set, indicates that incoming data is scrambled. This bit should be set at the same time as bit o47 or earlier.
#define SERDES_LEFT_DCR__TS_CTL_A__RCV_DESCRAMBLE_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_A__RCV_DESCRAMBLE_M_position     42
#define SERDES_LEFT_DCR__TS_CTL_A__RCV_DESCRAMBLE_M_get(x)       _BGQ_GET(1,42,x)
#define SERDES_LEFT_DCR__TS_CTL_A__RCV_DESCRAMBLE_M_set(v)       _BGQ_SET(1,42,v)
#define SERDES_LEFT_DCR__TS_CTL_A__RCV_DESCRAMBLE_M_insert(x,v)  _BGQ_INSERT(1,42,x,v)

//   Field:       rcv_descramble_p
//   Description: When set, indicates that incoming data is scrambled. This bit should be set at the same time as bit 48 or earlier.
#define SERDES_LEFT_DCR__TS_CTL_A__RCV_DESCRAMBLE_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_A__RCV_DESCRAMBLE_P_position     43
#define SERDES_LEFT_DCR__TS_CTL_A__RCV_DESCRAMBLE_P_get(x)       _BGQ_GET(1,43,x)
#define SERDES_LEFT_DCR__TS_CTL_A__RCV_DESCRAMBLE_P_set(v)       _BGQ_SET(1,43,v)
#define SERDES_LEFT_DCR__TS_CTL_A__RCV_DESCRAMBLE_P_insert(x,v)  _BGQ_INSERT(1,43,x,v)

//   Field:       send_train_m
//   Description: Send byte/word alignment training pattern.
#define SERDES_LEFT_DCR__TS_CTL_A__SEND_TRAIN_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_A__SEND_TRAIN_M_position     45
#define SERDES_LEFT_DCR__TS_CTL_A__SEND_TRAIN_M_get(x)       _BGQ_GET(1,45,x)
#define SERDES_LEFT_DCR__TS_CTL_A__SEND_TRAIN_M_set(v)       _BGQ_SET(1,45,v)
#define SERDES_LEFT_DCR__TS_CTL_A__SEND_TRAIN_M_insert(x,v)  _BGQ_INSERT(1,45,x,v)

//   Field:       send_train_p
//   Description: Send byte/word alignment training pattern.
#define SERDES_LEFT_DCR__TS_CTL_A__SEND_TRAIN_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_A__SEND_TRAIN_P_position     46
#define SERDES_LEFT_DCR__TS_CTL_A__SEND_TRAIN_P_get(x)       _BGQ_GET(1,46,x)
#define SERDES_LEFT_DCR__TS_CTL_A__SEND_TRAIN_P_set(v)       _BGQ_SET(1,46,v)
#define SERDES_LEFT_DCR__TS_CTL_A__SEND_TRAIN_P_insert(x,v)  _BGQ_INSERT(1,46,x,v)

//   Field:       rcv_train_m
//   Description: Start byte/word alignment process (rising edge sensitive). This starts off the automatic state machine to byte and word align the incoming data.
#define SERDES_LEFT_DCR__TS_CTL_A__RCV_TRAIN_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_A__RCV_TRAIN_M_position     47
#define SERDES_LEFT_DCR__TS_CTL_A__RCV_TRAIN_M_get(x)       _BGQ_GET(1,47,x)
#define SERDES_LEFT_DCR__TS_CTL_A__RCV_TRAIN_M_set(v)       _BGQ_SET(1,47,v)
#define SERDES_LEFT_DCR__TS_CTL_A__RCV_TRAIN_M_insert(x,v)  _BGQ_INSERT(1,47,x,v)

//   Field:       rcv_train_p
//   Description: Start byte/word alignment process (rising edge sensitive). This starts off the automatic state machine to byte and word align the incoming data.
#define SERDES_LEFT_DCR__TS_CTL_A__RCV_TRAIN_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_A__RCV_TRAIN_P_position     48
#define SERDES_LEFT_DCR__TS_CTL_A__RCV_TRAIN_P_get(x)       _BGQ_GET(1,48,x)
#define SERDES_LEFT_DCR__TS_CTL_A__RCV_TRAIN_P_set(v)       _BGQ_SET(1,48,v)
#define SERDES_LEFT_DCR__TS_CTL_A__RCV_TRAIN_P_insert(x,v)  _BGQ_INSERT(1,48,x,v)

//   Field:       loopback_en
//   Description: Loops xmit plus data to receive minus data, and vice versa
#define SERDES_LEFT_DCR__TS_CTL_A__LOOPBACK_EN_width        1
#define SERDES_LEFT_DCR__TS_CTL_A__LOOPBACK_EN_position     49
#define SERDES_LEFT_DCR__TS_CTL_A__LOOPBACK_EN_get(x)       _BGQ_GET(1,49,x)
#define SERDES_LEFT_DCR__TS_CTL_A__LOOPBACK_EN_set(v)       _BGQ_SET(1,49,v)
#define SERDES_LEFT_DCR__TS_CTL_A__LOOPBACK_EN_insert(x,v)  _BGQ_INSERT(1,49,x,v)

//   Field:       dataloop_en_m
//   Description: pma serial loopback control
#define SERDES_LEFT_DCR__TS_CTL_A__DATALOOP_EN_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_A__DATALOOP_EN_M_position     50
#define SERDES_LEFT_DCR__TS_CTL_A__DATALOOP_EN_M_get(x)       _BGQ_GET(1,50,x)
#define SERDES_LEFT_DCR__TS_CTL_A__DATALOOP_EN_M_set(v)       _BGQ_SET(1,50,v)
#define SERDES_LEFT_DCR__TS_CTL_A__DATALOOP_EN_M_insert(x,v)  _BGQ_INSERT(1,50,x,v)

//   Field:       dataloop_en_p
//   Description: pma serial loopback control
#define SERDES_LEFT_DCR__TS_CTL_A__DATALOOP_EN_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_A__DATALOOP_EN_P_position     51
#define SERDES_LEFT_DCR__TS_CTL_A__DATALOOP_EN_P_get(x)       _BGQ_GET(1,51,x)
#define SERDES_LEFT_DCR__TS_CTL_A__DATALOOP_EN_P_set(v)       _BGQ_SET(1,51,v)
#define SERDES_LEFT_DCR__TS_CTL_A__DATALOOP_EN_P_insert(x,v)  _BGQ_INSERT(1,51,x,v)

//   Field:       parlpbk_en_m
//   Description: pcs parallel loopback control
#define SERDES_LEFT_DCR__TS_CTL_A__PARLPBK_EN_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_A__PARLPBK_EN_M_position     52
#define SERDES_LEFT_DCR__TS_CTL_A__PARLPBK_EN_M_get(x)       _BGQ_GET(1,52,x)
#define SERDES_LEFT_DCR__TS_CTL_A__PARLPBK_EN_M_set(v)       _BGQ_SET(1,52,v)
#define SERDES_LEFT_DCR__TS_CTL_A__PARLPBK_EN_M_insert(x,v)  _BGQ_INSERT(1,52,x,v)

//   Field:       parlpbk_en_p
//   Description: pcs parallel loopback control
#define SERDES_LEFT_DCR__TS_CTL_A__PARLPBK_EN_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_A__PARLPBK_EN_P_position     53
#define SERDES_LEFT_DCR__TS_CTL_A__PARLPBK_EN_P_get(x)       _BGQ_GET(1,53,x)
#define SERDES_LEFT_DCR__TS_CTL_A__PARLPBK_EN_P_set(v)       _BGQ_SET(1,53,v)
#define SERDES_LEFT_DCR__TS_CTL_A__PARLPBK_EN_P_insert(x,v)  _BGQ_INSERT(1,53,x,v)

//   Field:       refclk_disable
//   Description: disables the pll reference clock
#define SERDES_LEFT_DCR__TS_CTL_A__REFCLK_DISABLE_width        1
#define SERDES_LEFT_DCR__TS_CTL_A__REFCLK_DISABLE_position     54
#define SERDES_LEFT_DCR__TS_CTL_A__REFCLK_DISABLE_get(x)       _BGQ_GET(1,54,x)
#define SERDES_LEFT_DCR__TS_CTL_A__REFCLK_DISABLE_set(v)       _BGQ_SET(1,54,v)
#define SERDES_LEFT_DCR__TS_CTL_A__REFCLK_DISABLE_insert(x,v)  _BGQ_INSERT(1,54,x,v)

//   Field:       lane_reset_m
//   Description: resets each lane in minus direction
#define SERDES_LEFT_DCR__TS_CTL_A__LANE_RESET_M_width        4
#define SERDES_LEFT_DCR__TS_CTL_A__LANE_RESET_M_position     58
#define SERDES_LEFT_DCR__TS_CTL_A__LANE_RESET_M_get(x)       _BGQ_GET(4,58,x)
#define SERDES_LEFT_DCR__TS_CTL_A__LANE_RESET_M_set(v)       _BGQ_SET(4,58,v)
#define SERDES_LEFT_DCR__TS_CTL_A__LANE_RESET_M_insert(x,v)  _BGQ_INSERT(4,58,x,v)

//   Field:       lane_reset_p
//   Description: resets each lane in plus direction
#define SERDES_LEFT_DCR__TS_CTL_A__LANE_RESET_P_width        4
#define SERDES_LEFT_DCR__TS_CTL_A__LANE_RESET_P_position     62
#define SERDES_LEFT_DCR__TS_CTL_A__LANE_RESET_P_get(x)       _BGQ_GET(4,62,x)
#define SERDES_LEFT_DCR__TS_CTL_A__LANE_RESET_P_set(v)       _BGQ_SET(4,62,v)
#define SERDES_LEFT_DCR__TS_CTL_A__LANE_RESET_P_insert(x,v)  _BGQ_INSERT(4,62,x,v)

//   Field:       hss_arefen
//   Description: Drives the HSSAREFEN pin on pma
#define SERDES_LEFT_DCR__TS_CTL_A__HSS_AREFEN_width        1
#define SERDES_LEFT_DCR__TS_CTL_A__HSS_AREFEN_position     63
#define SERDES_LEFT_DCR__TS_CTL_A__HSS_AREFEN_get(x)       _BGQ_GET(1,63,x)
#define SERDES_LEFT_DCR__TS_CTL_A__HSS_AREFEN_set(v)       _BGQ_SET(1,63,v)
#define SERDES_LEFT_DCR__TS_CTL_A__HSS_AREFEN_insert(x,v)  _BGQ_INSERT(1,63,x,v)

  
// Register:
//   Name:        ts_ctl_b
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged and TestInt
//   Reset:       002FF420000001FE
//   Description: Torus dimension B control register
#define SERDES_LEFT_DCR__TS_CTL_B_offset  (0x0103) // Offset of register in instance 
#define SERDES_LEFT_DCR__TS_CTL_B_reset  UNSIGNED64(0x002FF420000001FE)

//   Field:       spare
//   Description: Spare control signals, no function assigned
#define SERDES_LEFT_DCR__TS_CTL_B__SPARE_width        4
#define SERDES_LEFT_DCR__TS_CTL_B__SPARE_position     9
#define SERDES_LEFT_DCR__TS_CTL_B__SPARE_get(x)       _BGQ_GET(4,9,x)
#define SERDES_LEFT_DCR__TS_CTL_B__SPARE_set(v)       _BGQ_SET(4,9,v)
#define SERDES_LEFT_DCR__TS_CTL_B__SPARE_insert(x,v)  _BGQ_INSERT(4,9,x,v)

//   Field:       pcs_reset
//   Description: Drives the HSSPCSRESET signal on PCS Core
#define SERDES_LEFT_DCR__TS_CTL_B__PCS_RESET_width        1
#define SERDES_LEFT_DCR__TS_CTL_B__PCS_RESET_position     10
#define SERDES_LEFT_DCR__TS_CTL_B__PCS_RESET_get(x)       _BGQ_GET(1,10,x)
#define SERDES_LEFT_DCR__TS_CTL_B__PCS_RESET_set(v)       _BGQ_SET(1,10,v)
#define SERDES_LEFT_DCR__TS_CTL_B__PCS_RESET_insert(x,v)  _BGQ_INSERT(1,10,x,v)

//   Field:       pipe_resetn
//   Description: Drives the PIPE0RESETN signal on PCS Core. Note that the PIPE0RESETN signal is active low.
#define SERDES_LEFT_DCR__TS_CTL_B__PIPE_RESETN_width        1
#define SERDES_LEFT_DCR__TS_CTL_B__PIPE_RESETN_position     11
#define SERDES_LEFT_DCR__TS_CTL_B__PIPE_RESETN_get(x)       _BGQ_GET(1,11,x)
#define SERDES_LEFT_DCR__TS_CTL_B__PIPE_RESETN_set(v)       _BGQ_SET(1,11,v)
#define SERDES_LEFT_DCR__TS_CTL_B__PIPE_RESETN_insert(x,v)  _BGQ_INSERT(1,11,x,v)

//   Field:       rx_reset_m
//   Description: Resets the RX logic in the PCS layer (PRBS checker). This field should not be set to zero until rx lane clocks are active.
#define SERDES_LEFT_DCR__TS_CTL_B__RX_RESET_M_width        4
#define SERDES_LEFT_DCR__TS_CTL_B__RX_RESET_M_position     15
#define SERDES_LEFT_DCR__TS_CTL_B__RX_RESET_M_get(x)       _BGQ_GET(4,15,x)
#define SERDES_LEFT_DCR__TS_CTL_B__RX_RESET_M_set(v)       _BGQ_SET(4,15,v)
#define SERDES_LEFT_DCR__TS_CTL_B__RX_RESET_M_insert(x,v)  _BGQ_INSERT(4,15,x,v)

//   Field:       rx_reset_p
//   Description: Resets the RX logic in the PCS layer (PRBS checker). This field should not be set to zero until rx lane clocks are active.
#define SERDES_LEFT_DCR__TS_CTL_B__RX_RESET_P_width        4
#define SERDES_LEFT_DCR__TS_CTL_B__RX_RESET_P_position     19
#define SERDES_LEFT_DCR__TS_CTL_B__RX_RESET_P_get(x)       _BGQ_GET(4,19,x)
#define SERDES_LEFT_DCR__TS_CTL_B__RX_RESET_P_set(v)       _BGQ_SET(4,19,v)
#define SERDES_LEFT_DCR__TS_CTL_B__RX_RESET_P_insert(x,v)  _BGQ_INSERT(4,19,x,v)

//   Field:       rx_elastic_buf_run_m
//   Description: Enables elastic buffer. This signal is muxed with the automatic state machine. Set the rx_elastic_buf_run_ovr_m bit to enable this field.
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_RUN_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_RUN_M_position     20
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_RUN_M_get(x)       _BGQ_GET(1,20,x)
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_RUN_M_set(v)       _BGQ_SET(1,20,v)
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_RUN_M_insert(x,v)  _BGQ_INSERT(1,20,x,v)

//   Field:       rx_elastic_buf_run_ovr_m
//   Description: Allows SW control of sync buffer. When this bit is set, the rx_elastic_buf_run_m field controls the sync buffer. When reset, the rcv state machine controls the sync buffer. 
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_RUN_OVR_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_RUN_OVR_M_position     21
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_RUN_OVR_M_get(x)       _BGQ_GET(1,21,x)
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_RUN_OVR_M_set(v)       _BGQ_SET(1,21,v)
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_RUN_OVR_M_insert(x,v)  _BGQ_INSERT(1,21,x,v)

//   Field:       rx_elastic_buf_lat_m
//   Description: Latency of elastic buffer. Sets the capture point of data in the sync buffer. Fast is lower latency than Slow, and is the preferred setting. Only values of Slow and Fast are allowed. All other settings will produce undefined results.
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_LAT_M_width        3
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_LAT_M_position     24
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_LAT_M_get(x)       _BGQ_GET(3,24,x)
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_LAT_M_set(v)       _BGQ_SET(3,24,v)
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_LAT_M_insert(x,v)  _BGQ_INSERT(3,24,x,v)

//   Field:       rx_elastic_buf_run_p
//   Description: Enables elastic buffer. This signal is muxed with the automatic state machine. Set the rx_elastic_buf_run_ovr_p bit to enable this field.
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_RUN_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_RUN_P_position     25
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_RUN_P_get(x)       _BGQ_GET(1,25,x)
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_RUN_P_set(v)       _BGQ_SET(1,25,v)
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_RUN_P_insert(x,v)  _BGQ_INSERT(1,25,x,v)

//   Field:       rx_elastic_buf_run_ovr_p
//   Description: Allows SW control of sync buffer. When this bit is set, the rx_elastic_buf_run_p field controls the sync buffer. When reset, the rcv state machine controls the sync buffer. 
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_RUN_OVR_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_RUN_OVR_P_position     26
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_RUN_OVR_P_get(x)       _BGQ_GET(1,26,x)
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_RUN_OVR_P_set(v)       _BGQ_SET(1,26,v)
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_RUN_OVR_P_insert(x,v)  _BGQ_INSERT(1,26,x,v)

//   Field:       rx_elastic_buf_lat_p
//   Description: Latency of elastic buffer. Sets the capture point of data in the sync buffer. Fast is lower latency than Slow, and is the preferred setting. Only values of Slow and Fast are allowed. All other settings will produce undefined results.
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_LAT_P_width        3
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_LAT_P_position     29
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_LAT_P_get(x)       _BGQ_GET(3,29,x)
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_LAT_P_set(v)       _BGQ_SET(3,29,v)
#define SERDES_LEFT_DCR__TS_CTL_B__RX_ELASTIC_BUF_LAT_P_insert(x,v)  _BGQ_INSERT(3,29,x,v)

//   Field:       tx_elastic_buf_run_m
//   Description: Enables the elastic buffer. When 0, the elastic buffer is disabled.
#define SERDES_LEFT_DCR__TS_CTL_B__TX_ELASTIC_BUF_RUN_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_B__TX_ELASTIC_BUF_RUN_M_position     30
#define SERDES_LEFT_DCR__TS_CTL_B__TX_ELASTIC_BUF_RUN_M_get(x)       _BGQ_GET(1,30,x)
#define SERDES_LEFT_DCR__TS_CTL_B__TX_ELASTIC_BUF_RUN_M_set(v)       _BGQ_SET(1,30,v)
#define SERDES_LEFT_DCR__TS_CTL_B__TX_ELASTIC_BUF_RUN_M_insert(x,v)  _BGQ_INSERT(1,30,x,v)

//   Field:       tx_elastic_buf_lat_m
//   Description: Latency of elastic buffer. Sets the capture point of data in the sync buffer. Fast is lower latency than Slow, and is the preferred setting. Only values of Slow and Fast are allowed. All other settings will produce undefined results.
#define SERDES_LEFT_DCR__TS_CTL_B__TX_ELASTIC_BUF_LAT_M_width        3
#define SERDES_LEFT_DCR__TS_CTL_B__TX_ELASTIC_BUF_LAT_M_position     34
#define SERDES_LEFT_DCR__TS_CTL_B__TX_ELASTIC_BUF_LAT_M_get(x)       _BGQ_GET(3,34,x)
#define SERDES_LEFT_DCR__TS_CTL_B__TX_ELASTIC_BUF_LAT_M_set(v)       _BGQ_SET(3,34,v)
#define SERDES_LEFT_DCR__TS_CTL_B__TX_ELASTIC_BUF_LAT_M_insert(x,v)  _BGQ_INSERT(3,34,x,v)

//   Field:       tx_elastic_buf_run_p
//   Description: Enables the elastic buffer. When 0, the elastic buffer is disabled.
#define SERDES_LEFT_DCR__TS_CTL_B__TX_ELASTIC_BUF_RUN_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_B__TX_ELASTIC_BUF_RUN_P_position     35
#define SERDES_LEFT_DCR__TS_CTL_B__TX_ELASTIC_BUF_RUN_P_get(x)       _BGQ_GET(1,35,x)
#define SERDES_LEFT_DCR__TS_CTL_B__TX_ELASTIC_BUF_RUN_P_set(v)       _BGQ_SET(1,35,v)
#define SERDES_LEFT_DCR__TS_CTL_B__TX_ELASTIC_BUF_RUN_P_insert(x,v)  _BGQ_INSERT(1,35,x,v)

//   Field:       tx_elastic_buf_lat_p
//   Description: Latency of elastic buffer. Sets the capture point of data in the sync buffer. Fast is lower latency than Slow, and is the preferred setting. Only values of Slow and Fast are allowed. All other settings will produce undefined results.
#define SERDES_LEFT_DCR__TS_CTL_B__TX_ELASTIC_BUF_LAT_P_width        3
#define SERDES_LEFT_DCR__TS_CTL_B__TX_ELASTIC_BUF_LAT_P_position     39
#define SERDES_LEFT_DCR__TS_CTL_B__TX_ELASTIC_BUF_LAT_P_get(x)       _BGQ_GET(3,39,x)
#define SERDES_LEFT_DCR__TS_CTL_B__TX_ELASTIC_BUF_LAT_P_set(v)       _BGQ_SET(3,39,v)
#define SERDES_LEFT_DCR__TS_CTL_B__TX_ELASTIC_BUF_LAT_P_insert(x,v)  _BGQ_INSERT(3,39,x,v)

//   Field:       xmit_scramble_m
//   Description: When set, send data is scrambled
#define SERDES_LEFT_DCR__TS_CTL_B__XMIT_SCRAMBLE_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_B__XMIT_SCRAMBLE_M_position     40
#define SERDES_LEFT_DCR__TS_CTL_B__XMIT_SCRAMBLE_M_get(x)       _BGQ_GET(1,40,x)
#define SERDES_LEFT_DCR__TS_CTL_B__XMIT_SCRAMBLE_M_set(v)       _BGQ_SET(1,40,v)
#define SERDES_LEFT_DCR__TS_CTL_B__XMIT_SCRAMBLE_M_insert(x,v)  _BGQ_INSERT(1,40,x,v)

//   Field:       xmit_scramble_p
//   Description: When set, send data is scrambled
#define SERDES_LEFT_DCR__TS_CTL_B__XMIT_SCRAMBLE_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_B__XMIT_SCRAMBLE_P_position     41
#define SERDES_LEFT_DCR__TS_CTL_B__XMIT_SCRAMBLE_P_get(x)       _BGQ_GET(1,41,x)
#define SERDES_LEFT_DCR__TS_CTL_B__XMIT_SCRAMBLE_P_set(v)       _BGQ_SET(1,41,v)
#define SERDES_LEFT_DCR__TS_CTL_B__XMIT_SCRAMBLE_P_insert(x,v)  _BGQ_INSERT(1,41,x,v)

//   Field:       rcv_descramble_m
//   Description: When set, indicates that incoming data is scrambled. This bit should be set at the same time as bit o47 or earlier.
#define SERDES_LEFT_DCR__TS_CTL_B__RCV_DESCRAMBLE_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_B__RCV_DESCRAMBLE_M_position     42
#define SERDES_LEFT_DCR__TS_CTL_B__RCV_DESCRAMBLE_M_get(x)       _BGQ_GET(1,42,x)
#define SERDES_LEFT_DCR__TS_CTL_B__RCV_DESCRAMBLE_M_set(v)       _BGQ_SET(1,42,v)
#define SERDES_LEFT_DCR__TS_CTL_B__RCV_DESCRAMBLE_M_insert(x,v)  _BGQ_INSERT(1,42,x,v)

//   Field:       rcv_descramble_p
//   Description: When set, indicates that incoming data is scrambled. This bit should be set at the same time as bit 48 or earlier.
#define SERDES_LEFT_DCR__TS_CTL_B__RCV_DESCRAMBLE_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_B__RCV_DESCRAMBLE_P_position     43
#define SERDES_LEFT_DCR__TS_CTL_B__RCV_DESCRAMBLE_P_get(x)       _BGQ_GET(1,43,x)
#define SERDES_LEFT_DCR__TS_CTL_B__RCV_DESCRAMBLE_P_set(v)       _BGQ_SET(1,43,v)
#define SERDES_LEFT_DCR__TS_CTL_B__RCV_DESCRAMBLE_P_insert(x,v)  _BGQ_INSERT(1,43,x,v)

//   Field:       send_train_m
//   Description: Send byte/word alignment training pattern.
#define SERDES_LEFT_DCR__TS_CTL_B__SEND_TRAIN_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_B__SEND_TRAIN_M_position     45
#define SERDES_LEFT_DCR__TS_CTL_B__SEND_TRAIN_M_get(x)       _BGQ_GET(1,45,x)
#define SERDES_LEFT_DCR__TS_CTL_B__SEND_TRAIN_M_set(v)       _BGQ_SET(1,45,v)
#define SERDES_LEFT_DCR__TS_CTL_B__SEND_TRAIN_M_insert(x,v)  _BGQ_INSERT(1,45,x,v)

//   Field:       send_train_p
//   Description: Send byte/word alignment training pattern.
#define SERDES_LEFT_DCR__TS_CTL_B__SEND_TRAIN_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_B__SEND_TRAIN_P_position     46
#define SERDES_LEFT_DCR__TS_CTL_B__SEND_TRAIN_P_get(x)       _BGQ_GET(1,46,x)
#define SERDES_LEFT_DCR__TS_CTL_B__SEND_TRAIN_P_set(v)       _BGQ_SET(1,46,v)
#define SERDES_LEFT_DCR__TS_CTL_B__SEND_TRAIN_P_insert(x,v)  _BGQ_INSERT(1,46,x,v)

//   Field:       rcv_train_m
//   Description: Start byte/word alignment process (rising edge sensitive). This starts off the automatic state machine to byte and word align the incoming data.
#define SERDES_LEFT_DCR__TS_CTL_B__RCV_TRAIN_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_B__RCV_TRAIN_M_position     47
#define SERDES_LEFT_DCR__TS_CTL_B__RCV_TRAIN_M_get(x)       _BGQ_GET(1,47,x)
#define SERDES_LEFT_DCR__TS_CTL_B__RCV_TRAIN_M_set(v)       _BGQ_SET(1,47,v)
#define SERDES_LEFT_DCR__TS_CTL_B__RCV_TRAIN_M_insert(x,v)  _BGQ_INSERT(1,47,x,v)

//   Field:       rcv_train_p
//   Description: Start byte/word alignment process (rising edge sensitive). This starts off the automatic state machine to byte and word align the incoming data.
#define SERDES_LEFT_DCR__TS_CTL_B__RCV_TRAIN_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_B__RCV_TRAIN_P_position     48
#define SERDES_LEFT_DCR__TS_CTL_B__RCV_TRAIN_P_get(x)       _BGQ_GET(1,48,x)
#define SERDES_LEFT_DCR__TS_CTL_B__RCV_TRAIN_P_set(v)       _BGQ_SET(1,48,v)
#define SERDES_LEFT_DCR__TS_CTL_B__RCV_TRAIN_P_insert(x,v)  _BGQ_INSERT(1,48,x,v)

//   Field:       loopback_en
//   Description: Loops xmit plus data to receive minus data, and vice versa
#define SERDES_LEFT_DCR__TS_CTL_B__LOOPBACK_EN_width        1
#define SERDES_LEFT_DCR__TS_CTL_B__LOOPBACK_EN_position     49
#define SERDES_LEFT_DCR__TS_CTL_B__LOOPBACK_EN_get(x)       _BGQ_GET(1,49,x)
#define SERDES_LEFT_DCR__TS_CTL_B__LOOPBACK_EN_set(v)       _BGQ_SET(1,49,v)
#define SERDES_LEFT_DCR__TS_CTL_B__LOOPBACK_EN_insert(x,v)  _BGQ_INSERT(1,49,x,v)

//   Field:       dataloop_en_m
//   Description: pma serial loopback control
#define SERDES_LEFT_DCR__TS_CTL_B__DATALOOP_EN_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_B__DATALOOP_EN_M_position     50
#define SERDES_LEFT_DCR__TS_CTL_B__DATALOOP_EN_M_get(x)       _BGQ_GET(1,50,x)
#define SERDES_LEFT_DCR__TS_CTL_B__DATALOOP_EN_M_set(v)       _BGQ_SET(1,50,v)
#define SERDES_LEFT_DCR__TS_CTL_B__DATALOOP_EN_M_insert(x,v)  _BGQ_INSERT(1,50,x,v)

//   Field:       dataloop_en_p
//   Description: pma serial loopback control
#define SERDES_LEFT_DCR__TS_CTL_B__DATALOOP_EN_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_B__DATALOOP_EN_P_position     51
#define SERDES_LEFT_DCR__TS_CTL_B__DATALOOP_EN_P_get(x)       _BGQ_GET(1,51,x)
#define SERDES_LEFT_DCR__TS_CTL_B__DATALOOP_EN_P_set(v)       _BGQ_SET(1,51,v)
#define SERDES_LEFT_DCR__TS_CTL_B__DATALOOP_EN_P_insert(x,v)  _BGQ_INSERT(1,51,x,v)

//   Field:       parlpbk_en_m
//   Description: pcs parallel loopback control
#define SERDES_LEFT_DCR__TS_CTL_B__PARLPBK_EN_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_B__PARLPBK_EN_M_position     52
#define SERDES_LEFT_DCR__TS_CTL_B__PARLPBK_EN_M_get(x)       _BGQ_GET(1,52,x)
#define SERDES_LEFT_DCR__TS_CTL_B__PARLPBK_EN_M_set(v)       _BGQ_SET(1,52,v)
#define SERDES_LEFT_DCR__TS_CTL_B__PARLPBK_EN_M_insert(x,v)  _BGQ_INSERT(1,52,x,v)

//   Field:       parlpbk_en_p
//   Description: pcs parallel loopback control
#define SERDES_LEFT_DCR__TS_CTL_B__PARLPBK_EN_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_B__PARLPBK_EN_P_position     53
#define SERDES_LEFT_DCR__TS_CTL_B__PARLPBK_EN_P_get(x)       _BGQ_GET(1,53,x)
#define SERDES_LEFT_DCR__TS_CTL_B__PARLPBK_EN_P_set(v)       _BGQ_SET(1,53,v)
#define SERDES_LEFT_DCR__TS_CTL_B__PARLPBK_EN_P_insert(x,v)  _BGQ_INSERT(1,53,x,v)

//   Field:       refclk_disable
//   Description: disables the pll reference clock
#define SERDES_LEFT_DCR__TS_CTL_B__REFCLK_DISABLE_width        1
#define SERDES_LEFT_DCR__TS_CTL_B__REFCLK_DISABLE_position     54
#define SERDES_LEFT_DCR__TS_CTL_B__REFCLK_DISABLE_get(x)       _BGQ_GET(1,54,x)
#define SERDES_LEFT_DCR__TS_CTL_B__REFCLK_DISABLE_set(v)       _BGQ_SET(1,54,v)
#define SERDES_LEFT_DCR__TS_CTL_B__REFCLK_DISABLE_insert(x,v)  _BGQ_INSERT(1,54,x,v)

//   Field:       lane_reset_m
//   Description: resets each lane in minus direction
#define SERDES_LEFT_DCR__TS_CTL_B__LANE_RESET_M_width        4
#define SERDES_LEFT_DCR__TS_CTL_B__LANE_RESET_M_position     58
#define SERDES_LEFT_DCR__TS_CTL_B__LANE_RESET_M_get(x)       _BGQ_GET(4,58,x)
#define SERDES_LEFT_DCR__TS_CTL_B__LANE_RESET_M_set(v)       _BGQ_SET(4,58,v)
#define SERDES_LEFT_DCR__TS_CTL_B__LANE_RESET_M_insert(x,v)  _BGQ_INSERT(4,58,x,v)

//   Field:       lane_reset_p
//   Description: resets each lane in plus direction
#define SERDES_LEFT_DCR__TS_CTL_B__LANE_RESET_P_width        4
#define SERDES_LEFT_DCR__TS_CTL_B__LANE_RESET_P_position     62
#define SERDES_LEFT_DCR__TS_CTL_B__LANE_RESET_P_get(x)       _BGQ_GET(4,62,x)
#define SERDES_LEFT_DCR__TS_CTL_B__LANE_RESET_P_set(v)       _BGQ_SET(4,62,v)
#define SERDES_LEFT_DCR__TS_CTL_B__LANE_RESET_P_insert(x,v)  _BGQ_INSERT(4,62,x,v)

//   Field:       hss_arefen
//   Description: Drives the HSSAREFEN pin on pma
#define SERDES_LEFT_DCR__TS_CTL_B__HSS_AREFEN_width        1
#define SERDES_LEFT_DCR__TS_CTL_B__HSS_AREFEN_position     63
#define SERDES_LEFT_DCR__TS_CTL_B__HSS_AREFEN_get(x)       _BGQ_GET(1,63,x)
#define SERDES_LEFT_DCR__TS_CTL_B__HSS_AREFEN_set(v)       _BGQ_SET(1,63,v)
#define SERDES_LEFT_DCR__TS_CTL_B__HSS_AREFEN_insert(x,v)  _BGQ_INSERT(1,63,x,v)

  
// Register:
//   Name:        ts_ctl_c
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged and TestInt
//   Reset:       002FF420000001FE
//   Description: Torus dimension C control register
#define SERDES_LEFT_DCR__TS_CTL_C_offset  (0x0104) // Offset of register in instance 
#define SERDES_LEFT_DCR__TS_CTL_C_reset  UNSIGNED64(0x002FF420000001FE)

//   Field:       spare
//   Description: Spare control signals, no function assigned
#define SERDES_LEFT_DCR__TS_CTL_C__SPARE_width        4
#define SERDES_LEFT_DCR__TS_CTL_C__SPARE_position     9
#define SERDES_LEFT_DCR__TS_CTL_C__SPARE_get(x)       _BGQ_GET(4,9,x)
#define SERDES_LEFT_DCR__TS_CTL_C__SPARE_set(v)       _BGQ_SET(4,9,v)
#define SERDES_LEFT_DCR__TS_CTL_C__SPARE_insert(x,v)  _BGQ_INSERT(4,9,x,v)

//   Field:       pcs_reset
//   Description: Drives the HSSPCSRESET signal on PCS Core
#define SERDES_LEFT_DCR__TS_CTL_C__PCS_RESET_width        1
#define SERDES_LEFT_DCR__TS_CTL_C__PCS_RESET_position     10
#define SERDES_LEFT_DCR__TS_CTL_C__PCS_RESET_get(x)       _BGQ_GET(1,10,x)
#define SERDES_LEFT_DCR__TS_CTL_C__PCS_RESET_set(v)       _BGQ_SET(1,10,v)
#define SERDES_LEFT_DCR__TS_CTL_C__PCS_RESET_insert(x,v)  _BGQ_INSERT(1,10,x,v)

//   Field:       pipe_resetn
//   Description: Drives the PIPE0RESETN signal on PCS Core. Note that the PIPE0RESETN signal is active low.
#define SERDES_LEFT_DCR__TS_CTL_C__PIPE_RESETN_width        1
#define SERDES_LEFT_DCR__TS_CTL_C__PIPE_RESETN_position     11
#define SERDES_LEFT_DCR__TS_CTL_C__PIPE_RESETN_get(x)       _BGQ_GET(1,11,x)
#define SERDES_LEFT_DCR__TS_CTL_C__PIPE_RESETN_set(v)       _BGQ_SET(1,11,v)
#define SERDES_LEFT_DCR__TS_CTL_C__PIPE_RESETN_insert(x,v)  _BGQ_INSERT(1,11,x,v)

//   Field:       rx_reset_m
//   Description: Resets the RX logic in the PCS layer (PRBS checker). This field should not be set to zero until rx lane clocks are active.
#define SERDES_LEFT_DCR__TS_CTL_C__RX_RESET_M_width        4
#define SERDES_LEFT_DCR__TS_CTL_C__RX_RESET_M_position     15
#define SERDES_LEFT_DCR__TS_CTL_C__RX_RESET_M_get(x)       _BGQ_GET(4,15,x)
#define SERDES_LEFT_DCR__TS_CTL_C__RX_RESET_M_set(v)       _BGQ_SET(4,15,v)
#define SERDES_LEFT_DCR__TS_CTL_C__RX_RESET_M_insert(x,v)  _BGQ_INSERT(4,15,x,v)

//   Field:       rx_reset_p
//   Description: Resets the RX logic in the PCS layer (PRBS checker). This field should not be set to zero until rx lane clocks are active.
#define SERDES_LEFT_DCR__TS_CTL_C__RX_RESET_P_width        4
#define SERDES_LEFT_DCR__TS_CTL_C__RX_RESET_P_position     19
#define SERDES_LEFT_DCR__TS_CTL_C__RX_RESET_P_get(x)       _BGQ_GET(4,19,x)
#define SERDES_LEFT_DCR__TS_CTL_C__RX_RESET_P_set(v)       _BGQ_SET(4,19,v)
#define SERDES_LEFT_DCR__TS_CTL_C__RX_RESET_P_insert(x,v)  _BGQ_INSERT(4,19,x,v)

//   Field:       rx_elastic_buf_run_m
//   Description: Enables elastic buffer. This signal is muxed with the automatic state machine. Set the rx_elastic_buf_run_ovr_m bit to enable this field.
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_RUN_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_RUN_M_position     20
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_RUN_M_get(x)       _BGQ_GET(1,20,x)
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_RUN_M_set(v)       _BGQ_SET(1,20,v)
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_RUN_M_insert(x,v)  _BGQ_INSERT(1,20,x,v)

//   Field:       rx_elastic_buf_run_ovr_m
//   Description: Allows SW control of sync buffer. When this bit is set, the rx_elastic_buf_run_m field controls the sync buffer. When reset, the rcv state machine controls the sync buffer. 
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_RUN_OVR_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_RUN_OVR_M_position     21
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_RUN_OVR_M_get(x)       _BGQ_GET(1,21,x)
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_RUN_OVR_M_set(v)       _BGQ_SET(1,21,v)
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_RUN_OVR_M_insert(x,v)  _BGQ_INSERT(1,21,x,v)

//   Field:       rx_elastic_buf_lat_m
//   Description: Latency of elastic buffer. Sets the capture point of data in the sync buffer. Fast is lower latency than Slow, and is the preferred setting. Only values of Slow and Fast are allowed. All other settings will produce undefined results.
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_LAT_M_width        3
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_LAT_M_position     24
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_LAT_M_get(x)       _BGQ_GET(3,24,x)
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_LAT_M_set(v)       _BGQ_SET(3,24,v)
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_LAT_M_insert(x,v)  _BGQ_INSERT(3,24,x,v)

//   Field:       rx_elastic_buf_run_p
//   Description: Enables elastic buffer. This signal is muxed with the automatic state machine. Set the rx_elastic_buf_run_ovr_p bit to enable this field.
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_RUN_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_RUN_P_position     25
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_RUN_P_get(x)       _BGQ_GET(1,25,x)
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_RUN_P_set(v)       _BGQ_SET(1,25,v)
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_RUN_P_insert(x,v)  _BGQ_INSERT(1,25,x,v)

//   Field:       rx_elastic_buf_run_ovr_p
//   Description: Allows SW control of sync buffer. When this bit is set, the rx_elastic_buf_run_p field controls the sync buffer. When reset, the rcv state machine controls the sync buffer. 
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_RUN_OVR_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_RUN_OVR_P_position     26
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_RUN_OVR_P_get(x)       _BGQ_GET(1,26,x)
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_RUN_OVR_P_set(v)       _BGQ_SET(1,26,v)
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_RUN_OVR_P_insert(x,v)  _BGQ_INSERT(1,26,x,v)

//   Field:       rx_elastic_buf_lat_p
//   Description: Latency of elastic buffer. Sets the capture point of data in the sync buffer. Fast is lower latency than Slow, and is the preferred setting. Only values of Slow and Fast are allowed. All other settings will produce undefined results.
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_LAT_P_width        3
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_LAT_P_position     29
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_LAT_P_get(x)       _BGQ_GET(3,29,x)
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_LAT_P_set(v)       _BGQ_SET(3,29,v)
#define SERDES_LEFT_DCR__TS_CTL_C__RX_ELASTIC_BUF_LAT_P_insert(x,v)  _BGQ_INSERT(3,29,x,v)

//   Field:       tx_elastic_buf_run_m
//   Description: Enables the elastic buffer. When 0, the elastic buffer is disabled.
#define SERDES_LEFT_DCR__TS_CTL_C__TX_ELASTIC_BUF_RUN_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_C__TX_ELASTIC_BUF_RUN_M_position     30
#define SERDES_LEFT_DCR__TS_CTL_C__TX_ELASTIC_BUF_RUN_M_get(x)       _BGQ_GET(1,30,x)
#define SERDES_LEFT_DCR__TS_CTL_C__TX_ELASTIC_BUF_RUN_M_set(v)       _BGQ_SET(1,30,v)
#define SERDES_LEFT_DCR__TS_CTL_C__TX_ELASTIC_BUF_RUN_M_insert(x,v)  _BGQ_INSERT(1,30,x,v)

//   Field:       tx_elastic_buf_lat_m
//   Description: Latency of elastic buffer. Sets the capture point of data in the sync buffer. Fast is lower latency than Slow, and is the preferred setting. Only values of Slow and Fast are allowed. All other settings will produce undefined results.
#define SERDES_LEFT_DCR__TS_CTL_C__TX_ELASTIC_BUF_LAT_M_width        3
#define SERDES_LEFT_DCR__TS_CTL_C__TX_ELASTIC_BUF_LAT_M_position     34
#define SERDES_LEFT_DCR__TS_CTL_C__TX_ELASTIC_BUF_LAT_M_get(x)       _BGQ_GET(3,34,x)
#define SERDES_LEFT_DCR__TS_CTL_C__TX_ELASTIC_BUF_LAT_M_set(v)       _BGQ_SET(3,34,v)
#define SERDES_LEFT_DCR__TS_CTL_C__TX_ELASTIC_BUF_LAT_M_insert(x,v)  _BGQ_INSERT(3,34,x,v)

//   Field:       tx_elastic_buf_run_p
//   Description: Enables the elastic buffer. When 0, the elastic buffer is disabled.
#define SERDES_LEFT_DCR__TS_CTL_C__TX_ELASTIC_BUF_RUN_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_C__TX_ELASTIC_BUF_RUN_P_position     35
#define SERDES_LEFT_DCR__TS_CTL_C__TX_ELASTIC_BUF_RUN_P_get(x)       _BGQ_GET(1,35,x)
#define SERDES_LEFT_DCR__TS_CTL_C__TX_ELASTIC_BUF_RUN_P_set(v)       _BGQ_SET(1,35,v)
#define SERDES_LEFT_DCR__TS_CTL_C__TX_ELASTIC_BUF_RUN_P_insert(x,v)  _BGQ_INSERT(1,35,x,v)

//   Field:       tx_elastic_buf_lat_p
//   Description: Latency of elastic buffer. Sets the capture point of data in the sync buffer. Fast is lower latency than Slow, and is the preferred setting. Only values of Slow and Fast are allowed. All other settings will produce undefined results.
#define SERDES_LEFT_DCR__TS_CTL_C__TX_ELASTIC_BUF_LAT_P_width        3
#define SERDES_LEFT_DCR__TS_CTL_C__TX_ELASTIC_BUF_LAT_P_position     39
#define SERDES_LEFT_DCR__TS_CTL_C__TX_ELASTIC_BUF_LAT_P_get(x)       _BGQ_GET(3,39,x)
#define SERDES_LEFT_DCR__TS_CTL_C__TX_ELASTIC_BUF_LAT_P_set(v)       _BGQ_SET(3,39,v)
#define SERDES_LEFT_DCR__TS_CTL_C__TX_ELASTIC_BUF_LAT_P_insert(x,v)  _BGQ_INSERT(3,39,x,v)

//   Field:       xmit_scramble_m
//   Description: When set, send data is scrambled
#define SERDES_LEFT_DCR__TS_CTL_C__XMIT_SCRAMBLE_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_C__XMIT_SCRAMBLE_M_position     40
#define SERDES_LEFT_DCR__TS_CTL_C__XMIT_SCRAMBLE_M_get(x)       _BGQ_GET(1,40,x)
#define SERDES_LEFT_DCR__TS_CTL_C__XMIT_SCRAMBLE_M_set(v)       _BGQ_SET(1,40,v)
#define SERDES_LEFT_DCR__TS_CTL_C__XMIT_SCRAMBLE_M_insert(x,v)  _BGQ_INSERT(1,40,x,v)

//   Field:       xmit_scramble_p
//   Description: When set, send data is scrambled
#define SERDES_LEFT_DCR__TS_CTL_C__XMIT_SCRAMBLE_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_C__XMIT_SCRAMBLE_P_position     41
#define SERDES_LEFT_DCR__TS_CTL_C__XMIT_SCRAMBLE_P_get(x)       _BGQ_GET(1,41,x)
#define SERDES_LEFT_DCR__TS_CTL_C__XMIT_SCRAMBLE_P_set(v)       _BGQ_SET(1,41,v)
#define SERDES_LEFT_DCR__TS_CTL_C__XMIT_SCRAMBLE_P_insert(x,v)  _BGQ_INSERT(1,41,x,v)

//   Field:       rcv_descramble_m
//   Description: When set, indicates that incoming data is scrambled. This bit should be set at the same time as bit o47 or earlier.
#define SERDES_LEFT_DCR__TS_CTL_C__RCV_DESCRAMBLE_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_C__RCV_DESCRAMBLE_M_position     42
#define SERDES_LEFT_DCR__TS_CTL_C__RCV_DESCRAMBLE_M_get(x)       _BGQ_GET(1,42,x)
#define SERDES_LEFT_DCR__TS_CTL_C__RCV_DESCRAMBLE_M_set(v)       _BGQ_SET(1,42,v)
#define SERDES_LEFT_DCR__TS_CTL_C__RCV_DESCRAMBLE_M_insert(x,v)  _BGQ_INSERT(1,42,x,v)

//   Field:       rcv_descramble_p
//   Description: When set, indicates that incoming data is scrambled. This bit should be set at the same time as bit 48 or earlier.
#define SERDES_LEFT_DCR__TS_CTL_C__RCV_DESCRAMBLE_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_C__RCV_DESCRAMBLE_P_position     43
#define SERDES_LEFT_DCR__TS_CTL_C__RCV_DESCRAMBLE_P_get(x)       _BGQ_GET(1,43,x)
#define SERDES_LEFT_DCR__TS_CTL_C__RCV_DESCRAMBLE_P_set(v)       _BGQ_SET(1,43,v)
#define SERDES_LEFT_DCR__TS_CTL_C__RCV_DESCRAMBLE_P_insert(x,v)  _BGQ_INSERT(1,43,x,v)

//   Field:       send_train_m
//   Description: Send byte/word alignment training pattern.
#define SERDES_LEFT_DCR__TS_CTL_C__SEND_TRAIN_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_C__SEND_TRAIN_M_position     45
#define SERDES_LEFT_DCR__TS_CTL_C__SEND_TRAIN_M_get(x)       _BGQ_GET(1,45,x)
#define SERDES_LEFT_DCR__TS_CTL_C__SEND_TRAIN_M_set(v)       _BGQ_SET(1,45,v)
#define SERDES_LEFT_DCR__TS_CTL_C__SEND_TRAIN_M_insert(x,v)  _BGQ_INSERT(1,45,x,v)

//   Field:       send_train_p
//   Description: Send byte/word alignment training pattern.
#define SERDES_LEFT_DCR__TS_CTL_C__SEND_TRAIN_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_C__SEND_TRAIN_P_position     46
#define SERDES_LEFT_DCR__TS_CTL_C__SEND_TRAIN_P_get(x)       _BGQ_GET(1,46,x)
#define SERDES_LEFT_DCR__TS_CTL_C__SEND_TRAIN_P_set(v)       _BGQ_SET(1,46,v)
#define SERDES_LEFT_DCR__TS_CTL_C__SEND_TRAIN_P_insert(x,v)  _BGQ_INSERT(1,46,x,v)

//   Field:       rcv_train_m
//   Description: Start byte/word alignment process (rising edge sensitive). This starts off the automatic state machine to byte and word align the incoming data.
#define SERDES_LEFT_DCR__TS_CTL_C__RCV_TRAIN_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_C__RCV_TRAIN_M_position     47
#define SERDES_LEFT_DCR__TS_CTL_C__RCV_TRAIN_M_get(x)       _BGQ_GET(1,47,x)
#define SERDES_LEFT_DCR__TS_CTL_C__RCV_TRAIN_M_set(v)       _BGQ_SET(1,47,v)
#define SERDES_LEFT_DCR__TS_CTL_C__RCV_TRAIN_M_insert(x,v)  _BGQ_INSERT(1,47,x,v)

//   Field:       rcv_train_p
//   Description: Start byte/word alignment process (rising edge sensitive). This starts off the automatic state machine to byte and word align the incoming data.
#define SERDES_LEFT_DCR__TS_CTL_C__RCV_TRAIN_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_C__RCV_TRAIN_P_position     48
#define SERDES_LEFT_DCR__TS_CTL_C__RCV_TRAIN_P_get(x)       _BGQ_GET(1,48,x)
#define SERDES_LEFT_DCR__TS_CTL_C__RCV_TRAIN_P_set(v)       _BGQ_SET(1,48,v)
#define SERDES_LEFT_DCR__TS_CTL_C__RCV_TRAIN_P_insert(x,v)  _BGQ_INSERT(1,48,x,v)

//   Field:       loopback_en
//   Description: Loops xmit plus data to receive minus data, and vice versa
#define SERDES_LEFT_DCR__TS_CTL_C__LOOPBACK_EN_width        1
#define SERDES_LEFT_DCR__TS_CTL_C__LOOPBACK_EN_position     49
#define SERDES_LEFT_DCR__TS_CTL_C__LOOPBACK_EN_get(x)       _BGQ_GET(1,49,x)
#define SERDES_LEFT_DCR__TS_CTL_C__LOOPBACK_EN_set(v)       _BGQ_SET(1,49,v)
#define SERDES_LEFT_DCR__TS_CTL_C__LOOPBACK_EN_insert(x,v)  _BGQ_INSERT(1,49,x,v)

//   Field:       dataloop_en_m
//   Description: pma serial loopback control
#define SERDES_LEFT_DCR__TS_CTL_C__DATALOOP_EN_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_C__DATALOOP_EN_M_position     50
#define SERDES_LEFT_DCR__TS_CTL_C__DATALOOP_EN_M_get(x)       _BGQ_GET(1,50,x)
#define SERDES_LEFT_DCR__TS_CTL_C__DATALOOP_EN_M_set(v)       _BGQ_SET(1,50,v)
#define SERDES_LEFT_DCR__TS_CTL_C__DATALOOP_EN_M_insert(x,v)  _BGQ_INSERT(1,50,x,v)

//   Field:       dataloop_en_p
//   Description: pma serial loopback control
#define SERDES_LEFT_DCR__TS_CTL_C__DATALOOP_EN_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_C__DATALOOP_EN_P_position     51
#define SERDES_LEFT_DCR__TS_CTL_C__DATALOOP_EN_P_get(x)       _BGQ_GET(1,51,x)
#define SERDES_LEFT_DCR__TS_CTL_C__DATALOOP_EN_P_set(v)       _BGQ_SET(1,51,v)
#define SERDES_LEFT_DCR__TS_CTL_C__DATALOOP_EN_P_insert(x,v)  _BGQ_INSERT(1,51,x,v)

//   Field:       parlpbk_en_m
//   Description: pcs parallel loopback control
#define SERDES_LEFT_DCR__TS_CTL_C__PARLPBK_EN_M_width        1
#define SERDES_LEFT_DCR__TS_CTL_C__PARLPBK_EN_M_position     52
#define SERDES_LEFT_DCR__TS_CTL_C__PARLPBK_EN_M_get(x)       _BGQ_GET(1,52,x)
#define SERDES_LEFT_DCR__TS_CTL_C__PARLPBK_EN_M_set(v)       _BGQ_SET(1,52,v)
#define SERDES_LEFT_DCR__TS_CTL_C__PARLPBK_EN_M_insert(x,v)  _BGQ_INSERT(1,52,x,v)

//   Field:       parlpbk_en_p
//   Description: pcs parallel loopback control
#define SERDES_LEFT_DCR__TS_CTL_C__PARLPBK_EN_P_width        1
#define SERDES_LEFT_DCR__TS_CTL_C__PARLPBK_EN_P_position     53
#define SERDES_LEFT_DCR__TS_CTL_C__PARLPBK_EN_P_get(x)       _BGQ_GET(1,53,x)
#define SERDES_LEFT_DCR__TS_CTL_C__PARLPBK_EN_P_set(v)       _BGQ_SET(1,53,v)
#define SERDES_LEFT_DCR__TS_CTL_C__PARLPBK_EN_P_insert(x,v)  _BGQ_INSERT(1,53,x,v)

//   Field:       refclk_disable
//   Description: disables the pll reference clock
#define SERDES_LEFT_DCR__TS_CTL_C__REFCLK_DISABLE_width        1
#define SERDES_LEFT_DCR__TS_CTL_C__REFCLK_DISABLE_position     54
#define SERDES_LEFT_DCR__TS_CTL_C__REFCLK_DISABLE_get(x)       _BGQ_GET(1,54,x)
#define SERDES_LEFT_DCR__TS_CTL_C__REFCLK_DISABLE_set(v)       _BGQ_SET(1,54,v)
#define SERDES_LEFT_DCR__TS_CTL_C__REFCLK_DISABLE_insert(x,v)  _BGQ_INSERT(1,54,x,v)

//   Field:       lane_reset_m
//   Description: resets each lane in minus direction
#define SERDES_LEFT_DCR__TS_CTL_C__LANE_RESET_M_width        4
#define SERDES_LEFT_DCR__TS_CTL_C__LANE_RESET_M_position     58
#define SERDES_LEFT_DCR__TS_CTL_C__LANE_RESET_M_get(x)       _BGQ_GET(4,58,x)
#define SERDES_LEFT_DCR__TS_CTL_C__LANE_RESET_M_set(v)       _BGQ_SET(4,58,v)
#define SERDES_LEFT_DCR__TS_CTL_C__LANE_RESET_M_insert(x,v)  _BGQ_INSERT(4,58,x,v)

//   Field:       lane_reset_p
//   Description: resets each lane in plus direction
#define SERDES_LEFT_DCR__TS_CTL_C__LANE_RESET_P_width        4
#define SERDES_LEFT_DCR__TS_CTL_C__LANE_RESET_P_position     62
#define SERDES_LEFT_DCR__TS_CTL_C__LANE_RESET_P_get(x)       _BGQ_GET(4,62,x)
#define SERDES_LEFT_DCR__TS_CTL_C__LANE_RESET_P_set(v)       _BGQ_SET(4,62,v)
#define SERDES_LEFT_DCR__TS_CTL_C__LANE_RESET_P_insert(x,v)  _BGQ_INSERT(4,62,x,v)

//   Field:       hss_arefen
//   Description: Drives the HSSAREFEN pin on pma
#define SERDES_LEFT_DCR__TS_CTL_C__HSS_AREFEN_width        1
#define SERDES_LEFT_DCR__TS_CTL_C__HSS_AREFEN_position     63
#define SERDES_LEFT_DCR__TS_CTL_C__HSS_AREFEN_get(x)       _BGQ_GET(1,63,x)
#define SERDES_LEFT_DCR__TS_CTL_C__HSS_AREFEN_set(v)       _BGQ_SET(1,63,v)
#define SERDES_LEFT_DCR__TS_CTL_C__HSS_AREFEN_insert(x,v)  _BGQ_INSERT(1,63,x,v)

  
// Register:
//   Name:        ts_align_stat_a
//   Type:        Read Only
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Torus dimenson A alignment status register 
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A_offset  (0x0108) // Offset of register in instance 
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A_reset  UNSIGNED64(0x0000000000000000)

//   Field:       lane0_muxsel_m
//   Description: Torus minus lane0 alignment mux select
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE0_MUXSEL_M_width        6
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE0_MUXSEL_M_position     5
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE0_MUXSEL_M_get(x)       _BGQ_GET(6,5,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE0_MUXSEL_M_set(v)       _BGQ_SET(6,5,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE0_MUXSEL_M_insert(x,v)  _BGQ_INSERT(6,5,x,v)

//   Field:       lane0_muxsel_p
//   Description: Torus plus lane0 alignment mux select
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE0_MUXSEL_P_width        6
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE0_MUXSEL_P_position     11
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE0_MUXSEL_P_get(x)       _BGQ_GET(6,11,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE0_MUXSEL_P_set(v)       _BGQ_SET(6,11,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE0_MUXSEL_P_insert(x,v)  _BGQ_INSERT(6,11,x,v)

//   Field:       lane1_muxsel_m
//   Description: Torus minus lane1 alignment mux select
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE1_MUXSEL_M_width        6
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE1_MUXSEL_M_position     17
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE1_MUXSEL_M_get(x)       _BGQ_GET(6,17,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE1_MUXSEL_M_set(v)       _BGQ_SET(6,17,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE1_MUXSEL_M_insert(x,v)  _BGQ_INSERT(6,17,x,v)

//   Field:       lane1_muxsel_p
//   Description: Torus plus lane1 alignment mux select
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE1_MUXSEL_P_width        6
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE1_MUXSEL_P_position     23
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE1_MUXSEL_P_get(x)       _BGQ_GET(6,23,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE1_MUXSEL_P_set(v)       _BGQ_SET(6,23,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE1_MUXSEL_P_insert(x,v)  _BGQ_INSERT(6,23,x,v)

//   Field:       lane2_muxsel_m
//   Description: Torus minus lane2 alignment mux select
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE2_MUXSEL_M_width        6
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE2_MUXSEL_M_position     29
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE2_MUXSEL_M_get(x)       _BGQ_GET(6,29,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE2_MUXSEL_M_set(v)       _BGQ_SET(6,29,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE2_MUXSEL_M_insert(x,v)  _BGQ_INSERT(6,29,x,v)

//   Field:       lane2_muxsel_p
//   Description: Torus plus lane2 alignment mux select
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE2_MUXSEL_P_width        6
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE2_MUXSEL_P_position     35
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE2_MUXSEL_P_get(x)       _BGQ_GET(6,35,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE2_MUXSEL_P_set(v)       _BGQ_SET(6,35,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE2_MUXSEL_P_insert(x,v)  _BGQ_INSERT(6,35,x,v)

//   Field:       lane3_muxsel_m
//   Description: Torus minus lane3 alignment mux select
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE3_MUXSEL_M_width        6
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE3_MUXSEL_M_position     41
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE3_MUXSEL_M_get(x)       _BGQ_GET(6,41,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE3_MUXSEL_M_set(v)       _BGQ_SET(6,41,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE3_MUXSEL_M_insert(x,v)  _BGQ_INSERT(6,41,x,v)

//   Field:       lane3_muxsel_p
//   Description: Torus plus lane3 alignment mux select
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE3_MUXSEL_P_width        6
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE3_MUXSEL_P_position     47
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE3_MUXSEL_P_get(x)       _BGQ_GET(6,47,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE3_MUXSEL_P_set(v)       _BGQ_SET(6,47,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__LANE3_MUXSEL_P_insert(x,v)  _BGQ_INSERT(6,47,x,v)

//   Field:       byte_aligned_m
//   Description: Torus minus direction byte alignment status
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__BYTE_ALIGNED_M_width        4
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__BYTE_ALIGNED_M_position     51
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__BYTE_ALIGNED_M_get(x)       _BGQ_GET(4,51,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__BYTE_ALIGNED_M_set(v)       _BGQ_SET(4,51,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__BYTE_ALIGNED_M_insert(x,v)  _BGQ_INSERT(4,51,x,v)

//   Field:       byte_aligned_p
//   Description: Torus plus direction byte alignment status
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__BYTE_ALIGNED_P_width        4
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__BYTE_ALIGNED_P_position     55
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__BYTE_ALIGNED_P_get(x)       _BGQ_GET(4,55,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__BYTE_ALIGNED_P_set(v)       _BGQ_SET(4,55,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__BYTE_ALIGNED_P_insert(x,v)  _BGQ_INSERT(4,55,x,v)

//   Field:       fourbyte_aligned_m
//   Description: Torus minus direction is four byte aligned
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__FOURBYTE_ALIGNED_M_width        1
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__FOURBYTE_ALIGNED_M_position     56
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__FOURBYTE_ALIGNED_M_get(x)       _BGQ_GET(1,56,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__FOURBYTE_ALIGNED_M_set(v)       _BGQ_SET(1,56,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__FOURBYTE_ALIGNED_M_insert(x,v)  _BGQ_INSERT(1,56,x,v)

//   Field:       fourbyte_aligned_p
//   Description: Torus plus direction is four byte aligned
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__FOURBYTE_ALIGNED_P_width        1
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__FOURBYTE_ALIGNED_P_position     57
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__FOURBYTE_ALIGNED_P_get(x)       _BGQ_GET(1,57,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__FOURBYTE_ALIGNED_P_set(v)       _BGQ_SET(1,57,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__FOURBYTE_ALIGNED_P_insert(x,v)  _BGQ_INSERT(1,57,x,v)

//   Field:       spare
//   Description: Spare status signals
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__SPARE_width        4
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__SPARE_position     63
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__SPARE_get(x)       _BGQ_GET(4,63,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__SPARE_set(v)       _BGQ_SET(4,63,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_A__SPARE_insert(x,v)  _BGQ_INSERT(4,63,x,v)

  
// Register:
//   Name:        ts_align_stat_b
//   Type:        Read Only
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Torus dimenson B alignment status register
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B_offset  (0x0109) // Offset of register in instance 
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B_reset  UNSIGNED64(0x0000000000000000)

//   Field:       lane0_muxsel_m
//   Description: Torus minus lane0 alignment mux select
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE0_MUXSEL_M_width        6
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE0_MUXSEL_M_position     5
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE0_MUXSEL_M_get(x)       _BGQ_GET(6,5,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE0_MUXSEL_M_set(v)       _BGQ_SET(6,5,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE0_MUXSEL_M_insert(x,v)  _BGQ_INSERT(6,5,x,v)

//   Field:       lane0_muxsel_p
//   Description: Torus plus lane0 alignment mux select
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE0_MUXSEL_P_width        6
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE0_MUXSEL_P_position     11
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE0_MUXSEL_P_get(x)       _BGQ_GET(6,11,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE0_MUXSEL_P_set(v)       _BGQ_SET(6,11,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE0_MUXSEL_P_insert(x,v)  _BGQ_INSERT(6,11,x,v)

//   Field:       lane1_muxsel_m
//   Description: Torus minus lane1 alignment mux select
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE1_MUXSEL_M_width        6
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE1_MUXSEL_M_position     17
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE1_MUXSEL_M_get(x)       _BGQ_GET(6,17,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE1_MUXSEL_M_set(v)       _BGQ_SET(6,17,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE1_MUXSEL_M_insert(x,v)  _BGQ_INSERT(6,17,x,v)

//   Field:       lane1_muxsel_p
//   Description: Torus plus lane1 alignment mux select
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE1_MUXSEL_P_width        6
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE1_MUXSEL_P_position     23
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE1_MUXSEL_P_get(x)       _BGQ_GET(6,23,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE1_MUXSEL_P_set(v)       _BGQ_SET(6,23,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE1_MUXSEL_P_insert(x,v)  _BGQ_INSERT(6,23,x,v)

//   Field:       lane2_muxsel_m
//   Description: Torus minus lane2 alignment mux select
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE2_MUXSEL_M_width        6
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE2_MUXSEL_M_position     29
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE2_MUXSEL_M_get(x)       _BGQ_GET(6,29,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE2_MUXSEL_M_set(v)       _BGQ_SET(6,29,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE2_MUXSEL_M_insert(x,v)  _BGQ_INSERT(6,29,x,v)

//   Field:       lane2_muxsel_p
//   Description: Torus plus lane2 alignment mux select
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE2_MUXSEL_P_width        6
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE2_MUXSEL_P_position     35
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE2_MUXSEL_P_get(x)       _BGQ_GET(6,35,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE2_MUXSEL_P_set(v)       _BGQ_SET(6,35,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE2_MUXSEL_P_insert(x,v)  _BGQ_INSERT(6,35,x,v)

//   Field:       lane3_muxsel_m
//   Description: Torus minus lane3 alignment mux select
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE3_MUXSEL_M_width        6
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE3_MUXSEL_M_position     41
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE3_MUXSEL_M_get(x)       _BGQ_GET(6,41,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE3_MUXSEL_M_set(v)       _BGQ_SET(6,41,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE3_MUXSEL_M_insert(x,v)  _BGQ_INSERT(6,41,x,v)

//   Field:       lane3_muxsel_p
//   Description: Torus plus lane3 alignment mux select
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE3_MUXSEL_P_width        6
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE3_MUXSEL_P_position     47
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE3_MUXSEL_P_get(x)       _BGQ_GET(6,47,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE3_MUXSEL_P_set(v)       _BGQ_SET(6,47,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__LANE3_MUXSEL_P_insert(x,v)  _BGQ_INSERT(6,47,x,v)

//   Field:       byte_aligned_m
//   Description: Torus minus direction byte alignment status
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__BYTE_ALIGNED_M_width        4
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__BYTE_ALIGNED_M_position     51
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__BYTE_ALIGNED_M_get(x)       _BGQ_GET(4,51,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__BYTE_ALIGNED_M_set(v)       _BGQ_SET(4,51,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__BYTE_ALIGNED_M_insert(x,v)  _BGQ_INSERT(4,51,x,v)

//   Field:       byte_aligned_p
//   Description: Torus plus direction byte alignment status
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__BYTE_ALIGNED_P_width        4
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__BYTE_ALIGNED_P_position     55
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__BYTE_ALIGNED_P_get(x)       _BGQ_GET(4,55,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__BYTE_ALIGNED_P_set(v)       _BGQ_SET(4,55,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__BYTE_ALIGNED_P_insert(x,v)  _BGQ_INSERT(4,55,x,v)

//   Field:       fourbyte_aligned_m
//   Description: Torus minus direction is four byte aligned
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__FOURBYTE_ALIGNED_M_width        1
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__FOURBYTE_ALIGNED_M_position     56
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__FOURBYTE_ALIGNED_M_get(x)       _BGQ_GET(1,56,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__FOURBYTE_ALIGNED_M_set(v)       _BGQ_SET(1,56,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__FOURBYTE_ALIGNED_M_insert(x,v)  _BGQ_INSERT(1,56,x,v)

//   Field:       fourbyte_aligned_p
//   Description: Torus plus direction is four byte aligned
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__FOURBYTE_ALIGNED_P_width        1
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__FOURBYTE_ALIGNED_P_position     57
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__FOURBYTE_ALIGNED_P_get(x)       _BGQ_GET(1,57,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__FOURBYTE_ALIGNED_P_set(v)       _BGQ_SET(1,57,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__FOURBYTE_ALIGNED_P_insert(x,v)  _BGQ_INSERT(1,57,x,v)

//   Field:       spare
//   Description: Spare status signals
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__SPARE_width        4
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__SPARE_position     63
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__SPARE_get(x)       _BGQ_GET(4,63,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__SPARE_set(v)       _BGQ_SET(4,63,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_B__SPARE_insert(x,v)  _BGQ_INSERT(4,63,x,v)

  
// Register:
//   Name:        ts_align_stat_c
//   Type:        Read Only
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Torus dimenson C alignment status register
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C_offset  (0x0110) // Offset of register in instance 
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C_reset  UNSIGNED64(0x0000000000000000)

//   Field:       lane0_muxsel_m
//   Description: Torus minus lane0 alignment mux select
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE0_MUXSEL_M_width        6
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE0_MUXSEL_M_position     5
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE0_MUXSEL_M_get(x)       _BGQ_GET(6,5,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE0_MUXSEL_M_set(v)       _BGQ_SET(6,5,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE0_MUXSEL_M_insert(x,v)  _BGQ_INSERT(6,5,x,v)

//   Field:       lane0_muxsel_p
//   Description: Torus plus lane0 alignment mux select
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE0_MUXSEL_P_width        6
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE0_MUXSEL_P_position     11
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE0_MUXSEL_P_get(x)       _BGQ_GET(6,11,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE0_MUXSEL_P_set(v)       _BGQ_SET(6,11,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE0_MUXSEL_P_insert(x,v)  _BGQ_INSERT(6,11,x,v)

//   Field:       lane1_muxsel_m
//   Description: Torus minus lane1 alignment mux select
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE1_MUXSEL_M_width        6
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE1_MUXSEL_M_position     17
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE1_MUXSEL_M_get(x)       _BGQ_GET(6,17,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE1_MUXSEL_M_set(v)       _BGQ_SET(6,17,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE1_MUXSEL_M_insert(x,v)  _BGQ_INSERT(6,17,x,v)

//   Field:       lane1_muxsel_p
//   Description: Torus plus lane1 alignment mux select
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE1_MUXSEL_P_width        6
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE1_MUXSEL_P_position     23
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE1_MUXSEL_P_get(x)       _BGQ_GET(6,23,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE1_MUXSEL_P_set(v)       _BGQ_SET(6,23,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE1_MUXSEL_P_insert(x,v)  _BGQ_INSERT(6,23,x,v)

//   Field:       lane2_muxsel_m
//   Description: Torus minus lane2 alignment mux select
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE2_MUXSEL_M_width        6
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE2_MUXSEL_M_position     29
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE2_MUXSEL_M_get(x)       _BGQ_GET(6,29,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE2_MUXSEL_M_set(v)       _BGQ_SET(6,29,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE2_MUXSEL_M_insert(x,v)  _BGQ_INSERT(6,29,x,v)

//   Field:       lane2_muxsel_p
//   Description: Torus plus lane2 alignment mux select
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE2_MUXSEL_P_width        6
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE2_MUXSEL_P_position     35
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE2_MUXSEL_P_get(x)       _BGQ_GET(6,35,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE2_MUXSEL_P_set(v)       _BGQ_SET(6,35,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE2_MUXSEL_P_insert(x,v)  _BGQ_INSERT(6,35,x,v)

//   Field:       lane3_muxsel_m
//   Description: Torus minus lane3 alignment mux select
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE3_MUXSEL_M_width        6
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE3_MUXSEL_M_position     41
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE3_MUXSEL_M_get(x)       _BGQ_GET(6,41,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE3_MUXSEL_M_set(v)       _BGQ_SET(6,41,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE3_MUXSEL_M_insert(x,v)  _BGQ_INSERT(6,41,x,v)

//   Field:       lane3_muxsel_p
//   Description: Torus plus lane3 alignment mux select
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE3_MUXSEL_P_width        6
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE3_MUXSEL_P_position     47
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE3_MUXSEL_P_get(x)       _BGQ_GET(6,47,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE3_MUXSEL_P_set(v)       _BGQ_SET(6,47,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__LANE3_MUXSEL_P_insert(x,v)  _BGQ_INSERT(6,47,x,v)

//   Field:       byte_aligned_m
//   Description: Torus minus direction byte alignment status
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__BYTE_ALIGNED_M_width        4
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__BYTE_ALIGNED_M_position     51
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__BYTE_ALIGNED_M_get(x)       _BGQ_GET(4,51,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__BYTE_ALIGNED_M_set(v)       _BGQ_SET(4,51,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__BYTE_ALIGNED_M_insert(x,v)  _BGQ_INSERT(4,51,x,v)

//   Field:       byte_aligned_p
//   Description: Torus plus direction byte alignment status
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__BYTE_ALIGNED_P_width        4
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__BYTE_ALIGNED_P_position     55
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__BYTE_ALIGNED_P_get(x)       _BGQ_GET(4,55,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__BYTE_ALIGNED_P_set(v)       _BGQ_SET(4,55,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__BYTE_ALIGNED_P_insert(x,v)  _BGQ_INSERT(4,55,x,v)

//   Field:       fourbyte_aligned_m
//   Description: Torus minus direction is four byte aligned
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__FOURBYTE_ALIGNED_M_width        1
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__FOURBYTE_ALIGNED_M_position     56
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__FOURBYTE_ALIGNED_M_get(x)       _BGQ_GET(1,56,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__FOURBYTE_ALIGNED_M_set(v)       _BGQ_SET(1,56,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__FOURBYTE_ALIGNED_M_insert(x,v)  _BGQ_INSERT(1,56,x,v)

//   Field:       fourbyte_aligned_p
//   Description: Torus plus direction is four byte aligned
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__FOURBYTE_ALIGNED_P_width        1
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__FOURBYTE_ALIGNED_P_position     57
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__FOURBYTE_ALIGNED_P_get(x)       _BGQ_GET(1,57,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__FOURBYTE_ALIGNED_P_set(v)       _BGQ_SET(1,57,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__FOURBYTE_ALIGNED_P_insert(x,v)  _BGQ_INSERT(1,57,x,v)

//   Field:       spare
//   Description: Spare status signals
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__SPARE_width        4
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__SPARE_position     63
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__SPARE_get(x)       _BGQ_GET(4,63,x)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__SPARE_set(v)       _BGQ_SET(4,63,v)
#define SERDES_LEFT_DCR__TS_ALIGN_STAT_C__SPARE_insert(x,v)  _BGQ_INSERT(4,63,x,v)

  
// Register:
//   Name:        ts_hss_pll_stat
//   Type:        Read Only
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Torus PLL status register 
#define SERDES_LEFT_DCR__TS_HSS_PLL_STAT_offset  (0x0114) // Offset of register in instance 
#define SERDES_LEFT_DCR__TS_HSS_PLL_STAT_reset  UNSIGNED64(0x0000000000000000)

//   Field:       a_plla_locked
//   Description: Dimension A HSS PLLA is locked
#define SERDES_LEFT_DCR__TS_HSS_PLL_STAT__A_PLLA_LOCKED_width        1
#define SERDES_LEFT_DCR__TS_HSS_PLL_STAT__A_PLLA_LOCKED_position     48
#define SERDES_LEFT_DCR__TS_HSS_PLL_STAT__A_PLLA_LOCKED_get(x)       _BGQ_GET(1,48,x)
#define SERDES_LEFT_DCR__TS_HSS_PLL_STAT__A_PLLA_LOCKED_set(v)       _BGQ_SET(1,48,v)
#define SERDES_LEFT_DCR__TS_HSS_PLL_STAT__A_PLLA_LOCKED_insert(x,v)  _BGQ_INSERT(1,48,x,v)

//   Field:       b_plla_locked
//   Description: Dimension B HSS PLLA is locked
#define SERDES_LEFT_DCR__TS_HSS_PLL_STAT__B_PLLA_LOCKED_width        1
#define SERDES_LEFT_DCR__TS_HSS_PLL_STAT__B_PLLA_LOCKED_position     49
#define SERDES_LEFT_DCR__TS_HSS_PLL_STAT__B_PLLA_LOCKED_get(x)       _BGQ_GET(1,49,x)
#define SERDES_LEFT_DCR__TS_HSS_PLL_STAT__B_PLLA_LOCKED_set(v)       _BGQ_SET(1,49,v)
#define SERDES_LEFT_DCR__TS_HSS_PLL_STAT__B_PLLA_LOCKED_insert(x,v)  _BGQ_INSERT(1,49,x,v)

//   Field:       c_plla_locked
//   Description: Dimension C HSS PLLA is locked
#define SERDES_LEFT_DCR__TS_HSS_PLL_STAT__C_PLLA_LOCKED_width        1
#define SERDES_LEFT_DCR__TS_HSS_PLL_STAT__C_PLLA_LOCKED_position     50
#define SERDES_LEFT_DCR__TS_HSS_PLL_STAT__C_PLLA_LOCKED_get(x)       _BGQ_GET(1,50,x)
#define SERDES_LEFT_DCR__TS_HSS_PLL_STAT__C_PLLA_LOCKED_set(v)       _BGQ_SET(1,50,v)
#define SERDES_LEFT_DCR__TS_HSS_PLL_STAT__C_PLLA_LOCKED_insert(x,v)  _BGQ_INSERT(1,50,x,v)

  
// Register:
//   Name:        ts_hss_lane_stat
//   Type:        Read Only
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Torus HSS status register. NOTE: This register should not be read until after the PLL is locked and a lane reset has been issued and released. (lane_ready is X until after pll is locked and lanes have been reset). 
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT_offset  (0x0117) // Offset of register in instance 
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT_reset  UNSIGNED64(0x0000000000000000)

//   Field:       lane_ready_a_m
//   Description: Torus Dimension A minus direction hss lane ready
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_A_M_width        4
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_A_M_position     35
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_A_M_get(x)       _BGQ_GET(4,35,x)
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_A_M_set(v)       _BGQ_SET(4,35,v)
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_A_M_insert(x,v)  _BGQ_INSERT(4,35,x,v)

//   Field:       lane_ready_a_p
//   Description: Torus Demension A plus direction hss lane ready
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_A_P_width        4
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_A_P_position     39
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_A_P_get(x)       _BGQ_GET(4,39,x)
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_A_P_set(v)       _BGQ_SET(4,39,v)
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_A_P_insert(x,v)  _BGQ_INSERT(4,39,x,v)

//   Field:       lane_ready_b_m
//   Description: Torus Dimension B minus direction hss lane ready
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_B_M_width        4
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_B_M_position     43
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_B_M_get(x)       _BGQ_GET(4,43,x)
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_B_M_set(v)       _BGQ_SET(4,43,v)
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_B_M_insert(x,v)  _BGQ_INSERT(4,43,x,v)

//   Field:       lane_ready_b_p
//   Description: Torus Demension B plus direction hss lane ready
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_B_P_width        4
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_B_P_position     47
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_B_P_get(x)       _BGQ_GET(4,47,x)
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_B_P_set(v)       _BGQ_SET(4,47,v)
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_B_P_insert(x,v)  _BGQ_INSERT(4,47,x,v)

//   Field:       lane_ready_c_m
//   Description: Torus Dimension C minus direction hss lane ready
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_C_M_width        4
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_C_M_position     51
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_C_M_get(x)       _BGQ_GET(4,51,x)
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_C_M_set(v)       _BGQ_SET(4,51,v)
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_C_M_insert(x,v)  _BGQ_INSERT(4,51,x,v)

//   Field:       lane_ready_c_p
//   Description: Torus Demension C plus direction hss lane ready
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_C_P_width        4
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_C_P_position     55
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_C_P_get(x)       _BGQ_GET(4,55,x)
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_C_P_set(v)       _BGQ_SET(4,55,v)
#define SERDES_LEFT_DCR__TS_HSS_LANE_STAT__LANE_READY_C_P_insert(x,v)  _BGQ_INSERT(4,55,x,v)

  
// Register:
//   Name:        prbs_ctl_a
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged and TestInt
//   Reset:       0000000000000000
//   Description: Torus dimension A PRBS control register
#define SERDES_LEFT_DCR__PRBS_CTL_A_offset  (0x0120) // Offset of register in instance 
#define SERDES_LEFT_DCR__PRBS_CTL_A_reset  UNSIGNED64(0x0000000000000000)

//   Field:       tx_prbs_en_m
//   Description: Enables prbs xmit for minus direction
#define SERDES_LEFT_DCR__PRBS_CTL_A__TX_PRBS_EN_M_width        4
#define SERDES_LEFT_DCR__PRBS_CTL_A__TX_PRBS_EN_M_position     3
#define SERDES_LEFT_DCR__PRBS_CTL_A__TX_PRBS_EN_M_get(x)       _BGQ_GET(4,3,x)
#define SERDES_LEFT_DCR__PRBS_CTL_A__TX_PRBS_EN_M_set(v)       _BGQ_SET(4,3,v)
#define SERDES_LEFT_DCR__PRBS_CTL_A__TX_PRBS_EN_M_insert(x,v)  _BGQ_INSERT(4,3,x,v)

//   Field:       tx_prbs_en_p
//   Description: Enables prbs xmit for plus direction
#define SERDES_LEFT_DCR__PRBS_CTL_A__TX_PRBS_EN_P_width        4
#define SERDES_LEFT_DCR__PRBS_CTL_A__TX_PRBS_EN_P_position     7
#define SERDES_LEFT_DCR__PRBS_CTL_A__TX_PRBS_EN_P_get(x)       _BGQ_GET(4,7,x)
#define SERDES_LEFT_DCR__PRBS_CTL_A__TX_PRBS_EN_P_set(v)       _BGQ_SET(4,7,v)
#define SERDES_LEFT_DCR__PRBS_CTL_A__TX_PRBS_EN_P_insert(x,v)  _BGQ_INSERT(4,7,x,v)

//   Field:       rx_prbs_en_m
//   Description: Enables prbs rcv checking for minus direction
#define SERDES_LEFT_DCR__PRBS_CTL_A__RX_PRBS_EN_M_width        4
#define SERDES_LEFT_DCR__PRBS_CTL_A__RX_PRBS_EN_M_position     11
#define SERDES_LEFT_DCR__PRBS_CTL_A__RX_PRBS_EN_M_get(x)       _BGQ_GET(4,11,x)
#define SERDES_LEFT_DCR__PRBS_CTL_A__RX_PRBS_EN_M_set(v)       _BGQ_SET(4,11,v)
#define SERDES_LEFT_DCR__PRBS_CTL_A__RX_PRBS_EN_M_insert(x,v)  _BGQ_INSERT(4,11,x,v)

//   Field:       rx_prbs_en_p
//   Description: Enables prbs rcv checking for plus direction
#define SERDES_LEFT_DCR__PRBS_CTL_A__RX_PRBS_EN_P_width        4
#define SERDES_LEFT_DCR__PRBS_CTL_A__RX_PRBS_EN_P_position     15
#define SERDES_LEFT_DCR__PRBS_CTL_A__RX_PRBS_EN_P_get(x)       _BGQ_GET(4,15,x)
#define SERDES_LEFT_DCR__PRBS_CTL_A__RX_PRBS_EN_P_set(v)       _BGQ_SET(4,15,v)
#define SERDES_LEFT_DCR__PRBS_CTL_A__RX_PRBS_EN_P_insert(x,v)  _BGQ_INSERT(4,15,x,v)

//   Field:       prbs_sel_m
//   Description: Pattern select for prbs generator and checker, minus direction
#define SERDES_LEFT_DCR__PRBS_CTL_A__PRBS_SEL_M_width        3
#define SERDES_LEFT_DCR__PRBS_CTL_A__PRBS_SEL_M_position     22
#define SERDES_LEFT_DCR__PRBS_CTL_A__PRBS_SEL_M_get(x)       _BGQ_GET(3,22,x)
#define SERDES_LEFT_DCR__PRBS_CTL_A__PRBS_SEL_M_set(v)       _BGQ_SET(3,22,v)
#define SERDES_LEFT_DCR__PRBS_CTL_A__PRBS_SEL_M_insert(x,v)  _BGQ_INSERT(3,22,x,v)

//   Field:       prbs_sel_p
//   Description: Pattern select for prbs generator and checker, plus direction
#define SERDES_LEFT_DCR__PRBS_CTL_A__PRBS_SEL_P_width        3
#define SERDES_LEFT_DCR__PRBS_CTL_A__PRBS_SEL_P_position     25
#define SERDES_LEFT_DCR__PRBS_CTL_A__PRBS_SEL_P_get(x)       _BGQ_GET(3,25,x)
#define SERDES_LEFT_DCR__PRBS_CTL_A__PRBS_SEL_P_set(v)       _BGQ_SET(3,25,v)
#define SERDES_LEFT_DCR__PRBS_CTL_A__PRBS_SEL_P_insert(x,v)  _BGQ_INSERT(3,25,x,v)

//   Field:       prbs_rst_m
//   Description: When set, the PRBS checker is reset for minus direction
#define SERDES_LEFT_DCR__PRBS_CTL_A__PRBS_RST_M_width        4
#define SERDES_LEFT_DCR__PRBS_CTL_A__PRBS_RST_M_position     29
#define SERDES_LEFT_DCR__PRBS_CTL_A__PRBS_RST_M_get(x)       _BGQ_GET(4,29,x)
#define SERDES_LEFT_DCR__PRBS_CTL_A__PRBS_RST_M_set(v)       _BGQ_SET(4,29,v)
#define SERDES_LEFT_DCR__PRBS_CTL_A__PRBS_RST_M_insert(x,v)  _BGQ_INSERT(4,29,x,v)

//   Field:       prbs_rst_p
//   Description: When set, the PRBS checker is reset for plus direction
#define SERDES_LEFT_DCR__PRBS_CTL_A__PRBS_RST_P_width        4
#define SERDES_LEFT_DCR__PRBS_CTL_A__PRBS_RST_P_position     33
#define SERDES_LEFT_DCR__PRBS_CTL_A__PRBS_RST_P_get(x)       _BGQ_GET(4,33,x)
#define SERDES_LEFT_DCR__PRBS_CTL_A__PRBS_RST_P_set(v)       _BGQ_SET(4,33,v)
#define SERDES_LEFT_DCR__PRBS_CTL_A__PRBS_RST_P_insert(x,v)  _BGQ_INSERT(4,33,x,v)

  
// Register:
//   Name:        prbs_ctl_b
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged and TestInt
//   Reset:       0000000000000000
//   Description: Torus dimension B PRBS control register
#define SERDES_LEFT_DCR__PRBS_CTL_B_offset  (0x0121) // Offset of register in instance 
#define SERDES_LEFT_DCR__PRBS_CTL_B_reset  UNSIGNED64(0x0000000000000000)

//   Field:       tx_prbs_en_m
//   Description: Enables prbs xmit for minus direction
#define SERDES_LEFT_DCR__PRBS_CTL_B__TX_PRBS_EN_M_width        4
#define SERDES_LEFT_DCR__PRBS_CTL_B__TX_PRBS_EN_M_position     3
#define SERDES_LEFT_DCR__PRBS_CTL_B__TX_PRBS_EN_M_get(x)       _BGQ_GET(4,3,x)
#define SERDES_LEFT_DCR__PRBS_CTL_B__TX_PRBS_EN_M_set(v)       _BGQ_SET(4,3,v)
#define SERDES_LEFT_DCR__PRBS_CTL_B__TX_PRBS_EN_M_insert(x,v)  _BGQ_INSERT(4,3,x,v)

//   Field:       tx_prbs_en_p
//   Description: Enables prbs xmit for plus direction
#define SERDES_LEFT_DCR__PRBS_CTL_B__TX_PRBS_EN_P_width        4
#define SERDES_LEFT_DCR__PRBS_CTL_B__TX_PRBS_EN_P_position     7
#define SERDES_LEFT_DCR__PRBS_CTL_B__TX_PRBS_EN_P_get(x)       _BGQ_GET(4,7,x)
#define SERDES_LEFT_DCR__PRBS_CTL_B__TX_PRBS_EN_P_set(v)       _BGQ_SET(4,7,v)
#define SERDES_LEFT_DCR__PRBS_CTL_B__TX_PRBS_EN_P_insert(x,v)  _BGQ_INSERT(4,7,x,v)

//   Field:       rx_prbs_en_m
//   Description: Enables prbs rcv checking for minus direction
#define SERDES_LEFT_DCR__PRBS_CTL_B__RX_PRBS_EN_M_width        4
#define SERDES_LEFT_DCR__PRBS_CTL_B__RX_PRBS_EN_M_position     11
#define SERDES_LEFT_DCR__PRBS_CTL_B__RX_PRBS_EN_M_get(x)       _BGQ_GET(4,11,x)
#define SERDES_LEFT_DCR__PRBS_CTL_B__RX_PRBS_EN_M_set(v)       _BGQ_SET(4,11,v)
#define SERDES_LEFT_DCR__PRBS_CTL_B__RX_PRBS_EN_M_insert(x,v)  _BGQ_INSERT(4,11,x,v)

//   Field:       rx_prbs_en_p
//   Description: Enables prbs rcv checking for plus direction
#define SERDES_LEFT_DCR__PRBS_CTL_B__RX_PRBS_EN_P_width        4
#define SERDES_LEFT_DCR__PRBS_CTL_B__RX_PRBS_EN_P_position     15
#define SERDES_LEFT_DCR__PRBS_CTL_B__RX_PRBS_EN_P_get(x)       _BGQ_GET(4,15,x)
#define SERDES_LEFT_DCR__PRBS_CTL_B__RX_PRBS_EN_P_set(v)       _BGQ_SET(4,15,v)
#define SERDES_LEFT_DCR__PRBS_CTL_B__RX_PRBS_EN_P_insert(x,v)  _BGQ_INSERT(4,15,x,v)

//   Field:       prbs_sel_m
//   Description: Pattern select for prbs generator and checker, minus direction
#define SERDES_LEFT_DCR__PRBS_CTL_B__PRBS_SEL_M_width        3
#define SERDES_LEFT_DCR__PRBS_CTL_B__PRBS_SEL_M_position     22
#define SERDES_LEFT_DCR__PRBS_CTL_B__PRBS_SEL_M_get(x)       _BGQ_GET(3,22,x)
#define SERDES_LEFT_DCR__PRBS_CTL_B__PRBS_SEL_M_set(v)       _BGQ_SET(3,22,v)
#define SERDES_LEFT_DCR__PRBS_CTL_B__PRBS_SEL_M_insert(x,v)  _BGQ_INSERT(3,22,x,v)

//   Field:       prbs_sel_p
//   Description: Pattern select for prbs generator and checker, plus direction
#define SERDES_LEFT_DCR__PRBS_CTL_B__PRBS_SEL_P_width        3
#define SERDES_LEFT_DCR__PRBS_CTL_B__PRBS_SEL_P_position     25
#define SERDES_LEFT_DCR__PRBS_CTL_B__PRBS_SEL_P_get(x)       _BGQ_GET(3,25,x)
#define SERDES_LEFT_DCR__PRBS_CTL_B__PRBS_SEL_P_set(v)       _BGQ_SET(3,25,v)
#define SERDES_LEFT_DCR__PRBS_CTL_B__PRBS_SEL_P_insert(x,v)  _BGQ_INSERT(3,25,x,v)

//   Field:       prbs_rst_m
//   Description: When set, the PRBS checker is reset for minus direction
#define SERDES_LEFT_DCR__PRBS_CTL_B__PRBS_RST_M_width        4
#define SERDES_LEFT_DCR__PRBS_CTL_B__PRBS_RST_M_position     29
#define SERDES_LEFT_DCR__PRBS_CTL_B__PRBS_RST_M_get(x)       _BGQ_GET(4,29,x)
#define SERDES_LEFT_DCR__PRBS_CTL_B__PRBS_RST_M_set(v)       _BGQ_SET(4,29,v)
#define SERDES_LEFT_DCR__PRBS_CTL_B__PRBS_RST_M_insert(x,v)  _BGQ_INSERT(4,29,x,v)

//   Field:       prbs_rst_p
//   Description: When set, the PRBS checker is reset for plus direction
#define SERDES_LEFT_DCR__PRBS_CTL_B__PRBS_RST_P_width        4
#define SERDES_LEFT_DCR__PRBS_CTL_B__PRBS_RST_P_position     33
#define SERDES_LEFT_DCR__PRBS_CTL_B__PRBS_RST_P_get(x)       _BGQ_GET(4,33,x)
#define SERDES_LEFT_DCR__PRBS_CTL_B__PRBS_RST_P_set(v)       _BGQ_SET(4,33,v)
#define SERDES_LEFT_DCR__PRBS_CTL_B__PRBS_RST_P_insert(x,v)  _BGQ_INSERT(4,33,x,v)

  
// Register:
//   Name:        prbs_ctl_c
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged and TestInt
//   Reset:       0000000000000000
//   Description: Torus dimension C PRBS control register
#define SERDES_LEFT_DCR__PRBS_CTL_C_offset  (0x0122) // Offset of register in instance 
#define SERDES_LEFT_DCR__PRBS_CTL_C_reset  UNSIGNED64(0x0000000000000000)

//   Field:       tx_prbs_en_m
//   Description: Enables prbs xmit for minus direction
#define SERDES_LEFT_DCR__PRBS_CTL_C__TX_PRBS_EN_M_width        4
#define SERDES_LEFT_DCR__PRBS_CTL_C__TX_PRBS_EN_M_position     3
#define SERDES_LEFT_DCR__PRBS_CTL_C__TX_PRBS_EN_M_get(x)       _BGQ_GET(4,3,x)
#define SERDES_LEFT_DCR__PRBS_CTL_C__TX_PRBS_EN_M_set(v)       _BGQ_SET(4,3,v)
#define SERDES_LEFT_DCR__PRBS_CTL_C__TX_PRBS_EN_M_insert(x,v)  _BGQ_INSERT(4,3,x,v)

//   Field:       tx_prbs_en_p
//   Description: Enables prbs xmit for plus direction
#define SERDES_LEFT_DCR__PRBS_CTL_C__TX_PRBS_EN_P_width        4
#define SERDES_LEFT_DCR__PRBS_CTL_C__TX_PRBS_EN_P_position     7
#define SERDES_LEFT_DCR__PRBS_CTL_C__TX_PRBS_EN_P_get(x)       _BGQ_GET(4,7,x)
#define SERDES_LEFT_DCR__PRBS_CTL_C__TX_PRBS_EN_P_set(v)       _BGQ_SET(4,7,v)
#define SERDES_LEFT_DCR__PRBS_CTL_C__TX_PRBS_EN_P_insert(x,v)  _BGQ_INSERT(4,7,x,v)

//   Field:       rx_prbs_en_m
//   Description: Enables prbs rcv checking for minus direction
#define SERDES_LEFT_DCR__PRBS_CTL_C__RX_PRBS_EN_M_width        4
#define SERDES_LEFT_DCR__PRBS_CTL_C__RX_PRBS_EN_M_position     11
#define SERDES_LEFT_DCR__PRBS_CTL_C__RX_PRBS_EN_M_get(x)       _BGQ_GET(4,11,x)
#define SERDES_LEFT_DCR__PRBS_CTL_C__RX_PRBS_EN_M_set(v)       _BGQ_SET(4,11,v)
#define SERDES_LEFT_DCR__PRBS_CTL_C__RX_PRBS_EN_M_insert(x,v)  _BGQ_INSERT(4,11,x,v)

//   Field:       rx_prbs_en_p
//   Description: Enables prbs rcv checking for plus direction
#define SERDES_LEFT_DCR__PRBS_CTL_C__RX_PRBS_EN_P_width        4
#define SERDES_LEFT_DCR__PRBS_CTL_C__RX_PRBS_EN_P_position     15
#define SERDES_LEFT_DCR__PRBS_CTL_C__RX_PRBS_EN_P_get(x)       _BGQ_GET(4,15,x)
#define SERDES_LEFT_DCR__PRBS_CTL_C__RX_PRBS_EN_P_set(v)       _BGQ_SET(4,15,v)
#define SERDES_LEFT_DCR__PRBS_CTL_C__RX_PRBS_EN_P_insert(x,v)  _BGQ_INSERT(4,15,x,v)

//   Field:       prbs_sel_m
//   Description: Pattern select for prbs generator and checker, minus direction
#define SERDES_LEFT_DCR__PRBS_CTL_C__PRBS_SEL_M_width        3
#define SERDES_LEFT_DCR__PRBS_CTL_C__PRBS_SEL_M_position     22
#define SERDES_LEFT_DCR__PRBS_CTL_C__PRBS_SEL_M_get(x)       _BGQ_GET(3,22,x)
#define SERDES_LEFT_DCR__PRBS_CTL_C__PRBS_SEL_M_set(v)       _BGQ_SET(3,22,v)
#define SERDES_LEFT_DCR__PRBS_CTL_C__PRBS_SEL_M_insert(x,v)  _BGQ_INSERT(3,22,x,v)

//   Field:       prbs_sel_p
//   Description: Pattern select for prbs generator and checker, plus direction
#define SERDES_LEFT_DCR__PRBS_CTL_C__PRBS_SEL_P_width        3
#define SERDES_LEFT_DCR__PRBS_CTL_C__PRBS_SEL_P_position     25
#define SERDES_LEFT_DCR__PRBS_CTL_C__PRBS_SEL_P_get(x)       _BGQ_GET(3,25,x)
#define SERDES_LEFT_DCR__PRBS_CTL_C__PRBS_SEL_P_set(v)       _BGQ_SET(3,25,v)
#define SERDES_LEFT_DCR__PRBS_CTL_C__PRBS_SEL_P_insert(x,v)  _BGQ_INSERT(3,25,x,v)

//   Field:       prbs_rst_m
//   Description: When set, the PRBS checker is reset for minus direction
#define SERDES_LEFT_DCR__PRBS_CTL_C__PRBS_RST_M_width        4
#define SERDES_LEFT_DCR__PRBS_CTL_C__PRBS_RST_M_position     29
#define SERDES_LEFT_DCR__PRBS_CTL_C__PRBS_RST_M_get(x)       _BGQ_GET(4,29,x)
#define SERDES_LEFT_DCR__PRBS_CTL_C__PRBS_RST_M_set(v)       _BGQ_SET(4,29,v)
#define SERDES_LEFT_DCR__PRBS_CTL_C__PRBS_RST_M_insert(x,v)  _BGQ_INSERT(4,29,x,v)

//   Field:       prbs_rst_p
//   Description: When set, the PRBS checker is reset for plus direction
#define SERDES_LEFT_DCR__PRBS_CTL_C__PRBS_RST_P_width        4
#define SERDES_LEFT_DCR__PRBS_CTL_C__PRBS_RST_P_position     33
#define SERDES_LEFT_DCR__PRBS_CTL_C__PRBS_RST_P_get(x)       _BGQ_GET(4,33,x)
#define SERDES_LEFT_DCR__PRBS_CTL_C__PRBS_RST_P_set(v)       _BGQ_SET(4,33,v)
#define SERDES_LEFT_DCR__PRBS_CTL_C__PRBS_RST_P_insert(x,v)  _BGQ_INSERT(4,33,x,v)

  
// Register:
//   Name:        prbs_stat_a
//   Type:        Read Only
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Torus dimenson A prbs status register
#define SERDES_LEFT_DCR__PRBS_STAT_A_offset  (0x0126) // Offset of register in instance 
#define SERDES_LEFT_DCR__PRBS_STAT_A_reset  UNSIGNED64(0x0000000000000000)

//   Field:       sync_m
//   Description: PRBS checker has synced, minus direction
#define SERDES_LEFT_DCR__PRBS_STAT_A__SYNC_M_width        4
#define SERDES_LEFT_DCR__PRBS_STAT_A__SYNC_M_position     3
#define SERDES_LEFT_DCR__PRBS_STAT_A__SYNC_M_get(x)       _BGQ_GET(4,3,x)
#define SERDES_LEFT_DCR__PRBS_STAT_A__SYNC_M_set(v)       _BGQ_SET(4,3,v)
#define SERDES_LEFT_DCR__PRBS_STAT_A__SYNC_M_insert(x,v)  _BGQ_INSERT(4,3,x,v)

//   Field:       sync_p
//   Description: PRBS checker has synced, plus direction
#define SERDES_LEFT_DCR__PRBS_STAT_A__SYNC_P_width        4
#define SERDES_LEFT_DCR__PRBS_STAT_A__SYNC_P_position     7
#define SERDES_LEFT_DCR__PRBS_STAT_A__SYNC_P_get(x)       _BGQ_GET(4,7,x)
#define SERDES_LEFT_DCR__PRBS_STAT_A__SYNC_P_set(v)       _BGQ_SET(4,7,v)
#define SERDES_LEFT_DCR__PRBS_STAT_A__SYNC_P_insert(x,v)  _BGQ_INSERT(4,7,x,v)

//   Field:       error_m
//   Description: PRBS error detected, minus direction
#define SERDES_LEFT_DCR__PRBS_STAT_A__ERROR_M_width        4
#define SERDES_LEFT_DCR__PRBS_STAT_A__ERROR_M_position     11
#define SERDES_LEFT_DCR__PRBS_STAT_A__ERROR_M_get(x)       _BGQ_GET(4,11,x)
#define SERDES_LEFT_DCR__PRBS_STAT_A__ERROR_M_set(v)       _BGQ_SET(4,11,v)
#define SERDES_LEFT_DCR__PRBS_STAT_A__ERROR_M_insert(x,v)  _BGQ_INSERT(4,11,x,v)

//   Field:       error_p
//   Description: PRBS error detected, plus direction
#define SERDES_LEFT_DCR__PRBS_STAT_A__ERROR_P_width        4
#define SERDES_LEFT_DCR__PRBS_STAT_A__ERROR_P_position     15
#define SERDES_LEFT_DCR__PRBS_STAT_A__ERROR_P_get(x)       _BGQ_GET(4,15,x)
#define SERDES_LEFT_DCR__PRBS_STAT_A__ERROR_P_set(v)       _BGQ_SET(4,15,v)
#define SERDES_LEFT_DCR__PRBS_STAT_A__ERROR_P_insert(x,v)  _BGQ_INSERT(4,15,x,v)

  
// Register:
//   Name:        prbs_stat_b
//   Type:        Read Only
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Torus dimenson B prbs status register
#define SERDES_LEFT_DCR__PRBS_STAT_B_offset  (0x0127) // Offset of register in instance 
#define SERDES_LEFT_DCR__PRBS_STAT_B_reset  UNSIGNED64(0x0000000000000000)

//   Field:       sync_m
//   Description: PRBS checker has synced, minus direction
#define SERDES_LEFT_DCR__PRBS_STAT_B__SYNC_M_width        4
#define SERDES_LEFT_DCR__PRBS_STAT_B__SYNC_M_position     3
#define SERDES_LEFT_DCR__PRBS_STAT_B__SYNC_M_get(x)       _BGQ_GET(4,3,x)
#define SERDES_LEFT_DCR__PRBS_STAT_B__SYNC_M_set(v)       _BGQ_SET(4,3,v)
#define SERDES_LEFT_DCR__PRBS_STAT_B__SYNC_M_insert(x,v)  _BGQ_INSERT(4,3,x,v)

//   Field:       sync_p
//   Description: PRBS checker has synced, plus direction
#define SERDES_LEFT_DCR__PRBS_STAT_B__SYNC_P_width        4
#define SERDES_LEFT_DCR__PRBS_STAT_B__SYNC_P_position     7
#define SERDES_LEFT_DCR__PRBS_STAT_B__SYNC_P_get(x)       _BGQ_GET(4,7,x)
#define SERDES_LEFT_DCR__PRBS_STAT_B__SYNC_P_set(v)       _BGQ_SET(4,7,v)
#define SERDES_LEFT_DCR__PRBS_STAT_B__SYNC_P_insert(x,v)  _BGQ_INSERT(4,7,x,v)

//   Field:       error_m
//   Description: PRBS error detected, minus direction
#define SERDES_LEFT_DCR__PRBS_STAT_B__ERROR_M_width        4
#define SERDES_LEFT_DCR__PRBS_STAT_B__ERROR_M_position     11
#define SERDES_LEFT_DCR__PRBS_STAT_B__ERROR_M_get(x)       _BGQ_GET(4,11,x)
#define SERDES_LEFT_DCR__PRBS_STAT_B__ERROR_M_set(v)       _BGQ_SET(4,11,v)
#define SERDES_LEFT_DCR__PRBS_STAT_B__ERROR_M_insert(x,v)  _BGQ_INSERT(4,11,x,v)

//   Field:       error_p
//   Description: PRBS error detected, plus direction
#define SERDES_LEFT_DCR__PRBS_STAT_B__ERROR_P_width        4
#define SERDES_LEFT_DCR__PRBS_STAT_B__ERROR_P_position     15
#define SERDES_LEFT_DCR__PRBS_STAT_B__ERROR_P_get(x)       _BGQ_GET(4,15,x)
#define SERDES_LEFT_DCR__PRBS_STAT_B__ERROR_P_set(v)       _BGQ_SET(4,15,v)
#define SERDES_LEFT_DCR__PRBS_STAT_B__ERROR_P_insert(x,v)  _BGQ_INSERT(4,15,x,v)

  
// Register:
//   Name:        prbs_stat_c
//   Type:        Read Only
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Torus dimenson C prbs status register
#define SERDES_LEFT_DCR__PRBS_STAT_C_offset  (0x0128) // Offset of register in instance 
#define SERDES_LEFT_DCR__PRBS_STAT_C_reset  UNSIGNED64(0x0000000000000000)

//   Field:       sync_m
//   Description: PRBS checker has synced, minus direction
#define SERDES_LEFT_DCR__PRBS_STAT_C__SYNC_M_width        4
#define SERDES_LEFT_DCR__PRBS_STAT_C__SYNC_M_position     3
#define SERDES_LEFT_DCR__PRBS_STAT_C__SYNC_M_get(x)       _BGQ_GET(4,3,x)
#define SERDES_LEFT_DCR__PRBS_STAT_C__SYNC_M_set(v)       _BGQ_SET(4,3,v)
#define SERDES_LEFT_DCR__PRBS_STAT_C__SYNC_M_insert(x,v)  _BGQ_INSERT(4,3,x,v)

//   Field:       sync_p
//   Description: PRBS checker has synced, plus direction
#define SERDES_LEFT_DCR__PRBS_STAT_C__SYNC_P_width        4
#define SERDES_LEFT_DCR__PRBS_STAT_C__SYNC_P_position     7
#define SERDES_LEFT_DCR__PRBS_STAT_C__SYNC_P_get(x)       _BGQ_GET(4,7,x)
#define SERDES_LEFT_DCR__PRBS_STAT_C__SYNC_P_set(v)       _BGQ_SET(4,7,v)
#define SERDES_LEFT_DCR__PRBS_STAT_C__SYNC_P_insert(x,v)  _BGQ_INSERT(4,7,x,v)

//   Field:       error_m
//   Description: PRBS error detected, minus direction
#define SERDES_LEFT_DCR__PRBS_STAT_C__ERROR_M_width        4
#define SERDES_LEFT_DCR__PRBS_STAT_C__ERROR_M_position     11
#define SERDES_LEFT_DCR__PRBS_STAT_C__ERROR_M_get(x)       _BGQ_GET(4,11,x)
#define SERDES_LEFT_DCR__PRBS_STAT_C__ERROR_M_set(v)       _BGQ_SET(4,11,v)
#define SERDES_LEFT_DCR__PRBS_STAT_C__ERROR_M_insert(x,v)  _BGQ_INSERT(4,11,x,v)

//   Field:       error_p
//   Description: PRBS error detected, plus direction
#define SERDES_LEFT_DCR__PRBS_STAT_C__ERROR_P_width        4
#define SERDES_LEFT_DCR__PRBS_STAT_C__ERROR_P_position     15
#define SERDES_LEFT_DCR__PRBS_STAT_C__ERROR_P_get(x)       _BGQ_GET(4,15,x)
#define SERDES_LEFT_DCR__PRBS_STAT_C__ERROR_P_set(v)       _BGQ_SET(4,15,v)
#define SERDES_LEFT_DCR__PRBS_STAT_C__ERROR_P_insert(x,v)  _BGQ_INSERT(4,15,x,v)

  
// Register:
//   Name:        error_inject_timer_val
//   Type:        Read/Write External
//   Permission:  Read: All; Write: Privileged and TestInt
//   Reset:       0000000000000000
//   Description: Serdes Error Injection Timer Value Register
#define SERDES_LEFT_DCR__ERROR_INJECT_TIMER_VAL_offset  (0x0132) // Offset of register in instance 
#define SERDES_LEFT_DCR__ERROR_INJECT_TIMER_VAL_range  (0x1) // Range of external address space
#define SERDES_LEFT_DCR__ERROR_INJECT_TIMER_VAL_reset  UNSIGNED64(0x0000000000000000)

//   Field:       rnd
//   Description: Define the starting value for the random number generator
#define SERDES_LEFT_DCR__ERROR_INJECT_TIMER_VAL__RND_width        16
#define SERDES_LEFT_DCR__ERROR_INJECT_TIMER_VAL__RND_position     31
#define SERDES_LEFT_DCR__ERROR_INJECT_TIMER_VAL__RND_get(x)       _BGQ_GET(16,31,x)
#define SERDES_LEFT_DCR__ERROR_INJECT_TIMER_VAL__RND_set(v)       _BGQ_SET(16,31,v)
#define SERDES_LEFT_DCR__ERROR_INJECT_TIMER_VAL__RND_insert(x,v)  _BGQ_INSERT(16,31,x,v)

//   Field:       count
//   Description: Number of clk500 cycles until next error injection event
#define SERDES_LEFT_DCR__ERROR_INJECT_TIMER_VAL__COUNT_width        32
#define SERDES_LEFT_DCR__ERROR_INJECT_TIMER_VAL__COUNT_position     63
#define SERDES_LEFT_DCR__ERROR_INJECT_TIMER_VAL__COUNT_get(x)       _BGQ_GET(32,63,x)
#define SERDES_LEFT_DCR__ERROR_INJECT_TIMER_VAL__COUNT_set(v)       _BGQ_SET(32,63,v)
#define SERDES_LEFT_DCR__ERROR_INJECT_TIMER_VAL__COUNT_insert(x,v)  _BGQ_INSERT(32,63,x,v)

  
// Register:
//   Name:        error_inject_limit
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged and TestInt
//   Reset:       0000000000000000
//   Description: Serdes Error Injection Timer Register
#define SERDES_LEFT_DCR__ERROR_INJECT_LIMIT_offset  (0x0133) // Offset of register in instance 
#define SERDES_LEFT_DCR__ERROR_INJECT_LIMIT_reset  UNSIGNED64(0x0000000000000000)

//   Field:       rnd_mask
//   Description: This field is ANDed with random value before being added to low_limit
#define SERDES_LEFT_DCR__ERROR_INJECT_LIMIT__RND_MASK_width        16
#define SERDES_LEFT_DCR__ERROR_INJECT_LIMIT__RND_MASK_position     31
#define SERDES_LEFT_DCR__ERROR_INJECT_LIMIT__RND_MASK_get(x)       _BGQ_GET(16,31,x)
#define SERDES_LEFT_DCR__ERROR_INJECT_LIMIT__RND_MASK_set(v)       _BGQ_SET(16,31,v)
#define SERDES_LEFT_DCR__ERROR_INJECT_LIMIT__RND_MASK_insert(x,v)  _BGQ_INSERT(16,31,x,v)

//   Field:       low_limit
//   Description: Counter reload lower limit. The masked random vaue is added to this field and used as the injection timer reload value
#define SERDES_LEFT_DCR__ERROR_INJECT_LIMIT__LOW_LIMIT_width        32
#define SERDES_LEFT_DCR__ERROR_INJECT_LIMIT__LOW_LIMIT_position     63
#define SERDES_LEFT_DCR__ERROR_INJECT_LIMIT__LOW_LIMIT_get(x)       _BGQ_GET(32,63,x)
#define SERDES_LEFT_DCR__ERROR_INJECT_LIMIT__LOW_LIMIT_set(v)       _BGQ_SET(32,63,v)
#define SERDES_LEFT_DCR__ERROR_INJECT_LIMIT__LOW_LIMIT_insert(x,v)  _BGQ_INSERT(32,63,x,v)

  
// Register:
//   Name:        error_inject_control
//   Type:        Read/Write Internal
//   Permission:  Read: All; Write: Privileged and TestInt
//   Reset:       0000000000000000
//   Description: Serdes Error Injection Control Register
#define SERDES_LEFT_DCR__ERROR_INJECT_CONTROL_offset  (0x0134) // Offset of register in instance 
#define SERDES_LEFT_DCR__ERROR_INJECT_CONTROL_reset  UNSIGNED64(0x0000000000000000)

//   Field:       enable
//   Description: Enables error injection logic
#define SERDES_LEFT_DCR__ERROR_INJECT_CONTROL__ENABLE_width        1
#define SERDES_LEFT_DCR__ERROR_INJECT_CONTROL__ENABLE_position     0
#define SERDES_LEFT_DCR__ERROR_INJECT_CONTROL__ENABLE_get(x)       _BGQ_GET(1,0,x)
#define SERDES_LEFT_DCR__ERROR_INJECT_CONTROL__ENABLE_set(v)       _BGQ_SET(1,0,v)
#define SERDES_LEFT_DCR__ERROR_INJECT_CONTROL__ENABLE_insert(x,v)  _BGQ_INSERT(1,0,x,v)

//   Field:       link_select
//   Description: Selects which links participate in corruption, order is T0-5
#define SERDES_LEFT_DCR__ERROR_INJECT_CONTROL__LINK_SELECT_width        6
#define SERDES_LEFT_DCR__ERROR_INJECT_CONTROL__LINK_SELECT_position     6
#define SERDES_LEFT_DCR__ERROR_INJECT_CONTROL__LINK_SELECT_get(x)       _BGQ_GET(6,6,x)
#define SERDES_LEFT_DCR__ERROR_INJECT_CONTROL__LINK_SELECT_set(v)       _BGQ_SET(6,6,v)
#define SERDES_LEFT_DCR__ERROR_INJECT_CONTROL__LINK_SELECT_insert(x,v)  _BGQ_INSERT(6,6,x,v)

//   Field:       mask
//   Description: Defines which network bits 0-31 will be corrupted
#define SERDES_LEFT_DCR__ERROR_INJECT_CONTROL__MASK_width        32
#define SERDES_LEFT_DCR__ERROR_INJECT_CONTROL__MASK_position     63
#define SERDES_LEFT_DCR__ERROR_INJECT_CONTROL__MASK_get(x)       _BGQ_GET(32,63,x)
#define SERDES_LEFT_DCR__ERROR_INJECT_CONTROL__MASK_set(v)       _BGQ_SET(32,63,v)
#define SERDES_LEFT_DCR__ERROR_INJECT_CONTROL__MASK_insert(x,v)  _BGQ_INSERT(32,63,x,v)

  
// Register:
//   Name:        interrupt_internal_error_sw_info
//   Type:        Interrupt Information
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Contains status for software type internal errors
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO_offset  (0x0157) // Offset of register in instance 
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO_reset  UNSIGNED64(0x0000000000000000)

//   Field:       read
//   Description: set if the transaction was a read
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_width        1
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_position     0
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_get(x)       _BGQ_GET(1,0,x)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_set(v)       _BGQ_SET(1,0,v)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__READ_insert(x,v)  _BGQ_INSERT(1,0,x,v)

//   Field:       write
//   Description: set if the transaction was a write
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_width        1
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_position     1
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_get(x)       _BGQ_GET(1,1,x)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_set(v)       _BGQ_SET(1,1,v)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__WRITE_insert(x,v)  _BGQ_INSERT(1,1,x,v)

//   Field:       privileged
//   Description: set if the transaction had privileged permission
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_width        1
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_position     2
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_get(x)       _BGQ_GET(1,2,x)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_set(v)       _BGQ_SET(1,2,v)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__PRIVILEGED_insert(x,v)  _BGQ_INSERT(1,2,x,v)

//   Field:       test_int
//   Description: set if the transaction had test_int permission
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_width        1
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_position     3
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_get(x)       _BGQ_GET(1,3,x)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_set(v)       _BGQ_SET(1,3,v)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__TEST_INT_insert(x,v)  _BGQ_INSERT(1,3,x,v)

//   Field:       address
//   Description: reflects the address at the time of error
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_width        24
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_position     63
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_get(x)       _BGQ_GET(24,63,x)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_set(v)       _BGQ_SET(24,63,v)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_SW_INFO__ADDRESS_insert(x,v)  _BGQ_INSERT(24,63,x,v)

  
// Register:
//   Name:        interrupt_internal_error_hw_info
//   Type:        Interrupt Information
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Contains status for hardware type internal errors
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO_offset  (0x0158) // Offset of register in instance 
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO_reset  UNSIGNED64(0x0000000000000000)

//   Field:       address_parity_good
//   Description: reflects each individual parity status for address
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_width        3
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_position     6
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_get(x)       _BGQ_GET(3,6,x)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_set(v)       _BGQ_SET(3,6,v)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_PARITY_GOOD_insert(x,v)  _BGQ_INSERT(3,6,x,v)

//   Field:       data_parity_good
//   Description: reflects each individual parity status for data
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_width        8
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_position     14
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_get(x)       _BGQ_GET(8,14,x)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_set(v)       _BGQ_SET(8,14,v)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__DATA_PARITY_GOOD_insert(x,v)  _BGQ_INSERT(8,14,x,v)

//   Field:       address
//   Description: reflects the address at the time of error
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_width        24
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_position     63
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_get(x)       _BGQ_GET(24,63,x)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_set(v)       _BGQ_SET(24,63,v)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_HW_INFO__ADDRESS_insert(x,v)  _BGQ_INSERT(24,63,x,v)

  
// Register:
//   Name:        interrupt_internal_error_data_info
//   Type:        Interrupt Information
//   Permission:  All
//   Reset:       0000000000000000
//   Description: Contains status for hardware type internal errors
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO_offset  (0x0159) // Offset of register in instance 
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO_reset  UNSIGNED64(0x0000000000000000)

//   Field:       data
//   Description: reflects the local data at the time of error
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_width        64
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_position     63
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_get(x)       _BGQ_GET(64,63,x)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_set(v)       _BGQ_SET(64,63,v)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_DATA_INFO__DATA_insert(x,v)  _BGQ_INSERT(64,63,x,v)

  
// Interrupt State Register: serdes_interrupt_state
//   State Permission: Read: All; Write: Privileged and TestInt
//   First Permission: All
//   Force Permission: Privileged and TestInt
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__STATE_offset         (0x0135) // Offset of State register in instance
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__FIRST_offset         (0x0139) // Offset of First register in instance
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__FORCE_offset         (0x0138) // Offset of Force register in instance
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__MACHINE_CHECK_offset (0x013A) // Offset of Machine Check register in instance
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__CRITICAL_offset      (0x013B) // Offset of Critical register in instance
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__NONCRITICAL_offset   (0x013C) // Offset of Noncritical register in instance

#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__LOCAL_RING_width        1
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__LOCAL_RING_position     0
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__LOCAL_RING_get(x)       _BGQ_GET(1,0,x)
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__LOCAL_RING_set(v)       _BGQ_SET(1,0,v)
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__LOCAL_RING_insert(x,v)  _BGQ_INSERT(1,0,x,v)


#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__A_PLLA_LOCK_LOST_width        1
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__A_PLLA_LOCK_LOST_position     1
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__A_PLLA_LOCK_LOST_get(x)       _BGQ_GET(1,1,x)
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__A_PLLA_LOCK_LOST_set(v)       _BGQ_SET(1,1,v)
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__A_PLLA_LOCK_LOST_insert(x,v)  _BGQ_INSERT(1,1,x,v)


#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__B_PLLA_LOCK_LOST_width        1
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__B_PLLA_LOCK_LOST_position     3
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__B_PLLA_LOCK_LOST_get(x)       _BGQ_GET(1,3,x)
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__B_PLLA_LOCK_LOST_set(v)       _BGQ_SET(1,3,v)
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__B_PLLA_LOCK_LOST_insert(x,v)  _BGQ_INSERT(1,3,x,v)


#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__C_PLLA_LOCK_LOST_width        1
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__C_PLLA_LOCK_LOST_position     4
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__C_PLLA_LOCK_LOST_get(x)       _BGQ_GET(1,4,x)
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__C_PLLA_LOCK_LOST_set(v)       _BGQ_SET(1,4,v)
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__C_PLLA_LOCK_LOST_insert(x,v)  _BGQ_INSERT(1,4,x,v)

  
// Interrupt State Register: interrupt_internal_error
//   State Permission: Read: All; Write: Privileged and TestInt
//   First Permission: All
//   Force Permission: Privileged and TestInt
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR__STATE_offset         (0x0150) // Offset of State register in instance
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR__FIRST_offset         (0x0153) // Offset of First register in instance
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR__FORCE_offset         (0x0152) // Offset of Force register in instance
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK_offset (0x0154) // Offset of Machine Check register in instance
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR__CRITICAL_offset      (0x0155) // Offset of Critical register in instance
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR__NONCRITICAL_offset   (0x0156) // Offset of Noncritical register in instance

#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_width        1
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_position     0
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_get(x)       _BGQ_GET(1,0,x)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_set(v)       _BGQ_SET(1,0,v)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_insert(x,v)  _BGQ_INSERT(1,0,x,v)


#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_width        1
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_position     1
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_get(x)       _BGQ_GET(1,1,x)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_set(v)       _BGQ_SET(1,1,v)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_PRIV_insert(x,v)  _BGQ_INSERT(1,1,x,v)


#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_width        1
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_position     2
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_get(x)       _BGQ_GET(1,2,x)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_set(v)       _BGQ_SET(1,2,v)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_insert(x,v)  _BGQ_INSERT(1,2,x,v)


#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_width        1
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_position     3
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_get(x)       _BGQ_GET(1,3,x)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_set(v)       _BGQ_SET(1,3,v)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR__BAD_ADDRESS_PARITY_insert(x,v)  _BGQ_INSERT(1,3,x,v)

  
// Interrupt Control Register: serdes_interrupt_state_control_low
//   Permission: Read: All; Write: Privileged and TestInt
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE_CONTROL_LOW_offset         (0x0136) // Offset of Control register in instance
  
// Interrupt Control Register: serdes_interrupt_state_control_high
//   Permission: Read: All; Write: Privileged and TestInt
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH_offset         (0x0137) // Offset of Control register in instance

#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_width        2
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_position     1
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_get(x)       _BGQ_GET(2,1,x)
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_set(v)       _BGQ_SET(2,1,v)
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_insert(x,v)  _BGQ_INSERT(2,1,x,v)


#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__A_PLLA_LOCK_LOST_width        2
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__A_PLLA_LOCK_LOST_position     3
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__A_PLLA_LOCK_LOST_get(x)       _BGQ_GET(2,3,x)
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__A_PLLA_LOCK_LOST_set(v)       _BGQ_SET(2,3,v)
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__A_PLLA_LOCK_LOST_insert(x,v)  _BGQ_INSERT(2,3,x,v)


#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__B_PLLA_LOCK_LOST_width        2
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__B_PLLA_LOCK_LOST_position     7
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__B_PLLA_LOCK_LOST_get(x)       _BGQ_GET(2,7,x)
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__B_PLLA_LOCK_LOST_set(v)       _BGQ_SET(2,7,v)
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__B_PLLA_LOCK_LOST_insert(x,v)  _BGQ_INSERT(2,7,x,v)


#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__C_PLLA_LOCK_LOST_width        2
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__C_PLLA_LOCK_LOST_position     9
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__C_PLLA_LOCK_LOST_get(x)       _BGQ_GET(2,9,x)
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__C_PLLA_LOCK_LOST_set(v)       _BGQ_SET(2,9,v)
#define SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__C_PLLA_LOCK_LOST_insert(x,v)  _BGQ_INSERT(2,9,x,v)

  
// Interrupt Control Register: interrupt_internal_error_control_high
//   Permission: Read: All; Write: Privileged and TestInt
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH_offset         (0x0151) // Offset of Control register in instance

#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_width        2
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_position     1
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_get(x)       _BGQ_GET(2,1,x)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(v)       _BGQ_SET(2,1,v)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_insert(x,v)  _BGQ_INSERT(2,1,x,v)


#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_width        2
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_position     3
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_get(x)       _BGQ_GET(2,3,x)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(v)       _BGQ_SET(2,3,v)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_insert(x,v)  _BGQ_INSERT(2,3,x,v)


#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_width        2
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_position     5
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_get(x)       _BGQ_GET(2,5,x)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(v)       _BGQ_SET(2,5,v)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_insert(x,v)  _BGQ_INSERT(2,5,x,v)


#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_width        2
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_position     7
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_get(x)       _BGQ_GET(2,7,x)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(v)       _BGQ_SET(2,7,v)
#define SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_insert(x,v)  _BGQ_INSERT(2,7,x,v)

#ifndef __ASSEMBLY__

typedef struct serdes_left_dcr { 
    /* 0x0000 */ uint64_t rcbus[SERDES_LEFT_DCR__RCBUS_range];
    /* 0x0100 */ uint64_t rcbus_config;
    /* 0x0101 */ uint64_t sd_config;
    /* 0x0102 */ uint64_t ts_ctl_a;
    /* 0x0103 */ uint64_t ts_ctl_b;
    /* 0x0104 */ uint64_t ts_ctl_c;
    /* 0x0105 */ uint64_t reserve_0105[0x0003];
    /* 0x0108 */ uint64_t ts_align_stat_a;
    /* 0x0109 */ uint64_t ts_align_stat_b;
    /* 0x010A */ uint64_t reserve_010A[0x0006];
    /* 0x0110 */ uint64_t ts_align_stat_c;
    /* 0x0111 */ uint64_t reserve_0111[0x0003];
    /* 0x0114 */ uint64_t ts_hss_pll_stat;
    /* 0x0115 */ uint64_t reserve_0115[0x0002];
    /* 0x0117 */ uint64_t ts_hss_lane_stat;
    /* 0x0118 */ uint64_t reserve_0118[0x0008];
    /* 0x0120 */ uint64_t prbs_ctl_a;
    /* 0x0121 */ uint64_t prbs_ctl_b;
    /* 0x0122 */ uint64_t prbs_ctl_c;
    /* 0x0123 */ uint64_t reserve_0123[0x0003];
    /* 0x0126 */ uint64_t prbs_stat_a;
    /* 0x0127 */ uint64_t prbs_stat_b;
    /* 0x0128 */ uint64_t prbs_stat_c;
    /* 0x0129 */ uint64_t reserve_0129[0x0009];
    /* 0x0132 */ uint64_t error_inject_timer_val;
    /* 0x0133 */ uint64_t error_inject_limit;
    /* 0x0134 */ uint64_t error_inject_control;
    /* 0x0135 */ uint64_t serdes_interrupt_state__state;
    /* 0x0136 */ uint64_t serdes_interrupt_state_control_low;
    /* 0x0137 */ uint64_t serdes_interrupt_state_control_high;
    /* 0x0138 */ uint64_t serdes_interrupt_state__force;
    /* 0x0139 */ uint64_t serdes_interrupt_state__first;
    /* 0x013A */ uint64_t serdes_interrupt_state__machine_check;
    /* 0x013B */ uint64_t serdes_interrupt_state__critical;
    /* 0x013C */ uint64_t serdes_interrupt_state__noncritical;
    /* 0x013D */ uint64_t reserve_013D[0x0013];
    /* 0x0150 */ uint64_t interrupt_internal_error__state;
    /* 0x0151 */ uint64_t interrupt_internal_error_control_high;
    /* 0x0152 */ uint64_t interrupt_internal_error__force;
    /* 0x0153 */ uint64_t interrupt_internal_error__first;
    /* 0x0154 */ uint64_t interrupt_internal_error__machine_check;
    /* 0x0155 */ uint64_t interrupt_internal_error__critical;
    /* 0x0156 */ uint64_t interrupt_internal_error__noncritical;
    /* 0x0157 */ uint64_t interrupt_internal_error_sw_info;
    /* 0x0158 */ uint64_t interrupt_internal_error_hw_info;
} serdes_left_dcr_t;
  
#define SERDES_LEFT_DCR_USER_PTR _DCR_USER_PTR(serdes_left_dcr_t, SERDES_LEFT_DCR)
#define SERDES_LEFT_DCR_PRIV_PTR _DCR_PRIV_PTR(serdes_left_dcr_t, SERDES_LEFT_DCR)
  
#endif // !__ASSEMBLY__
  
__END_DECLS
  
#endif // add nothing below this line.
