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
/* (C) Copyright IBM Corp.  2007, 2012                              */
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


#include <stdio.h>
#include "Kernel.h"
#include <linux/futex.h>

#define CR_ERROR  (0x10000000)
#define UNBLOCK_LIST_SIZE  (CONFIG_MAX_APP_THREADS*(CONFIG_SCHED_SLOTS_PER_HWTHREAD-1))

int Futex_IsShared(Futex_t *futex_vaddr, uint32_t op_and_flags)
{
    if (op_and_flags & FUTEX_PRIVATE_FLAG)
    {
        return 0; // Treat as as private futex regardless of what storage that it occupies
    }
    else
    {
        AppProcess_t *proc = GetMyProcess();
        // If in shared storage, indicate that this is a shared futex
        if (((uint64_t)futex_vaddr >= proc->Shared_VStart) && 
            ((uint64_t)futex_vaddr <= proc->Shared_VEnd))
        {
            return 1; // The Private flag is not on and we are in the shared memory region. Treat as a shared mutex
        }
        return 0; // Not in the shared memory region, and not specifically identified as private, however, treat as private
    }
}

uint64_t Lock_AtomicAcquire(int isShared)
{
    Lock_Atomic64_t *alock;
    Lock_Atomic64_t *olock;
    if (isShared)
    {
        alock   = &(NodeState.FutexLockShared);
        olock   = &(NodeState.FutexOrderLockShared);
    }
    else
    {
        AppProcess_t *proc = GetMyProcess();
        alock   = &(proc->FutexLockPrivate);
        olock   = &(proc->FutexOrderLockPrivate);
    }
    uint64_t my_turn;

    my_turn = fetch_and_add64(alock, 1);

    if (ATOMIC_READ( olock ) != my_turn)
    {
        // lower our priority while we wait for the lock to become available
        ThreadPriority_Low();
        while ( ATOMIC_READ( olock ) != my_turn )
        {
        }
        ThreadPriority_Medium(); // restore priority
    }

    return my_turn;
}

// see cnk/bgp_futex.c and cnk/syscalls/bgp_atomic_workarounds.c
void Lock_AtomicRelease(int isShared, uint64_t my_turn)
{
    Lock_Atomic64_t *olock;
    if (isShared)
    {
        olock   = &(NodeState.FutexOrderLockShared);
    }
    else
    {
        AppProcess_t *proc = GetMyProcess();
        olock   = &(proc->FutexOrderLockPrivate);
    }

    ATOMIC_WRITE( olock, my_turn + 1 );
}


// Returns the entry associated with the given futex address.  This routine should
// be called within a lock context!

Futex_State_t* Futex_findTableEntry(Futex_t* futexAddress, int isShared, int allocateIfNecessary )
{
    Futex_State_t* result = (Futex_State_t*)0;
    int i;

    Futex_State_t *futex_table;
    if (isShared)
    {
        futex_table = NodeState.Futex_Table_Shared;
    }
    else
    {
        futex_table = GetMyProcess()->Futex_Table_Private;
    }
    // Find the entry in the futex table corresponding to the futex's
    // virtual (user) address:
    for ( i = 0 ; (i < NUM_FUTEX) && !result ; i++ )
    {
        if ( futex_table[i].futex_vaddr == futexAddress )
        {
            result = &(futex_table[i]);
        }
    }

    // If it isnt there, then allocate one:
    if ( !result && allocateIfNecessary)
    {
        for (i = 0; (i < NUM_FUTEX) && !result; i++)
        {
            if (futex_table[i].futex_vaddr == 0)
            {
                result = &(futex_table[i]);
                result->futex_vaddr = futexAddress;
                result->pKThr_Waiter_Next = NULL;
            }
        }

        if (!result)
        {
            printf("(E) Unable to allocate a new futex slot\n");
        }
    }

    return result;
}



// Note: they will/must wake in FIFO order.
uint64_t Futex_Wait( uint32_t op_and_flags, KThread_t *pKThr, Futex_t* futex_vaddr, uint32_t futex_val, uint64_t timeout, uint64_t current_time )
{
    int thd_index = ProcessorID();
    KThread_t *FtQ;
    int isShared = Futex_IsShared(futex_vaddr, op_and_flags);

    // Enter Short-Term Critical Section and Grab the Atomic Operations Control Lock

    //printf("[%d] before-crit:  futex@%x = %d\n", core, futex_vaddr, *((int*)futex_vaddr));
    uint64_t my_turn = Lock_AtomicAcquire(isShared);
    //printf("[%d] after-crit:   futex@%x = %d\n", core, futex_vaddr, *((int*)futex_vaddr));

    //_FutexRecord( _FUTHIST_WAIT, futex_vaddr, futex_val, *((uint32_t*)futex_vaddr) );



    if ( *((uint32_t*)futex_vaddr) != futex_val )
    {
        TRACE( TRACE_Futex, ("(I) Futex_Wait[%d]: EWOULDBLOCK address 0x%016lx futex_val %016lx, *futex_vaddr %d\n",
                               thd_index, (uint64_t)futex_vaddr, (uint64_t)futex_val, *((uint32_t*)futex_vaddr) ));
        Lock_AtomicRelease(isShared, my_turn);

        return CNK_RC_FAILURE(EWOULDBLOCK);
    }

    TRACE( TRACE_Futex, ("(I) Futex_Wait[%d]: WAITING address 0x%016lx (kthread=0x%p) time=0x%016lx\n", 
                         thd_index, (uint64_t)futex_vaddr, pKThr, current_time));

    Futex_State_t* futexTableEntry = Futex_findTableEntry(futex_vaddr, isShared, 1);

    if (futexTableEntry)
    {
        if (futexTableEntry->pKThr_Waiter_Next)
        {
            for (FtQ = futexTableEntry->pKThr_Waiter_Next; FtQ->FutexQueueNext; FtQ = FtQ->FutexQueueNext)
            {
            }
            FtQ->FutexQueueNext = pKThr;
        }
        else
        {
            futexTableEntry->pKThr_Waiter_Next = pKThr;
        }

        pKThr->FutexQueueNext = (KThread_t *)0;
        pKThr->FutexVAddr     = futex_vaddr;
        pKThr->FutexValue     = *futex_vaddr;
        pKThr->FutexTimeout   = timeout;
        pKThr->FutexIsShared  = isShared;
        ppc_msync();
        // If this is a timed futex. Setup the udecr timer 
        if (timeout)
        {
            if (timeout < current_time)
            {
                timeout = current_time+1;
            }
            Timer_enableFutexTimeout(current_time, timeout);
        }
    }
    else
    {
        TRACE( TRACE_Futex, ("(I) Futex_Wait[%d]: futexTableEntry == NULL\n", thd_index));
    }

    Sched_Block(GetMyKThread(), SCHED_STATE_FUTEX );
    Lock_AtomicRelease(isShared, my_turn);
    // TRACE( TRACE_Futex, ("(<) %s[%d]\n", __func__, core));

    // The thread's non-volatile state has not been saved yet.  Set a Pending
    // bit, which will result in a full state save and a call to Scheduler().
    pKThr->Pending |= KTHR_PENDING_YIELD;

    return CNK_RC_SUCCESS(0);
}

int Futex_Timeout(KThread_t* thread)
{
    KThread_t *FtQ;

    Futex_State_t* futexTableEntry = Futex_findTableEntry(thread->FutexVAddr, thread->FutexIsShared, 0);
    if (futexTableEntry != NULL)
    {
        if (futexTableEntry->pKThr_Waiter_Next == thread)
        {
            futexTableEntry->pKThr_Waiter_Next = thread->FutexQueueNext;
            // If we have no waiters on this futex address, we must remove it from the table.
            if (futexTableEntry->pKThr_Waiter_Next == 0)
            {
                futexTableEntry->futex_vaddr = 0;
            }
        }
        else
        {
	    FtQ = futexTableEntry->pKThr_Waiter_Next;
	    while ((FtQ != NULL) && (FtQ->FutexQueueNext != thread))
	    {
		FtQ = FtQ->FutexQueueNext;
	    }
	    if (FtQ != NULL)
	    {
		FtQ->FutexQueueNext = thread->FutexQueueNext;
	    }
        }
    }
    thread->Reg_State.cr     |= CR_ERROR;  // syscall failed
    thread->Reg_State.gpr[3] = ETIMEDOUT; // this is the result of the futex syscall
    thread->FutexQueueNext    = (KThread_t *)0;
    thread->FutexVAddr        = NULL;
    thread->FutexValue        = 0;
    thread->FutexTimeout      = 0;
    thread->FutexIsShared     = 0;
    //thread->pad3 = 3; // TEMP PROBLEM ANALYSIS
    ppc_msync();

    Sched_Unblock( thread, SCHED_STATE_FUTEX );

    return 0;
}

int Futex_CheckTimeout(uint64_t currentTime, KThread_t *thread, uint64_t *pending_expiration)
{
    int rc = 0;
    *pending_expiration = 0; // return code value indicates an existing expiration for a futex on this kthread.
    if (thread && thread->FutexTimeout)
    {
        int isShared = thread->FutexIsShared;
        uint64_t my_turn = Lock_AtomicAcquire(isShared);
        // Check again after obtaining the lock. May have changed
        uint64_t expiration = thread->FutexTimeout; // Read this volatile into a local variable
        // Is there a timed futex on this kthread?
        if (expiration)
        {
            if (expiration <= currentTime)
            {
                Futex_Timeout(thread);
                rc = 1;
            }
            else
            {
                // Expiration for this timed futex has not been reached. Return the expiration value to the caller.
                *pending_expiration = expiration;
            }
        }
        Lock_AtomicRelease(isShared, my_turn);
    }
    return rc;
}

static int Futex_WakeQueue(Futex_State_t* futexTableEntry, int maxToWake, Futex_State_t* secondaryFutexTableEntry, uint16_t *unblockList)
{

    int numberAwoken = 0;
    int thd_index = ProcessorID();

    TRACE( TRACE_Futex, ("(I) %s[%d]: table:%p (vaddr:%p) num2wake:%d secondary:%p\n",
                           __func__, thd_index,
                           futexTableEntry, (futexTableEntry ? futexTableEntry->futex_vaddr : 0LL),
                           maxToWake,
                           secondaryFutexTableEntry));

    if (!futexTableEntry)
    {
        TRACE( TRACE_Futex, ("(I) %s[%d]: futexTableEntry == NULL\n",  __func__, thd_index));
        return numberAwoken;
    }

    KThread_t* thread;

    for (thread = futexTableEntry->pKThr_Waiter_Next; thread; thread = futexTableEntry->pKThr_Waiter_Next)
    {

        // TRACE( TRACE_Futex, ("(D) %s[%d]: process thread:%08x (next->%08x\n", __func__, core, (unsigned)thread, (unsigned)(thread->FutexQueueNext)));

        // If we haven't hit the limit, wake up the thread:
        if ( numberAwoken < maxToWake )
        {
            // Advance the futex table to the next entry now because we will be destroying the link
            // in the current entry:
            futexTableEntry->pKThr_Waiter_Next = thread->FutexQueueNext;
            TRACE( TRACE_Futex, ("(I) %s[%d]: waking kthread:%p processorid:%d for futex:%p\n", __func__, thd_index, thread, thread->ProcessorID, thread->FutexVAddr));

            thread->Reg_State.gpr[3] = 0; // this is the result of the futex syscall
            thread->FutexQueueNext   = (KThread_t *)0;
            thread->FutexVAddr       = NULL;
            thread->FutexValue       = 0;
            thread->FutexTimeout     = 0;
            thread->FutexIsShared    = 0;
            //thread->pad3 = 1; // TEMP PROBLEM ANALYSIS
	    unblockList[numberAwoken] = GetTID(thread); // save the kthread in an abbreviated 2-byte format.
            numberAwoken++;
            ppc_msync();
        }
        else if ( secondaryFutexTableEntry )
        {
            // We are requeueing ... move the entire FIFO to the secondary
            // futex queue.  Since the FIFO is a linked list, we can do
            // this by simply snipping off the remaining chain and pasting
            // it onto the secondary:

            KThread_t** tail;

            for (tail = &(secondaryFutexTableEntry->pKThr_Waiter_Next); *tail; tail = &((*tail)->FutexQueueNext)); // find the end of the secondary queue

            futexTableEntry->pKThr_Waiter_Next = 0; // The old queue is now empty
            *tail = thread;                            // Paste the queue remainder onto the end of the existing queue

            // Now update the entries
            for ( ; thread; thread = thread->FutexQueueNext )
            {
                thread->FutexVAddr = secondaryFutexTableEntry->futex_vaddr;
                thread->FutexValue = *thread->FutexVAddr;
                ppc_msync();
            }

            TRACE( TRACE_Futex, ("(I) %s[%d]: some waiters requeued to futex:%016lx (sys:%p) \n",
                                   __func__, thd_index, (uint64_t)secondaryFutexTableEntry->futex_vaddr, secondaryFutexTableEntry ));
        }
        else
        {
            break;
        }
    }

    // If the waiter list is now empty, then remove the entry from the table:
    if ( futexTableEntry->pKThr_Waiter_Next == NULL )
    {
        futexTableEntry->futex_vaddr = 0;
        ppc_msync();
    }
    if ( secondaryFutexTableEntry && (secondaryFutexTableEntry->pKThr_Waiter_Next == NULL) )
    {
        secondaryFutexTableEntry->futex_vaddr = 0;
        ppc_msync();
    }

    // TRACE( TRACE_Futex, ("(<) %s[%d]\n", __func__, core));
    return numberAwoken;
}



#define FUTEX_WAKE        1
#define FUTEX_REQUEUE     3
#define FUTEX_CMP_REQUEUE 4
#define FUTEX_WAKE_OP     5
#define FUTEX_SHIFT       8

#define FUTEX_SET           0
#define FUTEX_ADD           1
#define FUTEX_OR            2
#define FUTEX_NAND          3
#define FUTEX_XOR           4

#define FUTEX_EQ            0
#define FUTEX_NE            1
#define FUTEX_LT            2
#define FUTEX_LE            3
#define FUTEX_GT            4
#define FUTEX_GE            5


uint64_t Futex_Wake( uint32_t op_and_flags, Futex_t* futex_vaddr, int max_to_wake, Futex_t* secondary_vaddr, int max_to_wake2, Futex_t secondary_value )
{
    int thd_index = ProcessorID();
    int sleepers = 0;
    int wakeSecondary = 0;
    uint32_t operation = op_and_flags & FUTEX_CMD_MASK;
    int isShared = Futex_IsShared(futex_vaddr, op_and_flags);
    // List of tids that must be unblocked.
    uint16_t unblockList[UNBLOCK_LIST_SIZE]; 

    // Enter Short-Term Critical Section and Grab the Atomic Operations Control Lock
    //printf("[%d] before-crit:  futex@%x = %d\n", core, futex_vaddr, *((int*)futex_vaddr));
    uint64_t my_turn = Lock_AtomicAcquire(isShared);
    //printf("[%d] after-crit:  futex@%x = %d\n", core, futex_vaddr, *((int*)futex_vaddr));

    if (operation == FUTEX_CMP_REQUEUE)
    {
        if ( *((int32_t*)futex_vaddr) != secondary_value )
        {
            Lock_AtomicRelease(isShared, my_turn);
	    return CNK_RC_FAILURE(EAGAIN);
        }
    }


    if (operation  == FUTEX_WAKE_OP)
    {

        // This is a conditional wake operation.  The secondary_value
        // is encoded  as follows:
        //
        //   - The operator is encoded in bits 0-3 and can be set, add,
        //     or, nand or xor.
        //   - The operand of the operation is encoded in bits 8-19 as a
        //     12-bit signed integer.
        //   - The comparison operator is encoded in bits 4-7 and is
        //     equal, not equal, less than, less than or equal, greater
        //     than, greater than or equal.
        //   - The comparison operand is encoded in bits 20-31 as a 12-bit
        //     signed integer.
        //
        // The conditional wake is defined as follows:
        //   1) the futex is updated using the operator and operand.
        //   2) the original futex value is then compared against the
        //      comparison operand using the comparison operator.  If
        //      the result of the comparison is true, then normal wake
        //      processing occurs.  Otherwise it is skipped.

        Futex_t* futexAddr = secondary_vaddr;
        int32_t op = _G4(secondary_value,3+32);
        int32_t cmpop = _G4(secondary_value,7+32);
        int32_t operand = _G12(secondary_value,19+32);
        operand |= (operand & 0x800) ? 0xfffff000 : 0;
        int32_t cmpOperand = _G12(secondary_value,31+32);
        cmpOperand |= (cmpOperand & 0x800) ? 0xfffff000 : 0;

        if (op & FUTEX_SHIFT)
        {

            // NOTE: BEAM has observed that the shift value (operand) may not be valid.  This is
            //       correct, but we will assume that this is GIGO.  Linux appears to do the same.

            op &= ~FUTEX_SHIFT;
            operand = (1<<operand); /*invalid shift*/
        }

        Futex_t previousValue, newValue;

	do
	{
	    previousValue = (Futex_t) LoadReserved32((uint32_t *) futexAddr);
	    switch ( op )
	    {
		
		case FUTEX_SET :
		    newValue = operand;
		    break;

		case FUTEX_ADD :
		    newValue = previousValue + operand;
		    break;

		case FUTEX_OR :
		    newValue = previousValue | operand;
		    break;

		case FUTEX_NAND :
		    newValue = previousValue & ~operand;
		    break;

		case FUTEX_XOR :
		    newValue = previousValue ^ operand;
		    break;

		default:
		    printf("(E) Unsupported FUTEX_WAKE_OP operation: %d\n", op );
		    newValue = previousValue;
		    break;
	    }
	} while (!StoreConditional32((uint32_t *) futexAddr,
				     (uint32_t) newValue));


        switch ( cmpop )
        {
            case FUTEX_EQ:
                wakeSecondary = (previousValue == cmpOperand);
                break;

            case FUTEX_NE:
                wakeSecondary = (previousValue != cmpOperand);
                break;

            case FUTEX_LT:
                wakeSecondary = (previousValue < cmpOperand);
                break;

            case FUTEX_LE :
                wakeSecondary = (previousValue <= cmpOperand);
                break;

            case FUTEX_GT :
                wakeSecondary = (previousValue > cmpOperand);
                break;

            case FUTEX_GE :
                wakeSecondary = (previousValue >= cmpOperand);
                break;
        }

        TRACE( TRACE_Futex, ("(I) Futex_Wake[%d]: FUTEX_WAKE_OP op=%d operand=%d cmpop=%d cmpoperand=%d old=%d wake=%d\n",
                               thd_index, op, operand, cmpop, cmpOperand, previousValue, wakeSecondary));
    }


    // _FutexRecord( _FUTHIST_WAKE, futex_vaddr, 0, *((uint32_t*)futex_vaddr) );

    TRACE( TRACE_Futex, ("(I) Futex_Wake[%d]: WAKING address %016lx MaxToWake=%d time=0x%016lx\n", 
                         thd_index, (uint64_t)futex_vaddr, max_to_wake, GetTimeBase() ));

    Futex_State_t* futexTableEntry = Futex_findTableEntry(futex_vaddr, isShared, 0);
    Futex_State_t* secondaryFutexTableEntry = (Futex_State_t*)0;

    // For requeing operations, we must locate the secondary futex which is the target
    // of the requeueing operation.  Note that we may need to allocate this entry

    if (futexTableEntry && ((operation == FUTEX_REQUEUE) || (operation == FUTEX_CMP_REQUEUE)) )
    {

        secondaryFutexTableEntry = Futex_findTableEntry(secondary_vaddr, isShared, 1);

        // If we couldnt allocate, we have issues:
        if ( ! secondaryFutexTableEntry )
        {
            TRACE( TRACE_Futex, ("(E) Futex_Wake[%d]: SYSTEM ERROR: not futex table entries available.", thd_index));
            Lock_AtomicRelease(isShared, my_turn);
            return CNK_RC_FAILURE(EAGAIN); 
        }
    }

    if ( futexTableEntry == NULL )
    {
        TRACE( TRACE_Futex, ("(I) Futex_Wake[%d]: No waiter on address %016lx\n", thd_index, (uint64_t)futex_vaddr ));
        sleepers = 0;
    }
    else
    {
        if (max_to_wake > UNBLOCK_LIST_SIZE)
        {
            max_to_wake = UNBLOCK_LIST_SIZE;
        }
        sleepers = Futex_WakeQueue(futexTableEntry, max_to_wake, secondaryFutexTableEntry, &unblockList[0]);
    }

    // WAKE_OP wakes up both futex queues.
    if ((operation == FUTEX_WAKE_OP) && wakeSecondary)
    {
        if (max_to_wake2 > (UNBLOCK_LIST_SIZE - sleepers))
        {
            max_to_wake2 = (UNBLOCK_LIST_SIZE - sleepers);
        }
        sleepers += Futex_WakeQueue(Futex_findTableEntry(secondary_vaddr, isShared, 0), max_to_wake2, NULL, &unblockList[sleepers]);
    }

    Lock_AtomicRelease(isShared, my_turn);

    int i;
    for (i = 0; i < sleepers; i++)
    {
        KThread_t *thread = GetKThreadFromTid(unblockList[i]);
        if (thread) 
        {
            Sched_Unblock( thread, SCHED_STATE_FUTEX );
            // If we are waking a kthread on our hardware thread, we must enter the scheduler to re-evaluate who should run
            if (thread->ProcessorID == ProcessorID())
            {
                // The thread's non-volatile state has not been saved yet.  Set a Pending
                // bit, which will result in a full state save and a call to Scheduler().
                GetMyKThread()->Pending |= KTHR_PENDING_YIELD;
            }
        }
    }
    return CNK_RC_SUCCESS(sleepers);
}

void Futex_Interrupt( KThread_t *pKThr )
{

    int isShared = pKThr->FutexIsShared; // create a local copy of the shared indicator 
    uint64_t my_turn = Lock_AtomicAcquire(isShared);

    Futex_State_t* futexTableEntry = Futex_findTableEntry(pKThr->FutexVAddr, isShared, 0);

    if (futexTableEntry != NULL)
    {
        if (futexTableEntry->pKThr_Waiter_Next == pKThr)
        {
            futexTableEntry->pKThr_Waiter_Next = pKThr->FutexQueueNext;
            // If there are no waiters, free the entry in the futex table.
            if (futexTableEntry->pKThr_Waiter_Next == NULL)
            {
                futexTableEntry->futex_vaddr = 0;
            }
        } 
        else
        {
            KThread_t *FtQ = futexTableEntry->pKThr_Waiter_Next;
            while ((FtQ != NULL) && (FtQ->FutexQueueNext != pKThr))
            {
                FtQ = FtQ->FutexQueueNext;
            }
            if (FtQ != NULL)
            {
                FtQ->FutexQueueNext = pKThr->FutexQueueNext;
            }
        }
    }
    pKThr->Reg_State.cr     |= CR_ERROR;  // syscall failed
    pKThr->Reg_State.gpr[3]  = EINTR; // this is the result of the futex syscall
    pKThr->FutexQueueNext    = (KThread_t *)0;
    pKThr->FutexVAddr        = NULL;
    pKThr->FutexValue        = 0;
    pKThr->FutexTimeout      = 0;
    pKThr->FutexIsShared     = 0;
    //pKThr->pad3 = 2; // TEMP PROBLEM ANALYSIS
    ppc_msync();

    Lock_AtomicRelease(isShared, my_turn);

    Sched_Unblock(pKThr, SCHED_STATE_FUTEX);

}

__END_DECLS


