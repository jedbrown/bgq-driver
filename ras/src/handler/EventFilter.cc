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
#include "EventFilter.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include "RasLog.h"

using namespace RasFilter;
using namespace std;

#ifdef __linux
LOG_DECLARE_FILE("ras");
#endif


RasEvent& EventFilter::handle(RasEvent& event, const RasEventMetadata& /*metadata*/)
{

  LOG_TRACE_MSG("EventFilter handling event id=" <<  event.getDetail(RasEvent::MSG_ID).c_str() << " msg=" <<  event.getDetail(RasEvent::MESSAGE).c_str());
  // TRACE(("EventFilter handling event id=%s msg=%s \n", event.getDetail(RasEvent::MSG_ID).c_str(),  event.getDetail(RasEvent::MESSAGE).c_str()));

  string id = event.getDetail(RasEvent::MSG_ID);
  map<string, string>& details = event.getDetails();
  for (unsigned i = 0; i < _specs.size(); ++i) {
    if (id == _specs[i]._id) {
      details[_specs[i]._key] = _specs[i]._value;
      LOG_INFO_MSG(" EventFilter setting " << _specs[i]._key.c_str() << "=" << _specs[i]._value.c_str() );
    } 
  }
  return event;
}

EventFilter::EventFilter(vector<BgRasEnvironments::BgRasEnvironment::RasEventChangeSpec>& specs) :  _name("EventFilter"), _specs(specs)
{
}

EventFilter::~EventFilter()
{
}
