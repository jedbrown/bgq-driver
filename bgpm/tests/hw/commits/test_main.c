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


#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <omp.h>

#include "bgpm/include/bgpm.h"
#include "spi/include/upci/testlib/upci_debug.h"
#include "spi/include/l1p/sprefetch.h"

//#define TEST_CASE_VERBOSE
#include "../../test_utils.h"

TEST_INIT();



// #defines:
#define INCL_MUXOPS   printf("Operations INCL_MUXOPS\n")
//#define INCL_NOOPS    printf("Operations INCL_NOOPS\n");
#define INCL_XU_OPS   printf("Operations INCL_XU_OPS\n");
//#define INCL_XU_UCODE printf("Operations INCL_XU_UCODE\n");
//#define INCL_AXU_OPS  printf("Operations INCL_AXU_OPS\n");
//#define INCL_AXU_UCODE  printf("Operations INCL_AXU_UCODE\n");
//#define INCL_BARR     printf("Operations INCL_BARR\n");


// FWD_STRIDE overrides BWD_STRIDE - leave BWD_STRIDE defined.
#define FWD_STRIDE     printf("Stride %d bytes thru memory forwards (low mem to high), access stride=%d bytes\n", DATA_STRIDE*8, ACCESS_STRIDE*8);
#define BWD_STRIDE     printf("Stride %d bytes thru memory backwards (high mem to low), access stride=%d bytes\n", DATA_STRIDE*8, ACCESS_STRIDE*8);
#define DATA_STRIDE    8  // Stride between separate loops (factors of 8)
#define ACCESS_STRIDE  8  // Stride within a loop (use to cause accesses to multiple cache lines in same loop)
                          // be careful - when ACCESS_STRIDE or DATA_STRIDE exceeds a cache line, we could
                          // exceed the memory buffer - might need some changes to account for this.




//#define MEM_ACCESSES  printf("Operations break out of L2 cache to memory\n");
#define L2_CACHE      printf("Operations stay within L2 cache\n");
//define L1_CACHE      printf("Operations stay within L1 cache\n");
#define CACHE_BURST   10  // only choose 0, 1, 7, 8 or 10 (greater than cache line)

#define PRINT_CACHE_BURST  printf("Load/Store op burst value = %d\n", CACHE_BURST);

#define INCL_FIXED_LOADS  printf("Operations INCL_FIXED_LOADS\n");
//#define INCL_FIXED_STORES printf("Operations INCL_FIXED_STORES\n");
//#define INCL_FLOAT_LOADS  printf("Operations INCL_FLOAT_LOADS\n");
//#define INCL_FLOAT_STORES printf("Operations INCL_FLOAT_STORES\n");
//#define INCL_QFLOAT_LOADS  printf("Operations INCL_QFLOAT_LOADS\n");
//#define INCL_QFLOAT_STORES printf("Operations INCL_QFLOAT_STORES\n");

void PrintActiveOperations()
{
#ifdef INCL_MUXOPS
    INCL_MUXOPS;
#endif
#ifdef INCL_NOOPS
    INCL_NOOPS;
#endif
#ifdef INCL_XU_OPS
    INCL_XU_OPS;
#endif
#ifdef INCL_AXU_OPS
    INCL_AXU_OPS;
#endif
#ifdef INCL_XU_UCODE
    INCL_XU_UCODE;
#endif
#ifdef INCL_AXU_UCODE
    INCL_AXU_UCODE;
#endif
#ifdef INCL_BARR
    INCL_BARR;
#endif
#ifdef INCL_FIXED_LOADS
    INCL_FIXED_LOADS;
#endif
#ifdef INCL_FIXED_STORES
    INCL_FIXED_STORES;
#endif
#ifdef INCL_FLOAT_LOADS
    INCL_FLOAT_LOADS;
#endif
#ifdef INCL_FLOAT_STORES
    INCL_FLOAT_STORES;
#endif
#ifdef INCL_QFLOAT_LOADS
    INCL_QFLOAT_LOADS;
#endif
#ifdef INCL_QFLOAT_STORES
    INCL_QFLOAT_STORES;
#endif
#ifdef FWD_STRIDE
    FWD_STRIDE;
#else
    BWD_STRIDE;
#endif


#ifdef MEM_ACCESSES
    MEM_ACCESSES;
    #if (CACHE_BURST > 0)
        PRINT_CACHE_BURST;
    #endif
#else
    #ifdef L2_CACHE
        L2_CACHE;
        #if (CACHE_BURST > 0)
            PRINT_CACHE_BURST;
        #endif
    #else
        #ifdef L1_CACHE
            L1_CACHE;
            #if (CACHE_BURST > 0)
                PRINT_CACHE_BURST;
            #endif
        #endif
    #endif
#endif

}


//#define EXAM_ORIG ??
//#define EXAM_TOP_CYCLES
//#define EXAM_STALLS
//#define EXAM_COMPLETES
//#define EXAM_FLUSHES1
//#define EXAM_FLUSHES2
//#define EXAM_FLUSHES3
//#define EXAM_TLB
#define EXAM_EVERYTHING


// Create a common data buffer to use for examining memory cacheing effects.
// Use separate sections of the buffer for each of the storage operations:
//   fixed load, fixed store, floating load, floating store, quad load, quad store.
// Make so each pass will use the same number of loads/stores whether or not
//   it's supposed to hit in cache, or miss in cache.
//
// Cache sizes:
//   < 8192 - remains in L1 cache  (12 sections total < 8192)
//   < 32Meg - remains in L2 cache (12 sections total < 32 Meg)
//   > 32 Meg - out of memory      (12 sections each  > 32 Meg)

// Anticipated operations:
//   Fixed point loads  va = a + fixedLoadData[i]
//   Double loads       vd = d + floatLoadData[i]
//   Fixed point stores fixedStoreData[i] = a++;
//   Double stores      floatStoreData[i] = d++;
//   quad loads         vqd = floatQLoadData[i]
//   quad stores        floatQStoreData[i] = d

// Dataspace layout:
//    fixedLoadData
//    floatLoadData
//    fixedStoreData
//    floatStoreData
//    floatQLoadData
//    floatQStoreData
//    unused
//    unused

//              2meg/slice    16 slices  twice size  8 data spaces - each capabile of flushing cache
#define BUFFSIZE   (1024*1024*2  * 16 *  2          * 8)
#define MEM_ENTRIES  (BUFFSIZE / 8 / 8 * (DATA_STRIDE/8))             // num var entries per space when test should break out of the L2 cache
#define L2_ENTRIES   ((1024*1024*2 * 16) / 8 / 8 * (DATA_STRIDE/8))   // num entries per space when test should remain in l2 cache.
#define L1_ENTRIES   (8192 / 8 / 8 * (DATA_STRIDE/8))                 // num entries per space when test should remain in L1 cache

#ifdef MEM_ACCESSES
#define NUM_ENTRIES MEM_ENTRIES
#else
    #ifdef L2_CACHE
        #define NUM_ENTRIES L2_ENTRIES
    #else
        #define NUM_ENTRIES L1_ENTRIES
    #endif
#endif



// Place these routines at the end, put the data in the heap,
// and create some simple routines to return the number of loops or max index to
// hide from the compiler the size of the data to prevent it from optimizing differently
// when the numbers are small.  It changes the loop significantly otherwise.
volatile uint64_t *fixedLoadData;
volatile uint64_t *fixedStoreData;
volatile double   *floatLoadData;
volatile double   *floatStoreData;
volatile double   *floatQLoadData;
volatile double   *floatQStoreData;

void InitData();
int DataLoops();
int MaxIndex();


int numThreads;
int numProcs;

UpciBool_t useCounting[CONFIG_MAX_APP_THREADS];


#ifdef EXAM_ORIG
unsigned evtList[] = {
    PEVT_CYCLES,
    //PEVT_XU_TIMEBASE_TICK,
    //PEVT_XU_THRD_RUNNING,

    PEVT_INST_ALL,
    PEVT_INST_XU_ALL,
    PEVT_INST_QFPU_ALL,

    PEVT_XU_PPC_COMMIT,
    //PEVT_XU_COMMIT,
    //PEVT_XU_UCODE_COMMIT,
	PEVT_XU_BR_TAKEN_COMMIT,

    PEVT_AXU_INSTR_COMMIT,
	PEVT_AXU_UCODE_OPS_COMMIT,

	PEVT_IU_TOT_ISSUE_COUNT,
	//PEVT_IU_FXU_ISSUE_COUNT,
	//PEVT_IU_AXU_ISSUE_COUNT,
	PEVT_IU_IBUFF_FLUSH,

	PEVT_XU_ANY_FLUSH,
	PEVT_IU_IU0_REDIR_CYC,
	PEVT_XU_BR_MISPRED_COMMIT,
	PEVT_IU_IBUFF_EMPTY_CYC,
};
#endif



// IU5 Examine Stall List
#if EXAM_STALLS
unsigned evtList[] = {
    PEVT_CYCLES,

    PEVT_INST_ALL,
    PEVT_INST_XU_ALL,
    PEVT_INST_QFPU_ALL,

    PEVT_XU_PPC_COMMIT,
    PEVT_XU_COMMIT,
    PEVT_XU_UCODE_COMMIT,

    PEVT_AXU_INSTR_COMMIT,
	PEVT_AXU_UCODE_OPS_COMMIT,

	PEVT_IU_TOT_ISSUE_COUNT,
	//PEVT_IU_FXU_ISSUE_COUNT,

	PEVT_IU_IS1_STALL_CYC,
	PEVT_IU_IS2_STALL_CYC,
	PEVT_IU_AXU_FXU_DEP_HIT_CYC,

	//PEVT_IU_IBUFF_EMPTY_CYC,
	//PEVT_IU_IU0_REDIR_CYC,

	//PEVT_XU_ANY_FLUSH,
	//PEVT_XU_BR_MISPRED_COMMIT,
	//PEVT_INST_XU_MATCH,
};
#endif



#ifdef EXAM_TOP_CYCLES
unsigned evtList[] = {
    PEVT_CYCLES,
    PEVT_INST_ALL,
    PEVT_INST_XU_ALL,
    PEVT_INST_QFPU_ALL,
    PEVT_INST_QFPU_UCODE,

    PEVT_XU_PPC_COMMIT,
    PEVT_XU_COMMIT,
    PEVT_XU_UCODE_COMMIT,
    PEVT_XU_ANY_FLUSH,

    PEVT_AXU_INSTR_COMMIT,
	PEVT_AXU_UCODE_OPS_COMMIT,

	PEVT_IU_TOT_ISSUE_COUNT,
	PEVT_IU_IS2_STALL_CYC,
	PEVT_IU_IS1_STALL_CYC,
	PEVT_IU_IBUFF_EMPTY_CYC,

    PEVT_INST_XU_MATCH,
};
#endif



#ifdef EXAM_COMPLETES
unsigned evtList[] = {
    PEVT_CYCLES,
    PEVT_INST_ALL,
    PEVT_INST_XU_ALL,
    PEVT_INST_QFPU_ALL,

    PEVT_XU_COMMIT,
    PEVT_XU_UCODE_COMMIT,

    PEVT_AXU_INSTR_COMMIT,
	PEVT_AXU_UCODE_OPS_COMMIT,

	PEVT_IU_TOT_ISSUE_COUNT,
	PEVT_IU_FXU_ISSUE_COUNT,

	PEVT_XU_ANY_FLUSH,
	PEVT_INST_QPFU_UCODE,

	PEVT_XU_BR_MISPRED_COMMIT,
	PEVT_IU_IBUFF_FLUSH,
	PEVT_LSU_LD_MISS_Q_FULL_FLUSH,
	PEVT_LSU_COMMIT_LD_MISSES,
    PEVT_INST_XU_MATCH,

};
#endif



#ifdef EXAM_FLUSHES1
unsigned evtList[] = {
    PEVT_CYCLES,
    PEVT_INST_ALL,
    PEVT_INST_XU_ALL,
    PEVT_INST_QFPU_ALL,
    PEVT_INST_QFPU_UCODE,

    PEVT_XU_ANY_FLUSH,
    PEVT_XU_BR_MISPRED_COMMIT,
    PEVT_XU_BR_TARG_ADDR_MISPRED_COMMIT,
    //PEVT_XU_INTS_TAKEN,

    PEVT_AXU_DENORM_FLUSH,

    PEVT_IU_RSV_ANY_L2_BACK_INV,
    PEVT_IU_IBUFF_FLUSH,


    //  PEVT_LSU_UNSUPPORTED_ALIGNMENT_FLUSH,
    //  PEVT_LSU_RELOAD_RESRC_CONFLICT_FLUSH,
    PEVT_LSU_INTER_THRD_DIR_ACCESS_FLUSH,
    //  PEVT_LSU_LMQ_DEPENDENCY_FLUSH,
    //  PEVT_LSU_LD_MISS_Q_FULL_FLUSH,
    //  PEVT_LSU_ST_Q_FULL_FLUSH,
    PEVT_LSU_HIT_LD_FLUSH,
    PEVT_LSU_HIT_IG1_REQ_FLUSH,
    //  PEVT_LSU_LD_LAT_MEM_SUBSYS_CYC,

};

#endif



#ifdef EXAM_FLUSHES2
unsigned evtList[] = {
    PEVT_CYCLES,
    PEVT_INST_ALL,
    PEVT_INST_XU_ALL,
    PEVT_INST_QFPU_ALL,
    PEVT_INST_QFPU_UCODE,

    PEVT_LSU_UNSUPPORTED_ALIGNMENT_FLUSH,
    PEVT_LSU_RELOAD_RESRC_CONFLICT_FLUSH,
    //PEVT_LSU_INTER_THRD_DIR_ACCESS_FLUSH,
    PEVT_LSU_LMQ_DEPENDENCY_FLUSH,
    PEVT_LSU_LD_MISS_Q_FULL_FLUSH,
    //PEVT_LSU_ST_Q_FULL_FLUSH,
    //PEVT_LSU_HIT_LD_FLUSH,
    //PEVT_LSU_HIT_IG1_REQ_FLUSH,
    //PEVT_LSU_LD_LAT_MEM_SUBSYS_CYC,

};

#endif



#ifdef EXAM_FLUSHES3
unsigned evtList[] = {
    PEVT_CYCLES,
    PEVT_INST_ALL,
    PEVT_INST_XU_ALL,
    PEVT_INST_QFPU_ALL,
    PEVT_INST_QFPU_UCODE,

    PEVT_XU_ANY_FLUSH,
    //PEVT_XU_MULT_DIV_COLLISION    ,

    PEVT_IU_IU0_REDIR_CYC,
    PEVT_IU_IERAT_MISS,

    //PEVT_LSU_UNSUPPORTED_ALIGNMENT_FLUSH,
    //PEVT_LSU_RELOAD_RESRC_CONFLICT_FLUSH,
    //PEVT_LSU_INTER_THRD_DIR_ACCESS_FLUSH,
    //PEVT_LSU_LMQ_DEPENDENCY_FLUSH,
    //PEVT_LSU_LD_MISS_Q_FULL_FLUSH,
    PEVT_LSU_ST_Q_FULL_FLUSH,
    //PEVT_LSU_HIT_LD_FLUSH,
    //PEVT_LSU_HIT_IG1_REQ_FLUSH,
    PEVT_LSU_LD_LAT_MEM_SUBSYS_CYC,

    PEVT_LSU_L1_CACHE_PTYERR_DETECTED,
};

#endif


#ifdef EXAM_TLB
unsigned evtList[] = {
    PEVT_CYCLES,
    PEVT_INST_ALL,
    PEVT_INST_XU_ALL,
    PEVT_INST_QFPU_ALL,
    PEVT_INST_QFPU_UCODE,

   PEVT_MMU_TLB_HIT_DIRECT_IERAT,
   PEVT_MMU_TLB_MISS_DIRECT_IERAT,
   PEVT_MMU_TLB_MISS_INDIR_IERAT,
   PEVT_MMU_HTW_HIT_IERAT,
   PEVT_MMU_HTW_MISS_IERAT,
   PEVT_MMU_TLB_HIT_DIRECT_DERAT,
   PEVT_MMU_TLB_MISS_DIRECT_DERAT,
   PEVT_MMU_TLB_MISS_INDIR_DERAT,
   PEVT_MMU_HTW_HIT_DERAT,
   PEVT_MMU_HTW_MISS_DERAT,
   PEVT_MMU_IERAT_MISS,
   PEVT_MMU_IERAT_MISS_CYC,
   PEVT_MMU_DERAT_MISS,
   PEVT_MMU_DERAT_MISS_CYC,
};

#endif



#ifdef EXAM_EVERYTHING
unsigned evtList[] = {
    PEVT_CYCLES,
    PEVT_INST_ALL,
    PEVT_INST_XU_ALL,
    PEVT_INST_QFPU_ALL,
    PEVT_INST_QFPU_UCODE,
    PEVT_INST_XU_MATCH,
    PEVT_XU_ANY_FLUSH,

    // AXU
    PEVT_AXU_INSTR_COMMIT,
    PEVT_AXU_CR_COMMIT,
    PEVT_AXU_IDLE,
    PEVT_AXU_FP_DS_ACTIVE,
    PEVT_AXU_FP_DS_ACTIVE_CYC,
    PEVT_AXU_DENORM_FLUSH,
    PEVT_AXU_UCODE_OPS_COMMIT,
    PEVT_AXU_FP_EXCEPT,
    PEVT_AXU_FP_ENAB_EXCEPT,


    // IU
    PEVT_IU_IL1_MISS,
    PEVT_IU_IL1_MISS_CYC,
    PEVT_IU_IL1_RELOADS_DROPPED,
    PEVT_IU_RELOAD_COLLISIONS,
    PEVT_IU_RELOAD_COLLISIONS_CYC,
    PEVT_IU_IU0_REDIR_CYC,
    PEVT_IU_IERAT_MISS,
    PEVT_IU_IERAT_MISS_CYC,
    PEVT_IU_ICACHE_FETCH,
    PEVT_IU_ICACHE_FETCH_CYC,
    PEVT_IU_INSTR_FETCHED,
    PEVT_IU_INSTR_FETCHED_CYC,
    PEVT_IU_RSV_ANY_L2_BACK_INV,
    PEVT_IU_RSV_ANY_L2_BACK_INV_CYC,
    PEVT_IU_L2_BACK_INV_HITS,
    PEVT_IU_L2_BACK_INV_HITS_CYC,
    PEVT_IU_IBUFF_EMPTY,
    PEVT_IU_IBUFF_EMPTY_CYC,
    PEVT_IU_IBUFF_FLUSH,
    PEVT_IU_IBUFF_FLUSH_CYC,
    PEVT_IU_IS1_STALL_CYC,
    PEVT_IU_IS2_STALL_CYC,
    PEVT_IU_BARRIER_OP_STALL_CYC,
    PEVT_IU_SLOW_SPR_ACCESS_CYC,
    PEVT_IU_RAW_DEP_HIT_CYC,
    PEVT_IU_WAW_DEP_HIT_CYC,
    PEVT_IU_SYNC_DEP_HIT_CYC,
    PEVT_IU_SPR_DEP_HIT_CYC,
    PEVT_IU_AXU_DEP_HIT_CYC,
    PEVT_IU_FXU_DEP_HIT_CYC,
    PEVT_IU_AXU_FXU_DEP_HIT_CYC,
    //PEVT_IU_TWO_INSTR_ISSUE,
    PEVT_IU_AXU_ISSUE_PRI_LOSS_CYC,
    PEVT_IU_FXU_ISSUE_PRI_LOSS_CYC,
    //PEVT_IU_AXU_ISSUE_COUNT,
    PEVT_IU_FXU_ISSUE_COUNT,
    PEVT_IU_TOT_ISSUE_COUNT,

    // XU
    //PEVT_XU_PROC_BUSY,
    //PEVT_XU_BR_COMMIT_CORE,
    //PEVT_XU_BR_MISPRED_COMMIT_CORE,
    //PEVT_XU_BR_TARG_ADDR_MISPRED_COMMIT_CORE,
    PEVT_XU_THRD_RUNNING,
    PEVT_XU_TIMEBASE_TICK,
    PEVT_XU_SPR_READ_COMMIT,
    PEVT_XU_SPR_WRITE_COMMIT,
    PEVT_XU_STALLED_ON_WAITRSV,
    PEVT_XU_STALLED_ON_WAITRSV_CYC,
    PEVT_XU_EXT_INT_ASSERT,
    PEVT_XU_CRIT_EXT_INT_ASSERT,
    PEVT_XU_PERF_MON_INT_ASSERT,
    PEVT_XU_PPC_COMMIT,
    PEVT_XU_COMMIT,
    PEVT_XU_UCODE_COMMIT,
    PEVT_XU_ANY_FLUSH,
    PEVT_XU_BR_COMMIT,
    PEVT_XU_BR_MISPRED_COMMIT,
    PEVT_XU_BR_TAKEN_COMMIT,
    PEVT_XU_BR_TARG_ADDR_MISPRED_COMMIT,
    PEVT_XU_MULT_DIV_COLLISION,
    PEVT_XU_EXT_INT_PEND,
    PEVT_XU_CRIT_EXT_INT_PEND,
    PEVT_XU_PERF_MON_INT_PEND,
    PEVT_XU_RUN_INSTR,
    PEVT_XU_INTS_TAKEN,
    PEVT_XU_EXT_INT_TAKEN,
    PEVT_XU_CRIT_EXT_INT_TAKEN,
    PEVT_XU_PERF_MON_INT_TAKEN,
    PEVT_XU_DOORBELL_INT_TAKEN,
    PEVT_XU_STCX_FAIL,
    PEVT_XU_ICSWX_FAILED,
    PEVT_XU_ICSWX_COMMIT,
    PEVT_XU_MULT_DIV_BUSY,

    // LSU
    PEVT_LSU_COMMIT_STS,
    PEVT_LSU_COMMIT_ST_MISSES,
    PEVT_LSU_COMMIT_LD_MISSES,
    PEVT_LSU_COMMIT_CACHE_INHIB_LD_MISSES,
    PEVT_LSU_COMMIT_CACHEABLE_LDS,
    PEVT_LSU_COMMIT_DCBT_MISSES,
    PEVT_LSU_COMMIT_DCBT_HITS,
    PEVT_LSU_COMMIT_AXU_LDS,
    PEVT_LSU_COMMIT_AXU_STS,
    PEVT_LSU_COMMIT_STCX,
    PEVT_LSU_COMMIT_WCLR,
    PEVT_LSU_COMMIT_WCLR_WL,
    PEVT_LSU_COMMIT_LDAWX,
    PEVT_LSU_UNSUPPORTED_ALIGNMENT_FLUSH,
    PEVT_LSU_RELOAD_RESRC_CONFLICT_FLUSH,
    PEVT_LSU_COMMIT_DUPLICATE_LDAWX,
    PEVT_LSU_INTER_THRD_DIR_ACCESS_FLUSH,
    PEVT_LSU_LMQ_DEPENDENCY_FLUSH,
    PEVT_LSU_COMMIT_WCHKALL,
    PEVT_LSU_COMMIT_SUCC_WCHKALL,
    PEVT_LSU_LD_MISS_Q_FULL_FLUSH,
    PEVT_LSU_ST_Q_FULL_FLUSH,
    PEVT_LSU_HIT_LD_FLUSH,
    PEVT_LSU_HIT_IG1_REQ_FLUSH,
    PEVT_LSU_LARX_FINISHED,
    PEVT_LSU_INTER_THRD_ST_WATCH_LOST,
    PEVT_LSU_RELOAD_WATCH_LOST,
    PEVT_LSU_BACK_INV_WATCH_LOST,
    PEVT_LSU_L1_DCACHE_BACK_INVAL,
    PEVT_LSU_L1_DCACHE_BACK_INVAL_HITS,
    PEVT_LSU_L1_CACHE_PTYERR_DETECTED,
    PEVT_LSU_LD_LAT_MEM_SUBSYS_CYC,

    // MMU
    PEVT_MMU_TLB_HIT_DIRECT_IERAT,
    PEVT_MMU_TLB_MISS_DIRECT_IERAT,
    PEVT_MMU_TLB_MISS_INDIR_IERAT,
    PEVT_MMU_HTW_HIT_IERAT,
    PEVT_MMU_HTW_MISS_IERAT,
    PEVT_MMU_TLB_HIT_DIRECT_DERAT,
    PEVT_MMU_TLB_MISS_DIRECT_DERAT,
    PEVT_MMU_TLB_MISS_INDIR_DERAT,
    PEVT_MMU_HTW_HIT_DERAT,
    PEVT_MMU_HTW_MISS_DERAT,
    PEVT_MMU_IERAT_MISS,
    PEVT_MMU_IERAT_MISS_CYC,
    PEVT_MMU_DERAT_MISS,
    PEVT_MMU_DERAT_MISS_CYC,
    //PEVT_MMU_IERAT_MISS_TOT,
    //PEVT_MMU_DERAT_MISS_TOT,
    //PEVT_MMU_TLB_MISS_DIRECT_TOT,
    //PEVT_MMU_TLB_HIT_FIRSTSIZE_TOT,
    //PEVT_MMU_TLB_HIT_INDIR_TOT,
    //PEVT_MMU_HTW_PTERELOAD_TOT,
    //PEVT_MMU_LRAT_TRANS_TOT,
    //PEVT_MMU_LRAT_MISS_TOT,
    //PEVT_MMU_PT_FAULT_TOT,
    //PEVT_MMU_PT_INELIG_TOT,
    //PEVT_MMU_TLBWEC_FAIL_TOT,
    //PEVT_MMU_TLBWEC_SUCC_TOT,
    //PEVT_MMU_TLBILX_SRC_TOT,
    //PEVT_MMU_TLBIVAX_SRC_TOT,
    //PEVT_MMU_TLBIVAX_SNOOP_TOT,
    //PEVT_MMU_TLB_FLUSH_REQ_TOT,
    //PEVT_MMU_TLB_FLUSH_REQ_TOT_CYC,

    // l1p
    PEVT_L1P_BAS_STRM_LINE_ESTB,
    PEVT_L1P_BAS_HIT,
    PEVT_L1P_BAS_PF2DFC,
    PEVT_L1P_BAS_MISS,
    PEVT_L1P_BAS_LU_DRAIN,
    PEVT_L1P_BAS_LU_DRAIN_CYC,
    PEVT_L1P_BAS_LD,
    PEVT_L1P_BAS_ST_WC,
    PEVT_L1P_BAS_ST_32BYTE,
    PEVT_L1P_BAS_ST_CRACKED,
    PEVT_L1P_BAS_LU_STALL_SRT,
    PEVT_L1P_BAS_LU_STALL_SRT_CYC,
    PEVT_L1P_BAS_LU_STALL_MMIO_DCR,
    PEVT_L1P_BAS_LU_STALL_MMIO_DCR_CYC,
    PEVT_L1P_BAS_LU_STALL_STRM_DET,
    PEVT_L1P_BAS_LU_STALL_STRM_DET_CYC,
    PEVT_L1P_BAS_LU_STALL_LIST_RD,
    PEVT_L1P_BAS_LU_STALL_LIST_RD_CYC,
    PEVT_L1P_BAS_ST,
    PEVT_L1P_BAS_LU_STALL_LIST_WRT,
    PEVT_L1P_BAS_LU_STALL_LIST_WRT_CYC,

    PEVT_L1P_STRM_LINE_ESTB,
    PEVT_L1P_STRM_HIT_FWD,
    PEVT_L1P_STRM_L1_HIT_FWD,
    PEVT_L1P_STRM_EVICT_UNUSED,
    PEVT_L1P_STRM_EVICT_PART_USED,
    PEVT_L1P_STRM_REMOTE_INVAL_MATCH,
    PEVT_L1P_STRM_DONT_CACHE,
    PEVT_L1P_STRM_STRM_DEPTH_STEAL,
    PEVT_L1P_STRM_STRM_ESTB,
    PEVT_L1P_STRM_WRT_INVAL,
    PEVT_L1P_STRM_LINE_ESTB_ALL_LIST,
    PEVT_L1P_STRM_HIT_LIST,
    PEVT_L1P_STRM_PF2DFC_LIST,
    PEVT_L1P_STRM_PART_INVAL_REFCH,


    // other op codes.
    PEVT_INST_XU_FLD,
    PEVT_INST_XU_FST,
    PEVT_INST_XU_QLD,
    PEVT_INST_XU_QST,
    PEVT_INST_XU_BITS,
    PEVT_INST_XU_BRC,
    PEVT_INST_XU_BRU,
    PEVT_INST_XU_CINV,
    PEVT_INST_XU_CSTO,
    PEVT_INST_XU_CTCH,
    PEVT_INST_XU_IADD,
    PEVT_INST_XU_ICMP,
    PEVT_INST_XU_ICSW,
    PEVT_INST_XU_IDIV,
    PEVT_INST_XU_ILOG,
    PEVT_INST_XU_IMOV,
    PEVT_INST_XU_IMUL,
    PEVT_INST_XU_INT,
    PEVT_INST_XU_LD,
    PEVT_INST_XU_LSCX,
    PEVT_INST_XU_ST,
    PEVT_INST_XU_SYNC,

    PEVT_INST_QFPU_FOTH,
    PEVT_INST_QFPU_FRES,
    PEVT_INST_QFPU_FADD,
    PEVT_INST_QFPU_FCVT,
    PEVT_INST_QFPU_FCMP,
    PEVT_INST_QFPU_FDIV,
    PEVT_INST_QFPU_FMOV,
    PEVT_INST_QFPU_FMA,
    PEVT_INST_QFPU_FMUL,
    PEVT_INST_QFPU_FRND,
    PEVT_INST_QFPU_FSQE,
    PEVT_INST_QFPU_FSQ,
    PEVT_INST_QFPU_QMOV,
    PEVT_INST_QFPU_QOTH,
    PEVT_INST_QFPU_QADD,
    PEVT_INST_QFPU_QMUL,
    PEVT_INST_QFPU_QRES,
    PEVT_INST_QFPU_QMA,
    PEVT_INST_QFPU_QRND,
    PEVT_INST_QFPU_QCVT,
    PEVT_INST_QFPU_QCMP,
};

#endif




#ifdef INCL_MUXOPS
#define MUXOP(cmd)  cmd
#else
#define MUXOP(cmd)
#endif



uint32_t totalLoops = 0;

void Exercise(int times, int loops, int hEvtSet, int limitMux)
{
    InitData();

    //Bgpm_Punit_Handles_t hCtrs;
    //Bgpm_Punit_GetHandles(hEvtSet, &hCtrs);


    // Exercise something

    volatile uint32_t count;
    count = 0;
    register int i;
    register unsigned j, m;
    register unsigned k, di;
    unsigned distore;  // use to add 1 store per loop to prevent di from being optomized out.
    register double floatData0; floatData0 = 1.0;
    register double floatData1; floatData1 = 1.0;
    register double floatData2; floatData2 = 1.0;
    register double floatData3; floatData3 = 1.0;
    register double floatData4; floatData4 = 1.0;
    register double floatData5; floatData5 = 1.0;
    register double floatData6; floatData6 = 1.0;
    register double floatData7; floatData7 = 1.0;
    register double floatData8; floatData8 = 1.0;
    register double floatData9; floatData9 = 1.0;
    register uint64_t fixedData0; fixedData0 = 0;
    register uint64_t fixedData1; fixedData1 = 0;
    register uint64_t fixedData2; fixedData2 = 0;
    register uint64_t fixedData3; fixedData3 = 0;
    register uint64_t fixedData4; fixedData4 = 0;
    register uint64_t fixedData5; fixedData5 = 0;
    register uint64_t fixedData6; fixedData6 = 0;
    register uint64_t fixedData7; fixedData7 = 0;
    register uint64_t fixedData8; fixedData8 = 0;
    register uint64_t fixedData9; fixedData9 = 0;

    int dataLoops;  dataLoops = DataLoops();
    int maxIndex;  maxIndex = MaxIndex();

    int muxGrps = Bgpm_GetMuxGroups(hEvtSet);
    if (limitMux) { muxGrps = 1; }
    for (k=0; k<muxGrps+1; k++) {

        if (k > 0) Bgpm_Start(hEvtSet);
        //Bgpm_Punit_Start(hCtrs.allCtrMask);

        for (m=0; m<times; m++) {
            for (j=0; j<loops; ++j) {
                #ifdef FWD_STRIDE
                for (i=0; i<dataLoops+1; i+=DATA_STRIDE) {
                #else
                for (i=dataLoops; i>=0; i-=DATA_STRIDE) {
                #endif


                    di = i % maxIndex;  // index into data sections to stay within caching criteria
                    distore = di;  // keep di from being optomized out without stores (if made volatile).

                    //totalLoops++;

                    //#define INCL_NOOPS
                    #ifdef INCL_NOOPS
                    // flush out interesting instructions from pipeline so branches
                    // less likely to confuse results (really only need 11 nops I think)
                    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                    #endif

                    // 7 xu ops (or 14 or so, compiler will expand differently)
                    #ifdef INCL_XU_OPS
                        #ifdef INCL_FIXED_LOADS
                            #if (CACHE_BURST > 8)
                            fixedData0 = fixedLoadData[di+9*ACCESS_STRIDE];
                            fixedData1 = fixedLoadData[di+8*ACCESS_STRIDE];
                            #endif
                            #if (CACHE_BURST > 7)
                            fixedData2 = fixedLoadData[di+7*ACCESS_STRIDE];
                            #endif
                            #if (CACHE_BURST > 1)
                            fixedData3 = fixedLoadData[di+6*ACCESS_STRIDE];
                            fixedData4 = fixedLoadData[di+5*ACCESS_STRIDE];
                            fixedData5 = fixedLoadData[di+4*ACCESS_STRIDE];
                            fixedData6 = fixedLoadData[di+3*ACCESS_STRIDE];
                            fixedData7 = fixedLoadData[di+2*ACCESS_STRIDE];
                            fixedData8 = fixedLoadData[di+1*ACCESS_STRIDE];
                            #endif
                            #if (CACHE_BURST > 0)
                            fixedData9 = fixedLoadData[di+0];
                            #endif
                        #endif
                        #ifdef INCL_FIXED_STORES
                            #if (CACHE_BURST > 8)
                            fixedStoreData[di+9] = fixedData0;
                            fixedStoreData[di+8] = fixedData1;
                            #endif
                            #if (CACHE_BURST > 7)
                            fixedStoreData[di+7] = fixedData2;
                            #endif
                            #if (CACHE_BURST > 1)
                            fixedStoreData[di+6] = fixedData3;
                            fixedStoreData[di+5] = fixedData4;
                            fixedStoreData[di+4] = fixedData5;
                            fixedStoreData[di+3] = fixedData6;
                            fixedStoreData[di+2] = fixedData7;
                            fixedStoreData[di+1] = fixedData8;
                            #endif
                            #if (CACHE_BURST > 0)
                            fixedStoreData[di+0] = fixedData9;
                            #endif
                        #endif
                    #endif

                    // 5 axu ops
                    #ifdef INCL_AXU_OPS
                    #ifdef INCL_FLOAT_LOADS
                    floatData0 = floatLoadData[di+4];
                    floatData1 = floatLoadData[di+3];
                    floatData2 = floatLoadData[di+2];
                    floatData3 = floatLoadData[di+1];
                    floatData4 = floatLoadData[di+0];
                    #endif
                    #ifdef INCL_FLOAT_STORES
                    floatStoreData[di+4] = floatData0*0.9;
                    floatStoreData[di+3] = floatData1*0.91;
                    floatStoreData[di+2] = floatData2*0.92;
                    floatStoreData[di+1] = floatData3*0.93;
                    floatStoreData[di+0] = floatData4*0.94;
                    #endif
                    #endif

                    // 4 xu ucoded ops
                    #ifdef INCL_XU_UCODE
                    count = upc_cntlz32(0x08);
                    count = upc_cntlz32(0x08);
                    count = upc_cntlz32(0x08);
                    count = upc_cntlz32(0x08);
                    #endif

                    // 3 axu ucoded ops
                    #ifdef INCL_AXU_UCODE
                    #ifdef INCL_FLOAT_LOADS
                    floatData /= floatLoadData[di];
                    floatData /= floatLoadData[di+1];
                    floatData /= floatLoadData[di+2];
                    #endif
                    #ifdef INCL_FLOAT_STORES
                    floatStoreData[di]   = floatData / 0.9;
                    floatStoreData[di+1] = floatData / 0.91;
                    floatStoreData[di+2] = floatData / 0.92;
                    #endif
                    #endif

					// 9 barrier operations
                    #ifdef INCL_BARR
                    mbar();
                    mbar();
                    mbar();
                    mbar();
                    mbar();
                    mbar();
                    mbar();
                    mbar();
                    mbar();
                    #endif

                    #ifdef INCL_NOOPS
                    // flush out interesting instructions from pipeline so branches
                    // less likely to confuse results.
                    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                    #endif
                }
                //fprintf(stderr, "    totalLoops=%d, loop=%d\n", totalLoops, j);
            }
            //fprintf(stderr, "  times=%d\n", m);
        }
        //fprintf(stderr, "mux=%d\n", k);

        //Bgpm_Punit_Stop(hCtrs.allCtrMask);
        //Delay(800);
        if (k > 0) Bgpm_Stop(hEvtSet);


        MUXOP(Bgpm_SwitchMux(hEvtSet));
    }


}



void PrintCounts(unsigned hEvtSet)
{
    Bgpm_MuxStats_t mStats;
    Bgpm_GetMultiplex(hEvtSet, &mStats);
    PrintActiveOperations();
    printf("\nMux Stats: numGrps=%d, numSwitches=%d, elapsedCycles=0x%016lx\n",
            mStats.numGrps, mStats.numSwitches, mStats.elapsedCycles);

    printf("Counter Results:\n%-40s  %10s   %10s %6s  =   %13s   %13s   %13s\n", "Label", "select   ", "edge/cycle", "Metric", "ElapsedCycles", "NormScaledCount", "RawCount");
    int i;
    int numEvts = Bgpm_NumEvents(hEvtSet);

    uint64_t cnt, muxCycles, normCycles;
    for (i=0; i<numEvts; i++) {
        // get event info
        int evtId = Bgpm_GetEventId(hEvtSet, i);
        Bgpm_EventInfo_t einfo;
        Bgpm_GetEventIdInfo(evtId, &einfo);
        ushort edge = BGPM_TRUE;
        Bgpm_GetEventEdge(hEvtSet,i,&edge);

        Bgpm_ReadEvent(hEvtSet, i, &cnt);
        muxCycles = Bgpm_GetMuxEventElapsedCycles(hEvtSet, i);
        normCycles = 0;
        if (muxCycles > 0) {
            normCycles = (((mStats.elapsedCycles * 100000000) / muxCycles) * cnt) / 100000000;
        }

#define DECSTRGLEN 50
        char strg1[DECSTRGLEN];
        char strg2[DECSTRGLEN];
        char strg3[DECSTRGLEN];

        printf("%-40s  0x%08x %10s  %5s   = %16s     %16s   %16s\n", einfo.label, einfo.selVal, (edge ? "edge":"cycle"), Upci_GetMetricText(einfo.metric),
                UInt64toStrg(muxCycles, strg1, DECSTRGLEN),
                UInt64toStrg(normCycles, strg2, DECSTRGLEN),
                UInt64toStrg(cnt, strg3, DECSTRGLEN));
    }
}




void CollectCounts(int times, int loops)
{
    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(stderr,numThreads,numProcs,evtList,numFailures,loops,times,totalLoops)
    {
        L1P_SetStreamPolicy(L1P_stream_disable);

        Bgpm_Init(BGPM_MODE_SWDISTRIB);
        int hEvtSet = Bgpm_CreateEventSet();
        MUXOP(Bgpm_SetMultiplex(hEvtSet, 0, BGPM_NOTNORMAL));
        //MUXOP(Bgpm_SetMultiplex(hEvtSet, 0, BGPM_NORMAL));

        //Bgpm_ExitOnError(BGPM_FALSE);
        Bgpm_AddEventList(hEvtSet, evtList, sizeof(evtList)/sizeof(unsigned));
        //Bgpm_DumpEvtSet(hEvtSet, 0);
        //exit(-1);
        //Bgpm_ExitOnError(BGPM_TRUE);

        int grpIdx = Bgpm_GetEventIndex(hEvtSet, PEVT_INST_QFPU_GRP_MASK, 0);
        if (grpIdx < Bgpm_NumEvents(hEvtSet)) {
        	Bgpm_SetQfpuGrpMask(hEvtSet, grpIdx, UPC_P_AXU_OGRP_FDIV | UPC_P_AXU_OGRP_FSQ);
        }
        //int flushIdx = Bgpm_GetEventIndex(hEvtSet, PEVT_XU_ANY_FLUSH, 0);
        //if (flushIdx < Bgpm_NumEvents(hEvtSet)) {
        //    Bgpm_SetEventEdge(hEvtSet, flushIdx, BGPM_TRUE);
        //}
        int xuMatchIdx = Bgpm_GetEventIndex(hEvtSet, PEVT_INST_XU_MATCH, 0);
        if (xuMatchIdx < Bgpm_NumEvents(hEvtSet)) {
            Bgpm_SetXuMatch(hEvtSet, xuMatchIdx, BGPM_INST_D_FORM_CODE(24), BGPM_INST_D_FORM_MASK);   // Match Noop
        }


        //int iuIu0Idx = Bgpm_GetEventIndex(hEvtSet, PEVT_IU_IU0_REDIR_CYC, 0);
        //if (iuIu0Idx < Bgpm_NumEvents(hEvtSet)) {
        //    Bgpm_SetEventEdge(hEvtSet, iuIu0Idx, 1);
        //}


        Bgpm_Apply(hEvtSet);

        //Bgpm_DumpUPCPState();

        // Reset and measure for real
        totalLoops = 0;
        Bgpm_Reset(hEvtSet);
        Exercise(times, loops, hEvtSet, 0);

        // Print Results each event.
        #pragma omp critical (PrintEvts)
        {
            fprintf(stderr, "times=%d, loops=%d, dataLoops=%d, TotalLoops=%d\n", times, loops, DataLoops()/DATA_STRIDE, totalLoops);

            PrintCounts(hEvtSet);

            //Bgpm_PrintCurrentPunitReservations(hEvtSet);
            //Bgpm_DumpUPCPState();
        }

        Bgpm_Disable();
    }
}



int main(int argc, char *argv[])
{
    TEST_MODULE_PRINT0(Compare committed operations);

    if (argc < 2) {
        fprintf(stderr, "ERROR: Pass in number of threads\n");
        exit(-1);
    }
    numThreads = atoi(argv[1]);
    numProcs = Kernel_ProcessCount();

    fprintf(stderr, "numProcesses=%d, numThreads=%d\n", numProcs, numThreads);

    TEST_CASE(CPI Metric Experiments);
    CollectCounts( 2, 5 );

    TEST_RETURN();

}



volatile ALIGN_L2_CACHE uint64_t bfixedLoadData[NUM_ENTRIES];
volatile ALIGN_L2_CACHE uint64_t bfixedStoreData[NUM_ENTRIES];;
volatile ALIGN_L2_CACHE double   bfloatLoadData[NUM_ENTRIES];;
volatile ALIGN_L2_CACHE double   bfloatStoreData[NUM_ENTRIES];;
volatile ALIGN_L2_CACHE double   bfloatQLoadData[NUM_ENTRIES];;
volatile ALIGN_L2_CACHE double   bfloatQStoreData[NUM_ENTRIES];;

void InitData()
{
    fixedLoadData   = bfixedLoadData;
    fixedStoreData  = bfixedStoreData;
    floatLoadData   = bfloatLoadData;
    floatStoreData  = bfloatStoreData;
    floatQLoadData  = bfloatQLoadData;
    floatQStoreData = bfloatQStoreData;

   int di;
   for (di=0; di<NUM_ENTRIES; di++) {
        fixedLoadData[di] = di;
        fixedStoreData[di] = di;
        floatLoadData[di] = di;
        floatStoreData[di] = di;
        floatQLoadData[di] = di;
        floatQStoreData[di] = di;
   }
}

int DataLoops() {  return MEM_ENTRIES*2; }
int MaxIndex() { return (NUM_ENTRIES - DATA_STRIDE); }

int MaxIndex();
