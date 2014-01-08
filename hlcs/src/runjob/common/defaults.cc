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
#include "common/defaults.h"

#include <fstream>

namespace runjob {
namespace defaults {

// multiplexer stuff
const std::string   MuxLocalSocket              = "runjob_mux";
const size_t        MuxClientBufferSize         = 512;
const size_t        MuxClientTimeout            = 10;
const std::string   MuxCommandService           = "26510";
const unsigned      MuxPerfCounterInterval      = 15;

// server stuff
const std::string   ServerMuxService            = "25510";
const std::string   ServerCommandService        = "24510";
const bool          ServerJobSimulation         = false;
const size_t        ServerMaxUserProcesses      = 256;
const unsigned      ServerPerfCounterInterval   = 30;
const std::string   ServerIosdIdCounterName     = "/tmp/runjob_server_iosd_counter";
const size_t        ServerIosdIdCounterSize     = 2048;
const unsigned      ServerKillJobTimeout        = 60;
const uint16_t      ServerJobctlPort            = 7002;
const uint16_t      ServerStdioPort             = 7003;
const unsigned      ServerJobctlHeartbeat       = 60;

// command stuff
const unsigned      CommandConnectTimeout       = 0;

// client stuff
const unsigned      ClientRanksPerNode          = 1;
const std::string   ClientMapping               = "ABCDET";
const int32_t       ClientStdinRank             = 0;

} // defaults
} // runjob
