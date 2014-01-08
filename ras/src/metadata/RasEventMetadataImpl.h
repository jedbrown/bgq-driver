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
#ifndef RASEVENTMETADATAIMPL_H_
#define RASEVENTMETADATAIMPL_H_

#include <sstream>
#include <iostream> 
#include <vector> 

#include "RasEventMetadata.h"
#include <stdarg.h>


class RasEventMetadataImpl : public RasEventMetadata
{
private:
  std::string _msgId;
  std::string _category;
  std::string _component;
  std::string _severity; 
  std::string _message;
  std::string _decoder;
  std::string _controlAction;
  std::vector<std::string> _vars;
  std::map<std::string,std::string>* _formats;
public:
  virtual const std::string& msgId() const { return _msgId; }
  virtual const std::string& category() const { return _category; }
  virtual const std::string& component() const { return _component; }
  virtual const std::string& severity() const { return _severity; }
  virtual const std::string& message() const { return _message; }
  virtual const std::string& decoder() const { return _decoder; }
  virtual const std::string& controlAction() const { return _controlAction; }
  virtual const std::vector<std::string>& vars() const { return _vars; }
  virtual std::string format(const std::string&) const;
	
  virtual ~RasEventMetadataImpl();
  RasEventMetadataImpl(const std::string& msgId,
		       const std::string& category, 
		       const std::string& component, 
		       const std::string& sev, 
		       const std::string& msg, 
		       const std::string& decoder,
		       const std::string& controlAction);
  RasEventMetadataImpl(const std::string& msgId);       
  RasEventMetadataImpl(const RasEventMetadataImpl& impl);       
};

#endif /*RASEVENTMETADATAIMPL_H_*/
