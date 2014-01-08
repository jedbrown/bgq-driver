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
#include "MetadataAdder.h"
#include "RasEventMetadata.h"
#include <map>
#include <stdio.h>
#include "RasLog.h"
using namespace std;

#ifdef __linux
LOG_DECLARE_FILE("ras");
#endif

RasEvent& MetadataAdder::handle(RasEvent& event, const RasEventMetadata& metadata)
{
  // TRACE(("MetadataAdder handling RAS Msg Id=%s\n", metadata.msgId().c_str()));
  LOG_TRACE_MSG("MetadataAdder handling RAS Msg Id=" << metadata.msgId().c_str());

	// set values for the well known keys
	event.setDetail(RasEvent::MSG_ID, metadata.msgId());
	event.setDetail(RasEvent::CATEGORY, metadata.category());
	event.setDetail(RasEvent::COMPONENT, metadata.component());
	event.setDetail(RasEvent::MESSAGE, metadata.message());
	event.setDetail(RasEvent::SEVERITY, metadata.severity());

        if (metadata.controlAction() != "")
	  event.setDetail(RasEvent::CONTROL_ACTION, metadata.controlAction());

	if (metadata.decoder() != "")
	  event.setDetail(RasEvent::DECODER, metadata.decoder());
	
	// add details
	const vector<string>& vars = metadata.vars();
	if (vars.size() == 0) return event;
	
	// replace the default detail key with its label from the metadata
	map<string,string>& details = event.getDetails();
	string detail = "DETAIL";
	char buf[12];
	for (unsigned i = 0; i < vars.size(); ++i) {
	  snprintf(buf, sizeof(buf), "%d", i);
	  string name =  detail + buf;
	  map<string,string>::iterator pos = details.find(name);
	  if (pos != details.end()) {  
	    details[vars[i]] = details[name];
	    details.erase(name);
	  }
	}
	return event;
}

MetadataAdder::MetadataAdder() : _name("MetadataAdder")
{
}

MetadataAdder::~MetadataAdder()
{
}
