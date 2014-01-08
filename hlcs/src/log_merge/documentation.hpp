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


/*! \mainpage

\section Introduction

The log_merge utility can be used
to merge existing log files
or to monitor log files.

It's installed to &lt;floor&gt;/hlcs/bin/log_merge.


\section Configuration

The log_merge utility reads the
Blue Gene configuration file (bg.properties)
for configuration.

The only configuration option is "default_directory" in the "log_merge" section.
This is the directory that is used if no arguments
are given on the command line.
It's typically set to /bgsys/logs/BGQ.
If not present,
the default value of /bgsys/logs/BGQ is used.


\section Usage

<pre>
Usage: ./log_merge [OPTION]... [FILE or DIRECTORY]...
Merge Blue Gene log files.

Allowed options:
  --live                Live updates
  -m [ --map ]          Display short names for log files
  --start arg           Interval start time
  --end arg             Interval end time
  -0 [ --null ]         Read null-terminated file names from stdin
  --properties arg      Blue Gene configuration file
  --verbose arg         Logging configuration
  -h [ --help ]         Print this help text

Use a FILENAME of - to read file or directory names from stdin.
</pre>

The --start and --end options cannot be used with the --live option.

log_merge uses the standard bgq::utility::Properties program option
handling for --properties.

log_merge uses the standard bgq::utility::LoggingProgramOptions program option
handling for --verbose.


\section Modes

log_merge works in either "live" or "historical" mode.

In "historical" mode,
log_merge processes existing log files and merges
log records from these files.
This is the default mode.
This mode is useful for analyzing the
logs to debug a problem that occurred.

In "live" mode,
log_merge monitors existing log files or directories
for changes and displays any new log records.
This mode is used when the --live command-line option is present.
This mode is useful for monitoring the system
while it's running.


\section files Specifying input files

Input files can be specified on the command line as positional parameters
or read from the program's standard input.

To read the file name from the program's standard input,
use - or -0.
When using -, each filename is on a separate line.
When using -0, each filename ends with \\0 rather than \\n
(You can generate names like this using find -print0.)

If the name is for a directory, log_merge
will use all the files in that directory as the input files.


\section Filtering

When in historical mode,
it can be useful to view the entries
in the time range when the error occurred.
You can do this with log_merge by using the
--start and --end command line options.

The format for the --start and --end parameters is

<table>
<tr><th>Format</th><th>Notes</th></tr>
<tr><td>YYYY-mm-dd HH:MM:SS.sss</td><td></td></tr>
<tr><td>YYYY-mm-dd</td><td>midnight morning if --start, midnight night if --end</td></tr>
<tr><td>mm-dd</td><td>Current year, midnight morning if --start, midnight night if --end</td></tr>
<tr><td>HH:MM:SS.sss</td><td>Today</td></tr>
</table>


\section Input

The log_merge utility reads and parses Blue Gene log files.
These log files have a specific format to the log entries.

A BG log entry starts with a line beginning with a timestamp
and continues until the next line beginning with a timestamp.
(A log entry can span multiple lines.)

Here's an example of the format for a timestamp in BG/Q log files:
<pre>
2010-07-14 10:31:54.660
</pre>


\section Output

The log_merge utility gathers all the log entries and
outputs them in order.
Each log entry is output like this:

<pre>
&lt;label&gt;: &lt;log entry&gt;
</pre>

where the label is either the filename or
the filename mapped to a shorter string (if -m is used).


\subsection mapping Filename mapping

When filename mapping is enabled
(using the -m option),
the log_merge utility maps filenames as follows:

<table>
<tr><th>Filename pattern</th><th>Label</th></tr>
<tr><td>.*-bgagentd-.*</td><td>bgagentd      </td></tr>
<tr><td>.*-bgmaster_server-.*</td><td>bgmaster      </td></tr>
<tr><td>.*-mc_server-.*</td><td>mc_server     </td></tr>
<tr><td>.*-mmcs_server-.*</td><td>mmcs_server   </td></tr>
<tr><td>.*-runjob_mux.*</td><td>runjob_mux    </td></tr>
<tr><td>.*-runjob_server-.*</td><td>runjob_server </td></tr>
<tr><td>.*-Subnet.*</td><td>subnet_mc     </td></tr>
<tr><td>.*-bgws_server-.*</td><td>bgws_server   </td></tr>
</table>

Any file that doesn't match the pattern is not mapped.

 */
