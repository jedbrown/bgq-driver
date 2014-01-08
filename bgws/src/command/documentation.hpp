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

/*! \page commandClients Command Clients

Administrative commands:

- bgws_server_logging - Get and set BGWS server logging configuration.
- bgws_server_refresh_config - Cause the BGWS server to re-load it's configuration.
- bgws_server_status - Get BGWS server status.
- create_block - Create a block.
- delete_block - Delete a block (or blocks).
- list_blocks - Get blocks summary or block details.

Administrative commands are installed to <em>driver</em>/bgws/sbin with symlinks in <em>driver</em>/sbin.

User commands:

- list_jobs - Get jobs summary or job details.

User commands are installed to <em>driver</em>/bgws/bin with symlinks in <em>driver</em>/bin.

The commands all take a -h option that describes usage.


\section commandClientsConfiguration Configuration

The commands read configuration options from the BG configuration file (bg.properties). They read values from the bgws.commands section.

<pre>
[bgws.commands]
    # Settings for the BGWS commands.

base_url = https://localhost:32071/bg
    # base URL to BGWS server, defaults to https://localhost:32071/bg
</pre>

 */
