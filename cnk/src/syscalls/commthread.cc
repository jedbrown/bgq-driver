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
/* (C) Copyright IBM Corp.  2009, 2012                              */
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
#include "flih.h"
#include <hwi/include/bqc/BIC_inlines.h>
#include <hwi/include/bqc/wu_mmio.h>
#include <spi/include/kernel/thread.h>



// Generate the MU interrupt number from the PUEA status register bit index. Expected input is 
// on of the Messaging unit PUEA interrupt status bit positions: 37,38,39,40, or the 
// PUEA status bit position corresponding to the following GEA lane: COMM_MU_INT_GEA
#define GEN_MU_INTX( n ) ( ((n>40) ? KERNEL_COMM_INT_GEA : n-37) ) 


// Disable mu interrupts that have been enabled through pthread_install_handler
static void commthread_disable(int mu_int_index)
{
    HWThreadState_t *pHwt = GetMyHWThreadState();
    uint32_t threadid = ProcessorThreadID();
    uint64_t puea_map = BIC_ReadInterruptMap(threadid);
    puea_map &= ~(pHwt->commThreadData[mu_int_index].puea_map_mask);
    BIC_WriteInterruptMap(threadid, puea_map);
}

// Re-enable mu interrupts in the PUEA that were disabled by the commthread_disable function
static void commthread_enable(int mu_int_index)
{
    HWThreadState_t *pHwt = GetMyHWThreadState();
    uint32_t threadid = ProcessorThreadID();
    uint64_t puea_map = BIC_ReadInterruptMap(threadid);
    puea_map &= ~(pHwt->commThreadData[mu_int_index].puea_map_mask);
    puea_map |= pHwt->commThreadData[mu_int_index].puea_map_set; 
    BIC_WriteInterruptMap(threadid, puea_map);
}

// Remove the installed handlers and the associated interrupt enablements for this thread 
void commthread_reset()
{
    HWThreadState_t *pHwt = GetMyHWThreadState();
    // Set the PUEA mapping register values for the MU interrupts back to their reset state for this thread
    puea_ResetGroup(FLIH_GRP_MSG);

    // Loop through the comm data entries for this thread and remove all handlers
    int i;
    for (i=0; i<KERNEL_COMM_NUM_INTS; i++)
    {
        pHwt->commThreadData[i].funcptr = NULL;
    }
}


/*! \brief Interrupt handler for CommThread events
 * \param[in] puea interrupt index. Expected value is one of 
 * the Messaging unit PUEA interrupt status bit positions: 37,38,39,40, or the 
 * PUEA status bit position corresponding to the following GEA lane: COMM_MU_INT_GEA
 */
void IntHandler_MU(int puea_intrp_index)
{
    int i;
    // Determine if the comm thread is active on this core
    HWThreadState_t *pHwt = GetMyHWThreadState();
    KThread_t *tcb = pHwt->pCommThread;

    // generate the mu interrupt index corresponding to the puea status bit that is set
    int mu_int_index = GEN_MU_INTX(puea_intrp_index);

    // If the comm thread does not exist on this core, something is very wrong since
    // this interrupt should not have been enabled for delivery to this hardware thread.
    // Possibly a setaffinity was done by the messaging code to move the comm thread after
    // it had installed handlers.
    if ( !tcb ) 
    {
        printf("(E) Unexpected MU interrupt condition. No Comm thread to deliver interrupt\n");
        return;
    }
    // Bump hardware thread priority to high
    ThreadPriority_High();

    // bump pthread priority to high if not already high
    if (tcb->Priority != CONFIG_SCHED_MAX_COMM_PRIORITY)
    {
        tcb->Priority = CONFIG_SCHED_MAX_COMM_PRIORITY;
        KThread_refreshPriorityData(pHwt);
    }
    // Disable the interrupt at the PUEA.
    commthread_disable(mu_int_index); 

    // Initialize the function to be called
    tcb->Reg_State.ip     = (uint64_t)(pHwt->commThreadData[mu_int_index].funcptr);
    // Initialize the parameter registers
    for (i=0; i< NUM_COMMTHREAD_GPRS; i++)
    {
        tcb->Reg_State.gpr[3+i] = pHwt->commThreadData[mu_int_index].gpr[i];
    }
    // !!! 
    // Should we start the user stack at its orginal location or just leave it alone, linking it into 
    // where it currently is? The stack pointer is likely sitting at the function that did the poof() sys-call. It 
    // may be better for debug if we leave it alone. The thought is that the poof() is likely done at
    // a location of relatively low stack depth so we will not be wasting stack space. It is also possible that
    // a poof was not done and we are in a polling loop in which messaging is running with the mu 
    // interrupts enabled. The same assumption is going to be made that the interrupt is likely occurring
    // at a point where the stack is not deep.

    // Make the thread runnable.     
    // !!!! THIS BEHAVIOR NEEDS SOME ADDITIONAL INVESTIGATION and discussion with our messaging folks.
    Sched_Unblock(tcb, SCHED_STATE_POOF);

    // Enter the scheduler to dispatch the comm thread
    Scheduler();

}

/*! \brief Re-enable message unit interrupts. Loop throught the installed mu interrupt 
 * handlers installed on this hardware thread and re-enable the interrupt at the PUEA 
 *
 */
uint64_t sc_COMMTHREADENABLEINTS( SYSCALL_FCN_ARGS )
{
    HWThreadState_t *pHwt = GetMyHWThreadState();
    int i;
    for (i=0; i<KERNEL_COMM_NUM_INTS; i++)
    {
        if (pHwt->commThreadData[i].funcptr)
        {
            commthread_enable(i);
        }
    }
    return CNK_RC_SPI(0);
}

/*! \brief Disable message unit interrupts. Loop throught the installed mu interrupt 
 * handlers installed on this hardware thread and re-enable the interrupt at the PUEA 
 *
 */
uint64_t sc_COMMTHREADDISABLEINTS( SYSCALL_FCN_ARGS )
{
    HWThreadState_t *pHwt = GetMyHWThreadState();
    int i;
    for (i=0; i<KERNEL_COMM_NUM_INTS; i++)
    {
        if (pHwt->commThreadData[i].funcptr)
        {
            commthread_disable(i);
        }
    }
    return CNK_RC_SPI(0);
}



/*! \brief Removes comm. thread from the runqueue and re-enable any disabled MU interrupts 
 *         for installed handlers. 
 */
uint64_t sc_COMMTHREADPOOF( SYSCALL_FCN_ARGS )
{
    HWThreadState_t *pHwt = GetMyHWThreadState();
    int i;
    // Re-enable messaging interrupts for installed handlers on this thread
    for (i=0; i<KERNEL_COMM_NUM_INTS; i++)
    {
        if (pHwt->commThreadData[i].funcptr)
        {
            commthread_enable(i);
        }
    }
    // Change the thread state to the poof state 
    Sched_Block( GetMyKThread(), SCHED_STATE_POOF );
    // Enter the scheduler
    Scheduler();
   // NORETURN
   return 0;
}

/*! \brief Configures an interrupt line and the interrupt handler function 
 *         for a communications thread.
 *
 */
uint64_t sc_COMMTHREADINSTALLHANDLER( SYSCALL_FCN_ARGS )
{
    HWThreadState_t *pHwt = GetMyHWThreadState();
    // Parameters passed in the system call:
    // GPR3 - MU interrupt identifier
    // GPR4 - Function pointer
    // GPR5 - Parm1 value to be passed to the MU interrupt handler function
    // GPR6 - Parm2 value to be passed to the MU interrupt handler function
    // GPR7 - Parm3 value to be passed to the MU interrupt handler function
    // GPR8 - Parm4 value to be passed to the MU interrupt handler function
    uint64_t interrupt_id = r3;  // Interrupt identifier
    Kernel_CommThreadHandler funcptr = (Kernel_CommThreadHandler)r4;
    uint64_t rc;
    
    // If the function pointer is not NULL, then we are enabling the requested interrupt
    int enableHandler = funcptr ? 1 : 0; 

    switch(interrupt_id)
    {
    case KERNEL_COMM_INT0:
    case KERNEL_COMM_INT1:
    case KERNEL_COMM_INT2:
    case KERNEL_COMM_INT3:
    case KERNEL_COMM_INT_GEA:
         // Set the function pointer of the interrupt handler
        pHwt->commThreadData[interrupt_id].funcptr = funcptr;
        // Load the parameter registers that will be set when the interrupt handler function is launched.
	pHwt->commThreadData[interrupt_id].gpr[0] = r5;
	pHwt->commThreadData[interrupt_id].gpr[1] = r6;
	pHwt->commThreadData[interrupt_id].gpr[2] = r7;
	pHwt->commThreadData[interrupt_id].gpr[3] = r8;
        // calculate the bit offset in the mapping register for the interrupt we want to set. 
        // Find the n
        int puea_map_offset;
        puea_map_offset = 2 * ( (KERNEL_COMM_INT_GEA) ?  BIC_MAP_GEA_LANE(GEA_MU) : BIC_MAP_MU_LANE(interrupt_id) );
        // generate a mask of the mapping bits for the interrupt we are interested in.
        pHwt->commThreadData[interrupt_id].puea_map_mask = (uint64_t)0x3 << (62-puea_map_offset); 
        // generate the value of the mapping register bits that we want to set into the register
        pHwt->commThreadData[interrupt_id].puea_map_set = ((enableHandler) ? ((uint64_t)BIC_EXTERNAL_INTERRUPT << (62-puea_map_offset)) : 0);

        // Enable the interrupt
        commthread_enable(interrupt_id);
        // Set return information
        rc = CNK_RC_SPI(0);
        break;
    default:
        rc = CNK_RC_SPI(EINVAL);
	break;
    }
    return rc;
}
