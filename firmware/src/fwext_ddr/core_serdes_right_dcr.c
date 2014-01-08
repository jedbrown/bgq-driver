/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q                                                      */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/* US Government Users Restricted Rights - Use, duplication or      */
/*   disclosure restricted by GSA ADP Schedule Contract with IBM    */
/*   Corp.                                                          */
/*                                                                  */
/* This software is available to you under the Eclipse Public       */
/* License (EPL).                                                   */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
#include <firmware/include/fwext/sd_init_auto.h>
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Automatically generated with dcr-h-to-perl, do not modify
// stk@de.ibm.com
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//   Name:        rcbus
int RIGHT_RCBUS = 0x144000;
int RIGHT_RCBUS_WMASK[] = {16, 39};
int RIGHT_RCBUS_MASKED_WRITE[] = {1, 40};
int RIGHT_RCBUS_DATA[] = {16, 63};
//   Name:        rcbus_config
int RIGHT_RCBUS_CONFIG = 0x144100;
int RIGHT_RCBUS_CONFIG_ADDRESS[] = {24, 55};
int RIGHT_RCBUS_CONFIG_RESET[] = {1, 56};
//   Name:        sd_config
int RIGHT_SD_CONFIG = 0x144101;
int RIGHT_SD_CONFIG_PCIE_DISABLE[] = {1, 32};
int RIGHT_SD_CONFIG_HSSL2PLLON[] = {1, 39};
int RIGHT_SD_CONFIG_PCS_TESTMODE[] = {1, 40};
int RIGHT_SD_CONFIG_HALFRATE[] = {1, 41};
//   Name:        ts_ctl_d
int RIGHT_TS_CTL_D = 0x144102;
int RIGHT_TS_CTL_D_PCS_RESET[] = {1, 10};
int RIGHT_TS_CTL_D_PIPE_RESETN[] = {1, 11};
int RIGHT_TS_CTL_D_RX_RESET_M[] = {4, 15};
int RIGHT_TS_CTL_D_RX_RESET_P[] = {4, 19};
int RIGHT_TS_CTL_D_RX_ELASTIC_BUF_RUN_M[] = {1, 20};
int RIGHT_TS_CTL_D_RX_ELASTIC_BUF_RUN_OVR_M[] = {1, 21};
int RIGHT_TS_CTL_D_RX_ELASTIC_BUF_LAT_M[] = {3, 24};
int RIGHT_TS_CTL_D_RX_ELASTIC_BUF_RUN_P[] = {1, 25};
int RIGHT_TS_CTL_D_RX_ELASTIC_BUF_RUN_OVR_P[] = {1, 26};
int RIGHT_TS_CTL_D_RX_ELASTIC_BUF_LAT_P[] = {3, 29};
int RIGHT_TS_CTL_D_TX_ELASTIC_BUF_RUN_M[] = {1, 30};
int RIGHT_TS_CTL_D_TX_ELASTIC_BUF_LAT_M[] = {3, 34};
int RIGHT_TS_CTL_D_TX_ELASTIC_BUF_RUN_P[] = {1, 35};
int RIGHT_TS_CTL_D_TX_ELASTIC_BUF_LAT_P[] = {3, 39};
int RIGHT_TS_CTL_D_XMIT_SCRAMBLE_M[] = {1, 40};
int RIGHT_TS_CTL_D_XMIT_SCRAMBLE_P[] = {1, 41};
int RIGHT_TS_CTL_D_RCV_DESCRAMBLE_M[] = {1, 42};
int RIGHT_TS_CTL_D_RCV_DESCRAMBLE_P[] = {1, 43};
int RIGHT_TS_CTL_D_SEND_TRAIN_M[] = {1, 45};
int RIGHT_TS_CTL_D_SEND_TRAIN_P[] = {1, 46};
int RIGHT_TS_CTL_D_RCV_TRAIN_M[] = {1, 47};
int RIGHT_TS_CTL_D_RCV_TRAIN_P[] = {1, 48};
int RIGHT_TS_CTL_D_LOOPBACK_EN[] = {1, 49};
int RIGHT_TS_CTL_D_DATALOOP_EN_M[] = {1, 50};
int RIGHT_TS_CTL_D_DATALOOP_EN_P[] = {1, 51};
int RIGHT_TS_CTL_D_PARLPBK_EN_M[] = {1, 52};
int RIGHT_TS_CTL_D_PARLPBK_EN_P[] = {1, 53};
int RIGHT_TS_CTL_D_REFCLK_DISABLE[] = {1, 54};
int RIGHT_TS_CTL_D_LANE_RESET_M[] = {4, 58};
int RIGHT_TS_CTL_D_LANE_RESET_P[] = {4, 62};
int RIGHT_TS_CTL_D_HSS_AREFEN[] = {1, 63};
//   Name:        ts_ctl_e
int RIGHT_TS_CTL_E = 0x144103;
int RIGHT_TS_CTL_E_PCS_RESET[] = {1, 10};
int RIGHT_TS_CTL_E_PIPE_RESETN[] = {1, 11};
int RIGHT_TS_CTL_E_RX_RESET_M[] = {4, 15};
int RIGHT_TS_CTL_E_RX_RESET_P[] = {4, 19};
int RIGHT_TS_CTL_E_RX_ELASTIC_BUF_RUN_M[] = {1, 20};
int RIGHT_TS_CTL_E_RX_ELASTIC_BUF_RUN_OVR_M[] = {1, 21};
int RIGHT_TS_CTL_E_RX_ELASTIC_BUF_LAT_M[] = {3, 24};
int RIGHT_TS_CTL_E_RX_ELASTIC_BUF_RUN_P[] = {1, 25};
int RIGHT_TS_CTL_E_RX_ELASTIC_BUF_RUN_OVR_P[] = {1, 26};
int RIGHT_TS_CTL_E_RX_ELASTIC_BUF_LAT_P[] = {3, 29};
int RIGHT_TS_CTL_E_TX_ELASTIC_BUF_RUN_M[] = {1, 30};
int RIGHT_TS_CTL_E_TX_ELASTIC_BUF_LAT_M[] = {3, 34};
int RIGHT_TS_CTL_E_TX_ELASTIC_BUF_RUN_P[] = {1, 35};
int RIGHT_TS_CTL_E_TX_ELASTIC_BUF_LAT_P[] = {3, 39};
int RIGHT_TS_CTL_E_XMIT_SCRAMBLE_M[] = {1, 40};
int RIGHT_TS_CTL_E_XMIT_SCRAMBLE_P[] = {1, 41};
int RIGHT_TS_CTL_E_RCV_DESCRAMBLE_M[] = {1, 42};
int RIGHT_TS_CTL_E_RCV_DESCRAMBLE_P[] = {1, 43};
int RIGHT_TS_CTL_E_SEND_TRAIN_M[] = {1, 45};
int RIGHT_TS_CTL_E_SEND_TRAIN_P[] = {1, 46};
int RIGHT_TS_CTL_E_RCV_TRAIN_M[] = {1, 47};
int RIGHT_TS_CTL_E_RCV_TRAIN_P[] = {1, 48};
int RIGHT_TS_CTL_E_LOOPBACK_EN[] = {1, 49};
int RIGHT_TS_CTL_E_DATALOOP_EN_M[] = {1, 50};
int RIGHT_TS_CTL_E_DATALOOP_EN_P[] = {1, 51};
int RIGHT_TS_CTL_E_PARLPBK_EN_M[] = {1, 52};
int RIGHT_TS_CTL_E_PARLPBK_EN_P[] = {1, 53};
int RIGHT_TS_CTL_E_REFCLK_DISABLE[] = {1, 54};
int RIGHT_TS_CTL_E_LANE_RESET_M[] = {4, 58};
int RIGHT_TS_CTL_E_LANE_RESET_P[] = {4, 62};
int RIGHT_TS_CTL_E_HSS_AREFEN[] = {1, 63};
//   Name:        ts_ctl_io
int RIGHT_TS_CTL_IO = 0x144104;
int RIGHT_TS_CTL_IO_PCS_RESET[] = {1, 10};
int RIGHT_TS_CTL_IO_PIPE_RESETN[] = {1, 11};
int RIGHT_TS_CTL_IO_RX_RESET[] = {4, 15};
int RIGHT_TS_CTL_IO_RX_ELASTIC_BUF_RUN[] = {1, 20};
int RIGHT_TS_CTL_IO_RX_ELASTIC_BUF_RUN_OVR[] = {1, 21};
int RIGHT_TS_CTL_IO_RX_ELASTIC_BUF_LAT[] = {3, 24};
int RIGHT_TS_CTL_IO_TX_ELASTIC_BUF_RUN[] = {1, 30};
int RIGHT_TS_CTL_IO_TX_ELASTIC_BUF_LAT[] = {3, 34};
int RIGHT_TS_CTL_IO_XMIT_SCRAMBLE[] = {1, 40};
int RIGHT_TS_CTL_IO_RCV_DESCRAMBLE[] = {1, 42};
int RIGHT_TS_CTL_IO_SEND_TRAIN[] = {1, 45};
int RIGHT_TS_CTL_IO_RCV_TRAIN[] = {1, 47};
int RIGHT_TS_CTL_IO_LOOPBACK_EN[] = {1, 49};
int RIGHT_TS_CTL_IO_DATALOOP_EN[] = {1, 50};
int RIGHT_TS_CTL_IO_PARLPBK_EN[] = {1, 52};
int RIGHT_TS_CTL_IO_REFCLK_DISABLE[] = {1, 54};
int RIGHT_TS_CTL_IO_LANE_RESET[] = {4, 58};
int RIGHT_TS_CTL_IO_HSS_AREFEN[] = {1, 63};
//   Name:        ts_align_stat_d
int RIGHT_TS_ALIGN_STAT_D = 0x144108;
int RIGHT_TS_ALIGN_STAT_D_LANE0_MUXSEL_M[] = {6, 5};
int RIGHT_TS_ALIGN_STAT_D_LANE0_MUXSEL_P[] = {6, 11};
int RIGHT_TS_ALIGN_STAT_D_LANE1_MUXSEL_M[] = {6, 17};
int RIGHT_TS_ALIGN_STAT_D_LANE1_MUXSEL_P[] = {6, 23};
int RIGHT_TS_ALIGN_STAT_D_LANE2_MUXSEL_M[] = {6, 29};
int RIGHT_TS_ALIGN_STAT_D_LANE2_MUXSEL_P[] = {6, 35};
int RIGHT_TS_ALIGN_STAT_D_LANE3_MUXSEL_M[] = {6, 41};
int RIGHT_TS_ALIGN_STAT_D_LANE3_MUXSEL_P[] = {6, 47};
int RIGHT_TS_ALIGN_STAT_D_BYTE_ALIGNED_M[] = {4, 51};
int RIGHT_TS_ALIGN_STAT_D_BYTE_ALIGNED_P[] = {4, 55};
int RIGHT_TS_ALIGN_STAT_D_FOURBYTE_ALIGNED_M[] = {1, 56};
int RIGHT_TS_ALIGN_STAT_D_FOURBYTE_ALIGNED_P[] = {1, 57};
//   Name:        ts_align_stat_e
int RIGHT_TS_ALIGN_STAT_E = 0x144109;
int RIGHT_TS_ALIGN_STAT_E_LANE0_MUXSEL_M[] = {6, 5};
int RIGHT_TS_ALIGN_STAT_E_LANE0_MUXSEL_P[] = {6, 11};
int RIGHT_TS_ALIGN_STAT_E_LANE1_MUXSEL_M[] = {6, 17};
int RIGHT_TS_ALIGN_STAT_E_LANE1_MUXSEL_P[] = {6, 23};
int RIGHT_TS_ALIGN_STAT_E_LANE2_MUXSEL_M[] = {6, 29};
int RIGHT_TS_ALIGN_STAT_E_LANE2_MUXSEL_P[] = {6, 35};
int RIGHT_TS_ALIGN_STAT_E_LANE3_MUXSEL_M[] = {6, 41};
int RIGHT_TS_ALIGN_STAT_E_LANE3_MUXSEL_P[] = {6, 47};
int RIGHT_TS_ALIGN_STAT_E_BYTE_ALIGNED_M[] = {4, 51};
int RIGHT_TS_ALIGN_STAT_E_BYTE_ALIGNED_P[] = {4, 55};
int RIGHT_TS_ALIGN_STAT_E_FOURBYTE_ALIGNED_M[] = {1, 56};
int RIGHT_TS_ALIGN_STAT_E_FOURBYTE_ALIGNED_P[] = {1, 57};
//   Name:        ts_align_stat_io
int RIGHT_TS_ALIGN_STAT_IO = 0x144110;
int RIGHT_TS_ALIGN_STAT_IO_LANE0_MUXSEL[] = {6, 5};
int RIGHT_TS_ALIGN_STAT_IO_LANE1_MUXSEL[] = {6, 17};
int RIGHT_TS_ALIGN_STAT_IO_LANE2_MUXSEL[] = {6, 29};
int RIGHT_TS_ALIGN_STAT_IO_LANE3_MUXSEL[] = {6, 41};
int RIGHT_TS_ALIGN_STAT_IO_BYTE_ALIGNED[] = {4, 51};
int RIGHT_TS_ALIGN_STAT_IO_FOURBYTE_ALIGNED[] = {1, 56};
//   Name:        ts_hss_pll_stat
int RIGHT_TS_HSS_PLL_STAT = 0x144114;
int RIGHT_TS_HSS_PLL_STAT_D_PLLA_LOCKED[] = {1, 48};
int RIGHT_TS_HSS_PLL_STAT_IO_PLLA_LOCKED[] = {1, 50};
int RIGHT_TS_HSS_PLL_STAT_E_PLLB_LOCKED[] = {1, 57};
//   Name:        ts_hss_lane_stat
int RIGHT_TS_HSS_LANE_STAT = 0x144117;
int RIGHT_TS_HSS_LANE_STAT_LANE_READY_D_M[] = {4, 35};
int RIGHT_TS_HSS_LANE_STAT_LANE_READY_D_P[] = {4, 39};
int RIGHT_TS_HSS_LANE_STAT_LANE_READY_E_M[] = {4, 43};
int RIGHT_TS_HSS_LANE_STAT_LANE_READY_E_P[] = {4, 47};
int RIGHT_TS_HSS_LANE_STAT_LANE_READY_IO[] = {4, 51};
//   Name:        prbs_ctl_d
int RIGHT_PRBS_CTL_D = 0x144120;
int RIGHT_PRBS_CTL_D_TX_PRBS_EN_M[] = {4, 3};
int RIGHT_PRBS_CTL_D_TX_PRBS_EN_P[] = {4, 7};
int RIGHT_PRBS_CTL_D_RX_PRBS_EN_M[] = {4, 11};
int RIGHT_PRBS_CTL_D_RX_PRBS_EN_P[] = {4, 15};
int RIGHT_PRBS_CTL_D_PRBS_SEL_M[] = {3, 22};
int RIGHT_PRBS_CTL_D_PRBS_SEL_P[] = {3, 25};
int RIGHT_PRBS_CTL_D_PRBS_RST_M[] = {4, 29};
int RIGHT_PRBS_CTL_D_PRBS_RST_P[] = {4, 33};
//   Name:        prbs_ctl_e
int RIGHT_PRBS_CTL_E = 0x144121;
int RIGHT_PRBS_CTL_E_TX_PRBS_EN_M[] = {4, 3};
int RIGHT_PRBS_CTL_E_TX_PRBS_EN_P[] = {4, 7};
int RIGHT_PRBS_CTL_E_RX_PRBS_EN_M[] = {4, 11};
int RIGHT_PRBS_CTL_E_RX_PRBS_EN_P[] = {4, 15};
int RIGHT_PRBS_CTL_E_PRBS_SEL_M[] = {3, 22};
int RIGHT_PRBS_CTL_E_PRBS_SEL_P[] = {3, 25};
int RIGHT_PRBS_CTL_E_PRBS_RST_M[] = {4, 29};
int RIGHT_PRBS_CTL_E_PRBS_RST_P[] = {4, 33};
//   Name:        prbs_ctl_io
int RIGHT_PRBS_CTL_IO = 0x144122;
int RIGHT_PRBS_CTL_IO_TX_PRBS_EN[] = {4, 3};
int RIGHT_PRBS_CTL_IO_RX_PRBS_EN[] = {4, 11};
int RIGHT_PRBS_CTL_IO_PRBS_SEL[] = {3, 22};
int RIGHT_PRBS_CTL_IO_PRBS_RST[] = {4, 29};
//   Name:        prbs_stat_d
int RIGHT_PRBS_STAT_D = 0x144126;
int RIGHT_PRBS_STAT_D_SYNC_M[] = {4, 3};
int RIGHT_PRBS_STAT_D_SYNC_P[] = {4, 7};
int RIGHT_PRBS_STAT_D_ERROR_M[] = {4, 11};
int RIGHT_PRBS_STAT_D_ERROR_P[] = {4, 15};
//   Name:        prbs_stat_e
int RIGHT_PRBS_STAT_E = 0x144127;
int RIGHT_PRBS_STAT_E_SYNC_M[] = {4, 3};
int RIGHT_PRBS_STAT_E_SYNC_P[] = {4, 7};
int RIGHT_PRBS_STAT_E_ERROR_M[] = {4, 11};
int RIGHT_PRBS_STAT_E_ERROR_P[] = {4, 15};
//   Name:        prbs_stat_io
int RIGHT_PRBS_STAT_IO = 0x144128;
int RIGHT_PRBS_STAT_IO_SYNC[] = {4, 3};
int RIGHT_PRBS_STAT_IO_ERROR[] = {4, 11};
//   Name:        error_inject_timer_val
int RIGHT_ERROR_INJECT_TIMER_VAL = 0x144132;
int RIGHT_ERROR_INJECT_TIMER_VAL_RND[] = {16, 31};
int RIGHT_ERROR_INJECT_TIMER_VAL_COUNT[] = {32, 63};
//   Name:        error_inject_limit
int RIGHT_ERROR_INJECT_LIMIT = 0x144133;
int RIGHT_ERROR_INJECT_LIMIT_RND_MASK[] = {16, 31};
int RIGHT_ERROR_INJECT_LIMIT_LOW_LIMIT[] = {32, 63};
//   Name:        error_inject_control
int RIGHT_ERROR_INJECT_CONTROL = 0x144134;
int RIGHT_ERROR_INJECT_CONTROL_ENABLE[] = {1, 0};
int RIGHT_ERROR_INJECT_CONTROL_LINK_SELECT[] = {5, 5};
int RIGHT_ERROR_INJECT_CONTROL_MASK[] = {32, 63};
//   Name:        interrupt_internal_error_sw_info
int RIGHT_INTERRUPT_INTERNAL_ERROR_SW_INFO = 0x144157;
int RIGHT_INTERRUPT_INTERNAL_ERROR_SW_INFO_READ[] = {1, 0};
int RIGHT_INTERRUPT_INTERNAL_ERROR_SW_INFO_WRITE[] = {1, 1};
int RIGHT_INTERRUPT_INTERNAL_ERROR_SW_INFO_PRIVILEGED[] = {1, 2};
int RIGHT_INTERRUPT_INTERNAL_ERROR_SW_INFO_TEST_INT[] = {1, 3};
int RIGHT_INTERRUPT_INTERNAL_ERROR_SW_INFO_ADDRESS[] = {24, 63};
//   Name:        interrupt_internal_error_hw_info
int RIGHT_INTERRUPT_INTERNAL_ERROR_HW_INFO = 0x144158;
int RIGHT_INTERRUPT_INTERNAL_ERROR_HW_INFO_ADDRESS_PARITY_GOOD[] = {3, 6};
int RIGHT_INTERRUPT_INTERNAL_ERROR_HW_INFO_DATA_PARITY_GOOD[] = {8, 14};
int RIGHT_INTERRUPT_INTERNAL_ERROR_HW_INFO_ADDRESS[] = {24, 63};
//   Name:        interrupt_internal_error_data_info
int RIGHT_INTERRUPT_INTERNAL_ERROR_DATA_INFO = 0x144159;
int RIGHT_INTERRUPT_INTERNAL_ERROR_DATA_INFO_DATA[] = {64, 63};
// Interrupt State Register: serdes_interrupt_state
//   State Permission: Read: All; Write: Privileged and TestInt
//   First Permission: All
//   Force Permission: Privileged and TestInt
int RIGHT_SERDES_INTERRUPT_STATE_STATE = 0x144135;
int RIGHT_SERDES_INTERRUPT_STATE_FIRST = 0x144139;
int RIGHT_SERDES_INTERRUPT_STATE_FORCE = 0x144138;
int RIGHT_SERDES_INTERRUPT_STATE_MACHINE_CHECK = 0x14413a;
int RIGHT_SERDES_INTERRUPT_STATE_CRITICAL = 0x14413b;
int RIGHT_SERDES_INTERRUPT_STATE_NONCRITICAL = 0x14413c;
int RIGHT_SERDES_INTERRUPT_STATE_LOCAL_RING[] = {1, 0};
int RIGHT_SERDES_INTERRUPT_STATE_D_PLLA_LOCK_LOST[] = {1, 1};
int RIGHT_SERDES_INTERRUPT_STATE_E_PLLB_LOCK_LOST[] = {1, 3};
int RIGHT_SERDES_INTERRUPT_STATE_IO_PLLA_LOCK_LOST[] = {1, 4};
// Interrupt State Register: interrupt_internal_error
//   State Permission: Read: All; Write: Privileged and TestInt
//   First Permission: All
//   Force Permission: Privileged and TestInt
int RIGHT_INTERRUPT_INTERNAL_ERROR_STATE = 0x144150;
int RIGHT_INTERRUPT_INTERNAL_ERROR_FIRST = 0x144153;
int RIGHT_INTERRUPT_INTERNAL_ERROR_FORCE = 0x144152;
int RIGHT_INTERRUPT_INTERNAL_ERROR_MACHINE_CHECK = 0x144154;
int RIGHT_INTERRUPT_INTERNAL_ERROR_CRITICAL = 0x144155;
int RIGHT_INTERRUPT_INTERNAL_ERROR_NONCRITICAL = 0x144156;
int RIGHT_INTERRUPT_INTERNAL_ERROR_BAD_ADDRESS[] = {1, 0};
int RIGHT_INTERRUPT_INTERNAL_ERROR_BAD_PRIV[] = {1, 1};
int RIGHT_INTERRUPT_INTERNAL_ERROR_BAD_DATA_PARITY[] = {1, 2};
int RIGHT_INTERRUPT_INTERNAL_ERROR_BAD_ADDRESS_PARITY[] = {1, 3};
// Interrupt Control Register: serdes_interrupt_state_control_low
int RIGHT_SERDES_INTERRUPT_STATE_CONTROL_LOW = 0x144136;
// Interrupt Control Register: serdes_interrupt_state_control_high
int RIGHT_SERDES_INTERRUPT_STATE_CONTROL_HIGH = 0x144137;
int RIGHT_SERDES_INTERRUPT_STATE_CONTROL_HIGH_LOCAL_RING[] = {2, 1};
int RIGHT_SERDES_INTERRUPT_STATE_CONTROL_HIGH_D_PLLA_LOCK_LOST[] = {2, 3};
int RIGHT_SERDES_INTERRUPT_STATE_CONTROL_HIGH_E_PLLB_LOCK_LOST[] = {2, 7};
int RIGHT_SERDES_INTERRUPT_STATE_CONTROL_HIGH_IO_PLLA_LOCK_LOST[] = {2, 9};
// Interrupt Control Register: interrupt_internal_error_control_high
int RIGHT_INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH = 0x144151;
int RIGHT_INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH_BAD_ADDRESS[] = {2, 1};
int RIGHT_INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH_BAD_PRIV[] = {2, 3};
int RIGHT_INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH_BAD_DATA_PARITY[] = {2, 5};
int RIGHT_INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH_BAD_ADDRESS_PARITY[] = {2, 7};
