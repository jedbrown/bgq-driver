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
#include "RasEventMetadataImpl.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdarg.h>

using namespace std;

RasEventMetadataImpl::
RasEventMetadataImpl(const string& msgId,
		     const string& category, 
		     const string& component, 
		     const string& sev, 
		     const string& msg, 
		     const string& decoder,
		     const string& controlAction)
  :  _msgId(msgId), _category(category), _component(component),
     _severity(sev), _message(msg), _decoder(decoder), _controlAction(controlAction),
     _vars(), _formats(NULL)
{
  // parse message for labels (vars)
  string var_start = "$(";
  string var_end = ")";
  size_t cur = 0;
  string::size_type ve_index = 0;
  string::size_type vs_index = _message.find(var_start, cur);
  while (vs_index != string::npos) { 
    // get the variable name
    cur = vs_index + 2;
    ve_index = _message.find(var_end, cur);
    if (ve_index != string::npos) {
      string var_name = _message.substr(cur, ve_index-cur);

      string::size_type pos = var_name.find (",");
      if (pos != string::npos) { 
	
	string format = var_name.substr(0,pos);
	string var = var_name.substr(pos+1);
	if (_formats == NULL) _formats = new map<string,string>();
	// insert the var name -> format string in a map
	if (format != "%s")   // protect against the use of %s
	  (*_formats)[var] = format;
	// replace $(format,var_name) with &(var_name) in the message
	_message.replace(vs_index, ve_index-vs_index+1, var_start+var+var_end);
	var_name = var;
      }

      _vars.push_back(var_name);
    }
    vs_index = _message.find(var_start, cur);
  }
}

string RasEventMetadataImpl::format(const string& var_name) const 
{
  if (_formats != NULL) {
    map<string,string>::iterator iter = _formats->find(var_name);
    if( iter != _formats->end() ) {
      return iter->second;
    }
  }
  return string(""); 
}

RasEventMetadataImpl::
RasEventMetadataImpl(const string& msgId) 
  : _msgId(msgId), _category("UNKNOWN"),  _component("UNKNOWN"),
    _severity("UNKNOWN"),  _message("UNKNOWN"), _decoder(""), _controlAction(""), _vars(), _formats(NULL)
{
}

RasEventMetadataImpl::
RasEventMetadataImpl(const RasEventMetadataImpl& impl)
  :  _msgId(impl._msgId), _category(impl._category),  _component(impl._component),  _severity(impl._severity), 
     _message(impl._message), _decoder(impl._decoder), _controlAction(impl._controlAction),
     _vars(impl._vars), _formats(NULL) 
{
  if (impl._formats != NULL) {
    _formats = new map<string,string>(*(impl._formats));
  }
}


RasEventMetadataImpl::
~RasEventMetadataImpl() 
{
  if (_formats != NULL) delete _formats;
}

