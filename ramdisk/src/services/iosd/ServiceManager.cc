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

//! \file  ServiceManager.cc
//! \brief Methods for bgcios::iosctl::ServiceManager class.

// Includes
#include "ServiceManager.h"
#include <ramdisk/include/services/common/LocalSocket.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/RasEvent.h>
#include <ramdisk/include/services/MessageUtility.h>
#include <ramdisk/include/services/common/logging.h>

using namespace bgcios::iosctl;

LOG_DECLARE_FILE("cios.iosd");


int
ServiceManager::start(std::string program, std::vector <std::string>& args, std::string cmdChannelPath, uint16_t numStarts)
{
   // Lock the list of services.
   PthreadMutexHolder h;
   h.Lock(&_mutex);

   // Create a service process object.
   ServiceProcessPtr service = ServiceProcessPtr(new ServiceProcess(program));
   service->addArguments(args);
   service->setCmdChannelPath(cmdChannelPath);
   service->setNumStarts(numStarts);
   for (char **ep = environ; *ep != NULL; ++ep) { // Give current environment to service process.
      service->addEnvironVariable(*ep);
   }

   // Start the service process.
   bgcios::MessageResult result = service->start();
   if (result.isError()) {
      LOG_ERROR_MSG("error starting service " << service->getName() << ": " << bgcios::errorString(result.errorCode()));
      service.reset();
      return result.errorCode();
   }

   // Add the process to the list of services.
   _serviceList.add(service->getProcessId(), service);
   LOG_CIOS_DEBUG_MSG("started service '" << service->getName() << "' in process " << service->getProcessId());

   return 0;
}

int
ServiceManager::restart(pid_t child)
{
   // Lock the list of services.
   PthreadMutexHolder h;
   h.Lock(&_mutex);

   // Find the service process in the list.
   ServiceProcessPtr service = _serviceList.get(child);
   if (service == NULL) {
      LOG_ERROR_MSG("service with process id " << child << " is not in the list of services");
      return ENOENT;
   }

   // Remove the old process from the list of services.
   LOG_CIOS_WARN_MSG(service->getName() << " service in process " << child << " has ended");
   _serviceList.remove(child);

   // See if the service has been restarted the maximum number of times.
   if (service->getNumStarts() > _maxServiceRestarts) {
      LOG_ERROR_MSG(service->getName() << " service has exceeded " << _maxServiceRestarts << " restarts and has been terminated");
      bgcios::RasEvent rasEvent(bgcios::DaemonRestartLimit);
      rasEvent << child << service->getNumStarts();
      rasEvent.send();
      return ECANCELED;
   }

   // Start the service process again.
   bgcios::MessageResult result = service->start();
   if (result.isError()) {
      LOG_ERROR_MSG("error restarting service " << service->getName() << ": " << bgcios::errorString(result.errorCode()));
      return result.errorCode();
   }

   // Add the new process to list of services.
   _serviceList.add(service->getProcessId(), service);
   LOG_CIOS_WARN_MSG(service->getName() << " service in process " << service->getProcessId() << " has been restarted " << service->getNumStarts() - 1 << " times");

   // Send ras event.
   bgcios::RasEvent rasEvent(bgcios::DaemonRestarted);
   rasEvent << child << service->getProcessId() << service->getNumStarts();
   rasEvent.send();

   return 0;
}

int
ServiceManager::remove(pid_t child)
{
   // Lock the list of services.
   PthreadMutexHolder h;
   h.Lock(&_mutex);

   // Make sure the service process is valid.
   if (!_serviceList.isValid(child)) {
      LOG_ERROR_MSG("service with process id " << child << " is not in the list of services");
      return ENOENT;
   }

   // Remove the process from the list of services.
   _serviceList.remove(child);

   return 0;
}

int
ServiceManager::sendToAll(void *buffer)
{
   // Lock the list of services.
   PthreadMutexHolder h;
   h.Lock(&_mutex);

   // Send the message to every service in the list of services.
   bgcios::MessageHeader *msghdr = (bgcios::MessageHeader *)buffer;
   int errors = 0;
   for (service_list_iterator iter = _serviceList.begin(); iter != _serviceList.end(); ++iter) {
      ServiceProcessPtr service = iter->second;
      std::string dest = service->getCmdChannelPath();
      try {
         _controller->sendToCommandChannel(dest, buffer);
         LOG_CIOS_DEBUG_MSG(toString(msghdr->type) << " message sent to service at '" << dest << "'");
      }
      catch (bgcios::SocketError& e) {
         LOG_ERROR_MSG("error sending " << toString(msghdr->type) << " message to service at '" << dest << "': " << bgcios::errorString(e.errcode()));
         errors += 1;
      }
   }

   return errors;
}
