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
/* (C) Copyright IBM Corp.  2011, 2011                              */
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
#include <string>
#include <ifaddrs.h>
#include <bgq_util/include/pthreadmutex.h>
#include <ramdisk/include/services/ToolctlMessages.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/MessageUtility.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/wait.h>
#include <sstream>
#include <vector>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <utility/include/Properties.h>
#include <utility/include/Log.h>
#include "Job.h"
#include "CoreFile.h"
#include <ramdisk/include/services/common/LocalSocket.h>
#include <ramdisk/include/services/common/PointerMap.h>

extern map<bgcios::LocalStreamSocketPtr, AttachAckMessage> attachMap;

extern int  connectToTool(int argc, char* argv[]);
extern bool doCommand(JobPtr job);
extern void sendResponse();
