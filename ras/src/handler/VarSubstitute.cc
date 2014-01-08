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
#include "VarSubstitute.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <map>
#include "RasLog.h"
using namespace std;

#ifdef __linux
LOG_DECLARE_FILE("ras");
#endif

RasEvent& VarSubstitute::handle(RasEvent& event, const RasEventMetadata& metadata)
{

  LOG_TRACE_MSG("VarSubstitute handling event id=" << event.getDetail(RasEvent::MSG_ID).c_str() << " msg=" <<  event.getDetail(RasEvent::MESSAGE).c_str());

  const vector<string>& vars = metadata.vars();
  if (vars.size() == 0) return event;

  map<string, string>& details = event.getDetails();
  string msg = details[RasEvent::MESSAGE];
  bool sub = false;
  for (unsigned i = 0; i < vars.size(); ++i) {
    string var_name = vars[i];
    if (var_name == RasEvent::MESSAGE) continue; // disallow msg to sub itself
    map<string,string>::iterator pos = details.find(var_name);
    if (pos != details.end()) {
      string var_value = pos->second;
      // Scan the message for variable index
      string var = "$(" + var_name + ")";
      string::size_type v_index = msg.find(var, 0);
      if (v_index != string::npos) { 
	// replace the variable with its value
	LOG_TRACE_MSG("\tVarSub " << var.c_str() << " at " << v_index << " for " << var.size() << " ch with " << var_value.c_str());
	msg.replace(v_index, var.size(), var_value);
	sub = true;
	// erase var name from map if not a std key
	if (_std_keys.find(var_name) == _std_keys.end())
	  details.erase(var_name);
      } else continue;
    } else continue;
  }

  if (sub) details[RasEvent::MESSAGE] = msg;
  LOG_TRACE_MSG("\tVarSub post-sub msg = " << msg.c_str());
  return event;
}

VarSubstitute::VarSubstitute() :  _name("VarSubstitute"), _std_keys()

{
  _std_keys.insert(RasEvent::MSG_ID);
  _std_keys.insert(RasEvent::SEVERITY);
  _std_keys.insert(RasEvent::MESSAGE);
  _std_keys.insert(RasEvent::LOCATION);
  _std_keys.insert(RasEvent::MBOX_PAYLOAD);
  _std_keys.insert(RasEvent::JTAG_PORT);
  _std_keys.insert(RasEvent::CPU);
  _std_keys.insert(RasEvent::ECID);
  _std_keys.insert(RasEvent::SERIAL_NUMBER);
  _std_keys.insert(RasEvent::COOKIE);  
  _std_keys.insert(RasEvent::CONTROL_ACTION);
  _std_keys.insert(RasEvent::JOBID);  
  _std_keys.insert(RasEvent::COUNT);
}

VarSubstitute::~VarSubstitute()
{
}
