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

//! \file  SimulationManager.h
//! \brief Declaration and methods for bgcios::jobctl::SimulationManager class.

#ifndef JOBCTL_SIMULATIONMANAGER_H
#define JOBCTL_SIMULATIONMANAGER_H

// Includes
#include <ramdisk/include/services/ServicesConstants.h>
#include <tr1/memory>

namespace bgcios
{

namespace jobctl
{

//! \brief Manage control system simulator environment.

class SimulationManager
{
public:

   //! \brief  Default constructor.

   SimulationManager() :
      _simId( bgcios::SimulationDisabled ),
      _numRanks( 0 ),
      _startRank( 0 ),
      _np( 0 )
   {
   
   }

   //! \brief  Constructor.
   //! \param  simId Simulation id.

   explicit SimulationManager(uint16_t simId) :
      _simId( simId ),
      _numRanks( 1 ),
      _startRank( 0 ),
      _np( 0 ) 
   {
   
   }

   //! \brief  Check if simulation is enabled.
   //! \return True if simulation is enabled, false if simulation is disabled.

   bool isEnabled(void) const { return _simId == bgcios::SimulationDisabled ? false : true; }

   //! \brief  Get the id that identifies simulation session.
   //! \return Simulation id value.

   uint16_t getSimulationId(void) const { return _simId; }

   //! \brief  Set number of ranks to simulate.
   //! \param  ranks New value for number of ranks.
   //! \return Nothing.

   void setNumRanks(uint32_t ranks) { _numRanks = ranks; }

   //! \brief  Get number of ranks to simulate.
   //! \return Number of ranks.

   uint32_t getNumRanks(void) const { return _numRanks; }

   //! \brief  Set first simulated rank on this I/O node.
   //! \param  startRank New value for first rank.
   //! \return Nothing.

   void setStartRank(uint32_t startRank) { _startRank = startRank; }

   //! \brief  Get first simulated rank on this I/O node.
   //! \return Rank number.

   uint32_t getStartRank(void) const { return _startRank; }

   //! \brief  Set np value.
   //! \param  np New value for np.
   //! \return Nothing.
   
   void setNp( uint32_t np ) { _np = np; }

   //! \brief  Get np value.
   //! \return np.

   uint32_t getNp(void) const { return _np; }

private:

   //! Simulation id.
   const uint16_t _simId;

   //! Number of ranks to simulate.
   uint32_t _numRanks;

   //! First simulated rank on this I/O node.
   uint32_t _startRank;

   //! Np
   uint32_t _np;
};

//! Smart pointer for SimulationManager class.
typedef std::tr1::shared_ptr<SimulationManager> SimulationManagerPtr;

} // namespace jobctl

} // namespace bgcios

#endif // JOBCTL_SIMULATIONMANAGER_H
