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

//! \file  ServiceManager.h
//! \brief Declaration for bgcios::iosctl::ServiceManager class.

#ifndef IOSCTL_SERVICEMANAGER_H
#define IOSCTL_SERVICEMANAGER_H

// Includes
#include "ServiceProcess.h"
#include "IosController.h"
#include <ramdisk/include/services/common/PointerMap.h>
#include <bgq_util/include/pthreadmutex.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <tr1/memory>

namespace bgcios
{

namespace iosctl
{

//! \brief Manage I/O service daemons.

class ServiceManager
{
public:

   //! \brief  Default constructor.
   //! \param  maxServiceRestarts Maximum number of times a service can be restarted.

   ServiceManager(uint16_t maxServiceRestarts)
   {
      _maxServiceRestarts = maxServiceRestarts;
   }

   //! \brief  Default destructor.

   ~ServiceManager()
   {
      _maxServiceRestarts = 0;
   }

   //! \brief  Set the I/O service controller object.
   //! \param  controller Pointer to I/O service controller object.
   //! \return Nothing.

   void setController(IosControllerPtr controller) { _controller = controller; }

   //! \brief  Start the specified service.
   //! \param  program Path to executable program.
   //! \param  args List of arguments.
   //! \param  cmdChannelPath Path to command channel for service.
   //! \param  numStarts number of times to restart this service
   //! \return 0 when successful, errno when unsuccessful.

   int start(std::string program, std::vector <std::string>& args, std::string cmdChannelPath, uint16_t numStarts = 0);

   //! \brief  Restart the specified service.
   //! \param  child Process id of child process that was running service. 
   //! \return 0 when successful, errno when unsuccessful.

   int restart(pid_t child);

   //! \brief  Remove the specified service from the list of services.
   //! \param  child Process id of child process that was running service. 
   //! \return 0 when successful, errno when unsuccessful.

   int remove(pid_t child);

   //! \brief  Send a message to all services in the list of services.
   //! \param  buffer Pointer to message.
   //! \return 0 when successful, non-zero value indicates number of services in error.

   int sendToAll(void *buffer);

   //! \brief  Return the size of the list of services.
   //! \return Number of services in list.

   size_t listSize(void) const { return _serviceList.size(); }


   //! \brief Return the maximum times to restart a service.
    
   uint16_t maxServiceRestarts() const { return _maxServiceRestarts; }


private:

   //! List of running services.
   bgcios::PointerMap <pid_t, ServiceProcessPtr> _serviceList;

   //! Typedef for services list iterator.
   typedef bgcios::PointerMap <pid_t, ServiceProcessPtr>::const_iterator service_list_iterator;

   //! I/O service controller object.
   IosControllerPtr _controller;

   //! Maximum number of times a service can be restarted.
   uint16_t _maxServiceRestarts;

   //! Lock to serialize access to the list of services.
   PthreadMutex _mutex;

};

//! Smart pointer for ServiceManager object.
typedef std::tr1::shared_ptr<ServiceManager> ServiceManagerPtr;

} // namespace iosctl

} // namespace bgcios

#endif // IOSCTL_SERVICEMANAGER_H

