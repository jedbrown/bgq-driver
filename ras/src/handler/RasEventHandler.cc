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
#include "RasEventHandler.h"

#include <unistd.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdio.h>
using namespace std;


string RasEventHandler::logPrefix() { 
  time_t tod;			// output from time
  struct tm tod_tm;		// output from localtime
  char time_str[16];		// output from ctime
  // show the current time
  time(&tod);
  localtime_r(const_cast<const time_t*>(&tod), &tod_tm);
  strftime(time_str, sizeof(time_str), "%b %d %H:%M:%S", &tod_tm);
  return string(time_str);
}


