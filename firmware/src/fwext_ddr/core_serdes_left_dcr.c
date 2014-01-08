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
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Automatically generated with dcr-h-to-perl, do not modify
// stk@de.ibm.com
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//   Name:        rcbus
int LEFT_RCBUS = 0x140000;
int LEFT_RCBUS_WMASK[] = {16, 39};
int LEFT_RCBUS_MASKED_WRITE[] = {1, 40};
int LEFT_RCBUS_DATA[] = {16, 63};
//   Name:        rcbus_config
int LEFT_RCBUS_CONFIG = 0x140100;
int LEFT_RCBUS_CONFIG_ADDRESS[] = {24, 55};
int LEFT_RCBUS_CONFIG_RESET[] = {1, 56};
//   Name:        sd_config
int LEFT_SD_CONFIG = 0x140101;
int LEFT_SD_CONFIG_HSSL2PLLON[] = {1, 39};
int LEFT_SD_CONFIG_PCS_TESTMODE[] = {1, 40};
int LEFT_SD_CONFIG_HALFRATE[] = {1, 41};
//   Name:        ts_ctl_a
int LEFT_TS_CTL_A = 0x140102;
int LEFT_TS_CTL_A_PCS_RESET[] = {1, 10};
int LEFT_TS_CTL_A_PIPE_RESETN[] = {1, 11};
int LEFT_TS_CTL_A_RX_RESET_M[] = {4, 15};
int LEFT_TS_CTL_A_RX_RESET_P[] = {4, 19};
int LEFT_TS_CTL_A_RX_ELASTIC_BUF_RUN_M[] = {1, 20};
int LEFT_TS_CTL_A_RX_ELASTIC_BUF_RUN_OVR_M[] = {1, 21};
int LEFT_TS_CTL_A_RX_ELASTIC_BUF_LAT_M[] = {3, 24};
int LEFT_TS_CTL_A_RX_ELASTIC_BUF_RUN_P[] = {1, 25};
int LEFT_TS_CTL_A_RX_ELASTIC_BUF_RUN_OVR_P[] = {1, 26};
int LEFT_TS_CTL_A_RX_ELASTIC_BUF_LAT_P[] = {3, 29};
int LEFT_TS_CTL_A_TX_ELASTIC_BUF_RUN_M[] = {1, 30};
int LEFT_TS_CTL_A_TX_ELASTIC_BUF_LAT_M[] = {3, 34};
int LEFT_TS_CTL_A_TX_ELASTIC_BUF_RUN_P[] = {1, 35};
int LEFT_TS_CTL_A_TX_ELASTIC_BUF_LAT_P[] = {3, 39};
int LEFT_TS_CTL_A_XMIT_SCRAMBLE_M[] = {1, 40};
int LEFT_TS_CTL_A_XMIT_SCRAMBLE_P[] = {1, 41};
int LEFT_TS_CTL_A_RCV_DESCRAMBLE_M[] = {1, 42};
int LEFT_TS_CTL_A_RCV_DESCRAMBLE_P[] = {1, 43};
int LEFT_TS_CTL_A_SEND_TRAIN_M[] = {1, 45};
int LEFT_TS_CTL_A_SEND_TRAIN_P[] = {1, 46};
int LEFT_TS_CTL_A_RCV_TRAIN_M[] = {1, 47};
int LEFT_TS_CTL_A_RCV_TRAIN_P[] = {1, 48};
int LEFT_TS_CTL_A_LOOPBACK_EN[] = {1, 49};
int LEFT_TS_CTL_A_DATALOOP_EN_M[] = {1, 50};
int LEFT_TS_CTL_A_DATALOOP_EN_P[] = {1, 51};
int LEFT_TS_CTL_A_PARLPBK_EN_M[] = {1, 52};
int LEFT_TS_CTL_A_PARLPBK_EN_P[] = {1, 53};
int LEFT_TS_CTL_A_REFCLK_DISABLE[] = {1, 54};
int LEFT_TS_CTL_A_LANE_RESET_M[] = {4, 58};
int LEFT_TS_CTL_A_LANE_RESET_P[] = {4, 62};
int LEFT_TS_CTL_A_HSS_AREFEN[] = {1, 63};
//   Name:        ts_ctl_b
int LEFT_TS_CTL_B = 0x140103;
int LEFT_TS_CTL_B_PCS_RESET[] = {1, 10};
int LEFT_TS_CTL_B_PIPE_RESETN[] = {1, 11};
int LEFT_TS_CTL_B_RX_RESET_M[] = {4, 15};
int LEFT_TS_CTL_B_RX_RESET_P[] = {4, 19};
int LEFT_TS_CTL_B_RX_ELASTIC_BUF_RUN_M[] = {1, 20};
int LEFT_TS_CTL_B_RX_ELASTIC_BUF_RUN_OVR_M[] = {1, 21};
int LEFT_TS_CTL_B_RX_ELASTIC_BUF_LAT_M[] = {3, 24};
int LEFT_TS_CTL_B_RX_ELASTIC_BUF_RUN_P[] = {1, 25};
int LEFT_TS_CTL_B_RX_ELASTIC_BUF_RUN_OVR_P[] = {1, 26};
int LEFT_TS_CTL_B_RX_ELASTIC_BUF_LAT_P[] = {3, 29};
int LEFT_TS_CTL_B_TX_ELASTIC_BUF_RUN_M[] = {1, 30};
int LEFT_TS_CTL_B_TX_ELASTIC_BUF_LAT_M[] = {3, 34};
int LEFT_TS_CTL_B_TX_ELASTIC_BUF_RUN_P[] = {1, 35};
int LEFT_TS_CTL_B_TX_ELASTIC_BUF_LAT_P[] = {3, 39};
int LEFT_TS_CTL_B_XMIT_SCRAMBLE_M[] = {1, 40};
int LEFT_TS_CTL_B_XMIT_SCRAMBLE_P[] = {1, 41};
int LEFT_TS_CTL_B_RCV_DESCRAMBLE_M[] = {1, 42};
int LEFT_TS_CTL_B_RCV_DESCRAMBLE_P[] = {1, 43};
int LEFT_TS_CTL_B_SEND_TRAIN_M[] = {1, 45};
int LEFT_TS_CTL_B_SEND_TRAIN_P[] = {1, 46};
int LEFT_TS_CTL_B_RCV_TRAIN_M[] = {1, 47};
int LEFT_TS_CTL_B_RCV_TRAIN_P[] = {1, 48};
int LEFT_TS_CTL_B_LOOPBACK_EN[] = {1, 49};
int LEFT_TS_CTL_B_DATALOOP_EN_M[] = {1, 50};
int LEFT_TS_CTL_B_DATALOOP_EN_P[] = {1, 51};
int LEFT_TS_CTL_B_PARLPBK_EN_M[] = {1, 52};
int LEFT_TS_CTL_B_PARLPBK_EN_P[] = {1, 53};
int LEFT_TS_CTL_B_REFCLK_DISABLE[] = {1, 54};
int LEFT_TS_CTL_B_LANE_RESET_M[] = {4, 58};
int LEFT_TS_CTL_B_LANE_RESET_P[] = {4, 62};
int LEFT_TS_CTL_B_HSS_AREFEN[] = {1, 63};
//   Name:        ts_ctl_c
int LEFT_TS_CTL_C = 0x140104;
int LEFT_TS_CTL_C_PCS_RESET[] = {1, 10};
int LEFT_TS_CTL_C_PIPE_RESETN[] = {1, 11};
int LEFT_TS_CTL_C_RX_RESET_M[] = {4, 15};
int LEFT_TS_CTL_C_RX_RESET_P[] = {4, 19};
int LEFT_TS_CTL_C_RX_ELASTIC_BUF_RUN_M[] = {1, 20};
int LEFT_TS_CTL_C_RX_ELASTIC_BUF_RUN_OVR_M[] = {1, 21};
int LEFT_TS_CTL_C_RX_ELASTIC_BUF_LAT_M[] = {3, 24};
int LEFT_TS_CTL_C_RX_ELASTIC_BUF_RUN_P[] = {1, 25};
int LEFT_TS_CTL_C_RX_ELASTIC_BUF_RUN_OVR_P[] = {1, 26};
int LEFT_TS_CTL_C_RX_ELASTIC_BUF_LAT_P[] = {3, 29};
int LEFT_TS_CTL_C_TX_ELASTIC_BUF_RUN_M[] = {1, 30};
int LEFT_TS_CTL_C_TX_ELASTIC_BUF_LAT_M[] = {3, 34};
int LEFT_TS_CTL_C_TX_ELASTIC_BUF_RUN_P[] = {1, 35};
int LEFT_TS_CTL_C_TX_ELASTIC_BUF_LAT_P[] = {3, 39};
int LEFT_TS_CTL_C_XMIT_SCRAMBLE_M[] = {1, 40};
int LEFT_TS_CTL_C_XMIT_SCRAMBLE_P[] = {1, 41};
int LEFT_TS_CTL_C_RCV_DESCRAMBLE_M[] = {1, 42};
int LEFT_TS_CTL_C_RCV_DESCRAMBLE_P[] = {1, 43};
int LEFT_TS_CTL_C_SEND_TRAIN_M[] = {1, 45};
int LEFT_TS_CTL_C_SEND_TRAIN_P[] = {1, 46};
int LEFT_TS_CTL_C_RCV_TRAIN_M[] = {1, 47};
int LEFT_TS_CTL_C_RCV_TRAIN_P[] = {1, 48};
int LEFT_TS_CTL_C_LOOPBACK_EN[] = {1, 49};
int LEFT_TS_CTL_C_DATALOOP_EN_M[] = {1, 50};
int LEFT_TS_CTL_C_DATALOOP_EN_P[] = {1, 51};
int LEFT_TS_CTL_C_PARLPBK_EN_M[] = {1, 52};
int LEFT_TS_CTL_C_PARLPBK_EN_P[] = {1, 53};
int LEFT_TS_CTL_C_REFCLK_DISABLE[] = {1, 54};
int LEFT_TS_CTL_C_LANE_RESET_M[] = {4, 58};
int LEFT_TS_CTL_C_LANE_RESET_P[] = {4, 62};
int LEFT_TS_CTL_C_HSS_AREFEN[] = {1, 63};
//   Name:        ts_align_stat_a
int LEFT_TS_ALIGN_STAT_A = 0x140108;
int LEFT_TS_ALIGN_STAT_A_LANE0_MUXSEL_M[] = {6, 5};
int LEFT_TS_ALIGN_STAT_A_LANE0_MUXSEL_P[] = {6, 11};
int LEFT_TS_ALIGN_STAT_A_LANE1_MUXSEL_M[] = {6, 17};
int LEFT_TS_ALIGN_STAT_A_LANE1_MUXSEL_P[] = {6, 23};
int LEFT_TS_ALIGN_STAT_A_LANE2_MUXSEL_M[] = {6, 29};
int LEFT_TS_ALIGN_STAT_A_LANE2_MUXSEL_P[] = {6, 35};
int LEFT_TS_ALIGN_STAT_A_LANE3_MUXSEL_M[] = {6, 41};
int LEFT_TS_ALIGN_STAT_A_LANE3_MUXSEL_P[] = {6, 47};
int LEFT_TS_ALIGN_STAT_A_BYTE_ALIGNED_M[] = {4, 51};
int LEFT_TS_ALIGN_STAT_A_BYTE_ALIGNED_P[] = {4, 55};
int LEFT_TS_ALIGN_STAT_A_FOURBYTE_ALIGNED_M[] = {1, 56};
int LEFT_TS_ALIGN_STAT_A_FOURBYTE_ALIGNED_P[] = {1, 57};
//   Name:        ts_align_stat_b
int LEFT_TS_ALIGN_STAT_B = 0x140109;
int LEFT_TS_ALIGN_STAT_B_LANE0_MUXSEL_M[] = {6, 5};
int LEFT_TS_ALIGN_STAT_B_LANE0_MUXSEL_P[] = {6, 11};
int LEFT_TS_ALIGN_STAT_B_LANE1_MUXSEL_M[] = {6, 17};
int LEFT_TS_ALIGN_STAT_B_LANE1_MUXSEL_P[] = {6, 23};
int LEFT_TS_ALIGN_STAT_B_LANE2_MUXSEL_M[] = {6, 29};
int LEFT_TS_ALIGN_STAT_B_LANE2_MUXSEL_P[] = {6, 35};
int LEFT_TS_ALIGN_STAT_B_LANE3_MUXSEL_M[] = {6, 41};
int LEFT_TS_ALIGN_STAT_B_LANE3_MUXSEL_P[] = {6, 47};
int LEFT_TS_ALIGN_STAT_B_BYTE_ALIGNED_M[] = {4, 51};
int LEFT_TS_ALIGN_STAT_B_BYTE_ALIGNED_P[] = {4, 55};
int LEFT_TS_ALIGN_STAT_B_FOURBYTE_ALIGNED_M[] = {1, 56};
int LEFT_TS_ALIGN_STAT_B_FOURBYTE_ALIGNED_P[] = {1, 57};
//   Name:        ts_align_stat_c
int LEFT_TS_ALIGN_STAT_C = 0x140110;
int LEFT_TS_ALIGN_STAT_C_LANE0_MUXSEL_M[] = {6, 5};
int LEFT_TS_ALIGN_STAT_C_LANE0_MUXSEL_P[] = {6, 11};
int LEFT_TS_ALIGN_STAT_C_LANE1_MUXSEL_M[] = {6, 17};
int LEFT_TS_ALIGN_STAT_C_LANE1_MUXSEL_P[] = {6, 23};
int LEFT_TS_ALIGN_STAT_C_LANE2_MUXSEL_M[] = {6, 29};
int LEFT_TS_ALIGN_STAT_C_LANE2_MUXSEL_P[] = {6, 35};
int LEFT_TS_ALIGN_STAT_C_LANE3_MUXSEL_M[] = {6, 41};
int LEFT_TS_ALIGN_STAT_C_LANE3_MUXSEL_P[] = {6, 47};
int LEFT_TS_ALIGN_STAT_C_BYTE_ALIGNED_M[] = {4, 51};
int LEFT_TS_ALIGN_STAT_C_BYTE_ALIGNED_P[] = {4, 55};
int LEFT_TS_ALIGN_STAT_C_FOURBYTE_ALIGNED_M[] = {1, 56};
int LEFT_TS_ALIGN_STAT_C_FOURBYTE_ALIGNED_P[] = {1, 57};
//   Name:        ts_hss_pll_stat
int LEFT_TS_HSS_PLL_STAT = 0x140114;
int LEFT_TS_HSS_PLL_STAT_A_PLLA_LOCKED[] = {1, 48};
int LEFT_TS_HSS_PLL_STAT_B_PLLA_LOCKED[] = {1, 49};
int LEFT_TS_HSS_PLL_STAT_C_PLLA_LOCKED[] = {1, 50};
//   Name:        ts_hss_lane_stat
int LEFT_TS_HSS_LANE_STAT = 0x140117;
int LEFT_TS_HSS_LANE_STAT_LANE_READY_A_M[] = {4, 35};
int LEFT_TS_HSS_LANE_STAT_LANE_READY_A_P[] = {4, 39};
int LEFT_TS_HSS_LANE_STAT_LANE_READY_B_M[] = {4, 43};
int LEFT_TS_HSS_LANE_STAT_LANE_READY_B_P[] = {4, 47};
int LEFT_TS_HSS_LANE_STAT_LANE_READY_C_M[] = {4, 51};
int LEFT_TS_HSS_LANE_STAT_LANE_READY_C_P[] = {4, 55};
//   Name:        prbs_ctl_a
int LEFT_PRBS_CTL_A = 0x140120;
int LEFT_PRBS_CTL_A_TX_PRBS_EN_M[] = {4, 3};
int LEFT_PRBS_CTL_A_TX_PRBS_EN_P[] = {4, 7};
int LEFT_PRBS_CTL_A_RX_PRBS_EN_M[] = {4, 11};
int LEFT_PRBS_CTL_A_RX_PRBS_EN_P[] = {4, 15};
int LEFT_PRBS_CTL_A_PRBS_SEL_M[] = {3, 22};
int LEFT_PRBS_CTL_A_PRBS_SEL_P[] = {3, 25};
int LEFT_PRBS_CTL_A_PRBS_RST_M[] = {4, 29};
int LEFT_PRBS_CTL_A_PRBS_RST_P[] = {4, 33};
//   Name:        prbs_ctl_b
int LEFT_PRBS_CTL_B = 0x140121;
int LEFT_PRBS_CTL_B_TX_PRBS_EN_M[] = {4, 3};
int LEFT_PRBS_CTL_B_TX_PRBS_EN_P[] = {4, 7};
int LEFT_PRBS_CTL_B_RX_PRBS_EN_M[] = {4, 11};
int LEFT_PRBS_CTL_B_RX_PRBS_EN_P[] = {4, 15};
int LEFT_PRBS_CTL_B_PRBS_SEL_M[] = {3, 22};
int LEFT_PRBS_CTL_B_PRBS_SEL_P[] = {3, 25};
int LEFT_PRBS_CTL_B_PRBS_RST_M[] = {4, 29};
int LEFT_PRBS_CTL_B_PRBS_RST_P[] = {4, 33};
//   Name:        prbs_ctl_c
int LEFT_PRBS_CTL_C = 0x140122;
int LEFT_PRBS_CTL_C_TX_PRBS_EN_M[] = {4, 3};
int LEFT_PRBS_CTL_C_TX_PRBS_EN_P[] = {4, 7};
int LEFT_PRBS_CTL_C_RX_PRBS_EN_M[] = {4, 11};
int LEFT_PRBS_CTL_C_RX_PRBS_EN_P[] = {4, 15};
int LEFT_PRBS_CTL_C_PRBS_SEL_M[] = {3, 22};
int LEFT_PRBS_CTL_C_PRBS_SEL_P[] = {3, 25};
int LEFT_PRBS_CTL_C_PRBS_RST_M[] = {4, 29};
int LEFT_PRBS_CTL_C_PRBS_RST_P[] = {4, 33};
//   Name:        prbs_stat_a
int LEFT_PRBS_STAT_A = 0x140126;
int LEFT_PRBS_STAT_A_SYNC_M[] = {4, 3};
int LEFT_PRBS_STAT_A_SYNC_P[] = {4, 7};
int LEFT_PRBS_STAT_A_ERROR_M[] = {4, 11};
int LEFT_PRBS_STAT_A_ERROR_P[] = {4, 15};
//   Name:        prbs_stat_b
int LEFT_PRBS_STAT_B = 0x140127;
int LEFT_PRBS_STAT_B_SYNC_M[] = {4, 3};
int LEFT_PRBS_STAT_B_SYNC_P[] = {4, 7};
int LEFT_PRBS_STAT_B_ERROR_M[] = {4, 11};
int LEFT_PRBS_STAT_B_ERROR_P[] = {4, 15};
//   Name:        prbs_stat_c
int LEFT_PRBS_STAT_C = 0x140128;
int LEFT_PRBS_STAT_C_SYNC_M[] = {4, 3};
int LEFT_PRBS_STAT_C_SYNC_P[] = {4, 7};
int LEFT_PRBS_STAT_C_ERROR_M[] = {4, 11};
int LEFT_PRBS_STAT_C_ERROR_P[] = {4, 15};
//   Name:        error_inject_timer_val
int LEFT_ERROR_INJECT_TIMER_VAL = 0x140132;
int LEFT_ERROR_INJECT_TIMER_VAL_RND[] = {16, 31};
int LEFT_ERROR_INJECT_TIMER_VAL_COUNT[] = {32, 63};
//   Name:        error_inject_limit
int LEFT_ERROR_INJECT_LIMIT = 0x140133;
int LEFT_ERROR_INJECT_LIMIT_RND_MASK[] = {16, 31};
int LEFT_ERROR_INJECT_LIMIT_LOW_LIMIT[] = {32, 63};
//   Name:        error_inject_control
int LEFT_ERROR_INJECT_CONTROL = 0x140134;
int LEFT_ERROR_INJECT_CONTROL_ENABLE[] = {1, 0};
int LEFT_ERROR_INJECT_CONTROL_LINK_SELECT[] = {6, 6};
int LEFT_ERROR_INJECT_CONTROL_MASK[] = {32, 63};
//   Name:        interrupt_internal_error_sw_info
int LEFT_INTERRUPT_INTERNAL_ERROR_SW_INFO = 0x140157;
int LEFT_INTERRUPT_INTERNAL_ERROR_SW_INFO_READ[] = {1, 0};
int LEFT_INTERRUPT_INTERNAL_ERROR_SW_INFO_WRITE[] = {1, 1};
int LEFT_INTERRUPT_INTERNAL_ERROR_SW_INFO_PRIVILEGED[] = {1, 2};
int LEFT_INTERRUPT_INTERNAL_ERROR_SW_INFO_TEST_INT[] = {1, 3};
int LEFT_INTERRUPT_INTERNAL_ERROR_SW_INFO_ADDRESS[] = {24, 63};
//   Name:        interrupt_internal_error_hw_info
int LEFT_INTERRUPT_INTERNAL_ERROR_HW_INFO = 0x140158;
int LEFT_INTERRUPT_INTERNAL_ERROR_HW_INFO_ADDRESS_PARITY_GOOD[] = {3, 6};
int LEFT_INTERRUPT_INTERNAL_ERROR_HW_INFO_DATA_PARITY_GOOD[] = {8, 14};
int LEFT_INTERRUPT_INTERNAL_ERROR_HW_INFO_ADDRESS[] = {24, 63};
//   Name:        interrupt_internal_error_data_info
int LEFT_INTERRUPT_INTERNAL_ERROR_DATA_INFO = 0x140159;
int LEFT_INTERRUPT_INTERNAL_ERROR_DATA_INFO_DATA[] = {64, 63};
// Interrupt State Register: serdes_interrupt_state
//   State Permission: Read: All; Write: Privileged and TestInt
//   First Permission: All
//   Force Permission: Privileged and TestInt
int LEFT_SERDES_INTERRUPT_STATE_STATE = 0x140135;
int LEFT_SERDES_INTERRUPT_STATE_FIRST = 0x140139;
int LEFT_SERDES_INTERRUPT_STATE_FORCE = 0x140138;
int LEFT_SERDES_INTERRUPT_STATE_MACHINE_CHECK = 0x14013a;
int LEFT_SERDES_INTERRUPT_STATE_CRITICAL = 0x14013b;
int LEFT_SERDES_INTERRUPT_STATE_NONCRITICAL = 0x14013c;
int LEFT_SERDES_INTERRUPT_STATE_LOCAL_RING[] = {1, 0};
int LEFT_SERDES_INTERRUPT_STATE_A_PLLA_LOCK_LOST[] = {1, 1};
int LEFT_SERDES_INTERRUPT_STATE_B_PLLA_LOCK_LOST[] = {1, 3};
int LEFT_SERDES_INTERRUPT_STATE_C_PLLA_LOCK_LOST[] = {1, 4};
// Interrupt State Register: interrupt_internal_error
//   State Permission: Read: All; Write: Privileged and TestInt
//   First Permission: All
//   Force Permission: Privileged and TestInt
int LEFT_INTERRUPT_INTERNAL_ERROR_STATE = 0x140150;
int LEFT_INTERRUPT_INTERNAL_ERROR_FIRST = 0x140153;
int LEFT_INTERRUPT_INTERNAL_ERROR_FORCE = 0x140152;
int LEFT_INTERRUPT_INTERNAL_ERROR_MACHINE_CHECK = 0x140154;
int LEFT_INTERRUPT_INTERNAL_ERROR_CRITICAL = 0x140155;
int LEFT_INTERRUPT_INTERNAL_ERROR_NONCRITICAL = 0x140156;
int LEFT_INTERRUPT_INTERNAL_ERROR_BAD_ADDRESS[] = {1, 0};
int LEFT_INTERRUPT_INTERNAL_ERROR_BAD_PRIV[] = {1, 1};
int LEFT_INTERRUPT_INTERNAL_ERROR_BAD_DATA_PARITY[] = {1, 2};
int LEFT_INTERRUPT_INTERNAL_ERROR_BAD_ADDRESS_PARITY[] = {1, 3};
// Interrupt Control Register: serdes_interrupt_state_control_low
int LEFT_SERDES_INTERRUPT_STATE_CONTROL_LOW = 0x140136;
// Interrupt Control Register: serdes_interrupt_state_control_high
int LEFT_SERDES_INTERRUPT_STATE_CONTROL_HIGH = 0x140137;
int LEFT_SERDES_INTERRUPT_STATE_CONTROL_HIGH_LOCAL_RING[] = {2, 1};
int LEFT_SERDES_INTERRUPT_STATE_CONTROL_HIGH_A_PLLA_LOCK_LOST[] = {2, 3};
int LEFT_SERDES_INTERRUPT_STATE_CONTROL_HIGH_B_PLLA_LOCK_LOST[] = {2, 7};
int LEFT_SERDES_INTERRUPT_STATE_CONTROL_HIGH_C_PLLA_LOCK_LOST[] = {2, 9};
// Interrupt Control Register: interrupt_internal_error_control_high
int LEFT_INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH = 0x140151;
int LEFT_INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH_BAD_ADDRESS[] = {2, 1};
int LEFT_INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH_BAD_PRIV[] = {2, 3};
int LEFT_INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH_BAD_DATA_PARITY[] = {2, 5};
int LEFT_INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH_BAD_ADDRESS_PARITY[] = {2, 7};
