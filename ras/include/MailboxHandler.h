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
#ifndef MAILBOXHANDLER_H_
#define MAILBOXHANDLER_H_

#include "RasEventHandler.h"
#include "firmware/include/mailbox.h"

#include <string>

#define MAX_MAILBOX_LEN 4096
typedef struct MailBoxPayload_Common_RAS_t
{
  union {
    MailBoxPayload_RAS_t mboxBinary;
    MailBoxPayload_RAS_ASCII_t mboxAscii;
  };
  char text[MAX_MAILBOX_LEN];
}  __PACKED MailBoxPayload_Common_RAS_t;


/** 
 * \class MailboxHandler
 *
 */

class MailboxHandler : public RasEventHandler
{
public:
  /** \brief Handle the RasEvent.
   * Returns event 
   */
  RasEvent& handle(RasEvent& event, const RasEventMetadata& metadata);
  virtual const std::string& name() { return _name; }
  MailboxHandler();
  virtual ~MailboxHandler();
  void handleMailbox(RasEvent& event, const std::string& mboxPayload, const RasEventMetadata& metadata);
  void handleNonMailbox(RasEvent& event, const RasEventMetadata& metadata);

private:
  const std::string _name;

  void defaultAscii(RasEvent& event, const MailBoxPayload_Common_RAS_t& mbox, const RasEventMetadata& metadata);
  void defaultBinary(RasEvent& event, const MailBoxPayload_Common_RAS_t& mbox, const RasEventMetadata& metadata);
  void defaultNonMailbox(RasEvent& event, const RasEventMetadata& metadata);
  std::vector<std::string> tokenize(const std::string &rStr, const std::string &szDelimiters);
};

#endif /*MAILBOXHANDLER_H_*/
