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

//! \file  SysioConfig.cc
//! \brief Methods for bgcios::sysio::SysioConfig class.

// Includes
#include "SysioConfig.h"
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/logging.h>
#include <iostream>
#include <iterator>
#include <dlfcn.h>

using namespace bgcios::sysio;

LOG_DECLARE_FILE("cios.sysiod");

//! Default service id.
const uint32_t DefaultServiceId = 0;

//! Default path to short circuit file.
const std::string DefaultShortCircuitPath = "";

const uint64_t DefaultSlowSyscallTimeout = 300ull; // 5 minutes
const uint64_t DefaultHungSyscallTimeout = 900ull; // 15 minutes

SysioConfig::SysioConfig(int argc, char **argv) : bgcios::CiosConfig()
{
   // Create list of valid arguments.
   po::options_description desc("sysiod options");
   desc.add_options()
      ("help,h", "display help text")
      ("service_id", po::value<uint32_t>()->default_value(DefaultServiceId), "unique id for this instance of daemon")
      ("large_region_size", po::value<size_t>()->default_value(bgcios::DefaultLargeRegionSize), "size in bytes of large memory regions")
      ("log_job_statistics", po::bool_switch(), "log system I/O statistics from a job when it ends")
      ("short_circuit_path", po::value<std::string>()->default_value(DefaultShortCircuitPath), "path to short circuit file")
      ("posix_mode", po::bool_switch(), "run I/O operations using posix rules")
      ("log_function_ship_errors", po::bool_switch(), "log errors from function shipped operations")
      ("slow_syscall_timeout", po::value<uint64_t>()->default_value(DefaultSlowSyscallTimeout), "timeout in seconds that defines a slow system call")      
      ("hung_syscall_timeout", po::value<uint64_t>()->default_value(DefaultHungSyscallTimeout), "timeout in seconds that defines a hung system call")      
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
   std::string loggingSection = "logging.cios.sysiod";
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
   log4cxx::LoggerPtr mylog = log4cxx::Logger::getLogger("ibm.cios.sysiod");
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
SysioConfig::getServiceId(void) const                                               
{                                                                                   
   // First, check for command line argument.                                       
   uint32_t serviceId = _variableMap["service_id"].as<uint32_t>();                  
   if (!_variableMap["service_id"].defaulted()) {                                   
      LOG_CIOS_DEBUG_MSG("set service_id to " << serviceId << " from command line argument");
   }                                                                                   
                                                                                       
   // Second, check for key in properties file.                                        
   else {                                                                              
      try {                                                                            
         serviceId = boost::lexical_cast<uint32_t>( _properties->getValue("cios.sysiod", "service_id") );
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

bool
SysioConfig::getLogJobStatistics(void) const
{
   // First, check for command line argument.
   bool value = _variableMap["log_job_statistics"].as<bool>();
   if (!_variableMap["log_job_statistics"].defaulted()) {
      LOG_CIOS_DEBUG_MSG("set log_job_statistics to " << value << " from command line argument");
   }

   // Second, check for key in properties file.
   else {
      try {
         const std::string valueString = _properties->getValue("cios.sysiod", "log_job_statistics");
         std::istringstream str(valueString);
         str >> std::boolalpha >> value;
         LOG_CIOS_DEBUG_MSG("set log_job_statistics to " << value << " from properties file " << _properties->getFilename());
      }
      catch (const std::invalid_argument& e) {
         // This isn't fatal so we'll use the default value.
         LOG_CIOS_DEBUG_MSG("log_job_statistics not found in properties file '" << _properties->getFilename() << "' so using default value false");
      } 
      catch (const boost::bad_lexical_cast& e) {
         // Value is invalid.
         LOG_CIOS_WARN_MSG("log_job_statistics property from properties file '" << _properties->getFilename() << "' is invalid so using default value " <<
                      " false (" << e.what() << ")");
      }
   }

   return value;
}

std::string
SysioConfig::getShortCircuitPath(void) const
{
   // First, check for command line argument.
   std::string path = _variableMap["short_circuit_path"].as<std::string>();
   if (!_variableMap["short_circuit_path"].defaulted()) {
      LOG_CIOS_DEBUG_MSG("set short_circuit_path to '" << path << "' from command line argument");
   }

   // Second, check for key in properties file.
   else {
      try {
         path = boost::lexical_cast<std::string>( _properties->getValue("cios.sysiod", "short_circuit_path") );
         LOG_CIOS_DEBUG_MSG("set short_circuit_path to '" << path << "' from properties file " << _properties->getFilename());
      }
      catch (const std::invalid_argument& e) {
         // This isn't fatal so we'll use the default value.
         LOG_CIOS_DEBUG_MSG("short_circuit_path not found in properties file '" << _properties->getFilename() << "' so using default value '" << DefaultShortCircuitPath << "'");
      } 
      catch (const boost::bad_lexical_cast& e) {
         // Value is invalid.
         LOG_CIOS_WARN_MSG("short_circuit_path property from properties file '" << _properties->getFilename() << "' is invalid so using default value " <<
                      DefaultShortCircuitPath << " (" << e.what() << ")");
      }
   }

   return path;
}

bool
SysioConfig::getPosixMode(void) const
{
   // First, check for command line argument.
   bool value = _variableMap["posix_mode"].as<bool>();
   if (!_variableMap["posix_mode"].defaulted()) {
      LOG_CIOS_DEBUG_MSG("set posix_mode to " << value << " from command line argument");
   }

   // Second, check for key in properties file.
   else {
      try {
         const std::string valueString = _properties->getValue("cios.sysiod", "posix_mode");
         std::istringstream str(valueString);
         str >> std::boolalpha >> value;
         LOG_CIOS_DEBUG_MSG("set posix_mode to " << value << " from properties file " << _properties->getFilename());
      }
      catch (const std::invalid_argument& e) {
         // This isn't fatal so we'll use the default value.
         value = true;
         LOG_CIOS_DEBUG_MSG("posix_mode not found in properties file '" << _properties->getFilename() << "' so using default value true");
      } 
      catch (const boost::bad_lexical_cast& e) {
         // Value is invalid.
         value = true;
         LOG_CIOS_WARN_MSG("posix_mode property from properties file '" << _properties->getFilename() << "' is invalid so using default value " <<
                      " true (" << e.what() << ")");
      }
   }

   return value;
}

bool
SysioConfig::getLogFunctionShipErrors(void) const
{
   // First, check for command line argument.
   bool value = _variableMap["log_function_ship_errors"].as<bool>();
   if (!_variableMap["log_function_ship_errors"].defaulted()) {
      LOG_CIOS_DEBUG_MSG("set log_function_ship_errors to " << value << " from command line argument");
   }

   // Second, check for key in properties file.
   else {
      try {
         const std::string valueString = _properties->getValue("cios.sysiod", "log_function_ship_errors");
         std::istringstream str(valueString);
         str >> std::boolalpha >> value;
         LOG_CIOS_DEBUG_MSG("set log_function_ship_errors to " << value << " from properties file " << _properties->getFilename());
      }
      catch (const std::invalid_argument& e) {
         // This isn't fatal so we'll use the default value.
         value = false;
         LOG_CIOS_DEBUG_MSG("log_function_ship_errors not found in properties file '" << _properties->getFilename() << "' so using default value false");
      } 
      catch (const boost::bad_lexical_cast& e) {
         // Value is invalid.
         value = false;
         LOG_CIOS_WARN_MSG("log_function_ship_errors property from properties file '" << _properties->getFilename() << "' is invalid so using default value " <<
                      " false (" << e.what() << ")");
      }
   }

   return value;
}

const std::string 
SysioConfig::getDynamicLoadLibrary(void) const
{

   //  check for key in properties file.

      try {
         const std::string valueString = _properties->getValue("cios.sysiod", "sysiod_plugin");
         return valueString;
      }
      catch (const std::invalid_argument& e) {
          //LOG_CIOS_WARN_MSG("dynamic_library property from properties file '" << _properties->getFilename() << " invalid_argument=" << e.what() );
      } 
      catch (const boost::bad_lexical_cast& e) {
         LOG_CIOS_WARN_MSG("dynamic_library property from properties file '" << _properties->getFilename() << " bad_lexical_cast=" << e.what() );
      }
              
      const std::string valueString("");
      return valueString;   
}

int
SysioConfig::getFlags() const
{
    // default flags if none are found in bg.properties
    int result = RTLD_LAZY;

    const std::string key = "sysiod_plugin_flags";
    try {
        std::string value = _properties->getValue("cios.sysiod", key);
        if ( value.empty() ) {
            return result;
        }

        // strip off leading 0x if present
        if ( value.size() > 2 && value.substr(0,2) == "0x" ) {
            value.erase(0,2);
        }

        std::istringstream is( value );
        is >> std::hex >> result;
        if ( is.fail() || !is.eof() ) {
            LOG_WARN_MSG(
                    "garbage value for " << key << " key in [cios.sysiod] section: " <<
                    _properties->getValue( "cios.sysiod", key )
                    );

            result = RTLD_LAZY;
        }
    } catch ( const std::invalid_argument& e ) {
        //LOG_WARN_MSG( e.what() );
    }catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }

    return result;
}



uint64_t SysioConfig::getSlowSyscallTimeout(void) const
{
   // First, check for command line argument.
   uint64_t slowSyscallTimeout = _variableMap["slow_syscall_timeout"].as<uint64_t>();
   if (!_variableMap["slow_syscall_timeout"].defaulted()) {
      LOG_CIOS_DEBUG_MSG("set slow_syscall_timeout to " << slowSyscallTimeout << " from command line argument");
   }

   // Second, check for key in properties file.
   else {
      try {
         slowSyscallTimeout = boost::lexical_cast<uint64_t>( _properties->getValue("cios.sysiod", "slow_syscall_timeout") );
         LOG_CIOS_DEBUG_MSG("set slow_syscall_timeout to " << slowSyscallTimeout << " from properties file " << _properties->getFilename());
      }
      catch (const std::invalid_argument& e) {
         // This isn't fatal so we'll use the default value.
         LOG_CIOS_DEBUG_MSG("slow_syscall_timeout not found in properties file '" << _properties->getFilename() << "' so using default value " << DefaultSlowSyscallTimeout);
      } 
      catch (const boost::bad_lexical_cast& e) {
         // Value is invalid.
         LOG_CIOS_WARN_MSG("slow_syscall_timeout property from properties file '" << _properties->getFilename() << "' is invalid so using default value " <<
                      DefaultSlowSyscallTimeout << " (" << e.what() << ")");
      }
   }
   
   return slowSyscallTimeout;
}


uint64_t SysioConfig::getHungSyscallTimeout(void) const
{
   // First, check for command line argument.
   uint64_t hungSyscallTimeout = _variableMap["hung_syscall_timeout"].as<uint64_t>();
   if (!_variableMap["hung_syscall_timeout"].defaulted()) {
      LOG_CIOS_DEBUG_MSG("set hung_syscall_timeout to " << hungSyscallTimeout << " from command line argument");
   }

   // Second, check for key in properties file.
   else {
      try {
         hungSyscallTimeout = boost::lexical_cast<uint64_t>( _properties->getValue("cios.sysiod", "hung_syscall_timeout") );
         LOG_CIOS_DEBUG_MSG("set hung_syscall_timeout to " << hungSyscallTimeout << " from properties file " << _properties->getFilename());
      }
      catch (const std::invalid_argument& e) {
         // This isn't fatal so we'll use the default value.
	  LOG_CIOS_DEBUG_MSG("hung_syscall_timeout not found in properties file '" << _properties->getFilename() << "' so using default value " << DefaultHungSyscallTimeout );
      } 
      catch (const boost::bad_lexical_cast& e) {
         // Value is invalid.
         LOG_CIOS_WARN_MSG("hung_syscall_timeout property from properties file '" << _properties->getFilename() << "' is invalid so using default value " <<
                      DefaultHungSyscallTimeout << " (" << e.what() << ")");
      }
   }
   
   return hungSyscallTimeout;
}





