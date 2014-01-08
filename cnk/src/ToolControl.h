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
#include <cnk/include/Verbs.h>

#define siginfo_t kern_siginfo_t
#include <ramdisk/include/services/ToolctlMessages.h>

#define MAX_NUM_TOOLS 4

//#define TRAP_TABLE_ENTRIES 8  // testing value to cause hash collision and a full table
#define TRAP_TABLE_ENTRIES 16384

#define TOOL_DYNAMIC_APP_NOTIFY_DEFAULT 0

#define TOOL_BREAKPOINT_INSTRUCTION 0x7d821008
#define TOOL_FASTTRAP_INSTRUCTION(number) (uint64_t)( 0x0ddf8000 | (number & 0x00007fff) ) 
#define TOOL_FASTTRAP_NUMBER(instruction) (uint64_t)( 0xffff8000 & instruction ) 
#define TOOL_ISFASTTRAP(instruction) (((instruction & 0xffff8000) == 0x0ddf8000) ? 1 : 0) 
// The following #define will return true for any trap word, trap doubleword, trap immediate, trap doubleword immediate
#define TOOL_ISTRAP(inst) (((0xf8000000 & inst) == 0x08000000) || ((0xfc00077E & inst) == 0x7c000008))

const uint64_t TOOL_DBSR_DAC_MATCH = (DBSR_DAC1R | DBSR_DAC1W | DBSR_DAC2R | DBSR_DAC2W | DBSR_DAC3R | DBSR_DAC3W | DBSR_DAC4R | DBSR_DAC4W);
const uint64_t TOOL_DBSR_IAC_MATCH = (DBSR_IAC1 | DBSR_IAC2 | DBSR_IAC3 | DBSR_IAC4);

struct TrapTableEntry
{
    void*     instAddress;   // Virtual address of the modified instruction
    uint64_t  processMask;   // Mask indicating the hwthread id of the process leaders participating in this breakpoint
    uint32_t  originalValue; // Original instruction that existed in the text segment
    uint32_t  modifiedValue; // Modified instruction (i.e. the trap instruction)
};

struct ToolTableEntry
{
    //! Tool Identifier.
    uint32_t tool_id; 
    
    // Tool identifier string defined by the tool.
    char tag[bgcios::toolctl::ToolTagSize];      

    //! AppState object that this tool is associated with.
    AppState_t* app; 
    
    //! Mask of signals to be monitored.
    uint64_t sigmask; 

    //! Priority of this tool.
    //! 0 = query authority, 1 - 99 update priority with 99 being the most favored priority.
    uint32_t priority;
    
    //! Last thread id to notify tool. This thread id is used for subsequent operations if
    //! "pick thread id" was specified on the command interface 
    uint32_t default_thread_id;

    //! Signal to send when control authority is obtained.
    int sndsignal;

    //! Conflicting toolid
    uint32_t conflict;

    //! Status flags
    uint8_t fastBreakpointsEnabled;  // fast breakpoints are disabled:0 enabled:1
    uint8_t fastWatchpointsEnabled;  // fast watchpoints are disabled:0 enabled:1
    uint8_t dynamicAppNotifyStart;    // interrupt first instruction in loader:0 main:1

};

class BreakpointController
{
    public:
        //! Initialize
        void init();

        //! Return reference to a TrapTableEntry. If the address is not found in the table, the next available entry will be returned. 
        TrapTableEntry* findTrapTableEntry(void* address, int fromTrapHandler);

        //! Hash the breakpoint address into a trap table entry
        TrapTableEntry* hashBreakpointAddress(void *address);

        //! Remove a TrapTableEntry from the Trap table
        int removeTrapTableEntry(TrapTableEntry*);

        //! Return the next trap table entry in the table. This will wrap from the end to the beginning of the table.
        TrapTableEntry* nextTrapTableEntry(TrapTableEntry *entry);

        //! Update the controller with new trap information
        int update(uint64_t address, uint32_t prevInstruction, uint32_t newInstruction, AppProcess_t *proc);

        //! Handle breakpoint traps
        int trapHandler(Regs_t* context);
          
        //! Handle Instruction complete interrupt conditions
        int stepHandler(Regs_t* context);

        //! Setup for a step operation
        void prepareToStep(KThread_t *kthread);          

        //! Add breakpoint
        int add(void* address, uint32_t trapInstruction, AppProcess_t *proc);

        //! Remove breakpoint
        int remove(void* address, uint32_t originalInstruction, AppProcess_t *proc);

        //! Read 4 bytes of data and conditionally replace with previous data
        uint32_t read(uint32_t* address, AppProcess_t *proc);

        //! Set/get the trap-on/trap-after indicator.
        inline void setTrapAfterDAC() { trapAfterEnabled = 1;  }
        inline void setTrapOnDAC() { trapAfterEnabled = 0;  }
        inline int isTrapAfterDAC() { return(trapAfterEnabled);}
        inline int isTrapOnDAC() { return (!trapAfterEnabled); }

        //! Allocate the trap table. Initializes and locks the shared kernel area used to hold the trap table. 
        void trapTableAllocate();

        //! Trap table free. Release the locks on the shared kernel area which is used to hold the trap table.
        void trapTableFree(); 

        //! Get the kthread of a pending step operation.
        KThread_t* getPendingStepKThread(int processorID, AppProcess_t *proc);

        // ! Control access to the trap table.
        void TrapTableReadLock();
        void TrapTableReadUnlock();
        void TrapTableWriteLock();
        void TrapTableWriteUnlock();

    private:
        // Trap table is used to track traps that have been written into memory. Traps that have been written into
        // a memory segment that is not unique across the processes are put into the trap table indexed by the processorID
        // of the Application leader thread. Traps that have been written into a memory segment that is unique across
        // the processes are tracked in the trap table indexed by the processorID of the Process leader thread.
        struct {
            //! Lock to synchronize trap table accesses
            Lock_Atomic64_t lock; // control access to the breakpoint table.
            uint32_t numActiveEntries; // Number of active trap entries. These entries may be scattered throughout the table.
            int      trapTableAllocated; // The shared kernel area has been locked, initialized, and allocated for use by the tools interface.
            TrapTableEntry* entry;
        } trapTable;

        struct {
            uint64_t   virtualAltText;  // Virtual address of the alternate text work area for trap handling
            uint64_t   physicalAltText; // Physical address of the alternate text work area for trap handling
            uint64_t   trapAddress;     // virtual address of the trap instruction that we are stepping around
            struct {
                uint64_t stepAddress;
                uint64_t dbcr0;
                uint64_t dear;
                uint64_t dbsr;
                KThread_t *pendingStepKthread;
            } hwt[CONFIG_HWTHREADS_PER_CORE];
        } coreControl[CONFIG_MAX_CORES];

        // Physical work area for handling stepping around false traps. One 4k boundary aligned area defined for each core
        uint8_t trapWorkArea[ ( CONFIG_MAX_APP_CORES + 1 ) * 4096]; // Allocate 16+1 to allow for boundary alignment 

        uint8_t trapAfterEnabled;


};

struct WatchpointEntry {
    uint64_t refcount;      // Reference counts of the number of software threads using this watchpoint
    uint64_t address_base; // base address of the active watchpoint
    uint64_t address_mask; // mask applied to base address to define the watched range
};

struct WatchpointControl {
    WatchpointEntry dac_pair[2]; // The two sets of watchpoints available on a core (DAC1/2 and DAC3/4) 
};

class WatchpointManager
{
public:
    //! Initialize the watchpoint manager
    void init();

    //! Set a watchpoint at the specified address/length for the specified kthread.
    bgcios::toolctl::CmdReturnCode setWatchpoint(uint64_t addr, int length, bgcios::toolctl::WatchType type, KThread_t *kthread);

    //! Reset a watchpoint previously set at the specfied address.
    bgcios::toolctl::CmdReturnCode resetWatchpoint(uint64_t addr, KThread_t *kthread);

    //! Cleanup watchpoints in the current thread
    void cleanupThreadWatchpoints(KThread_t *kthread);

    //! Migrate watchpoints during the migration of a kthread from the current hwthread to a new hardware thread
    uint64_t migrateWatchpoints(KThread_t *kthread, int new_hwthread, int clone);


private:
    WatchpointControl watchpointControl[CONFIG_MAX_APP_CORES];
};

class ToolTable
{
    friend int Tool_IsNotifyRequired(KThread_t *kthread);
    friend int Tool_SignalEncountered(KThread_t *kthread);
    friend int Tool_AppStart(KThread_t* kthread);

    public:
        //! Object initialization. 
        int init(void);

        //! Add a tool. Requires that the validateAddTool was run without any errors dectected.
        void add(uint32_t tool_id, AppState_t *appstate, uint32_t priority, char *toolTag);

        //! Validate request to add a tool.
        uint32_t validateAdd(uint32_t tool_id, AppState_t *appstate, uint32_t priority);

        //! Remove a tool.
        int remove(uint32_t tool_id);

        //! Remove all tools.
        void removeAll();

        //! Return the AppProcess object associated with this toolid. NULL value signifies tool controls all processes.
        inline int numTools() { return numToolsAttached; }

        //! Return the toolid of the currently active updater tool. If none exists, zero is returned.
        uint32_t controllingToolId();

        //! Return the toolid for a given ToolTable index
        inline uint32_t toolId(int index) { return entry[index].tool_id; }

        //! Return 1 if the specified signal number is contained in the signal mask for the specfied toolid
        //! Return 0 if the specified signal number is not in the mask or the Tool id does not exist
        int signalMonitored(uint32_t tool_id, int signal);

        //! Set the default thread id
        void setDefaultThreadId(uint32_t toolid, uint32_t threadid);

        //! Set the default thread id
        uint32_t getDefaultThreadId(uint32_t toolid);

        //! Enable fast breakpoints
        void enableFastBreakpoints(uint32_t toolid);

        //! Disable fast breakpoints
        void disableFastBreakpoints(uint32_t toolid);

        //! Enable fast watchpoints
        void enableFastWatchpoints(uint32_t toolid);

        //! Disable fast watchpoints
        void disableFastWatchpoints(uint32_t toolid);

        //! Are Fast breakpoints enabled within this tool for this process
        int isFastBreakpointEnabled(uint32_t toolid);

        //! Are Fast watchpoints enabled within this tool for this process
        int isFastWatchpointEnabled(uint32_t toolid);

        //! Is the notify location for a dynamic app set to interrupt in ._start function in main
        int isDynamicAppNotifyStart(uint32_t toolid);

        //! Is the notify location for a dynamic app set to interrupt in .dl_start function ld.so
        int isDynamicAppNotifyDLoad(uint32_t toolid);

        //! Set indicator to cause a notify at start of main  
        void setDynamicAppNotifyStart(uint32_t toolid);

        //! Set indicator to cause a notify at start of ld.so 
        void setDynamicAppNotifyDLoader(uint32_t toolid);

        //! Grant control authority to a tool. Returns the toolid with control authority
        uint32_t grantControlAuthority(uint32_t toolid, uint64_t sigmask, int sndsignal);

        //! Grant control authority to a tool. Returns 0 for success
        int releaseControlAuthority(uint32_t toolid);

        //! Mask of signals to be monitored.
        uint64_t signalMask(uint32_t toolid); 

        //! Priority of this tool.
        uint32_t toolPriority(uint32_t toolid);

        //! Signal to send when control authority is obtained.
        int  controlAquiredSignal(uint32_t toolid);

        //! Tool tag
        char* toolTag(uint32_t toolid);

        //! Return the toolid of the highest priority conflicting tool and reset the condition
        uint32_t findConflictingTool();

        //! Set the conflict indicator to cause this tool to get a notify message when another tool releases
        void setConflict(uint32_t toolid);

        //! Is a tool attached.
        int isAttached(uint32_t toolid);

  private:
        uint32_t numToolsAttached;
        uint32_t toolWithControlAuthority; // Toolid of the tool the curently has control authority.
        ToolTableEntry entry[MAX_NUM_TOOLS];
};

enum ContinueProcessRC
{
    ContinueProcessRC_Success,       // Operation successful, suspended threads will be started.
    ContinueProcessRC_PendingNotify, // Suspended threads were not started due to additional pending Notification messages which are now armed to be sent.
    ContinueProcessRC_InvalidThread  // Operation has failed due to the target TID not being in the correct state.
};

enum ToolPendingAction {
    ToolPendingAction_Reset,
    ToolPendingAction_NotifySignal,
    ToolPendingAction_ResumeProc,
    ToolPendingAction_ResumeThd,
    ToolPendingAction_NotifyAvail
};

class ToolControl
{
    friend int      Tool_ProcessCommands(ToolIpiRequest*);
    friend int      Tool_IsNotifyRequired(KThread_t *);
    friend int      Tool_SignalEncountered(KThread_t*);
    friend int      Tool_TrapHandler(Regs_t *context);
    friend uint64_t Tool_MigrateWatchpoints(KThread_t *, int, int);
    friend int      Tool_AppStart(KThread_t*);
    friend int      Tool_Suspend(AppProcess_t*);
    friend int      Tool_Resume(AppProcess_t*);
    friend int      Tool_Release(AppProcess_t*);
    friend int      Tool_NotifyControl(ToolIpiNotifyControl*);
    friend int      Tool_DeferredSignalNotification(KThread_t *);

public:
        // Public functions

        //! Initialize the object
        int init(cnv_pd *protectionDomain, cnv_cq *completionQ);

        //! Terminate when node is shutdown.
        int term(void);

        //! Setup for a new job.
        int setupJob(void);

        //! Process a tool message
        int processMessage(uint32_t lkey);

        //! Return the attatched Tools. 
        int getAttachedTools(AppProcess_t* proc, int arraysize, uint32_t* toolIds);

        //! Return the queue pair number.
        uint32_t getQueuePairNum(void) const { return _queuePair.qp_num; }

        //! Detach all tools associated with the AppState 
        void detachAll(AppState_t *app);

        //! Inform appropriate attached tools of the exit process condition
        void exitProcess(AppProcess_t *proc);

        //! Return a reference to the breakpoint controller
        BreakpointController& breakpointController();

        //! Return a reference to the watchpoint manager
        WatchpointManager& watchpointManager();

        //! Cleanup watchpoints in the current thread
        void cleanupThreadWatchpoints(KThread_t *kthread);
    private:
        // Private functions

        //! Handle a SetupJobAck message.
        void setupJobAck(struct cnv_mr& region);

        //! Record the association of a tool to this node.
        void attach(struct cnv_mr& region);

        //! Remove the association of a tool to this node.
        void detach(struct cnv_mr& region);

        //! Request control authority.
        void control(struct cnv_mr& region);

        //! Execute a tools query message
        void query(struct cnv_mr& region);

        //! Execute a tools update message
        void update(struct cnv_mr& region);

        //! Handle a NotifyAck message.
        void notifyAck(struct cnv_mr& region);

        //! Spin until all of the submitted IPI operations for a given message are complete.
        int pollAllMessageIPIsComplete();

        //! Determine the AppProcess object associated with the targeted Rank.
        AppProcess_t* getProcessFromMessage();

        //! Allocate outbound storage area. 
        void * allocateOutbound(int size);

        //! Decrement the count of pending IPIs.
        void decrementPendingIPIs();

        //! Increment the count of pending IPIs.
        void incrementPendingIPIs();

        //! Send a command to the appropriate hardware thread for processing.
        uint16_t prepareCommand(bgcios::toolctl::CommandDescriptor& cmd_desc, bgcios::toolctl::CommandDescriptor& cmd_ack_desc);

        //! Post an outbound message.
        int postSend(cnv_mr& region, bool waitForAck = false);

        //! Post a receive using the inbound message region.
        int postRecv(cnv_mr& region);

        //! Find the tool id to be notified for a particular signal from a specific TID.
        int findToolToNotify(KThread_t *kthread, int signal);

        //! Notify a tool that an event originating within the identified thread id has occurred.
        int notify(int toolid, KThread_t *kthread, int signal);

        //! Puts all of the TIDs of a process into the Suspend state.  
        int suspendProcess(AppProcess_t *proc);

        //! Removes the SUSPEND state from all TIDs within an process. 
        int resumeProcess(AppProcess_t *proc);

        //! Puts all of the TIDs on the current hardware thread for the indicated process into the Suspend state.  
        int suspend(AppProcess_t *proc);

        //! Removes SUSPEND state all TIDs on the current hardware thread for the indicated process.  
        int resume(AppProcess_t *proc);

        //! Removes HOLD state all TIDs on the current hardware thread for the indicated process.  
        int release(AppProcess_t *proc);

        //! Removes HOLD from all TIDs within the entire process, across all hardware threads.
        int releaseProcess(AppProcess_t* proc);

        //! Call the appropriate IPI target handler to process a specific command.
        int processCommands(ToolIpiRequest&);

        //! Set default thread id. This will be used when no TID was specified on the command
        void setDefaultThreadId(int toolid, AppProcess_t *proc, uint32_t threadid);

        //! Get default thread id. This will be used when no TID was specified on the command
        uint32_t getDefaultThreadId(AppProcess_t *proc);

        //! Lock the breakpoint controller
        void lockWatchpointManager();

        //! Unlock the breakpoint controller
        void unlockWatchpointManager();

        //! Notify control authority is conflicting or available
        void notifyControl(uint32_t target_tool, bgcios::toolctl::NotifyControlReason reason, uint32_t reported_toolid, char* toolTag, uint8_t priority, uint64_t jobid, uint32_t rank);

        //! Is the ToolID in the current message attached to the Rank in the current message.
        int isToolAttached();

        //! Resume threads that have a step operation pending
        int resumePendingSteps(AppProcess_t* proc);

        //--------------------------------------------------------------------
        // Query Command functions
        //--------------------------------------------------------------------

        //! Get special registers.
        void getSpecialRegs(bgcios::toolctl::GetSpecialRegsCmd&, bgcios::toolctl::CommandDescriptor&);

        //! Get general purpose registers.
        void getGeneralRegs(bgcios::toolctl::GetGeneralRegsCmd&, bgcios::toolctl::CommandDescriptor&);

        //! Get floating point registers.
        void getFloatRegs(bgcios::toolctl::GetFloatRegsCmd&, bgcios::toolctl::CommandDescriptor&);

        //! Get debug registers.
        void getDebugRegs(bgcios::toolctl::GetDebugRegsCmd&, bgcios::toolctl::CommandDescriptor&);

        //! Get memory.
        void getMemory(bgcios::toolctl::GetMemoryCmd&, bgcios::toolctl::CommandDescriptor&);

        //! Get auxillary vectors.
        void getAuxVectors(bgcios::toolctl::GetAuxVectorsCmd&, bgcios::toolctl::CommandDescriptor&);

        //! Get process data.
        void getProcessData(bgcios::toolctl::GetProcessDataCmd&, bgcios::toolctl::CommandDescriptor& );

        //! Get thread data.
        void getThreadData(bgcios::toolctl::GetThreadDataCmd&, bgcios::toolctl::CommandDescriptor&);

        //! Get thread list.
        void getThreadList(bgcios::toolctl::GetThreadListCmd&, bgcios::toolctl::CommandDescriptor&);

        //! Get preferences
        void getPreferences(bgcios::toolctl::GetPreferencesCmd&,  bgcios::toolctl::CommandDescriptor&);

        //! Get list of persistent files
        void getFilenames(bgcios::toolctl::GetFilenamesCmd&,  bgcios::toolctl::CommandDescriptor&);
        
        //! Stat persistent file
        void getFileStatData(bgcios::toolctl::GetFileStatDataCmd&,  bgcios::toolctl::CommandDescriptor&);
        
        //! Read persistent file
        void getFileContents(bgcios::toolctl::GetFileContentsCmd&,  bgcios::toolctl::CommandDescriptor&);
        
        
        //--------------------------------------------------------------------
        // Update Command functions
        //--------------------------------------------------------------------

        //! Set special purpose register
        void setSpecialReg(bgcios::toolctl::SetSpecialRegCmd&, bgcios::toolctl::CommandDescriptor&);

        //! Set general purpose register.
        void setGeneralReg(bgcios::toolctl::SetGeneralRegCmd&, bgcios::toolctl::CommandDescriptor&);

        //! Set floating point register.
        void setFloatReg(bgcios::toolctl::SetFloatRegCmd&, bgcios::toolctl::CommandDescriptor&);

        //! Set debug register.
        void setDebugReg(bgcios::toolctl::SetDebugRegCmd&, bgcios::toolctl::CommandDescriptor&);

        //! Set general purpose register.
        void setGeneralRegs(bgcios::toolctl::SetGeneralRegsCmd&, bgcios::toolctl::CommandDescriptor&);

        //! Set floating point registers.
        void setFloatRegs(bgcios::toolctl::SetFloatRegsCmd&, bgcios::toolctl::CommandDescriptor&);

        //! Set debug registers.
        void setDebugRegs(bgcios::toolctl::SetDebugRegsCmd&, bgcios::toolctl::CommandDescriptor&);

        //! Set special registers.
        void setSpecialRegs(bgcios::toolctl::SetSpecialRegsCmd&, bgcios::toolctl::CommandDescriptor&);

        //! Set memory.
        void setMemory(bgcios::toolctl::SetMemoryCmd&, bgcios::toolctl::CommandDescriptor&);

        //! Install signal handler for SIGTRAP.
        void installTrapHandler(bgcios::toolctl::InstallTrapHandlerCmd&, bgcios::toolctl::CommandDescriptor&);    

                //! Install signal handler for SIGTRAP.
        void removeTrapHandler(bgcios::toolctl::RemoveTrapHandlerCmd&, bgcios::toolctl::CommandDescriptor&);    

        //! MMAP anonymous memory.
        void allocateMemory(bgcios::toolctl::AllocateMemoryCmd&, bgcios::toolctl::CommandDescriptor&);          

        //! MMAP anonymous memory.
        void freeMemory(bgcios::toolctl::FreeMemoryCmd&, bgcios::toolctl::CommandDescriptor&);          

        //! Send a signal to a process
        void sendSignal(bgcios::toolctl::SendSignalCmd&, bgcios::toolctl::CommandDescriptor&); 

        //! Set trap breakpoint
        void setBreakpoint(bgcios::toolctl::SetBreakpointCmd&,  bgcios::toolctl::CommandDescriptor&);
        
        //! Reset trap breakpoint
        void resetBreakpoint(bgcios::toolctl::ResetBreakpointCmd&,  bgcios::toolctl::CommandDescriptor&);

        //! Set hardware watchpoint
        void setWatchpoint(bgcios::toolctl::SetWatchpointCmd&,  bgcios::toolctl::CommandDescriptor&);

        //! Reset hardware watchpoint
        void resetWatchpoint(bgcios::toolctl::ResetWatchpointCmd&,  bgcios::toolctl::CommandDescriptor&);

        //! Set preferences
        void setPreferences(bgcios::toolctl::SetPreferencesCmd&,  bgcios::toolctl::CommandDescriptor&);

        //! Set continuation signal
        void setContinuationSignal(bgcios::toolctl::SetContinuationSignalCmd&, bgcios::toolctl::CommandDescriptor&);

        //! Send a signal to a process
        void sendSignalToProcess(bgcios::toolctl::SendSignalToProcessCmd&, bgcios::toolctl::CommandDescriptor&); 

        //--------------------------------------------------------------------
        // Action Commands
        //--------------------------------------------------------------------

        //! Put a thread into the Hold state, suspended in the scheduler.
        void holdThread(bgcios::toolctl::HoldThreadCmd&, bgcios::toolctl::CommandDescriptor&);

        //! Release a thread from the Hold state.
        void releaseThread(bgcios::toolctl::ReleaseThreadCmd&, bgcios::toolctl::CommandDescriptor&);

        //! Release all threads within the process from the Hold state.
        void releaseAllThreads(bgcios::toolctl::ReleaseAllThreadsCmd&, bgcios::toolctl::CommandDescriptor&);

        //! Continue a thread from a suspended signal delivery using the supplied signal and release holds on all other threads in the process.
        void continueProcess(bgcios::toolctl::ContinueProcessCmd&, bgcios::toolctl::CommandDescriptor&);      
        ContinueProcessRC continueProcess(KThread_t *);

        //! Step a thread 
        void stepThread(bgcios::toolctl::StepThreadCmd&, bgcios::toolctl::CommandDescriptor&);

        //! Release a tool from control authority.
        void releaseControl(bgcios::toolctl::ReleaseControlCmd&, bgcios::toolctl::CommandDescriptor&);      

        //--------------------------------------------------------------------
        // Special error handling command functions
        //--------------------------------------------------------------------

        //! Handle an invalid Thread Identifier specification.
        void invalidTID(bgcios::toolctl::ToolCommand&, bgcios::toolctl::CommandDescriptor&); 

        //! Handle an invalid command specification.
        void invalidCmd(bgcios::toolctl::ToolCommand& cmd, bgcios::toolctl::CommandDescriptor& ackdesc);

        //! Handle a previous command failure preventing an action command (ContinueProcess, ReleaseControl, StepThread) from executing.
        void previousCmdFailure(bgcios::toolctl::ToolCommand& cmd, bgcios::toolctl::CommandDescriptor& ackdesc);

        //! Handle a process starting its exit flow before the command begins processing.
        void processExitingCmdFailure(bgcios::toolctl::ToolCommand& cmd, bgcios::toolctl::CommandDescriptor& ackdesc);

        //----------------------------------------------
        // Private Data
        //----------------------------------------------
        //! Outbound data buffer pointer. 
        volatile uint64_t outbound_cmd_data;

        //! Pointer to the currrently active message.
        bgcios::toolctl::ToolMessage *currentMsg;

        //! Message result. Information to be provided in the acknowledgment message.
        int messageReturnCode;

        //! Message error data. Information to be provided in the acknowledgment message.
        int messageErrorData;

        //! Table of attached tools indexed by the Application ProcessLeader's processorID (0-63).
        ToolTable processEntry[CONFIG_MAX_PROCESSES];

        //! The kthread address that are in the process of Notification for each hardware thread
        KThread_t *pendingNotifyKThread[CONFIG_MAX_APP_THREADS];

        //! Communication area for sending IPIs to hardware threads
        ToolIpiRequest ipiRequest[CONFIG_MAX_APP_THREADS];

        //! Sequence id for tracking message exchanges.
        uint32_t _sequenceId;

        //! Device context.
        cnv_context *_context;

        //! Queue pair connected to jobctld.
        cnv_qp _queuePair;

        //! First memory region for inbound messages.
        struct cnv_mr _inMessageRegion1;

        //! Second memory region for inbound messages.
        struct cnv_mr _inMessageRegion2;

        //! Memory region for outbound acknowledgment messages.
        struct cnv_mr _outMessageRegion;

        //! Memory region for outbound Notify messages.
        struct cnv_mr _outNotifyMessageRegion;

        //! Storage for outbound acknowledgment messages.
        char _outMessage[bgcios::SmallMessageRegionSize];

        //! Storage for outbound Notify messages.
        char _outNotifyMessage[bgcios::SmallMessageRegionSize];

        //! Storage for inbound messages.
        char _inMessage1[bgcios::SmallMessageRegionSize];

        //! Storage for inbound messages.
        char _inMessage2[bgcios::SmallMessageRegionSize];

        //! Breakpoint manager contains active modifications made by processes within this node
        BreakpointController brkptController;

        //! Watchpoint manager
        WatchpointManager watchpointmanager;

        //! Pending notify available data
        ToolIpiNotifyControl pendingNotifyControl;

        //! Atomic counter to control number of outstanding IPIs
        Lock_Atomic_t pendingIPIs;

        //! Lock to synchronize use of the Notify outNotifyMessageRegion.
        Lock_Atomic_t _notifyLock;

        //! Lock to synchronize use of the queue pair.
        Lock_Atomic_t _qpLock;

        //! Lock to synchronize tool attaches, detaches, and searches
        Lock_Atomic_t _toolTableLock;

        //! Atomic counter to wait for an ack message.
        Lock_Atomic_t _ackReceived;

        //! Lock to synchronize access to the watchpoint manager
        Lock_Atomic_t _watchpointManagerLock;

};

extern class ToolControl toolControl;






