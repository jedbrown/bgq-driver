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

//! \file  SimIosController.cc
//! \brief Methods for bgcios::iosctl::SimIosController class.

// Includes
#include "SimIosController.h"
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/logging.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fstream>

using namespace bgcios::iosctl;

LOG_DECLARE_FILE("cios.iosd");


SimIosController::SimIosController(uint16_t simId) : IosController()
{
   // Set work directory.
   std::ostringstream wdir;
   wdir << bgcios::SimulationDirectory << simId << "/";
   _workDirectory = wdir.str();
}

SimIosController::~SimIosController()
{
   free(_inboundMessage);
   free(_outboundMessage);
}

int
SimIosController::startup(in_port_t dataChannelPort)
{
   // Create the work directory when running under simulation.
   int rc = access(_workDirectory.c_str(), W_OK);
   if ((rc != 0) && (errno != ENOENT)) {
      LOG_ERROR_MSG("error checking work directory: " << bgcios::errorString(errno));
      LOG_FATAL_MSG("Simulation id is already in use.  Check " << _workDirectory << " directory.");
      exit(EXIT_FAILURE);
   }
   if (errno == ENOENT) {
      rc = mkdir(_workDirectory.c_str(), 0775);
      if (rc != 0) {
         LOG_FATAL_MSG("error creating directory " << _workDirectory << ": " << bgcios::errorString(errno));
         exit(EXIT_FAILURE);
      }
      LOG_CIOS_INFO_MSG("created new work directory '" << _workDirectory << "'");
   }

   // Build the path to the command channel.
   std::ostringstream cmdChannelPath;
   cmdChannelPath << _workDirectory << bgcios::IosctlCommandChannelName << "." << getpid();

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

   // Get the address assigned by TCP and store port number in a file.
   std::ostringstream portFileName;
   portFileName << _workDirectory << "ioscontrol.port";
   std::ofstream portFile(portFileName.str().c_str(), std::ios_base::trunc);
   portFile << ntohs(_dataListener->getPort()) << std::endl;
   LOG_CIOS_DEBUG_MSG("stored port number " << ntohs(_dataListener->getPort()) << " in file " << portFileName.str());

   // Prepare socket to listen for connections.
   try {
      _dataListener->listen(1);
   }
   catch (bgcios::SocketError& e) {
      LOG_ERROR_MSG("error listening for new connections: " << e.what());
      return e.errcode();
   }
   LOG_CIOS_DEBUG_MSG("listening for new data channel connections on fd " << _dataListener->getSd() << " using address " << _dataListener->getName());

   return 0;
}

int
SimIosController::cleanup(void)
{
   LOG_CIOS_INFO_MSG("running cleanup ...");

   // Remove the port number file.
   std::ostringstream portFileName;
   portFileName << _workDirectory << "ioscontrol.port";
   if (::unlink(portFileName.str().c_str()) != 0) {
      LOG_ERROR_MSG("error removing port number file " << portFileName.str() << ": " << bgcios::errorString(errno));
   }

   // Remove the command channel.
   _cmdChannel.reset();

   return 0;
}
