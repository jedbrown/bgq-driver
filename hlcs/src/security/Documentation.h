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
 * \page API
 *
 * \section Introduction
 *
 * This API provides classes and free functions exposing security policy and enforcement functions for
 * high level control system daemons and libraries. The main interface is the hlcs::security::Enforcer class
 
\verbatim
#include <hlcs/include/security/Enforcer.h>
\endverbatim
 
 * For linking, you probably want to do something like:

\verbatim
library_dir=$(BGQ_INSTALL_DIR)/hlcs/lib
\endverbatim

 * Then use these LDFLAGS:

\verbatim
LDFLAGS += -L$(library_dir) -lsecurity -Wl,-rpath,$(library_dir)
\endverbatim

 * Unit tests are located in the hlcs/src/security/test directory.
 *
 *
 * \section Overview
 *
 * This API is intended as the central interface to enforce object ownership, actions, and permissions for
 * control system components. It provides mechanisms to validate actions upon objects, as well as
 * grainting and revoking global and per-object permissions.
 *
 * There are three types of \link hlcs::security::Object::Type objects\endlink used in this API
 *
 * - Job
 * - Block
 * - Hardware
 *
 * There are five types of \link hlcs::security::Action::Type actions\endlink upon these objects
 * 
 * - Create
 * - Read
 * - Update
 * - Delete
 * - Execute
 *
 * There's also a special All action, which cannot be granted. It means a user has
 * the authority to perform any of the above actions.
 *  
 * \section Permissions
 *
 * Permissions for these objects can come from one of three locations
 *
 * - bg.properties file
 * - database
 *
 * \subsection properties
 *
 * Permissions in the properties file are located in the sections [security.jobs]
 * [security.hardware] and [security.blocks]. Both user and group IDs are supported.
 * An example
 *
 * \code
 * [security.jobs]
 * all=bgadmin,root
 *
 * [security.blocks]
 * all=bgadmin,root
 *
 * \endcode
 * 
 * would allow bgadmin and root users, or any users in those groups, to perform any
 * action on any job or any block
 *
 * \subsection database
 *
 * Permissions for each object are stored in the control system database. 
 * The hlcs::security::grant and hlcs::security::revoke free functions can be
 * used to grant and revoke authority for certain objects. The hlcs::security::list
 * free function can be used to list existing authority for an object.
 *
 */

 /*!
  * \namespace hlcs::security
  * \brief namespace for all security methods and types
  */
