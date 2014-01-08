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

//! \file  IosConfig.cc
//! \brief Methods for bgcios::iosctl::IosConfig class.

// Includes
#include "IosConfig.h"
#include <ramdisk/include/services/common/RasEvent.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/logging.h>
#include <iostream>
#include <iterator>
#include <netinet/in.h>

using namespace bgcios::iosctl;

LOG_DECLARE_FILE("cios.iosd");

//! Default port number for data connection.
const uint16_t DefaultListenPort = 7001;

//! Default path to job control daemon.
const std::string DefaultJobctlDaemonPath = "/sbin/jobctld";

//! Default path to standard I/O daemon.
const std::string DefaultStdioDaemonPath = "/sbin/stdiod";

//! Default path to system I/O daemon.
const std::string DefaultSysioDaemonPath = "/sbin/sysiod";

//! Default path to tool manager daemon.
const std::string DefaultToolctlDaemonPath = "/sbin/toolctld";

//! Default maximum number of service restarts.
const uint16_t DefaultMaxServiceRestarts = 5;

IosConfig::IosConfig(int argc, char **argv) : bgcios::CiosConfig()
{
   // Create list of valid arguments.
   po::options_description desc("iosd options");
   desc.add_options()
      ("help,h", "display help text")
      ("listen_port", po::value<uint16_t>()->default_value(DefaultListenPort), "port number for control system connection")
      ("jobctl_daemon_path", po::value<std::string>()->default_value(DefaultJobctlDaemonPath), "path to job control daemon")
      ("stdio_daemon_path", po::value<std::string>()->default_value(DefaultStdioDaemonPath), "path to standard i/o daemon")
      ("sysio_daemon_path", po::value<std::string>()->default_value(DefaultSysioDaemonPath), "path to system i/o daemon")
      ("toolctl_daemon_path", po::value<std::string>()->default_value(DefaultToolctlDaemonPath), "path to toolctl daemon")
      ("max_service_restarts", po::value<uint16_t>()->default_value(DefaultMaxServiceRestarts), "maximum number of times a service is restarted")
      ("large_region_size", po::value<uint32_t>()->default_value(DefaultLargeRegionSize), "size in bytes for large message memory regions")
      ("num_large_regions", po::value<uint16_t>()->default_value(DefaultNumLargeRegions), "number of large message memory regions")
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
   try {
      if (_variableMap.count("properties")) {
         _properties = bgq::utility::Properties::create( _variableMap["properties"].as<std::string>() );
      }
      else {
         _properties = bgq::utility::Properties::create();
      }
   }
   catch (const std::exception& e) {
      std::cout << "Error with properties file: " << e.what() << std::endl;
      bgcios::RasEvent rasEvent(bgcios::DaemonStartFailed);
      rasEvent << getpid() << -1; // bogus errno
      rasEvent.send();
      exit(EXIT_FAILURE);
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
   log4cxx::LoggerPtr mylog = log4cxx::Logger::getLogger("ibm.cios.iosd");
   if (_variableMap.count("log_level")) {
      mylog->setLevel(log4cxx::Level::toLevel(_variableMap["log_level"].as<std::string>()));
   }

   // If logging level is set for daemon, set cios.common logger to match.
   if (mylog->getLevel() != NULL) {
      log4cxx::LoggerPtr commonLog = log4cxx::Logger::getLogger("ibm.cios.common");
      commonLog->setLevel(mylog->getLevel());
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
IosConfig::getListenPort(void) const
{
   // First, check for command line argument.
   uint16_t port = _variableMap["listen_port"].as<uint16_t>();
   if (!_variableMap["listen_port"].defaulted()) {
      LOG_CIOS_DEBUG_MSG("set listen_port to " << port << " from command line argument");
   }

   // Second, check for key in properties file.
   else {
      try {
         port = boost::lexical_cast<uint16_t>( _properties->getValue("cios.iosd", "listen_port") );
         LOG_CIOS_DEBUG_MSG("set listen_port to " << port << " from properties file " << _properties->getFilename());
      }
      catch (const std::invalid_argument& e) {
         // This isn't fatal so we'll use the default value.
         LOG_CIOS_DEBUG_MSG("listen_port property not found in properties file '" << _properties->getFilename() << "' so using default value " << DefaultListenPort);
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

std::string
IosConfig::getJobctlDaemonPath(void) const
{
   // First, check for command line argument.
   std::string path = _variableMap["jobctl_daemon_path"].as<std::string>();
   if (!_variableMap["jobctl_daemon_path"].defaulted()) {
      LOG_CIOS_DEBUG_MSG("set jobctl_daemon_path to '" << path << "' from command line argument");
   }

   // Second, check for key in properties file.
   else {
      try {
         path = boost::lexical_cast<std::string>( _properties->getValue("cios.iosd", "jobctl_daemon_path") );
         LOG_CIOS_DEBUG_MSG("set jobctl_daemon_path to '" << path << "' from properties file " << _properties->getFilename());
      }
      catch (const std::invalid_argument& e) {
         // This isn't fatal so we'll use the default value.
         LOG_CIOS_DEBUG_MSG("jobctl_daemon_path not found in properties file '" << _properties->getFilename() << "' so using default value " << DefaultJobctlDaemonPath);
      } 
      catch (const boost::bad_lexical_cast& e) {
         // Value is invalid.
         LOG_CIOS_WARN_MSG("jobctl_daemon_path property from properties file '" << _properties->getFilename() << "' is invalid so using default value " <<
                      DefaultJobctlDaemonPath << " (" << e.what() << ")");
      }
   }

   return path;
}

std::string
IosConfig::getStdioDaemonPath(void) const
{
   // First, check for command line argument.
   std::string path = _variableMap["stdio_daemon_path"].as<std::string>();
   if (!_variableMap["stdio_daemon_path"].defaulted()) {
      LOG_CIOS_DEBUG_MSG("set stdio_daemon_path to '" << path << "' from command line argument");
   }

   // Second, check for key in properties file.
   else {
      try {
         path = boost::lexical_cast<std::string>( _properties->getValue("cios.iosd", "stdio_daemon_path") );
         LOG_CIOS_DEBUG_MSG("set stdio_daemon_path to '" << path << "' from properties file " << _properties->getFilename());
      }
      catch (const std::invalid_argument& e) {
         // This isn't fatal so we'll use the default value.
         LOG_CIOS_DEBUG_MSG("stdio_daemon_path not found in properties file '" << _properties->getFilename() << "' so using default value " << DefaultStdioDaemonPath);
      } 
      catch (const boost::bad_lexical_cast& e) {
         // Value is invalid.
         LOG_CIOS_WARN_MSG("stdio_daemon_path property from properties file '" << _properties->getFilename() << "' is invalid so using default value " <<
                      DefaultStdioDaemonPath << " (" << e.what() << ")");
      }
   }

   return path;
}

std::string
IosConfig::getSysioDaemonPath(void) const
{
   // First, check for command line argument.
   std::string path = _variableMap["sysio_daemon_path"].as<std::string>();
   if (!_variableMap["sysio_daemon_path"].defaulted()) {
      LOG_CIOS_DEBUG_MSG("set sysio_daemon_path to '" << path << "' from command line argument");
   }

   // Second, check for key in properties file.
   else {
      try {
         path = boost::lexical_cast<std::string>( _properties->getValue("cios.iosd", "sysio_daemon_path") );
         LOG_CIOS_DEBUG_MSG("set sysio_daemon_path to '" << path << "' from properties file " << _properties->getFilename());
      }
      catch (const std::invalid_argument& e) {
         // This isn't fatal so we'll use the default value.
         LOG_CIOS_DEBUG_MSG("sysio_daemon_path not found in properties file '" << _properties->getFilename() << "' so using default value " << DefaultSysioDaemonPath);
      } 
      catch (const boost::bad_lexical_cast& e) {
         // Value is invalid.
         LOG_CIOS_WARN_MSG("sysio_daemon_path property from properties file '" << _properties->getFilename() << "' is invalid so using default value " <<
                      DefaultSysioDaemonPath << " (" << e.what() << ")");
      }
   }

   return path;
}

std::string
IosConfig::getToolctlDaemonPath(void) const
{
   // First, check for command line argument.
   std::string path = _variableMap["toolctl_daemon_path"].as<std::string>();
   if (!_variableMap["toolctl_daemon_path"].defaulted()) {
      LOG_CIOS_DEBUG_MSG("set toolctl_daemon_path to '" << path << "' from command line argument");
   }

   // Second, check for key in properties file.
   else {
      try {
         path = boost::lexical_cast<std::string>( _properties->getValue("cios.iosd", "toolctl_daemon_path") );
         LOG_CIOS_DEBUG_MSG("set toolctl_daemon_path to '" << path << "' from properties file " << _properties->getFilename());
      }
      catch (const std::invalid_argument& e) {
         // This isn't fatal so we'll use the default value.
         LOG_CIOS_DEBUG_MSG("toolctl_daemon_path not found in properties file '" << _properties->getFilename() << "' so using default value " << DefaultToolctlDaemonPath);
      } 
      catch (const boost::bad_lexical_cast& e) {
         // Value is invalid.
         LOG_CIOS_WARN_MSG("toolctl_daemon_path property from properties file '" << _properties->getFilename() << "' is invalid so using default value " <<
                      DefaultToolctlDaemonPath << " (" << e.what() << ")");
      }
   }

   return path;
}

uint16_t
IosConfig::getMaxServiceRestarts(void) const
{
   // First, check for command line argument.
   uint16_t maxServiceRestarts = _variableMap["max_service_restarts"].as<uint16_t>();
   if (!_variableMap["max_service_restarts"].defaulted()) {
      LOG_CIOS_DEBUG_MSG("set max_service_restarts to " << maxServiceRestarts << " from command line argument");
   }

   // Second, check for key in properties file.
   else {
      try {
         maxServiceRestarts = boost::lexical_cast<uint16_t>( _properties->getValue("cios.iosd", "max_service_restarts") );
         LOG_CIOS_DEBUG_MSG("set max_service_restarts to " << maxServiceRestarts << " from properties file " << _properties->getFilename());
      }
      catch (const std::invalid_argument& e) {
         // This isn't fatal so we'll use the default value.
         LOG_CIOS_DEBUG_MSG("max_service_restarts not found in properties file '" << _properties->getFilename() << "' so using default value " << DefaultMaxServiceRestarts);
      } 
      catch (const boost::bad_lexical_cast& e) {
         // Value is invalid.
         LOG_CIOS_WARN_MSG("max_service_restarts property from properties file '" << _properties->getFilename() << "' is invalid so using default value " <<
                      DefaultMaxServiceRestarts << " (" << e.what() << ")");
      }
   }

   return maxServiceRestarts;
}

