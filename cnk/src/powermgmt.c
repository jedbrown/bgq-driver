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
#include "hwi/include/bqc/BIC_inlines.h"
#include "hwi/include/bqc/gea_dcr.h"
#include "hwi/include/bqc/l2_central_inlines.h"
#include "hwi/include/bqc/ddr.h"
#include "hwi/include/bqc/dr_arb_dcr.h"
#include "spi/include/l2/atomic.h"
#include "hwi/include/bqc/en_dcr.h"

extern NodeState_t NodeState;

#define POWERMGMT_REACTIVE_PERIOD       100   // 100 usec
#define POWERMGMT_REACTIVE_DURATION      10   // 10  usec
#define POWERMGMT_REACTIVE_DDRTHROTTLE    0   // 0 clicks out of 126

Lock_Atomic_t PowerMgmtWakeup;
uint64_t      threadThrottling K_ATOMIC;
uint64_t      powermgmt_duration = 0;

void IntHandler_PowerEvent(int status_reg, int bitnum)
{
    uint64_t intstatus = DCRReadPriv(EN_DCR(ENVMON_INTERRUPT_STATE__STATE));
    
    if(intstatus & EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN3_set(1))
    {
        uint64_t value = DCRReadPriv(EN_DCR(EN3_INPUT)) * 256;
        RASBEGIN(1);
        RASPUSH(value);
        RASFINAL(RAS_POWERTHRESHOLD_A2);
        
        Kernel_WriteFlightLog(FLIGHTLOG, FL_PWREVNTA2, value, POWERMGMT_REACTIVE_DURATION, POWERMGMT_REACTIVE_PERIOD, mfspr(SPRN_SRR0));
        
        uint64_t powermgmt_period = ((uint64_t)POWERMGMT_REACTIVE_PERIOD) * GetPersonality()->Kernel_Config.FreqMHz / 2;
        powermgmt_duration = ((uint64_t)POWERMGMT_REACTIVE_DURATION) * GetPersonality()->Kernel_Config.FreqMHz;
        
        BIC_WriteGeaInterruptControlRegisterHigh(GEA_DCR__GEA_INTERRUPT_STATE_CONTROL_HIGH__TIMER1_INT_set(3));
        DCRWritePriv(GEA_DCR(TIMER1_CONFIG), GEA_DCR__TIMER1_CONFIG__RELOAD_VAL_set(powermgmt_period) | GEA_DCR__TIMER1_CONFIG__IE_set(1) | GEA_DCR__TIMER1_CONFIG__ARE_set(1));
        DCRWritePriv(GEA_DCR(TIMER1), GEA_DCR__TIMER1__COUNT_set(powermgmt_period));
        ppc_msync();
        
        DCRWritePriv(EN_DCR(EN3_THRESHOLDS), 0xffc00);
        DCRWritePriv(EN_DCR(ENVMON_INTERRUPT_STATE__STATE), EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN3_set(1));
    }
    
    if(intstatus & EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN4_set(1))
    {
        int mc;
        uint64_t mcamisc;
        uint64_t ddr_throttle = POWERMGMT_REACTIVE_DDRTHROTTLE;
        
        uint64_t value = DCRReadPriv(EN_DCR(EN4_INPUT)) * 64;
        RASBEGIN(1);
        RASPUSH(value);
        RASFINAL(RAS_POWERTHRESHOLD_DDR);
        Kernel_WriteFlightLog(FLIGHTLOG, FL_PWREVNTMC, value, POWERMGMT_REACTIVE_DDRTHROTTLE, mfspr(SPRN_SRR0), 0);

        for(mc=0; mc<2; mc++)
        {        
            mcamisc = DCRReadPriv(_DDR_MC_MCAMISC(mc)) & (~_B7(43, -1ULL));
            DCRWritePriv(_DDR_MC_MCAMISC(mc), mcamisc | _B7(43, (uint64_t)ddr_throttle));
        }
        DCRWritePriv(EN_DCR(EN4_THRESHOLDS), 0xffc00);
        DCRWritePriv(EN_DCR(ENVMON_INTERRUPT_STATE__STATE), EN_DCR__ENVMON_INTERRUPT_STATE__EXCEEDS_EN4_set(1));
    }
}

void PowerMgmt_PollThrottlingThreads()
{
    uint64_t spinuntil = GetTimeBase() + powermgmt_duration;
    while(spinuntil > GetTimeBase())
    {
    }
    uint64_t threadcnt = L2_AtomicLoadClear(&threadThrottling);
    ppc_msync();
    
    GEA_DCR_PRIV_PTR->gea_interrupt_state__noncritical = GEA_DCR__GEA_INTERRUPT_STATE__TIMER1_INT_set(1);
    BIC_WriteGeaInterruptState(GEA_DCR__GEA_INTERRUPT_STATE__TIMER1_INT_set(1));
    
    LoadReserved32( &(PowerMgmtWakeup.atom) );
    StoreConditional32( &(PowerMgmtWakeup.atom), 0);
    
    Kernel_WriteFlightLog(FLIGHTLOG, FL_PWRMGMTEV, powermgmt_duration, spinuntil, threadcnt, 0);
}

void IntHandler_PowerMgmt(int status_reg, int bitnum)
{
    uint64_t value;
    LoadReserved32( &(PowerMgmtWakeup.atom) );
    ppc_msync();
    value = L2_AtomicLoadIncrement(&threadThrottling);
    if(value == 0)
    {
        PowerMgmt_PollThrottlingThreads();
    }
    else
    {
        ppc_waitrsv();
    }
}

int PowerManagement_Init()
{
    int mc;
    uint32_t val;
    uint32_t ddr_throttle;
    uint64_t period;
    uint64_t mcamisc;
    
    if((NodeState.Personality.Kernel_Config.NodeConfig & PERS_ENABLE_EnvMon) != 0)
    {
        uint64_t envmon_control;
        envmon_control = DCRReadPriv( EN_DCR(ENVMON_INTERRUPT_STATE_CONTROL_HIGH));
        EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN3_insert(envmon_control,  3);
        EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN4_insert(envmon_control,  3);
        DCRWritePriv( EN_DCR(ENVMON_INTERRUPT_STATE_CONTROL_HIGH), envmon_control);
    }
    
    ddr_throttle = 0;
    App_GetEnvValue("BG_POWERMGMTDDRTHROTTLE", &ddr_throttle);
    for(mc=0; mc<2; mc++)
    {
        ddr_throttle = MIN(ddr_throttle, 126);
        mcamisc = DCRReadPriv(_DDR_MC_MCAMISC(mc)) & (~_B7(43, -1ULL));
        DCRWritePriv(_DDR_MC_MCAMISC(mc), mcamisc | _B7(43, (uint64_t)ddr_throttle));
    }
    
    val = 0;
    App_GetEnvValue("BG_POWERMGMTPERIOD", &val);
    period = ((uint64_t)val) * GetPersonality()->Kernel_Config.FreqMHz / 2; // convert usecs to pclks_div_2
    
    val = 0;
    App_GetEnvValue("BG_POWERMGMTDUR", &val);
    powermgmt_duration = ((uint64_t)val) * GetPersonality()->Kernel_Config.FreqMHz; // convert usecs to pclks
    
    if(period)
    {
       printf("(I) Activating Proactive Power Management.   period=%ld pclk   spinduration=%ld pclk\n", period*2, powermgmt_duration);
       BIC_WriteGeaInterruptControlRegisterHigh(GEA_DCR__GEA_INTERRUPT_STATE_CONTROL_HIGH__TIMER1_INT_set(3));
       DCRWritePriv(GEA_DCR(TIMER1_CONFIG), GEA_DCR__TIMER1_CONFIG__RELOAD_VAL_set(period) | GEA_DCR__TIMER1_CONFIG__IE_set(1) | GEA_DCR__TIMER1_CONFIG__ARE_set(1));
       DCRWritePriv(GEA_DCR(TIMER1), GEA_DCR__TIMER1__COUNT_set(period));
       ppc_msync();
    }
    else
    {
        DCRWritePriv(GEA_DCR(TIMER1_CONFIG), 0);
    }
    Kernel_WriteFlightLog(FLIGHTLOG, FL_PWRMGMTTH, ddr_throttle, powermgmt_duration, period*2, 0);
    
    return 0;
}

int PowerManagement_Shutdown()
{
    int mc;
    for(mc=0; mc<2; mc++)
    {
        uint64_t mcamisc = DCRReadPriv(_DDR_MC_MCAMISC(mc)) & (~_B7(43, -1ULL));
        DCRWritePriv(_DDR_MC_MCAMISC(mc), mcamisc);
    }

    DCRWritePriv(GEA_DCR(TIMER1_CONFIG), 0);
    Kernel_WriteFlightLog(FLIGHTLOG, FL_PWRMGMTSD, 0,0,0,0);
    return 0;
}

