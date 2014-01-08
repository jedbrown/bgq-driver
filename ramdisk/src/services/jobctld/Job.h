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
/* (C) Copyright IBM Corp.  2011, 2012                              */
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

//! \file  jobctld/Job.h
//! \brief Declaration and inline methods for bgcios::jobctl::Job class.

#ifndef JOBCTL_JOB_H
#define JOBCTL_JOB_H

// Includes
#include "ToolProcess.h"
#include "SimJobMonitor.h"
#include "ComputeNode.h"
#include "JobctlAccumulators.h"
#include "PrologProgramMonitor.h"
#include "EpilogProgramMonitor.h"
#include <ramdisk/include/services/common/PointerMap.h>
#include <ramdisk/include/services/common/UserIdentity.h>
#include <ramdisk/include/services/common/MessageResult.h>
#include <ramdisk/include/services/JobctlMessages.h>
#include <bgq_util/include/pthreadmutex.h>
#include <vector>
#include <map>
#include <tr1/memory>
#include <sstream>

namespace bgcios
{

namespace jobctl
{

//! \brief Track a job submitted to a compute block.

class Job
{
public:

   //! \brief  Default constructor.
   //! \param  jobId Job identifier.
   //! \param  blockId Block identifier.

   Job(uint64_t jobId, uint32_t blockId)
   {
      _jobId = jobId;
      _blockId = blockId;
      _exitStatus = 0;
   }

   //! \brief  Constructor.
   //! \param  jobId Job identifier.
   //! \param  blockId Block identifier.
   //! \param  corner Torus coordinates of corner compute node.
   //! \param  shape Size of each torus dimension.
   //! \param  jobLeader Torus coordinates of job leader compute node.
   //! \param  numRanks Number of ranks participating in job.

   Job(uint64_t jobId, uint32_t blockId, NodeCoordinates& corner, NodeCoordinates& shape, NodeCoordinates& jobLeader, uint32_t numRanks)
   {
      _jobId = jobId;
      _blockId = blockId;
      _exitStatus = 0;
      _corner = corner;
      _shape = shape;
      _jobLeader = jobLeader;
      _numRanks = numRanks;
   }

   //! \brief  Default destructor.

   ~Job();

   //! \brief  Get the job identifier.
   //! \return Job id value.

   uint64_t getJobId(void) const { return _jobId; }

   //! \brief  Get the block identifier.
   //! \return Block id value.

   uint32_t getBlockId(void) const { return _blockId; }

   //! \brief  Get the user identity.
   //! \return User identity object.

   UserIdentity& getUserIdentity(void) { return _identity; }

   //! \brief  Set the exit status for the job.
   //! \param  status New value for exit status.
   //! \return Nothing.

   void setExitStatus(int status) { _exitStatus = status; }

   //! \brief  Get the exit status for the job.
   //! \return Exit status value.

   int getExitStatus(void) const { return _exitStatus; }

   //! \brief  Setup on the I/O node before running the job.
   //! \param  msg Input LoadJob message for the job.
   //! \return Result of operation.

   bgcios::MessageResult setup(LoadJobMessage *msg);

   //! \brief  Cleanup on the I/O node after running the job.
   //! \return 0 when successful, errno when unsuccessful.

   int cleanup(void);

   //! \brief  Start a tool for the job and add it to the list of active tools.
   //! \param  msg Input StartTool message for the job.
   //! \param  simulation flag indication if we are in simulation mode
   //! \return Result of operation.

   bgcios::MessageResult startTool(StartToolMessage *msg, bool simulation = false);

   //! \brief  Get the pointer to the tool process object for the specified tool identifier.
   //! \param  toolId Tool identifier.
   //! \return Pointer to ToolProcess object.

   ToolProcessPtr getTool(uint32_t toolId) { return _tools.get(toolId); }

   //! \brief  Check if the specified tool is currently running.
   //! \param  toolId Tool identifier.
   //! \return True if tool is running, otherwise false.

   bool isToolRunning(uint32_t toolId);

   //! \brief  End a tool for the job.
   //! \param  toolId Tool identifier.
   //! \param  signo Signal number to send to end tool.
   //! \return Result of operation.

   bgcios::MessageResult endTool(uint32_t toolId, int signo);

   //! \brief  Find a tool that has ended.
   //! \return Tool identifier of a tool that ended or 0 if no tool has ended.

   uint32_t findEndedTool(void);

   //! \brief  Wait for a tool to end.
   //! \param  toolId Tool identifier.
   //! \return Exit status of tool when successful or -1 when unsuccessful.

   int waitForTool(uint32_t toolId);

   //! \brief  Remove a tool from the list of active tools.
   //! \param  toolId Tool identifier.
   //! \return Result of operation.

   bgcios::MessageResult removeTool(uint32_t toolId);

   //! \brief  Get the number of active tools.
   //! \return Number of active tools.

   uint32_t numTools(void) const { return (uint32_t)_tools.size(); }

   //! \brief  End all tools with the specified signal.
   //! \param  signo Signal number to send to end tool.
   //! \return Nothing.

   void endAllTools(int signo);

   //! \brief  Check status of all tools
   //! \param  seconds Previous number of seconds to check for tool activity.
   //! \return Number of tools that are active.

   uint32_t checkAllToolsStatus(uint32_t seconds);

   //! \brief  Run the prolog program for the job.
   //! \param  prologProgramPath Path to prolog program executable.
   //! \param  timeout Number of seconds to wait for prolog program to complete.
   //! \return 0 when successful, errno when unsuccessful.

   int runPrologProgram(std::string prologProgramPath, int32_t timeout);

   //! \brief  Run the epilog program for the job.
   //! \param  epilogProgramPath Path to epilog program executable.
   //! \param  timeout Number of seconds to wait for epilog program to complete.
   //! \return 0 when successful, errno when unsuccessful.

   int runEpilogProgram(std::string epilogProgramPath, int32_t timeout);

   //! \brief  Add a compute node object to list of compute nodes for the job.
   //! \param  cnode Pointer to ComputeNode object.
   //! \return Nothing.

   void addComputeNode(ComputeNodePtr cnode);

   //! \brief  Get the number of compute nodes in the job.
   //! \return Number of compute nodes.

   size_t numComputeNodes(void) const { return _cnodes.size(); }

   //! \brief  Return indicator if specified compute node is contained in job.
   //! \param  cnode Pointer to compute node object.
   //! \return True if compute node is contained in job, otherwise false.

   bool isComputeNodeInJob(const ComputeNodePtr& cnode) const;

   //! \brief  Send a message to job leader compute node for the job.
   //! \param  msghdr Pointer to message header.
   //! return  Nothing.

   void sendMessageToJobLeaderNode(bgcios::MessageHeader *msghdr);

   //! \brief  Send a message to all compute nodes in the job.
   //! \param  msghdr Pointer to message header.
   //! return  Nothing.

   void sendMessageToAllNodes(bgcios::MessageHeader *msghdr);

   //! Accumulate SetupJob messages.
   bgcios::MessageAccumulator setupJobAckAccumulator;

   //! Accumulate StartJob messages.
   bgcios::MessageAccumulator startJobAckAccumulator;

   //! Accumulate CleanupJob messages.
   CleanupJobAckAccumulator cleanupJobAckAccumulator;

   //! Accumulate LoadJobAck messages.
   bgcios::MessageAccumulator loadJobAckAccumulator;

   //! Accumulate ExitProcess messages.
   ExitProcessAccumulator exitProcessAccumulator;

   //! Accumulate ExitJob messages.
   ExitJobAccumulator exitJobAccumulator;

   //! Accumulate SignalJobAck messages.
   bgcios::MessageAccumulator signalJobAckAccumulator;


   //! \brief  Set the pointer to the monitor object for the job.
   //! \param  monitor Pointer to SimJobMonitor object.
   //! \return Nothing.

   void setSimJobMonitor(SimJobMonitorPtr monitor) { _monitor = monitor; }

   //! \brief  Get the pointer to the monitor object for the job.
   //! \return Pointer to SimJobMonitor object.

   SimJobMonitorPtr& getSimJobMonitor(void) { return _monitor; }

   //! \brief  Write info about the job to the specified stream.
   //! \param  os Output stream to write to.
   //! \return Output stream.

   std::ostream& writeTo(std::ostream& os) const;

private:

   //! \brief  Create a directory.
   //! \param  directory Path to directory.
   //! \return Result of operation.

   bgcios::MessageResult createDirectory(std::string directory);

   //! Job identifier.
   uint64_t _jobId;

   //! Block identifier.
   uint32_t _blockId;

   //! Exit status of job.
   int _exitStatus;

   //! User identity for the job.
   bgcios::UserIdentity _identity;

   //! Map of active tools for the job.
   bgcios::PointerMap <uint32_t, ToolProcessPtr> _tools;

   // Typedef for tool list iterator.
   typedef bgcios::PointerMap <uint32_t, ToolProcessPtr>::const_iterator tool_list_iterator;

   //! Pointer to prolog program monitor object.
   PrologProgramMonitorPtr _prologProgramMonitor;

   //! Pointer to epilog program monitor object.
   EpilogProgramMonitorPtr _epilogProgramMonitor;

   //! Pointer to monitor object when running under simulation.
   SimJobMonitorPtr _monitor;

   //! Torus coordinates of corner compute node for job.
   NodeCoordinates _corner;

   //! Size of each torus dimension for job.
   NodeCoordinates _shape;

   //! Torus coordinates of job leader compute node.
   NodeCoordinates _jobLeader;

   //! Client connection object of job leader compute node.
   RdmaClientPtr _jobLeaderClient;

   //! Number of ranks participating in job.
   uint32_t _numRanks;

   //! List of compute nodes participating in job.
   std::vector <ComputeNodePtr> _cnodes;

   //! Typedef for compute node list iterator.
   typedef std::vector <ComputeNodePtr>::const_iterator cnode_list_iterator;

};

//! Smart pointer for Job class.
typedef std::tr1::shared_ptr<Job> JobPtr;

//! \brief  Job object shift operator for output.
//! \param  os Output stream to write to.
//! \param  job Job object.
//! \return Output stream.

inline std::ostream& operator<<(std::ostream& os, const Job& job)
{
   return job.writeTo(os);
}

} // namespace jobctl

} // namespace bgcios

#endif // JOBCTL_JOB_H
