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
/* (C) Copyright IBM Corp.  2011, 2011                              */
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
#include "proxy.h"
#include "toolintf.h"
#include "network.h"

using namespace bgcios::toolctl;

LOG_DECLARE_FILE("cios.tests");

uint32_t
initCommand(struct CommandDescriptor *desc, uint16_t type, uint32_t length, uint32_t offset)
{
    desc->type = type;
    desc->reserved = 0;
    desc->length = length;
    desc->offset = offset;
    desc->returnCode = 0;
    return (offset + length);
}

static int sequenceCount = 0;

void touchProcess(JobPtr job)
{
    // Build Attach message (the same one is sent to every compute node).
    // Build a Query message to get info about the process.
    int x;
    union
    {
        QueryMessage outMsg;
        char pad[32768];
    };
    memset(&outMsg.header, 0x00, sizeof(outMsg));
    outMsg.header.service = bgcios::ToolctlService;
    outMsg.header.version = ProtocolVersion;
    outMsg.header.type = Query;
    outMsg.header.length = sizeof(outMsg);
    outMsg.header.sequenceId = ++sequenceCount;
    outMsg.numCommands = 1;
    outMsg.header.jobId = job->getJobId();
    outMsg.toolId = job->getToolId();
    uint32_t nextOffset = sizeof(QueryMessage);
    
    // Add get thread list command to the message.
    nextOffset = initCommand(&(outMsg.cmdList[0]), GetThreadList, sizeof(GetThreadListCmd), nextOffset);
    outMsg.header.length += (uint32_t)sizeof(GetThreadListCmd);
    GetThreadListCmd *threadList = (GetThreadListCmd *)((char *)&outMsg + outMsg.cmdList[0].offset);
    threadList->threadID = 0;
    
    // Attach to all of the compute node processes.
    for (Job::channel_list_iterator iter = job->toolChannels.begin(); iter != job->toolChannels.end(); ++iter) 
    {   
        AttachAckMessage* ptr = &attachMap[(*iter).second];
        for(x=0; x<ptr->numProcess; x++)
        {
            outMsg.header.rank = ptr->rank[x];
            CoreFilePtr coreFile = job->coreFiles.get(outMsg.header.rank);
            if(coreFile->validProcessInfo)
                continue;
            
            sendMessage(outMsg, iter->second);
        }
    }
    pollForData(job);
}

void touchThreads(JobPtr job)
{
    // Build a Query message to get info about the thread.
    int x;
    int tindex;
    union
    {
        QueryMessage outMsg;
        char pad[32768];
    };
    memset(&outMsg.header, 0x00, sizeof(outMsg));
    outMsg.header.service = bgcios::ToolctlService;
    outMsg.header.version = ProtocolVersion;
    outMsg.header.type = Query;
    outMsg.header.rank = 0;
    outMsg.header.length = sizeof(outMsg);
    outMsg.header.sequenceId = ++sequenceCount;
    outMsg.header.jobId = job->getJobId();
    outMsg.toolId = job->getToolId();
    outMsg.numCommands = 0;
    uint32_t nextOffset = sizeof(QueryMessage);
    
    // Attach to all of the compute node processes.
    for (Job::channel_list_iterator iter = job->toolChannels.begin(); iter != job->toolChannels.end(); ++iter) 
    {
        AttachAckMessage* ptr = &attachMap[(*iter).second];
        for(x=0; x<ptr->numProcess; x++)
        {
            outMsg.header.rank = ptr->rank[x];
            CoreFilePtr coreFile = job->coreFiles.get(outMsg.header.rank);
            if(coreFile->validThreadInfo)
                continue;
            
            for(tindex=0; tindex<coreFile->getNumThreads(); tindex++)
            {
                // Add get thread data command to the message.
                nextOffset = initCommand(&(outMsg.cmdList[outMsg.numCommands]), GetThreadData, sizeof(GetThreadDataCmd), nextOffset);
                outMsg.header.length += (uint32_t)sizeof(GetThreadDataCmd);
                GetThreadDataCmd *threadData = (GetThreadDataCmd *)((char *)&outMsg + outMsg.cmdList[outMsg.numCommands].offset);
                threadData->threadID = coreFile->getThreadIdByIndex(tindex);
                outMsg.numCommands++;

                // Add get special purpose regs
                nextOffset = initCommand(&(outMsg.cmdList[outMsg.numCommands]), GetSpecialRegs, sizeof(GetSpecialRegsCmd), nextOffset);
                outMsg.header.length += (uint32_t)sizeof(GetSpecialRegsCmd);
                GetSpecialRegsCmd *specialRegs = (GetSpecialRegsCmd *)((char *)&outMsg + outMsg.cmdList[outMsg.numCommands].offset);
                specialRegs->threadID = coreFile->getThreadIdByIndex(tindex);
                outMsg.numCommands++;
                
                // Add get general registers command to the message.
                nextOffset = initCommand(&(outMsg.cmdList[outMsg.numCommands]), GetGeneralRegs, sizeof(GetGeneralRegsCmd), nextOffset);
                outMsg.header.length += (uint32_t)sizeof(GetGeneralRegsCmd);
                GetGeneralRegsCmd *generalRegs = (GetGeneralRegsCmd *)((char *)&outMsg + outMsg.cmdList[outMsg.numCommands].offset);
                generalRegs->threadID = coreFile->getThreadIdByIndex(tindex);
                outMsg.numCommands++;
                
                if(outMsg.numCommands >= 16-3)
                {
                    sendMessage(outMsg, iter->second);
                    outMsg.numCommands = 0;
                    nextOffset = sizeof(QueryMessage);
                }
            }
            if(outMsg.numCommands > 0)
            {
                sendMessage(outMsg, iter->second);
                outMsg.numCommands = 0;
                nextOffset = sizeof(QueryMessage);
            }
        }
    }
    pollForData(job);
}

int addStatus(CxxSockets::Message& msg, int status)
{
    msg << "RC=" << status << endl;
    return 0;
}

int addIar(CxxSockets::Message& msg, JobPtr job)
{
    int x;
    int thread;
    uint64_t rank;
    for (Job::channel_list_iterator iter = job->toolChannels.begin(); iter != job->toolChannels.end(); ++iter)
    {
        AttachAckMessage* ptr = &attachMap[(*iter).second];
        for(x=0; x<ptr->numProcess; x++)
        {
            rank = ptr->rank[x];
            CoreFilePtr coreFile = job->coreFiles.get(attachMap[(*iter).second].rank[x]);
            
            for(thread=0; thread < coreFile->getNumThreads(); thread++)
            {
                CoreFileThreadPtr corethread = coreFile->getThreadById(coreFile->getThreadIdByIndex(thread));
                msg << "rank" << dec << rank << "." << thread << " " << hex << "0x" << corethread->getIAR() << endl;
            }
        }
    }
}

int addStacks(CxxSockets::Message& msg, JobPtr job)
{
    int x;
    int thread;
    int depth;
    uint64_t rank;
    for (Job::channel_list_iterator iter = job->toolChannels.begin(); iter != job->toolChannels.end(); ++iter)
    {
        AttachAckMessage* ptr = &attachMap[(*iter).second];
        for(x=0; x<ptr->numProcess; x++)
        {
            rank = ptr->rank[x];
            CoreFilePtr coreFile = job->coreFiles.get(attachMap[(*iter).second].rank[x]);
            
            for(thread=0; thread < coreFile->getNumThreads(); thread++)
            {
                msg << "rank" << dec << rank << "." << thread << " ";
                
                CoreFileThreadPtr corethread = coreFile->getThreadById(coreFile->getThreadIdByIndex(thread));
                for(depth=0; depth < corethread->getNumStackFrames(); depth++)
                {
                    msg << hex << "0x" << (corethread->getStackInfo())[depth].savedLR << " ";
                }
                msg << endl;
            }
        }
    }
}

int addGprs(CxxSockets::Message& msg, JobPtr job)
{
    int x;
    int thread;
    int gpr;
    uint64_t rank;
    for (Job::channel_list_iterator iter = job->toolChannels.begin(); iter != job->toolChannels.end(); ++iter)
    {
        AttachAckMessage* ptr = &attachMap[(*iter).second];
        for(x=0; x<ptr->numProcess; x++)
        {
            rank = ptr->rank[x];
            CoreFilePtr coreFile = job->coreFiles.get(attachMap[(*iter).second].rank[x]);
            
            for(thread=0; thread < coreFile->getNumThreads(); thread++)
            {
                CoreFileThreadPtr corethread = coreFile->getThreadById(coreFile->getThreadIdByIndex(thread));
                const GetGeneralRegsAckCmd* gprs = corethread->getGeneralRegs();
                msg << "rank" << dec << rank << "." << thread;
                for(gpr=0; gpr<32; gpr++)
                {
                    msg << hex << gprs->gpr[gpr] << " ";
                }
                msg << endl;
            }
        }
    }
}

void clearProcess(JobPtr job)
{
    int x;
    // Attach to all of the compute node processes.
    for (Job::channel_list_iterator iter = job->toolChannels.begin(); iter != job->toolChannels.end(); ++iter) 
    {
        AttachAckMessage* ptr =&attachMap[(*iter).second];
        for(x=0; x<ptr->numProcess; x++)
        {
            CoreFilePtr coreFile = job->coreFiles.get(ptr->rank[x]);
            coreFile->validThreadInfo  = false;
            coreFile->validProcessInfo = false;
        }
    }
}

bool doCommand(JobPtr job)
{
    int rc = 0;
    bool terminate = false;
    CxxSockets::Message side_msg;
    CxxSockets::Message response;
    
    try
    {
        printf("receiving command\n");
        SocketVector[0]->Receive(side_msg);
        printf("gotmessage\n");
        
#define CMD(name) else if(side_msg.str() == name) 
        if(0) {}
        
        CMD("iar")
        {
            touchProcess(job);
            touchThreads(job);
            addStatus(response, rc);
            if(rc==0) 
                addIar(response, job);
        }
        CMD("stacks")
        {
            touchProcess(job);
            touchThreads(job);
            addStatus(response, rc);
            if(rc==0) 
                addStacks(response, job);
        }
        CMD("gprs")
        {
            touchProcess(job);
            touchThreads(job);
            addStatus(response, rc);
            if(rc==0) 
                addGprs(response, job);
        }
        CMD("halt")
        {
//          sendHalt();
            clearProcess(job);
            if(rc==0) 
                addStatus(response, rc);
        }
        CMD("run")
        {
//          sendRun();
            clearProcess(job);
            if(rc==0) 
                addStatus(response, rc);
        }
        CMD("terminate")
        {
            terminate = true;
            addStatus(response, 0);
        }
        else
        {
            cout << "Invalid command!!!  \"" << side_msg.str() << "\"" << endl;
            addStatus(response, -1);
        }
        SocketVector[0]->Send(response);
    }
    catch(CxxSockets::Error& e) // CxxSockets::SockCloseUnexpected & e)
    {
        LOG_ERROR_MSG("Error connecting to tool data channel socket:  " << e.what());
        terminate = true; // an exception occurred, abort
    }
    return terminate;
}


int main (int argc, char *argv[])
{
    // Initialize logging for the test.
    bgq::utility::Properties::Ptr properties;
    char *propertiesFile = getenv("BG_PROPERTIES");
    if (propertiesFile != NULL) 
        properties = bgq::utility::Properties::create(propertiesFile);
    else 
        properties = bgq::utility::Properties::create();
    bgq::utility::initializeLogging(*properties);
    
    // Ignore SIGTERM - if we don't go away nicely the control daemon sends a SIGKILL.
    signal(SIGTERM, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    // Get info about the job being monitored.
    uint64_t jobId  = boost::lexical_cast<uint64_t>(getenv("BG_JOBID"));
    uint32_t toolId = boost::lexical_cast<uint32_t>(getenv("BG_TOOLID"));
    
    // Create a Job object and connect to tool control daemons.
    JobPtr job = JobPtr(new Job(jobId, toolId));
    
    attach(job);
    pollForData(job);
    printf("connecting\n");
    if(connectToTool(argc, argv) == 0)
    {
        while(doCommand(job) == false)
        {
        }
    }
    detach(job);
    exit(0);
}
