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

#include "RasEventImpl.h"
#include <stdarg.h>
#include <cstdio>
#include <sstream>
#include <iomanip>


using namespace std;

string RasEventImpl::getDetail(const string& key) const { 
  map<string,string>::const_iterator pos = _details.find(key);
  if (pos != _details.end()) return pos->second;
  else if (key == RasEvent::MSG_ID) {
     ostringstream ss;
     ss << hex << setw(8) << uppercase << setfill('0') << _msgId;
     return ss.str(); 
  }
  else return "";
} 


RasEventImpl::RasEventImpl(uint32_t msgId, 
			   const char *details, ...) :
  _msgId(msgId), _handled(false) 
{
  gettimeofday(&_time, 0);
  if (details) { 
    char buf[1000]; 
    va_list ap;
    va_start(ap, details);
    vsnprintf(buf, sizeof(buf), details, ap);
    va_end(ap);
    this->setDetail("DETAIL0", buf);
  }
  ostringstream ss;
  ss << hex << setw(8) << uppercase << setfill('0') << msgId;
  this->setDetail(RasEvent::MSG_ID,ss.str());
}

vector<string> RasEventImpl::tokenize(const string &details,
				      const string &delimiter)
{
   vector<string> words;
   
   string::size_type lastPos(details.find_first_not_of(delimiter, 0));
   string::size_type pos(details.find_first_of(delimiter, lastPos));
   while (string::npos != pos || string::npos != lastPos) {
      words.push_back(details.substr(lastPos, pos - lastPos));
      lastPos = details.find_first_not_of(delimiter, pos);
      pos = details.find_first_of(delimiter, lastPos);
   }
   return words;
}

void RasEventImpl::va_setDetails_pair(const char *details, va_list ap)
{
   if (details) {
      char buf[1000];				       
      vsnprintf(buf, sizeof(buf), details, ap);  
      // process comma delimited key value pair of details
      vector<string> d = tokenize(buf,","); 
      for (unsigned n = 0; n+1 < d.size(); n+=2) {
	 setDetail(d[n], d[n+1]); 
      }
   }
}


RasEventImpl RasEventImpl::createRasEvent_pair(uint32_t msgId, 
					       const string& location,
					       const char *details, ...)
{
   RasEventImpl event(msgId);
   event.setDetail(RasEvent::LOCATION, location);
   if (details) {					       
      va_list ap;				       
      va_start(ap, details);			       
      event.va_setDetails_pair(details,ap);
      va_end(ap);                                    
   }						       
   return event;
}


RasEventImpl::RasEventImpl(uint32_t msgId, 
			   bool handled, 
			   unsigned time_secs, 
			   unsigned time_usecs, 
			   std::vector<std::string>& keys, 
			   std::vector<std::string>& values ) 
  : _msgId(msgId), _handled(handled), _time(), _details()
{
  _time.tv_sec = time_secs;
  _time.tv_usec = time_usecs;
  // set the size to the smaller of the two vectors
  size_t size = keys.size();
  if (values.size() < keys.size()) size = values.size();
  for (size_t i = 0; i < size; ++i)
    _details[ keys[i] ] = values[i];
  ostringstream ss;
  ss << hex << setw(8) << uppercase << setfill('0') << msgId;
  this->setDetail(RasEvent::MSG_ID,ss.str());

}

ostream& operator<< (ostream& o, const RasEventImpl& event) 
{
  o << (RasEvent&) event; 
  return o;
} 

