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

#ifndef _FLIGHT_RECORDER_H_
#define _FLIGHT_RECORDER_H_

#include <map>
#include <string>
#include <pthread.h>
#include <sys/types.h>
#include "NoPassByValue.h"
// #include "pthreadmutex.h"

class FlightRecorder : NoPassByValue
{
static const uint NumMsgs = 27;
static const uint SizeMsg = 150;
private:
   std::string _name;
   uint _numMsgs;
   uint _sizeMsg;
   char _msgs[NumMsgs][SizeMsg];
   uint _indexNextMsg;
   static pthread_mutex_t _frMapLock; // sync access to fr map 
   static std::map<std::string,FlightRecorder*> _flightRecorders;
public:
   // FlightRecorder(); 
   FlightRecorder(const std::string& name, uint numMsgs = NumMsgs, uint sizeMsg = SizeMsg); 
   ~FlightRecorder(); 

   int saveMsg(const char* pNewMsg);
   void displayMsgs(const std::string& sPrefix);
   
   uint getMsgSize() { return SizeMsg; }

   static std::string list();
   static void dump(const std::string& name);
};
// End class FlightRecorder

#endif /* _FLIGHT_RECORDER_H_ */
