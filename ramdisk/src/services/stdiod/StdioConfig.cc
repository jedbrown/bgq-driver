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

//! \file  StdioConfig.cc
//! \brief Methods for bgcios::stdio::StdioConfig class.

// Includes
#include "StdioConfig.h"
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/logging.h>
#include <iostream>
#include <iterator>
#include <netinet/in.h>

using namespace bgcios::stdio;

LOG_DECLARE_FILE("cios.stdiod");

//! Default port number for control system connection.
const uint16_t DefaultListenPort = 7003;

//! Default send buffer size for control system connection.
const int DefaultSendBufferSize = 262144;

StdioConfig::StdioConfig(int argc, char **argv) : bgcios::CiosConfig()
{
   // Create list of valid arguments.
   po::options_description desc("stdiod options");
   desc.add_options()
      ("help,h", "display help text")
      ("listen_port", po::value<uint16_t>()->default_value(DefaultListenPort), "port number for control system connection")
      ("send_buffer_size", po::value<int>()->default_value(DefaultSendBufferSize), "size of send buffer for control system connection")
      ("simulation_id", po::value<uint16_t>()->default_value(bgcios::DefaultSimulationId), "unique id for simulation mode")
      ("log_level", po::value<std::string>(), "logging level")
      ("properties", po::value<std::string>(), "path to properties file")
      ;
   
   // Parse the command line options.
   try {
      po::store(po::parse_command_line(argc, argv, desc), _variableMap);
      po::notify(_variableMap);
   }
   catch (boost::program_options::unknown_option e) {
      std::cout << "Error parsing command line: " << e.what() << std::endl;
      std::cout << desc << std::endl;
      exit(EXIT_FAILURE);
   }
   
   // If help option was specified, display help text and exit.
   if (_variableMap.count("help")) {
      std::cout << desc << std::endl;
      exit(EXIT_SUCCESS);
   }
   
   // Open properties file.
   if (_variableMap.count("properties")) {
      _properties = bgq::utility::Properties::create( _variableMap["properties"].as<std::string>() );
   }
   else {
      _properties = bgq::utility::Properties::create();
   }
   
   // Check to see if logging section is in the properties file to avoid extraneous error message.
   std::string loggingSection = "logging.cios.iosd";
   try {
      (void)_properties->getValues(loggingSection);
   }
   catch (const std::invalid_argument& e) {
      loggingSection = "logging.cios";
      try {
         (void)_properties->getValues(loggingSection);
      }
      catch (const std::invalid_argument& e) {
         loggingSection.clear();
      }
   }

   // Initialize logging for the daemon.
   if (!loggingSection.empty()) {
      bgq::utility::initializeLogging(*_properties, loggingSection.substr(8));
   }
   else {
      bgq::utility::initializeLogging(*_properties);
   }
 
   // If specified, override the logging level.
   log4cxx::LoggerPtr mylog = log4cxx::Logger::getLogger("ibm.cios.stdiod");
   if (_variableMap.count("log_evel")) {
      mylog->setLevel(log4cxx::Level::toLevel(_variableMap["log_level"].as<std::string>()));
   }
   
   // If logging level is set for daemon, set cios.common logger to match.
   if (mylog->getLevel() != NULL) {
      log4cxx::LoggerPtr commonLog = log4cxx::Logger::getLogger("ibm.cios.common");
      commonLog->setLevel(mylog->getLevel());
   }

   // Log info about properties.
   LOG_CIOS_INFO_MSG("configured from properties file '" << _properties->getFilename() << "'");
   std::ostringstream args;
   std::copy(argv, argv + argc, std::ostream_iterator<char*>(args, " "));
   LOG_CIOS_DEBUG_MSG("startup parameters: " << args.str());
}

uint16_t
StdioConfig::getListenPort(void) const
{
   // First, check for command line argument.
   uint16_t port = _variableMap["listen_port"].as<uint16_t>();
   if (!_variableMap["listen_port"].defaulted()) {
      LOG_CIOS_DEBUG_MSG("set listen_port to " << port << " from command line argument");
   }

   // Second, check for key in properties file.
   else {
      try {
         port = boost::lexical_cast<uint16_t>( _properties->getValue("cios.stdiod", "listen_port") );
         LOG_CIOS_DEBUG_MSG("set listen_port to " << port << " from properties file " << _properties->getFilename());
      }
      catch (const std::invalid_argument& e) {
         // This isn't fatal so we'll use the default value.
         LOG_CIOS_DEBUG_MSG("listen_port not found in properties file '" << _properties->getFilename() << "' so using default value " << DefaultListenPort);
      } 
      catch (const boost::bad_lexical_cast& e) {
         // Value is invalid.
         LOG_CIOS_WARN_MSG("listen_port property from properties file '" << _properties->getFilename() << "' is invalid so using default value " <<
                      DefaultListenPort << " (" << e.what() << ")");
      }
   }
   
   // Make sure the value is valid.
   if (port < IPPORT_RESERVED) {
      LOG_CIOS_WARN_MSG("listen_port property value " << port << " is set to a reserved port so using default value " << DefaultListenPort);
      port = DefaultListenPort;
   }

   return port;
}

int
StdioConfig::getSendBufferSize(void) const
{
   // First, check for command line argument.
   int bufsize = _variableMap["send_buffer_size"].as<int>();
   if (!_variableMap["send_buffer_size"].defaulted()) {
      LOG_CIOS_DEBUG_MSG("set send_buffer_size to " << bufsize << " from command line argument");
   }

   // Second, check for key in properties file.
   else {
      try {
         bufsize = boost::lexical_cast<int>( _properties->getValue("cios.stdiod", "send_buffer_size") );
         LOG_CIOS_DEBUG_MSG("set send_buffer_size to " << bufsize << " from properties file " << _properties->getFilename());
      }
      catch (const std::invalid_argument& e) {
         // This isn't fatal so we'll use the default value.
         LOG_CIOS_DEBUG_MSG("send_buffer_size not found in properties file '" << _properties->getFilename() << "' so using default value " << DefaultSendBufferSize);
      } 
      catch (const boost::bad_lexical_cast& e) {
         // Value is invalid.
         LOG_CIOS_WARN_MSG("send_buffer_size property from properties file '" << _properties->getFilename() << "' is invalid so using default value " <<
                      DefaultSendBufferSize << " (" << e.what() << ")");
      }
   }
   
   // Make sure the value is valid.
   if (bufsize < 65536) {
      LOG_CIOS_WARN_MSG("send_buffer_size property value " << bufsize << " is too small so using default value " << DefaultSendBufferSize);
      bufsize = DefaultSendBufferSize;
   }

   return bufsize;
}

