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
/* (C) Copyright IBM Corp.  2009, 2011                              */
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
#ifndef RASEVENT_H
#define RASEVENT_H

#include <string>
#include <map>
#include <stdint.h>
#include <sys/time.h> 
#include <iostream>

/** 
 * \Class RasEvent
 *
 * RasEvent is responsible for caching the details of a RasEvent
 * while it is being handled by the control system.  
 * 
 * The includes some attributes that are not mutable like the 
 * message id and time the event was created.
 * These will have getter methods only.
 * 
 * Other details like Message and Severity can be read and updated 
 * via the get and set methods provided. 
 */
class RasEvent
{
public:
	// standard keys for values stored in the RasEvent map 
	static std::string MSG_ID;
        static std::string CATEGORY;
        static std::string COMPONENT;
	static std::string SEVERITY;
	static std::string MESSAGE;
	static std::string LOCATION;
	static std::string MBOX_TYPE;
	static std::string MBOX_PAYLOAD;
	static std::string JTAG_PORT;
	static std::string CPU;
	static std::string ECID;
	static std::string SERIAL_NUMBER;
	static std::string COOKIE;
	static std::string CONTROL_ACTION;
	static std::string DECODER;
	static std::string JOBID;
	static std::string BLOCKID;
	static std::string COUNT;

	// List of event severities
	enum Severity {
		INFO,  	// designates informational messages that highlight the progress of system software.
		WARN,	// designates potentially harmful situations like an error threshold being exceeded.or a redundant component failed
		FATAL, 	// designates severe error events.
		UNKNOWN // the severity is not known
	};

	/**
	 * Get the refCode
	 * @return refCode
	 */
	virtual uint32_t msgId() const = 0;

	/**
	 * Get the time the event was created 
	 * @return timeval
	 */
	virtual const timeval time() const = 0;

	/**
	 * Get the handled flag that indicates this event 
	 * has been through the handler chain 
	 * @return boolean handled
	 */
	virtual bool handled() const = 0;

	/**
	 * Get the value associated with the key
	 * @param key 
	 * @return string value, If the key is not defined, "" is returned
	 */
	virtual std::string getDetail(const std::string& key) const = 0;
      
	/**
	 * Set the value 
	 * @param key 
	 * @param value 
	 */
	virtual void setDetail(const std::string& key, const std::string& value) = 0;
  
	/**
	 * get a reference the detail map 
	 * @return map<string,string>& of key,value detail pairs
	 */
	virtual std::map<std::string,std::string>& getDetails() = 0;
  
	// destructor
	virtual ~RasEvent() {}  

	// static std::string componentAsString(RasEvent::Component comp); 
    
protected:
	friend class RasEventHandlerChain;
	/**
	 * Set the handled flag that indicates this event 
	 * has been through the handler chain 
	 * @param bool flag
	 */
	virtual void setHandled(bool flag) = 0;

private:
	// static std::string _componentAsString[];
	friend std::ostream& operator<< (std::ostream& o, const RasEvent& e);

};

std::ostream& operator<< (std::ostream& o, const RasEvent& e);

#endif // RASEVENT_H
