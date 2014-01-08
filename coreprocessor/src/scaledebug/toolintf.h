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
// Includes
#include <ramdisk/include/services/ToolctlMessages.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/MessageUtility.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <iostream>
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

using namespace bgcios::toolctl;

// Function Prototypes
void sendMessage(ToolMessage& msg, bgcios::LocalStreamSocketPtr toolChannel);
bool receiveMessage(ToolMessage *msg, JobPtr job, bgcios::LocalStreamSocketPtr toolChannel);
void messageHandler(bgcios::LocalStreamSocketPtr toolChannel, JobPtr job);
int pollForData(JobPtr job);

void attachAck(AttachAckMessage *inMsg, bgcios::LocalStreamSocketPtr toolChannel, JobPtr job);
void detachAck(DetachAckMessage *inMsg, bgcios::LocalStreamSocketPtr toolChannel, JobPtr job);
void queryAck(QueryAckMessage *inMsg, bgcios::LocalStreamSocketPtr toolChannel, JobPtr job);
void updateAck(UpdateAckMessage *inMsg, JobPtr job);
void notify(NotifyMessage *inMsg, bgcios::LocalStreamSocketPtr toolChannel, JobPtr job);

void attach(JobPtr job);
void detach(JobPtr job);

