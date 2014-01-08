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
#ifndef RASEVENTIMPL_H
#define RASEVENTIMPL_H


#include "RasEvent.h"
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <cstdarg>


/** 
 * \class RasEventImpl
 *
 * RasEventImpl is a default implementation of RasEvent that can
 * be used to create RasEvents.
 */
class RasEventImpl : public RasEvent
{
private:
  uint32_t _msgId;
  bool _handled; 
  timeval  _time;
  std::map<std::string,std::string> _details;
protected: 
  /**
   * Set the handled flag that indicates this event 
   * has been through the handler chain 
   * @param bool flag
   */
  virtual void setHandled(bool flag) { _handled = flag; }

  /**
   * set the event details - KEY,VALUE pairs
   */
  void va_setDetails_pair(const char *details, va_list ap);
  
  /**
   * helper function to parse the string of KEY,VALUE pairs
   */
  std::vector<std::string> tokenize(const std::string &details,
				    const std::string &delimiter);

public:	
  /**
   * Get the RAS message id
   */
  virtual uint32_t msgId() const { return  _msgId; }

  /**
   * Get the time the event was created 
   */
  virtual const timeval time() const         { return _time; }
  virtual void  setTimeVal(timeval newTime)  { _time = newTime; }

  /**
   * Get the handled flag that indicates this event 
   * has been through the handler chain 
   * @return boolean handled
   */
  virtual bool handled() const { return _handled; }

  /**
   * Get the value associated with the key
   * \param key 
   */
  virtual std::string getDetail(const std::string& key) const;
       
  /**
   * Set the value 
   * \param key 
   * \param value 
   */
  virtual void setDetail(const std::string& key, const std::string& value) { _details[key]=value; }
  
  /**
   * Get the detail map 
   * \returns map<string,string> of key,value pairs
   */
  virtual std::map<std::string,std::string>& getDetails() { return _details; }
       
  /**
   * Constructor
   * @param msgId The RAS event message identifier 
   * @param details A format string followed by a variable number of args 
   */
  RasEventImpl(uint32_t msgId, 
	       const char *details = 0, ...);

  /**
   * Create a RasEventImpl providing the message id, location string, 
   * and optional KEY,VALUE pairs of arguments.
   * @param msgId The RAS event message identifier 
   * @param location The location string associated with the event.
   * @param details A optional format string followed by the optional 
   * variable number of arguments.  The format string is required to have
   * one or more matching pairs of KEY,VALUE (for example: "DATA1,%s,DATA2,%s")
   * The format of the string follows the printf style of parameters.
   */
  static RasEventImpl createRasEvent_pair(uint32_t msgId, 
					  const std::string& location,
					  const char *details = 0, ...);

  /**
   * Constructor 
   * @param msgId The RAS message id
   * @param handled flag
   * @param time in seconds
   * @param time in micro seconds
   * @param keys vector of ras keys
   * @param values vector of ras values   
   */
  RasEventImpl(uint32_t msgID, 
	       bool handled, 
	       unsigned t_secs, 
	       unsigned t_usecs, 
	       std::vector<std::string>& keys, 
	       std::vector<std::string>& values);
     
  virtual ~RasEventImpl() {}
};

std::ostream& operator<< (std::ostream& o, const RasEventImpl& event);


#endif // RASEVENTIMPL_H

