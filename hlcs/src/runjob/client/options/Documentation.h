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
/*!
 * \page runjob runjob
 *
 * BG/Q job submission interface
 *
 * \section SYNOPSIS
 *
 * runjob [OPTIONS] : exe [arg1 arg2 ... argn]
 *
 * Launch exe with optional arguments on a compute block.
 *
 * \section DESCRIPTION
 *
 * runjob is the interface to launch jobs.  It is conceptually the same as mpirun from previous
 * Blue Gene software releases because it acts as a shadow process for the job's lifetime.  It's
 * important to note that runjob has one significant difference compared to mpirun in that it's 
 * not a scheduler.  It does not provide any queuing support, nor any support for creating or
 * booting blocks.  If the block that is requested to run a job is not initialized, runjob immediately
 * terminates with a descriptive error message.
 *
 * This documentation was generated for driver DRIVER_NAME with revision VERSION_NUMBER on DOCUMENTATION_DATE.
 *
 * \section OPTIONS
 *
 * All arguments can also be specified by environment variables by prefixing RUNJOB_ to their
 * name.  For example, the --exe argument can be specified with RUNJOB_EXE.  Arguments will always
 * take precedence over their environmental equivalent. For arguments containing a dash (-) character,
 * the corresponding environment variable should use an underscore. For example, the --ranks-per-node=8
 * argument can be specified with RUNJOB_RANKS_PER_NODE=8. Arguments with implicit boolean values like
 * --raise can be specified with RUNJOB_RAISE=true or RUNJOB_RAISE=1.
 *
 * \subsection exe --exe
 * path name for the executable to run. Your user id must have permission to read and execute
 * this executable. The path can be an absolute path or a relative path to --cwd. This value
 * can also be specified as the first positional parameter after the : token. Note the total 
 * length of all --args and --exe arguments must be less than LOAD_JOB_ARGS_SIZE characters. The 
 * executable must be less than EXE_SIZE characters.
 *
 * This parameter is required.
 *
 * for example:
 *
 * runjob --block R00-M0 : /home/user/exe
 *
 * With the executable specified as the first positional argument, is equivalent to
 *
 * runjob --block R00-M0 --exe /home/user/exe
 *
 * \subsection args --args
 * arguments for the executable specified by --exe.  Multiple tokens to --args are split on spaces until
 * another argument is encountered.  To retain spaces in your tokens, enclose them in quotations. Note that
 * multiple --args parameters must be used to disambiguate between runjob arguments and application arguments.
 * Arguments can also be specified after the first positional executable parameter, in which case
 * disambiguation between runjob arguments and application arguments is not necessary.
 *
 * Arguments are composing, their values are combined from both command line parameters and 
 * environments (RUNJOB_ARGS) to generate the final set. Note that all --exe and --args parameters 
 * must be less than LOAD_JOB_ARGS_SIZE characters.
 *
 * for example:
 *
 * runjob --block R00-M0 : a.out hello world
 *
 * is equivalent to
 *
 * runjob --block R00-M0 --args hello world --exe a.out
 *
 * and
 *
 * runjob --block R00-M0 --args --one foo --args --two bar --exe a.out
 *
 * is equivalent to
 *
 * runjob --block R00-M0 : a.out --one foo --two bar
 *
 * \subsection envs --envs
 * environment variables to export. This value must contain an equals character that is not the first
 * character of the string. Multiple tokens are split on spaces until another argument is 
 * encountered. The leading character of an environment variable name cannot be a digit or a space.
 * To retain spaces in your environment variables enclose them in double quotes.
 * Environment variables are composing, their values are combined from both
 * arguments and environments to generate the final set. Note the total length of all environment 
 * variables must be less than LOAD_JOB_ENVS_SIZE characters, this includes the environment variable 
 * name and the equals character.
 *
 * \subsection exp-env --exp-env
 * export an environment variable from the current environment to the job. Multiple tokens are
 * split on spaces. The variable must be present in the current environment.
 *
 * \subsection env-all --env-all
 * export all environment variables from the current environment to the job. Note this may
 * quickly increase the total size of environment variables past the LOAD_JOB_ENVS_SIZE
 * character limit. Use this argument with caution.
 *
 * \subsection cwd --cwd
 * The current working directory for the job. This value must be a fully qualified path. The default value
 * is the working directory when runjob is started. Must be less than WORKING_DIRECTORY_SIZE characters.
 *
 * \subsection timeout --timeout
 * When the job starts running, deliver a SIGKILL after the specified number of seconds.
 *
 * \subsection block --block
 * The compute block ID on which to run the job.  The block must be initialized prior to invoking
 * runjob. If the system administrator has configured a job scheduler, this parameter may be
 * changed before the job starts. This parameter must be less than BLOCK_SIZE characters.
 *
 * Also see --corner and --shape for requesting a job use sub-block resources.
 *
 * \subsection corner --corner
 *
 * Location string of a single node *or* single core within the block to represent the corner of
 * a sub-block job. If the system administrator has configured a job scheduler, this parameter
 * may be changed before the job starts.
 *
 * Example:
 *  - R00-M0-N00-J00 (node J00 on board N00 in midplane R00-M0)
 *  - R00-M1-N12-J05-C00 (core 0 on node J05 on board N12 in midplane R00-M1)
 *
 * Core corner locations cannot use a shape or alternate ranks per node, therefore --shape and --ranks-per-node
 * are both ignored when specified with a core corner location. Node corner locations require a --shape argument. 
 * Multiple sub-node jobs on a single node using core corner locations are limited to a single user.
 *
 * \subsection shape --shape
 *
 * 5 dimensional AxBxCxDxE torus shape of a sub-block job, starting from a corner given by --corner. 
 * Must be equal to or less than a midplane (4x4x4x4x2) in size and cannot span multiple midplanes.
 * Shapes also cannot wrap around torus dimensions. All dimensions must be a power of two: either 1,
 * 2, or 4 nodes in size. If the system administrator has configured a job scheduler, this parameter 
 * may be changed before the job starts.
 *
 * Requires --corner and --block to be specified as well.
 *
 * Some commonly used shapes:
 * - 32  nodes 2x2x2x2x2
 * - 64  nodes 2x2x4x2x2
 * - 128 nodes 2x2x4x4x2
 * - 256 nodes 4x2x4x4x2
 *
 * Note: shapes are not limited to these sizes, they are described here as a guideline.
 *
 * \subsection ranks --ranks-per-node | -p
 * Number of ranks per compute node. Valid values are 1, 2, 4, 8, 16, 32, and 64.
 *
 * \subsection np --np | -n
 * Number of ranks in the entire job. The default value is the entire size of the block or sub-block shape.
 * The np value must be equal to or less than the block (or sub-block shape) size multiplied by the
 * ranks per node value.
 *
 * \subsection mapping --mapping
 * Permutation of ABCDET or a path to a mapping file containing coordinates for each rank.
 * If the path is relative, the combination of --cwd and --mapping must be less than MAPPING_SIZE characters. 
 * If the path is absolute, --mapping must be less than MAPPING_SIZE characters.  The syntax of a mapping 
 * file is 6 columns separated by white space. The columns indicate the A,B,C,D,E,T coordinates, and the 
 * line number is the rank. Comments can be used anywhere in the line with the # character, any text after 
 * the # is ignored. Blank lines are also ignored. An example
 *
\verbatim
0 0 0 0 0 0 # rank 0
0 0 0 0 1 0 # rank 1
0 0 0 1 0 0 # rank 2
0 0 0 1 1 0 # rank 3
\endverbatim
 *
 * \subsection label --label
 * Enable or disable standard output and standard error prefix labels. Values are none, short, or long. None
 * omits all prefixes. Short includes only the rank. Long includes the source in addition
 * to the rank in square brackets. The default value is none, the implicit value is long. Example:
 *
 * --label short
\verbatim
1: hello world
\endverbatim
 *
 * --label long
\verbatim
stdout[1]: hello world
\endverbatim
 *
 * --label
\verbatim
stdout[1]: hello world
\endverbatim
 *
 * --label none
\verbatim
hello world
\endverbatim
 *
 * \subsection strace --strace
 * Enable or disable system call tracing. Values are none, or n where n is a specific rank to enable
 * the tracing. The rank must be a valid rank in the job.
 *
 * \subsection start-tool --start-tool
 *
 * Fully qualified path to the tool daemon to launch before the job starts. The combination of
 * this value and --tool-args must be less than TOOL_ARGS_SIZE characters. Tools cannot be started
 * for sub-node jobs. For interactive jobs where standard input is a tty, a proctable prompt will be
 * presented allowing you to query the contents of the proctable once the tool has been started.
 *
 * \note When using a mapping file (see --mapping) this file must be readable by the runjob_server (typically
 * the bgqadmin uid) to correctly calculate location to rank mappings.
 *
 * \subsection tool-args --tool-args
 *
 * Single token containing arguments to be passed to the tool daemons. This token is split
 * on spaces. The --start-tool option must be given with this option. The combination of this value and 
 * --start-tool must be less than TOOL_ARGS_SIZE characters.
 *
 * \subsection tool-subset --tool-subset
 *
 * A subset specification is a single token that consists of a space separated list. Each element in the
 * list can have one of three formats:
 *
 * - rank1 : single rank
 * - rank1-rank2 : all ranks between rank1 and rank2
 * - rank1-rank2:stride : every strideth rank between rank1 and rank2
 *
 * A rank specification can either be a number or the special keywords $max or max. Both of them represent
 * the last rank participating in the job. The $ character is optional to prevent shell escaping. This subset
 * specification will restrict the I/O nodes used for launching the tool. Note each element in the 
 * specification must be in increasing order, and cannot have any overlapping ranks. The subset must specify 
 * at least one rank that is participating in the job. The --start-tool option must be given with this 
 * option. If not specified, the default value is 0-$max.
 *
 * \subsection stdinrank --stdinrank
 * The rank to deliver standard input to. Must be a valid rank in the job.
 *
 * \subsection raise --raise
 * 
 * If the job abnormally terminates by a signal, re-raise that signal when runjob terminates.
 * 
 * \subsection help --help | -h
 *
 * display help text and exit.
 *
 * \subsection version --version | -v
 *
 * display version information and exit.
 *
 * \subsection verbose --verbose
 *
 * \if MAN
 * \verbinclude verbose.man
 * \else
 * \htmlinclude verbose.html
 * \endif
 *
 * \section input STANDARD INPUT, OUTPUT, and ERROR
 *
 * After a job starts, standard input, output, and error are transparently forwarded by runjob on
 * behalf of the application. Each line of output or error can be optionally prefixed with the rank
 * using the --label option. Note that input is only forwarded to the application when requested. In
 * other words
 *
 * read( STDIN_FILENO, buf, 1024 );
 *
 * by the rank specified using --stdinrank triggers a corresponding read system call by runjob. For
 * ranks other than --stdinrank, a read system call will always return 0. Note that when runjob's
 * standard input file descriptor is a tty, line buffering is enabled. Otherwise, block buffering 
 * is used.
 *
 * \section signals SIGNAL HANDLING
 *
 * SIGINT and SIGXPU signals are handled by runjob. Delivering either one causes runjob to deliver
 * a SIGKILL to the job. When a SIGKILL is delivered to the job, a 60 second kill timer starts. If that
 * time period elapses before the job has terminated on its own, the job is forcefully terminated leaving
 * the compute nodes in use unavailable for future jobs until the block is rebooted.
 *
 * Signals other than SIGINT or SIGXPU are not handled by runjob, they can be delivered to the job using
 * kill_job. Similarly, the kill timeout can be changed when using kill_job instead of delivering a
 * signal to runjob. Successive signals sent to runjob will not cause additional SIGKILL signals to be
 * delivered to the job. Only the first one causes the kill timer to start. 
 *
 * \section termination JOB TERMINATION
 *
 * After a job starts, if any rank in the job terminates normally with exit(1), the remaining ranks will 
 * be delivered SIGTERM. If the any rank in the job abnormally terminates by a signal, the remaining ranks 
 * will be delivered a SIGKILL. 
 *  
 * \section notes NOTES
 *
 * This command uses a local (AF_UNIX) socket to the runjob_mux process. If this connection is prematurely
 * closed due to the process abnormally terminating, the job is delivered a SIGKILL signal.
 *
 * \section exit EXIT STATUS
 *
 * If the job fails to start for whatever reason, the exit status will be EXIT_FAILURE and an informative
 * message will be logged indicating the failure. After a job starts, the exit status is the first non-zero 
 * exit status reported by any rank in the job. For non-zero exit status values, a message will be logged 
 * indicating which rank caused the termination. When a rank terminates with an unhandled signal, runjob can
 * optionally re-raise this signal using the --raise option. If --raise is not given, the exit status will 
 * be 128 + the signal number. An informative message is also logged indicating which rank generated the 
 * signal.
 *
 * If delivering a SIGKILL times out (see kill_job) the exit status will be EXIT_FAILURE and an informative
 * message will be logged.
 *
 * \section author AUTHOR
 *
 * IBM
 *
 * \section copyright COPYRIGHT
 *
 * © Copyright IBM Corp. 2010, 2011
 *
 * \section also SEE ALSO
 *
 * - \link kill_job kill_job \endlink
 * - \link locate_rank locate_rank \endlink
 * - \link runjob_mux runjob_mux \endlink
 * - \link runjob_server runjob_server \endlink
 */


