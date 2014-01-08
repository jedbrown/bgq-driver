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

//! \file  HwIosController.cc
//! \brief Methods for bgcios::iosctl::HwIosController class.

// Includes
#include "HwIosController.h"
#include <ramdisk/include/services/IosctlMessages.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/logging.h>
#include <stdlib.h>

using namespace bgcios::iosctl;

LOG_DECLARE_FILE("cios.iosd");


HwIosController::HwIosController(IosConfig& config, ServiceManagerPtr manager) : IosController()
{
   // Set private data.
   _serviceManager = manager;
   _workDirectory = bgcios::WorkDirectory;

   // Set the paths and arguments for the daemons that are started for each compute node.
   _sysiodPath = config.getSysioDaemonPath();
   _toolctldPath = config.getToolctlDaemonPath();

   std::ostringstream propertiesArg;
   propertiesArg << "--properties=" << config.getProperties()->getFilename();
   _sysiodArguments.push_back(propertiesArg.str());
   _toolctldArguments.push_back(propertiesArg.str());
}

HwIosController::~HwIosController()
{
   free(_inboundMessage);
   free(_outboundMessage);
}

int
HwIosController::startup(in_port_t dataChannelPort)
{
   // Build the path to the command channel.
   std::ostringstream cmdChannelPath;
   cmdChannelPath << _workDirectory << bgcios::IosctlCommandChannelName;

   // Create the command channel socket.
   try {
      _cmdChannel = LocalDatagramSocketPtr(new LocalDatagramSocket(cmdChannelPath.str()));
   }
   catch (SocketError& e) {
      LOG_ERROR_MSG("error creating command channel '" << cmdChannelPath.str() << "': " << e.what());
      return e.errcode();
   }
   LOG_CIOS_DEBUG_MSG("[" << _cmdChannel->getSd() << "] created command channel at '" << _cmdChannel->getName() << "'");

   // Create listening socket for data channel.
   try {
      _dataListener = bgcios::InetStreamSocketPtr(new bgcios::InetStreamSocket(dataChannelPort, true));
   }
   catch (bgcios::SocketError& e) {
      LOG_ERROR_MSG("error creating listening socket for data channel: " << e.what());
      _dataListener.reset();
      return e.errcode();
   }

   // Prepare socket to listen for connections.
   try {
      _dataListener->listen(1);
   }
   catch (bgcios::SocketError& e) {
      LOG_ERROR_MSG("error listening for new connections: " << e.what());
      return e.errcode();
   }
   LOG_CIOS_DEBUG_MSG("listening for new data channel connections on fd " << _dataListener->getSd() << " using address " << _dataListener->getName());

   // Create and initialize a system-scoped lock for allocating memory regions.
   _allocateMemoryRegionLock = bgcios::SystemLockPtr(new SystemLock(bgcios::AllocateMemoryRegionKey));
   _allocateMemoryRegionLock->setValue(1);

   return 0;
}

int
HwIosController::cleanup(void)
{
   return 0;
}

int
#ifdef LOG_CIOS_DEBUG_DISABLE
HwIosController::ready(std::string)
#else
HwIosController::ready(std::string dest)
#endif
{
   // Get pointer to Ready message available from inbound buffer.
   ReadyMessage *inMsg = (ReadyMessage *)_inboundMessage;
   LOG_CIOS_DEBUG_MSG("Ready message received from service at '" << dest << "' with service id " << inMsg->serviceId);

   // If accumulating Ready messages, just increment the accumulator.
   if (_readyAccumulator.getLimit() > 0) {
      _readyAccumulator.add(&(inMsg->header));
      return 0;
   }
   return 0;
}

int
#ifdef LOG_CIOS_DEBUG_DISABLE
HwIosController::startNodeServices(std::string)
#else
HwIosController::startNodeServices(std::string dest)
#endif
{
   // Get pointer to StartNodeServices message available from inbound buffer.
   StartNodeServicesMessage *inMsg = (StartNodeServicesMessage *)_inboundMessage;
   LOG_CIOS_DEBUG_MSG("StartNodeServices message received from service at '" << dest << "' with service id " << inMsg->serviceId);

   // Set the serviceId configuration variable.
   std::ostringstream serviceIdArg;
   serviceIdArg << "--service_id=" << inMsg->serviceId;
   
   //Add on info as to compute node being served
   std::ostringstream CNtorusArg;
   CNtorusArg << "--CNtorus=("<< ((inMsg->CNtorus>>24) & 0x3F)<< ","<< ((inMsg->CNtorus>>18) & 0x3F)<< ","<< ((inMsg->CNtorus>>12) & 0x3F)<< "," << ((inMsg->CNtorus>>6) & 0x3F)<< ","<<(inMsg->CNtorus & 0x3F)<<")";
   
   // Start a sysiod.
   std::ostringstream sysiodCmdChannelPath;
   sysiodCmdChannelPath << _workDirectory << bgcios::SysioCommandChannelName << "." << inMsg->serviceId;
   _sysiodArguments.push_back(serviceIdArg.str());
   _sysiodArguments.push_back(CNtorusArg.str());
   _serviceManager->start(_sysiodPath, _sysiodArguments, sysiodCmdChannelPath.str(), _serviceManager->maxServiceRestarts());
   _sysiodArguments.pop_back();
   _sysiodArguments.pop_back();
   LOG_CIOS_TRACE_MSG("started '" << _sysiodPath << "' with service id " << inMsg->serviceId);

      // Start a toolctld.
   std::ostringstream toolctldCmdChannelPath;
   toolctldCmdChannelPath << _workDirectory << bgcios::ToolctlCommandChannelName << "." << inMsg->serviceId;
   _toolctldArguments.push_back(serviceIdArg.str());
   _toolctldArguments.push_back(CNtorusArg.str());
   _serviceManager->start(_toolctldPath, _toolctldArguments, toolctldCmdChannelPath.str(), _serviceManager->maxServiceRestarts());
   _toolctldArguments.pop_back();
   _toolctldArguments.pop_back();
   LOG_CIOS_TRACE_MSG("started '" << _toolctldPath << "' with service id " << inMsg->serviceId);


   return 0;
}


