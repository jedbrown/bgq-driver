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
 * \author HLCS Team
 *
 * \section toc Table of Contents
 *
 * - \ref security_overview
 * - \ref security_requirements
 * - \ref security_objects
 * - \ref security_trust
 * - \ref security_history
 * - \ref security_configuration
 * - \ref security_opensource
 * - \ref security_info
 *
 * \section security_overview Overview
 *
 * This documentation was generated for driver DRIVER_NAME with revision VERSION_NUMBER.
 *
 * The BlueGene/Q High Level Control System (HLCS) features a new security model that was
 * not present in the BlueGene/L or BlueGene/P control systems. For a historical
 * perspective, once a user obtained access to the Service Node in a BG/L or BG/P environment
 * they could effectively obtain superuser privileges for the Blue Gene environment with
 * the setusername command within mmcs_db_console. During the initial design phase with
 * members of the BG/Q System Management Working Group, such functionality was described
 * as undesireable for understandable reasons.  This document describes the security model
 * from a high level, as well as links to some implementation details used in various
 * components.
 *
 * \section security_requirements Requirements
 *
 * The requirements for the security model are listed below
 *
 * <ul>
 * <li> no privilege escalation
 * <li> commands to grant and revoke privileges
 * <li> can be configured to mimic lack of security like BG/L and BG/P
 * </ul>
 *
 * \section security_objects Objects and Owners
 *
 * To implement the requirements, we chose an object based approach. Much of the control
 * system already has a heritage of using this concept for blocks and jobs, they've
 * always had owners. Both the BG/L and BG/P control system also had the ability to
 * grant users other than the block owner to run jobs on the block.
 *
 * The main difference in the BG/Q HLCS is the removal of the setusername command from mmcs_db_console
 * (now bg_console in BG/Q).  Once a user initiates a console session, there is no ability to 
 * change the username of that session. Similarly, each object has a permanent owner that never 
 * changes over its lifetime. That owner can grant and revoke privileges to other users or groups 
 * allowing them to perform actions on the object in question. Additionally, superusers have 
 * the ability to grant and revoke privileges to objects they do not own.
 *
 * The \ref API "HLCS Security API" provides an in depth overview of how this is implemented
 * in various control system components. Basically, object permissions are stored in two places
 *
 * <ol>
 * <li> \link bgq::utility::Properties bg.properties \endlink file contains global permissions
 * <li> database contains per object permissions that can be granted and revoked
 * </ol>
 *
 * \section security_trust Trust
 *
 * Establishing trust between a client and server (ex: bg_console and mmcs_server) requires
 * using encrypted communication to transmit the user's credentials. Three private
 * key and certificate pairs will be created during system installation:
 *
 * <ol>
 * <li> root certificate authority </li>
 * <li> administrator </li>
 * <li> commands </li>
 * </ol>
 *
 * The administrator certificate is used by various control system daemons to establish
 * trust between each other. It is also used by an administrator to perform various administrative
 * actions such as diagnostics or service actions. The command certificate is used by commands
 * requiring communication with control system daemons. Using the command certificate requires
 * transmitting encrypted user ID credentials. The command certificate is only readable
 * by a group, which requires any binaries requiring the command certificate to be setuid.
 *
 * The \link bgq::utility::ClientPortConfiguration ClientPortConfiguration\endlink and
 * \link bgq::utility::ServerPortConfiguration ServerPortConfiguration\endlink classes describe 
 * functionality in greater detail.
 *
 * \image html trust.png
 *
 * \section security_history Object History
 *
 * Blocks, jobs, and hardware  are preserved in a history table in the database after their 
 * destruction, death, or replacement.  It's not practical to also retain a snapshot permission 
 * information at the time of the object's demise. With that in mind, only an object's owner 
 * or a user with special permissions can view objects in the history tables.
 *
 * \section security_configuration Configuration
 *
 * Various aspects of the security model must be configured for it to be effective. The two primary places
 * for this configuration information is in the properties file, and permissions of the private key
 * and certificate files.
 *
 * \subsection security_properties Properties file
 *
 * The \ref API provides detailed information for setting up security configuration settings in the
 * \link bgq::utility::Properties bg.properties \endlink file.
 *
 * \subsection security_permissions Permissions
 *
 * To establish effective trust between components, the private keys must be kept secret to prevent 
 * unauthorized users from escalating their privileges. The administrative key is only readable
 * by the bgqadmin group. The command key is readable by the bgqcommand group. Processes that are
 * executed by non-administrative users are setuid to bgqcommand so they can read the command key
 * when required.
 *
 * \section security_opensource Open Source Considerations
 *
 * Downloading open source BG/Q packages and rebuilding them to bypass the security mechanisms described
 * here is not concern due to the permissions imposed on the private key and certificate pairs. 
 * Regardless of where or how a binary is built, it will still have to authenticate with control system
 * daemons as described in the previous sections.
 *
 * \section security_info More Information
 *
 * <ul>
 * <li>\ref API "Security API"
 * <li>\link bgq::utility::ClientPortConfiguration ClientPortConfiguration\endlink and \link bgq::utility::ServerPortConfiguration ServerPortConfiguration\endlink classes for encrypted communication
 * </ul>
 */
