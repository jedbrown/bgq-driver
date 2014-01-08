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
#include <sstream>
#include <map>
#include <ctime>
#include <cassert>
#include <stdarg.h>

#include <RasEvent.h>
#include <RasDecoder.h>
#include <RasEventImpl.h>
#include <RasEventHandlerChain.h>
#include <MailboxHandler.h>
#include "../RasLog.h"

using namespace std;

#ifdef __linux
LOG_DECLARE_FILE("ras");
#endif

/*
  <rasevent
    id="00000010"
    category="@TODO"
    component="Reserved1"
    severity="INFO"
    message="This is a test ras message for binary mailbox. VAR1=$(VAR1)  VAR2=$(VAR2)."
    description="This RAS is used for testing only."
    service_action="None"
    control_action=""
    decoder="testDecoder"
    relevant_diags=""
    />
 */

/*
  <rasevent
    id="00000011"
    category="@TODO"
    component="Reserved1"
    severity="INFO"
    message="This is a test ras message for ASCII mailbox. VAR1=$(VAR1)."
    description="This RAS is used for testing only."
    service_action="None"
    control_action=""
    decoder=""
    relevant_diags=""
    />
 */

/*
  Sample call for Ras Event id="00000010":
    ./testMailbox BINARY '0000000000000000 0000001000000002 0000000a00000011 0000000b00000022'
                          UUUUUUUUUUUUUUUU MMMMMMMMRRRRNNNN DDDDDDDDDDDDDDDD DDDDDDDDDDDDDDDD

    ./testMailbox ASCII '0000000000000000 00000011 Test ASCII RAS Message'
                         UUUUUUUUUUUUUUUU MMMMMMMM AAAAAAAAAAAAAAAAAAAAAA
                         
     U = UCI (8 bytes)
     M = message ID (4 bytes)
     R = reserved (2 bytes)
     N = number of details (2 ytes)
     D = detail (8 bytes)
     A = Ascii string (null terminated)
*/

void test_mailbox_binary(string mbox)
{
  LOG_INFO_MSG("=========================================");
  
  LOG_INFO_MSG("Decoding BINARY MBOX string ...");
  istringstream in(mbox,istringstream::in);

  uint64_t uci;
  uint64_t descriptor;

  in >> hex >> uci; // not used further here
  in >> hex >> descriptor;

  uint32_t message_id = descriptor >> 32;

  cout.width(8);
  LOG_INFO_MSG("Creating TestRasEvent message_id=" << hex << message_id);
  RasEventImpl tst_event = RasEventImpl(message_id );
  tst_event.setDetail(RasEvent::MBOX_TYPE, "BINARY");
  tst_event.setDetail(RasEvent::MBOX_PAYLOAD, mbox);

  LOG_INFO_MSG("Running RAS Handler Chain ...");
  RasEventMetadata::setTestPath("../../../work");
  RasDecoder::setTestLibPath("../../../work");
  RasEventHandlerChain::handle(tst_event);

  LOG_INFO_MSG(tst_event);

  LOG_INFO_MSG("=========================================");	
}

void test_mailbox_ascii(string mbox)
{
  LOG_INFO_MSG("=========================================");
  
  LOG_INFO_MSG("Decoding ASCII MBOX string ...");
  istringstream in(mbox,istringstream::in);

  uint64_t uci;
  uint64_t descriptor;

  in >> hex >> uci; // not used further here
  in >> hex >> descriptor;

  uint32_t message_id = descriptor;

  cout.width(8);
  LOG_INFO_MSG("Creating TestRasEvent message_id=" << hex << message_id);
  RasEventImpl tst_event = RasEventImpl(message_id );
  tst_event.setDetail(RasEvent::MBOX_TYPE, "ASCII");
  tst_event.setDetail(RasEvent::MBOX_PAYLOAD, mbox);

  LOG_INFO_MSG("Running RAS Handler Chain ...");
  RasEventMetadata::setTestPath("../../../work");
  RasDecoder::setTestLibPath("../../../work");
  RasEventHandlerChain::handle(tst_event);

  LOG_INFO_MSG(tst_event);

  LOG_INFO_MSG("=========================================");	
}

void usage() {
  cout << "./testMailbox BINARY|ASCII 'MBX payload string'" << endl;
  cout << "  Where the MBX payload string is the value of the MBX key in a Ras Event (in quotes)." << endl;
  cout << "  For example: " << endl;
  cout << "    ./testMailbox BINARY '0000000000000000 0000001000000002 0000000a00000011 0000000b00000022'" << endl;
  cout << "    ./testMailbox ASCII '0000000000000000 00000011 Test Message'" << endl;
}

int main(int argc, char* argv[])
{
  string bg_props_file = "test.properties";
  bgq::utility::Properties properties(bg_props_file);
  bgq::utility::initializeLogging(properties);

  if (argc != 3 ) {
    usage();
    return 1;
  }

  if (strcmp(argv[1], "BINARY") == 0)
    test_mailbox_binary(argv[2]);
  else if (strcmp(argv[1], "ASCII") == 0)
    test_mailbox_ascii(argv[2]);

  RasEventHandlerChain::clear();
  return 0;
}
