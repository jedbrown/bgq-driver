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
#ifndef RUNJOB_COMMON_DEFAULTS_H
#define RUNJOB_COMMON_DEFAULTS_H

#include <string>

#include <sys/types.h>

#include <stdint.h>
#include <unistd.h>

namespace runjob {
namespace defaults {

// multiplexer stuff
extern const std::string    MuxLocalSocket;
extern const size_t         MuxClientBufferSize;
extern const size_t         MuxClientTimeout;
extern const std::string    MuxCommandService;
extern const unsigned       MuxPerfCounterInterval;

// server stuff
extern const std::string    ServerMuxService;
extern const std::string    ServerCommandService;
extern const bool           ServerJobSimulation;
extern const size_t         ServerMaxUserProcesses;
extern const unsigned       ServerPerfCounterInterval;
extern const std::string    ServerIosdIdCounterName;
extern const size_t         ServerIosdIdCounterSize;
extern const unsigned       ServerKillJobTimeout;
extern const uint16_t       ServerJobctlPort;
extern const uint16_t       ServerStdioPort;
extern const unsigned       ServerJobctlHeartbeat;

// command stuff
extern const unsigned       CommandConnectTimeout;

// client stuff
extern const unsigned       ClientRanksPerNode;
extern const std::string    ClientMapping;
extern const int32_t        ClientStdinRank;

} // defaults
} // runjob

#endif
