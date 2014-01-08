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

/*!
 *
 * \page bgpm_event_tips BGPM Event Tips
 *
 * \if HTML
\htmlonly
<div class="toc">
\endhtmlonly

<h2>Table Of Contents</h2>

- \ref index "Return to Main Page"

- \ref bgpm_tips_basic
- \ref bgpm_tips_cpi_stack
- \ref bgpm_tips_pipeline
\htmlonly
</div>
<div class="contents-right">
\endhtmlonly 

 * \endif
 *
 * \section hip_tips_metrics Performance Metrics
 * This page is a repository for tips related to using the events, rough cpi stack, and overview of 
 * the CPU pipeline. *
 *
 * \subsection bgpm_tips_basic  Basic Performance Metrics
 *
 * <b>CPI =</b> <a href="bgpm_events.html#A_PEVT_CYCLES" class="el" title="x1 cycles">PEVT_CYCLES</a> / <a href="bgpm_events.html#A_PEVT_INST_ALL" class="el" title="All Instruction Completions">PEVT_INST_ALL</a>;    <span class="comment">// cycles per instruction</span>
 *
 * <b>IPC =</b> 1 / CPI;                        <span class="comment">// Instructions per cycle</span>
 *
 * <b>Instr_Load_miss_rate    =</b> <a href="bgpm_events.html#A_PEVT_IU_IL1_MISS" class="el" title="IL1 Miss">PEVT_IU_IL1_MISS</a> / <a href="bgpm_events.html#A_PEVT_INST_ALL" class="el" title="All Instruction Completions">PEVT_INST_ALL</a>;
 *
 * <b>Instr_Load_miss_latency =</b> <a href="bgpm_events.html#A_PEVT_IU_IL1_MISS_CYC" class="el" title="IL1 Miss cycles">PEVT_IU_IL1_MISS_CYC</a> / <a href="bgpm_events.html#A_PEVT_IU_IL1_MISS" class="el" title="IL1 Miss">PEVT_IU_IL1_MISS</a>
 *
 * <b>Data_Load_miss_rate     =</b> <a href="bgpm_events.html#A_PEVT_LSU_COMMIT_LD_MISSES" class="el" title="Committed Load Misses">PEVT_LSU_COMMIT_LD_MISSES</a> / <a href="bgpm_events.html#A_PEVT_LSU_COMMIT_CACHEABLE_LDS" class="el" title="Committed Cacheable Loads">PEVT_LSU_COMMIT_CACHEABLE_LDS</a>
 *
 * 
 *
 *
 *
 *
 * \section bgpm_tips_cpi_stack CPU Core CPI Stack event relationships
 *
 * The following image illustrates a hierarchical relationships of cycles in the A2 CPU Core, 
 * 
 *  \image html cpi_stack.gif "BG/Q A2 Core CPI Stack"
 *
 * 
 * 
 *
 * \section bgpm_tips_memory_bw Memory Bandwidth
 *
 * To calculate the average memory bandwidth used by an application, you can the following events
 * from the L2. 
 *
 * - <a href="bgpm_events.html#A_PEVT_L2_FETCH_LINE" class="el" title="L2 fetch 128 byte line from main memory">PEVT_L2_FETCH_LINE</a> = # of 128-byte lines loaded from memory
 * - <a href="bgpm_events.html#A_PEVT_L2_STORE_LINE" class="el" title="L2 store 128 byte line to main memory">PEVT_L2_STORE_LINE</a> = # of 128-bytes lines stored to memory
 *
 * average memory bandwidth = (<a href="bgpm_events.html#A_PEVT_L2_FETCH_LINE" class="el" title="L2 fetch 128 byte line from main memory">PEVT_L2_FETCH_LINE</a> + <a href="bgpm_events.html#A_PEVT_L2_STORE_LINE" class="el" title="L2 store 128 byte line to main memory">PEVT_L2_STORE_LINE</a>) * 128 bytes / elapsed_time.
 *
 *
 *
 * \section bgpm_tips_pipeline CPU Core Pipeline
 *
 * To further clarify the meaning of some of the A2 CPU Events, 
 * the following is a simplified description of the CPU execution pipeline stages. 
 *
 * \image html A2pipeline.gif "BG/Q A2 Core Pipeline"
 * 
 * The front end of the pipeline consists of seven stages, IU0 through IU6. 
 * The front end is responsible for fetching instructions, predicting branches, 
 * checking for register dependencies, and arbitrating between threads for instruction issue. 
 * The back end of the pipeline consists of eight stages, RF0 - 1 and EX1 - 6. 
 * The back end is responsible for executing instructions and interfacing to the L2.
 *
 * The IU4, IU5, and IU6 stages are replicated for each thread. 
 * All other stages are shared in a fine-grain manner. 
 * Instructions from different threads are interleaved on a cycle-by-cycle basis.
 * The IU5 and IU6 stages are the major stall points in the pipeline. 
 * Instructions stall in IU5 primarily for register dependencies. 
 * Instructions stall in IU6 primarily for thread arbitration. 
 * Loads that miss the data cache can also stall in the load miss queue.
 * 
 * In the IU0 - IU4 pipeline stages, 
 * the next one to four instructions from one thread are fetched from the I-cache and decoded. 
 * Branches are predicted in the IU3 and IU4 stages.  
 * Up to eight instructions per thread are buffered in IU4 in the instruction buffer (IBUFF). 
 * Instructions are not fetched unless there is room for them in the instruction buffer. 
 * Hence, there are no stalls before IU4.
 * 
 * The single oldest instruction is decoded and sent to the IU5 stage. 
 * Register dependency checking is performed in IU5, 
 * and the instruction stalls here if input operands are not available. 
 * Instructions can stall in IU5 for a variety of other typically infrequent reasons. 
 * Because IU4 and IU5 are replicated per thread, stalls at IU5 affect only that thread.
 *
 * If the instruction is ready to issue, it is forwarded to the IU6 stage. 
 * The IU6 pipeline stage holds one ready instruction from each thread. 
 * IU6 selects one of these for issue to the XU and the FU each cycle whenever possible. 
 * Instructions can stall in IU6 for a variety of other typically infrequent reasons. 
 *
 * The last seven stages of the pipeline are unified for integer arithmetic and logic instructions, 
 * load and store instructions, and branch instructions. 
 * Register file access and bypassing is performed in RF0 and RF1.
 * Branches and most simple ALU instruction produce their results in EX1. 
 * The data cache directory and the D-ERAT are accessed in EX2. 
 * The data cache data array is accessed in EX4. 
 * Stores and loads that miss the data cache are sent to the L2 in EX6.
 *
 * Under certain circumstances, the pipeline might need to refetch and execute instructions. 
 * This is known as a flush. 
 * During a flush, all instructions up to a particular instruction are removed from the pipeline,
 * and the fetch address register is reset to the correct instruction address. 
 * Instructions from all other threads are unaffected.
 * Depending on the condition, an instruction might trigger a flush of either itself 
 * or the next instruction following itself.
 * A flush is triggered at one of five stages in the pipeline: IU2, IU5, RF0, EX1, or EX4.
 *
 *\section bgpm_tips_MU  MU Event Tips 

The MU provides various events that can be used to count traffic injected by the local node and traffic received by the local node.  Because it's a DMA interface between the local memory system and the network, the MU has no knowledge of network pass-through traffic; the network counters should be used to track that traffic, if desired.  

On the message injection side, the MU provides the  PEVT_MU_PKT_INJ event which can be used to count packets injected by this node's MU.  There is an important caveat here: PEVT_MU_PKT_INJ does not count the first packet of each message, so to get an accurate injected packet count the user must add PEVT_MU_PKT_INJ with PEVT_MU_MSG_INJ. The PEVT_MU_MSG_INJ event counts each new message that is injected by the MU into the local node's network device (ND).

  So on the injection side, a user can count total packets injected and total messages injected.  On the reception side, there is additional packet tracking fidelity since we provide three MU counters (PEVT_MU_FIFO_PKT_RCV, PEVT_MU_RGET_PKT_RCV, PEVT_MU_PUT_PKT_RCV), one for each different type of network packet (FIFO packets, remote get/rDMA read packets, and direct put/rDMA write packets). While the aforementioned event counters track packets injected and received, the MU also provides events to track the injection and reception bandwidth used by the MU's three master ports attached to the crossbar switch.  

On the reception side, the PEVT_MU_PORT0/1/2_16B_WRT events can be used to count the number of 16B writes from the MU over the crossbar switch to the shared L2 caches. Thus a user can get an exact accounting of how much memory store bandwidth is utilized to sink packets received by this node (again, this does not track any pass-through network traffic).

 On the injection side, there are three counters per master port (PEVT_MU_PORT0/1/2_32B_RD, PEVT_MU_PORT0/1/2_64B_RD, PEVT_MU_PORT0/1/2_128B_RD) since there are three load data sizes that the MU supports (32B, 64B, and 128B).  

 To calculate the total memory bandwidth used by the MU injection side, the user must consider all nine of these _RD event counts. Both MU reception and injection sides operate at half the 1.6GHz core frequency, or 800MHz.
 */


 */


