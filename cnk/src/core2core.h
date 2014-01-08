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
#ifndef _CORE2CORE_H_ // Prevent multiple inclusion
#define _CORE2CORE_H_

struct sPunit_Cfg;  // forward declare for IPI_upc_apply() opaque structure

// IPI Requestor functions
void IPI_setaffinity(KThread_t *pKThr);
void IPI_complete_migration(KThread_t *pKThr);
void IPI_run_scheduler(int hwthread_index, KThread_t *pKThr, int newstate);
void IPI_signal(int hwthread_index, AppProcess_t *proc, int tid, int sig);
void IPI_process_exit(int hwthread_index, int phase);
void IPI_agent_exit(int hwthread_index, int phase);
void IPI_guard_adjust(int hwthread_index, uint64_t high_mark, MoveGuardDirection_t direction);
void IPI_block_thread(int hwthread_index, KThread_t *kthr, int yield_reason);
void IPI_load_application(int hwthread_index);
void IPI_load_agent(int hwthread_index);
void IPI_update_MMU(int processorID, uint64_t slot);
void IPI_tool_cmd(int processorID, ToolIpiRequest* toolIpiRequest);
void IPI_upc_attach(unsigned coreID, struct sPunit_Cfg* pCfg);
void IPI_upcp_init(unsigned coreID, int mode, unsigned ctrMode);
void IPI_upcp_disable(unsigned coreID);
void ipi_cleanup();
void IPI_start_jitterbug(uint64_t seed);
void IPI_tool_suspend(int processorID, AppProcess_t *proc);
void IPI_tool_resume(int processorID, AppProcess_t *proc);
void IPI_tool_release(int processorID, AppProcess_t *proc);
void IPI_tool_notifysignal(KThread_t* kthread);
void IPI_tool_notifycontrol(int processorID, ToolIpiNotifyControl* controlParms);
void IPI_DeliverWakeup(int processorID);
int  IPI_AcknowledgeWakeup(uint64_t *mask1, uint64_t *mask2);
int IPI_AbortForProcessExit();
void IPI_invalidate_icache(int coreID);
void IPI_changepolicy(KThread_t *pKThr, int newPolicy, int newPriority);
void IPI_remote_thread_exit(int hwthread_index, AppProcess_t *proc);

// Obtain IPI control for syscall usage. Returns 0:success, (-1):busy.
int Syscall_GetIpiControl();
// Release IPI control for syscall usage. Returns 0:success, (-1):error.
int Syscall_ReleaseIpiControl();
// flush pending IPIs 
void IPI_DeadlockAvoidance(int processorID);


#ifndef IPIHANDLER
#define IPIHANDLER(name) void name(uint64_t parm1, uint64_t parm2)
#endif
#else
#ifndef IPIHANDLER
#define IPIHANDLER(name)
#endif
#endif


// IPI handler functions
IPIHANDLER(IPI_handler_setaffinity);
IPIHANDLER(IPI_handler_run_scheduler);
IPIHANDLER(IPI_handler_block_thread);
IPIHANDLER(IPI_handler_signal);
IPIHANDLER(IPI_handler_process_exit);
IPIHANDLER(IPI_handler_agent_exit);
IPIHANDLER(IPI_handler_guard_adjust);
IPIHANDLER(IPI_handler_load_application);
IPIHANDLER(IPI_handler_load_agent);
IPIHANDLER(IPI_handler_update_MMU);
IPIHANDLER(IPI_handler_tool_cmd);
IPIHANDLER(IPI_handler_tool_suspend);
IPIHANDLER(IPI_handler_tool_resume);
IPIHANDLER(IPI_handler_tool_release);
IPIHANDLER(IPI_handler_tool_notifysignal);
IPIHANDLER(IPI_handler_tool_notifycontrol);
IPIHANDLER(IPI_handler_jitterbug);
IPIHANDLER(IPI_handler_upc_attach);
IPIHANDLER(IPI_handler_upcp_init);
IPIHANDLER(IPI_handler_upcp_disable);
IPIHANDLER(IPI_handler_complete_migration);
IPIHANDLER(IPI_handler_invalidate_icache);
IPIHANDLER(IPI_handler_remote_thread_exit);
IPIHANDLER(IPI_handler_changepolicy);
#undef IPIHANDLER
