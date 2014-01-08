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

//! \file  CiosConfig.cc
//! \brief Methods for bgcios::CiosConfig class.

// Includes
#include <ramdisk/include/services/common/CiosConfig.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/logging.h>
#include <iostream>

using namespace bgcios;

LOG_DECLARE_FILE("cios.common");

uint16_t
CiosConfig::getSimulationId(void) const
{
   // First, check for command line argument.
   uint16_t simulationId = _variableMap["simulation_id"].as<uint16_t>();
   if (!_variableMap["simulation_id"].defaulted()) {
      LOG_CIOS_DEBUG_MSG("set simulation_id to " << simulationId << " from command line argument");
   }

   // Second, check for key in properties file.
   else {
      try {
         simulationId = boost::lexical_cast<uint16_t>( _properties->getValue("cios", "simulation_id") );
         LOG_CIOS_DEBUG_MSG("set simulation_id to " << simulationId << " from properties file " << _properties->getFilename());
      }
      catch (const std::invalid_argument& e) {
         // This isn't fatal so we'll use the default value.
         LOG_CIOS_DEBUG_MSG("simulation_id not found in properties file '" << _properties->getFilename() << "' so using default value " << DefaultSimulationId);
      } 
      catch (const boost::bad_lexical_cast& e) {
         // Value is invalid.
         LOG_CIOS_WARN_MSG("simulation_id property from properties file '" << _properties->getFilename() << "' is invalid so using default value " <<
                      DefaultSimulationId << " (" << e.what() << ")");
      }
   }

   return simulationId;
}

size_t
CiosConfig::getLargeRegionSize(void) const
{
   // First, check for command line argument.
   size_t regionSize = _variableMap["large_region_size"].as<size_t>();
   if (!_variableMap["large_region_size"].defaulted()) {
      LOG_CIOS_DEBUG_MSG("set large_region_size to " << regionSize << " from command line argument");
   }

   // Second, check for key in properties file.
   else {
      try {
         regionSize = boost::lexical_cast<size_t>( _properties->getValue("cios", "large_region_size") );
         LOG_CIOS_DEBUG_MSG("set large_region_size to " << regionSize << " from properties file " << _properties->getFilename());
      }
      catch (const std::invalid_argument& e) {
         // This isn't fatal so we'll use the default value.
         LOG_CIOS_DEBUG_MSG("large_region_size not found in properties file '" << _properties->getFilename() << "' so using default value " << DefaultLargeRegionSize);
      } 
      catch (const boost::bad_lexical_cast& e) {
         // Value is invalid.
         LOG_CIOS_WARN_MSG("large_region_size property from properties file '" << _properties->getFilename() << "' is invalid so using default value " <<
                      DefaultLargeRegionSize << " (" << e.what() << ")");
      }
   }

   if ((regionSize < MinLargeRegionSize) || (regionSize > MaxLargeRegionSize)) {
      LOG_CIOS_WARN_MSG("large_region_size property value " << regionSize << " is invalid so using default value " << DefaultLargeRegionSize);
      regionSize = DefaultLargeRegionSize;
   }

   return regionSize;
}

