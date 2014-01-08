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

// Includes
#include "Job.h"
#include <ramdisk/include/services/ServicesConstants.h>
#include <utility/include/Log.h>
#include <limits.h>
#include <errno.h>
#include <sstream>
#include <boost/filesystem.hpp>

LOG_DECLARE_FILE("cios.tests");

using namespace boost::filesystem;

Job::Job(uint64_t jobId, uint32_t toolId)
{
   // Initialize private data.
   _jobId = jobId;
   _toolId = toolId;

   // Get info about the job that is running.
   std::ostringstream exePath;
   exePath << "/jobs/" << _jobId << "/exe";
   char *name = new char [PATH_MAX];
   name[0] = '\0';
   int rc = ::readlink(exePath.str().c_str(), name, PATH_MAX);
   if (rc > 0) {
      _programName = name;
   }
   else {
      LOG_ERROR_MSG("Error getting program name: " << bgcios::errorString(errno));
   }

   // Connect to all of the tool control daemons servicing the job on this I/O node.
   std::ostringstream dirPath;
   dirPath << bgcios::JobsDirectory << _jobId << bgcios::ToolctlNodeDirectory;
   for (directory_iterator iter(dirPath.str()); iter != directory_iterator(); ++iter) {
      LOG_DEBUG_MSG("Attempting to connect to " << iter->path());
      bgcios::LocalStreamSocketPtr toolChannel;
      try {
         toolChannel = bgcios::LocalStreamSocketPtr(new bgcios::LocalStreamSocket());
         toolChannel->connect(iter->path().string());
      }
      catch (bgcios::SocketError& e) {
         LOG_ERROR_MSG("Error connecting to tool data channel socket '" << iter->path() << ": " << e.what());
         exit(1);
      }
      toolChannels.add(toolChannel->getSd(), toolChannel);
      LOG_DEBUG_MSG("Connected to " << iter->path());
   }
}

void
Job::addCoreFile(AttachAckMessage *inMsg)
{
    // Create a CoreFile object and add it to the list.
    int x;
    for(x=0; x<inMsg->numProcess; x++)
    {
        CoreFilePtr coreFile = CoreFilePtr(new CoreFile());
        coreFile->setJobId(_jobId);
        coreFile->setProgramName(_programName);
        coreFile->setRank(inMsg->rank[x]);
        coreFiles.add(inMsg->rank[x], coreFile);
        LOG_DEBUG_MSG("Created core file for rank " << inMsg->rank[x]);
    }
}
