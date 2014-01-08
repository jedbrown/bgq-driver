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
#include "RasEventHandlerChain.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include "RasLog.h"

using namespace std;

#ifdef __linux
LOG_DECLARE_FILE("ras");
#endif

pthread_mutex_t RasEventHandlerChain::chainLock = PTHREAD_MUTEX_INITIALIZER;

string RasEventHandlerChain::env = "PROD";
void RasEventHandlerChain::setEnvironment(string e) { 
  env = e; 
  LOG_TRACE_MSG("RasEventHandlerChain environment=" << env.c_str());
}
string RasEventHandlerChain::getEnvironment() { return env; }

string RasEventHandlerChain::filterFile = "";
void RasEventHandlerChain::setFilterFile(string file) { 
  filterFile = file; 
  LOG_TRACE_MSG("RasEventHandlerChain filter file=" << filterFile.c_str());
}
string RasEventHandlerChain::getFilterFile() { return filterFile; }

#ifdef __linux__
bgq::utility::Properties::ConstPtr RasEventHandlerChain::properties;
void RasEventHandlerChain::setProperties(const bgq::utility::Properties::ConstPtr props) { 
  properties = props; 
  if ( properties ) {
      LOG_TRACE_MSG("RasEventHandlerChain properties file=" << properties->getFilename());
  }
}
bgq::utility::Properties::ConstPtr RasEventHandlerChain::getProperties() { return properties; }
#endif

bool RasEventHandlerChain::initialized = false;

std::vector<RasEventHandler*> RasEventHandlerChain::handlers;

void RasEventHandlerChain::handle(RasEvent& event)
{  
  // only handle the event once 
  if (event.handled()) return; 

  RasEventHandlerChain::initChain();
  unsigned i = 0;
  bool md_ok = false;
  try {
    uint32_t msgId = event.msgId();
    stringstream stm;
    stm << hex << uppercase << setfill('0') << setw(8) << msgId;
    string id = stm.str();
    const RasEventMetadata& metadata = RasEventMetadata::factory(id);

    md_ok = true;

    for (i = 0; i<handlers.size(); ++i) {
      handlers[i]->handle(event, metadata);
    }

    event.setHandled(true);
  } catch(exception& e) {
    if (md_ok) {
      if (handlers[i] != NULL) {
        LOG_ERROR_MSG("RasEventHandlerChain handler " << handlers[i]->name() << " failed. " << e.what());
      }
      else {
        LOG_ERROR_MSG("RasEventHandlerChain handler index " << i << " yields null pointer. " << e.what());
      }
    }
    else {
      LOG_ERROR_MSG("RasEventHandlerChain failed getting metadata for RasEvent. " << e.what());
    }
    LOG_ERROR_MSG(event);
  }
}

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <xml/include/c_api/RasEnvironmentFilter.h>

#include "MetadataAdder.h"
#include "VarSubstitute.h"
#include "RasDecoder.h"
#include "RasSmartLock.h"

using namespace std;
void RasEventHandlerChain::initChain() {
  RasSmartLock lock(&chainLock);

  if (initialized) return;

  // Clear prior storage and handlers, in case this is not the first time it is being called.
  RasEventHandlerChain::clear();

  // read the ras properties file
  readRasProperties();

  // read the ras metadata files
  RasEventMetadata::readRasMetadata();

  // The handler to add metadata is first
  handlers.push_back(new MetadataAdder());

  // Read the RAS decoder files
  RasDecoder::readRasDecoder();

  // The handler for decoder 
  handlers.push_back(new RasDecoder());

  // read ras filters 
  readRasFilters();
    
  // The variable substitution handler is last
  handlers.push_back(new VarSubstitute());

  initialized = true;
}

#include "EventFilter.h"
using namespace RasFilter;

void  RasEventHandlerChain::readRasFilters() {
  string file = getFilterFile();
  if (file != "") {
    ifstream fin(file.c_str());
    if (fin) {
      BgRasEnvironments environs;
      bool ok = environs.read(fin);
      if (ok) {
	string current = getEnvironment();
	vector<BgRasEnvironments::BgRasEnvironment>& env = environs._environments;
	for (unsigned i = 0; i < env.size(); ++i) {
	  if (current == env[i]._environment) {
	    // we have an environment match, create a ras filter
	    handlers.push_back(new EventFilter(env[i]._changeSpecs));
	    break;
	  }
	}
      }
      else {
	LOG_ERROR_MSG("RasEventHandlerChain failed to load environmental ras filters");
      }
    }
    else {
      LOG_ERROR_MSG("RasEventHandlerChain cannot open ras environment filter file " << file);
    }
  }
}


#ifdef __linux__
void  RasEventHandlerChain::readRasProperties() {

  bgq::utility::Properties::ConstPtr props = getProperties();

  try {
    if (!props) {
      // Open default bg.properties file 
      props = bgq::utility::Properties::create();
    }
  } catch (exception& e) {
    LOG_ERROR_MSG("RasEventHandlerChain error: " << e.what());
  }

  if (!props) {
    // Can't find bg.properties file, set the default values 
    setEnvironment("PROD");
    setFilterFile("/bgsys/drivers/ppcfloor/ras/etc/ras_environment_filter.xml");
    RasEventMetadata::setInstallPath("/bgsys/drivers/ppcfloor");
    RasDecoder::setInstallLibPath("/bgsys/drivers/ppcfloor");
  }
  else {
    LOG_TRACE_MSG("RasEventHandlerChain ras property file=" << props->getFilename());

    // Set fields for ras section in bg.properties file 
    try {
      setEnvironment(props->getValue("ras", "environment"));
      setFilterFile(props->getValue("ras", "filter"));
      RasEventMetadata::setInstallPath(props->getValue("ras", "md_install_path"));
      RasDecoder::setInstallLibPath(props->getValue("ras", "decoder_install_path"));
      // RasEventMetadata::setTestPath(props->getValue("ras", "md_test_path"));
    }
    catch (std::invalid_argument &ia) {
      LOG_WARN_MSG("RasEventHandlerChain error: " << ia.what());
    }
  }

}
#else
void  RasEventHandlerChain::readRasProperties() {
  // Set the default values 
  setEnvironment("PROD");
  setFilterFile("");
  RasEventMetadata::setInstallPath("/bgsys/drivers/ppcfloor");
  RasDecoder::setInstallLibPath("/bgsys/drivers/ppcfloor");
}
#endif

void RasEventHandlerChain::clear() {
  //Clear all memory allocated during initChain()
  RasEventMetadata::clearMetadata();
  RasDecoder::clearDecoder();
  for (unsigned i=0; i<handlers.size(); ++i) {
    delete handlers[i];
  }
  handlers.clear();
  RasEventHandlerChain::reinitialized();
}
