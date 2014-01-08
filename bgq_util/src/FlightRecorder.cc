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
/* (C) Copyright IBM Corp.  2007, 2011                              */
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

// 
//
#include "FlightRecorder.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include "Time.h"

using namespace std;

  #define TRACE(x) printf x
//#define TRACE(x)

  #define PRINT(x) printf x

pthread_mutex_t FlightRecorder::_frMapLock = PTHREAD_MUTEX_INITIALIZER;
std::map<std::string, FlightRecorder*> FlightRecorder::_flightRecorders;

FlightRecorder::FlightRecorder(const string& name, uint numMsgs, uint sizeMsg)
  : _name(name)
    , _numMsgs(numMsgs)
    , _sizeMsg(sizeMsg)
    , _indexNextMsg(0)
{
   memset(&_msgs[0][0], 0x00, numMsgs*sizeMsg);
   // add this flight recorder to the map
   pthread_mutex_lock(&_frMapLock);
   _flightRecorders[_name] = this;
   pthread_mutex_unlock(&_frMapLock);
}
// End FlightRecorder::FlightRecorder()

FlightRecorder::~FlightRecorder()
{
  // remove this flight recorder to the map
  pthread_mutex_lock(&_frMapLock);
  _flightRecorders.erase(_name);
  pthread_mutex_unlock(&_frMapLock);
}

string FlightRecorder::list() {
  stringstream stm;
  map<string,FlightRecorder*>::iterator iter;
  pthread_mutex_lock(&_frMapLock);
  for (iter = _flightRecorders.begin(); iter != _flightRecorders.end(); iter++)
  {
    stm << iter->first << "\n";
  }
  pthread_mutex_unlock(&_frMapLock);
  return stm.str();
}

void FlightRecorder::dump(const string& name) {
  pthread_mutex_lock(&_frMapLock);
  map<string,FlightRecorder*>::iterator iter = _flightRecorders.find(name);
  if (iter != _flightRecorders.end())
  {
    iter->second->displayMsgs(name);
  }
  pthread_mutex_unlock(&_frMapLock);
}

int FlightRecorder::saveMsg(const char* pNewMsg)
{
   Time now;
   // Save the message in the flight recorder (including a timestamp).
   snprintf(&_msgs[_indexNextMsg][0], _sizeMsg, "%s %s", now.curTimeString(true).c_str(), pNewMsg);
   // Determine the index of where the next message should be put.
   ++_indexNextMsg;
   if (_indexNextMsg >= _numMsgs)
      _indexNextMsg = 0;
   return 0;
}
// End FlightRecorder::saveMsg(char* pNewMsg)


void FlightRecorder::displayMsgs(const string& sPrefix)
{
   // Loop through displaying all messages in the flight recorder (in oldest to newest order).
   for (uint uMsgCntr=0, uMsgIndx=_indexNextMsg; uMsgCntr < _numMsgs; ++uMsgCntr)
   {
      // Display message.
      printf("FlightRecorder for %s: %s\n", sPrefix.c_str(), &_msgs[uMsgIndx][0]);
      // Determine index of the next message to display.
      ++uMsgIndx;
      if (uMsgIndx >= _numMsgs)
         uMsgIndx = 0;
   }
}
// End FlightRecorder::displayMsgs(const string& sPrefix)

