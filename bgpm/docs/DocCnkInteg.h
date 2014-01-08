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
 *
 * \page bgpm_cnk_integ BGPM Integration with CNK
 *
 *
 * \if HTML
\htmlonly
<div class="toc">
\endhtmlonly

<h2>Table Of Contents</h2>

- \ref index "Return to Main Page"

- \ref bgpm_cnk_assumptions
- \ref bgpm_cnk_sharedmem
- \ref bgpm_cnk_kthread
- \ref upci_cnk_threadinit
- \ref upci_cnk_syscall
- \ref upci_cnk_crosscore
- \ref bgpm_cnk_perfmonint
- \ref bgpm_cnk_autocount
\htmlonly
</div>
<div class="contents-right">
\endhtmlonly
 * \endif
 *
 *
 * *WARNING* this information is old and stale and not maintained.
 *
 * This section talks to kernel function extensions needed
 * to integrate BGPM function with CNK, and a somewhat high-level description
 * of the operations.
 *
 *
 * \section bgpm_cnk_assumptions Assumptions to remember
 * -# BGPM requires "Blocked" threads.  That is, software threads must run on a single hardware thread
 *    and cannot move hardware threads after Bgpm_Init has occurred.  Doing so will give indeterminant results.
 *    \n\n
 * -# Bgpm will not support the CNK "enhanced thread model".  Thus, there can be only one
 *    process id per hardware thread.
 *    \n\n
 * -# The UPC_P counters have a hardwired HW Thread affinity.  That is, half of the counters can only receive
 *    events from core threads 0 and 1, and the other half from 2 and 3.
 *    Thus, in order to prevent wasting 1/2 of the counters when the user is only running 2 threads per core
 *    (which is likely to be a frequent configuration), we need CNK to schedule user threads to core threads
 *    0 and 2 before using 1 and 3.
 *    CNK thread scheduling has already been implemented to accommodate this requirement.
 *    \n\n
 *
 *
 * \section bgpm_cnk_sharedmem Shared Memory Requirement
 *
 * When BGPM is used, the Bgpm_Init() function will need to allocate memory
 * out of shared memory or process global memory, and thread local memory.
 * This shared memory or process memory will manage resource reservation
 * structures, locking, and overflow tracking which needs node wide tracking
 * model of the hardware.  Process memory will be used if there is only
 * one process per node.
 *
 *
 * \section bgpm_cnk_kthread CNK KThread and HWThreadState Structure Additions
 *
 * UPCI maintains a dumb \ref UPCI_Punit_Cfg_t structure of events, counter and hw signal settings
 * to be applied to hardware.  The structure also contains a list of events and snapshot
 * memory locations for the punit events configured by a user for a thread.
 * The structure does not
 * contain any pointers to be processed within the kernel.
 * Only the base pointer to the structure, and allocated size needs to be
 * to be validated upon use. Thus
 * - Any indexes within the structure will also be validated or masked to insure
 *   offsets can not exceed the compiled object size.
 * - The stored pointer will be in the context of the current process, and we are assuming that
 *   any swaps between threads will also be with the current process id.
 *
 * In order to snapshot or start/start punit counters during a context switch,
 * two pointers needs to be added to the KThread and HWThreadState structures.
 *
 *
 * <b>void * KThread::Upci_CfgPtr</b>: Active upci config for this software thread.
 * - A non-null pointer value indicates to CNK \e when UPCI operations apply to the
 * software thread.
 * - The value will be set by a new Kernel_Upci_Punit_Attach() syscall operation.
 * - The pointer provides to UPCI operations the user-state structure to
 * use when starting/stopping or making snapshots of counters.
 *
 *
 * <b>HWThreadState::Upci_ActiveCfgPtr</b>: Active upci config for this hardware thread.
 * - a non-null pointer indicates to UPCI kernel operations which configuration is applied
 * to the hardware thread.
 * - it will only be maintained by Upci operations.
 * - this allow upci to stop counting for the current sw thread, and continue to
 * track the configuration applied to hardware through software thread which not counting.
 * Upon returning to the original thread, rather than reapplying the hardware configure
 * configuration, it need only just restart the counters.
 *
 *
 * <b>These two new pointers will be used in the following fashion:</b>
 * - Bgpm/Upci user state operations will create and configure the
 *   \ref Upci_Punit_Cfg_t objects in user state.
 *   The punit objects are static in size, and only owned by a single software thread.
 *
 * - The Bgpm_Apply() and Bgpm_Attach(), Bgpm_Detach() user operations will
 *   pass through Kernel_Upci_Punit_Cfg_Attach() to the Kernel_Upci_Raw_Syscall() operations to
 *   actually set the Upci_CfgPtr and Upci_ActiveCfgPtr.
 *   These indicating which configurations are applied to the hardware,
 *   and when being used by the software thread.
 *
 * - During a swap out, the CNK context switch needs to check for a non-null
 *   Upci_CfgPtr value.
 *   If the pointer is non-null, then call
 *   Upci_Punit_SwapOut() (w argument KThread::Upci_CfgPtr),
 *   which will stop event counting for that object.
 *
 * - During a swap in, the CNK context switch also needs to check for a
 *   non-null Upci_CfgPtr value.
 *   If the pointer is non-null, then call
 *   Upci_Punit_SwapIn() (w argument KThread::Upci_CfgPtr) which will:
 *   - check Upci_ActiveCfgPtr to determine which object owns the hw thread counters.
 *   - If the owning thread is different than the passed Upci_CfgPtr object
 *     - snapshot the existing counter values to the Upci_ActiveCfgPtr object.
 *     - apply The Upci_CfgPtr configuration to the hw thread.
 *     - restore a prior counter snapshot from the Upci_CfgPtr object.
 *   - start/restart event counters from the incoming Upci_CfgPtr
 *
 *
 *
 * <b>Context switch additional overhead:</b>
 * - If there is no Perf counting active, then the Upci_CfgPtr is null,
 *   and the context switch only has additional the cost of the pointer check.
 *
 * - When there is only one sw thread actually event counting for a
 *   hardware thread, the context swap only results in a start or stop of counting.
 *
 * - When counting is active for multiple sw threads on the same hardware thread,
 *   each context switch will pay a counter save/restore penalty.
 *
 *
 *
 * \section upci_cnk_threadinit CNK Thread Initialization Additions
 * The following operations also need to be added during CNK Thread initialization:
 * - Set KThread::Upci_CfgPtr = NULL;
 *
 * - Set HWThreadState::Upci_ActiveCfgPtr = NULL;
 *
 *
 *
 * \section upci_cnk_syscall CNK / UPCI Syscalls
 *
 * There are some activities needed by the user state code which require privileged
 * access. To this end, we define:
 *
 * <b>int Kernel_Upci_Raw_Syscall( uint64_t op, uint64_t target, void *pData, size_t dSize)</b>
 *
 * This call will be used for all Upci operations, passing an operation id to determine the appropriate
 * function needed for performance counting. Some of the operations which will funnel through this Raw
 * syscall are listed below:
 *
 * - int Kernel_Upci_GetNodeParms(Upci_Node_Parms_t *pNodeParms);
 * - int Kernel_Upci_Mode_Init( int upcMode, UPC_Ctr_Mode_t ctrMode, short unit );
 * - int Kernel_Upci_Mode_Free();
 * - int Kernel_Upci_A2PC_ApplyRegs( Upci_A2PC_t *pA2pc);
 * - int Kernel_Upci_A2PC_GetRegs( Upci_A2PC_Val_t *pA2pcVal);
 * - int Kernel_Upci_Punit_Cfg_Apply( Upci_Punit_Cfg_t *pCfg, short unitId);
 *
 * More will be added as further implementation progresses.
 *
 * The syscall code objects for Upci are supplied to the CNK build via:
 * - the library <b>spi/lib/libSPI_upci_fwcnk.a</b>, which contains the minimal Upci code to support
 *   the syscalls and kernel operations.
 * - header files following the SPI build architecture using
 *   spi/include/kernel/upci.h, and spi/include/kernel/cnk|firmware upci_impl.h headers.
 *   There are additional upci_hw.h and upci_hw_impl.h headers used to resolve
 *   some otherwise circular include dependencies.
 * - <b>cnk/src/syscalls/upci_op.cc</b> implements the actual syscall interface, which directs operations
 *   to Upci_Do_Syscall() from the upci library for execution.
 *
 *
 * \section upci_cnk_crosscore Cross Core Interrupts
 *
 * The A2 core event configuration SPRs have only privileged access, and are only addressable by
 * code running on that A2 core.  Thus, when a user thread wishes to configuration counters for
 * threads on another core, the operation needs to do a cross core interrupt to complete the operation.
 *
 *
 *
 * \section bgpm_cnk_perfmonint BGPM Performance Monitor Interrupt Operation
 *
 * The performance monitor A2 interrupt is separate set of 4 interrupt lines
 * into the A2 (one for each hw thread).
 * They do not route through the GEA, but are directly connected
 * through the UPC_P component. They are low-priority external interrupt.
 *
 * When a performance monitor interrupt occurs, the interrupt handler will
 * mask the UPC_P interrupts by calling <b>Kernel_Upci_MaskInts()</b> which will mask the
 * interrupts for the current thread.  These interrupts will remain masked
 * until the BGPM signal handler can resolve the cause of the interrupt.  The
 * upc_p interrupt mask for the thread will also be saved and restored across context
 * switches.
 *
 * If for some reason the BGPM signal handler doesn't occur, the interrupts will
 * remain masked and no further interrupts occur.
 *
 *
 *
 * \section bgpm_cnk_autocount BGPM Auto-Counting Support Operation (Deferred)
 *
 * Auto counting is a "stretch goal", and allows the node to
 * count from a hardware perspective without any knowledge by the
 * software threads, and then collect a job summary at the end.
 *
 * Auto-counting requires more definition, but the counting activity would
 * occur completely within the kernel:
 * - At kernel start, get counting configuration and start hardware counters.
 *   Don't keep the config around.
 * - The counting config would be in terms of UPC config profiles,
 *   like the Upci_Punit_Cfg object, but would include configs for other units as well.
 *   The config would also indicate how the individual counts should be aggregated for the node.
 * - At kernel end, reget the counting config, capture, then communicate the
 *   counts.
 * - An external process will roll the counts from each node into loggable values.
 *
 *
 */
