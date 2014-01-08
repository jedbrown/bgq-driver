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
/* (C) Copyright IBM Corp.  2008, 2012                              */
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

#include <errno.h>
#include "cnk/include/Config.h"
#include "cnk/include/kcounters.h"
#include "spi/include/upci/upci_syscall.h"
#include "spi/include/kernel/upci.h"
#include "spi/include/upci/upci.h"
#include "hwi/include/bqc/l1p_dcr.h"


#define DEBUG_PRINT(...)   //printf(__VA_ARGS__)
#define DEBUG_PRINT2(...)  printf(__VA_ARGS__)


// Functions which bridge between CNK and SPI compiles.
// That is, this SPI module is very closely related to the UPC and UPCI functionality and contains
// compile time details which are not part of the public interface.
// But, this function is linked onto the compute node kernel, and also needs kernel
// functionality.  But, those details are not part of the public interface of CNK either.
// Thus, the following externals provide a bridge for operations these two compile time domains.

// Use the following two functions to lock operations across the whole node.  The are no cases where
// the across node lock doesn't potentially conflict with IPI operations from another core.
// But, still maintain the individual core locks for cases where operations only need locked on
// for single core.
extern int Syscall_GetIpiControl();      // Obtain Ipi control for syscall usage. Returns 0:success; (-1) busy.
extern int Syscall_ReleaseIpiControl();  // Release Ipi control for syscall usage. Returns 0:success; (-1) error (not current controlling thread)

extern void IPI_upc_attach(unsigned coreID, Upci_Punit_Cfg_t* pCfg);
extern void IPI_upcp_init(unsigned coreID, int mode, unsigned ctrMode);
extern void IPI_upcp_disable(unsigned coreID);

extern void UPC_KBridge_ClearAllActiveKThreads();
extern void UPC_KBridge_SetActiveKThread();
extern void UPC_KBridge_ClearActiveKThread();
extern void UPC_KBridge_Signal_Deliver(int sig);
extern void UPC_KBridge_SetSwapCfg(void *pCfg);  // set kthread ptr to indicate which cfg to use on sw thread swap
extern void UPC_KBridge_ClearSwapCfg();


// Insure the configured UPC Mode and Counter usage is consistent.
// And - only initialize the hardware once while in use.
static unsigned         upc_ActiveCnt = 0;   // number of threads using UPC (have done a mode init)
static short            active_UpcMode = 0;
static UPC_Ctr_Mode_t   active_CtrMode = 0;
static short            active_Unit = 0;
static uint64_t         nodeConfig = 0;      // cache the nodeConfig (from personality)

// lock UPC hardware updates by core - a thread should only touch a lock for it's core
static UPC_Lock_t        upcHwThdLocks[CONFIG_MAX_CORES] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

// copy of Punit configure to apply during cross core IPI operations
static Upci_Punit_Cfg_t  xCoreCfg;

static Upci_KDebug_t     upci_KDebug = UPCI_KDEBUG_INIT;

static int               upci_PMSignalVal = 0;


// Interrupt masks and counter control state to save/restore during swaps between software thread.
typedef struct sUpci_SwapCfg {
    uint64_t saveIntEnables;   // save for just the int enable bits from upc_p_local->p_config;
    //uint64_t saveIntStatus;    // save for the int status bits from upc_p_local->int_status[thd]
    uint64_t saveControlState; // save run state of counters
    uint64_t punitControl;     // control bits for the active punit
    //UpciBool_t   upcCIntEnable;    // indicate if upc_c int enable bits should be active.
} Upci_SwapCfg_t;

// We only support 1 SW Thread per HW thread for Upci Counting, so only need to track
// one set per HW Thread.  We count on Bgpm to prevent multiples per hardware thread.
static Upci_SwapCfg_t upcHwSwapCfgs[CONFIG_MAX_HWTHREADS];



#if 0
// For debug,
// copied and tweaked from spi/src/upci/upc_c_debug.c, us_debug.c since whole thing can't be linked.
// in mmcs, redirect_block on; and leave mmcs session open to see output.
void PrintUPCHwState()
{
    char indentStrg[] = "";

    int i;
    printf("%sMU UPC Config: \n", indentStrg);
    printf("%s  dcr reset: 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(RESET)));
    for (i=0; i<MU_DCR__UPC_MU_COUNTER_range; i++) {
        printf("%s  mu ctr[%02d]: 0x%016lx\n", indentStrg, i, DCRReadUser(MU_DCR(UPC_MU_COUNTER)+i));
    }
    printf("%s  mu ctr ctl: 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(UPC_MU_COUNTER_CONTROL_RW)));
    printf("%s  mu upc cfg: 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(UPC_MU_CONFIG)));
    printf("%s  mu int internal error sw info  : 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(INTERRUPT_INTERNAL_ERROR_SW_INFO)));
    printf("%s  mu int internal error hw info  : 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(INTERRUPT_INTERNAL_ERROR_HW_INFO)));
    printf("%s  mu int internal error data info: 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(INTERRUPT_INTERNAL_ERROR_DATA_INFO)));
    printf("%s  mu int internal error state    : 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(INTERRUPT_INTERNAL_ERROR__STATE)));
    printf("%s  mu int internal error first    : 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(INTERRUPT_INTERNAL_ERROR__FIRST)));
    printf("%s  mu int internal error machchk  : 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK)));
    printf("%s  mu int internal error critical : 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(INTERRUPT_INTERNAL_ERROR__CRITICAL)));
    printf("%s  mu int internal error noncrit  : 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(INTERRUPT_INTERNAL_ERROR__NONCRITICAL)));
    printf("%s  mu misc int state    : 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(MISC_INTERRUPTS__STATE)));
    printf("%s  mu misc int first    : 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(MISC_INTERRUPTS__FIRST)));
    printf("%s  mu misc int machchk  : 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(MISC_INTERRUPTS__MACHINE_CHECK)));
    printf("%s  mu misc int critical : 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(MISC_INTERRUPTS__CRITICAL)));
    printf("%s  mu misc int noncrit  : 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(MISC_INTERRUPTS__NONCRITICAL)));
    printf("%s  mu misc int ctl high : 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(MISC_INTERRUPTS_CONTROL_HIGH)));

    printf( "-----------------------------------------\n"
            "UPC_C Unit : Addr= 0x%011lx\n", (uint64_t)upc_c);
    printf("c_config                  = 0x%016lx\n", upc_c->c_config);
    printf("mode1_ec_config           = 0x%016lx\n", upc_c->mode1_ec_config);
    printf("status                    = 0x%016lx\n", upc_c->status);
    printf("mode1_counter_control0_rw = 0x%016lx\n", upc_c->mode1_counter_control0_rw);
    printf("mode1_counter_control1_rw = 0x%016lx\n", upc_c->mode1_counter_control1_rw);
    printf("mode1_counter_control2_rw = 0x%016lx\n", upc_c->mode1_counter_control2_rw);
    printf("ccg_int_en                = 0x%016lx\n", upc_c->ccg_int_en);
    printf("ccg_int_status_w1s        = 0x%016lx\n", upc_c->ccg_int_status_w1s);
    printf("io_ccg_int_en             = 0x%016lx\n", upc_c->io_ccg_int_en);
    printf("io_ccg_int_status_w1s     = 0x%016lx\n", upc_c->io_ccg_int_status_w1s);
    printf("accum_control             = 0x%016lx\n", upc_c->accum_control);
    printf("error_force               = 0x%016lx\n", upc_c->error_force);
    printf("m2_trigger                = 0x%016lx\n", upc_c->m2_trigger);
    printf("counter_start             = 0x%016lx\n", upc_c->counter_start);
    printf("interrupt_state__state        = 0x%016lx\n", upc_c->interrupt_state__state);
    printf("interrupt_state_control_low  = 0x%016lx\n", upc_c->interrupt_state_control_low);
    printf("interrupt_state_control_high = 0x%016lx\n", upc_c->interrupt_state_control_high);
    // printf("interrupt_state__force    = 0x%016lx\n", upc_c->interrupt_state__force);  (write only)
    printf("interrupt_state__first        = 0x%016lx\n", upc_c->interrupt_state__first);
    printf("interrupt_state__machinecheck = 0x%016lx\n", upc_c->interrupt_state__machinecheck);
    printf("interrupt_state__critical     = 0x%016lx\n", upc_c->interrupt_state__critical);
    printf("interrupt_state__noncritical  = 0x%016lx\n", upc_c->interrupt_state__noncritical);
    printf("upc_sram_parity_info          = 0x%016lx\n", upc_c->upc_sram_parity_info);
    printf("interrupt_internal_error__state        = 0x%016lx\n", upc_c->interrupt_internal_error__state);
    printf("interrupt_internal_error_control_high = 0x%016lx\n", upc_c->interrupt_internal_error_control_high);
    // printf("interrupt_internal_error__force        = 0x%016lx\n", upc_c->interrupt_internal_error__force);  (Write only)
    printf("interrupt_internal_error__first        = 0x%016lx\n", upc_c->interrupt_internal_error__first);
    printf("interrupt_internal_error_sw_info       = 0x%016lx\n", upc_c->interrupt_internal_error_sw_info);
    printf("interrupt_internal_error_hw_info       = 0x%016lx\n", upc_c->interrupt_internal_error_hw_info);
    printf("interrupt_internal_error_data_info     = 0x%016lx\n", upc_c->interrupt_internal_error_data_info);
    printf("interrupt_internal_error__machinecheck = 0x%016lx\n", upc_c->interrupt_internal_error__machinecheck);
    printf("interrupt_internal_error__critical     = 0x%016lx\n", upc_c->interrupt_internal_error__critical);
    printf("interrupt_internal_error__noncritical  = 0x%016lx\n", upc_c->interrupt_internal_error__noncritical);
    printf("overflow_state__state         = 0x%016lx\n", upc_c->overflow_state__state);
    printf("overflow_state_control_low   = 0x%016lx\n", upc_c->overflow_state_control_low);
    printf("overflow_state_control_high  = 0x%016lx\n", upc_c->overflow_state_control_high);
    // printf("overflow_state__force         = 0x%016lx\n", upc_c->overflow_state__force);  (write only)
    printf("overflow_state__first         = 0x%016lx\n", upc_c->overflow_state__first);
    printf("overflow_state__machinecheck  = 0x%016lx\n", upc_c->overflow_state__machinecheck);
    printf("overflow_state__critical      = 0x%016lx\n", upc_c->overflow_state__critical);
    printf("overflow_state__noncritical   = 0x%016lx\n", upc_c->overflow_state__noncritical);
}
#endif



// UPC_Init() to be called by Kernel, once on each core (0-16), during Job Load step.
// This routine should clear UPC hardware and disable the UPC_Ps.
// Core 0 will clear the other UPC hardware.
// 1st call in new job must reset the global vars above.
void UPC_Init(uint64_t jobID) {

    // log which cores have been inited for debug purposes.
    Syscall_GetIpiControl(); // old logic = UPC_Lock(&upcHwNodeLock);
    if (upci_KDebug.jobid != jobID) {

        //printf(_AT_ " In Kernel UPC_Init at job start: old jobid=%ld, new jobid=%ld\n", upci_KDebug.jobid, jobID);

        // Initialize all other kernel vars when new job
        upc_ActiveCnt = 0;
        active_UpcMode = 0;
        active_CtrMode = 0;
        active_Unit = 0;
        memset((void*)&upcHwThdLocks, 0, sizeof(upcHwThdLocks));
        memset((void*)&upci_KDebug,0,sizeof(upci_KDebug));
        memset((void*)&upcHwSwapCfgs,0,sizeof(upcHwSwapCfgs));
        upci_PMSignalVal = 0;
        upci_KDebug.jobid = jobID;
        upci_KDebug.coreInitMask = 0;
    }

    upci_KDebug.coreInitMask |= MASK32_bit(Kernel_ProcessorCoreID());
    mbar();
    //printf("core=%d, jobid=0x%016lx\n", Kernel_ProcessorCoreID(), jobID );
    Syscall_ReleaseIpiControl(); // UPC_Unlock(&upcHwNodeLock);

    Personality_t personality;
    Kernel_GetPersonality(&personality, sizeof(Personality_t));
    nodeConfig = personality.Kernel_Config.NodeConfig;

    UPC_P_Disable_Unit( UPC_LOCAL_UNIT );
    UPC_P_Clear_Unit(UPC_LOCAL_UNIT);
    if (Kernel_ProcessorCoreID() == 0) {

        // PrintUPCHwState();

        if (!(nodeConfig & PERS_ENABLE_FPGA)) {
            if (nodeConfig & PERS_ENABLE_MU) {
                UPC_MU_DisableUPC();
            }
            if (nodeConfig & PERS_ENABLE_PCIe) {
                UPC_PCIe_DisableUPC();  // pcie disabled on compute nodes
            }
        }
        UPC_DevBus_DisableUPC();
        UPC_IO_Clear(nodeConfig);
        UPC_L2_DisableUPC();
        UPC_C_Full_Clear();

        UPC_KBridge_ClearAllActiveKThreads();

        upci_PMSignalVal = UPCI_DEFAULT_PM_SIGNAL;
    }
}


// ********
// Note about cross core interrupts:
//   Some of these syscalls use cross core interrupts to place execution on the right core to
//   configured.  There are two reasons for using cross core IPIs:
//   #- only the current core can access the A2 SPRs, thus must be on the correct core to configure the A2 Events.
//   #- there is a flaw in the cross core DCR access operations which can result in DCR timeouts.
//      The cross core DCR was designed to allow one core to configure UPC_P operations for another core.
//      We have had to abandon that approach because the hardware was not fixed.
//      Thus we use cross core interrupts to accomplish the UPC_P config.
//
//   We won't attempt to prevent simultaneous updates of a UPC_P between kernel and user state threads.
//   Rather, we rely on the following implementation characteristics:
//   - There are kernel core locks to prevent collisions within the kernel.
//   - User state operations only update UPC_P registers which don't have problems with simultaneous
//     update (set/clear register version)
//
// ********


// Serialization Call backs from core2core IPI functions
void Upci_Kernel_Punit_Init_Mode( int unit, int mode, unsigned ctrMode)
{
    unsigned core = Kernel_ProcessorCoreID();
    UPC_Lock(&upcHwThdLocks[core]);
    UPC_P_Init_Unit_Mode(unit, mode, ctrMode);
    UPC_Unlock(&upcHwThdLocks[core]);
}
void Upci_Kernel_Punit_Disable( int unit )
{
    unsigned core = Kernel_ProcessorCoreID();
    UPC_Lock(&upcHwThdLocks[core]);
    UPC_P_Disable_Unit(unit);
    UPC_Unlock(&upcHwThdLocks[core]);
}
void Upci_Kernel_Punit_Cfg_Apply(struct sPunit_Cfg *pCfg)
{
    unsigned core = Kernel_ProcessorCoreID();
    UPC_Lock(&upcHwThdLocks[core]);
    Upci_Punit_Cfg_Apply(pCfg);
    UPC_Unlock(&upcHwThdLocks[core]);
}




//! Do Upci Mode Init - initiates cross core interrupts when necessary
//! Assumes all other IPI operations are already locked out till this is complete.
static uint64_t Upci_Syscall_Mode_Init( unsigned upcMode, UPC_Ctr_Mode_t ctrMode, int unit )
{
    // Kernel_Upci_Mode_Init does not pass ctrMode to UPC_C_Init_Mode in kernel
    // - it skips subunit initialization so that cross core interrupts can be used instead.
    // - That is - the function was originally created to use the cross-core DCR support, but
    //   we have to abandon that because of hardware problems.
    uint64_t rc = Kernel_Upci_Mode_Init( upcMode, ctrMode, unit );
    if (rc == 0) {
        unsigned i;
        unsigned targCore = (unit >= 0) ? unit : Kernel_PhysicalProcessorID();
        UpciBool_t indepStart = ((ctrMode==UPC_CM_INDEP) || (ctrMode==UPC_CM_DISCON));

        //printf(_AT_ " core=%d\n", targCore);
        if (upcMode == UPC_DISTRIB_MODE) {
            for (i=0; i<CONFIG_MAX_CORES; i++) {
                //printf(_AT_ " core=%d\n", i);
                IPI_upcp_init(i, upcMode, (unsigned)ctrMode);
                //printf(_AT_ " core=%d\n", i);
            }
            UPC_L2_EnableUPC(indepStart, UpciTrue);
        }

        else {
            for (i=0; i<CONFIG_MAX_CORES; i++) {
                if (i != targCore) {
                    IPI_upcp_disable(i);
                }
                else {
                    IPI_upcp_init(i, upcMode, (unsigned)UPC_CM_SYNC);
                }
            }
            UPC_L2_DisableUPC();
        }

        Personality_t personality;
        Kernel_GetPersonality(&personality, sizeof(Personality_t));
        uint64_t nodeConfig = personality.Kernel_Config.NodeConfig;

        UPC_IO_EnableUPC(indepStart, nodeConfig);
    }
    return rc;
}



//! Free the UPC Hardware
//! Assumes all other  IPI operations are already locked out till this is complete.
static void Upci_Syscall_Mode_Free()
{
    UPC_L2_DisableUPC();
    UPC_IO_DisableUPC(nodeConfig);

    unsigned i;
    for (i=0; i<CONFIG_MAX_CORES; i++) {
        IPI_upcp_disable(i);
    }

    UPC_C_Disable( UPC_CM_NO_INIT );
}




// Main Upci Syscall routine
uint64_t Upci_Do_Syscall(uint64_t op, uint64_t targ, void *pData, size_t dsize)
{
    uint64_t rc = 0;

    switch ((Upci_Kops_t)op) {

    // Reset Current threads Interrupt Status bits indicated by target.
    case UPCI_KOP_RESETINTS: {
        unsigned core = Kernel_ProcessorCoreID();
        unsigned thd = Kernel_ProcessorThreadID();

        UPC_Lock(&upcHwThdLocks[core]);
        DEBUG_PRINT("UPCI Syscall ResetInts %d, val=0x%016lx\n", Kernel_ProcessorID(), targ);

        upc_p_local->int_status_w1c = targ; // 1st reset

        // retry needed to reset hw thread int status (at least sometimes)
        upc_p_local->int_status_w1c = targ &
                (UPC_P__INT_STATUS__UPC_C_THREAD_INT_set(0xF) | UPC_P__INT_STATUS__ANY_OVF_THREAD_INT_set(0xF));
        mbar();
        // Reenable interrupts for this hw thread.
        upc_p_local->p_config |= (UPC_P__CONFIG__UPC_P_INT_ENABLE_allbits(thd));

        DEBUG_PRINT("UPCI Syscall ResetInts, after ResetInts - p_config=0x%016lx, int_status=0x%016lx, upcp_control=0x%016lx\n",
                upc_p_local->p_config, upc_p_local->int_status_w1s, upc_p_local->control, upc_p_local->counter_cfg[16]);
#if 0
        DEBUG_PRINT("UPCI Syscall ResetInts, after ResetInts - p_config=0x%016lx, int_status=0x%016lx, "
                "upcp_control=0x%016lx, upcp_counter_config[16]=0x%016lx, "
                "upc_p_counter[16]=0x%016lx, upc_c_counter[16]=0x%016lx\n",
                upc_p_local->p_config, upc_p_local->int_status_w1s,
                upc_p_local->control, upc_p_local->counter_cfg[16],
                upc_p_local->counter[16], upc_c->data24.grp[core].counter[16]);
#endif

        UPC_Unlock(&upcHwThdLocks[core]);
        return 0;  // let's return as soon as possible
        break;
    }

    case UCPI_KOP_GETPARMS: {
        if (dsize == sizeof(Upci_Node_Parms_t)) {
            DEBUG_PRINT("UPC Syscall Get Node Parms %d\n", Kernel_ProcessorID());
            rc = Kernel_Upci_GetNodeParms((Upci_Node_Parms_t*)pData);
        }
        else {
            DEBUG_PRINT("(E) Upci_Do_Syscall GetNodeParms dsize=%ld, expected=%ld\n", dsize, sizeof(Upci_Node_Parms_t));
            rc = UPCI_ESYSCALL_DATASIZE;
        }
        break;
    }

    // Mode Init to set UPC hardware into given mode.
    // Bgpm_Init only calls once per node when in user state, but FWExt tests will
    // call once per thread.
    case UCPI_KOP_MODEINIT: {
        if (dsize == sizeof(Upci_ModeInitData_t)) {

            // Get node-wide lock : Mode init cannot block
            // - must return to user state if lock
            // is not acquired to allow IPIs to complete.
            if (UNLIKELY(Syscall_GetIpiControl() < 0)) {
                rc = -(EAGAIN);  // neg
            }
            else {

                short reqUpcMode = ((Upci_ModeInitData_t*)pData)->upcMode;
                UPC_Ctr_Mode_t reqCtrMode = ((Upci_ModeInitData_t*)pData)->ctrMode;
                short reqUnit = ((Upci_ModeInitData_t*)pData)->unit;

                DEBUG_PRINT("UPC Syscall Mode Init Thread %d\n", Kernel_ProcessorID());

                if (upc_ActiveCnt < 0) {
                    printf("ActiveCnt=%d\n", upc_ActiveCnt);
                    assert(upc_ActiveCnt >= 0);
                }
                if (upc_ActiveCnt == 0) {
                    DEBUG_PRINT("UPC Do Hw Configure\n");

                    rc = Upci_Syscall_Mode_Init( reqUpcMode, reqCtrMode, reqUnit );
                    if (rc == 0) {
                        upc_ActiveCnt++;
                        active_UpcMode = reqUpcMode;
                        active_CtrMode = reqCtrMode;
                        active_Unit    = reqUnit;
                    }
                    assert(rc == 0);
                    assert(upc_ActiveCnt == 1);
                }
                // Make sure modes are equivalent
                else if ((reqUpcMode != active_UpcMode) || (reqCtrMode != active_CtrMode)) {
                    rc = UPCI_EMODE_MISMATCH;
                }
                // If in detailed or trace mode - unit must match
                else if ((reqUpcMode > 0) && (reqUnit != active_Unit)) {
                    rc = UPCI_EMODE_MISMATCH_UNIT;
                }
                else {
                    upc_ActiveCnt++;
                }
                mbar();
                Syscall_ReleaseIpiControl();
            }
        }
        else {
            DEBUG_PRINT("(E) Upci_Do_Syscall ModeInit dsize=%ld, expected=%ld\n", dsize, sizeof(Upci_ModeInitData_t));
            rc = UPCI_ESYSCALL_DATASIZE;
        }
        break;
    }

    // debug function - only applies to current core
    case UPCI_KOP_A2PC_APPLY: {
        DEBUG_PRINT("UPCI Syscall A2PC_APPLY %d\n", Kernel_ProcessorID());

        if (dsize == sizeof(Upci_A2PC_t)) {
            unsigned core = Kernel_ProcessorCoreID();
            UPC_Lock(&upcHwThdLocks[core]);
            Kernel_Upci_A2PC_ApplyRegs( (Upci_A2PC_t*)pData);
            UPC_Unlock(&upcHwThdLocks[core]);
        }
        else {
            DEBUG_PRINT("(E) Upci_Do_Syscall A2PC_Apply dsize=%ld, expected=%ld\n", dsize, sizeof(Upci_A2PC_t));
            rc = UPCI_ESYSCALL_DATASIZE;
        }
        break;
    }

    // debug function - only applies to current core
    case UPCI_KOP_A2PC_GET: {
        DEBUG_PRINT("UPCI Syscall A2PC_GET %d\n", Kernel_ProcessorID());

        if (dsize == sizeof(Upci_A2PC_Val_t)) {
            unsigned core = Kernel_ProcessorCoreID();
            UPC_Lock(&upcHwThdLocks[core]);
            Kernel_Upci_A2PC_GetRegs( (Upci_A2PC_Val_t*)pData);
            UPC_Unlock(&upcHwThdLocks[core]);
        }
        else {
            DEBUG_PRINT("(E) Upci_Do_Syscall A2PC_Get dsize=%ld, expected=%ld\n", dsize, sizeof(Upci_A2PC_Val_t));
            rc = UPCI_ESYSCALL_DATASIZE;
        }
        break;
    }

    case UPCI_KOP_PUNIT_CFG_APPLY: {
        DEBUG_PRINT("UPCI Syscall Punit Cfg Apply %d\n", Kernel_ProcessorID());

        if (dsize == sizeof(Upci_Punit_Cfg_t)) {
            if (((Upci_Punit_Cfg_t*)pData)->cfgVersion != UPCI_CFG_VERSION) {
                rc = UPCI_EINV_UKVER_MISMATCH;
            }
            else if (targ == Kernel_ProcessorCoreID()) {
                unsigned core = Kernel_ProcessorCoreID();
                UPC_Lock(&upcHwThdLocks[core]);
                Upci_Punit_Cfg_Apply( (Upci_Punit_Cfg_t*)pData);

                // enable context switch support for this threads counters.
                unsigned procId = Kernel_ProcessorID();
                upcHwSwapCfgs[procId].punitControl = Upci_Punit_Cfg_GetCtlMask((Upci_Punit_Cfg_t*)pData);
                UPC_KBridge_SetSwapCfg((void*)&upcHwSwapCfgs[procId]);

                UPC_Unlock(&upcHwThdLocks[core]);
            }
            else {
                rc = UPCI_EINV_PARM;
            }
        }
        else {
            DEBUG_PRINT("(E) Upci_Do_Syscall CFG APPLY dsize=%ld, expected=%ld\n", dsize, sizeof(Upci_Punit_Cfg_t));
            rc = UPCI_ESYSCALL_DATASIZE;
        }
        break;
    }


    case UPCI_KOP_PUNIT_CFG_ATTACH: {
        DEBUG_PRINT("UPCI Syscall Punit Cfg Attach %d\n", Kernel_ProcessorID());

        if (dsize == sizeof(Upci_Punit_Cfg_t)) {
            if (((Upci_Punit_Cfg_t*)pData)->cfgVersion != UPCI_CFG_VERSION) {
                rc = UPCI_EINV_UKVER_MISMATCH;
            }
            else if (targ == Kernel_ProcessorCoreID()) {
                unsigned core = Kernel_ProcessorCoreID();
                UPC_Lock(&upcHwThdLocks[core]);
                // make sure the swap controls are clear.
                unsigned procId = Kernel_ProcessorID();
                upcHwSwapCfgs[procId].punitControl = 0;
                UPC_KBridge_ClearSwapCfg(); // make sure no active swap
                Upci_Punit_Cfg_Apply( (Upci_Punit_Cfg_t*)pData);
                UPC_Unlock(&upcHwThdLocks[core]);
            }
            else if (targ < CONFIG_MAX_CORES) {
                // Can't attach to a different target if not in a hardware mode
                if (active_CtrMode != UPC_CM_SYNC) {
                    rc = UPCI_EINV_OPCTRMODE;
                }
                // Get node-wide IPI lock : cannot block, must return to user state
                // to allow IPIs to complete.
                else if (UNLIKELY(Syscall_GetIpiControl() < 0)) {
                    rc = -(EAGAIN);  // neg
                }
                else {
                    memcpy(&xCoreCfg, pData, sizeof(Upci_Punit_Cfg_t));
                    IPI_upc_attach(targ, &xCoreCfg);
                    Syscall_ReleaseIpiControl();
                }
            }
            else {
                rc = UPCI_EINV_PARM;
            }
        }
        else {
            DEBUG_PRINT("(E) Upci_Do_Syscall CFG Attach dsize=%ld, expected=%ld\n", dsize, sizeof(Upci_Punit_Cfg_t));
            rc = UPCI_ESYSCALL_DATASIZE;
        }
        break;
    }


    case UCPI_KOP_MODEFREE: {
        // Get node-wide lock : Mode init cannot block
        // - so this routine must return to user state if lock
        // is not acquired to allow IPIs to complete.
        if (UNLIKELY(Syscall_GetIpiControl() < 0)) {
            rc = -(EAGAIN);  // neg
        }
        else {
            DEBUG_PRINT("UPCI Syscall Mode Free %d\n", Kernel_ProcessorID());
            if (upc_ActiveCnt > 0) {
                upc_ActiveCnt--;
                UPC_KBridge_ClearActiveKThread();
            }
            else {
                Upci_Syscall_Mode_Free();
                UPC_KBridge_ClearAllActiveKThreads();
            }
            mbar();
            Syscall_ReleaseIpiControl();
        }
        break;
    }

    case UPCI_KOP_SETBGPMTHREAD: {
        DEBUG_PRINT("UPCI Syscall SetBgpmThread %d\n", Kernel_ProcessorID());

        // Set this kthread as owning the hw threads upc hardware.
        UPC_KBridge_SetActiveKThread();

        // make sure swap config is clear (I'd bet this is overkill - shouldn't be needed)
        unsigned procId = Kernel_ProcessorID();
        upcHwSwapCfgs[procId].punitControl = 0;
        UPC_KBridge_ClearSwapCfg();

        break;
    }

    case UPCI_KOP_CLEARBGPMTHREAD: {
        DEBUG_PRINT("UPCI Syscall ClearBgpmThread %d\n", Kernel_ProcessorID());

        // make sure swap config is clear
        unsigned procId = Kernel_ProcessorID();
        upcHwSwapCfgs[procId].punitControl = 0;
        UPC_KBridge_ClearSwapCfg();

        // Set this kthread as owning the hw threads upc hardware.
        UPC_KBridge_ClearActiveKThread();
        break;
    }

    case UPCI_KOP_WAIT4MAILBOX: {
        DEBUG_PRINT("UPCI Syscall Wait4MailboxEmpty %d\n", Kernel_ProcessorID());

        rc = Kernel_Upci_Wait4MailboxEmpty();
        return rc;  // don't want to negate rc or add another possible print.
        break;
    }

    case UCPI_KOP_GETKDEBUG: {
        if (dsize == sizeof(Upci_KDebug_t)) {
            DEBUG_PRINT("UPCI Syscall KDebug %d\n", Kernel_ProcessorID());

            rc = Kernel_Upci_GetKDebug_Copy((Upci_KDebug_t*)pData, &upci_KDebug);
        }
        else {
            DEBUG_PRINT("(E) Upci_Do_Syscall GetKDebug dsize=%ld, expected=%ld\n", dsize, sizeof(Upci_KDebug_t));
            rc = UPCI_ESYSCALL_DATASIZE;
        }
        break;
    }


    case UCPI_KOP_SETPMSIG: {
        if (dsize == sizeof(int)) {
            DEBUG_PRINT("Upci_SetPMSig %d\n", *((int*)pData));
            upci_PMSignalVal = *((int*)pData);
        }
        else {
            DEBUG_PRINT("(E) Upci_Do_Syscall SetPmSig dsize=%ld, expected=%ld\n", dsize, sizeof(int));
            rc = UPCI_ESYSCALL_DATASIZE;
        }
        break;
    }


    case UPCI_KOP_GETCNKCTRS: {
        if ((dsize % sizeof(PerfCountItem_t)) == 0) {
            dsize /= sizeof(PerfCountItem_t);
            DEBUG_PRINT("Upci_GetCNKCounts ctrtype=%ld\n", targ);
            switch ((Upci_CNKCtrType_t)targ) {
            case CNKCtrNode: {
                PerfCtr_GetNodeData((int)dsize, (PerfCountItem_t*)pData);
                break;
            }
            case CNKCtrProc: {
                PerfCtr_GetProcData((int)dsize, (PerfCountItem_t*)pData);
                break;
            }
            case CNKCtrHWT: {
                PerfCtr_GetHWTData((int)dsize, (PerfCountItem_t*)pData);
                break;
            }
            }
        }
        else {
            DEBUG_PRINT("(E) Upci_Do_Syscall Upci_GetCNKCounts dsize=%ld, expected multiple of %ld\n", dsize, sizeof(PerfCountItem_t));
            rc = UPCI_ESYSCALL_DATASIZE;
        }
        break;
    }



    default: {
        DEBUG_PRINT("(E) Upci_Do_Syscall Op Invalid = %ld\n", op);
        rc = UPCI_ESYSCALL_INVALID;
    }
    }

    // return only positive return codes - syscall spi code will negate will appropriate.
    // Also, UPCI error values must not overflap ERRNO values.
    rc *= -1;
    DEBUG_PRINT("(I) Upci_Do_Syscall rc = 0x%016lx\n", rc);
    return rc;
}




void Upci_SwapOut(void *pCfg) {
    assert(pCfg);
    Upci_SwapCfg_t * pSwCfg = pCfg;
    unsigned core = Kernel_ProcessorCoreID();
    unsigned thd = Kernel_ProcessorThreadID();
    assert(pSwCfg == &upcHwSwapCfgs[Kernel_ProcessorID()]);

    pSwCfg->saveControlState = upc_p_local->control_w1s;  // get current run state
    upc_p_local->control_w1c = pSwCfg->punitControl; // stop counting for this thread
    pSwCfg->saveIntEnables = upc_p_local->p_config;  // get everything, but only restore int enable bits later
    // pSwCfg->saveIntStatus = upc_p_local->int_status[Kernel_ProcessorThreadID()];

    // \todo It would be faster to switch this locking in favor of an L2 lock rather than larx/stx
    UPC_Lock(&upcHwThdLocks[core]);
    upc_p_local->p_config &= ~UPC_P__CONFIG__UPC_P_INT_ENABLE_allbits(thd);
    UPC_Unlock(&upcHwThdLocks[core]);
}


void Upci_SwapIn(void *pCfg) {
    assert(pCfg);
    Upci_SwapCfg_t * pSwCfg = pCfg;
    assert(pSwCfg == &upcHwSwapCfgs[Kernel_ProcessorID()]);
    unsigned core = Kernel_ProcessorCoreID();
    unsigned thd = Kernel_ProcessorThreadID();

    UPC_Lock(&upcHwThdLocks[core]);
    upc_p_local->p_config |= pSwCfg->saveIntEnables &
            (UPC_P__CONFIG__UPC_P_INT_ENABLE_allbits(thd) | UPC_P__CONFIG__SHARED_MODE_set(1));
    UPC_Unlock(&upcHwThdLocks[core]);
    //upc_p_local->int_status_w1s = pSwCfg->saveIntStatus;
    upc_p_local->control_w1s = (pSwCfg->saveControlState & pSwCfg->punitControl);
}



// PerfMon Exception handler - called by perf mon exception vector handler.
void Upci_PerfMon_Exception() {

    // Mask the UPC_P interrupts for the current thread.
    // then register a signal handler for the current KThread owning the
    // UPC hardware for this thread.
    // The Signal handler is responsible to re-enable the UPC interrupts for this
    // hardware thread.
    unsigned core = Kernel_ProcessorCoreID();
    unsigned thd = Kernel_ProcessorThreadID();

    DEBUG_PRINT("(I) Perfmon exception for thread %02d:%d - signal=%d\n", core, thd, upci_PMSignalVal);

    UPC_Lock(&upcHwThdLocks[core]);
    upc_p_local->p_config &= ~(UPC_P__CONFIG__UPC_P_INT_ENABLE_allbits(thd));
    UPC_Unlock(&upcHwThdLocks[core]);

    DEBUG_PRINT("(I) after reset status - p_config=0x%016lx, int_status=0x%016lx\n",
            upc_p_local->p_config, upc_p_local->int_status_w1s);

    UPC_C_Stop_Sync_Counting();  // stop L2 without delay
    UPC_IO_Stop(nodeConfig);

    // UPC_P could still be driving interrupts between jobs.  If we start a new job
    // the signal value should be zero to prevent delivery anyplace.
    if (upci_PMSignalVal > 0) {
        UPC_KBridge_Signal_Deliver(upci_PMSignalVal);
    }
}



