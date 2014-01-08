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
#ifndef RASEVENTHANDLER_H_
#define RASEVENTHANDLER_H_

#include <string>
#include "RasEvent.h"
#include "RasEventMetadata.h"


/** 
 * \class RasEventHandler
 *
 * RasEventHandler is responsible for handling a RasEvent 
 * which is an object that contains the details about a RasEvent. 
 * 
 * The handler can add and update event details.
 */

class RasEventHandler
{
public:
  /** 
   * Handle the RasEvent.
   * @return event which is the same event returned (ostream design pattern)
   */
  virtual RasEvent& handle(RasEvent& event, const RasEventMetadata& metadata) = 0;

  virtual ~RasEventHandler() {}
  
  /** 
   * Get the name of the handler.
   * @return string handler name
   */
  virtual const std::string& name() = 0;

  static std::string logPrefix();
};

#endif /*RASEVENTHANDLER_H_*/
