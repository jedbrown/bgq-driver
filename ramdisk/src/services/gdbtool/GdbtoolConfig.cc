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

//! \file  GdbtoolConfig.cc
//! \brief Methods for bgcios::gdbtool::GdbtoolConfig class.

// Includes
#include "GdbtoolConfig.h"
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/logging.h>
#include <iostream>
#include <iterator>

using namespace bgcios::gdbtool;

LOG_DECLARE_FILE("cios.gdbtool");

//! Default port number for listening connection.
const uint16_t DefaultListenPort = 10000;

//! Default rank of process to debug.
const uint32_t DefaultRank = 0;

GdbtoolConfig::GdbtoolConfig(int argc, char **argv) : bgcios::CiosConfig()
{
   // Create list of valid arguments.
   po::options_description desc("gdbtool options");
   desc.add_options()
      ("help,h", "display help text")
      ("listen_port", po::value<uint16_t>()->default_value(DefaultListenPort), "port number for listening connection")
      ("rank", po::value<uint32_t>()->default_value(DefaultRank), "rank of process to debug")
      ("log_level", po::value<std::string>(), "logging level")
      ("properties", po::value<std::string>(), "path to properties file")
      ;

   // Create list of positional arguments.
   po::positional_options_description pd;
   pd.add("listen_port", 1).add("rank", 2);

   // Parse the command line options.
   try {
      po::store(po::command_line_parser(argc, argv).options(desc).positional(pd).run(), _variableMap);
      po::notify(_variableMap);
   }
   catch (const boost::program_options::error& e) {
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
   bool loggingSection = true;
   try {
      (void)_properties->getValues("logging.cios.gdbtool");
   }
   catch (const std::invalid_argument& e) {
      loggingSection = false;
   }

   // Initialize logging for the daemon.
   if (loggingSection) {
      bgq::utility::initializeLogging(*_properties, "cios.gdbtool");
   }
   else {
      bgq::utility::initializeLogging(*_properties);
   }

   // If specified, override the logging level.
   log4cxx::LoggerPtr mylog = log4cxx::Logger::getLogger("ibm.cios.gdbtool");
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
   LOG_DEBUG_MSG("startup parameters: " << args.str());
}

uint16_t
GdbtoolConfig::getListenPort(void) const
{
   return _variableMap["listen_port"].as<uint16_t>();
}

uint32_t
GdbtoolConfig::getRank(void) const
{
   return _variableMap["rank"].as<uint32_t>();
}
