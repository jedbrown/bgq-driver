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

//! \file  JobctlConfig.cc
//! \brief Methods for bgcios::jobctl::JobctlConfig class.

// Includes
#include "JobctlConfig.h"
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/logging.h>
#include <iostream>
#include <iterator>
#include <netinet/in.h>

using namespace bgcios::jobctl;

LOG_DECLARE_FILE("cios.jobctld");

//! Default port number for data connection.
const uint16_t DefaultListenPort = 7002;

//! Default path to job prolog program.
const std::string DefaultJobPrologProgramPath = "";

//! Default number of seconds to wait for job prolog program to complete.
const int32_t DefaultJobPrologProgramTimeout = -1;

//! Default path to job epilog program.
const std::string DefaultJobEpilogProgramPath = "";

//! Default number of seconds to wait for job epilog program to complete.
const int32_t DefaultJobEpilogProgramTimeout = -1;

//! Default number of seconds to wait for a tool to end.
const int32_t DefaultEndToolTimeout = 10;

//! Default number of seconds between heartbeats.
const int32_t DefaultHeartbeatTimeout = 60;

//! Default number of seconds for start time threshold.
const int32_t DefaultStartTimeThreshold = 0;


JobctlConfig::JobctlConfig(int argc, char **argv) : bgcios::CiosConfig()
{
   // Create list of valid arguments.
   po::options_description desc("jobctld options");
   desc.add_options()
      ("help,h", "display help text")
      ("listen_port", po::value<uint16_t>()->default_value(DefaultListenPort), "port number for control system connection")
      ("job_prolog_program_path", po::value<std::string>()->default_value(DefaultJobPrologProgramPath), "path to prolog program to run before a job is started")
      ("job_epilog_program_path", po::value<std::string>()->default_value(DefaultJobEpilogProgramPath), "path to epilog program to run after a job has ended")
      ("job_prolog_program_timeout", po::value<int32_t>()->default_value(DefaultJobPrologProgramTimeout), "number of seconds to wait for job prolog program to complete")
      ("job_epilog_program_timeout", po::value<int32_t>()->default_value(DefaultJobEpilogProgramTimeout), "number of seconds to wait for job epilog program to complete")
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
   std::string loggingSection = "logging.cios.jobctld";
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
   log4cxx::LoggerPtr mylog = log4cxx::Logger::getLogger("ibm.cios.jobctld");
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

uint16_t
JobctlConfig::getListenPort(void) const
{
   // First, check for command line argument.
   uint16_t port = _variableMap["listen_port"].as<uint16_t>();
   if (!_variableMap["listen_port"].defaulted()) {
      LOG_CIOS_DEBUG_MSG("set listen_port to " << port << " from command line argument");
   }

   // Second, check for key in properties file.
   else {
      try {
         port = boost::lexical_cast<uint16_t>( _properties->getValue("cios.jobctld", "listen_port") );
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

const std::string
JobctlConfig::getJobPrologProgramPath(void) const
{
   // First, check for command line argument.
   std::string path = _variableMap["job_prolog_program_path"].as<std::string>();
   if (!_variableMap["job_prolog_program_path"].defaulted()) {
      LOG_CIOS_DEBUG_MSG("set job_prolog_program_path to '" << path << "' from command line argument");
   }

   // Second, check for key in properties file.
   else {
      try {
         path = boost::lexical_cast<std::string>( _properties->getValue("cios.jobctld", "job_prolog_program_path") );
         LOG_CIOS_DEBUG_MSG("set job_prolog_program_path to '" << path << "' from properties file " << _properties->getFilename());
      }
      catch (const std::invalid_argument& e) {
         // This isn't fatal so we'll use the default value.
         LOG_CIOS_DEBUG_MSG("job_prolog_program_path not found in properties file '" << _properties->getFilename() << "' so using default value '" << DefaultJobPrologProgramPath << "'");
      } 
      catch (const boost::bad_lexical_cast& e) {
         // Value is invalid.
         LOG_CIOS_WARN_MSG("job_prolog_program_path property from properties file '" << _properties->getFilename() << "' is invalid so using default value " <<
                      DefaultJobPrologProgramPath << " (" << e.what() << ")");
      }
   }

   return path;
}

const std::string
JobctlConfig::getJobEpilogProgramPath(void) const
{
   // First, check for command line argument.
   std::string path = _variableMap["job_epilog_program_path"].as<std::string>();
   if (!_variableMap["job_epilog_program_path"].defaulted()) {
      LOG_CIOS_INFO_MSG("set job_epilog_program_path to '" << path << "' from command line argument");
   }

   // Second, check for key in properties file.
   else {
      try {
         path = boost::lexical_cast<std::string>( _properties->getValue("cios.jobctld", "job_epilog_program_path") );
         LOG_CIOS_DEBUG_MSG("set job_epilog_program_path to '" << path << "' from properties file " << _properties->getFilename());
      }
      catch (const std::invalid_argument& e) {
         // This isn't fatal so we'll use the default value.
         LOG_CIOS_DEBUG_MSG("job_epilog_program_path not found in properties file '" << _properties->getFilename() << "' so using default value '" << DefaultJobEpilogProgramPath << "'");
      } 
      catch (const boost::bad_lexical_cast& e) {
         // Value is invalid.
         LOG_CIOS_WARN_MSG("job_epilog_program_path property from properties file '" << _properties->getFilename() << "' is invalid so using default value " <<
                      DefaultJobEpilogProgramPath << " (" << e.what() << ")");
      }
   }

   return path;
}

int32_t
JobctlConfig::getJobPrologProgramTimeout(void) const
{
   // First, check for command line argument.
   int32_t timeout = _variableMap["job_prolog_program_timeout"].as<int32_t>();
   if (!_variableMap["job_prolog_program_timeout"].defaulted()) {
      LOG_CIOS_DEBUG_MSG("set job_prolog_program_timeout to " << timeout << " from command line argument");
   }

   // Second, check for key in properties file.
   else {
      try {
         timeout = boost::lexical_cast<int32_t>( _properties->getValue("cios.jobctld", "job_prolog_program_timeout") );
         LOG_CIOS_DEBUG_MSG("set job_prolog_program_timeout to " << timeout << " from properties file " << _properties->getFilename());
      }
      catch (const std::invalid_argument& e) {
         // This isn't fatal so we'll use the default value.
         LOG_CIOS_DEBUG_MSG("job_prolog_program_timeout not found in properties file '" << _properties->getFilename() << "' so using default value " << DefaultJobPrologProgramTimeout);
      } 
      catch (const boost::bad_lexical_cast& e) {
         // Value is invalid.
         LOG_CIOS_WARN_MSG("job_prolog_program_timeout property from properties file '" << _properties->getFilename() << "' is invalid so using default value " <<
                      DefaultJobPrologProgramTimeout << " (" << e.what() << ")");
      }
   }
   
   return timeout;
}

int32_t
JobctlConfig::getJobEpilogProgramTimeout(void) const
{
   // First, check for command line argument.
   int32_t timeout = _variableMap["job_epilog_program_timeout"].as<int32_t>();
   if (!_variableMap["job_epilog_program_timeout"].defaulted()) {
      LOG_CIOS_DEBUG_MSG("set job_epilog_program_timeout to " << timeout << " from command line argument");
   }

   // Second, check for key in properties file.
   else {
      try {
         timeout = boost::lexical_cast<int32_t>( _properties->getValue("cios.jobctld", "job_epilog_program_timeout") );
         LOG_CIOS_DEBUG_MSG("set job_epilog_program_timeout to " << timeout << " from properties file " << _properties->getFilename());
      }
      catch (const std::invalid_argument& e) {
         // This isn't fatal so we'll use the default value.
         LOG_CIOS_DEBUG_MSG("job_epilog_program_timeout not found in properties file '" << _properties->getFilename() << "' so using default value " << DefaultJobEpilogProgramTimeout);
      } 
      catch (const boost::bad_lexical_cast& e) {
         // Value is invalid.
         LOG_CIOS_WARN_MSG("job_epilog_program_timeout property from properties file '" << _properties->getFilename() << "' is invalid so using default value " <<
                      DefaultJobEpilogProgramTimeout << " (" << e.what() << ")");
      }
   }
   
   return timeout;
}

int32_t
JobctlConfig::getHeartbeatTimeout(void) const
{
    int32_t timeout = DefaultHeartbeatTimeout;
    try {
        timeout = boost::lexical_cast<int32_t>( _properties->getValue("runjob.server", "jobctl_heartbeat") );
    } catch (const boost::bad_lexical_cast& e) {
         LOG_CIOS_WARN_MSG(
                 "jobctl_heartbeat property is invalid, using default value " << 
                 DefaultHeartbeatTimeout << " (" << e.what() << ")"
                 );
    } catch (const std::exception& e) {
         // not found, or invalid. Use default.
    }

    return timeout;
}

uint32_t
JobctlConfig::getStartTimeThreshold(void) const
{
    int32_t timeout = DefaultStartTimeThreshold;
    try {
        timeout = boost::lexical_cast<int32_t>( _properties->getValue("cios.jobctld", "start_time_threshold") );
        if ( timeout < 0 ) {
            timeout = DefaultStartTimeThreshold;
            LOG_CIOS_WARN_MSG("start_time_threshold property is invalid, using default value " << timeout );
        }
    } catch (const boost::bad_lexical_cast& e) {
         LOG_CIOS_WARN_MSG(
                 "start_time_threshold property is invalid, using default value " << 
                 DefaultStartTimeThreshold << " (" << e.what() << ")"
                 );
    } catch (const std::exception& e) {
         // not found, or invalid. Use default.
    }

    return static_cast<uint32_t>(timeout);
}

