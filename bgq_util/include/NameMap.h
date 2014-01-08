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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

#ifndef __NAME_MAP_H__
#define __NAME_MAP_H__

#include <string>
#include <sstream>
#include <map>
#include "pthreadmutex.h"

/*!
 * Generic map for alias to real name.
 * CAUTION: NOT THREAD SAFE (use NameMapTS if necessary)
*/
class NameMap
{
   public:
   
   /*! Add known aliases here
   */
   NameMap()
   : _map()
   {
      //_map["BQC.CH.CLK_X1"] = "BQC.CH.CLK_X1";
   };
   
   virtual ~NameMap()
   {
      _map.clear();
   }
   
   /*! Dynamically add aliases here
   */
   virtual void addName ( const std::string& alias, const std::string& name )
   {
      _map[alias] = name;
   }
   
   /*! Get facility name from alias
   * If no mapping is defined, return the alias.
   */
   virtual std::string getName( const std::string& alias ) 
   {
      if( _map.find( alias ) != _map.end() )
         return _map[alias];
      else
         return std::string(alias);
   }

	/*! 
	 * \brief To std::string method.
	 * Need this in addition to the output operator because you can't have
	 * virtual friends.
	 */
	virtual std::string toString() const
	{
      std::ostringstream oss;
      for( std::map<std::string,std::string>::const_iterator it = _map.begin(); it != _map.end(); it++ )
      {
         oss << it->first << " = " << it->second << std::endl;
      }
      return oss.str();
   }
	
	/*! 
	 * \brief Output operator.
	 * Print the contents of the map to the stream.
	 */
	friend std::ostream& operator<< (std::ostream& os, const NameMap* map )
	{
      return os << map->toString();
   }
   
   protected:
   std::map < std::string, std::string > _map;
};

/*!
* Thread-safe version of NameMap
*/
class NameMapTS : public NameMap
{
   public:
   
   NameMapTS()
   : _maplock()
   {
   }
   
   /*! Dynamically add aliases here
   */
   virtual void addName ( const std::string& alias, const std::string& name )
   {
      PthreadMutexHolder mymutex;
      mymutex.Lock(&_maplock);
      NameMap::addName( alias, name );
      mymutex.Unlock();
   }
   
   /*! Get facility name from alias
   */
   virtual std::string getName( const std::string& alias )
   {
      PthreadMutexHolder mymutex;
      mymutex.Lock(&_maplock);
      return NameMap::getName( alias );
   }

   
   protected:
   PthreadMutex _maplock;
};

#endif

