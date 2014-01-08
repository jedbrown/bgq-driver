/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */


//************************************************************************
// WARNING: This is an script generated file - do not manually change.
//   The file is generated from bgpm/events/Events-*.xsl
//   based on the data in Events.xml and Events-map.xml and bgpm_event_tips.htpl
//
// make events
//   generates the bgpm_events* files and copies to appropriate directories for
//   manual SVN merge.
//
//************************************************************************


    
    
#ifndef _UPCI_EVENTS_H_
#define _UPCI_EVENTS_H_
#include <hwi/include/common/compiler_support.h>

__BEGIN_DECLS

//! \addtogroup upci_spi 
//@{
/**
 *
 * \file
 *
 * \brief UPCI Event Ids
 *
 * The enumerated list of all UPCI Event Ids.  
 *
 * \note: This file is originally generated from 
 *     bgpm/events/Events-CrtEnums.xsl and
 *     bgpm/events/Events.xml and bgpm/events/Events-Map.xml
 *     requiring an XSLT 2.0 processor.
 *
 *
 */
//@}

/** UPCI Event Ids
 */
enum eUpci_EventIds {
  
    
    // Punit Events
    PEVT_UNDEF                               = 0,  //!< undefined
    PEVT_AXU_INSTR_COMMIT                    = 1,  //!< AXU Instruction Committed
    PEVT_AXU_CR_COMMIT                       = 2,  //!< AXU CR Instruction Committed
    PEVT_AXU_IDLE                            = 3,  //!< AXU Idle
    PEVT_AXU_FP_DS_ACTIVE                    = 4,  //!< AXU FP Divide or Square root in progress
    PEVT_AXU_FP_DS_ACTIVE_CYC                = 5,  //!< AXU FP Divide or Square root in progress cycles
    PEVT_AXU_DENORM_FLUSH                    = 6,  //!< AXU Denormal Operand flush
    PEVT_AXU_UCODE_OPS_COMMIT                = 7,  //!< AXU uCode Operations Committed
    PEVT_AXU_FP_EXCEPT                       = 8,  //!< AXU Floating Point Exception
    PEVT_AXU_FP_ENAB_EXCEPT                  = 9,  //!< AXU Floating Point Enabled Exception
    PEVT_IU_IL1_MISS                         = 10,  //!< IL1 Miss
    PEVT_IU_IL1_MISS_CYC                     = 11,  //!< IL1 Miss cycles
    PEVT_IU_IL1_RELOADS_DROPPED              = 12,  //!< IL1 Reloads Dropped
    PEVT_IU_RELOAD_COLLISIONS                = 13,  //!< Reload Collisions
    PEVT_IU_RELOAD_COLLISIONS_CYC            = 14,  //!< Reload Collisions cycles
    PEVT_IU_IU0_REDIR_CYC                    = 15,  //!< IU0 Redirected cycles
    PEVT_IU_IERAT_MISS                       = 16,  //!< IERAT Miss
    PEVT_IU_IERAT_MISS_CYC                   = 17,  //!< IERAT Miss cycles
    PEVT_IU_ICACHE_FETCH                     = 18,  //!< ICache Fetch
    PEVT_IU_ICACHE_FETCH_CYC                 = 19,  //!< ICache Fetch cycles
    PEVT_IU_INSTR_FETCHED                    = 20,  //!< Instructions Fetched
    PEVT_IU_INSTR_FETCHED_CYC                = 21,  //!< Instructions Fetched cycles
    PEVT_IU_RSV_ANY_L2_BACK_INV              = 22,  //!< reserved any L2 Back Invalidates
    PEVT_IU_RSV_ANY_L2_BACK_INV_CYC          = 23,  //!< reserved any L2 Back Invalidates cycles
    PEVT_IU_L2_BACK_INV_HITS                 = 24,  //!< L2 Back Invalidates - Hits
    PEVT_IU_L2_BACK_INV_HITS_CYC             = 25,  //!< L2 Back Invalidates - Hits cycles
    PEVT_IU_IBUFF_EMPTY                      = 26,  //!< IBuff Empty
    PEVT_IU_IBUFF_EMPTY_CYC                  = 27,  //!< IBuff Empty cycles
    PEVT_IU_IBUFF_FLUSH                      = 28,  //!< IBuff Flush
    PEVT_IU_IBUFF_FLUSH_CYC                  = 29,  //!< IBuff Flush cycles
    PEVT_IU_IS1_STALL_CYC                    = 30,  //!< IU5 Stage Register Dependency Stall
    PEVT_IU_IS2_STALL_CYC                    = 31,  //!< IU6 Instruction Issue Stall
    PEVT_IU_BARRIER_OP_STALL_CYC             = 32,  //!< Barrier Op Stall
    PEVT_IU_SLOW_SPR_ACCESS_CYC              = 33,  //!< Slow SPR Access
    PEVT_IU_RAW_DEP_HIT_CYC                  = 34,  //!< RAW Dep Hit
    PEVT_IU_WAW_DEP_HIT_CYC                  = 35,  //!< WAW Dep Hit
    PEVT_IU_SYNC_DEP_HIT_CYC                 = 36,  //!< Sync Dep Hit
    PEVT_IU_SPR_DEP_HIT_CYC                  = 37,  //!< SPR Dep Hit
    PEVT_IU_AXU_DEP_HIT_CYC                  = 38,  //!< AXU Dep Hit
    PEVT_IU_FXU_DEP_HIT_CYC                  = 39,  //!< FXU Dep Hit
    PEVT_IU_AXU_FXU_DEP_HIT_CYC              = 40,  //!< Cycles the FP unit is stalled
    PEVT_IU_AXU_ISSUE_PRI_LOSS_CYC           = 41,  //!< AXU Issue Priority Loss
    PEVT_IU_FXU_ISSUE_PRI_LOSS_CYC           = 42,  //!< FXU Issue Priority Loss
    PEVT_IU_FXU_ISSUE_COUNT                  = 43,  //!< FXU Issue Count
    PEVT_IU_TOT_ISSUE_COUNT                  = 44,  //!< Total Issue Count
    PEVT_XU_PROC_BUSY                        = 45,  //!< Processor Busy
    PEVT_XU_BR_COMMIT_CORE                   = 46,  //!< Branch Commit
    PEVT_XU_BR_MISPRED_COMMIT_CORE           = 47,  //!< Branch Mispredict Commit
    PEVT_XU_BR_TARG_ADDR_MISPRED_COMMIT_CORE = 48,  //!< Branch Target Address Mispredict Commit
    PEVT_XU_THRD_RUNNING                     = 49,  //!< Thread Running
    PEVT_XU_TIMEBASE_TICK                    = 50,  //!< Timebase Tick
    PEVT_XU_SPR_READ_COMMIT                  = 51,  //!< SPR Read Commit
    PEVT_XU_SPR_WRITE_COMMIT                 = 52,  //!< SPR Write Commit
    PEVT_XU_STALLED_ON_WAITRSV               = 53,  //!< Cycles stalled on waitrsv
    PEVT_XU_STALLED_ON_WAITRSV_CYC           = 54,  //!< Cycles stalled on waitrsv cycles
    PEVT_XU_EXT_INT_ASSERT                   = 55,  //!< External Interrupt Asserted
    PEVT_XU_CRIT_EXT_INT_ASSERT              = 56,  //!< Critical External Interrupt Asserted
    PEVT_XU_PERF_MON_INT_ASSERT              = 57,  //!< Performance Monitor Interrupt Asserted
    PEVT_XU_PPC_COMMIT                       = 58,  //!< PPC Commit
    PEVT_XU_COMMIT                           = 59,  //!< XU Operations Committed
    PEVT_XU_UCODE_COMMIT                     = 60,  //!< uCode Sequence Commit
    PEVT_XU_ANY_FLUSH                        = 61,  //!< Any Flush
    PEVT_XU_BR_COMMIT                        = 62,  //!< Branch Commit
    PEVT_XU_BR_MISPRED_COMMIT                = 63,  //!< Branch Mispredict Commit
    PEVT_XU_BR_TAKEN_COMMIT                  = 64,  //!< Branch Taken Commit
    PEVT_XU_BR_TARG_ADDR_MISPRED_COMMIT      = 65,  //!< Branch Target Address Mispredict Commit
    PEVT_XU_MULT_DIV_COLLISION               = 66,  //!< Mult/Div Collision
    PEVT_XU_EXT_INT_PEND                     = 67,  //!< External Interrupt Pending
    PEVT_XU_CRIT_EXT_INT_PEND                = 68,  //!< Critical External Interrupt Pending
    PEVT_XU_PERF_MON_INT_PEND                = 69,  //!< Performance Monitor Interrupt Pending
    PEVT_XU_RUN_INSTR                        = 70,  //!< Run Instructions
    PEVT_XU_INTS_TAKEN                       = 71,  //!< Interrupts Taken
    PEVT_XU_EXT_INT_TAKEN                    = 72,  //!< External Interrupt Taken
    PEVT_XU_CRIT_EXT_INT_TAKEN               = 73,  //!< Critical External Interrupt Taken
    PEVT_XU_PERF_MON_INT_TAKEN               = 74,  //!< Performance Monitor Interrupt Taken
    PEVT_XU_DOORBELL_INT_TAKEN               = 75,  //!< Doorbell Interrupt Taken
    PEVT_XU_STCX_FAIL                        = 76,  //!< stcx fail
    PEVT_XU_ICSWX_FAILED                     = 77,  //!< icswx failed
    PEVT_XU_ICSWX_COMMIT                     = 78,  //!< icswx commit
    PEVT_XU_MULT_DIV_BUSY                    = 79,  //!< Mult/Div Busy
    PEVT_LSU_COMMIT_STS                      = 80,  //!< Committed Stores
    PEVT_LSU_COMMIT_ST_MISSES                = 81,  //!< Committed Store Misses
    PEVT_LSU_COMMIT_LD_MISSES                = 82,  //!< Committed Load Misses
    PEVT_LSU_COMMIT_CACHE_INHIB_LD_MISSES    = 83,  //!< Committed Cache-Inhibited Load Misses
    PEVT_LSU_COMMIT_CACHEABLE_LDS            = 84,  //!< Committed Cacheable Loads
    PEVT_LSU_COMMIT_DCBT_MISSES              = 85,  //!< Committed DCBT Misses
    PEVT_LSU_COMMIT_DCBT_HITS                = 86,  //!< Committed DCBT Hits
    PEVT_LSU_COMMIT_AXU_LDS                  = 87,  //!< Committed AXU Loads
    PEVT_LSU_COMMIT_AXU_STS                  = 88,  //!< Committed AXU Stores
    PEVT_LSU_COMMIT_STCX                     = 89,  //!< Committed STCX
    PEVT_LSU_COMMIT_WCLR                     = 90,  //!< Committed WCLR
    PEVT_LSU_COMMIT_WCLR_WL                  = 91,  //!< Committed WCLR Watchlost
    PEVT_LSU_COMMIT_LDAWX                    = 92,  //!< Committed LDAWX
    PEVT_LSU_UNSUPPORTED_ALIGNMENT_FLUSH     = 93,  //!< Unsupported Alignment Flush
    PEVT_LSU_RELOAD_RESRC_CONFLICT_FLUSH     = 94,  //!< Reload Resource Conflict Flush
    PEVT_LSU_COMMIT_DUPLICATE_LDAWX          = 95,  //!< Committed Duplicate LDAWX
    PEVT_LSU_INTER_THRD_DIR_ACCESS_FLUSH     = 96,  //!< Inter-Thread Directory Access Flush
    PEVT_LSU_LMQ_DEPENDENCY_FLUSH            = 97,  //!< Load Miss Queue Dependency Flush
    PEVT_LSU_COMMIT_WCHKALL                  = 98,  //!< Committed WCHKALL
    PEVT_LSU_COMMIT_SUCC_WCHKALL             = 99,  //!< Committed Successful WCHKALL
    PEVT_LSU_LD_MISS_Q_FULL_FLUSH            = 100,  //!< Load Miss Queue Full Flush
    PEVT_LSU_ST_Q_FULL_FLUSH                 = 101,  //!< Store Queue Full Flush
    PEVT_LSU_HIT_LD_FLUSH                    = 102,  //!< Hit Against Outstanding Load Flush
    PEVT_LSU_HIT_IG1_REQ_FLUSH               = 103,  //!< Hit Against Outstanding I=G=1 Request Flush
    PEVT_LSU_LARX_FINISHED                   = 104,  //!< LARX Finished
    PEVT_LSU_INTER_THRD_ST_WATCH_LOST        = 105,  //!< Inter-Thread Store Set Watch Lost Indicator
    PEVT_LSU_RELOAD_WATCH_LOST               = 106,  //!< Reload Set Watch Lost Indicator
    PEVT_LSU_BACK_INV_WATCH_LOST             = 107,  //!< Back-Invalidate Set Watch Lost Indicator
    PEVT_LSU_L1_DCACHE_BACK_INVAL            = 108,  //!< L1 Data Cache Back-Invalidate
    PEVT_LSU_L1_DCACHE_BACK_INVAL_HITS       = 109,  //!< L1 Data Cache Back-Invalidate Hits
    PEVT_LSU_L1_CACHE_PTYERR_DETECTED        = 110,  //!< L1 Cache Parity Error Detected
    PEVT_LSU_LD_LAT_MEM_SUBSYS_CYC           = 111,  //!< (event is broken) Load Latency memory subsystem
    PEVT_MMU_TLB_HIT_DIRECT_IERAT            = 112,  //!< TLB hit direct after ierat miss
    PEVT_MMU_TLB_MISS_DIRECT_IERAT           = 113,  //!< TLB miss direct after ierat miss
    PEVT_MMU_TLB_MISS_INDIR_IERAT            = 114,  //!< TLB miss indirect after ierat miss
    PEVT_MMU_HTW_HIT_IERAT                   = 115,  //!< Hardware Table walk hit after ierat miss
    PEVT_MMU_HTW_MISS_IERAT                  = 116,  //!< Hardware Table walk miss after ierat miss
    PEVT_MMU_TLB_HIT_DIRECT_DERAT            = 117,  //!< TLB hit direct after derat miss
    PEVT_MMU_TLB_MISS_DIRECT_DERAT           = 118,  //!< TLB miss direct after derat miss
    PEVT_MMU_TLB_MISS_INDIR_DERAT            = 119,  //!< TLB miss indirect after derat miss
    PEVT_MMU_HTW_HIT_DERAT                   = 120,  //!< Hardware Table walk hit after derat miss
    PEVT_MMU_HTW_MISS_DERAT                  = 121,  //!< Hardware Table walk miss after derat miss
    PEVT_MMU_IERAT_MISS                      = 122,  //!< Ierat miss
    PEVT_MMU_IERAT_MISS_CYC                  = 123,  //!< Ierat miss cycles
    PEVT_MMU_DERAT_MISS                      = 124,  //!< Derat miss
    PEVT_MMU_DERAT_MISS_CYC                  = 125,  //!< Derat miss cycles
    PEVT_MMU_IERAT_MISS_TOT                  = 126,  //!< Ierat miss total
    PEVT_MMU_DERAT_MISS_TOT                  = 127,  //!< derat miss total
    PEVT_MMU_TLB_MISS_DIRECT_TOT             = 128,  //!< tlb miss direct total
    PEVT_MMU_TLB_HIT_FIRSTSIZE_TOT           = 129,  //!< tlb hit firstsize total
    PEVT_MMU_TLB_HIT_INDIR_TOT               = 130,  //!< tlb hit indirect total
    PEVT_MMU_HTW_PTERELOAD_TOT               = 131,  //!< htw ptereload total
    PEVT_MMU_LRAT_TRANS_TOT                  = 132,  //!< lrat translation total
    PEVT_MMU_LRAT_MISS_TOT                   = 133,  //!< lrat miss total
    PEVT_MMU_PT_FAULT_TOT                    = 134,  //!< pt fault total
    PEVT_MMU_PT_INELIG_TOT                   = 135,  //!< pt inelig total
    PEVT_MMU_TLBWEC_FAIL_TOT                 = 136,  //!< tlbwec fail total
    PEVT_MMU_TLBWEC_SUCC_TOT                 = 137,  //!< tlbwec success total
    PEVT_MMU_TLBILX_SRC_TOT                  = 138,  //!< tlbilx local source total
    PEVT_MMU_TLBIVAX_SRC_TOT                 = 139,  //!< tlbivax local source total
    PEVT_MMU_TLBIVAX_SNOOP_TOT               = 140,  //!< tlbivax snoop total
    PEVT_MMU_TLB_FLUSH_REQ_TOT               = 141,  //!< tlb flush req total
    PEVT_MMU_TLB_FLUSH_REQ_TOT_CYC           = 142,  //!< tlb flush req total cycles
    PEVT_L1P_BAS_STRM_LINE_ESTB              = 143,  //!< L1p stream line establish
    PEVT_L1P_BAS_HIT                         = 144,  //!< L1p hit 
    PEVT_L1P_BAS_PF2DFC                      = 145,  //!< L1p pf2dfc
    PEVT_L1P_BAS_MISS                        = 146,  //!< L1p miss
    PEVT_L1P_BAS_LU_DRAIN                    = 147,  //!< L1p LU drain
    PEVT_L1P_BAS_LU_DRAIN_CYC                = 148,  //!< L1p LU drain Cyc
    PEVT_L1P_BAS_LD                          = 149,  //!< L1p Load
    PEVT_L1P_BAS_ST_WC                       = 150,  //!< L1p Store-WC
    PEVT_L1P_BAS_ST_32BYTE                   = 151,  //!< L1p Store 32byte
    PEVT_L1P_BAS_ST_CRACKED                  = 152,  //!< L1p Store-cracked
    PEVT_L1P_BAS_LU_STALL_SRT                = 153,  //!< L1p LU stall-SRT
    PEVT_L1P_BAS_LU_STALL_SRT_CYC            = 154,  //!< L1p LU stall-SRT Cyc
    PEVT_L1P_BAS_LU_STALL_MMIO_DCR           = 155,  //!< L1p LU stall-MMIO/DCR
    PEVT_L1P_BAS_LU_STALL_MMIO_DCR_CYC       = 156,  //!< L1p LU stall-MMIO/DCR Cyc
    PEVT_L1P_BAS_LU_STALL_STRM_DET           = 157,  //!< L1p LU stall-stream detect
    PEVT_L1P_BAS_LU_STALL_STRM_DET_CYC       = 158,  //!< L1p LU stall-stream detect Cyc
    PEVT_L1P_BAS_LU_STALL_LIST_RD            = 159,  //!< L1p LU stall-list read
    PEVT_L1P_BAS_LU_STALL_LIST_RD_CYC        = 160,  //!< L1p LU stall-list read Cyc
    PEVT_L1P_BAS_ST                          = 161,  //!< L1p Store
    PEVT_L1P_BAS_LU_STALL_LIST_WRT           = 162,  //!< L1p LU stall-list write
    PEVT_L1P_BAS_LU_STALL_LIST_WRT_CYC       = 163,  //!< L1p LU stall-list write Cyc
    PEVT_L1P_SW_MAS_SW_REQ_VAL               = 164,  //!< L1p switch master sw req valid
    PEVT_L1P_SW_MAS_SW_REQ_GATE              = 165,  //!< L1p switch master sw req gate
    PEVT_L1P_SW_MAS_SW_DATA_GATE             = 166,  //!< L1p switch master sw data gate
    PEVT_L1P_SW_SR_MAS_RD_VAL_2              = 167,  //!< L1p switch sr master rd valid 2 
    PEVT_L1P_SW_SI_MAS_REQ_VAL_2             = 168,  //!< L1p switch si master req valid 2
    PEVT_L1P_SW_SW_MAS_SKED_VAL_2            = 169,  //!< L1p switch sw master sked valid 2
    PEVT_L1P_SW_SW_MAS_EAGER_VAL_2           = 170,  //!< L1p switch sw master eager valid 2
    PEVT_L1P_SW_TLB_FILL                     = 171,  //!< L1p switch tlb fill
    PEVT_L1P_SW_STWCX_FAIL                   = 172,  //!< L1p switch stwcx fail
    PEVT_L1P_SW_STWCX                        = 173,  //!< L1p switch stwcx
    PEVT_L1P_SW_I_FETCH                      = 174,  //!< L1p switch I-fetch
    PEVT_L1P_SW_MSYNC                        = 175,  //!< L1p switch Msync
    PEVT_L1P_SW_LWARX                        = 176,  //!< L1p switch Lwarx
    PEVT_L1P_SW_KILL_L2_RSV                  = 177,  //!< L1p switch L1p killed L2 slice reserve
    PEVT_L1P_SW_L2_CANCEL_A2_RSV             = 178,  //!< L1p switch L2 slice cancelled A2 reserve
    PEVT_L1P_SW_L1_INVAL                     = 179,  //!< L1p switch L1 inval
    PEVT_L1P_SW_WC_EVICT_ADDR                = 180,  //!< L1p switch WC evict by nearby address
    PEVT_L1P_STRM_LINE_ESTB                  = 181,  //!< L1p stream L1p single line established
    PEVT_L1P_STRM_HIT_FWD                    = 182,  //!< L1p stream L1p hit forwarded
    PEVT_L1P_STRM_L1_HIT_FWD                 = 183,  //!< L1p stream L1 hit forwarded
    PEVT_L1P_STRM_EVICT_UNUSED               = 184,  //!< L1p stream L1p evict unused
    PEVT_L1P_STRM_EVICT_PART_USED            = 185,  //!< L1p stream L1p evict partially used
    PEVT_L1P_STRM_REMOTE_INVAL_MATCH         = 186,  //!< L1p stream L1p Remote Invalidate match
    PEVT_L1P_STRM_DONT_CACHE                 = 187,  //!< L1p stream L1p don't cache
    PEVT_L1P_STRM_STRM_DEPTH_STEAL           = 188,  //!< L1p stream L1p Stream depth steal
    PEVT_L1P_STRM_STRM_ESTB                  = 189,  //!< L1p stream L1p streams established
    PEVT_L1P_STRM_WRT_INVAL                  = 190,  //!< L1p stream L1p Write Invalidate
    PEVT_L1P_STRM_LINE_ESTB_ALL_LIST         = 191,  //!< L1p stream L1p line establish (all list)
    PEVT_L1P_STRM_HIT_LIST                   = 192,  //!< L1p stream L1p hit (list)
    PEVT_L1P_STRM_PF2DFC_LIST                = 193,  //!< L1p stream L1p pf2dfc (list)
    PEVT_L1P_STRM_PART_INVAL_REFCH           = 194,  //!< L1p stream L1p partial invalid refetched
    PEVT_L1P_LIST_SKIP_1                     = 195,  //!< L1p list skip by 1
    PEVT_L1P_LIST_SKIP_2                     = 196,  //!< L1p list skip by 2
    PEVT_L1P_LIST_SKIP_3                     = 197,  //!< L1p list skip by 3
    PEVT_L1P_LIST_SKIP_4                     = 198,  //!< L1p list skip by 4
    PEVT_L1P_LIST_SKIP_5                     = 199,  //!< L1p list skip by 5
    PEVT_L1P_LIST_SKIP_6                     = 200,  //!< L1p list skip by 6
    PEVT_L1P_LIST_SKIP_7                     = 201,  //!< L1p list skip by 7
    PEVT_L1P_LIST_ABANDON                    = 202,  //!< L1p list abandoned
    PEVT_L1P_LIST_CMP                        = 203,  //!< L1p list comparison
    PEVT_L1P_LIST_SKIP                       = 204,  //!< L1p list skip
    PEVT_L1P_LIST_MISMATCH                   = 205,  //!< L1p list mismatch 
    PEVT_L1P_LIST_STARTED                    = 206,  //!< L1p list started.
    PEVT_L1P_LIST_OVF_MEM                    = 207,  //!< L1p list overflow mem
    PEVT_L1P_LIST_CMP_OVRUN_PREFCH           = 208,  //!< L1p list cmp overrun prefetch
    PEVT_WAKE_HIT_10                         = 209,  //!< Wakeup hit 10
    PEVT_WAKE_HIT_11                         = 210,  //!< Wakeup hit 11
    PEVT_CYCLES                              = 211,  //!< x1 cycles
    PEVT_INST_XU_MATCH                       = 212,  //!< Match XU OpCode
    PEVT_INST_XU_GRP_MASK                    = 213,  //!< XU OpCode Mask Multiple Groups
    PEVT_INST_XU_FLD                         = 214,  //!< Floating Point Load Instr Compl
    PEVT_INST_XU_FST                         = 215,  //!< Floating Point Store Instr Compl
    PEVT_INST_XU_QLD                         = 216,  //!< Quad Floating Point Load Instr Compl
    PEVT_INST_XU_QST                         = 217,  //!< Quad Floating Point Store Instr Compl
    PEVT_INST_XU_BITS                        = 218,  //!< Bit manipulation Instr Compl
    PEVT_INST_XU_BRC                         = 219,  //!< Branch Conditional Instr Compl
    PEVT_INST_XU_BRU                         = 220,  //!< Branch Unconditional Instr Compl
    PEVT_INST_XU_CINV                        = 221,  //!< Cache Invalidate Instr Compl
    PEVT_INST_XU_CSTO                        = 222,  //!< Cache Store Instr Compl
    PEVT_INST_XU_CTCH                        = 223,  //!< Cache Touch Instr Compl
    PEVT_INST_XU_IADD                        = 224,  //!< Integer Arithmetic Instr Compl
    PEVT_INST_XU_ICMP                        = 225,  //!< Compare Instr Compl
    PEVT_INST_XU_ICSW                        = 226,  //!< Coprocessor Instr Compl
    PEVT_INST_XU_IDIV                        = 227,  //!< integer divide Instr Compl
    PEVT_INST_XU_ILOG                        = 228,  //!< Logical Instr Compl
    PEVT_INST_XU_IMOV                        = 229,  //!< Quick Register move Instr Compl
    PEVT_INST_XU_IMUL                        = 230,  //!< integer multiply Instr Compl
    PEVT_INST_XU_INT                         = 231,  //!< Interrupt/system call Instr Compl
    PEVT_INST_XU_LD                          = 232,  //!< Load Instr Compl
    PEVT_INST_XU_LSCX                        = 233,  //!< Load & Store reserve conditional Instr Compl
    PEVT_INST_XU_ST                          = 234,  //!< Store Instr Compl
    PEVT_INST_XU_SYNC                        = 235,  //!< Context Sync Instr Compl
    PEVT_INST_XU_TLB                         = 236,  //!< TLB other Instr Compl
    PEVT_INST_XU_TLBI                        = 237,  //!< TLB Invalidate Instr Compl
    PEVT_INST_XU_WCH                         = 238,  //!< Watch Instr Compl
    PEVT_INST_XU_ALL                         = 239,  //!< All XU Instruction Completions
    PEVT_INST_QFPU_MATCH                     = 240,  //!< AXU Opcode Match
    PEVT_INST_QFPU_GRP_MASK                  = 241,  //!< AXU Opcode Mask Multiple Groups
    PEVT_INST_QFPU_FOTH                      = 242,  //!< Floating Point Other Instr Compl
    PEVT_INST_QFPU_FRES                      = 243,  //!< Floating Point Reciprocal Estimate Instr Compl
    PEVT_INST_QFPU_FADD                      = 244,  //!< Floating Point Add Instr Compl
    PEVT_INST_QFPU_FCVT                      = 245,  //!< Floating Point Convert Instr Compl
    PEVT_INST_QFPU_FCMP                      = 246,  //!< Floating Point Compare Instr Compl
    PEVT_INST_QFPU_FDIV                      = 247,  //!< Floating Point Divide Instr Compl
    PEVT_INST_QFPU_FMOV                      = 248,  //!< Floating Point Move Instr Compl
    PEVT_INST_QFPU_FMA                       = 249,  //!< Floating Point Multiply-Add Instr Compl
    PEVT_INST_QFPU_FMUL                      = 250,  //!< Floating Point Multiply Instr Compl
    PEVT_INST_QFPU_FRND                      = 251,  //!< Floating Point Round Instr Compl
    PEVT_INST_QFPU_FSQE                      = 252,  //!< Floating Point Square Root Estimate Instr Compl
    PEVT_INST_QFPU_FSQ                       = 253,  //!< Floating Point Square Root Instr Compl
    PEVT_INST_QFPU_QMOV                      = 254,  //!< Quad Floating Point Move Instr Compl
    PEVT_INST_QFPU_QOTH                      = 255,  //!< Quad Floating Point Other Instr Compl
    PEVT_INST_QFPU_QADD                      = 256,  //!< Quad Floating Point Add Instr Compl
    PEVT_INST_QFPU_QMUL                      = 257,  //!< Quad Floating Point Multiply Instr Compl
    PEVT_INST_QFPU_QRES                      = 258,  //!< Quad Floating Point Reciprocal Estimate Instr Compl
    PEVT_INST_QFPU_QMA                       = 259,  //!< Quad Floating Point Multiply-Add Instr Compl
    PEVT_INST_QFPU_QRND                      = 260,  //!< Quad Floating Point Round Instr Compl
    PEVT_INST_QFPU_QCVT                      = 261,  //!< Quad Floating Point Convert Instr Compl
    PEVT_INST_QFPU_QCMP                      = 262,  //!< Quad Floating Point Compare Instr Compl
    PEVT_INST_QFPU_UCODE                     = 263,  //!< Floating Point microcoded instructions
    PEVT_INST_QFPU_ALL                       = 264,  //!< All AXU Instruction Completions
    PEVT_INST_QFPU_FPGRP1                    = 265,  //!< FP Operations Group 1 
    PEVT_INST_QFPU_FPGRP1_SINGLE             = 266,  //!< FP Operations Group 1 single FP subset
    PEVT_INST_QFPU_FPGRP1_QUAD               = 267,  //!< FP Operations Group 1 quad FP subset
    PEVT_INST_QFPU_FPGRP2                    = 268,  //!< FP Operations Group 2 
    PEVT_INST_QFPU_FPGRP2_SINGLE             = 269,  //!< FP Operations Group 2 single FP subset
    PEVT_INST_QFPU_FPGRP2_QUAD               = 270,  //!< FP Operations Group 2 quad FP subset
    PEVT_INST_QFPU_FPGRP1_INSTR              = 271,  //!< FP Operations Group 1 instr count
    PEVT_INST_QFPU_FPGRP1_SINGLE_INSTR       = 272,  //!< FP Operations Group 1 single FP subset instr count
    PEVT_INST_QFPU_FPGRP1_QUAD_INSTR         = 273,  //!< FP Operations Group 1 quad FP subset instr count
    PEVT_INST_QFPU_FPGRP2_INSTR              = 274,  //!< FP Operations Group 2 instr count
    PEVT_INST_QFPU_FPGRP2_SINGLE_INSTR       = 275,  //!< FP Operations Group 2 single FP subset instr count
    PEVT_INST_QFPU_FPGRP2_QUAD_INSTR         = 276,  //!< FP Operations Group 2 quad FP subset instr count
    PEVT_INST_ALL                            = 277,  //!< All Instruction Completions
    PEVT_PUNIT_LAST_EVENT                    = 277,

    // L2unit Events
    PEVT_L2_HITS                             = 278,  //!< L2 Hits
    PEVT_L2_MISSES                           = 279,  //!< L2 Misses
    PEVT_L2_PREFETCH                         = 280,  //!< L2 prefetch ahead of L1P prefetch
    PEVT_L2_FETCH_LINE                       = 281,  //!< L2 fetch 128 byte line from main memory
    PEVT_L2_STORE_LINE                       = 282,  //!< L2 store 128 byte line to main memory
    PEVT_L2_STORE_PARTIAL_LINE               = 283,  //!< L2 store partial line to main memory
    PEVT_L2_REQ_REJECT                       = 284,  //!< L2 Req Reject
    PEVT_L2_REQ_RETIRE                       = 285,  //!< L2 Req Retire
    PEVT_L2_REQ_MISS_AND_EVICT               = 286,  //!< L2 Req Miss and Evict
    PEVT_L2_REQ_MISS_AND_EVICT_SPEC          = 287,  //!< L2 Req Miss and Evict Spec
    PEVT_L2_MISS_AND_EVICT                   = 288,  //!< L2 Miss and Evict
    PEVT_L2_MISS_AND_EVICT_SPEC              = 289,  //!< L2 Miss and Evict Spec
    PEVT_L2_REQ_SPEC_ADDR_CLEANUP            = 290,  //!< L2 Req Spec Addr Cleanup
    PEVT_L2_SPEC_ADDR_CLEANUP                = 291,  //!< L2 Spec Addr Cleanup
    PEVT_L2_SPEC_SET_CLEANUP                 = 292,  //!< L2 Spec Set Cleanup
    PEVT_L2_SPEC_RETRY                       = 293,  //!< L2 Spec Retry
    PEVT_L2_HITS_SLICE                       = 294,  //!< L2 Cache Slice Hits 
    PEVT_L2_MISSES_SLICE                     = 295,  //!< L2 Cache Slice Misses
    PEVT_L2_PREFETCH_SLICE                   = 296,  //!< L2 Cache Slice prefetch ahead of L1P prefetch
    PEVT_L2_FETCH_LINE_SLICE                 = 297,  //!< L2 Cache Slice fetch 128 byte line from main memory
    PEVT_L2_STORE_LINE_SLICE                 = 298,  //!< L2 Cache Slice store 128 byte line to main memory
    PEVT_L2_STORE_PARTIAL_LINE_SLICE         = 299,  //!< L2 Cache Slice store partial line to main memory
    PEVT_L2_REQ_REJECT_SLICE                 = 300,  //!< L2 Cache Slice Req Reject
    PEVT_L2_REQ_RETIRE_SLICE                 = 301,  //!< L2 Cache Slice Req Retire
    PEVT_L2_REQ_MISS_AND_EVICT_SLICE         = 302,  //!< L2 Cache Slice Req Miss and Evict
    PEVT_L2_REQ_MISS_AND_EVICT_SPEC_SLICE    = 303,  //!< L2 Cache Slice Req Miss and Evict Spec
    PEVT_L2_MISS_AND_EVICT_SLICE             = 304,  //!< L2 Cache Slice Miss and Evict
    PEVT_L2_MISS_AND_EVICT_SPEC_SLICE        = 305,  //!< L2 Cache Slice Miss and Evict Spec
    PEVT_L2_REQ_SPEC_ADDR_CLEANUP_SLICE      = 306,  //!< L2 Cache Slice Req Spec Addr Cleanup
    PEVT_L2_SPEC_ADDR_CLEANUP_SLICE          = 307,  //!< L2 Cache Slice Spec Addr Cleanup
    PEVT_L2_SPEC_SET_CLEANUP_SLICE           = 308,  //!< L2 Cache Slice Spec Set Cleanup
    PEVT_L2_SPEC_RETRY_SLICE                 = 309,  //!< L2 Cache Slice Spec Retry
    PEVT_L2UNIT_LAST_EVENT                   = 309,

    // IOunit Events
    PEVT_MU_PKT_INJ                          = 310,  //!< Mu packet injected
    PEVT_MU_MSG_INJ                          = 311,  //!< Mu message injected
    PEVT_MU_FIFO_PKT_RCV                     = 312,  //!< Mu FIFO packet received
    PEVT_MU_RGET_PKT_RCV                     = 313,  //!< Mu RGET packet received
    PEVT_MU_PUT_PKT_RCV                      = 314,  //!< Mu PUT packet received
    PEVT_MU_PORT0_16B_WRT                    = 315,  //!< Mu port 0 16B write
    PEVT_MU_PORT0_32B_RD                     = 316,  //!< Mu port 0 32B read
    PEVT_MU_PORT0_64B_RD                     = 317,  //!< Mu port 0 64B read
    PEVT_MU_PORT0_128B_RD                    = 318,  //!< Mu port 0 128B read
    PEVT_MU_PORT1_16B_WRT                    = 319,  //!< Mu port 1 16B write
    PEVT_MU_PORT1_32B_RD                     = 320,  //!< Mu port 1 32B read
    PEVT_MU_PORT1_64B_RD                     = 321,  //!< Mu port 1 64B read
    PEVT_MU_PORT1_128B_RD                    = 322,  //!< Mu port 1 128B read
    PEVT_MU_PORT2_16B_WRT                    = 323,  //!< Mu port 2 16B write
    PEVT_MU_PORT2_32B_RD                     = 324,  //!< Mu port 2 32B read
    PEVT_MU_PORT2_64B_RD                     = 325,  //!< Mu port 2 64B read
    PEVT_MU_PORT2_128B_RD                    = 326,  //!< Mu port 2 128B read
    PEVT_MU_SLV_PORT_RD                      = 327,  //!< Mu Slave port read
    PEVT_MU_SLV_PORT_WRT                     = 328,  //!< Mu Slave port write
    PEVT_MU_PORT0_PEND_WRT                   = 329,  //!< Mu port 0 pending write requests
    PEVT_MU_PORT0_PEND_RD                    = 330,  //!< Mu port 0 pending read requests
    PEVT_MU_PORT1_PEND_WRT                   = 331,  //!< Mu port 1 pending write requests
    PEVT_MU_PORT1_PEND_RD                    = 332,  //!< Mu port 1 pending read requests
    PEVT_MU_PORT2_PEND_WRT                   = 333,  //!< Mu port 2 pending write requests
    PEVT_MU_PORT2_PEND_RD                    = 334,  //!< Mu port 2 pending read requests
    PEVT_PCIE_INB_RD_BYTES                   = 335,  //!< Pcie Inbound Read Bytes
    PEVT_PCIE_INB_RDS                        = 336,  //!< Pcie Inbound Read Requests
    PEVT_PCIE_INB_RD_CMPLT                   = 337,  //!< Pcie Inbound Read Completions
    PEVT_PCIE_OUTB_WRT_BYTES                 = 338,  //!< Pcie outbound memory write bytes
    PEVT_PCIE_OUTB_CFG_X                     = 339,  //!< Pcie Outbound CFG transactions
    PEVT_PCIE_OUTB_IO_X                      = 340,  //!< Pcie Outbound IO transactions
    PEVT_PCIE_RX_DLLP                        = 341,  //!< Pcie RX DLLP Count
    PEVT_PCIE_RX_TLP                         = 342,  //!< Pcie RX TLP Count
    PEVT_PCIE_RX_TLP_SIZE                    = 343,  //!< Pcie RX TLP Size in DWORDS
    PEVT_PCIE_TX_DLLP                        = 344,  //!< Pcie TX DLLP Count
    PEVT_PCIE_TX_TLP                         = 345,  //!< Pcie TX TLP Count
    PEVT_PCIE_TX_TLP_SIZE                    = 346,  //!< Pcie TX TLP Size in DWORDS
    PEVT_DB_PCIE_INB_WRT_BYTES               = 347,  //!< Devbus PCIe inbound write bytes written
    PEVT_DB_PCIE_OUTB_RD_BYTES               = 348,  //!< Devbus PCIe outbound read bytes requested
    PEVT_DB_PCIE_OUTB_RDS                    = 349,  //!< Devbus PCIe outbound read requests
    PEVT_DB_PCIE_OUTB_RD_CMPLT               = 350,  //!< Devbus PCIe outbound read completions
    PEVT_DB_BOOT_EDRAM_WRT_BYTES             = 351,  //!< Devbus Boot eDRAM bytes written
    PEVT_DB_BOOT_EDRAM_RD_BYTES              = 352,  //!< Devbus Boot eDRAM bytes read
    PEVT_DB_MSG_SIG_INTS                     = 353,  //!< Devbus Message-Signaled Interrupts (MSIs)
    PEVT_IOUNIT_LAST_EVENT                   = 353,

    // NWUnit Events
    PEVT_NW_USER_PP_SENT                     = 354,  //!< User pt-pt 32B packet chunks sent 
    PEVT_NW_USER_DYN_PP_SENT                 = 355,  //!< User dynamic pt-pt 32B packet chunks sent 
    PEVT_NW_USER_ESC_PP_SENT                 = 356,  //!< User escape pt-pt 32B packet chunks sent 
    PEVT_NW_USER_PRI_PP_SENT                 = 357,  //!< User priority pt-pt 32B packet chunks sent 
    PEVT_NW_SYS_PP_SENT                      = 358,  //!< System pt-pt 32B packet chunks sent 
    PEVT_NW_USER_WORLD_COL_SENT              = 359,  //!< User world collective 32B packet chunks sent 
    PEVT_NW_USER_SUBC_COL_SENT               = 360,  //!< User sub-comm collective 32B packet chunks sent 
    PEVT_NW_SYS_COL_SENT                     = 361,  //!< System collective 32B packet chunks sent 
    PEVT_NW_COMMWORLD_COL_SENT               = 362,  //!< Comm-world collective 32B packet chunks sent 
    PEVT_NW_USER_PP_RECV                     = 363,  //!< User pt-pt packets received 
    PEVT_NW_USER_DYN_PP_RECV                 = 364,  //!< User dynamic pt-pt packets received 
    PEVT_NW_USER_ESC_PP_RECV                 = 365,  //!< User escape pt-pt packets received 
    PEVT_NW_USER_PRI_PP_RECV                 = 366,  //!< User priority pt-pt packets received 
    PEVT_NW_SYS_PP_RECV                      = 367,  //!< System pt-pt packets received 
    PEVT_NW_USER_WORLD_COL_RECV              = 368,  //!< User world collective packets received 
    PEVT_NW_USER_SUBC_COL_RECV               = 369,  //!< User sub-comm collective packets received 
    PEVT_NW_SYS_COL_RECV                     = 370,  //!< System collective packets received 
    PEVT_NW_COMMWORLD_COL_RECV               = 371,  //!< Comm-world collective packets received 
    PEVT_NW_USER_PP_RECV_FIFO                = 372,  //!< User pt-pt packets in receive fifo 
    PEVT_NW_USER_DYN_PP_RECV_FIFO            = 373,  //!< User dynamic pt-pt packets in receive fifo 
    PEVT_NW_USER_ESC_PP_RECV_FIFO            = 374,  //!< User escape pt-pt packets in receive fifo 
    PEVT_NW_USER_PRI_PP_RECV_FIFO            = 375,  //!< User priority pt-pt packets in receive fifo 
    PEVT_NW_SYS_PP_RECV_FIFO                 = 376,  //!< System pt-pt packets in receive fifo 
    PEVT_NW_USER_WORLD_COL_RECV_FIFO         = 377,  //!< User world collective packets in receive fifo 
    PEVT_NW_USER_SUBC_COL_RECV_FIFO          = 378,  //!< User subcomm collective packets in receive fifo 
    PEVT_NW_SYS_COL_RECV_FIFO                = 379,  //!< System collective packets in receive fifo 
    PEVT_NW_COMMWORLD_COL_RECV_FIFO          = 380,  //!< Comm-world collective packets in receive fifo 
    PEVT_NW_SENT                             = 381,  //!< Set attribute to filter 32B packet chunks sent 
    PEVT_NW_RECV                             = 382,  //!< Set attribute to filter packets received
    PEVT_NW_RECV_FIFO                        = 383,  //!< Set attribute to filter packets in receive fifo
    PEVT_NW_CYCLES                           = 384,  //!< Network cycles since reset
    PEVT_NWUNIT_LAST_EVENT                   = 384,

    // CNKunit Events
    PEVT_CNKNODE_MUINT                       = 385,  //!< MU Non-fatal interrupt
    PEVT_CNKNODE_NDINT                       = 386,  //!< ND Non-fatal interrupt
    PEVT_CNKPROC_RSV                         = 387,  //!< Reserved - NO CNK Process events are defined
    PEVT_CNKHWT_SYSCALL                      = 388,  //!< System Calls
    PEVT_CNKHWT_STANDARD                     = 389,  //!< External Input Interrupts
    PEVT_CNKHWT_CRITICAL                     = 390,  //!< Critical Input Interrupts
    PEVT_CNKHWT_DECREMENTER                  = 391,  //!< Decrementer Interrupts
    PEVT_CNKHWT_FIT                          = 392,  //!< Fixed Interval Timer Interrupts
    PEVT_CNKHWT_WATCHDOG                     = 393,  //!< Watchdog Timer Interrupts
    PEVT_CNKHWT_UDEC                         = 394,  //!< User Decrementer Interrupts
    PEVT_CNKHWT_PERFMON                      = 395,  //!< Performance Monitor interrupts
    PEVT_CNKHWT_UNKDEBUG                     = 396,  //!< Unknown/Invalid Interrupts
    PEVT_CNKHWT_DEBUG                        = 397,  //!< Debug Interrupts
    PEVT_CNKHWT_DSI                          = 398,  //!< Data Storage Interrupts
    PEVT_CNKHWT_ISI                          = 399,  //!< Instruction Storage Interrupts
    PEVT_CNKHWT_ALIGNMENT                    = 400,  //!< Alignment Interrupts
    PEVT_CNKHWT_PROGRAM                      = 401,  //!< Program Interrupts
    PEVT_CNKHWT_FPU                          = 402,  //!< FPU Unavailable Interrupts
    PEVT_CNKHWT_APU                          = 403,  //!< APU Unavailable Interrupts
    PEVT_CNKHWT_DTLB                         = 404,  //!< Data TLB Interrupts
    PEVT_CNKHWT_ITLB                         = 405,  //!< Instruction TLB Interrupts
    PEVT_CNKHWT_VECTOR                       = 406,  //!< Vector Unavailable Interrupts
    PEVT_CNKHWT_UNDEF                        = 407,  //!< Undefined Interrupts
    PEVT_CNKHWT_PDBI                         = 408,  //!< Processor Doorbell Interrupts
    PEVT_CNKHWT_PDBCI                        = 409,  //!< Processor Doorbell Critical Ints
    PEVT_CNKHWT_GDBI                         = 410,  //!< Guest Doorbell Interrupts
    PEVT_CNKHWT_GDBCI                        = 411,  //!< Guest Doorbell Crit or MChk Ints
    PEVT_CNKHWT_EHVSC                        = 412,  //!< Embedded Hypervisor System Calls
    PEVT_CNKHWT_EHVPRIV                      = 413,  //!< Embedded Hypervisor Privileged
    PEVT_CNKHWT_LRATE                        = 414,  //!< LRAT exception
    PEVT_CNKUNIT_LAST_EVENT                  = 414,

    PEVT_LAST_EVENT  =   414 
    

};


typedef enum eUpci_EventIds Upci_EventIds;

#define UPCI_NUM_EVENTS (PEVT_LAST_EVENT+1)


__END_DECLS

#endif
   
        
