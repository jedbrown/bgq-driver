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
/*!
 * \page bgpm_hwfeatures UPC Hardware Features
 *
 * This is a summary of the features provided by the BGPM and UPC hardware.
 * The hardware complexities are mostly hidden by the software interface.
 *
 *
 * The UPC (Universal Performance Counting) function in BG/Q
 * is capable of collecting hardware
 * performance events from multiple modules within a node,
 * as shown in the figure below.
 * - Seventeen A2 CPU cores, L1P and Wakeup units
 * - Sixteen L2 units (memory slices)
 * - The Message, PCIe, and Devbus units (collectively referred to as I/O Units)
 *
 * \image html upc_ring.gif "BlueGene/Q UPC Hardware Modules"
 *
 * The base collection modules are divided into the UPC_P and UPC_C.
 *
 * A local UPC unit (UPC_P) is instantiated within each processor complex.
 * The UPC_P provides 24 14-bit counters,
 * with functions to count A2, L1P, Wakeup event signal edges and levels,
 * completed instructions and floating point operations.
 * In addition, it directs threaded performance monitor interrupts
 * to the A2 core.
 * Similarly, there are UPC units associated with each L2 slice (UPC_L2),
 * each containing 6 implemented event counters,
 * and I/O units with collectively 43 counters.
 *
 *
 *
 * Normally, counters from each of the UPC units are periodically accumulated into a
 * corresponding 64-bit counter within a central UPC module (UPC_C).
 * The UPC_C, in coordination with the UPC_P, provides overflow detection and
 * counter aggregation operations.
 *
 * The UPC_P / UPC_C provide 3 hardware modes of collection:
 * - Distributed (which collects the 24 counters from each UPC_P complex)
 * - Detailed (which can sample 116 event signals from one UPC_P complex) (not used under Bgpm)
 * - Trace (which triggers and captures a limited trace of 44 bits of A2 debug bus activity) (not useful)
 *
 *
 * The following diagram zooms in on the processor event selection logic configurable by the Punit software component.  
 * This logic is duplicated for each of 17 A2 processors per BGQ node. 
 * The selected events are counted by the 24 counters available in the UPC_P hardware component.  
 * The boxes in green represent registers accessible by the Punit code.  
 * The A2 processor core contains event selection registers for each of 5 of it's internal units:  FU, XU, IU, LSU, and MMU.  
 * Only 2 events per thread are routable concurrently if all hw threads are in use. 
 * The registers in the UPC_P and L1P/Wakeup units are accessible via MMIO operations.  
 * As you can see, there are greater than 600 total events to choose from, plus selected filtered opcodes, 
 * but they all contend for only 24 counter registers per UPC_P unit. 
 * Not all possible combinations of 24 events to be monitored concurrently are possible.
 * 
 * \image html upcp.jpg "A2 and UPC_P subunit to counter muxing"
 *
 *
 * \par
 *
 * The Network unit maintains a separate set of 66 64-bit counters;
 * 11 links of 6 counters each.
 * Each counter is capable of counting packets filtered by 7 virtual channel
 * values.
 * The virtual channels refer to the type of packet (user point to point, or
 * system collective, for example).
 * 4 counters per link count 32 byte packet chunks sent, and
 * two counters collect the received packets and queue length.
 * The receive counters may be used to
 * derive average queue lengths and link utilization.
 *
 *
 *
 * <h2>Up Next...</h2>
 *  \ref bgpm_units
 *
 *
 */
