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
#include <hwi/include/bqc/testint_dcr.h>
#include <firmware/include/mailbox.h>

extern int dumpThreadState();
extern int wakeupThreads();

extern "C"
{
    int mbox_init();
    void mbox_poll(int dopoll);
};

char MailboxIn_Buffer[4096];
extern uint64_t TraceConfigDefault;

int mbox_init()
{
    uint64_t value;
    value = DCRReadPriv( TESTINT_DCR(TI_INTERRUPT_STATE_CONTROL_HIGH));
    TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__INT_MBOX_23_NE_insert(value, 3);
    DCRWritePriv( TESTINT_DCR(TI_INTERRUPT_STATE_CONTROL_HIGH), value);
    return 0;
}

void mbox_poll(int dopoll)
{
    int rc = 0;
    fw_uint32_t messageType;
    union
    {
        char* setptr;
        MailBoxPayload_ControlSystemRequest_t* sysreq;
        MailBoxPayload_Stdin_t*                stdin;
    };
    setptr = MailboxIn_Buffer;
    
    // Check for a mailbox message.
    rc = NodeState.FW_Interface.pollInbox(MailboxIn_Buffer, &messageType, sizeof(MailboxIn_Buffer));
    if (rc > 0)
    {
        Kernel_WriteFlightLog(FLIGHTLOG_high, FL_MBOXINPUT, messageType, rc, 0, 0);
        switch(messageType)
        {
            case JMB_CMD2CORE_CONTROL_SYSTEM_REQUEST:
                switch(sysreq->sysreq_id)
                {
                    case JMB_CTRLSYSREQ_SHUTDOWN:
                        Kernel_Halt();
                        break;
                    default:
                        printf("(E) mboxin: unsupported control system request id %u was ignored\n", sysreq->sysreq_id);
                        break;
                }
                break;
            case JMB_CMD2CORE_STDIN:
                if(strncmp((char*)&stdin->data[0], "cat /dev/bglog\n", stdin->count)==0)
                {
                    uint64_t threadmask[2] = { 0,0 };
                    threadmask[PhysicalProcessorID()/16] = _BN(PhysicalThreadIndex()%64);
                    DCRWritePriv( TESTINT_DCR(THREAD_ACTIVE0), threadmask[0]);
                    DCRWritePriv( TESTINT_DCR(THREAD_ACTIVE1), threadmask[1]);

                    uint64_t origtraceconfig = NodeState.TraceConfig;
                    NodeState.TraceConfig |= TRACE_FlightLog;
                    dumpFlightRecorder();
                    NodeState.TraceConfig = origtraceconfig;
                    
                    DCRWritePriv( TESTINT_DCR(THREAD_ACTIVE0), ~0ull);
                    DCRWritePriv( TESTINT_DCR(THREAD_ACTIVE1), 0xf000000000000000ull);
                }
                else if (strncmp((char*)&stdin->data[0], "cat /dev/bgthreads\n", stdin->count)==0)
                {
                    uint64_t threadmask[2] = { 0,0 };
                    threadmask[PhysicalProcessorID()/16] = _BN(PhysicalThreadIndex()%64);
                    DCRWritePriv( TESTINT_DCR(THREAD_ACTIVE0), threadmask[0]);
                    DCRWritePriv( TESTINT_DCR(THREAD_ACTIVE1), threadmask[1]);
                    uint64_t origtraceconfig = NodeState.TraceConfig;
                    NodeState.TraceConfig |= TRACE_FlightLog;
                    dumpThreadState();
                    NodeState.TraceConfig = origtraceconfig;
                    DCRWritePriv( TESTINT_DCR(THREAD_ACTIVE0), ~0ull);
                    DCRWritePriv( TESTINT_DCR(THREAD_ACTIVE1), 0xf000000000000000ull);
                }
                else if (strncmp((char*)&stdin->data[0], "cat /dev/wakeupthreads\n", stdin->count)==0)
                {
                    for (int hwt=0; hwt< CONFIG_MAX_APP_THREADS; hwt++)
                    {
                        // send IPI to the target hardware thread to re-run the scheduler
                        IPI_run_scheduler(hwt,0,0);
                    }
                }
                else if (strncmp((char*)&stdin->data[0], "settrace ", sizeof("settrace")-1)==0)
                {
                    TraceConfigDefault = NodeState.TraceConfig = strtoull_((char*)&stdin->data[sizeof("settrace ")-1], NULL, 16);
                    printf("trace level now set to %lx  size=%ld\n", NodeState.TraceConfig, sizeof("settrace ") - 1);
                    printf("string: %s\n", &stdin->data[sizeof("settrace ") - 1]);
                }
                else if (strncmp((char*)&stdin->data[0], "jobleader ", sizeof("jobleader")-1)==0)
                {
                    Personality_t *pers = GetPersonality();
                    uint64_t origtraceconfig = NodeState.TraceConfig;
                    NodeState.TraceConfig |= _BN(0); // flip on a bit in the trace word to enable printfs
                    printf("Job leader:(%d %d %d %d %d)  Bridge:(%d %d %d %d %d)\n", NodeState.JobLeaderCoords[0], NodeState.JobLeaderCoords[1],NodeState.JobLeaderCoords[2],NodeState.JobLeaderCoords[3],NodeState.JobLeaderCoords[4],
                           pers->Network_Config.cnBridge_A,pers->Network_Config.cnBridge_B,pers->Network_Config.cnBridge_C,pers->Network_Config.cnBridge_D,pers->Network_Config.cnBridge_E);
                    NodeState.TraceConfig = origtraceconfig;
                }
                else if(strncmp((char*)&stdin->data[0], "halt\n", 5)==0)
                {
                    Kernel_Halt();
                }
                else
                {
                    printf("(E) mboxin:  unsupported stdin command '%s'\n", (char*)&stdin->data[0]);
                }
                break;
            default:
                printf("(E) mboxin: unsupported inbox message %u was ignored\n", messageType);
                break;
        }
    }
    else
    {
        if(dopoll == 0)
        {
            Kernel_WriteFlightLog(FLIGHTLOG, FL_MBOXINERR, rc, 0, 0, 0);
        }
    }
}

void IntHandler_MailboxIn(int status_reg, int bitnum)
{
    mbox_poll(0);
    DCRWritePriv(TESTINT_DCR(TI_INTERRUPT_STATE__STATE), TESTINT_DCR__TI_INTERRUPT_STATE__INT_MBOX_23_NE_set(1));
}
