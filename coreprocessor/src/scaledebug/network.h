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
#include <map>
#include <string>
#include <ifaddrs.h>
#include <bgq_util/include/pthreadmutex.h>
#include <utility/include/cxxsockets/ListenerSet.h>
#include <utility/include/cxxsockets/SecureTCPSocket.h>
#include <utility/include/cxxsockets/SockAddr.h>
#include <utility/include/cxxsockets/SockAddrList.h>
#include <utility/include/cxxsockets/TCPSocket.h>
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

using namespace std;

#ifndef USE_SECURE_CERTIFICATE
#define USE_SECURE_CERTIFICATE 0
#endif

extern int number_of_clients;

#if USE_SECURE_CERTIFICATE
extern std::vector<CxxSockets::SecureTCPSocketPtr> SocketVector;
#else
extern std::vector<CxxSockets::TCPSocketPtr> SocketVector;
#endif

int connectToTool(int argc, char* argv[]);
int sendCommand(const char* str);
typedef std::map<CxxSockets::TCPSocketPtr, std::string>  MsgMap;
int receiveReply(MsgMap& msgs);
void findIPs(std::vector<string>& IPs);
