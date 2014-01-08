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

//! \file  Job.cc
//! \brief Methods for bgcios::jobctl::Job class.

// Includes
#include "Job.h"
#include <ramdisk/include/services/common/SymbolicLink.h>
#include <ramdisk/include/services/common/TextFile.h>
#include <ramdisk/include/services/MessageUtility.h>
#include <ramdisk/include/services/common/logging.h>
#include <ramdisk/include/services/common/RdmaError.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <ramdisk/include/services/common/Cioslog.h>

#define LLUS long long unsigned int

using namespace bgcios::jobctl;

LOG_DECLARE_FILE("cios.jobctld");


Job::~Job()
{
   LOG_CIOS_TRACE_MSG("Job " << _jobId << ": destroying Job object");
   _monitor.reset();
}

bgcios::MessageResult
Job::setup(LoadJobMessage *msg)
{
   // Save the user identity.
   _identity.set(msg->userId, msg->groupId, msg->numSecondaryGroups, msg->secondaryGroups);

   // Create the jobs directory for this job.
   bgcios::MessageResult result;
   std::ostringstream jobDirectoryPath;
   jobDirectoryPath << bgcios::JobsDirectory << _jobId;
   LOG_CIOS_DEBUG_MSG("Job " << _jobId << ": creating jobs directory '" << jobDirectoryPath.str() << "'");
   result = createDirectory(jobDirectoryPath.str());
   if (result.isError()) {
      return result;
   }

   // Create wdir link.
   std::string workDirectoryPath = msg->workingDirectoryPath;
   std::ostringstream wdirPath;
   wdirPath << jobDirectoryPath.str() << "/wdir";
   try {
      bgcios::SymbolicLink link(workDirectoryPath, wdirPath.str());
      link.setOwner(_identity.getUserId(), _identity.getGroupId());
   }
   catch (bgcios::LinkError& e) {
      result.set(bgcios::JobsObjectError, e.errcode());
      LOG_ERROR_MSG("Job " << _jobId << ": error creating wdir link: " << bgcios::errorString(result.errorCode()));
      return result;
   }

   // Create exe link.
   std::string executablePath = msg->arguments; // First argument is executable path name
   std::ostringstream exeTargetPath;
   if (executablePath[0] != '/') { // Prepend work directory if relative path name
      exeTargetPath << workDirectoryPath << "/";
   }
   exeTargetPath << executablePath;
   std::ostringstream exePath;
   exePath << jobDirectoryPath.str() << "/exe";
   try {
      bgcios::SymbolicLink link(exeTargetPath.str(), exePath.str());
      link.setOwner(_identity.getUserId(), _identity.getGroupId());
   }
   catch (bgcios::LinkError& e) {
      result.set(bgcios::JobsObjectError, e.errcode());
      LOG_ERROR_MSG("Job " << _jobId << ": error creating exe link: " << bgcios::errorString(result.errorCode()));
      return result;
   }

   // Create cmdline file.
   std::ostringstream cmdlinePath;
   cmdlinePath << jobDirectoryPath.str() << "/cmdline";
   try {
      bgcios::TextFile cmdlineFile(cmdlinePath.str(), S_IRUSR|S_IRGRP|S_IROTH);
      cmdlineFile.setOwner(_identity.getUserId(), _identity.getGroupId());
      cmdlineFile.writeVector(msg->arguments, msg->argumentSize);
      cmdlineFile.close();
   }
   catch (bgcios::TextFileError& e) {
      result.set(bgcios::JobsObjectError, e.errcode());
      LOG_ERROR_MSG("Job " << _jobId << ": error creating cmdline file: " << bgcios::errorString(result.errorCode()));
      return result;
   }

   // Create environ file.
   std::ostringstream environPath;
   environPath << jobDirectoryPath.str() << "/environ";
   try {
      bgcios::TextFile environFile(environPath.str(), S_IRUSR|S_IRGRP|S_IROTH);
      environFile.setOwner(_identity.getUserId(), _identity.getGroupId());
      environFile.writeVector(msg->variables, msg->variableSize);
      environFile.close();
   }
   catch (bgcios::TextFileError& e) {
      result.set(bgcios::JobsObjectError, e.errcode());
      LOG_ERROR_MSG("Job " << _jobId << ": error creating environ file: " << bgcios::errorString(result.errorCode()));
      return result;
   }

   // Create loginuid file.
   std::ostringstream loginuidPath;
   loginuidPath << jobDirectoryPath.str() << "/loginuid";
   try {
      bgcios::TextFile loginuidFile(loginuidPath.str(), S_IRUSR|S_IRGRP|S_IROTH);
      loginuidFile.setOwner(_identity.getUserId(), _identity.getGroupId());
      loginuidFile.writeString(_identity.getUserIdString());
      loginuidFile.close();
   }
   catch (bgcios::TextFileError& e) {
      result.set(bgcios::JobsObjectError, e.errcode());
      LOG_ERROR_MSG("Job " << _jobId << ": error creating loginuid file: " << bgcios::errorString(result.errorCode()));
      return result;
   }

   // Create logingids file.
   std::ostringstream logingidPath;
   logingidPath << jobDirectoryPath.str() << "/logingids";
   std::string logingid = _identity.getGroupListString();
   try {
      bgcios::TextFile logingidFile(logingidPath.str(), S_IRUSR|S_IRGRP|S_IROTH);
      logingidFile.setOwner(_identity.getUserId(), _identity.getGroupId());
      logingidFile.writeVector((char *)logingid.c_str(), (uint16_t)logingid.size());
      logingidFile.close();
   }
   catch (bgcios::TextFileError& e) {
      result.set(bgcios::JobsObjectError, e.errcode());
      LOG_ERROR_MSG("Job " << _jobId << ": error creating environ file: " << bgcios::errorString(result.errorCode()));
      return result;
   }

   // Create toolctl_rank subdirectory.
   std::ostringstream toolctlRankDirectoryPath;
   toolctlRankDirectoryPath << bgcios::JobsDirectory << _jobId << bgcios::ToolctlRankDirectory;
   result = createDirectory(toolctlRankDirectoryPath.str());
   if (result.isError()) {
      return result;
   }

   // Create toolctl_node subdirectory.
   std::ostringstream toolctlNodeDirectoryPath;
   toolctlNodeDirectoryPath << bgcios::JobsDirectory << _jobId << bgcios::ToolctlNodeDirectory;
   result = createDirectory(toolctlNodeDirectoryPath.str());
   if (result.isError()) {
      return result;
   }

   // Create tools subdirectory.
   std::ostringstream toolsDirectoryPath;
   toolsDirectoryPath << bgcios::JobsDirectory << _jobId << bgcios::ToolsDirectory;
   result = createDirectory(toolsDirectoryPath.str());
   if (result.isError()) {
      return result;
   }

   // Create tools_status subdirectory.
   std::ostringstream toolsStatusDirectoryPath;
   toolsStatusDirectoryPath << bgcios::JobsDirectory << _jobId << bgcios::ToolsStatusDirectory;
   result = createDirectory(toolsStatusDirectoryPath.str());
   if (result.isError()) {
      return result;
   }

   return result;
}

bgcios::MessageResult
Job::createDirectory(std::string directory)
{
   bgcios::MessageResult result;

   // Create the directory.
   if (::mkdir(directory.c_str(), S_IRUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) != 0) {
      result.set(bgcios::JobsDirError, errno);
      LOG_ERROR_MSG("failed to create directory '" << directory << "': " << bgcios::errorString(result.errorCode()));
      return result;
   }

   // Set the owner and group to match the user running the job.
   if (::chown(directory.c_str(), _identity.getUserId(), _identity.getGroupId()) != 0) {
      result.set(bgcios::JobsDirError, errno);
      LOG_ERROR_MSG("Job " << _jobId << ": failed to change ownership of directory '" << directory << "' using uid " <<
                    _identity.getUserId() << " and gid " << _identity.getGroupId() << ": " << bgcios::errorString(result.errorCode()));
      return result;
   }

   return result;
}

int
Job::cleanup(void)
{
   uint32_t activeTools = numTools();
   if (activeTools > 0) {
      LOG_CIOS_DEBUG_MSG("Job " << _jobId << ": " << activeTools << " tools are active and will be ended");

      // End all of the tools in the list.
      for (tool_list_iterator iter = _tools.begin(); iter != _tools.end(); ++iter) {
         endTool(iter->first, SIGKILL); // Job is gone from compute nodes by now and tools should be gone too.
      }

      // Remove all of the tools in the list.
      for (tool_list_iterator iter = _tools.begin(); iter != _tools.end(); ++iter) {
         waitForTool(iter->first);
      }
   }
   else {
      LOG_CIOS_DEBUG_MSG("Job " << _jobId << ": there are no tools still active");
   }

   // Remove the jobs directory for this job.
   std::ostringstream removeCommand;
   removeCommand << "rm -rf " << bgcios::JobsDirectory << _jobId;
   LOG_CIOS_DEBUG_MSG("Job " << _jobId << ": removing jobs directory using command '" << removeCommand.str() << "'");
   if (system(removeCommand.str().c_str()) == -1) {
      int err = errno;
      LOG_ERROR_MSG("Job " << _jobId << ": failed to remove jobs directory using command'" << removeCommand.str() << "': " << bgcios::errorString(err));
      return err;
   }

   return 0;
}

bgcios::MessageResult
Job::startTool(StartToolMessage *msg, bool simulation)
{
   bgcios::MessageResult result;

   // Make sure another tool can be started.
   if (numTools() >= bgcios::toolctl::MaxActiveTools) {
      LOG_ERROR_MSG("Job " << _jobId << ": maximum number of tools (" << bgcios::toolctl::MaxActiveTools << ") already started");
      result.set(bgcios::ToolMaxAttachedExceeded, 0);
      return result;
   }

   // Create a process for running the tool.
   ToolProcessPtr tool = ToolProcessPtr(new ToolProcess(msg->arguments, msg->toolId, msg->header.jobId));
   char *ap = msg->arguments;
   while (*ap++ != 0); // Find start of next argument.
   for (uint16_t index = 1; index < msg->numArguments; ++index) {
      tool->addArgument(ap);
      while (*ap++ != 0); // Find start of next argument.
   }

   // Add BG_JOBID and BG_TOOLID variables.
   std::ostringstream jobidVariable;
   jobidVariable << "BG_JOBID=" << _jobId;
   tool->addEnvironVariable(jobidVariable.str());

   std::ostringstream toolidVariable;
   toolidVariable << "BG_TOOLID=" << tool->getToolId();
   tool->addEnvironVariable(toolidVariable.str());

   const char *libraryValue = getenv("LD_LIBRARY_PATH");
   if (libraryValue != NULL) {
      std::ostringstream libraryVariable;
      libraryVariable << "LD_LIBRARY_PATH=" << libraryValue;
      tool->addEnvironVariable(libraryVariable.str());
   }

   // Start the tool.
   result = tool->start(_identity, simulation);
   if (result.isError()) {
      LOG_ERROR_MSG("Job " << _jobId << ": error starting tool '" << tool->getName() << "': " << bgcios::errorString(result.errorCode()));
      tool.reset();
      return result;
   }

   // Add the tool to the list of tools for the job.
   _tools.add(tool->getToolId(), tool);
   LOG_CIOS_INFO_MSG("Job " << _jobId << ": started tool '" << tool->getName() << "' (" << tool->getToolId() << ") in process " << tool->getProcessId());

   if ( !simulation ) {
       // Add the tool to the tools subdirectory.
       std::ostringstream toolPath;
       toolPath << bgcios::JobsDirectory << _jobId << bgcios::ToolsDirectory << tool->getToolId();
       try {
           bgcios::SymbolicLink link(tool->getName(), toolPath.str());
           link.setOwner(_identity.getUserId(), _identity.getGroupId());
       }
       catch (bgcios::LinkError& e) {
           result.set(bgcios::JobsObjectError, e.errcode());
           LOG_ERROR_MSG("Job " << _jobId << ": error creating tools link for '" << tool->getName() << "' (" << tool->getToolId() << "): " << bgcios::errorString(result.errorCode()));
           return result;
       }

       // Add the tool to the tools_status subdirectory.
       std::ostringstream toolStatusPath;
       toolStatusPath << bgcios::JobsDirectory << _jobId << bgcios::ToolsStatusDirectory << tool->getToolId();
       try {
           bgcios::TextFile statusFile(toolStatusPath.str(), S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
           statusFile.setOwner(_identity.getUserId(), _identity.getGroupId());
           statusFile.close();
       }
       catch (bgcios::LinkError& e) {
           result.set(bgcios::JobsObjectError, e.errcode());
           LOG_ERROR_MSG("Job " << _jobId << ": error creating tool status file for '" << tool->getName() << "' (" << tool->getToolId() << "): " << bgcios::errorString(result.errorCode()));
           return result;
       }
   }

   return result;
}

bool
Job::isToolRunning(uint32_t toolId)
{
   // Validate the tool id.
   ToolProcessPtr tool = getTool(toolId);
   if (tool == NULL) {
      LOG_ERROR_MSG("Job " << _jobId << ": tool " << toolId << " is not active and cannot be checked");
      return false;
   }

   return tool->isRunning();
}

uint32_t
Job::findEndedTool(void)
{
   for (tool_list_iterator iter = _tools.begin(); iter != _tools.end(); ++iter) {
      ToolProcessPtr tool = iter->second;
      if ((!tool->isRunning()) && (!tool->isRemovalPending())) {
         return iter->first;
      }
   }
   return 0;
}

bgcios::MessageResult
Job::endTool(uint32_t toolId, int signo)
{
   bgcios::MessageResult result;

   // Validate the tool id.
   ToolProcessPtr tool = getTool(toolId);
   if (tool == NULL) {
      result.set(bgcios::ToolIdError, ESRCH);
      return result;
   }

   // Signal the tool if it is still running.
   if (tool->isRunning()) {
      int err = tool->signal(signo);
      if (err == 0) {
         LOG_CIOS_INFO_MSG("Job " << _jobId << ": ending tool '" << tool->getName() << "' (" << tool->getToolId() << ") with signal " << signo);
      }
      else {
         result.set(bgcios::RequestFailed, err);
         LOG_ERROR_MSG("Job " << _jobId << ": error ending tool '" << tool->getName() << "' (" << tool->getToolId() << "): " << bgcios::errorString(result.errorCode()));
      }
   }
   else {
      LOG_CIOS_DEBUG_MSG("Job " << _jobId << ": tool '" << tool->getName() << "' (" << tool->getToolId() << ") has already ended");
   }

   return result;
}

int
Job::waitForTool(uint32_t toolId)
{
   // Validate the tool id.
   ToolProcessPtr tool = getTool(toolId);
   if (tool == NULL) {
      return -1;
   }

   // Wait for the tool process to end.
   tool->waitFor();
   if (tool->isExited()) {
      LOG_CIOS_INFO_MSG("Job " << _jobId << ": tool '" << tool->getName() << "' (" << tool->getToolId() << ") exited with status " << tool->getExitStatus());
   }
   if (tool->isSignaled()) {
      LOG_CIOS_INFO_MSG("Job " << _jobId << ": tool '" << tool->getName() << "' (" << tool->getToolId() << ") ended by signal " << tool->getTermSignal());
   }

   // Remove the tool from the tools subdirectory.
   std::ostringstream toolPath;
   toolPath << bgcios::JobsDirectory << _jobId << bgcios::ToolsDirectory << tool->getToolId();
   ::unlink(toolPath.str().c_str());

   // Remove the tool from the tools status subdirectory.
   std::ostringstream toolStatusPath;
   toolStatusPath << bgcios::JobsDirectory << _jobId << bgcios::ToolsStatusDirectory << tool->getToolId();
   ::unlink(toolStatusPath.str().c_str());

   // Set flag to indicate removal is almost completed.
   tool->setRemovalPending(true);

   return tool->getStatus();
}

bgcios::MessageResult
Job::removeTool(uint32_t toolId)
{
   bgcios::MessageResult result;

   // Validate the tool id.
   ToolProcessPtr tool = getTool(toolId);
   if (tool == NULL) {
      result.set(bgcios::ToolIdError, ESRCH);
      return result;
   }

   // Remove the tool from the list of tools for the job.
   _tools.remove(toolId);

   // Destroy the tool process object.
   std::string name = tool->getName();
   tool.reset();
   LOG_CIOS_DEBUG_MSG("Job " << _jobId << ": tool '" << name << "' (" << toolId << ") removed from job");

   return result;
}

void
Job::endAllTools(int signo)
{
   uint32_t activeTools = numTools();
   if (activeTools > 0) {
      LOG_CIOS_DEBUG_MSG("Job " << _jobId << ": " << activeTools << " tools are active and will be ended");

      // End all of the tools in the list.
      for (tool_list_iterator iter = _tools.begin(); iter != _tools.end(); ++iter) {
         endTool(iter->first, signo);
      }
   }

   return;
}

uint32_t
Job::checkAllToolsStatus(uint32_t seconds)
{
   uint32_t activeTools = 0;
   time_t now = time(NULL);
   for (tool_list_iterator iter = _tools.begin(); iter != _tools.end(); ++iter) {
      std::ostringstream toolStatusPath;
      toolStatusPath << bgcios::JobsDirectory << _jobId << bgcios::ToolsStatusDirectory << iter->first;
      bgcios::TextFile statusFile(toolStatusPath.str());
      if (statusFile.getModificationTime() > now - seconds) {
         ++activeTools;
      }
   }
   return activeTools;
}

int
Job::runPrologProgram(std::string prologProgramPath, int32_t timeout)
{
   // Start a thread to run and monitor the prolog program.
   _prologProgramMonitor = PrologProgramMonitorPtr(new PrologProgramMonitor(prologProgramPath, timeout, _jobId, loadJobAckAccumulator.get()));
   int err = _prologProgramMonitor->start();
   if (err != 0) {
      LOG_ERROR_MSG("Job " << _jobId << ": error starting prolog program monitor thread: " << bgcios::errorString(err));
   }

   return err;
}

int
Job::runEpilogProgram(std::string epilogProgramPath, int32_t timeout)
{
   // Start a thread to run and monitor the epilog program.
   _epilogProgramMonitor = EpilogProgramMonitorPtr(new EpilogProgramMonitor(epilogProgramPath, timeout, _jobId, cleanupJobAckAccumulator.get()));
   int err = _epilogProgramMonitor->start();
   if (err != 0) {
      LOG_ERROR_MSG("Job " << _jobId << ": error starting epilog program monitor thread: " << bgcios::errorString(err));
   }

   return err;
}

void
Job::addComputeNode(ComputeNodePtr cnode)
{
   _cnodes.push_back(cnode);
   NodeCoordinates coords = cnode->getCoords();
   if ((coords.aCoord == _jobLeader.aCoord) &&
       (coords.bCoord == _jobLeader.bCoord) &&
       (coords.cCoord == _jobLeader.cCoord) &&
       (coords.dCoord == _jobLeader.dCoord) &&
       (coords.eCoord == _jobLeader.eCoord)) {
      _jobLeaderClient = cnode->getClient();
      LOG_CIOS_TRACE_MSG("Job " << _jobId << ": job leader is attached to this I/O node " << *cnode);
   }

   return;
}

bool
Job::isComputeNodeInJob(const ComputeNodePtr& cnode) const
{
   if (_blockId != cnode->getBlockId()) {
      LOG_CIOS_TRACE_MSG("block id " << _blockId << " does not match " << cnode->getBlockId());
      return false;
   }

   NodeCoordinates coords = cnode->getCoords();
   if ((coords.aCoord >= _corner.aCoord) &&
       (coords.bCoord >= _corner.bCoord) &&
       (coords.cCoord >= _corner.cCoord) &&
       (coords.dCoord >= _corner.dCoord) &&
       (coords.eCoord >= _corner.eCoord) &&
       (coords.aCoord < (_corner.aCoord + _shape.aCoord)) &&
       (coords.bCoord < (_corner.bCoord + _shape.bCoord)) &&
       (coords.cCoord < (_corner.cCoord + _shape.cCoord)) &&
       (coords.dCoord < (_corner.dCoord + _shape.dCoord)) &&
       (coords.eCoord < (_corner.eCoord + _shape.eCoord))) {
      return true;
   }

   LOG_CIOS_TRACE_MSG("no match on corner " << (int)_corner.aCoord << (int)_corner.bCoord << (int)_corner.cCoord << (int)_corner.dCoord << (int)_corner.eCoord);
   LOG_CIOS_TRACE_MSG("no match on shape " << (int)_shape.aCoord << (int)_shape.bCoord << (int)_shape.cCoord << (int)_shape.dCoord << (int)_shape.eCoord);
   LOG_CIOS_TRACE_MSG("no match on cnode " << (int)coords.aCoord << (int)coords.bCoord << (int)coords.cCoord << (int)coords.dCoord << (int)coords.eCoord);
   return false;
}

void
Job::sendMessageToJobLeaderNode(bgcios::MessageHeader *mh)
{
   // Just return if job leader is not connected to this I/O node.
   if (_jobLeaderClient == NULL) {
      return;
   }
   // Send the message to the job leader compute node.
   CIOSLOGMSG(BGV_SEND_JLN,mh);
   _jobLeaderClient->postSendAuxMessage( (char *)mh, mh->length);

   return;
}

void
Job::sendMessageToAllNodes(bgcios::MessageHeader *mh)
{
   // Run the list of compute nodes and send the message to each one.
   for (cnode_list_iterator iter = _cnodes.begin(); iter != _cnodes.end(); ++iter) {
      RdmaClientPtr client = (*iter)->getClient();
      try {
        client->postSendAuxMessage((char *)mh, mh->length);
      } catch( const bgcios::RdmaError& e ) {
          LOG_ERROR_MSG( "could not send message " << toString(mh) << " to client " << client->getRemoteAddressString() << ": " << e.what() );
      }
   }
   CIOSLOGMSG(BGV_SEND_ALL,mh);

   return;
}

std::ostream&
Job::writeTo(std::ostream& os) const
{
   os << "jobId=" << _jobId << " blockId=" << _blockId;
   os << " corner=" << (int)_corner.aCoord << "," << (int)_corner.bCoord << "," << (int)_corner.cCoord << ",";
   os << (int)_corner.dCoord << "," << (int)_corner.eCoord << "," << (int)_corner.core;
   os << " shape=" << (int)_shape.aCoord << "x" << (int)_shape.bCoord << "x" << (int)_shape.cCoord << "x";
   os << (int)_shape.dCoord << "x" << (int)_shape.eCoord << "+" << (int)_shape.core;
   os << " jobLeader=" << (int)_jobLeader.aCoord << "," << (int)_jobLeader.bCoord << "," << (int)_jobLeader.cCoord << ",";
   os << (int)_jobLeader.dCoord << "," << (int)_jobLeader.eCoord << " numRanks=" << _numRanks;
   return os;
}

