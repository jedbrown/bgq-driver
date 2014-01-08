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

#ifndef _TOOL_H_ // Prevent multiple inclusion.
#define _TOOL_H_

#define TOOL_IPI_STATE_AVAIL 0
#define TOOL_IPI_STATE_PENDING 1
#define TOOL_IPI_STATE_BUSY  2

#define TOOL_IPI_MAX_COMMANDS 16


typedef struct ToolIpiRequest
{
    volatile uint16_t state;      // state of this data area: free, request pending, request complete.
    uint16_t numCommands;         // number of commands within this request
    struct {
        void*    cmd; // pointer to the command descriptor 
        void*    ack; // pointer to location to store the ack descriptor
    } descriptor[TOOL_IPI_MAX_COMMANDS];
} ToolIpiRequest;

typedef struct ToolIpiNotifyControl
{
    AppProcess_t *process;       // originating process
    uint64_t jobid;              // originating jobid 
    uint32_t rank;               // originating rank
    uint32_t toolID_1;           // target toolid (conflict) or conflicting toolid (available).
    uint32_t toolID_2;           // requesting toolid (conflict) or releasing toolid (available). 
    uint8_t  toolPriority;       // requesting tool priority (conflict) or releasing tool priority (available).
    uint8_t  notifyControlReason;// Notify control reason: Conflict or Available
    char toolTag[8];             // requesting tool tag (conflict) or releasing tool tag (available).
    uint8_t resumeAllThreads;    // is a resume of the threads requested (available).

} ToolIpiNotifyControl;

typedef struct ToolIpiAvailableNotify
{
    uint32_t conflictingToolID;
    uint32_t releasingToolID;
    uint8_t  releasingToolPriority;
    char     releasingAToolTag[8];
} ToolIpiAvailableNotify;


// Declaration
__C_LINKAGE int Tool_IsNotifyRequired(KThread_t *kthread);
__C_LINKAGE int Tool_SignalEncountered(KThread_t* kthread);
__C_LINKAGE int Tool_ProcessCommands(ToolIpiRequest* toolIpiRequest);
__C_LINKAGE int Tool_TrapHandler(Regs_t *context);
__C_LINKAGE int Tool_StepHandler(Regs_t *context);
__C_LINKAGE uint64_t Tool_MigrateWatchpoints(KThread_t* kthread, int new_hwthread, int isClone);
__C_LINKAGE int Tool_AppStart(KThread_t*kthread);
__C_LINKAGE int Tool_Suspend(AppProcess_t* proc);
__C_LINKAGE int Tool_Resume(AppProcess_t* proc);
__C_LINKAGE int Tool_Release(AppProcess_t* proc);
__C_LINKAGE int Tool_NotifyControl(ToolIpiNotifyControl *parms);
__C_LINKAGE int Tool_DeferredSignalNotification(KThread_t *kthread);
__C_LINKAGE void Tool_CleanupForSIGKILL(KThread_t *kthr);

#endif // Add nothing below this line.





