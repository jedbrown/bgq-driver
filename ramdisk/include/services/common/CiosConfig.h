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

//! \file  CiosConfig.h
//! \brief Declaration and inline methods for bgcios::CiosConfig class.

#ifndef COMMON_CIOSCONFIG_H
#define COMMON_CIOSCONFIG_H

// Includes
#include <utility/include/Properties.h>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace bgcios
{

//! \brief  Base configuration for CIOS daemons.

class CiosConfig
{
public:

   // \brief  Default constructor.

   CiosConfig() : _variableMap(), _properties() { }

   //! \brief  Get the value of the simulation_id configuration variable.
   //! \return Unique id for simulation mode.

   uint16_t getSimulationId(void) const;

   //! \brief  Get the value of the large_region_size configuration variable.
   //! \return Size of large memory regions.

   size_t getLargeRegionSize(void) const;

   //! \brief  Get pointer to properties object.
   //! \return Properties object pointer.

   bgq::utility::Properties::ConstPtr getProperties(void) const { return _properties; }

protected:

   //! Map of configuration variables indexed by variable name.
   po::variables_map _variableMap;

   //! Properties file with configuration for daemon.
   bgq::utility::Properties::Ptr _properties;
};

} // namespace bgcios

#endif // COMMON_CIOSCONFIG_H

