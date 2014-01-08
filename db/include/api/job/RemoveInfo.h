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

#ifndef BGQDB_JOB_REMOVE_INFO_H
#define BGQDB_JOB_REMOVE_INFO_H

#include <string>

namespace BGQDB {
namespace job {

/*! \brief Arguments to BGQDB::job::remove() */
class RemoveInfo
{
public:

    /*! \brief Sets terminated and exit status. */
    RemoveInfo( int exit_status = 0 );

    /*! \brief If error, sets error message. */
    RemoveInfo( const std::string& error_message );

    /*! \brief sets terminated, exit status, and error message. */
    RemoveInfo( int exit_status, const std::string& error_message );

    /*! \brief True if have exit status. */
    bool isTerminated() const  { return _terminated; }

    int getExitStatus() const  { return _exit_status; }

    const std::string& getMessage() const  { return _message; }


private:

    bool _terminated;

    int _exit_status;

    std::string _message;

};


} } // namespace BGQDB::job

#endif
