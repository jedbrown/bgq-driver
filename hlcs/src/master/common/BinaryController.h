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

#ifndef MASTER_BINARY_CONTROLLER_H_
#define MASTER_BINARY_CONTROLLER_H_


#include "Ids.h"
#include "types.h"

#include <utility/include/cxxsockets/Host.h>

#include <boost/assign/list_of.hpp>
#include <boost/date_time.hpp>
#include <boost/thread.hpp>
#include <boost/utility.hpp>

#include <iostream>
#include <string>

#include <pthread.h>


//! \brief Controls a managed executable, base class
class BinaryController : private boost::noncopyable
{
public:
    enum Status { UNINITIALIZED, RUNNING, COMPLETED };

    static std::string status_to_string(Status s) {
        static std::map<Status,std::string> values = boost::assign::map_list_of
            (UNINITIALIZED, "UNINITIALIZED")
            (RUNNING, "RUNNING")
            (COMPLETED, "COMPLETED")
            ;
        const std::map<Status,std::string>::const_iterator result = values.find( s );
        if ( result == values.end() ) return std::string();
        return result->second;
    }

    static Status string_to_status(const std::string& statstr) {
        Status retstat = UNINITIALIZED;
        if(statstr == "RUNNING") {
            return RUNNING;
        } else if(statstr == "COMPLETED") {
            return COMPLETED;
        }
        return retstat;
    }

    //! \brief Condition variable to notify threads of changes in status.
    boost::condition_variable _status_notifier;
    //! \brief Lock for the status condition variable.
    boost::mutex _status_lock;

    //! \brief Default constructor
    BinaryController();

    /*!
     * \brief construct with a binary id and a status
     */
    BinaryController(
            const BinaryId& id, 
            const std::string& bin_path, 
            const std::string& alias,
            const std::string& user, 
            int exit_status = 0, 
            Status stat = UNINITIALIZED,
            const std::string& start_time = std::string()
            );

    /*!
     * \brief construct with a path and arguments
     */
    BinaryController(
            const std::string& path, 
            const std::string& arguments, 
            const std::string& logfile,
            const std::string& alias, 
            const CxxSockets::Host& host, 
            const std::string& user
            );

    /*!
     * \brief construct with an id, full 'bin_path', user, exit status and status
     */
    BinaryController(
            const std::string& id, 
            const std::string& bin_path, 
            const std::string& alias,
            const std::string& user, 
            int exit_status, 
            int stat, 
            const std::string& start_time
            );

    //! \brief start this binary
    //! \returns ID for started binary
    BinaryId startBinary(
            const std::string& user_list,   //!< [in]
            const std::string& properties   //!< [in]
            );

    //! \brief stop this binary
    //! \returns true if successful, false if not
    int stop(int signal);

    //! \brief utility functions
    const std::string& get_binary_bin_path() const { return _binary_bin_path; }
    const std::string& get_alias_name() const { return _alias_name; }
    const std::string& get_user() const { return _user; }
    const std::string& get_error_text() const { return _exec_error; }
    int get_exit_status() const { return _exit_status; }
    void set_exit_status(int exit_status) { _exit_status = exit_status; }
    Status get_status() const { return _status; }
    void set_status(Status s) { 
        boost::lock_guard<boost::mutex> lg(_status_lock);
        _status = s; 
        _status_notifier.notify_all();
    }
    const BinaryId& get_binid() const { return _binid; }
    const boost::posix_time::ptime& get_start_time() const { return _start_time; }
    bool valid() const {
        bool ret;
        (_status == UNINITIALIZED) ? ret = false : ret = true;
        return ret;
    }

    // \brief setter/getter for stop flag.
    bool stopping(bool stop = false) {
        if(stop == true) _stop_requested = true; return _stop_requested;
    }

protected:
    //! \brief monitor tid
    pthread_t _monitor_tid;

    //! \brief running status
    Status _status;

    //! \brief exit status
    int _exit_status;

    //! \brief The bin_path is the path and arguments for the binary
    std::string _binary_bin_path;

    //! \brief The alias name associated with the binary
    std::string _alias_name;

    //! \brief full path to the logfile
    std::string _logfile;

    //! \brief Host we're on.
    CxxSockets::Host _host;

    //! \brief Unique identifier for the binary.  Host/pid.
    BinaryId _binid;

    //! \brief flag to indicate that we have specifically
    //  asked this binary to die.
    bool _stop_requested;

    boost::posix_time::ptime _start_time;

    //! \brief user id under which we started the binary
    std::string _user;

    //! \brief error message returned on exec failures
    std::string _exec_error;
};

inline std::ostream& operator <<(std::ostream& os, const BinaryControllerPtr& b) { 
    os << b->get_binid().str() << "|" << b->get_alias_name() << "|"
       << b->BinaryController::status_to_string(b->get_status()) << "|"
       << b->get_user();
    return os;
}

#endif
