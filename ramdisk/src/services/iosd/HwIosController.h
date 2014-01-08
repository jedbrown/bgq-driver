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

//! \file  HwIosController.h
//! \brief Declaration and inline methods for bgcios::iosctl::HwIosController class.

#ifndef IOSCTL_HWIOSCONTROLLER_H
#define IOSCTL_HWIOSCONTROLLER_H

// Includes
#include "IosController.h"
#include "IosConfig.h"
#include "ServiceManager.h"
//#include "NodeServices.h"
#include <ramdisk/include/services/common/PointerMap.h>
#include <ramdisk/include/services/common/SystemLock.h>
#include <netinet/in.h>
#include <tr1/memory>

namespace bgcios
{

namespace iosctl
{

//! \brief Handle job control service messages.

class HwIosController : public IosController
{
public:

   //! \brief  Default constructor.

   HwIosController(IosConfig& config, ServiceManagerPtr serviceManager);

   //! \brief  Default destructor.

   ~HwIosController();

   //! \brief  Open all connections needed by the service daemon.
   //! \param  dataChannelPort Port number for listening data channel socket.
   //! \return 0 when successful, errno when unsuccessful.

   int startup(in_port_t dataChannelPort);

   //! \brief  Close all connections needed by the service daemon.
   //! \return 0 when successful, errno when unsuccessful.

   int cleanup(void);

private:

   //! \brief  Process a Ready message.
   //! \param  dest Path to destination command channel.
   //! \return 0 when successful, errno when unsuccessful.

   int ready(std::string dest);

   //! \brief  Process a StartNodeServices message.
   //! \param  dest Path to destination command channel.
   //! \return 0 when successful, errno when unsuccessful.

   int startNodeServices(std::string dest);

   //! Service manager.
   ServiceManagerPtr _serviceManager;

   //! Path to the system I/O daemon.
   std::string _sysiodPath;

   //! List of arguments for the system I/O daemon.
   std::vector <std::string> _sysiodArguments;

   //! Path to the tool control daemon.
   std::string _toolctldPath;

   //! List of arguments for the tool control daemon.
   std::vector <std::string> _toolctldArguments;

   //! System-scoped lock to serialize allocating memory regions.
   SystemLockPtr _allocateMemoryRegionLock;
};

//! Smart pointer for HwIosController object.
typedef std::tr1::shared_ptr<HwIosController> HwIosControllerPtr;

} // namespace iosctl

} // namespace bgcios

#endif // IOSCTL_HWIOSCONTROLLER_H

