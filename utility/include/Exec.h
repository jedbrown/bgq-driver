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
 * \file utility/include/Exec.h
 */

#include <string>

#ifndef BGQ_UTILITY_EXEC_H
#define BGQ_UTILITY_EXEC_H

namespace Exec {

//! \brief Small Interface to fork() and exec() a child process.
//! This provides a way to create a child process and return a file descriptor
//! and process id so that a process can read the child's stderr and stdout
//! and wait for it to complete.
//! \param pipefd A file descriptor to be filled out with the child's output pipe.
//!        This will not be set if logfilename is passed. 
//! \param path_and_args The full path of the executable and its arguments
//! \param managed Sets the non-portable Linux PR_SET_PDEATHSIG so ensure that 
//!        death of the parent takes out the child.
//! \param propfile of bg.properties file to pass as an env var.
//! \param errorstring
//! \param logfilename
//! \param userid
//! \returns the child's process ID
pid_t fexec(
        int& pipefd,
        const std::string& path_and_args,
        std::string& errorstring,
        const bool managed,
        const std::string& logfilename = std::string(), 
        const std::string& propfile = std::string(),
        const std::string& userid = std::string()
        );

} // Exec

#endif
