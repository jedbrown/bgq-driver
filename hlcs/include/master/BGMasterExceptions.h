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

#ifndef _BGMASTER_EXCEPTION_H
#define _BGMASTER_EXCEPTION_H

#include <stdexcept>

namespace BGMasterExceptions {

    //! \brief exception severity
    //!  OK:    No error
    //!  INFO:  The program will continue to run correctly. This is typically
    //!         the result of a bad parameter or a request for a resource
    //!         no longer being managed.
    //!  WARN:  The program will continue to run, but something in the internal
    //!         state is not what you requested.  User or application may be
    //!         able to repair the program.  A resource may be unresponsive.
    //!  FATAL: Program should not continue execution.

    enum Severity { OK = 0, INFO, WARN, FATAL };

    //! Base BGMaster base exception class inheriting from runtime_error
    class BGMasterError : public std::runtime_error {
    public:
        Severity errcode;
        BGMasterError(Severity err=INFO, const std::string& what="") : std::runtime_error(what), errcode(err) {}
    };


    //! \brief Fatal exception
    //! Thrown when the client API is misused
    //!
    class APIUserError : public BGMasterError {
    public:
        APIUserError(Severity err=WARN, const std::string& what="") : BGMasterError(err, what) {}
    };

    //! \brief API has a non-fatal return code
    //! Thrown when the client API is misused
    //!
    class APICommandError : public BGMasterError {
    public:
        APICommandError(Severity err=INFO, const std::string& what="") : BGMasterError(err, what) {}
    };

    //! \brief Error exception for communications
    //!
    //!
    class CommunicationError : public BGMasterError {
    public:
        CommunicationError(Severity err=INFO, const std::string& what="") : BGMasterError(err, what) {}
    };

    //! \brief exception for file I/O errors
    //!
    //!
    class FileError : public BGMasterError {
    public:
        FileError(Severity err=WARN, const std::string& what="") : BGMasterError(err, what) {}
    };

    //! \brief Fatal exception
    //!
    //!
    class FatalError : public BGMasterError {
    public:
        FatalError(Severity err=FATAL, const std::string& what="") : BGMasterError(err, what) {
        }
    };

    //! \brief Protocol exception
    //!
    //!
    class ProtocolError : public BGMasterError {
    public:
        ProtocolError(Severity err=WARN, const std::string& what="") : BGMasterError(err, what) {}
    };

    //! \brief config file error
    //! Thrown when the config file is bad
    //!
    class ConfigError : public BGMasterError {
    public:
        ConfigError(Severity err=WARN, const std::string& what="") : BGMasterError(err, what) {}
    };

    //! \brief Internal error
    //! Internal error message reporting.  Should not return to clients.
    //!
    class InternalError : public BGMasterError {
    public:
        InternalError(Severity err=WARN, const std::string& what="") : BGMasterError(err, what) {}
    };
}

#endif
