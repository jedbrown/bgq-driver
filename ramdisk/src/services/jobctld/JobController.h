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

//! \file  JobController.h
//! \brief Declaration and inline methods for bgcios::jobctl::JobController class.

#ifndef JOBCTL_JOBCONTROLLER_H
#define JOBCTL_JOBCONTROLLER_H

// Includes
#include <ramdisk/include/services/common/ServiceController.h>
#include "Job.h"
#include "JobctlConfig.h"
#include <ramdisk/include/services/common/PointerMap.h>
#include <ramdisk/include/services/common/InetSocket.h>
#include <tr1/memory>

namespace bgcios
{

namespace jobctl
{

//! \brief Handle job control service messages.

class JobController : public bgcios::ServiceController
{
public:

   //! \brief  Default constructor.
   //! \param  config Configuration from command line and properties file.

   JobController(JobctlConfigPtr config) : bgcios::ServiceController() { _config = config; }

   //! \brief  Default destructor.

   ~JobController();

   //! \brief  Open all connections needed by the service daemon.
   //! \param  dataChannelPort Port number for listening data channel socket.
   //! \return 0 when successful, errno when unsuccessful.

   virtual int startup(in_port_t dataChannelPort);

   //! \brief  Close all connections needed by the service daemon.
   //! \return 0 when successful, errno when unsuccessful.

   virtual int cleanup(void) { return 0; }

   //! \brief  Monitor for events from all connections.
   //! \return Nothing.

   virtual void eventMonitor(void) { return; }
   
   //! \brief  Handle a Heartbeat message.
   //! \return 0 when successful, errno when unsuccessful.

   int heartbeat(void);

protected:

   //! Configuration from command line and properties file.
   JobctlConfigPtr _config;

   //! Map of currently running jobs indexed by job id.
   bgcios::PointerMap<uint64_t, JobPtr> _jobs;

   //! Typedef for job list iterator.
   typedef bgcios::PointerMap <uint64_t, JobPtr>::const_iterator job_list_iterator;

};

//! Smart pointer for JobController object.
typedef std::tr1::shared_ptr<JobController> JobControllerPtr;

} // namespace jobctl

} // namespace bgcios

#endif // JOBCTL_JOBCONTROLLER_H

