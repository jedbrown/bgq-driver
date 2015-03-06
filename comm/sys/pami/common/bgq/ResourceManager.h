/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q 5765-PER 5765-PRP                                    */
/*                                                                  */
/* (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/* US Government Users Restricted Rights -                          */
/* Use, duplication, or disclosure restricted                       */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

////////////////////////////////////////////////////////////////////////////////
///
/// \file common/bgq/ResourceManager.h
///
/// \brief PAMI Resource Manager Definitions
///
/// The PAMI Resource Manager is a common class that controls resources
/// common to all PAMI components.
///
/// Current implementation uses environment variables to specify the resource
/// configuration.  Future implementation may use a file or some other means.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef __common_bgq_ResourceManager_h__
#define __common_bgq_ResourceManager_h__

#ifdef __FWEXT__

#include <firmware/include/fwext/fwext.h>
#include <firmware/include/Firmware.h>
#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <firmware/include/fwext/fwext_lib.h>
#include <firmware/include/fwext/fwext_nd.h>
#include <firmware/include/fwext/fwext_mu.h>

#endif // __FWEXT__

#include <stdio.h>
#include <stdlib.h>
#include <hwi/include/bqc/MU_Macros.h>
#include <spi/include/kernel/process.h>

#ifdef TRACE
#undef TRACE
#endif
#define TRACE(x) //fprintf x


////////////////////////////////////////////////////////////////////////////////
/// \env{pami,PAMI_CLIENTS}
/// A comma-separated ordered list of clients (no spaces).
/// The complete syntax is <tt> [name][:repeat][/weight][,[name][:repeat][/weight]]* </tt>
/// 
/// Each client has the form <tt>[name][:repeat][/weight]</tt>, where
/// - "name" is the name of the client.  For example, the BGQ MPICH2 client's
///   name is MPI.  The default value for this option is the null string.
/// - ":repeat" is the repetition factor, where repeat is the number of clients
///   having this same name.  The default value for this option is 1.
/// - "/weight" is the relative weight assigned to the client, where weight is
///   the weight value.  The default value for this option is 1.  The weight is
///   used to determine the portion of the messaging resources that are given to
///   the client, relative to the other clients.
///
/// When middleware calls PAMI_Client_create(), it provides the client's name.
/// PAMI searches through the PAMI_CLIENTS in the order they are specified,
/// looking for an exact name match.  If there is not an exact name match with
/// any of the PAMI_CLIENTS, PAMI searches through the PAMI_CLIENTS again,
/// looking for a client with a null name string.  The null name string is a
/// wildcard, and matches any client name.  If there are exact or wildcard name
/// matches, the first match that doesn't already have an active client is used,
/// and that client's weight determines the percentage of the available
/// resources that are allocated to the client.  If there are no available
/// and matching clients, the PAMI client is not created.
/// 
/// The default value of the PAMI_CLIENTS environment variable is ":1/1" which
/// means that all resources are assigned to the first client created, regardless
/// of the client name, and all subsequent attempts to create a client will fail
/// due to insufficient resources.
///
/// If any of the clients specified on PAMI_CLIENTS are unnamed, or more than
/// one client has the same name, the order in which the clients are created
/// must be the same on all processes in the job.
///   
/// The first client listed has exclusive use of the message unit combining
/// collective hardware for optimizing reduction operations.  The other clients
/// will use algorithms that do not use the message unit combining collective
/// hardware.
///
/// Examples:
/// - "PAMI_CLIENTS=MPI,ARMCI" means up to two clients can use PAMI, one must
///   be MPI and the other must be ARMCI.  The MPI client is assigned the
///   message unit combining collective hardware, and the two clients evenly
///   split the remaining messaging resources.
/// - "PAMI_CLIENTS=MPI:3,ARMCI/2,UPC:2/3" means up to seven clients can use
///   PAMI.  Three can be MPI, one can be ARMCI, and two can be UPC.
///   Each MPI client has weight 1, ARMCI has weight 2, and each UPC client
///   has weight 3.  In this example each UPC client gets 3 times the amount of
///   resources as each MPI client, and the first MPI client created is
///   assigned the message unit combining collective hardware.
/// - "PAMI_CLIENTS=MPI/3,/2," means up to three clients can use PAMI.  Two of
///   the clients are unnamed, meaning they can be any of the PAMI clients, and
///   one client can only be MPI.  The first MPI client created has
///   resource weight 3 and is assigned the message unit combining collective
///   hardware, the first non-MPI client created (or possibly the
///   second MPI client created) has resource weight 2, and the second
///   non-MPI client created (or possibly the second or third MPI
///   client created) has resource weight 1.
/// - "PAMI_CLIENTS" is not specified.  This means there can only be one
///   client, with any name, and it is assigned all of the resources.
///
/// \default :1/1
///
/// \note PAMI uses one reception FIFO per context and, optimally, uses 10
///       injection FIFOs per context, although fewer injection FIFOs could be
///       used when resources are constrained.
///
/// \see \ref MUSPI_NUMBATIDS
/// \see \ref MUSPI_NUMCLASSROUTES
/// \see \ref MUSPI_NUMINJFIFOS
/// \see \ref MUSPI_NUMRECFIFOS
/// \see \ref MUSPI_INJFIFOSIZE
/// \see \ref MUSPI_RECFIFOSIZE
/// \see \ref PAMI_MU_RESOURCES
///
/// \env{mudevice,PAMI_RGETINJFIFOSIZE}
/// The size, in bytes, of each remote get FIFO.  These
/// FIFOs store 64-byte descriptors, each describing a memory buffer to be
/// sent on the torus, and are used to queue requests for data (remote gets).
/// Making this larger can reduce torus network congestion and reduce overhead.
/// Making this smaller can increase that congestion and overhead.
/// PAMI Messaging uses 10 remote get FIFOs per node.
/// \default 65536


namespace PAMI
{
  const size_t numSpiUserInjFifosPerProcessDefault = 0;
  const size_t numSpiUserRecFifosPerProcessDefault = 0;
  const size_t numSpiUserBatIdsPerProcessDefault   = 0;
  const size_t numClientsDefault         = 1;
  const char   defaultClientName[]       = "";  // Null string indicates no name specified.
  const char   defaultClientWeight       = 1;
  const char   defaultClientRepeat       = 1;
  const size_t rgetInjFifoSizeDefault    = 65536;
  const size_t injFifoSizeDefault        = 65536;
  const size_t recFifoSizeDefault        = 1024*1024;

  class ResourceManager
  {
    public:

      typedef enum clientStatus
      {
	RESOURCE_MANAGER_CLIENT_STATUS_DEALLOCATED = 0,
	RESOURCE_MANAGER_CLIENT_STATUS_ALLOCATED
      } clientStatus_t;

      //////////////////////////////////////////////////////////////////////////
      ///
      /// \brief PAMI Resource Manager Default Constructor
      ///
      /////////////////////////////////////////////////////////////////////////
      ResourceManager ()
	{
	  getConfig();

	} // End: ResourceManager Default Constructor

	inline ~ResourceManager() {
	}

      inline size_t getNumSpiUserInjFifosPerProcess() { return _numSpiUserInjFifosPerProcess; }

      inline size_t getNumSpiUserRecFifosPerProcess() { return _numSpiUserRecFifosPerProcess; }

      inline size_t getNumSpiUserBatIdsPerProcess()   { return _numSpiUserBatIdsPerProcess;   }

      inline size_t getNumClients()         { return _numClients; }

      inline const char *getClientName( size_t RmClientId ) { return _clientNamesPtrs[RmClientId]; }

      inline size_t getClientWeight( size_t RmClientId )    { return _clientWeights[RmClientId]; }

      /// \brief Allocate a Client With The Specified Name
      ///
      /// \param[in]  clientName  Pointer to the null-terminated client name
      /// \param[out] clientId    Pointer to a size_t where the client ID is returned.
      ///
      /// \retval  PAMI_SUCCESS  The client ID is returned.
      /// \retval  PAMI_INVAL    Failure.  The client is not found.
      ///
      inline pami_result_t allocateClient( char   *clientName,
					   size_t *clientId );

      /// \brief Deallocate a Client With The Specified ID
      ///
      /// \param[in]  clientId  The ID of the client to be deallocated
      ///
      /// \retval PAMI_SUCCESS  The client has been deallocated
      /// \retval PAMI_INVAL    The client is invalid, e.g. already deallocated
      ///
      inline pami_result_t deallocateClient( size_t clientId );

      /// \brief Return Whether The Specified Client Can Use MU Hardware Optimization
      ///        for Combining Collectives
      ///
      /// The first client specified on the PAMI_CLIENTS env var can use MU hardware
      /// optmization for combining collectives.  The other clients cannot.
      ///
      inline bool   doesClientOptimizeCombiningCollectivesInMU( size_t RmClientId )
      {
	if ( RmClientId == 0 )
	  return true;
	else
	  return false;
      }

      inline size_t getRgetInjFifoSize() { return _rgetInjFifoSize; }

      inline size_t getInjFifoSize() { return _injFifoSize; }

      inline size_t getRecFifoSize() { return _recFifoSize; }

      inline size_t getNumProcessesPerNode() { return _numProcessesPerNode; }

      inline size_t getNumCoresPerProcess() { return _numCoresPerProcess; }

      inline size_t getNumProcessesPerCore() { return _numProcessesPerCore; }

      inline size_t getNumHWThreadsPerProcess() { return _numHWThreadsPerProcess; }

      inline size_t getNumHWThreadsPerProcessPerCore() { return _numHWThreadsPerProcessPerCore; }

    private:

      inline unsigned long getConfigValueSize_t( const char *configName,
						 size_t      defaultValue );

      inline char * getConfigValueString( const char *configName );

      inline void   getClients( const char *clientsConfigValueString );

      inline size_t getNumClientsInternal();

      inline void   initClientStatus();

      inline void   getConfig();

      inline void   getProcessInfo();

      //////////////////////////////////////////////////////////////////////////
      ///
      /// Member data:
      ///
      /////////////////////////////////////////////////////////////////////////

      size_t       _numSpiUserInjFifosPerProcess;
      size_t       _numSpiUserRecFifosPerProcess;
      size_t       _numSpiUserBatIdsPerProcess;
      size_t       _numClients;
      char        *_clientsConfigValueStringCopy;
      const char **_clientNamesPtrs;
      size_t      *_clientWeights;
      clientStatus_t *_clientStatus;
      size_t       _rgetInjFifoSize;
      size_t       _injFifoSize;
      size_t       _recFifoSize;
      size_t       _numProcessesPerNode;
      size_t       _numCoresPerProcess;
      size_t       _numProcessesPerCore;
      size_t       _numHWThreadsPerProcess;
      size_t       _numHWThreadsPerProcessPerCore;

  }; // ResourceManager class

};       // PAMI namespace


size_t PAMI::ResourceManager::getConfigValueSize_t( const char    *configName,
						    size_t  defaultValue )
{
  char *configValueString;
  unsigned long outputValue;

#ifdef __FWEXT__

  configValueString = fwext_getenv( configName );

#else // Not firmware extension

  configValueString = getenv( configName );

#endif

  if (configValueString)
    {

#ifdef __FWEXT__

      outputValue = fwext_strtoul( configValueString, 0, 10 );

#else

      outputValue = strtoul( configValueString, 0, 10 );

#endif

      TRACE((stderr,"PAMI ResourceManager: Overriding %s = %lu\n",configName,outputValue));
    }
  else
    {
      outputValue = defaultValue;
      TRACE((stderr,"PAMI ResourceManager: Using default for %s = %lu\n",configName,outputValue));
    }

  return (size_t)outputValue;

} // End: getConfigValueSize_t()


char * PAMI::ResourceManager::getConfigValueString( const char *configName )
{
  char *outputString;

#ifdef __FWEXT__

  outputString = fwext_getenv( configName );

#else // Not firmware extension

  outputString = getenv( configName );

#endif

  return outputString;

} // End: getConfigValueString()


void PAMI::ResourceManager::getClients( const char *clientsConfigValueString )
{
  size_t       i;

  // Allocate space for _numClients pointers, to point to the client names.
  pami_result_t prc;
  prc = __global.heap_mm->memalign((void **)&_clientNamesPtrs, 0,
				   _numClients * sizeof(*_clientNamesPtrs));
  PAMI_assertf(prc == PAMI_SUCCESS, "alloc of _clientNamesPtrs failed");

  // Allocate space for _numClients weights.
  prc = __global.heap_mm->memalign((void **)&_clientWeights, 0,
				_numClients * sizeof(*_clientWeights));
  PAMI_assertf(prc == PAMI_SUCCESS, "alloc of _clientWeights failed");

  // If the PAMI_CLIENTS env var is not specified, fill with the default values.
  if ( clientsConfigValueString == NULL )
    {
      for (i=0; i<_numClients; i++)
	{
	  _clientNamesPtrs[i] = defaultClientName;
	  _clientWeights[i]   = defaultClientWeight;
	}
    }
  else // PAMI_CLIENTS env var specified.
    {
      // Find out how long the env var string is, so we can make a copy of it
      unsigned int configValueStringLen = 0;
      while ( clientsConfigValueString[configValueStringLen++] != '\0' );

      // Allocate space for a copy of the comma-delimited list of values
      pami_result_t prc;
      prc = __global.heap_mm->memalign((void **)&_clientsConfigValueStringCopy, 0,
						configValueStringLen);
      PAMI_assertf(prc == PAMI_SUCCESS, "alloc of _clientsConfigValueStringCopy failed");

      // Copy the env var string into our writeable copy
      for ( i=0; i<configValueStringLen; i++ )
	_clientsConfigValueStringCopy[i] = clientsConfigValueString[i];
      
      char *currentChar = _clientsConfigValueStringCopy;
      size_t client = 0;
      int nameLen;

      for ( ; client < _numClients ; )
	{
	  char delimiter;
	  unsigned int r = defaultClientRepeat; // Repeat factor
	  unsigned int w = defaultClientWeight; // Weight
	  const char *clientNamePtr;

	  // Handle the client name.
	  clientNamePtr = currentChar; // Point to first character of the name.
	  nameLen = 0;
	  while ( ( *currentChar != ','  ) &&
		  ( *currentChar != ':'  ) &&
		  ( *currentChar != '/'  ) &&
		  ( *currentChar != '\0' ) )
	    {
	      currentChar++;
	      nameLen++;
	    }
	  if ( nameLen == 0 ) // Null name?  Point this client's name to the default (null) name.
	    clientNamePtr = defaultClientName;
	  delimiter = *currentChar; // Save the delimiter that we just hit.
	  *currentChar = '\0';      // Null terminate the name.

	  // Handle the :r and /w specifications.
	  while (  ( delimiter == ':' ) || ( delimiter == '/' ) )
	    {
	      // Handle the :r specification
	      if ( delimiter == ':' )
		{
		  r = defaultClientRepeat; // Init to default, in case r value not specified.
		  currentChar++; // Skip past the ':'
		  char *rPtr = currentChar;
		  while ( ( *currentChar != ','  ) &&
			  ( *currentChar != ':'  ) &&
			  ( *currentChar != '/'  ) &&
			  ( *currentChar != '\0' ) )
		    {
		      currentChar++;
		    }
		  delimiter = *currentChar; // Save ending delimiter.
		  if ( *rPtr != delimiter )
		    {
		      *currentChar = '\0'; // Replace it with a null delimiter.
		      r = atoi( rPtr );    // If the value string is null, just use default.
		    }
		}
	      else
		{
		  // Handle the :w specification
		  if ( delimiter == '/' )
		    {
		      w = defaultClientWeight; // Init to default, in case w value not specified.
		      currentChar++; // Skip past the '/'
		      char *wPtr = currentChar;
		      while ( ( *currentChar != ','  ) &&
			      ( *currentChar != ':'  ) &&
			      ( *currentChar != '/'  ) &&
			      ( *currentChar != '\0' ) )
			{
			  currentChar++;
			}
		      delimiter = *currentChar; // Save ending delimiter.
		      if ( *wPtr != delimiter ) 
			{
			  *currentChar = '\0';      // Replace it with a null delimiter.
			  w = atoi( wPtr ); // If the value string is null, just use default.
			}
		    }
		}
	    }

	  // We are done with the name, r, and w specifications.
	  // Process the repetition, saving the name ptr and w for each repeated client.
	  for ( i=0; i<r; i++ )
	    {
	      _clientNamesPtrs[client] = clientNamePtr;
	      _clientWeights[client]   = w;
	      TRACE((stderr,"GetClients: Client %zu name=%s, w=%u\n",client,clientNamePtr,w));
	      client++;
	    }

	  if ( delimiter == '\0' ) break;
	  currentChar++;
	}
    }

  // Change the client weights to percentages.
  size_t sum=0;
  for ( i=0; i<_numClients; i++ ) // Sum the weights.
    {
      sum += _clientWeights[i];
    }
  for ( i=0; i<_numClients; i++ ) // Convert to percentages.
    {
      _clientWeights[i] = _clientWeights[i] * 100 / sum;
    }
  sum = 0;
  for ( i=0; i<_numClients; i++ ) // Sum the percentages.
    {
      sum += _clientWeights[i];
    }
  if ( sum < 100 ) // Due to rounding, distribute left overs.
    {
      sum = 100 - sum; // Calculate amount left over
      for ( i=0; (sum > 0) && (i<_numClients); i++ )
	{
	  _clientWeights[i] += 1;
	  sum--;
	}
    }
  for ( i=0; i<_numClients; i++ )
    {
      TRACE((stderr,"WeightPercentage[%zu] = %zu\n",i,_clientWeights[i]));
    }

} // End: getClients()


void PAMI::ResourceManager::initClientStatus( )
{
  size_t i;

  // Allocate space for _clientStatus.
  pami_result_t prc;
  prc = __global.heap_mm->memalign((void **)&_clientStatus, 0,
				_numClients * sizeof(*_clientStatus));
  PAMI_assertf(prc == PAMI_SUCCESS, "alloc of _clientStatus failed");

  // Set the status of each client to "deallocated".
  for ( i=0; i<_numClients; i++ )
    _clientStatus[i] = RESOURCE_MANAGER_CLIENT_STATUS_DEALLOCATED;
}

/// \brief Get Number of Clients from PAMI_CLIENTS
/// 
/// Scan the PAMI_CLIENTS env var, counting commas.
/// Then, process :r specifications, increasing the number of clients by r-1, for each r.
///
size_t PAMI::ResourceManager::getNumClientsInternal()
{
  char *clientsConfigValueString;
  int i,j;
  size_t numClients;

  // Get PAMI_CLIENTS
  clientsConfigValueString = getConfigValueString( "PAMI_CLIENTS" );

  // If PAMI_CLIENTS is not specified, there must be 1 client.
  if ( clientsConfigValueString == NULL )
    return numClientsDefault;

  // Count the comma separators to find out how many client entries are specified.
  numClients = 1;
  for ( i=0; ; i++ )
    {
      if ( clientsConfigValueString[i] == '\0' ) break; // Done.

      if ( clientsConfigValueString[i] == ',' ) numClients++;
    }

  // Find each :r occurrence, convert r to a number, and add r-1 to the numClients count.
  i=0;
  int r=-1; // Init to "not specified".

  for ( ; ; )
    {
      if ( ( clientsConfigValueString[i] == '\0' ) ||
	   ( clientsConfigValueString[i] == ',' ) )
	{
	  if ( r != -1 ) numClients += (r-1); // Take the last r value and apply it to numClients.
	  r = -1; // Start over with no r value.
	  if ( clientsConfigValueString[i] == '\0' ) break;
	}
      
      if ( clientsConfigValueString[i] == ':' ) // Found a :r specification?
	{
	  r = defaultClientRepeat; // Init to default, in case r value not specified.
	  i++; // Skip past the ':'
	  for ( j=i; ; j++ ) // Find end of "r".
	    {
	      if ( ( clientsConfigValueString[j] == ',' ) ||
		   ( clientsConfigValueString[j] == ':' ) ||
		   ( clientsConfigValueString[j] == '/' ) ||
		   ( clientsConfigValueString[j] == '\0' ) ) break;
	    }
	  if ( i != j ) // Is there an 'r' value?
	    {
	      char ending;
	      ending = clientsConfigValueString[j]; // Save ending delimiter.
	      clientsConfigValueString[j] = '\0';   // Replace it with a null delimiter.
	      r = atoi( &clientsConfigValueString[i] );
	      PAMI_assertf(r > 0, "PAMI_CLIENTS :%d specification must be greater than zero.\n",r);
	      clientsConfigValueString[j] = ending; // Restore ending delimiter.
	      i = j; // Move past :r specification.
	    }
	}
      else
	i++;
    }

  return numClients;
}


void PAMI::ResourceManager::getProcessInfo()
{
  // Get number of processes per node.  This is the same on all nodes.
  _numProcessesPerNode = Kernel_ProcessCount();

  // Get number of cores per process.  This is the same on all processes.
  _numCoresPerProcess = 16 /* cores per node */ / _numProcessesPerNode;
  if ( _numCoresPerProcess == 0 ) _numCoresPerProcess = 1;

  // Get number of hardware threads per process.  This is the same on all processes.
  _numHWThreadsPerProcess = Kernel_ProcessorCount();

  _numProcessesPerCore = 4 /* HW threads per core */ / _numHWThreadsPerProcess;
  if ( _numProcessesPerCore == 0) _numProcessesPerCore = 1;

  // Get number of hardware threads per process per core.  This is the same on all cores
  // within a process.
  _numHWThreadsPerProcessPerCore = _numHWThreadsPerProcess / _numCoresPerProcess;

  TRACE((stderr,"PAMI ResourceManager: numProcessesPerNode = %zu, numCoresPerProcess = %zu, numProcessesPerCore = %zu, numHWThreadsPerProcess = %zu, numHWThreadsPerProcessPerCore = %zu\n",_numProcessesPerNode, _numCoresPerProcess, _numProcessesPerCore, _numHWThreadsPerProcess, _numHWThreadsPerProcessPerCore));

} // End: getProcessInfo()


void PAMI::ResourceManager::getConfig()
{
  char *clientsConfigValueString;

  // Get MUSPI_NUMINJFIFOS
  _numSpiUserInjFifosPerProcess    = getConfigValueSize_t( "MUSPI_NUMINJFIFOS",
							   numSpiUserInjFifosPerProcessDefault );

  // Get MUSPI_NUMRECFIFOS
  _numSpiUserRecFifosPerProcess    = getConfigValueSize_t( "MUSPI_NUMRECFIFOS",
							   numSpiUserRecFifosPerProcessDefault );

  // Get MUSPI_NUMBATIDS
  _numSpiUserBatIdsPerProcess    = getConfigValueSize_t( "MUSPI_NUMBATIDS",
							   numSpiUserBatIdsPerProcessDefault );

  // Get the number of clients
  _numClients = getNumClientsInternal();

  TRACE((stderr,"PAMI ResourceManager: numSpiUserInjFifosPerProcess=%zu, numSpiUserRecFifosPerProcess=%zu, numClients=%zu\n",_numSpiUserInjFifosPerProcess,_numSpiUserRecFifosPerProcess,_numClients));

  // Get PAMI_CLIENTS
  clientsConfigValueString = getConfigValueString( "PAMI_CLIENTS" );

  getClients( clientsConfigValueString );

  // Set the status of each client to "deallocated".
  initClientStatus();

  // Get PAMI_RGETINJFIFOSIZE
  _rgetInjFifoSize = getConfigValueSize_t( "PAMI_RGETINJFIFOSIZE",
					   rgetInjFifoSizeDefault );

  // Get MUSPI_INJFIFOSIZE
  _injFifoSize = getConfigValueSize_t( "MUSPI_INJFIFOSIZE",
				       injFifoSizeDefault );

  // Get MUSPI_RECFIFOSIZE
  _recFifoSize = getConfigValueSize_t( "MUSPI_RECFIFOSIZE",
				       recFifoSizeDefault );

  // Get Process Info
  getProcessInfo();

} // End: getConfig()

/// \brief Allocate a Client With The Specified Name
///
/// \param[in]  clientName  Pointer to the null-terminated client name
/// \param[out] clientId    Pointer to a size_t where the client ID is returned.
///
/// \retval  PAMI_SUCCESS  The client ID is returned.
/// \retval  PAMI_INVAL    Failure.  The client is not found.
///
pami_result_t PAMI::ResourceManager::allocateClient( char   *clientName,
						     size_t *clientId )
{
  size_t i;

  // Scan the client names for a match with the specified clientName.
  for ( i=0; i<_numClients; i++ )
    {
      if ( ( strcmp ( clientName, getClientName(i) ) == 0 ) && // Match?
	   ( _clientStatus[i] != RESOURCE_MANAGER_CLIENT_STATUS_ALLOCATED ) ) // AND Not allocated?
	{
	  // A matching client name was found and it is not allocated.
	  _clientStatus[i] = RESOURCE_MANAGER_CLIENT_STATUS_ALLOCATED; // Mark client allocated.
	  *clientId = i; // Return the client ID.
	  TRACE((stderr,"allocateClient: Client %s successfully allocated with ID %zu\n",clientName,i));
	  return PAMI_SUCCESS;
	}
    }

  // If we get to here, the specified client name does not match anything we have stored.
  // If there is an unallocated client with a NULL name (not specified), allocate it.  Otherwise, return invalid.

  for ( i=0; i<_numClients; i++ )
    {
      if ( ( strcmp ( "", getClientName(i) ) == 0 ) && // Configured client name is NULL?
	   ( _clientStatus[i] != RESOURCE_MANAGER_CLIENT_STATUS_ALLOCATED ) ) // Client not allocated?
	{
	  _clientStatus[i] = RESOURCE_MANAGER_CLIENT_STATUS_ALLOCATED; // Mark client allocated.
	  *clientId = i; // Return the client ID.
	  TRACE((stderr,"allocateClient: Client %s matches wildcard client with ID %zu\n",clientName,i));
	  return PAMI_SUCCESS;
	}
    }

  TRACE((stderr,"allocateClient: Client %s does not match any configured client or too many clients allocated.\n",clientName));

  return PAMI_INVAL;
}

/// \brief Deallocate a Client With The Specified ID
///
/// \param[in]  clientId  The ID of the client to be deallocated
///
/// \retval PAMI_SUCCESS  The client has been deallocated
/// \retval PAMI_INVAL    The client is invalid, e.g. already deallocated
///
pami_result_t PAMI::ResourceManager::deallocateClient( size_t clientId )
{
  if ( _clientStatus[clientId] == RESOURCE_MANAGER_CLIENT_STATUS_ALLOCATED )
    {
      TRACE((stderr,"deallocateClient: Client %zu successfully deallocated\n",clientId));
      _clientStatus[clientId] = RESOURCE_MANAGER_CLIENT_STATUS_DEALLOCATED;
      return PAMI_SUCCESS;
    }

  TRACE((stderr,"deallocateClient: Client %zu is not allocated...failed to deallocate\n",clientId));
  
  return PAMI_INVAL;
}


#undef TRACE

#endif   // __common_bgq_ResourceManager_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
