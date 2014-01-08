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
#include <RasEventHandlerChain.h>
#include <RasDecoder.h>
#include "../RasLog.h"
using namespace std;

#ifdef __linux
LOG_DECLARE_FILE("ras");
#endif


void test_handler_chain(unsigned eventId)
{
  RasEventImpl event = RasEventImpl(eventId);
  RasEventMetadata::setTestPath("../../../work");
  RasEventHandlerChain::handle(event);
  LOG_INFO_MSG(event);
}

int main()
{
  string bg_props_file = "/bgsys/local/etc/bg.properties";
  bgq::utility::Properties properties(bg_props_file);
  bgq::utility::initializeLogging(properties);

  unsigned int ras_id = 0;
  while (cin) {
     cout << "enter a ras event id: ";
     cin >> hex >> ras_id;
     cout << endl;
     test_handler_chain(ras_id);
  }
  return 0;
}
