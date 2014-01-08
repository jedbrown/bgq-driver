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

#ifndef MASTER_SERVER_RAS_H
#define MASTER_SERVER_RAS_H

/**
<rasevent
  id="00030000"
  category="Process"
  component="BGMASTER"
  severity="INFO"
  message="bgmaster_server has been started in process $(PID)"
  description="bgmaster_server has been started."
  service_action="None"
 />
<rasevent
  id="00030001"
  category="Process"
  component="BGMASTER"
  severity="INFO"
  message="bgmaster_server process $(PID) stopped"
  description="bgmaster_server has been stopped"
  service_action="None"
 />
<rasevent
  id="00030002"
  category="Process"
  component="BGMASTER"
  severity="INFO"
  message="bgmaster_server started binary $(BIN) for alias $(ALIAS)"
  description="bgmaster_server has started a binary"
  service_action="None"
 />
<rasevent
  id="00030003"
  category="Process"
  component="BGMASTER"
  severity="INFO"
  message="bgmaster_server stopped binary $(BIN) for alias $(ALIAS) with signal $(SIGNAL)."
  description="bgmaster_server has stopped a binary"
  service_action="None"
 />
<rasevent
  id="00030004"
  category="Process"
  component="BGMASTER"
  severity="WARN"
  threshold_count="1"
  message="bgmaster_server has detected a failure of binary $(BIN) for alias $(ALIAS) with signal $(SIGNAL) and exit status $(ESTAT). Error message is $(EMSG)."
  description="bgmaster_server has detected a failure of a managed binary"
  service_action="Check logs and restart the binary or allow the policy to manage it."
 />
<rasevent
  id="00030005"
  category="Process"
  component="BGMASTER"
  severity="INFO"
  message="bgmaster_server has executed a restart policy for alias $(ALIAS)"
  description="bgmaster_server has executed a restart policy."
  service_action="None"
 />
<rasevent
  id="00030006"
  category="Process"
  component="BGMASTER"
  severity="INFO"
  message="bgmaster_server has executed a failover policy for alias $(ALIAS) from $(SOURCE) to $(TARGET)"
  description="bgmaster_server has executed a failover policy."
  service_action="None"
 />
<rasevent
  id="00030007"
  category="Process"
  component="BGMASTER"
  severity="FATAL"
  threshold_count="1"
  message="bgmaster_server has detected a failure of bgagentd $(AGENT_ID)"
  description="bgmaster_server has detected a failed bgagentd"
  service_action="None"
 />
<rasevent
  id="00030008"
  category="Process"
  component="BGMASTER"
  severity="INFO"
  message="bgmaster_server has been requested to end bgagentd $(AGENT_ID)"
  description="bgmaster_server has been requested to end bgagentd."
  service_action="None"
 />
<rasevent
  id="00030009"
  category="Process"
  component="BGMASTER"
  severity="FATAL"
  threshold_count="1"
  message="bgmaster_server process $(PID) has failed with signal $(SIGNAL)"
  description="bgmaster_server failed"
  service_action="None"
 />
<rasevent
  id="00030010"
  category="Process"
  component="BGMASTER"
  severity="FATAL"
  message="bgmaster_server process $(PID) has failed with a configuration error $(ERROR)"
  description="bgmaster_server failed"
  service_action="None"
 />
<rasevent
  id="00030011"
  category="Process"
  component="BGMASTER"
  severity="INFO"
  message="bgmaster_server failed to start alias $(ALIAS)"
  description="bgmaster_server has failed to start a binary for an alias"
  service_action="Check bg.properties configuration."
 />
*/

const static unsigned int MASTER_STARTUP_RAS = 0x00030000;
const static unsigned int MASTER_SHUTDOWN_RAS = 0x00030001;
const static unsigned int BINARY_START_RAS = 0x00030002;
const static unsigned int BINARY_STOP_RAS = 0x00030003;
const static unsigned int BINARY_FAIL_RAS = 0x00030004;
const static unsigned int ALIAS_RESTART_RAS = 0x00030005;
const static unsigned int ALIAS_FAILOVER_RAS = 0x00030006;
const static unsigned int AGENT_FAIL_RAS = 0x00030007;
const static unsigned int AGENT_KILL_RAS = 0x00030008;
const static unsigned int MASTER_FAIL_RAS = 0x00030009;
const static unsigned int MASTER_CONFIG_RAS = 0x00030010;
const static unsigned int ALIAS_FAIL_RAS = 0x00030011;
const static unsigned int AUTHORITY_FAIL_RAS = 0x00063001;
#endif
