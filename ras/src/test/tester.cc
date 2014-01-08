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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

#include <iostream>

#include <RasEvent.h> 
#include <RasEventImpl.h>
#include <RasEventMetadata.h>
// #include <RasEventMetadataImpl.h>
#include <RasEventHandlerChain.h>
#include <RasDecoder.h>
#include "../RasLog.h"
using namespace std;

#ifdef __linux
LOG_DECLARE_FILE("ras");
#endif

/*
  <rasevent
    id="00000001"
    category="@TODO"
    component="Reserved1"
    severity="INFO"
    message="This is a test ras message, variation $(VARIATION)"
    description="This RAS is used for testing only."
    service_action="$(Diagnostics)"
    />
 */

/*
  <rasevent
    id="00000002"
    category="@TODO"
    component="Reserved1"
    severity="INFO"
    message="This is a test ras message, variation $(VARIATION)"
    description="This RAS is used for testing only."
    service_action="$(ThresholdPolicy)"
    control_action=""
    decoder="testDecoder"
    relevant_diags="small,medium"
    threshold_count="1"
    threshold_period="2 DAYS"
    />
 */

void test_ras_event_metadata(unsigned eventId)
{ 
  LOG_INFO_MSG("=========================================");
  LOG_INFO_MSG("RAS Event Metadata Test");
  const RasEventMetadata& metadata = RasEventMetadata::factory(eventId);
  LOG_INFO_MSG("\tMessage: " << metadata.message());
  LOG_INFO_MSG("\tSeverity: " << metadata.severity());
  LOG_INFO_MSG("Test RAS Event Metadata PASSED");
  LOG_INFO_MSG("=========================================");
}

void test_handler_chain(unsigned eventId)
{
  LOG_INFO_MSG("=========================================");
  LOG_INFO_MSG("RAS Event Handler Chain Test");
  RasEventImpl event = RasEventImpl(eventId);
  // event.setDetail("DETAIL0", "1");
  event.setDetail("VARIATION", "1");
  RasEventMetadata::setTestPath("../../../work");
  LOG_INFO_MSG("Before chain: " << event);
  RasEventHandlerChain::handle(event);
  LOG_INFO_MSG("After chain: " << event);
  string msg = event.getDetail(RasEvent::MESSAGE);
  assert(msg == "This is a test ras message, variation 1");
  LOG_INFO_MSG("Test RAS Event Handler Chain PASSED");
  LOG_INFO_MSG("=========================================");
}

void test_decoder(unsigned eventId)
{
  LOG_INFO_MSG("=========================================");
  LOG_INFO_MSG("RAS Decoder Test");
  RasEventImpl event = RasEventImpl(eventId);
  // event.setDetail("DETAIL0", "1");
  event.setDetail("VARIATION", "2");
  RasEventMetadata::setTestPath("../../../work");
  RasDecoder::setTestLibPath("../../../work");
  LOG_INFO_MSG("Before chain: " << event);
  RasEventHandlerChain::handle(event);
  LOG_INFO_MSG("After chain: " << event);
  string msg = event.getDetail(RasEvent::MESSAGE);
  assert(msg == "This is a test ras message, variation 2");
  LOG_INFO_MSG("Test RAS Decoder PASSED");
  LOG_INFO_MSG("=========================================");
}

void test_filter(unsigned eventId)
{
  LOG_INFO_MSG("=========================================");
  LOG_INFO_MSG("RAS Environment Filter Test");
  RasEventImpl event = RasEventImpl(eventId);
  event.setDetail("VARIATION", "3");
  RasEventHandlerChain::reinitialized();
  string bg_props_file = "test.properties";
  bgq::utility::Properties::ConstPtr properties = bgq::utility::Properties::create(bg_props_file);
  RasEventHandlerChain::setProperties(properties);
  RasEventHandlerChain::initChain();
  LOG_INFO_MSG("Before chain: " << event);
  RasEventHandlerChain::handle(event);
  LOG_INFO_MSG("After chain: " << event);
  string sev = event.getDetail(RasEvent::SEVERITY);
  assert(sev == "WARN"); 
  string msg = event.getDetail(RasEvent::MESSAGE);
  assert(msg == "This is a test ras message, variation 3");
  LOG_INFO_MSG("Test RAS Environment Filter PASSED");
  LOG_INFO_MSG("=========================================");
}

int main()
{
  // Initialize logging
  string bg_props_file = "test.properties";
  bgq::utility::Properties properties(bg_props_file);
  bgq::utility::initializeLogging(properties);

  LOG_DEBUG_MSG("RAS Test...");
  unsigned int ras_id1 = 0x00000001;
  unsigned int ras_id2 = 0x00000002;
  // test_ras_event_metadata(ras_id1);
  test_handler_chain(ras_id1);
  test_decoder(ras_id2);
  test_filter(ras_id1);
  RasEventHandlerChain::clear();
  return 0;
}
