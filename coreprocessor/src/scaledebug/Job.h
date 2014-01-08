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

#ifndef JOB_H
#define JOB_H

// Includes
#include "CoreFile.h"
#include <ramdisk/include/services/common/LocalSocket.h>
#include <ramdisk/include/services/common/PointerMap.h>
#include <tr1/memory>

class Job
{
public:

   //! \brief  Default constructor.
   //! \param  jobId Job identifier.
   //! \param  toolId Tool identifer.

   Job(uint64_t jobId, uint32_t toolId);

   //! \brief  Add a core file to the job.
   //! \param  inMsg Pointer to Notify message with details on fault.
   //! \return Pointer to CoreFile object that was added.

   void addCoreFile(AttachAckMessage *inMsg);

   //! \brief  Get the job identifier.
   //! \return Job identifier value.

   uint64_t getJobId(void) const { return _jobId; }

   //! \brief  Get the tool identifier.
   //! \return Tool identifier value.

   uint32_t getToolId(void) const { return _toolId; }

   //! Map of CoreFile objects indexed by rank.
   bgcios::PointerMap<uint32_t, CoreFilePtr> coreFiles;

   //! Typedef for core file list iterator.
   typedef bgcios::PointerMap <uint32_t, CoreFilePtr>::const_iterator corefile_list_iterator;

   //! Map of tool channel sockets indexed by file descriptor.
   bgcios::PointerMap<int, bgcios::LocalStreamSocketPtr> toolChannels;

   // Typedef for tool channel list iterator.
   typedef bgcios::PointerMap <int, bgcios::LocalStreamSocketPtr>::const_iterator channel_list_iterator;

private:

   //! Job identifier.
   uint64_t _jobId;

   //! Tool identifier.
   uint32_t _toolId;

   //! Path to running program.
   std::string _programName;
};

//! Smart pointer for Job object.
typedef std::tr1::shared_ptr<Job> JobPtr;

#endif // JOB_H

