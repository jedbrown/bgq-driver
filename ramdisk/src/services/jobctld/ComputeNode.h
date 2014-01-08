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

//! \file  ComputeNode.h
//! \brief Declaration and inline methods for bgcios::jobctl::ComputeNode class.

#ifndef JOBCTL_COMPUTENODE_H
#define JOBCTL_COMPUTENODE_H

// Includes
#include <ramdisk/include/services/common/RdmaClient.h>
#include <hwi/include/common/uci.h>
#include <tr1/memory>
#include <string>
#include <sstream>

namespace bgcios
{

namespace jobctl
{

//! \brief Track a compute node using the job control service.

class ComputeNode
{
public:

   //! \brief  Default constructor.
   //! \param  serviceId Unique service identifier assigned to compute node.
   //! \param  client Pointer to RdmaClient object for compute node.
   //! \param  blockId Block identifier of compute node block.
   //! \param  uci Universial component identifier of compute node.
   //! \param  coords Torus coordinates of compute node.
   //! \param  bridge Torus coordinates of bridge compute node with I/O link.

   ComputeNode(uint32_t serviceId, RdmaClientPtr client, uint32_t blockId, uint64_t uci, NodeCoordinates& coords, NodeCoordinates& bridge)
   {
      _ready_count=0;
      _serviceId = serviceId;
      _sysiodPort = 0;
      _toolctldPort = 0;
      _client = client;
      _blockId = blockId;

      _uci = uci;
      char uciBuffer[50];
      bg_uci_toString(_uci, uciBuffer);
      _uciString = uciBuffer;

      _coordinates = coords;
      std::ostringstream coordString;
      coordString << (int)_coordinates.aCoord << "," << (int)_coordinates.bCoord << "," << (int)_coordinates.cCoord << "," <<
         (int)_coordinates.dCoord << "," << (int)_coordinates.eCoord;
      _coordinateString = coordString.str();

      _bridge = bridge;
      std::ostringstream bridgeString;
      bridgeString << (int)_bridge.aCoord << "," << (int)_bridge.bCoord << "," << (int)_bridge.cCoord << "," <<
         (int)_bridge.dCoord << "," << (int)_bridge.eCoord;
      _bridgeString = bridgeString.str();
   }

   //! \brief  Get service identifier assigned to compute node.
   //! \return Service id value.

   uint32_t getServiceId(void) const { return _serviceId; }

   //! \brief  Get the pointer to the rdma connection client object for compute node.
   //! \return Pointer to RdmaClient object.

   const RdmaClientPtr& getClient(void) const { return _client; }

   //! \brief  Get the block identifier of compute node block.
   //! \return Block id value.

   uint32_t getBlockId(void) const { return _blockId; }

   //! \brief  Get the torus coordinates of compute node.
   //! \return Torus coordinates.

   const NodeCoordinates& getCoords(void) const { return _coordinates; }

   //! \brief  Get the torus coordinates of compute node as a string.
   //! \return Torus coordinates string.

   const std::string& getCoordString(void) const { return _coordinateString; }

   //! \brief  Get the torus coordinates of compute node's bridge compute node.
   //! \return Torus coordinates.

   const NodeCoordinates& getBridgeCoords(void) const { return _bridge; }

   //! \brief  Get the torus coordinates of compute node's bridge compute node as a string.
   //! \return Torus coordinates string.

   const std::string& getBridgeCoordString(void) const { return _bridgeString; }

   //! \brief  Get the node identifier of compute node.
   //! \return Node identifier value.

   uint32_t getNodeId(void) const { return coordsToNodeId(_coordinates.aCoord, _coordinates.bCoord, _coordinates.cCoord, _coordinates.dCoord, _coordinates.eCoord); }

   //! \brief  Get the universal component identifier of compute node.
   //! \return Universal component identifer value.

   uint64_t getUci(void) const { return _uci; }

   //! \brief  Get the universal component identifier of compute node as a string.
   //! \return Universal component identifier string.

   const std::string& getUciString(void) const { return _uciString; }

   //! \brief  Write info about the compute node to the specified stream.
   //! \param  os Output stream to write to.
   //! \return Output stream.

   std::ostream& writeTo(std::ostream& os) const
   {
      os << "uci=" << _uciString << " coordinates=" << getCoordString() << " bridge=" << getBridgeCoordString() << " blockId=" << _blockId << " serviceId=" << _serviceId;
      return os;
   }
   uint32_t incToReadyCount(){return ++_ready_count;}

  //! \brief  Set the port for daemons with varying ports
  //! \param  Service for having the port value updated
  //! \param  port value
  void setPort(int service,uint32_t port){
      if (service==SysioService) _sysiodPort=port;
      if (service==ToolctlService) _toolctldPort=port;
   }
   
   uint32_t getsysiodPort(){return _sysiodPort;}

   uint32_t gettoolctldPort(){return _toolctldPort;}

private:

   //! How many ready messages received?
   uint32_t _ready_count;

   //! Unique service identifier assigned to the compute node.
   uint32_t _serviceId;

   //! sysiod port number
   uint32_t _sysiodPort;

   //! tooldctld port number
   uint32_t _toolctldPort;

   //! Client connection to compute node.
   RdmaClientPtr _client;

   //! Block identifier of compute node block.
   uint32_t _blockId;

   //! Torus coordinates of the compute node.
   NodeCoordinates _coordinates;

   //! Torus coordinates as a string.
   std::string _coordinateString;

   //! Torus coordinates of the compute node's bridge compute node.
   NodeCoordinates _bridge;

   //! Torus coordinates of bridge compute node as a string.
   std::string _bridgeString;

   //! Universal component identifier of the compute node.
   uint64_t _uci;

   //! Universal component identifer as a string.
   std::string _uciString;

};

//! Smart pointer for ComputeNode object.
typedef std::tr1::shared_ptr<ComputeNode> ComputeNodePtr;

//! \brief  ComputeNode object shift operator for output.
//! \param  os Output stream to write to.
//! \param  cnode ComputeNode object.
//! \return Output stream.

inline std::ostream& operator<<(std::ostream& os, const ComputeNode& cnode)
{
   return cnode.writeTo(os);
}

} // namespace jobctl

} // namespace bgcios

#endif // JOBCTL_COMPUTENODE_H

