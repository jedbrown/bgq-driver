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
#ifndef RASEVENTMETADATA_H_
#define RASEVENTMETADATA_H_

#include <pthread.h>
#include <string>
#include <vector>
#include <map>
#include "RasEvent.h"

/** \class RasEventMetadata
 *
 * Responsible for caching the metadata associated with a RAS Event.
 */
class RasEventMetadata 
{
public:
  /** \brief Get the message id for this event.   
   * The message id is unique per event.
   * Returns message id string
   */
  virtual const std::string& msgId() const = 0;

  /** \brief Get the category for the event.   
   * Categories are defined in ras_categories.xml file.
   * Returns category string
   */
  virtual const std::string& category() const = 0;

  /** \brief Get the owning component of the event.   
   * Components are defined in ras_categories.xml file.
   * Returns component string
   */
  virtual const std::string& component() const = 0;

  /** \brief Get the default message.
   * Messages are defined in the RAS Event metadata.
   * Returns message string
   */    
  virtual const std::string& message() const = 0;
	
  /** \brief Get the default severity.
   * Severity is defined in the RAS Event metadata.
   * Returns severity string
   */    	
  virtual const std::string& severity() const = 0;

  /** \brief Get the decoder.
   * Decoders are may be defined for a kernel event.
   * Returns decoder string or "" if a decoder is not defined
   */    	
  virtual const std::string& decoder() const = 0;

  /** \brief Get the control_action.
   * Control_action may be defined in the RAS Event metadata.
   * Returns control_action string or "" if control_action is not defined
   */    	
  virtual const std::string& controlAction() const = 0;
	
  /** \brief Get a reference to the vector of variables for the 
   * message substitution.   
   * \returns vector<string>& labels   
   */    		
  virtual const std::vector<std::string>& vars() const = 0;
	
  /** \brief Get a format string for the variable name
   * \returns format string 
   */    		
  virtual std::string format(const std::string&) const = 0;
	
  /** \brief Virtual destructor
   */    		
  virtual ~RasEventMetadata() {};
	
  /** \brief A static factory method to get the RasEventMetadata 
   * for a RasEvent uniquely identified by its message id. 
   * \param msgId
   * Returns vector of detail label strings
   */    		
  static const RasEventMetadata& factory(const std::string& msgId);
  static const RasEventMetadata& factory_default(const std::string& msgId);
  static const RasEventMetadata& factory(uint32_t eventId);
  static void setInstallPath(std::string path);
  static std::string getInstallPath();
  static void setTestPath(std::string path);
  static std::string getTestPath();
  static void readRasMetadata();
  static void setMetadata(const std::string& msgId, RasEventMetadata* ptr);
  static void clearMetadata();
protected: 
  RasEventMetadata() {}	// implementations should be constructed by the factory
private:
  static pthread_mutex_t _mdLock; // sync access to msgId to metadata map
  static void findMetadataFiles(std::string& dir, std::map<std::string,std::string>& files);
  static std::string installPath;
  static std::string testPath;
  static std::map<std::string, RasEventMetadata*> _metadatas;
};

#endif /*RASEVENTMETADATA_H_*/
