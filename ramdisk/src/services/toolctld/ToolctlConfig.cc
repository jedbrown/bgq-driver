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

//! \file  ToolctlConfig.cc
//! \brief Methods for bgcios::toolctl::ToolctlConfig class.

// Includes
#include "ToolctlConfig.h"
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/logging.h>
#include <iostream>
#include <iterator>

using namespace bgcios::toolctl;

LOG_DECLARE_FILE("cios.toolctld");

//! Default service id.
const uint32_t DefaultServiceId = 0;


ToolctlConfig::ToolctlConfig(int argc, char **argv) : bgcios::CiosConfig()
{
   // Create list of valid arguments.
   po::options_description desc("toolctld options");
   desc.add_options()
      ("help,h", "display help text")
      ("service_id", po::value<uint32_t>()->default_value(DefaultServiceId), "unique id for this instance of daemon")
      ("log_level", po::value<std::string>(), "logging level")
      ("properties", po::value<std::string>(), "path to properties file")
      ("CNtorus",po::value<std::string>(), "torus coordinates")
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
   log4cxx::LoggerPtr mylog = log4cxx::Logger::getLogger("ibm.cios");
   if (_variableMap.count("log_level")) {
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

uint32_t
ToolctlConfig::getServiceId(void) const
{
   // First, check for command line argument.
   uint32_t serviceId = _variableMap["service_id"].as<uint32_t>();
   if (!_variableMap["service_id"].defaulted()) {
      LOG_CIOS_DEBUG_MSG("set service_id to " << serviceId << " from command line argument");
   }

   // Second, check for key in properties file.
   else {
      try {
         serviceId = boost::lexical_cast<uint32_t>( _properties->getValue("cios.toolctld", "service_id") );
         LOG_CIOS_DEBUG_MSG("set service_id to " << serviceId << " from properties file " << _properties->getFilename());
      }
      catch (const std::invalid_argument& e) {
         // This isn't fatal so we'll use the default value.
         LOG_CIOS_DEBUG_MSG("service_id not found in properties file '" << _properties->getFilename() << "' so using default value " << DefaultServiceId);
      } 
      catch (const boost::bad_lexical_cast& e) {
         // Value is invalid.
         LOG_CIOS_WARN_MSG("service_id property from properties file '" << _properties->getFilename() << "' is invalid so using default value " <<
                      DefaultServiceId << " (" << e.what() << ")");
      }
   }

   return serviceId;
}

