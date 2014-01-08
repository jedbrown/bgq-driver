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



#ifndef UPC_EVTSRC
#define UPC_EVTSRC(name, mnemonic, description)
#endif

    UPC_EVTSRC(UPC_ES_Undef, "undef", "Undefined source value")

    // The following 5 source entries must remain in this order and
    // probably consistent with UPC_P Counter Sub Event Groups defined in upc_p.h (about line 47)
    // Because: list used to index into arrays with starting offset at UPC_ES_A2_MMU.
    UPC_EVTSRC(UPC_ES_A2_MMU, "MMU", "Memory Management Unit Events")
    UPC_EVTSRC(UPC_ES_A2_AXU, "AXU", "Axu Execution Unit Events (Quad Processing Unit)")
    UPC_EVTSRC(UPC_ES_A2_IU,  "IU",  "Instruction Unit Events")
    UPC_EVTSRC(UPC_ES_A2_LSU, "LSU", "Load Store unit events")
    UPC_EVTSRC(UPC_ES_A2_XU,  "XU",  "Execution Unit Events")

    // Keep group in this order
    UPC_EVTSRC(UPC_ES_L1P_BASE,   "L1PBASE", "L1p Base Event")
    UPC_EVTSRC(UPC_ES_L1P_SWITCH, "L1PSW",   "L1p Switch Event")
    UPC_EVTSRC(UPC_ES_L1P_STREAM, "L1PSTR",  "L1p Stream Event")
    UPC_EVTSRC(UPC_ES_L1P_LIST,   "L1PLIST", "L1p List Event")
    UPC_EVTSRC(UPC_ES_WAKEUP,     "WAKEUP",  "Wakeup Event")

    // keep group in this order
    UPC_EVTSRC(UPC_ES_OP_XU,      "OPXU",  "XU Opcode Counting")
    UPC_EVTSRC(UPC_ES_OP_AXU,     "OPAXU", "AXU Opcode Counting")
    UPC_EVTSRC(UPC_ES_OP_FP_OPS,  "OPFPOPS", "AXU Opcode Floating Point Operations Mixes")
    UPC_EVTSRC(UPC_ES_OP_FP_INSTR,"OPFPINSTR", "AXU Opcode Floating Point Instruction Mixes")
    UPC_EVTSRC(UPC_ES_OP_ALL,     "OPALL", "All Opcode Counts (both AXU and XU Instructions)")

    // keep group in this order
    UPC_EVTSRC(UPC_ES_L2,         "L2",    "L2 Event")
    UPC_EVTSRC(UPC_ES_L2_SLICE,   "L2SLC", "L2 Unit From Selected Slices Event")

    // keep group in this order
    UPC_EVTSRC(UPC_ES_PCIE,       "PCIE",   "PCIe I/O Unit Events")
    UPC_EVTSRC(UPC_ES_DEVBUS,     "DEVBUS", "DevBus I/O Unit Events")
    UPC_EVTSRC(UPC_ES_MU,         "MU",     "Message Unit  Events")

    UPC_EVTSRC(UPC_ES_NW,         "NW",     "Network Unit Events")

    UPC_EVTSRC(UPC_ES_CNKNODE,    "CNKNODE", "Compute Node Kernel Node-Wide Events")
    UPC_EVTSRC(UPC_ES_CNKPROC,    "CNKPROC", "Compute Node Kernel Process Events")
    UPC_EVTSRC(UPC_ES_CNKHWT,     "CNKHWT",  "Compute Node Kernel Hardware Thread Events")


