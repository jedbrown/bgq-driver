/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
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
#ifndef __api_extension_c_collsel_CollselData_h__
#define __api_extension_c_collsel_CollselData_h__

#include <map>
#include "pami.h"
#include "boost/property_tree/detail/rapidxml.hpp"

namespace PAMI
{
  using  namespace boost::property_tree::detail::rapidxml;

  typedef std::map<unsigned, char *> AlgoMap;                  // Key is the algorithm ID used in XML
  typedef std::map<char *, unsigned> AlgoNameToIdMap;          // Key is the algorithm name
  typedef unsigned char* AlgoList;                             // Each byte in the array represents a collective algo ID
  typedef std::map<size_t, AlgoList> MessageSizeMap;           // Key is the message size
  typedef std::map<unsigned, MessageSizeMap> CollectivesMap;   // Key is the collective pami_xfer_type_t value
  typedef std::map<unsigned, CollectivesMap> GeometrySizeMap;  // Key is the geometry size 
  typedef std::map<unsigned, GeometrySizeMap> GeometryShapeMap;// Key is the geometry shape (PPN)

  struct CollselQuery
  {
    CollectivesMap    *coll_map;
    pami_algorithm_t **algorithms;
    pami_metadata_t  **metadata;
  };

  class CollselData
  {
    public:
    CollselData()
    {
    }

    GeometrySizeMap* find_nearest_geometry_shape(unsigned ppn)
    {
      //TODO: binary search is log(N), explore optimizations
      GeometryShapeMap::iterator iter;
      // Find a key >= ppn
      iter = collsel_map.lower_bound(ppn);
      if(iter->first == ppn || iter == collsel_map.begin())
      {
        // There is a matching ppn or ppn is smaller than any key in the map
        return &(iter->second);
      }
      else if(iter == collsel_map.end())
      {
        // ppn greater than any key in the map
        // return the last entry in the map
        return &((--iter)->second);
      }
      else
      {
        // Find closest match to ppn
        unsigned ub = iter->first, lb = (--iter)->first;
        return ((ub - ppn) >= (ppn - lb)) ? &(iter->second) : &((++iter)->second);
      }
    }


    CollectivesMap* find_nearest_geometry_size(unsigned geo_size, GeometrySizeMap *geo_map)
    {
      //TODO: binary search is log(N), explore optimizations
      GeometrySizeMap::iterator iter;
      // Find a key >= geo_size
      iter = geo_map->lower_bound(geo_size);
      if(iter->first == geo_size || iter == geo_map->begin())
      {
        // There is a matching geo_size or geo_size is smaller than any key in the map
        return &(iter->second);
      }
      else if(iter == geo_map->end())
      {
        // geo_size greater than any key in the map
        // return the last entry in the map
        return &((--iter)->second);
      }
      else
      {
        // Find closest match to geo_size
        unsigned ub = iter->first, lb = (--iter)->first;
        return ((ub - geo_size) >= (geo_size - lb)) ? &(iter->second) : &((++iter)->second);
      }
    }

    MessageSizeMap* find_collective(unsigned collective, CollectivesMap *coll_map)
    {
      CollectivesMap::iterator iter = coll_map->find(collective);
      return iter != coll_map->end()? &(iter->second) : NULL;
    }

    AlgoList* find_nearest_message_size(size_t msg_size, MessageSizeMap *msg_map)
    {
      //TODO: binary search is log(N), explore optimizations
      MessageSizeMap::iterator iter;
      // Find a key >= msg_size
      iter = msg_map->lower_bound(msg_size);
      if(iter->first == msg_size || iter == msg_map->begin())
      {
        // There is a matching msg_size or msg_size is smaller than any key in the map
        return &(iter->second);
      }
      else if(iter == msg_map->end())
      {
        // msg_size greater than any key in the map
        // return the last entry in the map
        return &((--iter)->second);
      }
      else
      {
        // Find closest match to msg_size
        size_t ub = iter->first, lb = (--iter)->first;
        return ((ub - msg_size) >= (msg_size - lb)) ? &(iter->second) : &((++iter)->second);
      }
    }

    /**
     *  Map the algorithm ID used in the XML to the actual PAMI algorithm
     */
    char*  lookup_algorithm(pami_xfer_type_t coll, unsigned char algo_id)
    {
      AlgoMap::iterator iter = algo_map[coll].find(algo_id);
      assert(iter != algo_map->end());
      return iter->second;
    }

    GeometryShapeMap& get_datastore()
    {
      return collsel_map;
    }

    AlgoMap* get_algorithm_map()
    {
      return algo_map;
    }
	
    AlgoNameToIdMap* get_algorithm_name_map()
    {
      return algo_name_map;
    }

    char * mempool_allocate(size_t size)
    {
      return mpool.allocate_string(NULL, size);
    }

    private:
    GeometryShapeMap collsel_map;      // Collsel data store
    AlgoMap algo_map[PAMI_XFER_COUNT]; // Map the algorithm ID in XML to the actual PAMI algorithm 
    AlgoNameToIdMap algo_name_map[PAMI_XFER_COUNT];
    memory_pool<>   mpool;   // Memory pool
  };
}
#endif
