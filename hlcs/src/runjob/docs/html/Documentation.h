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
 * \mainpage notitle
 * \author Sam Miller
 *
 * This documentation was generated for driver DRIVER_NAME with revision VERSION_NUMBER.
 *
 * \section toc Table of Contents
 *
 * - \ref introduction
 * - \ref architecture
 * - \ref differences
 * - \ref database
 * - \ref information
 *
 * \section introduction Introduction
 *
 * %runjob is an umbrella term for the BlueGene/Q job submission architecture, its 
 * BG/P equivalent is mpirun.  It has very similar goals compared mpirun, albeit 
 * implemented significantly different. The goals and design requirements are as 
 * follows:
 *
 *  - facilitate remote job launch for the BG/Q control system
 *  - leaverage the services oriented design of the Common I/O Services (CIOS) I/O node components
 *  - act as a shadow process for the entire duration of the job
 *  - replaces numerous methods for job submission from BG/P into a single  interface
 *      - mpirun for MPI jobs
 *      - submit for single node HTC jobs
 *      - submit_job from a mmcs_db_console
 *      - \htmlonly<strike>\endhtmlonlympiexec for Multiple Program Multiple Data (MPMD)
 *      jobs\htmlonly</strike>\endhtmlonly (MPMD will be supported after release 1 of BG/Q software)
 *  - fast job launch.  The Sequoia SOW requires a 8192 node job to be launched in under 3 seconds.
 *  - transparently relay the job's standard input, output, and error
 *  - relay job exit status to the user, including abnormal termination if requested
 *  - act as a gateway for debuggers such as TotalView to attach to jobs prior to and during
 *  their execution.
 *
 * \section architecture Architecture
 *
 * The %runjob architecture consists of three components, they are shown in the figure below:
 *
 * \image html design.jpg "BlueGene/Q job submission architecture"
 *
 *  - server
 *      - inserts, updates, and removes job entries from the database
 *      - communicates with daemons on the I/O nodes to start and stop jobs
 *      - communicates with multiplexer
 *  - multiplexer (mux)
 *      - multiplexes multiple job requests into a single connection to the server
 *      - facilitates failover
 *      - communicates with both the %runjob server and client
 *      - plugin functionality to interact with job schedulers
 *  - client
 *      - interface for users to launch jobs
 *      - communicates with multiplexer
 *  - Common I/O Services (CIOS)
 *      - interface with the Compute Node Kernel (CNK)
 *      - provides function shipping for I/O operations
 *      - tool launching support for debuggers
 *
 * \subsection internals Internals
 *
 * All of the %runjob components are heavily based on the
 * \htmlonly
 * <a href=http://www.boost.org/doc/libs/1_42_0/doc/html/boost_asio.html>Boost Asynchronous I/O</a>
 * \endhtmlonly
 * library and the
 * \htmlonly
 * <a href=http://en.wikipedia.org/wiki/Proactor_pattern> Proactor Design Pattern</a>.
 * \endhtmlonly
 * However, several %runjob components use the asio library in a synchronous manner. Most
 * notably, all of the \link runjob::server::commands server\endlink and
 * \link runjob::mux::commands mux\endlink commands do this.
 *
 * Communication between each component is achieved through a request/response protocol using
 * objects serialized with the
 * \htmlonly <a href=http://www.boost.org/doc/libs/1_42_0/libs/serialization/doc/index.html>Boost.Serilization library</a>.
 * \endhtmlonly
 * See runjob::message for detailed information.
 *
 * \section differences Differences compared to mpirun and submit
 *
 * The BG/P job submission architecture design is shown in the figure below:
 *
 * \image html design_bgp.png "BlueGene/P job submission architecture"
 *
 * The %runjob architecture is noticeably different.  Foremost, %runjob uses a client/server
 * architecture rather than a frontend/backend design like mpirun.  From a high level, the
 * mapping of component roles can be loosely interpreted as follows
 *
 *  - mpirun -> client
 *  - mpirun_be -> mux
 *  - mpirund -> server
 *  - mmcs -> server
 *
 * The challenge protocol used between mpirun and mpirund has been replaced by a local unix
 * socket used between the %runjob client and mux, as well as a SSL connection between the
 * mux and the server.  The %runjob mux does not interact with the database like mpirun_be
 * did, primarily because it does not use the database as an IPC mechanim.
 *
 * \subsection booting_creating_blocks Booting and Creating Blocks
 *
 * The most noticeable difference of %runjob compared to mpirun is its inability to create and boot blocks.
 * This feature was removed on purpose after discussions within the BG/Q System Management
 * working group. It was decided any scheduling-like behavior (such as creating blocks) belongs in the
 * domain of a scheduler and not in another place like %runjob. Therefore, %runjob requires the block
 * to be already initialized prior to its invocation.
 * 
 * \subsection arguments Arguments
 *
 * An important difference between mpirun and %runjob is the command line argument processing.
 * Seasoned mpirun users may launch a job as follows:
 *
\verbatim
mpirun -partition R00-M0 -exe /home/user/hello
\endverbatim
 * Whereas the equivalent %runjob invocation would be
\verbatim
runjob --block R00-M0 --exe /home/user/hello
\endverbatim
 * 
 * You'll notice %runjob requires two dashes instead of one before each argument. The vast majority
 * of arguments used to describe a job are similar between mpirun, submit, and %runjob, the differences are
 * in the table below. Consult the \ref runjob documentation for in-depth explanation of the arguments
 * and their supported values.
 *
 *  <table border=1 cellspacing=0 cellpadding=0> 
 *  <tr> 
 *     <td><b> BG/P mpirun </b></td> 
 *     <td><b> BG/P submit </b></td> 
 *     <td><b> BG/Q %runjob </b></td> 
 *     <td><b> notes </b></td>
 *  </tr> 
 *  <tr> 
 *     <td> -exe </td> 
 *     <td> -exe </td> 
 *     <td> --exe </td> 
 *     <td></td>
 *  </tr> 
 *  <tr> 
 *     <td> -args </td> 
 *     <td> -args </td> 
 *     <td> --args </td> 
 *     <td>multiple tokens instead of a single token <br>enclosed with double quotation marks</td>
 *  </tr>
 *  <tr> 
 *     <td> -env </td> 
 *     <td> -env </td> 
 *     <td> --envs </td> 
 *     <td>multiple tokens instead of a single token <br>enclosed with double quotation marks</td>
 *  </tr>
 *  <tr> 
 *     <td> -exp_env </td> 
 *     <td> -exp_env </td> 
 *     <td> --exp-env </td> 
 *     <td>multiple tokens instead of a single token <br>enclosed with double quotation marks</td>
 *  </tr>
 *  <tr> 
 *     <td> -env_all </td> 
 *     <td> -env_all </td> 
 *     <td> --env-all </td> 
 *     <td></td>
 *  </tr>
 *  <tr> 
 *     <td> -partition </td> 
 *     <td> -pool </td> 
 *     <td> --block </td> 
 *     <td></td>
 *  </tr>
 *  <tr> 
 *     <td> -mapping or -mapfile </td> 
 *     <td></td>
 *     <td> --mapping </td> 
 *     <td>ABCDET instead of XYZT permutation</td>
 *  </tr>   
 *  <tr> 
 *     <td> -np </td> 
 *     <td></td>
 *     <td> --np </td> 
 *     <td></td>
 *  </tr>
 *  <tr> 
 *     <td> -mode </td> 
 *     <td> -mode </td> 
 *     <td> --ranks-per-node or -p </td> 
 *     <td> Syntax has changed. </td>
 *  </tr>
 *  <tr> 
 *     <td> </td> 
 *     <td> --location </td> 
 *     <td> --corner </td> 
 *     <td> </td>
 *  </tr>
 *  <tr> 
 *     <td> -shape </td> 
 *     <td></td>
 *     <td> --shape </td> 
 *     <td> Used for sub-block jobs instead of block creation</td>
 *  </tr>
 *  <tr> 
 *     <td> -strace </td> 
 *     <td> -strace </td> 
 *     <td> --strace</td> 
 *     <td> --strace all is no longer supported </td>
 *  </tr>
 *  <tr> 
 *     <td> -start_gdbserver </td> 
 *     <td> -start_gdbserver </td> 
 *     <td> --start-tool </td> 
 *     <td> --tool-args to pass arguments </td>
 *  </tr>
 *  <tr> 
 *     <td> </td> 
 *     <td> -raise </td> 
 *     <td> --raise</td> 
 *     <td> </td>
 *  </tr>
 *  <tr> 
 *     <td> -label </td> 
 *     <td> </td>
 *     <td> --label</td> 
 *     <td> </td>
 *  </tr>  
 *  <tr>
 *     <td> -timeout </td> 
 *     <td> -timeout </td> 
 *     <td> --timeout</td> 
 *     <td> </td>
 *  </tr>  
 *  <tr> 
 *     <td> -psets_per_bp </td> 
 *     <td> </td> 
 *     <td> </td> 
 *     <td> No longer supported </td>
 *  </tr>
 *  <tr> 
 *     <td> -noallocate </td> 
 *     <td> </td> 
 *     <td> </td> 
 *     <td> No longer supported </td>
 *  </tr>
 *  <tr> 
 *     <td> -free </td> 
 *     <td> </td> 
 *     <td> </td> 
 *     <td> No longer supported </td>
 *  </tr>   
 *  <tr> 
 *     <td> -nofree </td> 
 *     <td> </td> 
 *     <td> </td> 
 *     <td> No longer supported </td>
 *  </tr>   
 *  <tr> 
 *     <td> -connect </td> 
 *     <td> </td> 
 *     <td> </td> 
 *     <td> No longer supported </td>
 *  </tr>
 *  <tr> 
 *     <td> -host</td> 
 *     <td></td>
 *     <td> --socket </td> 
 *     <td> UNIX socket instead of TCP </td> 
 *  </tr>
 *  <tr> 
 *     <td> -port</td> 
 *     <td> -port</td> 
 *     <td> --socket </td> 
 *     <td> UNIX socket instead of TCP </td>
 *  </tr>
 *  <tr> 
 *     <td> </td> 
 *     <td> </td> 
 *     <td> --tool-args </td> 
 *     <td> Arguments to a tool started with --start-tool. </td>
 *  </tr>
 *  <tr> 
 *     <td> -verbose </td> 
 *     <td></td>
 *     <td> --verbose </td> 
 *     <td> Syntax differs noticeably. See \link bgq::utility::LoggingProgramOptions logging\endlink. </td>
 *  </tr>
 *  <tr> 
 *     <td> -trace </td> 
 *     <td> -trace </td> 
 *     <td> --verbose </td> 
 *     <td> Syntax differs noticeably. See \link bgq::utility::LoggingProgramOptions logging\endlink. </td>
 *  </tr>
 *  <tr> 
 *     <td> -config </td> 
 *     <td> -config </td> 
 *     <td> --properties </td> 
 *     <td></td>
 *  </tr>
 *  </table> 
 * 
 * \subsection database Database usage
 *
 * All job information is stored in the database.  The job status values and their transitions
 * are shown below.
 *
 * \image html job_status.png "Possible Job Status Values"
 *
 * Before a job is inserted into the database, several validations are performed
 * <ul>
 *   <li> The block specified by --block exists and is (I)nitialized </li>
 *   <li> No overlaping compute nodes are in use by other jobs on the block </li>
 *   <li> The user has authority to run a job on the block </li>
 * </ul>
 *
 * If all these checks pass, the job is inserted into the database with a status of Setu(p).
 *
 * \subsection Setup
 *
 * In this step, the collective and global interrupt class route settings are calculated for
 * each node in the job. This takes the --np and --ranks-per-node settings into account to
 * properly create a circumscribing logical rectangle for all the nodes in use. A logical
 * root of the rectangle is selected to act as the job leader. This node is responsible
 * for coordinating job loading and job termination for each rank in the job. Each I/O node
 * participating in the job is sent a unique setup message describing these class routes
 * for each compute node it manages. This mapping of compute to I/O nodes is calculated when
 * the compute block is booted. The personality of each compute node has its bridging compute 
 * node and the associated I/O node servicing it.
 *
 * \subsection Loading
 *
 * In this step, the job description (executable, arguments, environment) is communicated to
 * the job leader compute node through its attached I/O node. Permission to read and execute
 * the executable is validated and the job leader compute node loads the executable from the 
 * file system through its I/O node. The job leader compute node then broadcasts the executable 
 * to the other compute nodes in the job.
 *
 * \subsection Debug
 *
 * In this step, the job has requested a tool be started prior to starting the job. This allows
 * the user time to perform any actions needed with the tool before the job begins execution. The
 * traditional example being a debugger, breakpoints can be setup after the job has been loaded
 * but before it begins execution.
 *
 * \subsection Starting
 *
 * After all of the compute nodes have loaded the executable, each stdio daemon servicing
 * the job is told to start the job so the necessary data structures can be setup for
 * handling standard output and error. Once each stdio daemon has reported a succesful ack,
 * each jobctl daemon is told to inform the compute nodes it manages to release control
 * to the application so it can start running.
 *
 * \subsection Running
 *
 * Once all compute nodes have reported starting the application, the job has (R)unning
 * status. It retains this status until it terminates normally or abnormally.
 *
 * \subsection Cleanup
 *
 * For normal termination, a job has a Clea(n)up status while the resources on the I/O
 * nodes and compute nodes are cleaned up in preparation for future jobs.
 *
 * \subsection Error
 *
 * A job has this status in the history table when it has terminated without entering the
 * Running stage, or if delivering a SIGKILL timed out.
 *
 * \subsection Terminated
 *
 * A job has this status in the history table when it was completed. The exit status and
 * error text columns will have more information about how the job completed.
 *
 * \section information More Information
 *
 * - see \ref runjob_server for more information about the %runjob %server.
 * - see \ref runjob_mux for more information about the %runjob multiplexer.
 * - see \ref runjob for more information about the %runjob client.
 */

/*!
 * \namespace runjob
 * \brief %common objects used by all %runjob components
 *
 * \namespace runjob::error_code
 * \brief internal error codes
 *
 * \namespace runjob::commands
 * \brief %common objects used to communicate with the %runjob %server and %mux
 * \ingroup command_protocol
 *
 * \namespace runjob::message
 * \brief message types used between all %runjob components.
 *
 * \namespace runjob::client
 * \brief objects used by the %runjob %client
 *
 * \namespace runjob::client::options
 * \brief Program option parsing objects used by the %runjob %client
 *
 * \namespace runjob::mux
 * \brief objects used by the %runjob multiplexer
 *
 * \namespace runjob::mux::client
 * \brief objects used by the %runjob %mux to communicate with %runjob %clients
 *
 * \namespace runjob::mux::commands
 * \brief objects used by the %runjob %mux and commands to communicate with the %runjob %mux
 *
 * \namespace runjob::mux::performance
 * \brief performance counter objects used by the %runjob %mux
 *
 * \namespace runjob::mux::server
 * \brief objects used by the %runjob %mux to communicate with the %runjob %server
 *
 * \namespace runjob::server
 * \brief objects used by the %runjob %server
 *
 * \namespace runjob::server::block
 * \brief block objects used by the %runjob %server
 *
 * \namespace runjob::server::cios
 * \brief Common I/O services (cios) objects used by the %runjob %server
 *
 * \namespace runjob::server::commands
 * \brief objects used by the %runjob %server and commands to communicate with the %runjob %server
 *
 * \namespace runjob::server::database
 * \brief Database objects used by the %runjob %server
 *
 * \namespace runjob::server::handlers
 * \brief Command handler objects used by the %runjob %server
 * 
 * \namespace runjob::server::handlers::locate_rank
 * \brief locate_rank command objects used by the %runjob %server
 *
 * \namespace runjob::server::job
 * \brief job objects used by the %runjob %server
 *  
 * \namespace runjob::server::job::class_route
 * \brief job class route objects used by the %runjob %server
 * 
 * \namespace runjob::server::job::tool
 * \brief job tool launching objects used by the %runjob %server
 * 
 * \namespace runjob::server::mux
 * \brief objects used by the %runjob %server to communicate with a %runjob %mux
 *
 * \namespace runjob::server::performance
 * \brief performance counter objects used by the %runjob %server
 *
 * \namespace runjob::server::realtime
 * \brief real-time API objects used by the %runjob %server
 *
 * \namespace runjob::server::sim
 * \brief job simulation objects used by the %runjob %server
 */

/*!
 * \dir include/runjob top level directory containing header files that can be used by other components
 * outside of hlcs.
 * \dir include/runjob/commands
 * \dir src/runjob top level directory containing private source code and headers.
 * \dir src/runjob/common Objects common to all components.
 * \dir src/runjob/common/commands Objects common to %server and %mux commands.
 * \dir src/runjob/common/message Objects common to all components for communication between them.
 * \dir src/runjob/client source code to build the runjob binary
 * \dir src/runjob/mux source code to build the runjob_mux binary
 * \dir src/runjob/mux/client source code to build the runjob_mux client objects
 * \dir src/runjob/mux/commands source code to build the runjob_mux command binaries
 * \dir src/runjob/mux/handlers source code for the runjob_mux command handler objects
 * \dir src/runjob/mux/performance source code for the runjob_mux command performance counter objects
 * \dir src/runjob/server source code to build the runjob_server binary
 * \dir src/runjob/server/commands source code to build the runjob_server command binaries
 * \dir src/runjob/server/handlers source code for the runjob_server command handler objects
 * \dir src/runjob/server/handlers/locate_rank source code for the runjob_server locate_rank command handler
 * \dir src/runjob/server/performance source code for the runjob_server command performance counter objects
 * \dir src/runjob/server/block source code for the runjob_server command block objects
 * \dir src/runjob/server/job source code for the runjob_server command job objects
 * \dir src/runjob/server/sim source code for the runjob_server command simulation objects
 */

/*!
 * \defgroup command_protocol runjob_server and runjob_mux protocol.
 * These classes are used by clients to make requests to the runjob_server and runjob_mux. To use them,
 * you must link to the <a href=http://www.boost.org/doc/libs/1_42_0/libs/serialization/doc/index.html>Boost.Serialization</a> library
 * like so
\verbatim
LDFLAGS += -lboost_serialization
\endverbatim
 * \section usage Sample Usage.
 *
 * To send a request to either the %runjob %server or %mux, first send a
 * \link runjob::commands::Header header\endlink containing the message length, then send the message itself.
 * Any message can be serialized into a string using the \link runjob::commands::Message::serialize serialize\endlink method.
 *
 * \include test/server/commands/sample.cc
 *
 * Responses can be deserialized into an object using a \htmlonly <a
 * href="http://www.boost.org/doc/libs/1_42_0/libs/serialization/doc/index.html">boost::archive::text_iarchive</a>\endhtmlonly.
 * The object type can be determined from the \link runjob::commands::Header::_type type\endlink and
 * \link runjob::commands::Header::_tag tag\endlink members of a runjob::commands::Header object.
 *
 * Both the %runjob %server and %runjob %mux are configured to listen on endpoints defined in the bg.properties
 * file in their respective sections. Each connection must be authenticated using the \link
 * bgq::utility::portConfig::UserType administrative\endlink SSL certificate
 * certificate. See bgq::utility::Handshaker and bgq::utility::Connector for more information.
 */

/*!
 * \defgroup argument_parsing Argument Parsing.
 *
 * These classes are used for parsing arguments in conjunction with the \htmlonly <a
 * href="http://www.boost.org/doc/libs/1_42_0/doc/html/program_options.html">Boost.ProgramOptions</a>\endhtmlonly library.
 */
