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
 * \file utility/include/ExitStatus.h
 */

#ifndef BGQ_UTILITY_EXIT_STATUS_HPP_
#define BGQ_UTILITY_EXIT_STATUS_HPP_


#include <sys/types.h>
#include <sys/wait.h>

#include <iosfwd>


namespace bgq {
namespace utility {


/*! \brief A wrapper around the process exit status macros for determining how a program exited (signaled or normal), supports <<, bool comparison.
 *
 *  When doing bool comparison, returns true if the exit status is non-zero.
 *
 *  Output is like &quot;status <i>number</i>&quot;, or &quot;signal <i>number</i>&quot;.
 */
class ExitStatus
{
public:

    typedef const int ExitStatus::*BoolType;


    /*! \brief Normal exit is exited() & exit status is 0. */
    static const ExitStatus Normal;


    explicit ExitStatus( int exit_status = -1 ) : _exit_status(exit_status) {}

    /*! \brief The raw exit status. */
    int get() const { return _exit_status; }

    /*! \brief Returns true iff the program exited with a normal exit status. */
    bool exited() const  { return WIFEXITED( _exit_status ); }

    /*! \brief The exit status, only valid if exited() is true. */
    int getExitStatus() const  { return WEXITSTATUS( _exit_status ); }

    /*! \brief Returns true iff the program exited due to a signal. */
    bool signaled() const  { return WIFSIGNALED( _exit_status ); }

    /*! \brief The exit status, only valid if signaled() is true. */
    int getSignal() const  { return WTERMSIG( _exit_status ); }

    /*! \brief Boolean comparison.
     *  \return true if abnormal exit, false if normal exit.
     */
    operator BoolType() const  { return (_exit_status != 0 ? &ExitStatus::_exit_status : 0 ); }


private:

    int _exit_status;
};


/*! \brief Outputs the exit status, like &quot;status <i>number</i>&quot;, or &quot;signal <i>number</i>&quot;. */
std::ostream& operator<<( std::ostream& os, const ExitStatus& exit_status );


} } // namespace bgq::utility

#endif
