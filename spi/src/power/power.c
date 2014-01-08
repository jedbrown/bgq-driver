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
/*------------------------------------------------------------------------*/
/* Simple interface to the BGQ hardware performance counters.             */
/*------------------------------------------------------------------------*/
/* C / C++:                                                               */
/*    HPM_Init(void);                       init the counters             */
/*    EMON_Init(void);                      init the emon                 */
/*    HPM_EMON_Init(void);                  init the counters and emon    */
/*    HPM_Start("label");                   start counting                */
/*    HPM_Stop("label");                    stop  counting                */
/*    HPM_Print(void);                      print counts                  */
/*    EMON_Print(void);                     print emon data               */
/*    HPM_EMON_Print(void);                 print counts and emon data    */
/*    HPM_Init_t(int num_threads);          init the counters             */
/*    EMON_Init_t(int num_threads);         init the emon                 */
/*    HPM_EMON_Init_t(int num_threads);     init the counters and emon    */
/*    HPM_Print_t(int num_threads);         print counts                  */
/*    EMON_Print_t(int num_threads);        print emon data               */
/*    HPM_EMON_Print_t(int num_threads);    print counts and emon data    */
/*------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spi/include/upci/events.h"
#include "spi/include/upci/upci.h"
#include "spi/include/upci/testlib/upc_p_debug.h"
#include "spi/include/upci/testlib/upc_c_debug.h"
#include "spi/include/power/power.h"
#include "spi/include/l2/barrier.h"

#define EMON_DEFINE_GLOBALS
#include "spi/include/emon/emon.h"

#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/bqc/A2_inlines.h>

#define MAX_COUNTERS 24
#define MAX_CORES 17
#define LABEL_LEN  80
#define MAX_CODE_BLOCKS 20
#define MAX_EVENTS 500
// #define DEBUG

static volatile int initialized = 0;
static int code_block  = 0;
static int num_counters = 24;
static int num_events = 6;
static int hpm_group = 0;
static int hpm_threads = 4;
// static int process_scope = 0;
// static int node_scope = 0;
// static int derived_metrics = 0;
// static int counter_mode = 0;
// static int event_set = 0;
static int *eventSet;

static long long timebase_in[MAX_CODE_BLOCKS];
static long long timebase_sum[MAX_CODE_BLOCKS];
static int counter_index[MAX_COUNTERS];
static uint64_t counter_in[MAX_CORES][MAX_CODE_BLOCKS][MAX_COUNTERS];
static uint64_t counter_sum[MAX_CORES][MAX_CODE_BLOCKS][MAX_COUNTERS];
static char code_block_label[MAX_CODE_BLOCKS][LABEL_LEN];
static int block_starts[MAX_CODE_BLOCKS];
static int block_stops[MAX_CODE_BLOCKS];
// static uint64_t process_counts[MAX_CODE_BLOCKS][MAX_COUNTERS];
static long long L2_in[MAX_CODE_BLOCKS][6];
static long long L2_sum[MAX_CODE_BLOCKS][6];
static char label[MAX_EVENTS][LABEL_LEN];
static Upci_Punit_t Punit[MAX_CORES];
static Upci_Punit_EventH_t eventHandle[MAX_CORES][MAX_COUNTERS];
static Upci_Mode_t Mode[MAX_CORES];
static int numcores;
static int coremask[MAX_CORES];
static int mask[MAX_EVENTS];
static unsigned int lock_val;
static L2_Barrier_t id_barrier = L2_BARRIER_INITIALIZER;

uint64_t * volatile atomic_ptr_4all;
uint64_t atomic_match_4all[68];

// long long timebase(void);
int index_from_label(char *);
void set_labels(void);
void set_aggregation_mask(void);
int hpm_lock_acquire();
int hpm_barrier_atomic_inc_no_match_all();
int hpm_atoi(char *);
char * hpm_strcpy(char *s, const char *t);

// HPM_GROUP=-1
// for experiments
static int exptSet[6] = { PEVT_IU_AXU_FXU_DEP_HIT_CYC,    //  0
                          PEVT_LSU_COMMIT_LD_MISSES,      //  1
                          PEVT_LSU_COMMIT_CACHEABLE_LDS,  //  2
                          PEVT_L1P_BAS_MISS,              //  3
                          PEVT_INST_XU_ALL,               //  4
                          PEVT_INST_QFPU_ALL};            //  5

// HPM_GROUP=0
// a good basic set, can count 1-4 threads/core
static int mySet[6] = { PEVT_IU_AXU_FXU_DEP_HIT_CYC,    //  0
                        PEVT_LSU_COMMIT_LD_MISSES,      //  1
                        PEVT_LSU_COMMIT_CACHEABLE_LDS,  //  2
                        PEVT_L1P_BAS_MISS,              //  3
                        PEVT_INST_XU_ALL,               //  4
                        PEVT_INST_QFPU_ALL};            //  5

// HPM_GROUP=1
// counters for load/stores, can count 1-2 threads/core
static int ldSet[12] = { PEVT_LSU_COMMIT_LD_MISSES, //  0
                         PEVT_L1P_BAS_HIT,          //  1
                         PEVT_L1P_BAS_MISS,         //  2
                         PEVT_L1P_BAS_LD,           //  3
                         PEVT_INST_XU_LD,           //  4
                         PEVT_INST_XU_ST,           //  5
                         PEVT_INST_XU_FLD,          //  6
                         PEVT_INST_XU_FST,          //  7
                         PEVT_INST_XU_QLD,          //  8
                         PEVT_INST_XU_QST,          //  9
                         PEVT_INST_XU_ALL,          // 10
                         PEVT_INST_ALL};            // 11

// HPM_GROUP=2
// all FPU counters, can count only 1 thread/core
static int fpuSet[24] = { PEVT_INST_ALL,            //  0
                          PEVT_INST_QFPU_ALL,       //  1
                          PEVT_INST_QFPU_FOTH,      //  2
                          PEVT_INST_QFPU_FRES,      //  3
                          PEVT_INST_QFPU_FADD,      //  4
                          PEVT_INST_QFPU_FCVT,      //  5
                          PEVT_INST_QFPU_FCMP,      //  6
                          PEVT_INST_QFPU_FDIV,      //  7
                          PEVT_INST_QFPU_FMOV,      //  8
                          PEVT_INST_QFPU_FMA,       //  9
                          PEVT_INST_QFPU_FMUL,      // 10
                          PEVT_INST_QFPU_FRND,      // 11
                          PEVT_INST_QFPU_FSQE,      // 12
                          PEVT_INST_QFPU_FSQ,       // 13
                          PEVT_INST_QFPU_QMOV,      // 14
                          PEVT_INST_QFPU_QOTH,      // 15
                          PEVT_INST_QFPU_QADD,      // 16
                          PEVT_INST_QFPU_QMUL,      // 17
                          PEVT_INST_QFPU_QRES,      // 18
                          PEVT_INST_QFPU_QMA,       // 19
                          PEVT_INST_QFPU_QRND,      // 20
                          PEVT_INST_QFPU_QCVT,      // 21
                          PEVT_INST_QFPU_QCMP,      // 22
                          PEVT_INST_QFPU_FPGRP1};   // 23

// HPM_GROUP=3
// normal pipe FPU instructions, can count 1-2 threads/core
static int fpSet0[12] = { PEVT_INST_QFPU_FOTH,      //  0   has all single-fpu counters 
                          PEVT_INST_QFPU_FRES,      //  1
                          PEVT_INST_QFPU_FADD,      //  2
                          PEVT_INST_QFPU_FCVT,      //  3
                          PEVT_INST_QFPU_FCMP,      //  4
                          PEVT_INST_QFPU_FDIV,      //  5
                          PEVT_INST_QFPU_FMOV,      //  6
                          PEVT_INST_QFPU_FMA,       //  7
                          PEVT_INST_QFPU_FMUL,      //  8
                          PEVT_INST_QFPU_FRND,      //  9
                          PEVT_INST_QFPU_FSQE,      // 10
                          PEVT_INST_QFPU_FSQ};      // 11

// HPM_GROUP=30
// normal pipe FPU instructions, can count 1-4 threads/core
static int fpSet00[6] = { PEVT_INST_QFPU_FOTH,      //  0   has single-fpu counters 
                          PEVT_INST_QFPU_FRES,      //  1
                          PEVT_INST_QFPU_FADD,      //  2
                          PEVT_INST_QFPU_FCVT,      //  3
                          PEVT_INST_QFPU_FCMP,      //  4
                          PEVT_INST_QFPU_FDIV};     //  5

// HPM_GROUP=31
// normal pipe FPU instructions, can count 1-4 threads/core
static int fpSet01[6] = { PEVT_INST_QFPU_FMOV,      //  0  has single-fpu counters
                          PEVT_INST_QFPU_FMA,       //  1
                          PEVT_INST_QFPU_FMUL,      //  2
                          PEVT_INST_QFPU_FRND,      //  3
                          PEVT_INST_QFPU_FSQE,      //  4
                          PEVT_INST_QFPU_FSQ};      //  5

// HPM_GROUP=4
// qpx FPU instructions, can count 1-2 threads/core
static int fpSet1[12] = { PEVT_INST_ALL,            //  0   has all qpx fpu counters + more
                          PEVT_INST_QFPU_ALL,       //  1
                          PEVT_INST_QFPU_QMOV,      //  2
                          PEVT_INST_QFPU_QOTH,      //  3
                          PEVT_INST_QFPU_QADD,      //  4
                          PEVT_INST_QFPU_QMUL,      //  5
                          PEVT_INST_QFPU_QRES,      //  6
                          PEVT_INST_QFPU_QMA,       //  7
                          PEVT_INST_QFPU_QRND,      //  8
                          PEVT_INST_QFPU_QCVT,      //  9
                          PEVT_INST_QFPU_QCMP,      // 10
                          PEVT_INST_QFPU_FPGRP1};   // 11

// HPM_GROUP=40
// qpx FPU instructions, can count 1-4 threads/core
static int fpSet10[6] = { PEVT_INST_ALL,            //  0   has qpx fpu counters 
                          PEVT_INST_QFPU_ALL,       //  1
                          PEVT_INST_QFPU_QMOV,      //  2
                          PEVT_INST_QFPU_QOTH,      //  3
                          PEVT_INST_QFPU_QADD,      //  4
                          PEVT_INST_QFPU_QMUL};     //  5

// HPM_GROUP=41
// qpx FPU instructions, can count 1-4 threads/core
static int fpSet11[6] = { PEVT_INST_QFPU_QRES,      //  0  has qpx fpu counters and more
                          PEVT_INST_QFPU_QMA,       //  1
                          PEVT_INST_QFPU_QRND,      //  2
                          PEVT_INST_QFPU_QCVT,      //  3
                          PEVT_INST_QFPU_QCMP,      //  4
                          PEVT_INST_QFPU_FPGRP1};   //  5


// HPM_GROUP=5
// all FXU counters, can count only 1 thread/core
static int fxuSet[24] = { PEVT_INST_XU_FLD,         //  0
                          PEVT_INST_XU_FST,         //  1
                          PEVT_INST_XU_QLD,         //  2
                          PEVT_INST_XU_QST,         //  3
                          PEVT_INST_XU_BITS,        //  4
                          PEVT_INST_XU_BRC,         //  5
                          PEVT_INST_XU_BRU,         //  6
                          PEVT_INST_XU_CINV,        //  7
                          PEVT_INST_XU_CSTO,        //  8
                          PEVT_INST_XU_CTCH ,       //  9
                          PEVT_INST_XU_IADD,        // 10
                          PEVT_INST_XU_ICMP,        // 11
                          PEVT_INST_XU_IDIV,        // 12
                          PEVT_INST_XU_ILOG,        // 13
                          PEVT_INST_XU_IMOV,        // 14
                          PEVT_INST_XU_IMUL,        // 15
                          PEVT_INST_XU_INT,         // 16
                          PEVT_INST_XU_LD,          // 17
                          PEVT_INST_XU_ST,          // 18
                          PEVT_INST_XU_LSCX,        // 19
                          PEVT_INST_XU_SYNC,        // 20
                          PEVT_INST_XU_TLB,         // 21
                          PEVT_INST_XU_TLBI,        // 22
                          PEVT_INST_XU_ALL};        // 23

// HPM_GROUP=6
// first-half of FXU counters, can count 1-2 threads/core
static int fxSet0[12] = { PEVT_INST_XU_FLD,         //  0
                          PEVT_INST_XU_FST,         //  1
                          PEVT_INST_XU_QLD,         //  2
                          PEVT_INST_XU_QST,         //  3
                          PEVT_INST_XU_BITS,        //  4
                          PEVT_INST_XU_BRC,         //  5
                          PEVT_INST_XU_BRU,         //  6
                          PEVT_INST_XU_CINV,        //  7
                          PEVT_INST_XU_CSTO,        //  8
                          PEVT_INST_XU_CTCH ,       //  9
                          PEVT_INST_XU_IADD,        // 10
                          PEVT_INST_XU_ICMP};       // 11

// HPM_GROUP=60
// 1st-quarter of FXU counters, can count 1-4 threads/core
static int fxSet00[6] = { PEVT_INST_XU_FLD,         //  0
                          PEVT_INST_XU_FST,         //  1
                          PEVT_INST_XU_QLD,         //  2
                          PEVT_INST_XU_QST,         //  3
                          PEVT_INST_XU_BITS,        //  4
                          PEVT_INST_XU_BRC};        //  5


// HPM_GROUP=61
// 2nd-quarter of FXU counters, can count 1-4 threads/core
static int fxSet01[6] = { PEVT_INST_XU_BRU,         //  0
                          PEVT_INST_XU_CINV,        //  1
                          PEVT_INST_XU_CSTO,        //  2
                          PEVT_INST_XU_CTCH ,       //  3
                          PEVT_INST_XU_IADD,        //  4
                          PEVT_INST_XU_ICMP};       //  5

// HPM_GROUP=7
// second-half of FXU counters, can count 1-2 threads/core
static int fxSet1[12] = { PEVT_INST_XU_IDIV,        //  0
                          PEVT_INST_XU_ILOG,        //  1
                          PEVT_INST_XU_IMOV,        //  2
                          PEVT_INST_XU_IMUL,        //  3
                          PEVT_INST_XU_INT,         //  4
                          PEVT_INST_XU_LD,          //  5
                          PEVT_INST_XU_ST,          //  6
                          PEVT_INST_XU_LSCX,        //  7
                          PEVT_INST_XU_SYNC,        //  8
                          PEVT_INST_XU_TLB,         //  9
                          PEVT_INST_XU_TLBI,        // 10
                          PEVT_INST_XU_ALL};        // 11

// HPM_GROUP=70
// 3rd-quarter of FXU counters, can count 1-4 threads/core
static int fxSet10[6] = { PEVT_INST_XU_IDIV,        //  0
                          PEVT_INST_XU_ILOG,        //  1
                          PEVT_INST_XU_IMOV,        //  2
                          PEVT_INST_XU_IMUL,        //  3
                          PEVT_INST_XU_INT,         //  4
                          PEVT_INST_XU_LD};         //  5

// HPM_GROUP=71
// 4th-quarter of FXU counters, can count 1-4 threads/core
static int fxSet11[6] = { PEVT_INST_XU_ST,          //  0
                          PEVT_INST_XU_LSCX,        //  1
                          PEVT_INST_XU_SYNC,        //  2
                          PEVT_INST_XU_TLB,         //  3
                          PEVT_INST_XU_TLBI,        //  4
                          PEVT_INST_XU_ALL};        //  5

// HPM_GROUP=81
// L1P counters
static int l1pset0[6] = { PEVT_L1P_BAS_STRM_LINE_ESTB,    //  0
                          PEVT_L1P_BAS_HIT,               //  1
                          PEVT_L1P_BAS_PF2DFC,            //  2
                          PEVT_L1P_BAS_MISS,              //  3
                          PEVT_L1P_BAS_LU_DRAIN,          //  4
                          PEVT_L1P_BAS_LD};               //  5

// HPM_GROUP=82
// L1P counters
static int l1pset1[6] = { PEVT_L1P_SW_SR_MAS_RD_VAL_2,    //  0
                          PEVT_L1P_SW_SI_MAS_REQ_VAL_2,   //  1
                          PEVT_L1P_SW_SW_MAS_SKED_VAL_2,  //  2
                          PEVT_L1P_SW_MAS_SW_REQ_VAL,     //  3
                          PEVT_L1P_SW_MAS_SW_REQ_GATE,    //  4
                          PEVT_L1P_SW_MAS_SW_DATA_GATE};  //  5

// HPM_GROUP=83
// L1P counters
static int l1pset2[6] = { PEVT_L1P_BAS_ST_WC,             //  0
                          PEVT_L1P_BAS_ST_32BYTE,         //  1
                          PEVT_L1P_BAS_ST_CRACKED,        //  2
                          PEVT_L1P_SW_SW_MAS_EAGER_VAL_2, //  3
                          PEVT_L1P_SW_MAS_SW_REQ_GATE,    //  4
                          PEVT_L1P_SW_MAS_SW_DATA_GATE};  //  5
                          
/*===================================================================*/
/* routine to set an aggregation mask for counters that count cycles */
/*===================================================================*/
void set_aggregation_mask(void)
{
   mask[PEVT_IU_AXU_FXU_DEP_HIT_CYC] = 1;
}

/*===========================================*/
/* Find the code-block number from the label.*/
/*===========================================*/
int index_from_label(char * this_label)
{
   int i, match;
   char * ptr;

   i = 0;
   if (code_block < MAX_CODE_BLOCKS)
   {
       match = 0; 
       for (i=code_block-1; i>=0; i--)
       {
           if (0 == fwext_strcmp(code_block_label[i], this_label))
           {
	     // printf("matched \n");
               match = 1;
               break;
           }
       }
    
       if (match == 0)
       {
           i = code_block;
	   // fixme
	   ptr = hpm_strcpy(code_block_label[i], this_label);
           if (ptr == NULL) code_block_label[i][0] = '\0';
           code_block ++;
       }
   }

   // fixme
   // code_block = 1;
   // i = 0;
   return i;

}

/*====================================================*/
/* routine to set labels for each of the BGQ counters */
/*====================================================*/
void set_labels(void)
{
  unsigned int i;
  for (i=0; i<=PEVT_LAST_EVENT; i++) {
    hpm_strcpy(label[i], Upci_GetEventDesc(i) );
  }

}


/*===============================================================*/
/* Stop counting for a particular code block, just using myrank. */
/*===============================================================*/
void HPM_Stop(char * this_label)
{
   int i, j, k;
   long long counter_value, tb;

   unsigned int tid, pid, cid;
   tid = PhysicalThreadID();    // between 0 and 3
   pid = PhysicalThreadIndex(); // between 0 and 67
   cid = pid/4;


   if (pid == 0)
     {

       if (code_block >= MAX_CODE_BLOCKS) return;

       tb = GetTimeBase();

       j = index_from_label(this_label);

       for (k=0; k<MAX_CORES; k++) {
	 if (coremask[k]) {
	   for (i=0; i<num_counters; i++) {
	     counter_value = Upci_Punit_Event_Read(&Punit[k], eventHandle[k][i]);
	     counter_sum[k][j][i] += (counter_value - counter_in[k][j][i]);
	   }
	 }
       }

       L2_sum[j][0] += UPC_L2_ReadCtr(0, UPC_L2_CTR_HITS)      - L2_in[j][0];
       L2_sum[j][1] += UPC_L2_ReadCtr(0, UPC_L2_CTR_MISSES)    - L2_in[j][1];
       L2_sum[j][2] += UPC_L2_ReadCtr(0, UPC_L2_PREFETCH)      - L2_in[j][2];
       L2_sum[j][3] += UPC_L2_ReadCtr(0, UPC_L2_FETCH_LINE)    - L2_in[j][3];
       L2_sum[j][4] += UPC_L2_ReadCtr(0, UPC_L2_STORE_LINE)    - L2_in[j][4];
       L2_sum[j][5] += UPC_L2_ReadCtr(0, UPC_L2_STORE_PARTIAL) - L2_in[j][5];
       
       block_stops[j] += 1;

       timebase_sum[j] += tb - timebase_in[j];

       // printf("entered stop by %d\n", pid);
#ifdef DEBUG
       Upci_A2PC_Val_t a2qry; 
       Kernel_Upci_A2PC_GetRegs(&a2qry); 
       Upci_A2PC_DumpRegs(&a2qry); 
       UPC_P_Dump_State(Kernel_ProcessorCoreID()); 
       Upci_Punit_Dump(0, &Punit[0]);
       UPC_C_Dump_State();
       UPC_C_Dump_Counters(0,1);
       for(i=0; i<24; i++) {
        Upci_Punit_EventH_Dump(0, i, &eventHandle[0][i]);
       }
#endif
     }
     

   return;
}

/*================================================================*/
/* Start counting for a particular code block */
/*================================================================*/
void HPM_Start(char * this_label)
{
   int i, j, k;
   long long tb;
   unsigned int tid, pid, cid;

   tid = PhysicalThreadID();    // between 0 and 3
   pid = PhysicalThreadIndex(); // between 0 and 67
   cid = pid/4;

   if (pid == 0)
     {

       tb = GetTimeBase();

       j = index_from_label(this_label);

       for (k=0; k<MAX_CORES; k++) {
	 if (coremask[k]) {
	   for (i=0; i<num_counters; i++) {
	     counter_in[k][j][i] = Upci_Punit_Event_Read(&Punit[k], eventHandle[k][i]);
	   }
	 }
       }

       L2_in[j][0] = UPC_L2_ReadCtr(0, UPC_L2_CTR_HITS);
       L2_in[j][1] = UPC_L2_ReadCtr(0, UPC_L2_CTR_MISSES);
       L2_in[j][2] = UPC_L2_ReadCtr(0, UPC_L2_PREFETCH);
       L2_in[j][3] = UPC_L2_ReadCtr(0, UPC_L2_FETCH_LINE);
       L2_in[j][4] = UPC_L2_ReadCtr(0, UPC_L2_STORE_LINE);
       L2_in[j][5] = UPC_L2_ReadCtr(0, UPC_L2_STORE_PARTIAL);

       // printf("entered start by %d\n", pid);

       block_starts[j] += 1;

       timebase_in[j] = tb;
     }

   return;
}

int EMON_rank_on_card()
{
  // return the rank of the  node, on a 2^5 card
   
  int a,b,c,d,e, na,nb,nc,nd,ne, node_id;
  Personality_t *pers;
  pers = fwext_getPersonality();
  a = pers->Network_Config.Acoord %2;
  b = pers->Network_Config.Bcoord %2;
  c = pers->Network_Config.Ccoord %2;
  d = pers->Network_Config.Dcoord %2;
  e = pers->Network_Config.Ecoord %2;
  na = nb= nc =nd = ne = 2;  // 2x2x2x2x2 card
  
  node_id = a+na*(b+nb*(c+nc*(d+nd*e)));
  return node_id;
  


}





void EMON_Init_t(int numthreads)
{
  unsigned int tid, pid, cid,node_id;

  tid = PhysicalThreadID();    // between 0 and 3
  pid = PhysicalThreadIndex(); // between 0 and 67
  cid = pid/4;
  node_id = EMON_rank_on_card();
  //  printf("node id = %d\n", node_id);
  

  // EMON set up is done only by one thread of one node
  if (pid == 0)// && (node_id == 0) )
    {
      int rc = EMON_SetupPowerMeasurement();

      if (rc) {
      	printf("ERROR : EMON_SetupPowerMeasurement failed with rc=%d\n", rc);
      	test_exit(rc);
      }
    }
     L2_Barrier(&id_barrier, numthreads);
}




void EMON_Print_t(int numthreads)
{
  unsigned int tid, pid, cid;
  int node_id;


  tid = PhysicalThreadID();    // between 0 and 3
  pid = PhysicalThreadIndex(); // between 0 and 67
  cid = pid/4;
  node_id =  EMON_rank_on_card( );

  //  printf("node id = %d\n", node_id);
  

  // EMON set up is done only by one thread of one node
  if ((pid == 0) && ( node_id  == 0) )
    {

      double power = EMON_ReportPower();
      if (power < 0) {
     	printf("ERROR : EMON_GetPower failed\n");
     	test_exit(1);
      }
      
    }
}


/*=================================================================*/
/* Initialize the counters, collective for all ranks in comm world */
/*=================================================================*/
void HPM_Init_t(int numthreads)
{
  int i, j, k, core;
  // int threads_per_core;
  //  int * eventSet;
  char * ptr;
  unsigned int tid, pid, cid;
  unsigned int lock_status;

  int rc;
//  Upci_Mode_t Mode;

  tid = PhysicalThreadID();    // between 0 and 3
  pid = PhysicalThreadIndex(); // between 0 and 67
  cid = pid/4;

  if (pid == 0)
    {
      // set the initial cumulative counter values to zero 
      for (k=0; k<MAX_CORES; k++)
	for (j=0; j<MAX_CODE_BLOCKS; j++)
	  for (i=0; i<MAX_COUNTERS; i++)
	    counter_sum[k][j][i] = 0LL;
	
      for (j=0; j<MAX_CODE_BLOCKS; j++) timebase_sum[j] = 0LL;
	
      for (j=0; j<MAX_CODE_BLOCKS; j++) 
	for (i=0; i<6; i++)
	  L2_sum[j][i] = 0LL;
	
      // keep track of code block starts and stops 
      for (j=0; j<MAX_CODE_BLOCKS; j++) {
	block_starts[j] = 0;
	block_stops[j]  = 0;
      }
	
      // set mask used for thread and core aggregation
      for (i=0; i<MAX_EVENTS; i++) mask[i] = 0;
	
      // check env variables
      // fixme
      ptr = fwext_getenv("HPM_GROUP");
      if (ptr == NULL)  {
        hpm_group = 0;
      }
      else hpm_group = hpm_atoi(ptr);
      // printf("hpm_group = %d\n", hpm_group);
      // hpm_group = 82;
      if (hpm_group < -1) hpm_group = 0;
      if (hpm_group > 99) hpm_group = 0;
	
      // fixme
      // ptr = fwext_getenv("HPM_SCOPE");   if (pid !=0) return;
      
      // if (ptr != NULL) {
      //          if (strncasecmp(ptr,"process", 7) == 0) process_scope = 1;
      //          if (strncasecmp(ptr,"node", 4) == 0)    node_scope = 1;
      // }
	
      // fixme
      // ptr = fwext_getenv("HPM_METRICS");
      // if (ptr != NULL) {
      //          if (strncasecmp(ptr,"yes", 3) == 0) derived_metrics = 1;
      // }
	
      for (i=0; i<MAX_CORES; i++) coremask[i] = 1;
	
      // find the number of cores used by this process
      // fixme
      // numcores = 0;
      // for (i=0; i<MAX_CORES; i++) numcores += coremask[i];
      numcores = 17;
	
      // determine the number of threads per core
      // numthreads = BgGetNumThreads();
      // numthreads = 68;
      // threads_per_core = numthreads / numcores;
	
      // hpm_threads = threads_per_core;
      // fixme
      hpm_threads = 4;
      	
      // optionally reset the number of threads per core that will be counted
      // fixme
      // ptr = fwext_getenv("HPM_THREADS");
      // if (ptr != NULL) {
      //         hpm_threads = fwext_atoi(ptr);
      // if (hpm_threads < 1) hpm_threads = 1;
      // if (hpm_threads > 4) hpm_threads = 4;
      // }
	
      // set num_events and num_counters based on hpm_group and hpm_threads
      switch (hpm_group) {
      case -1:
	num_events = 6;
	eventSet = exptSet;
	break;
	
      case 0:
	num_events = 6;
	eventSet = mySet;
	break;
	
      case 1:
	num_events = 12;
	if (hpm_threads > 2) hpm_threads = 2;
	eventSet = ldSet;
	break;
	
      case 2:
	num_events = 24;
	if (hpm_threads > 1) hpm_threads = 1;
	eventSet = fpuSet;
	break;
	
      case 3:
	num_events = 12;
	if (hpm_threads > 2) hpm_threads = 2;
	eventSet = fpSet0;
	break;
	
      case 30:
	num_events = 6;
	eventSet = fpSet00;
	break;
	
      case 31:
	num_events = 6;
	eventSet = fpSet01;
	break;
	
      case 4:
	num_events = 12;
	if (hpm_threads > 2) hpm_threads = 2;
	eventSet = fpSet1;
	break;
	
      case 40:
	num_events = 6;
	eventSet = fpSet10;
	break;
	
      case 41:
	num_events = 6;
	eventSet = fpSet11;
	break;
	
      case 5:
	num_events = 24;
	if (hpm_threads > 1) hpm_threads = 1;
	eventSet = fxuSet;
	break;
	
      case 6:
	num_events = 12;
	if (hpm_threads > 2) hpm_threads = 2;
	eventSet = fxSet0;
	break;
	
      case 60:
	num_events = 6;
	eventSet = fxSet00;
	break;
	
      case 61:
	num_events = 6;
	eventSet = fxSet01;
	break;
	
      case 7:
	num_events = 12;
	if (hpm_threads > 2) hpm_threads = 2;
	eventSet = fxSet1;
	break;
	
      case 70:
	num_events = 6;
	eventSet = fxSet10;
	break;
	
      case 71:
	num_events = 6;
	eventSet = fxSet11;
	break;

      case 81:
	num_events = 6;
	eventSet = l1pset0;
	break;

      case 82:
	num_events = 6;
	eventSet = l1pset1;
	break;

      case 83:
	num_events = 6;
	eventSet = l1pset2;
	break;
	
      default:
	break;
	
      }
	
      num_counters =  num_events * hpm_threads;
      ppc_msync();
      Upci_Mode_Init(&Mode[0], UPC_DISTRIB_MODE, UPC_CM_INDEP, 0);
      initialized = 1;
      ppc_msync();
    }
      
  
  while ((initialized == 0) && (tid == 0))
    {
      ;	
    }

  if (tid == 0) {

    lock_status = 0;
    while (lock_status == 0)
      {
	lock_status = hpm_lock_acquire();
      }
       
    core = cid;

    // initialize hardware counters
    // Upci_Mode_Init(&Mode[core], UPC_DISTRIB_MODE, UPC_CM_INDEP, core);
    Upci_Punit_Init(&Punit[core], &Mode[core], core);

    // UPC_L1p_SetMode(core, L1P_CFG_UPC_SWITCH);
       
    // use one thread per core to enable 24 different punit counters
       
    // add events to count, save hwthread in one of the reserved event handle slots
    k = 0;
    for (i=0; i<num_events; i++) {                         // hwthread 0
      rc = Upci_Punit_AddEvent(&Punit[core], eventSet[i],  0, &eventHandle[core][k]);
      if (rc != 0) printf("failed to add event %d\n", eventSet[i]);
      if (pid == 0)
	counter_index[k] = eventSet[i];
      eventHandle[core][k].rsv[0] = 0;
      k++;
    }
    if (hpm_threads > 1) {
      for (i=0; i<num_events; i++) {                         // hwthread 2
	rc = Upci_Punit_AddEvent(&Punit[core], eventSet[i],  2, &eventHandle[core][k]);
	if (rc != 0) printf("failed to add event %d\n", eventSet[i]);
	if (pid == 0)
	  counter_index[k] = eventSet[i];
	eventHandle[core][k].rsv[0] = 2;
	k++;
      }
    }
    if (hpm_threads > 2) {
      for (i=0; i<num_events; i++) {                         // hwthread 1
	rc = Upci_Punit_AddEvent(&Punit[core], eventSet[i],  1, &eventHandle[core][k]);
	if (rc != 0) printf("failed to add event %d\n", eventSet[i]);
	if (pid == 0)
	  counter_index[k] = eventSet[i];
	eventHandle[core][k].rsv[0] = 1;
	k++;
      }
    }
    if (hpm_threads > 3) {
      for (i=0; i<num_events; i++) {                         // hwthread 3
	rc = Upci_Punit_AddEvent(&Punit[core], eventSet[i],  3, &eventHandle[core][k]);
	if (rc != 0) printf("failed to add event %d\n", eventSet[i]);
	if (pid == 0)
	  counter_index[k] = eventSet[i];
        eventHandle[core][k].rsv[0] = 3;
	k++;
      }
    }
    

    rc = Upci_Punit_Apply(&Punit[core]);
    if (rc != 0) printf("Upci_Punit_Apply failed\n");
    
    Upci_Punit_Start(&Punit[core], (UPCI_CTL_RESET | UPCI_CTL_DELAY));
    // printf("Initialised upc by core = %d\n", cid);
    // Upci_Punit_Dump(2, &Punit[core]);

    lock_val = 0;
    ppc_msync();
    
  }

  if (pid == 0)
    {
      UPC_L2_EnableUPC(1, 1);  
      UPC_L2_Start();
    }

  // PMPI_Barrier(local_comm);

  L2_Barrier(&id_barrier, numthreads);
     
  return;
}

void HPM_EMON_Init_t(numthreads)
{
  EMON_Init_t(numthreads);
  HPM_Init_t(numthreads);
}


/*========================================================================*/
/* Print the counter values with event labels, collective on a given node */
/*========================================================================*/
void HPM_Print_t(int numthreads)
{
   int i, j, k, nblocks;
   //   uint64_t counts, counts_0, counts_1, counts_2, counts_3;
   long long counts, counts_0, counts_1, counts_2, counts_3;
   //   uint64_t l1_misses, l1p_misses, node_l1p_misses, node_l2_misses, loads, node_loads, node_l1_misses;
   //   double cycles, ipc, stall_cycles, node_stall_cycles, node_cycles;
   //   double node_fxu_instructions, node_fpu_instructions, node_l1_hits, node_l1p_hits, ddr_hit_fraction;
   //   double fxu_fraction, fpu_fraction, fxu_instructions, fpu_instructions;
   //   double cores_per_process, max_fraction, percent_max_issue_rate;
   //   double l1_hits, l1_hit_fraction, l1p_hits, l1p_hit_fraction, node_l2_hits, l2_hit_fraction;
   //   double ld_bytes_per_cycle, st_bytes_per_cycle;
   //   uint64_t node_punit_counts[MAX_COUNTERS];
   //   long long node_timebase_sum;
   //   int Ax, Bx, Cx, Dx, Ex;
   //   Personality_t personality;
   //   char filename[132];
   //   FILE * fp;
   unsigned int tid, pid, cid;
   int node_id;
 

   tid = PhysicalThreadID();    // between 0 and 3
   pid = PhysicalThreadIndex(); // between 0 and 67
   cid = pid/4;
   node_id = EMON_rank_on_card();
   L2_Barrier(&id_barrier, numthreads);

   if ((pid != 0) || (node_id != 0))
	return;

   set_labels();

   set_aggregation_mask();  // sets a mask to aggregate by sum (0) or by max (1)

   if (code_block >= MAX_CODE_BLOCKS) nblocks = MAX_CODE_BLOCKS;
   else                               nblocks = code_block;

   //   fp = stderr;

   // print counts for each thread and the aggregate for every core
   printf( "\n");
   printf( "======================================================================\n");
   printf( "Hardware counter report for BGQ  - thread and core-specific values.\n");
   printf( "======================================================================\n");
   for (k=0; k<MAX_CORES; k++) {
      if (coremask[k]) {
         printf ("core %d\n", k);
         for (j=0; j<nblocks; j++) { 
            if (block_starts[j] == block_stops[j]) {
               printf( "----------------------------------------------------------------\n");
               printf( "%s, call count = %d, cycles = %lld :\n", 
                       code_block_label[j], block_starts[j], timebase_sum[j]);
               printf( "  -- Processor counters (thread specific) --------------\n");
               if (hpm_threads == 1) {
                  for (i=0; i<num_events; i++) {
                     counts = counter_sum[k][j][i];
                     printf( "%-d %14lld  %s\n", hpm_group, counts, label[counter_index[i]]);
                  }
               }
               else if (hpm_threads == 2) {
                  printf( "  thread0 counts  thread2 counts      net counts  label\n");
                  for (i=0; i<num_events; i++) {
                     counts_0 = counter_sum[k][j][i];
                     counts_2 = counter_sum[k][j][i+num_events];
                     if (mask[counter_index[i]]) {
                          counts = (counts_0 > counts_2) ? counts_0 : counts_2;
                     }
                     else counts = counts_0 + counts_2;
                     printf( "%-d %14lld  %14lld  %14lld  %s\n", 
                                   hpm_group, counts_0, counts_2, counts, label[counter_index[i]]);
                  }
               }
               else if (hpm_threads == 3) {
                  printf( "  thread0 counts  thread1 counts  thread2 counts      net counts  label\n");
                  for (i=0; i<num_events; i++) {
                     counts_0 = counter_sum[k][j][i];
                     counts_2 = counter_sum[k][j][i+num_events];
                     counts_1 = counter_sum[k][j][i+2*num_events];
                     if (mask[counter_index[i]]) {
                          counts = (counts_0 > counts_2) ? counts_0 : counts_2;
                          counts = (counts   > counts_1) ? counts   : counts_1;
                     }
                     else counts = counts_0 + counts_1 + counts_2;
                     printf( "%-d %14lld  %14lld  %14lld  %14lld  %s\n", 
                                   hpm_group, counts_0, counts_1, counts_2, counts, label[counter_index[i]]);
                  }
               }
               else if (hpm_threads == 4) {
                  printf( "  thread0 counts  thread1 counts  thread2 counts  thread3 counts      net counts  label\n");
                  for (i=0; i<num_events; i++) {
                     counts_0 = counter_sum[k][j][i];
                     counts_2 = counter_sum[k][j][i+num_events];
                     counts_1 = counter_sum[k][j][i+2*num_events];
                     counts_3 = counter_sum[k][j][i+3*num_events];
                     if (mask[counter_index[i]]) {
                          counts = (counts_0 > counts_2) ? counts_0 : counts_2;
                          counts = (counts   > counts_1) ? counts   : counts_1;
                          counts = (counts   > counts_3) ? counts   : counts_3;
                     }
                     else counts = counts_0 + counts_1 + counts_2 + counts_3;
                     printf( "%-d %14lld  %14lld  %14lld  %14lld  %14lld  %s\n", 
                                   hpm_group, counts_0, counts_1, counts_2, counts_3, counts, label[counter_index[i]]);
                  }
               }
               printf( "  -- L2 counters (shared for the node) -----------------\n");
               printf( "%-d %14lld  L2 Hits\n",                          100, L2_sum[j][0]);
               printf( "%-d %14lld  L2 Misses\n",                        100, L2_sum[j][1]);
               printf( "%-d %14lld  L2 lines prefetched\n",              100, L2_sum[j][2]);
               printf( "%-d %14lld  L2 lines loaded from memory\n",      100, L2_sum[j][3]);
               printf( "%-d %14lld  L2 full lines stored to mem\n",      100, L2_sum[j][4]);
               printf( "%-d %14lld  L2 partial lines stored to mem\n",   100, L2_sum[j][5]);
               printf( "\n");
            }
            else {
               printf( "mismatch in starts/stops for code block '%s'\n", code_block_label[j]);
               printf( "  starts = %d\n", block_starts[j]);
               printf( "  stops  = %d\n", block_stops[j]);
            }
         }
         printf( "\n");
      }
   }

   return;
}

void HPM_EMON_Print_t(int numthreads)
{
  EMON_Print_t(numthreads);
  HPM_Print_t(numthreads);
}


void EMON_Init()
{
EMON_Init_t(BgGetNumThreads());
}

void EMON_Print()
{
EMON_Print_t(BgGetNumThreads());
}

void HPM_Init()
{
HPM_Init_t(BgGetNumThreads());
}

void HPM_Print()
{
  HPM_Print_t(BgGetNumThreads());
}

void HPM_EMON_Init()
{
  EMON_Init_t(BgGetNumThreads());
  HPM_Init_t(BgGetNumThreads());
}

void HPM_EMON_Print()
{
  EMON_Print_t(BgGetNumThreads());
  HPM_Print_t(BgGetNumThreads());
}


int hpm_lock_acquire()
 {
   int a, n;
   a = LoadReserved32(&lock_val);
   if (a==0)
     {
       n = StoreConditional32(&lock_val, 1);
       return(n);
     }
   return(0);
 }

// int hpm_barrier_atomic_inc_no_match_all(){

//   int id;
//   id=PhysicalThreadIndex();
//   uint64_t* volatile ptr;
//   int numt;

//   numt = BgGetNumThreads();
//   ptr=(uint64_t *)(0x10000000000+(2<<3)+((id&3) <<6)+(((uint64_t)atomic_ptr_4all)<<5)); // get the base for the atomics
//   *ptr=1; // atomic increment 

//  atomic_match_4all[id]+=numt;
//  while((uint64_t volatile )*atomic_ptr_4all < atomic_match_4all[id]){}
//  return(0);

// }

int hpm_atoi(char *s)
{
  int i, n;
  n=0;
  for(i=0; s[i] >='0' && s[i]<='9'; i++)
    n= 10*n + s[i] - '0';
  return(n);
}

char* hpm_strcpy(char *s, const char *t)
{
  int i = 0;
  while ((*s++ = *t++) != '\0')
    {
      i++;
    }
  if (i == 0)
    return(NULL);
  else
    return(s);
}
