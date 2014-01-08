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

#include "Kernel.h"
#include <hwi/include/bqc/testint_inlines.h>
#include <firmware/include/mailbox.h>

extern void puea_init();
extern void gea_init();
void wakeup_init();
extern int mbox_init();
extern int mbox_poll(int dopoll);
extern void File_InitFS(void);
extern void File_TermFS(void);
extern void Node_Run(void);
extern int Node_Init(void);
extern int Node_Term(void);
extern void nd_interrupt_init(void);
extern void dcrArb_init(void);
void puea_set_mc_handler_thread(int hwthreadid);

#define MULTI_NODE_MAMBO 0

#if MULTI_NODE_MAMBO
#include "callthru_config.h"
#endif

// It is an error for Kernel_Main() to return.
void Kernel_Finish( void )
{
    RASBEGIN(3);
    RASPUSH(ProcessorCoreID());
    RASPUSH(ProcessorThreadID());
    RASPUSH(GetTimeBase());
    RASFINAL(RAS_KERNELUNEXPECTEDEXIT);
    Kernel_Crash(RAS_KERNELUNEXPECTEDEXIT);
}

// technically, this is __NORETURN, but I want a stack frame.
void Kernel_Begin( void )
{
    int CoreID = ProcessorCoreID(); // 0..16
    int ThdID  = ProcessorThreadID();    // 0..3
    size_t Stk = CurrentStackAddress();
    size_t TOC = CurrentTOCAddress();

    // Prime CNK trace config with value from personality (probably a different way to do this in the future).
    NodeState.TraceConfig = Pers_GetTraceConfig();

    // You can explicitly enable a trace mask using the following:
//  NodeState.TraceConfig |= TRACE_Process;

    TRACE( TRACE_Entry, ("(I) Kernel_Main[%2d:%d]: Stack: 0x%lx. TOC: 0x%lx. Entry at 0x%016lx.\n",
                               CoreID, ThdID, Stk, TOC, GetTimeBase()) );

    // Select one thread within the node to perform the setup of the process structures and the Global Event Aggregator
    if (ProcessorID() == NodeState.NodeLeaderProcessorID)
    {
        int mchk_handler;
        if (TI_isDD1())
        {
            // Set the thread to be used by the firmware for machine check handling
            // For DD1, we must use the NodeController thread since this thread will never enter a wait instruction.
            // In DD2 we will use one of the AppAgent threads. We cannot use this in DD1 because the AppAgent threads
            // will enter a wait state if there is no AppAgent configured. Also the AppAgents themselves are allowed/capable 
            // to enter a wait instruction. A DD1 Hardware bug prevents machine checks from being reported to a thread
            // in a wait instruction.
            mchk_handler =NodeState.NodeLeaderProcessorID;
        }
        else
        {
            // Set the thread to be used by the firmware for machine check handling
            mchk_handler = NodeState.FirstAppAgentProcessorID+1;
        }

        // Set the thread to be used by the firmware for machine check handling
        puea_set_mc_handler_thread(mchk_handler);

        // Initialize the GEA
        gea_init();
        
        // Initialize the DCR Arbiter
        dcrArb_init();
        
        // Initialize mailbox stdin
        mbox_init();
        
        // Setup background scrub
        vmm_SetupBackgroundScrub();
    }
    
    // hold all threads until we get through initialization
    Kernel_Barrier(Barrier_HwthreadsInNode);

    // Initialize the L1p
    l1p_init();
    
    // Initialize the Wakeup Unit
    wakeup_init();
    
    // Initialize the BIC/PUEA 
    //  do this after the GEA has been initialized so that any pending GEA event conditions are directed to the 
    //  proper PUEA bit line and consequently the expected interrupt behavior occurs
    puea_init();

    if(ProcessorID() == NodeState.NodeLeaderProcessorID)
    {
       if((!Personality_IsFPGA()) &&
          (NodeState.Personality.Kernel_Config.NodeConfig & PERS_ENABLE_MU))
       {
          network_init();

#if MULTI_NODE_MAMBO
          // Wait for magic file to appear which tells us I/O services are ready.
          // Change the path to a file that you have access to.  Create the file when the rdma server is ready on
          // the Linux node in your multi-mambo session.
          while (MamboFileOpen((char *)"/bgq/data/usr/mmundy/ioservices.ready", 'r') != 0);
          MamboFileClose();
#endif

       }
       if (!Personality_IsFPGA())
       {
           nd_interrupt_init();
       }
    }
    
    Kernel_Barrier(Barrier_HwthreadsInNode);

    if(ProcessorID() == NodeState.NetworkManagerProcessorID)
    {
#if MULTI_NODE_MAMBO
       printf("Mambo=%d AppPreload=%d CiosEnabled=%d\n", Personality_IsMambo(), Personality_ApplicationPreLoaded(), Personality_CiosEnabled());
       uint64_t nodeconfig = GetPersonality()->Kernel_Config.NodeConfig;
       nodeconfig &= ~PERS_ENABLE_AppPreload;
       printf("nodeconfig %lx\n", nodeconfig);
       GetPersonality()->Kernel_Config.NodeConfig = nodeconfig;
#endif
       network_poll();
       // Previous call does not return

    }
    
    if (ProcessorID() == NodeState.NodeLeaderProcessorID)
    {
        // Initialize the static mapper
        vmm_setupTLBMapper();

        // Initialize job control (required once per boot -- still don't understand rollback and network cleanup issues).
        Node_Init();

        // Initialize file systems (required once per boot -- still don't understand rollback issues).
        File_InitFS();
        
        // Initialize process structures. This is required to be done once after a kernel boot
        Process_Init();

        // We are the NodeLeaderIndex. Run the node controller which polls for job control and tool messages.
        Node_Run(); 
        // Previous call does not return
    }

    // All other threads will enter the scheduler, awaiting to be configured 
    Scheduler();
    // Previous call does not return

    // Should not get here
    Kernel_Finish();
}

void Kernel_Halt( void )
{
    static int halted = 0; // protect against halt recursion
    if(halted)
    {
        Kernel_WriteFlightLog(FLIGHTLOG, FL_KERNHALTD, 1,0,0,0);
        return;
    }
    halted = 1;
    
    Kernel_WriteFlightLog(FLIGHTLOG, FL_KERNHALTD, 2,0,0,0);
    // Terminate file systems.
    File_TermFS();

    Kernel_WriteFlightLog(FLIGHTLOG, FL_KERNHALTD, 3,0,0,0);
    // Terminate job control and tool control.
    Node_Term();

    Kernel_WriteFlightLog(FLIGHTLOG, FL_KERNHALTD, 4,0,0,0);
    // Terminate network interfaces.
    network_term();

    Kernel_WriteFlightLog(FLIGHTLOG, FL_KERNHALTD, 5,0,0,0);
    // Enable "this" thread to be alive
    uint64_t threadmask[2] = { 0,0 };
    threadmask[PhysicalProcessorID()/16] = _BN(PhysicalThreadIndex()%64);
    DCRWritePriv( TESTINT_DCR(THREAD_ACTIVE0), threadmask[0]);
    DCRWritePriv( TESTINT_DCR(THREAD_ACTIVE1), threadmask[1]);
    
    // Tell the control system the node is halted.
    Kernel_WriteFlightLog(FLIGHTLOG, FL_KERNHALTD, 6,0,0,0);
    NodeState.FW_Interface.sendBlockStatus(JMB_BLOCKSTATE_HALTED,0,0);
    
    Kernel_WriteFlightLog(FLIGHTLOG, FL_KERNHALTD, 7,0,0,0);
    
    // Just spin as there is nothing else to do ...
    while (1)
    {
        mbox_poll(1);
    }
    
    // NO RETURN
}
