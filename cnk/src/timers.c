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
#include "flih.h"
#include <sys/time.h>
#include "hwi/include/bqc/BIC_inlines.h"
#include "hwi/include/bqc/gea_dcr.h"
#include "hwi/include/bqc/l2_central_inlines.h"

#define CR_ERROR  (0x10000000)

static void DisableDecrementer()
{
    // Clear decrementer-interrupt-enable and auto-reload-enable.  Then clear
    // the decrementer and clear any interrupt condition that may have just
    // occurred.
    mtspr(SPRN_TCR, mfspr(SPRN_TCR) & ~(TCR_DIE | TCR_ARE));
    mtspr(SPRN_DEC, 0);
    mtspr(SPRN_TSR, TSR_DIS);
}

static void EnableDecrementer(uint64_t value, uint64_t interval)
{
    // Enable decrementer interrupts.
    uint64_t tcr = TCR_DIE;
    if ((interval > 0) && ((interval >> 32) == 0))
    {
	// Interval is non-zero and fits in 32 bits, so arm the auto-reload
	// mechanism.
	mtspr(SPRN_DECAR, interval);
	tcr |= TCR_ARE;
    }
    if ((value >> 32) != 0)
    {
	// Value is out of range, so set it as far out as we can.
	value = 0xffffffff;
    }
    mtspr(SPRN_DEC, value);
    // Clear any residual interrupt condition and then enable.
    mtspr(SPRN_TSR, TSR_DIS);
    mtspr(SPRN_TCR, mfspr(SPRN_TCR) | tcr);
}

static void ResetDecrementer(uint64_t value, uint64_t interval)
{
    // If interval is non-zero and fits in 32 bits, we will have enabled
    // the auto-reload mechanism and do not need to reset the decrementer.
    if ((interval == 0) || ((interval >> 32) != 0))
    {
	if ((value >> 32) != 0)
	{
	    // Value is out of range, so set it as far out as we can.
	    value = 0xffffffff;
	}
	mtspr(SPRN_DEC, value);
    }
}

__C_LINKAGE void IntHandler_DEC(Regs_t *context)
{
    uint64_t currentTime = GetTimeBase();
    HWThreadState_t* hwthr = GetMyHWThreadState();
    // If speculation is active, set restart point. Defer the invalidation until later
    // when we determine if we are going to attempt to deliver the signal.
    Speculation_CheckJailViolation(&GetMyKThread()->Reg_State);

    // Ack the decrementer.
    mtspr(SPRN_TSR, TSR_DIS);

    // Check whether the timer has expired.
    if (hwthr->itimer.expiration == 0)
    {
	// spurious interrupt
    }
    else if (currentTime >= hwthr->itimer.expiration)
    {
	if (hwthr->itimer.signal != 0)
	{
	    Signal_Deliver(hwthr->itimer.owner->pAppProc, GetTID(hwthr->itimer.owner), hwthr->itimer.signal);
	}

	if (hwthr->itimer.interval > 0)
	{
	    hwthr->itimer.expiration += hwthr->itimer.interval;
	    if (hwthr->itimer.expiration <= currentTime)
	    {
		// Shouldn't happen, but just in case ...
		hwthr->itimer.expiration = currentTime + 1;
	    }
	    ResetDecrementer(hwthr->itimer.expiration - currentTime,
			     hwthr->itimer.interval);
	}
	else
	{
	    hwthr->itimer.expiration = 0;
	    hwthr->itimer.owner = NULL;
	    hwthr->itimer.signal = 0;
	    DisableDecrementer();
	}
    }
    else
    {
	ResetDecrementer(hwthr->itimer.expiration - currentTime,
			 hwthr->itimer.interval);
    }

    CountException(CNK_HWTPERFCOUNT_DECREMENTER);
}

__C_LINKAGE void IntHandler_FIT(Regs_t *context)
{
    CountException(CNK_HWTPERFCOUNT_FIT);
}

__C_LINKAGE void IntHandler_WDT(Regs_t *context)
{
    CountException(CNK_HWTPERFCOUNT_WATCHDOG);
}

uint64_t Timer_GetTimer(int sig, uint64_t *remainder, uint64_t *old_interval)
{
    HWThreadState_t* hwthr = GetMyHWThreadState();

    if (hwthr->itimer.signal == sig)
    {
	uint64_t cyclesPerMicro = GetPersonality()->Kernel_Config.FreqMHz;
	int64_t cycles = hwthr->itimer.expiration - GetTimeBase();
	if (cycles < 0) cycles = 0;
	if (remainder != NULL) (*remainder) = cycles / cyclesPerMicro;
	if (old_interval != NULL) (*old_interval) =
				    hwthr->itimer.interval / cyclesPerMicro;
    }
    else
    {
	if (remainder != NULL) (*remainder) = 0;
	if (old_interval != NULL) (*old_interval) = 0;
    }

    return CNK_RC_SUCCESS(0);
}

uint64_t Timer_SetTimer(int sig, uint64_t value, uint64_t interval,
			uint64_t *remainder, uint64_t *old_interval)
{
    HWThreadState_t* hwthr = GetMyHWThreadState();

    /*
     * Restriction: We only support a single ITIMER type.  This assumption
     *   allows us to dedicate the decrementer's usage to this single clock.
     *   To support more, we would need more elaborate data structures and
     *   would also need more  sophisticated algorithms to schedule DEC for
     *   the minimum interval of all pending timers.  For now, we simply
     *   retain the restriction from BG/L (blrts).
     */

    if ((hwthr->itimer.signal != 0) && (hwthr->itimer.signal != sig))
    {
	return CNK_RC_FAILURE(EAGAIN);
    }

    Timer_GetTimer(sig, remainder, old_interval);

    if (value > 0) 
    {
	if (hwthr->itimer.expiration != 0)
	{
	    DisableDecrementer();
	}
	uint64_t currentTime = GetTimeBase();
	uint64_t cyclesPerMicro = GetPersonality()->Kernel_Config.FreqMHz;
	hwthr->itimer.expiration = currentTime + (value * cyclesPerMicro);
	hwthr->itimer.interval   = interval * cyclesPerMicro;
	hwthr->itimer.owner      = GetMyKThread();
	hwthr->itimer.signal     = sig;
	EnableDecrementer(hwthr->itimer.expiration - currentTime,
			  hwthr->itimer.interval);
    }
    else
    {
	hwthr->itimer.expiration = 0;
	hwthr->itimer.interval   = 0;
	hwthr->itimer.owner      = NULL;
	hwthr->itimer.signal     = 0;
	DisableDecrementer();
    }

    return CNK_RC_SUCCESS(0);
}

uint64_t Timer_NanoSleep(struct timespec *req, struct timespec *rem)
{
    if ((req->tv_nsec < 0) || (req->tv_nsec >= 1000000000))
    {
	return CNK_RC_FAILURE(EINVAL);
    }

    uint64_t cyclesPerMicro = (GetPersonality()->Kernel_Config.FreqMHz);
    uint64_t micros = (((uint64_t) req->tv_sec) * 1000000) +
			    (((uint64_t) req->tv_nsec) / 1000);
    uint64_t current_timebase = GetTimeBase();
    uint64_t expiration = current_timebase + (micros * cyclesPerMicro);

    if (expiration > current_timebase)
    {
        HWThreadState_t *hwt = GetMyHWThreadState();
        KThread_t *kthr = GetMyKThread();
        kthr->SleepTimeout = expiration;
        kthr->SleepRemainder = rem; // may be NULL
        Sched_Block(kthr, SCHED_STATE_SLEEP);
        // Set a Pending bit to cause this thread to call Scheduler() before
        // it is resumed.
        kthr->Pending |= KTHR_PENDING_YIELD;

        // If no timer is set or if there is a timer that expires later in time, set the timer with this value.
        if (!(hwt->udecr_expiration) || (hwt->udecr_expiration > expiration))
        {
            Timer_enableUDECRwakeup(current_timebase, expiration);
        }
        else
        {
            // The timer is already set with an appropriate expiration
        }
    }
    return CNK_RC_SUCCESS(0);
}

void Timer_Interrupt(KThread_t *kthr)
{
    kthr->Reg_State.cr     |= CR_ERROR; // syscall failed
    kthr->Reg_State.gpr[3] = EINTR; // result of the nanosleep syscall
    if (kthr->SleepRemainder != NULL)
    {
	uint64_t cyclesPerMicro = GetPersonality()->Kernel_Config.FreqMHz;
	int64_t cycles = kthr->SleepTimeout - GetTimeBase();
	if (cycles < 0) cycles = 0;
	uint64_t micros = cycles / cyclesPerMicro;
	kthr->SleepRemainder->tv_sec = (time_t) (micros / 1000000);
	kthr->SleepRemainder->tv_nsec = (long) ((micros % 1000000) * 1000);
    }
    kthr->SleepTimeout = 0;
    Sched_Unblock(kthr, SCHED_STATE_SLEEP);
    Timer_disableUDECRwakeup();
}

void Timer_Awaken(KThread_t *kthr)
{
    if (kthr->SleepRemainder != NULL)
    {
	kthr->SleepRemainder->tv_sec = 0;
	kthr->SleepRemainder->tv_nsec = 0;
    }
    kthr->SleepTimeout = 0;
    Sched_Unblock(kthr, SCHED_STATE_SLEEP);
}

__C_LINKAGE void IntHandler_UDEC(Regs_t *context)
{
    CountException(CNK_HWTPERFCOUNT_UDEC);

    //printf("In UDEC handler\n");

    uint64_t new_expiration = 0;
    uint64_t candidate_expiration = 0;
    KThread_t *myKthread = GetMyKThread();
    int inTheScheduler = (myKthread->SlotIndex == CONFIG_SCHED_KERNEL_SLOT_INDEX) ? 1 : 0;
    int x;
    KThread_t *kthread;
    HWThreadState_t *hwt = GetMyHWThreadState();
    uint64_t currentTime = GetTimeBase();
    int dispatchNeeded = 0;

    // Look for a nanosleep and futex timeout conditions
    for (x=0; x<CONFIG_SCHED_KERNEL_SLOT_INDEX; x++)
    {
        kthread = hwt->SchedSlot[x];
        // First process nanosleep requests
        if (kthread->SleepTimeout)
        {
            if (kthread->SleepTimeout <= currentTime)
            {
                Timer_Awaken(kthread);

                // are we waking a higher priority thread?
                if (!inTheScheduler && (kthread->Priority > myKthread->Priority))
                {
                    dispatchNeeded = 1;
                }
            }
            else
            {
                // There is still a nanosleep that has not expired. Re-establish the timer interval
                candidate_expiration = kthread->SleepTimeout;
                if ((new_expiration == 0) || (new_expiration > candidate_expiration))
                {
                    new_expiration = candidate_expiration;
                }
            }
        }
        // Look for timed futex requests
        if(Futex_CheckTimeout(currentTime, kthread, &candidate_expiration))
        {
            // A timeout condition was satisfied. Check to see if we need to run the scheduler
            if (!inTheScheduler && (kthread->Priority > myKthread->Priority))
            {
                dispatchNeeded = 1;
            }
        }
        else
        {
            // If there is a pending expiration for this kthread, we must compare it to the existing expiration
            if (candidate_expiration)
            {
                if ((new_expiration == 0) || (new_expiration > candidate_expiration))
                {
                    new_expiration = candidate_expiration;
                }
            }
        }
    }
    // See if we have a condition in which the currently running kthread must be preempted.
    if (dispatchNeeded)
    {
        myKthread->Pending |= KTHR_PENDING_YIELD;  // Force a call to the scheduler after returning from this interrupt handler. 
    }
    // Test to see if there is an expiration of the IPI Message checker
    if (hwt->ipiMessageCheckerExpiration)
    {
        if (currentTime > hwt->ipiMessageCheckerExpiration)
        {
            // Run the IPI message checker and drive out IPI messages if necessary
            IntHandler_IPIchecker();
            hwt->ipiMessageCheckerExpiration = currentTime + ((uint64_t)CONFIG_IPIMESSAGECHECKER_INTERVAL * (uint64_t)GetPersonality()->Kernel_Config.FreqMHz);
            candidate_expiration = hwt->ipiMessageCheckerExpiration;
        }
        else
        {
            candidate_expiration = hwt->ipiMessageCheckerExpiration;
        }
        if ((new_expiration == 0) || (new_expiration > candidate_expiration))
        {
            new_expiration = candidate_expiration;
        }
    }
    if (new_expiration)
    {
        Timer_enableUDECRwakeup(currentTime, new_expiration);
    }
    else
    {
        Timer_disableUDECRwakeup();
    }
}

void Timer_disableUDECRwakeup() 
{
    // Disable the the counter by storing zero. (We just leave UDIE on. No counting or interrupt will occur with udecr = 0 )
    mtspr(SPRN_TCR, (mfspr(SPRN_TCR) | TCR_UD) & ~(TCR_UDIE));
    isync();
    // Clear the interrupt status  
    mtspr(SPRN_UDEC, 0); // Set to zero so that counting stops
    isync();
    mtspr(SPRN_TSR, TSR_UDIS); // Reset any pending interrupt condition.
    isync();
    // Turn off read/write of udec so that problem state does not have access to it.
    mtspr(SPRN_TCR, (mfspr(SPRN_TCR) & ~(TCR_UD)));
    GetMyHWThreadState()->udecr_expiration = 0; // indicate that no udecr timer is currently active.
}

void Timer_enableUDECRwakeup(uint64_t current_timebase, uint64_t expiration) 
{
    // if the interval is greater than the max allowed, set the max interval.
    uint64_t requested_interval = expiration - current_timebase;
    uint64_t interval = ((requested_interval>>32) ? 0xFFFFFFFFUL : requested_interval); 
    HWThreadState_t *hwt = GetMyHWThreadState();
    // Set the actual expiration time based on the max interval allowed. If it fires early, the interrupt
    // handler will continue to set the largest intervals possible until we reach the requested expiration.
    hwt->udecr_expiration = current_timebase + interval;

    // Allow writing to the udecr register
    mtspr(SPRN_TCR, (mfspr(SPRN_TCR) | TCR_UD) & ~(TCR_UDIE));
    isync(); // necessary to ensure that the TCR_UD setting has completed before touching the UDEC
    mtspr(SPRN_TSR, TSR_UDIS); // Reset any pending interrupt condition.
    isync();
    mtspr(SPRN_UDEC, interval); // Now setup the new interval. We want to process any interrupts after this point.
    isync(); // necessary before turning off the TCR_UD setting.
    mtspr(SPRN_TCR,((mfspr(SPRN_TCR) | TCR_UDIE) & ~(TCR_UD))); // turn off read/write of udec so that problem state does not have access to it.
}

void Timer_enableFutexTimeout(uint64_t current_timebase, uint64_t expiration)
{
    Timer_enableUDECRwakeup(current_timebase, expiration);
}

void IntHandler_GEATimerEvent(int status_reg, int bitnum)
{
    uint64_t nc_event = GEA_DCR_PRIV_PTR->gea_interrupt_state__state;
    if(nc_event & GEA_DCR__GEA_INTERRUPT_STATE__TIMER0_INT_set(1))
    {
        // GEA Timer 0 - - rollback
        IntHandler_RollbackSnapshot(status_reg, bitnum);
    }
    
    if(nc_event & GEA_DCR__GEA_INTERRUPT_STATE__TIMER1_INT_set(1))
    {
        // GEA Timer 1 - - Power management
        IntHandler_PowerMgmt(status_reg, bitnum);
    }

    if(nc_event & GEA_DCR__GEA_INTERRUPT_STATE__TIMER2_INT_set(1))
    {
        // GEA Timer 2 - - Background Scrub
        IntHandler_MemoryScrub(status_reg, bitnum);
    }

    if(nc_event & GEA_DCR__GEA_INTERRUPT_STATE__TIMER3_INT_set(1))
    {
        // GEA Timer 3 - - undefined
        assert(0);
    }
}
