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
#ifndef VARSUBSTITUTE_H_
#define VARSUBSTITUTE_H_

#include "RasEventHandler.h"

#include <set>
#include <string>

class VarSubstitute : public RasEventHandler
{
 public:
  virtual RasEvent& handle(RasEvent& event, const RasEventMetadata& metadata);
  virtual const std::string& name() { return _name; }
  VarSubstitute();
  virtual ~VarSubstitute();
 private:
  std::string _name;
  std::set<std::string> _std_keys;
};

#endif /*VARSUBSTITUTE_H_*/
