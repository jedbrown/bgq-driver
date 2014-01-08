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
#include "RasEvent.h"
#include "RasLog.h"
#include <iomanip>

using namespace std;

#ifdef __linux
LOG_DECLARE_FILE("ras");
#endif

// standard keys for values stored in the RasEvent map 
string RasEvent::MSG_ID         = "BG_ID";
string RasEvent::CATEGORY       = "BG_CAT";
string RasEvent::COMPONENT      = "BG_COMP";
string RasEvent::SEVERITY       = "BG_SEV";
string RasEvent::MESSAGE        = "BG_MSG";
string RasEvent::LOCATION       = "BG_LOC";
string RasEvent::MBOX_TYPE      = "BG_MBOX_TYPE";
string RasEvent::MBOX_PAYLOAD   = "BG_MBX";
string RasEvent::JTAG_PORT      = "BG_JTAG";
string RasEvent::CPU            = "BG_CPU";
string RasEvent::ECID           = "BG_ECID";
string RasEvent::SERIAL_NUMBER  = "BG_SN";
string RasEvent::COOKIE         = "BG_COOKIE";
string RasEvent::CONTROL_ACTION = "BG_CTL_ACT";
string RasEvent::DECODER        = "BG_DECODER";
string RasEvent::JOBID          = "BG_JOBID";
string RasEvent::BLOCKID        = "BG_BLOCKID";
string RasEvent::COUNT          = "BG_COUNT";


ostream& operator<< (ostream& o, const RasEvent& event) 
{
  char time_buf[256];
  timeval time = event.time();
  strftime(time_buf, sizeof(time_buf), "%F %T", localtime(&time.tv_sec));

  o << "<RasEvent time=\""
    << time_buf   << "\" "
    << "id=\"0x" << hex << setfill('0') << setw(8) << event.msgId() << dec << "\" "
    << "handled=\"" << event.handled() << "\" ";
  LOG_DEBUG_MSG("<RasEvent time=\"" << time_buf  << "\" " << "handled=\"" << event.handled() << "\" ");
  

  RasEvent& ev = const_cast<RasEvent&> (event);
  map<string,string> details = ev.getDetails();
  for (map<string,string>::const_iterator pos = details.begin(); pos != details.end(); ++pos) {
    o << pos->first << "=\"" << pos->second << "\" ";
    LOG_DEBUG_MSG(pos->first << "=\"" << pos->second << "\" ");
  }
  o << "/>" << endl;
  LOG_DEBUG_MSG("/>");
  return o;
} 

